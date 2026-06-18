# Brainfuck Interpreter

A lightweight, cross-platform Brainfuck interpreter written in C++23.

[![CI & Release](https://github.com/humonia-sys/brainfuck-interpreter/actions/workflows/ci.yml/badge.svg)](https://github.com/humonia-sys/brainfuck-interpreter/actions/workflows/ci.yml)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/humonia-sys/brainfuck-interpreter)](https://github.com/humonia-sys/brainfuck-interpreter/releases)

## Features
- **Cross-Platform Support**: Compiles and runs on Linux, macOS, and Windows.
- **Modern C++**: Built using modern C++23 features.
- **Bounded Tape**: Uses a 32,768-cell tape with high-performance bitwise circular wrapping.
- **Input Formatting**: Ignores non-Brainfuck characters automatically.

## Local Setup

### Prerequisites
- CMake 3.6 or higher
- A C++23 compatible compiler (e.g., GCC 13+, Clang 16+, or MSVC 2022+)

### Building
```bash
# Configure the build directory
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build --config Release
```

The compiled executable `brainfuck_interpreter` (or `brainfuck_interpreter.exe` on Windows) will be generated in the `build` directory.

### Running
```bash
./build/brainfuck_interpreter [options] <path_to_brainfuck_file>
```

#### Options:
* `-t, --time`: Enable execution time profiling (prints the elapsed time in milliseconds after execution completes).
* `-h, --help`: Display the usage help message.


---

## CI/CD & Releases

This project uses **GitHub Actions** for automated testing and deployment:

### 1. Continuous Integration (CI)
On every push and pull request to the `main` branch, the workflow:
- Configures and compiles the interpreter on Linux (`ubuntu-latest`), macOS (`macos-latest`), and Windows (`windows-latest`).
- Uploads the build binaries as action run artifacts.

### 2. Automated Releases
Whenever a new version tag (matching `v*` e.g. `v1.0.0`) is pushed to the repository:
- The build job compiles the binaries for all three platforms.
- The release job automatically packages the binaries (`.tar.gz` for Linux/macOS, `.zip` for Windows) and creates a new GitHub Release with these precompiled assets attached.

To publish a new release:
```bash
# 1. Create a version tag
git tag v1.0.0

# 2. Push the tag to GitHub (your SSH key will sign the commit/tag)
git push origin v1.0.0
```
