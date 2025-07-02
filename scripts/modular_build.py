#!/usr/bin/env python3
"""
Modular build system for BadCppLib
Supports selective module compilation and automatic dependency resolution.
"""

import os
import sys
import argparse
import subprocess
import json
from pathlib import Path
from typing import List, Dict, Set, Optional

class Colors:
    """ANSI colors for console output"""
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    WHITE = '\033[97m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

def colorize(text: str, color: str) -> str:
    """Adds color to text"""
    return f"{color}{text}{Colors.END}"

class ModularBuildSystem:
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.include_dir = self.project_root / "include"
        self.src_dir = self.project_root / "src"
        self.build_dir = self.project_root / "build_cmake"
        self.tests_dir = self.project_root / "tests"
        
        # Module definitions and dependencies (updated)
        self.modules = {
            'core': {
                'description': 'Core types and utilities',
                'depends': [],
                'headers': ['badcpplib/core.hpp'],
                'sources': [],
                'defines': ['BADCPPLIB_ENABLE_CORE'],
                'required': True  # Always included
            },
            'result': {
                'description': 'Result<T,E> type for error handling',
                'depends': ['core'],
                'headers': ['badcpplib/result.hpp'],
                'sources': [],
                'defines': ['BADCPPLIB_ENABLE_RESULT']
            },
            'string_utils': {
                'description': 'String manipulation utilities',
                'depends': ['core'],
                'headers': ['badcpplib/string_utils.hpp'],
                'sources': ['modules/string_utils.cpp'],
                'defines': ['BADCPPLIB_ENABLE_STRING']
            },
            'math_utils': {
                'description': 'Mathematical functions and utilities',
                'depends': ['core'],
                'headers': ['badcpplib/math_utils.hpp'],
                'sources': ['modules/math_utils.cpp'],
                'defines': ['BADCPPLIB_ENABLE_MATH']
            },
            'containers': {
                'description': 'Custom container classes',
                'depends': ['core'],
                'headers': ['badcpplib/containers.hpp'],
                'sources': [],
                'defines': ['BADCPPLIB_ENABLE_CONTAINERS']
            },
            'file_utils': {
                'description': 'File and filesystem utilities',
                'depends': ['core', 'result'],
                'headers': ['badcpplib/file_utils.hpp'],
                'sources': [],
                'defines': ['BADCPPLIB_ENABLE_FILE_UTILS']
            },
            'time_utils': {
                'description': 'Time and duration utilities',
                'depends': ['core', 'result'],
                'headers': ['badcpplib/time_utils.hpp'],
                'sources': [],
                'defines': ['BADCPPLIB_ENABLE_TIME_UTILS']
            },
            'functional': {
                'description': 'Functional programming utilities',
                'depends': ['core'],
                'headers': ['badcpplib/functional.hpp'],
                'sources': [],
                'defines': ['BADCPPLIB_ENABLE_FUNCTIONAL']
            },
            'debug': {
                'description': 'Debug and logging utilities',
                'depends': ['core', 'time_utils'],
                'headers': ['badcpplib/debug.hpp'],
                'sources': [],
                'defines': ['BADCPPLIB_ENABLE_DEBUG']
            },
            'memory': {
                'description': 'Memory management utilities',
                'depends': ['core'],
                'headers': ['badcpplib/memory.hpp'],
                'sources': [],
                'defines': ['BADCPPLIB_ENABLE_MEMORY']
            },
            'test_framework': {
                'description': 'Custom testing framework',
                'depends': ['core'],
                'headers': ['badcpplib/test_framework.hpp'],
                'sources': ['modules/test_framework.cpp'],
                'defines': ['BADCPPLIB_ENABLE_TEST']
            }
        }
        
        # Compilers
        self.compilers = {
            'g++': {
                'command': 'g++',
                'flags': ['-std=c++17', '-Wall', '-Wextra', '-Wpedantic']
            },
            'clang++': {
                'command': 'clang++',
                'flags': ['-std=c++17', '-Wall', '-Wextra', '-Wpedantic']
            },
            'cl': {
                'command': 'cl',
                'flags': ['/std:c++17', '/W4']
            }
        }
    
    def resolve_dependencies(self, module_names: List[str]) -> Set[str]:
        """Resolves module dependencies"""
        resolved = set()
        
        def resolve_module(name: str):
            if name in resolved:
                return
            
            if name not in self.modules:
                raise ValueError(f"Unknown module: {name}")
            
            module = self.modules[name]
            
            # First resolve dependencies
            for dep in module['depends']:
                resolve_module(dep)
            
            resolved.add(name)
        
        # core is always included
        resolve_module('core')
        
        for module_name in module_names:
            resolve_module(module_name)
        
        return resolved
    
    def list_modules(self):
        """Lists available modules"""
        print(colorize("Available modules:", Colors.BOLD + Colors.CYAN))
        print()
        
        for name, info in self.modules.items():
            status = colorize("[REQUIRED]", Colors.GREEN) if info.get('required', False) else ""
            print(f"  {colorize(name, Colors.YELLOW)} {status}")
            print(f"    {info['description']}")
            
            if info['depends']:
                deps = ", ".join(info['depends'])
                print(f"    Dependencies: {colorize(deps, Colors.MAGENTA)}")
            
            print()
    
    def get_compiler_command(self, compiler: str, build_type: str) -> List[str]:
        """Forms compiler command"""
        if compiler not in self.compilers:
            raise ValueError(f"Unknown compiler: {compiler}")
        
        comp_info = self.compilers[compiler]
        cmd = [comp_info['command']]
        cmd.extend(comp_info['flags'])
        
        # Build flags
        if build_type == 'debug':
            if compiler == 'cl':
                cmd.extend(['/Od', '/Zi', '/MDd'])
            else:
                cmd.extend(['-g', '-O0'])
        elif build_type == 'release':
            if compiler == 'cl':
                cmd.extend(['/O2', '/MD', '/DNDEBUG'])
            else:
                cmd.extend(['-O2', '-DNDEBUG'])
        
        return cmd
    
    def collect_sources(self, modules: Set[str]) -> List[Path]:
        """Collects source files for modules"""
        sources = []
        
        for module_name in modules:
            module = self.modules[module_name]
            for src in module['sources']:
                src_path = self.src_dir / src
                if src_path.exists():
                    sources.append(src_path)
                else:
                    print(colorize(f"Warning: file {src_path} not found", Colors.YELLOW))
        
        return sources
    
    def collect_defines(self, modules: Set[str]) -> List[str]:
        """Collects preprocessor macros for modules"""
        defines = []
        
        for module_name in modules:
            module = self.modules[module_name]
            defines.extend(module['defines'])
        
        return defines
    
    def build_library(self, modules: Set[str], compiler: str, build_type: str) -> bool:
        """Compiles the library"""
        print(colorize(f"Building library with modules: {', '.join(sorted(modules))}", Colors.BOLD))
        
        # Create build directory
        self.build_dir.mkdir(exist_ok=True)
        
        # Collect files
        sources = self.collect_sources(modules)
        defines = self.collect_defines(modules)
        
        if not sources:
            print(colorize("No source files to compile", Colors.YELLOW))
            return True
        
        # Form command
        cmd = self.get_compiler_command(compiler, build_type)
        
        # Add definitions
        for define in defines:
            if compiler == 'cl':
                cmd.append(f'/D{define}')
            else:
                cmd.append(f'-D{define}')
        
        # Add include paths
        if compiler == 'cl':
            cmd.append(f'/I{self.include_dir}')
        else:
            cmd.append(f'-I{self.include_dir}')
        
        # Add source files
        cmd.extend([str(src) for src in sources])
        
        # Output file
        output_file = self.build_dir / f"libbadcpplib.{'lib' if compiler == 'cl' else 'a'}"
        
        if compiler == 'cl':
            # Compile to object files
            obj_files = []
            for src in sources:
                obj_file = self.build_dir / f"{src.stem}.obj"
                obj_cmd = cmd.copy()
                obj_cmd.extend([f'/Fo{obj_file}', '/c', str(src)])
                
                print(f"Compiling {src.name}...")
                result = subprocess.run(obj_cmd, capture_output=True, text=True)
                if result.returncode != 0:
                    print(colorize(f"Compilation error {src.name}:", Colors.RED))
                    print(result.stderr)
                    return False
                obj_files.append(str(obj_file))
            
            # Create library
            lib_cmd = ['lib', f'/OUT:{output_file}'] + obj_files
        else:
            # Compile object files one by one
            obj_files = []
            for src in sources:
                obj_file = self.build_dir / f"{src.stem}.o"
                # Create separate command for each file
                src_cmd = self.get_compiler_command(compiler, build_type)
                
                # Add definitions
                for define in defines:
                    src_cmd.append(f'-D{define}')
                
                # Add include paths
                src_cmd.append(f'-I{self.include_dir}')
                
                # Compilation flags
                src_cmd.extend(['-c', str(src), '-o', str(obj_file)])
                
                print(f"Compiling {src.name}...")
                result = subprocess.run(src_cmd, capture_output=True, text=True)
                if result.returncode != 0:
                    print(colorize(f"Compilation error {src.name}:", Colors.RED))
                    print(result.stderr)
                    return False
                obj_files.append(str(obj_file))
            
            # Create archive
            lib_cmd = ['ar', 'rcs', str(output_file)] + obj_files
        
        print("Creating library...")
        result = subprocess.run(lib_cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(colorize("Library creation error:", Colors.RED))
            print(result.stderr)
            return False
        
        print(colorize(f"Library created: {output_file}", Colors.GREEN))
        return True
    
    def build_tests(self, modules: Set[str], compiler: str, build_type: str) -> bool:
        """Compiles and runs tests"""
        print(colorize("Building and running tests", Colors.BOLD))
        
        # Collect test files
        test_files = []
        
        # Basic test
        basic_test = self.tests_dir / "basic_test.cpp"
        if basic_test.exists():
            test_files.append(basic_test)
        
        # Module tests
        for module_name in modules:
            test_file = self.tests_dir / "modules" / f"test_{module_name}.cpp"
            if test_file.exists():
                test_files.append(test_file)
        
        if not test_files:
            print(colorize("Test files not found", Colors.YELLOW))
            return True
        
        success_count = 0
        total_count = len(test_files)
        
        for test_file in test_files:
            print(f"\nTest: {colorize(test_file.name, Colors.CYAN)}")
            
            # Form compilation command
            cmd = self.get_compiler_command(compiler, build_type)
            
            # Add definitions
            defines = self.collect_defines(modules)
            for define in defines:
                if compiler == 'cl':
                    cmd.append(f'/D{define}')
                else:
                    cmd.append(f'-D{define}')
            
            # Add include paths
            if compiler == 'cl':
                cmd.append(f'/I{self.include_dir}')
            else:
                cmd.append(f'-I{self.include_dir}')
            
            # Add source files (test + modules with sources)
            cmd.append(str(test_file))
            sources = self.collect_sources(modules)
            cmd.extend([str(src) for src in sources])
            
            # Output file
            exe_name = test_file.stem + ('.exe' if compiler == 'cl' else '')
            output_file = self.build_dir / exe_name
            
            if compiler == 'cl':
                cmd.extend([f'/Fe{output_file}'])
            else:
                cmd.extend(['-o', str(output_file)])
            
            # Compilation
            result = subprocess.run(cmd, capture_output=True, text=True)
            if result.returncode != 0:
                print(colorize(f"Test compilation error {test_file.name}:", Colors.RED))
                print(result.stderr)
                continue
            
            # Run test
            result = subprocess.run([str(output_file)], capture_output=True, text=True, 
                                  cwd=self.project_root)
            
            if result.returncode == 0:
                print(colorize("✓ PASSED", Colors.GREEN))
                success_count += 1
            else:
                print(colorize("✗ FAILED", Colors.RED))
                if result.stdout:
                    print("STDOUT:", result.stdout)
                if result.stderr:
                    print("STDERR:", result.stderr)
        
        # Final result
        print(f"\nTest results: {colorize(f'{success_count}/{total_count}', Colors.BOLD)}")
        
        if success_count == total_count:
            print(colorize("All tests passed successfully! ✓", Colors.GREEN))
            return True
        else:
            print(colorize(f"Failed tests: {total_count - success_count}", Colors.RED))
            return False

def main():
    parser = argparse.ArgumentParser(description="BadCppLib modular build system")
    
    parser.add_argument('--modules', nargs='+', default=['core'],
                       help='Modules to include (default: core)')
    parser.add_argument('--list-modules', action='store_true',
                       help='Show list of available modules')
    parser.add_argument('--minimal', action='store_true',
                       help='Minimal build (core + result)')
    parser.add_argument('--full', action='store_true',
                       help='Full build (all modules)')
    parser.add_argument('--compiler', choices=['g++', 'clang++', 'cl'], default='g++',
                       help='Compiler (default: g++)')
    parser.add_argument('--build-type', choices=['debug', 'release'], default='release',
                       help='Build type (default: release)')
    parser.add_argument('--test', action='store_true',
                       help='Run tests after build')
    parser.add_argument('--test-only', action='store_true',
                       help='Only run tests (without building library)')
    
    args = parser.parse_args()
    
    build_system = ModularBuildSystem()
    
    if args.list_modules:
        build_system.list_modules()
        return 0
    
    # Determine modules
    modules_to_build = set()
    
    if args.minimal:
        modules_to_build = {'core', 'result'}
    elif args.full or 'all' in args.modules:
        modules_to_build = set(build_system.modules.keys())
    else:
        modules_to_build = set(args.modules)
    
    try:
        # Resolve dependencies
        resolved_modules = build_system.resolve_dependencies(list(modules_to_build))
        
        print(colorize("Selected modules:", Colors.BOLD))
        for module in sorted(resolved_modules):
            info = build_system.modules[module]
            print(f"  • {colorize(module, Colors.YELLOW)} - {info['description']}")
        print()
        
        success = True
        
        # Build library
        if not args.test_only:
            success = build_system.build_library(resolved_modules, args.compiler, args.build_type)
        
        # Run tests
        if args.test or args.test_only:
            # Add test_framework to modules for testing
            test_modules = resolved_modules | {'test_framework'}
            test_success = build_system.build_tests(test_modules, args.compiler, args.build_type)
            success = success and test_success
        
        return 0 if success else 1
        
    except Exception as e:
        print(colorize(f"Error: {e}", Colors.RED))
        return 1

if __name__ == "__main__":
    sys.exit(main()) 