#!/usr/bin/env python3
"""
BadCppLib Cross-Platform Build Script
=====================================

Automatically detects available compilers and builds the library
on Windows, Linux, and macOS.
"""

import os
import sys
import platform
import subprocess
import argparse
from pathlib import Path


class Colors:
    """ANSI colors for pretty output"""

    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"

    @staticmethod
    def disable():
        """Disable colors on Windows if not supported"""
        Colors.HEADER = ""
        Colors.OKBLUE = ""
        Colors.OKCYAN = ""
        Colors.OKGREEN = ""
        Colors.WARNING = ""
        Colors.FAIL = ""
        Colors.ENDC = ""
        Colors.BOLD = ""
        Colors.UNDERLINE = ""


class BuildSystem:
    """BadCppLib build system"""

    def __init__(self):
        self.system = platform.system()

        # Determine project root directory (parent directory of scripts)
        script_dir = Path(__file__).parent
        self.project_root = script_dir.parent

        # Paths to directories
        self.src_dir = self.project_root / "src"
        self.include_dir = self.project_root / "include"
        self.examples_dir = self.project_root / "examples"
        self.tests_dir = self.project_root / "tests"
        self.build_dir = self.project_root / "build"

        # Create build directory if it doesn't exist
        self.build_dir.mkdir(exist_ok=True)

        # Change to project root directory for build
        os.chdir(self.project_root)

        self.sources = ["src/badcpplib.cpp", "src/pch.cpp"]
        self.headers = [
            "include/badcpplib.hpp",
            "include/framework.hpp",
            "include/pch.hpp",
        ]
        self.example_source = "examples/example.cpp"

        # Configure colors for Windows
        if self.system == "Windows" and not os.environ.get("FORCE_COLOR"):
            try:
                # Attempt to enable ANSI colors on Windows 10+
                import colorama

                colorama.init()
            except ImportError:
                Colors.disable()

        # Define executable extensions
        self.exe_ext = ".exe" if self.system == "Windows" else ""
        self.lib_ext = ".lib" if self.system == "Windows" else ".a"

    def print_header(self, text):
        """Print header"""
        print(f"\n{Colors.BOLD}{Colors.HEADER}{'='*50}")
        print(f"{text}")
        print(f"{'='*50}{Colors.ENDC}")

    def print_success(self, text):
        """Print success"""
        print(f"{Colors.OKGREEN}✓ {text}{Colors.ENDC}")

    def print_error(self, text):
        """Print error"""
        print(f"{Colors.FAIL}✗ {text}{Colors.ENDC}")

    def print_warning(self, text):
        """Print warning"""
        print(f"{Colors.WARNING}⚠ {text}{Colors.ENDC}")

    def print_info(self, text):
        """Print information"""
        print(f"{Colors.OKCYAN}ℹ {text}{Colors.ENDC}")

    def check_compiler(self, compiler):
        """Check compiler availability"""
        try:
            result = subprocess.run(
                [compiler, "--version"], capture_output=True, text=True, timeout=10
            )
            return result.returncode == 0
        except (subprocess.TimeoutExpired, FileNotFoundError, OSError):
            return False

    def find_compilers(self):
        """Find available compilers"""
        compilers = []

        # List of compilers to check
        candidates = ["g++", "clang++", "cl"]

        for compiler in candidates:
            if self.check_compiler(compiler):
                compilers.append(compiler)
                self.print_success(f"Found compiler: {compiler}")

        return compilers

    def get_compile_flags(self, compiler, build_type="release"):
        """Get compilation flags for each compiler"""
        base_flags = []

        if compiler == "cl":  # MSVC
            base_flags = ["/std:c++17", "/EHsc", f"/I{self.include_dir}"]
            if build_type == "debug":
                base_flags.extend(["/Od", "/Zi", "/MDd"])
            else:
                base_flags.extend(["/O2", "/MD"])
        else:  # GCC or Clang
            base_flags = ["-std=c++17", "-Wall", "-Wextra", f"-I{self.include_dir}"]
            if build_type == "debug":
                base_flags.extend(["-g", "-O0", "-DDEBUG"])
            else:
                base_flags.extend(["-O2", "-DNDEBUG"])

        return base_flags

    def compile_library(self, compiler, build_type="release"):
        """Compile static library"""
        self.print_info(f"Compiling library with {compiler} ({build_type})")

        # Compile object files
        obj_files = []
        flags = self.get_compile_flags(compiler, build_type)

        for source in self.sources:
            if not Path(source).exists():
                self.print_error(f"Source file not found: {source}")
                return False

            obj_file = (
                self.build_dir
                / Path(source).with_suffix(".obj" if compiler == "cl" else ".o").name
            )
            obj_files.append(str(obj_file))

            if compiler == "cl":
                cmd = [compiler] + flags + ["/c", source, f"/Fo{obj_file}"]
            else:
                cmd = [compiler] + flags + ["-c", source, "-o", str(obj_file)]

            try:
                result = subprocess.run(cmd, capture_output=True, text=True)
                if result.returncode != 0:
                    self.print_error(f"Compilation error {source}:")
                    print(result.stderr)
                    return False
            except Exception as e:
                self.print_error(f"Error running compiler: {e}")
                return False

        # Create static library
        lib_name = self.build_dir / f"libbadcpp{self.lib_ext}"

        if compiler == "cl":
            # Use lib.exe for MSVC
            cmd = ["lib", f"/OUT:{lib_name}"] + obj_files
        else:
            # Use ar for GCC/Clang
            cmd = ["ar", "rcs", str(lib_name)] + obj_files

        try:
            result = subprocess.run(cmd, capture_output=True, text=True)
            if result.returncode != 0:
                self.print_error("Error creating library:")
                print(result.stderr)
                return False
        except Exception as e:
            self.print_error(f"Error creating library: {e}")
            return False

        self.print_success(f"Library created: {lib_name}")
        return True

    def compile_example(self, compiler, build_type="release", use_library=True):
        """Compile example"""
        self.print_info(f"Compiling example with {compiler}")

        exe_name = self.build_dir / f"example{self.exe_ext}"
        flags = self.get_compile_flags(compiler, build_type)

        if use_library:
            # Link with library
            lib_name = self.build_dir / f"libbadcpp{self.lib_ext}"
            if not lib_name.exists():
                self.print_error(f"Library not found: {lib_name}")
                return False

            if compiler == "cl":
                cmd = (
                    [compiler]
                    + flags
                    + [self.example_source, str(lib_name), f"/Fe{exe_name}"]
                )
            else:
                cmd = (
                    [compiler]
                    + flags
                    + [
                        self.example_source,
                        f"-L{self.build_dir}",
                        "-lbadcpp",
                        "-o",
                        str(exe_name),
                    ]
                )
        else:
            # Direct compilation
            sources = [self.example_source] + self.sources
            if compiler == "cl":
                cmd = [compiler] + flags + sources + [f"/Fe{exe_name}"]
            else:
                cmd = [compiler] + flags + sources + ["-o", str(exe_name)]

        try:
            result = subprocess.run(cmd, capture_output=True, text=True)
            if result.returncode != 0:
                self.print_error("Error compiling example:")
                print(result.stderr)
                return False
        except Exception as e:
            self.print_error(f"Error compiling example: {e}")
            return False

        self.print_success(f"Example compiled: {exe_name}")
        return True

    def compile_tests(self, compiler, build_type="release"):
        """Compile tests"""
        if not self.tests_dir.exists():
            self.print_warning("Tests directory not found")
            return False

        test_files = list(self.tests_dir.glob("*.cpp"))
        if not test_files:
            self.print_warning("Test files not found")
            return False

        self.print_info(f"Compiling tests with {compiler}")

        lib_name = self.build_dir / f"libbadcpp{self.lib_ext}"
        if not lib_name.exists():
            self.print_error(f"Library not found: {lib_name}")
            return False

        flags = self.get_compile_flags(compiler, build_type)

        for test_file in test_files:
            test_name = test_file.stem
            exe_name = self.build_dir / f"{test_name}{self.exe_ext}"

            if compiler == "cl":
                cmd = (
                    [compiler]
                    + flags
                    + [str(test_file), str(lib_name), f"/Fe{exe_name}"]
                )
            else:
                cmd = (
                    [compiler]
                    + flags
                    + [
                        str(test_file),
                        f"-L{self.build_dir}",
                        "-lbadcpp",
                        "-o",
                        str(exe_name),
                    ]
                )

            try:
                result = subprocess.run(cmd, capture_output=True, text=True)
                if result.returncode != 0:
                    self.print_error(f"Error compiling test {test_name}:")
                    print(result.stderr)
                    return False
            except Exception as e:
                self.print_error(f"Error compiling test {test_name}: {e}")
                return False

            self.print_success(f"Test compiled: {exe_name}")

        return True

    def run_tests(self):
        """Run tests"""
        test_files = list(self.build_dir.glob(f"*test{self.exe_ext}"))
        if not test_files:
            self.print_error("Compiled tests not found")
            return False

        self.print_info("Running tests...")

        all_passed = True
        for test_file in test_files:
            self.print_info(f"Running {test_file.name}...")
            try:
                result = subprocess.run(
                    [str(test_file)], capture_output=True, text=True
                )
                if result.returncode == 0:
                    self.print_success(f"Test {test_file.name} passed")
                else:
                    self.print_error(f"Test {test_file.name} failed")
                    print(result.stdout)
                    print(result.stderr)
                    all_passed = False
            except Exception as e:
                self.print_error(f"Error running test {test_file.name}: {e}")
                all_passed = False

        return all_passed

    def run_example(self):
        """Run example"""
        exe_name = self.build_dir / f"example{self.exe_ext}"
        if not exe_name.exists():
            self.print_error(f"Executable not found: {exe_name}")
            return False

        self.print_info("Running example...")
        try:
            subprocess.run([str(exe_name)])
            return True
        except Exception as e:
            self.print_error(f"Error running example: {e}")
            return False

    def clean(self):
        """Clean generated files"""
        self.print_info("Cleaning...")

        patterns = [
            "*.o",
            "*.obj",
            "*.lib",
            "*.a",
            f"example{self.exe_ext}",
            "test.txt",
            "*.pdb",
            "*.ilk",
        ]

        cleaned = 0
        # Clean build directory
        if self.build_dir.exists():
            for pattern in patterns:
                if "*" in pattern:
                    # Use glob for patterns
                    for file in self.build_dir.glob(pattern):
                        try:
                            file.unlink()
                            cleaned += 1
                        except OSError:
                            pass
                else:
                    file_path = self.build_dir / pattern
                    if file_path.exists():
                        try:
                            file_path.unlink()
                            cleaned += 1
                        except OSError:
                            pass

        self.print_success(f"Files removed: {cleaned}")

    def print_system_info(self):
        """Print system information"""
        self.print_header("System Information")
        print(f"Operating System: {self.system}")
        print(f"Architecture: {platform.machine()}")
        print(f"Python version: {sys.version}")
        print(f"Working directory: {self.project_root}")
        print(f"Source files: {self.src_dir}")
        print(f"Header files: {self.include_dir}")
        print(f"Examples: {self.examples_dir}")
        print(f"Tests: {self.tests_dir}")
        print(f"Build directory: {self.build_dir}")

    def build(
        self,
        compiler=None,
        build_type="release",
        use_library=True,
        run_after=False,
        build_tests=False,
    ):
        """Main build function"""
        self.print_header("BadCppLib Build System")
        self.print_system_info()

        # Find compilers
        available_compilers = self.find_compilers()

        if not available_compilers:
            self.print_error("No suitable compilers found!")
            self.print_info("Install one of: g++, clang++, or Visual Studio (cl)")
            return False

        # Choose compiler
        if compiler and compiler in available_compilers:
            selected_compiler = compiler
        else:
            selected_compiler = available_compilers[0]
            if compiler:
                self.print_warning(
                    f"Compiler {compiler} not found, using {selected_compiler}"
                )

        self.print_info(f"Using compiler: {selected_compiler}")

        # Compile library
        if use_library:
            if not self.compile_library(selected_compiler, build_type):
                return False

        # Compile example
        if not self.compile_example(selected_compiler, build_type, use_library):
            return False

        # Compile tests
        if build_tests:
            if not self.compile_tests(selected_compiler, build_type):
                return False

        self.print_success("Build completed successfully!")

        # Run example
        if run_after:
            self.run_example()

        # Run tests
        if build_tests:
            if not self.run_tests():
                self.print_warning("Some tests failed")
                return False
            else:
                self.print_success("All tests passed!")

        return True


def main():
    """Main function"""
    parser = argparse.ArgumentParser(
        description="BadCppLib Cross-Platform Build System"
    )

    parser.add_argument(
        "--compiler", "-c", choices=["g++", "clang++", "cl"], help="Choose compiler"
    )

    parser.add_argument(
        "--build-type",
        "-b",
        choices=["debug", "release"],
        default="release",
        help="Build type (default: release)",
    )

    parser.add_argument(
        "--direct",
        "-d",
        action="store_true",
        help="Direct compilation without creating library",
    )

    parser.add_argument(
        "--run", "-r", action="store_true", help="Run example after build"
    )

    parser.add_argument(
        "--clean", action="store_true", help="Clean generated files"
    )

    parser.add_argument(
        "--info", "-i", action="store_true", help="Show system information"
    )

    parser.add_argument(
        "--test", "-t", action="store_true", help="Build and run tests"
    )

    args = parser.parse_args()

    build_system = BuildSystem()

    if args.clean:
        build_system.clean()
        return

    if args.info:
        build_system.print_system_info()
        compilers = build_system.find_compilers()
        if not compilers:
            build_system.print_warning("Compilers not found")
        return

    # Main build
    success = build_system.build(
        compiler=args.compiler,
        build_type=args.build_type,
        use_library=not args.direct,
        run_after=args.run,
        build_tests=args.test,
    )

    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
