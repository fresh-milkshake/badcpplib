#!/usr/bin/env python3
"""
BadCppLib Documentation Generator

This script generates Doxygen documentation for BadCppLib with various
output formats and configuration options.

Usage:
    python scripts/generate_docs.py [options]

Options:
    --format html|latex|xml|man    Output format (default: html)
    --output <dir>                 Output directory (default: docs/generated)
    --config <file>                Custom Doxyfile (default: docs/Doxyfile)
    --open                         Open documentation after generation
    --clean                        Clean output directory before generation
    --warnings-only                Show only warnings and errors
    --check                        Check for documentation issues
    --serve [port]                 Serve HTML documentation locally
    --help                         Show this help message

Examples:
    python scripts/generate_docs.py
    python scripts/generate_docs.py --format html --open
    python scripts/generate_docs.py --clean --warnings-only
    python scripts/generate_docs.py --serve 8080
"""

import os
import sys
import subprocess
import argparse
import shutil
import http.server
import socketserver
import webbrowser
import platform
from pathlib import Path

class Colors:
    """ANSI color codes for terminal output."""
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_colored(message, color=Colors.ENDC):
    """Print colored message to terminal."""
    print(f"{color}{message}{Colors.ENDC}")

def print_header(message):
    """Print header message."""
    print_colored(f"\n{'='*60}", Colors.HEADER)
    print_colored(f" {message}", Colors.HEADER + Colors.BOLD)
    print_colored(f"{'='*60}", Colors.HEADER)

def print_success(message):
    """Print success message."""
    print_colored(f"✓ {message}", Colors.OKGREEN)

def print_warning(message):
    """Print warning message."""
    print_colored(f"⚠ {message}", Colors.WARNING)

def print_error(message):
    """Print error message."""
    print_colored(f"✗ {message}", Colors.FAIL)

def print_info(message):
    """Print info message."""
    print_colored(f"ℹ {message}", Colors.OKBLUE)

def check_dependencies():
    """Check if required tools are installed."""
    print_header("Checking Dependencies")
    
    dependencies = {
        'doxygen': 'Doxygen is required for generating documentation',
        'dot': 'Graphviz (dot) is optional but recommended for diagrams'
    }
    
    missing_required = []
    missing_optional = []
    
    for tool, description in dependencies.items():
        try:
            result = subprocess.run([tool, '--version'], 
                                  capture_output=True, text=True)
            if result.returncode == 0:
                version = result.stdout.strip().split('\n')[0]
                if tool == 'doxygen':
                    print_success(f"{tool}: {version}")
                else:
                    print_success(f"{tool}: {version} (optional)")
            else:
                if tool == 'doxygen':
                    missing_required.append((tool, description))
                else:
                    missing_optional.append((tool, description))
        except FileNotFoundError:
            if tool == 'doxygen':
                missing_required.append((tool, description))
            else:
                missing_optional.append((tool, description))
    
    if missing_required:
        print_error("Missing required dependencies:")
        for tool, desc in missing_required:
            print_error(f"  {tool}: {desc}")
        print_info("\nInstallation instructions:")
        print_info("  Ubuntu/Debian: sudo apt-get install doxygen graphviz")
        print_info("  macOS: brew install doxygen graphviz")
        print_info("  Windows: choco install doxygen.install graphviz")
        return False
    
    if missing_optional:
        print_warning("Missing optional dependencies:")
        for tool, desc in missing_optional:
            print_warning(f"  {tool}: {desc}")
    
    return True

def find_project_root():
    """Find the project root directory."""
    current = Path.cwd()
    while current != current.parent:
        if (current / 'include' / 'badcpplib').exists():
            return current
        current = current.parent
    
    # If not found, assume current directory
    return Path.cwd()

def create_custom_doxyfile(base_config, output_dir, format_type):
    """Create a custom Doxyfile with specified options."""
    project_root = find_project_root()
    custom_config = project_root / 'docs' / f'Doxyfile.{format_type}'
    
    print_info(f"Creating custom configuration: {custom_config}")
    
    # Read base configuration
    with open(base_config, 'r') as f:
        content = f.read()
    
    # Modify settings based on format
    modifications = {
        'html': {
            'GENERATE_HTML': 'YES',
            'GENERATE_LATEX': 'NO',
            'GENERATE_XML': 'NO',
            'GENERATE_MAN': 'NO',
            'HTML_OUTPUT': 'html'
        },
        'latex': {
            'GENERATE_HTML': 'NO',
            'GENERATE_LATEX': 'YES',
            'GENERATE_XML': 'NO',
            'GENERATE_MAN': 'NO',
            'USE_PDFLATEX': 'YES',
            'PDF_HYPERLINKS': 'YES'
        },
        'xml': {
            'GENERATE_HTML': 'NO',
            'GENERATE_LATEX': 'NO',
            'GENERATE_XML': 'YES',
            'GENERATE_MAN': 'NO'
        },
        'man': {
            'GENERATE_HTML': 'NO',
            'GENERATE_LATEX': 'NO',
            'GENERATE_XML': 'NO',
            'GENERATE_MAN': 'YES'
        }
    }
    
    # Apply modifications
    for key, value in modifications.get(format_type, {}).items():
        content = content.replace(f'{key} = ', f'{key} = {value} #')
    
    # Set output directory
    content = content.replace(
        'OUTPUT_DIRECTORY       = docs/generated',
        f'OUTPUT_DIRECTORY       = {output_dir}'
    )
    
    # Write custom configuration
    with open(custom_config, 'w') as f:
        f.write(content)
    
    return custom_config

def generate_documentation(config_file, output_dir, warnings_only=False):
    """Generate documentation using Doxygen."""
    print_header("Generating Documentation")
    print_info(f"Configuration: {config_file}")
    print_info(f"Output directory: {output_dir}")
    
    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    # Run Doxygen
    try:
        result = subprocess.run(['doxygen', str(config_file)], 
                              capture_output=True, text=True)
        
        if warnings_only:
            # Filter to show only warnings and errors
            lines = result.stderr.split('\n')
            filtered_lines = [line for line in lines 
                            if 'warning' in line.lower() or 'error' in line.lower()]
            if filtered_lines:
                print_warning("Documentation warnings/errors:")
                for line in filtered_lines:
                    print(line)
            else:
                print_success("No warnings or errors found!")
        else:
            if result.stdout:
                print(result.stdout)
            if result.stderr:
                print(result.stderr)
        
        if result.returncode == 0:
            print_success("Documentation generated successfully!")
            return True
        else:
            print_error(f"Doxygen failed with return code {result.returncode}")
            return False
            
    except Exception as e:
        print_error(f"Error running Doxygen: {e}")
        return False

def check_documentation_issues(project_root):
    """Check for common documentation issues."""
    print_header("Checking Documentation Issues")
    
    issues = []
    
    # Check for undocumented functions
    include_dir = project_root / 'include' / 'badcpplib'
    if include_dir.exists():
        for header_file in include_dir.glob('*.hpp'):
            with open(header_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
                # Simple check for functions without documentation
                lines = content.split('\n')
                for i, line in enumerate(lines):
                    if (('(' in line and ')' in line and 
                         ('template' in line or 'inline' in line or 
                          line.strip().endswith(';'))) and
                        i > 0 and not lines[i-1].strip().startswith('*')):
                        if not any(doc_line.strip().startswith('//') or 
                                 doc_line.strip().startswith('*') 
                                 for doc_line in lines[max(0, i-3):i]):
                            issues.append(f"{header_file.name}:{i+1}: "
                                        f"Possible undocumented function: {line.strip()}")
    
    if issues:
        print_warning(f"Found {len(issues)} potential documentation issues:")
        for issue in issues[:10]:  # Show first 10
            print(f"  {issue}")
        if len(issues) > 10:
            print_info(f"  ... and {len(issues) - 10} more")
    else:
        print_success("No obvious documentation issues found!")

def open_documentation(output_dir, format_type):
    """Open generated documentation."""
    if format_type == 'html':
        index_file = Path(output_dir) / 'html' / 'index.html'
        if index_file.exists():
            print_info(f"Opening documentation: {index_file}")
            if platform.system() == 'Darwin':  # macOS
                subprocess.run(['open', str(index_file)])
            elif platform.system() == 'Windows':
                os.startfile(str(index_file))
            else:  # Linux
                subprocess.run(['xdg-open', str(index_file)])
        else:
            print_error(f"Documentation file not found: {index_file}")
    else:
        print_info(f"Generated {format_type} documentation in {output_dir}")

def serve_documentation(output_dir, port=8000):
    """Serve HTML documentation locally."""
    html_dir = Path(output_dir) / 'html'
    if not html_dir.exists():
        print_error(f"HTML documentation not found in {html_dir}")
        return
    
    print_header(f"Serving Documentation on Port {port}")
    print_info(f"Documentation directory: {html_dir}")
    print_info(f"URL: http://localhost:{port}")
    print_info("Press Ctrl+C to stop the server")
    
    # Change to documentation directory
    os.chdir(html_dir)
    
    # Open browser
    webbrowser.open(f'http://localhost:{port}')
    
    # Start server
    try:
        with socketserver.TCPServer(("", port), http.server.SimpleHTTPRequestHandler) as httpd:
            httpd.serve_forever()
    except KeyboardInterrupt:
        print_info("\nServer stopped")
    except OSError as e:
        print_error(f"Failed to start server: {e}")

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description='Generate Doxygen documentation for BadCppLib',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    
    parser.add_argument('--format', choices=['html', 'latex', 'xml', 'man'],
                       default='html', help='Output format (default: html)')
    parser.add_argument('--output', default='docs/generated',
                       help='Output directory (default: docs/generated)')
    parser.add_argument('--config', default='docs/Doxyfile',
                       help='Custom Doxyfile (default: docs/Doxyfile)')
    parser.add_argument('--open', action='store_true',
                       help='Open documentation after generation')
    parser.add_argument('--clean', action='store_true',
                       help='Clean output directory before generation')
    parser.add_argument('--warnings-only', action='store_true',
                       help='Show only warnings and errors')
    parser.add_argument('--check', action='store_true',
                       help='Check for documentation issues')
    parser.add_argument('--serve', type=int, nargs='?', const=8000,
                       help='Serve HTML documentation locally (default port: 8000)')
    
    args = parser.parse_args()
    
    # Find project root
    project_root = find_project_root()
    print_info(f"Project root: {project_root}")
    
    # Convert relative paths to absolute
    config_file = project_root / args.config
    output_dir = project_root / args.output
    
    # Check if we're just serving existing documentation
    if args.serve is not None:
        serve_documentation(output_dir, args.serve)
        return
    
    # Check dependencies
    if not check_dependencies():
        sys.exit(1)
    
    # Check for documentation issues if requested
    if args.check:
        check_documentation_issues(project_root)
        return
    
    # Verify configuration file exists
    if not config_file.exists():
        print_error(f"Configuration file not found: {config_file}")
        sys.exit(1)
    
    # Clean output directory if requested
    if args.clean and output_dir.exists():
        print_info(f"Cleaning output directory: {output_dir}")
        shutil.rmtree(output_dir)
    
    # Create custom configuration if format is not HTML
    if args.format != 'html':
        config_file = create_custom_doxyfile(config_file, output_dir, args.format)
    
    # Generate documentation
    success = generate_documentation(config_file, output_dir, args.warnings_only)
    
    if not success:
        sys.exit(1)
    
    # Clean up custom config file
    if args.format != 'html' and config_file.name.startswith('Doxyfile.'):
        config_file.unlink()
    
    # Open documentation if requested
    if args.open:
        open_documentation(output_dir, args.format)
    
    print_success("Documentation generation completed!")

if __name__ == '__main__':
    main() 