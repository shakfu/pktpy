# pktpy -- pocketpy Max/MSP external

A lightweight Python 3.x interpreter for Max/MSP, built with [pocketpy](https://github.com/pocketpy/pocketpy) v2.0.8.

## Overview

This project provides two Max externals for Python integration:

### `pktpy` - Python Interpreter

A complete Python interpreter that executes code directly within Max patches. Use it for scripting, data processing, and algorithmic composition.

**Key Features:**
- Execute Python code via `eval`, `exec`, and `execfile` messages
- Built-in code editor (double-click the object)
- Import Python modules and maintain interpreter state across executions
- Automatic type conversion between Max atoms and Python types (int, float, str, list, tuple, bool)
- Custom Python API bindings for Max integration
- Compact binary size (~572KB) with no external dependencies

### `pyext` - Write Externals in Python

Write Max externals entirely in Python without compiling C code. Think of it as the Python equivalent of Max's `js` object.

**Key Features:**
- Write Max objects in pure Python using class-based structure
- Hot reload - edit scripts while Max is running
- Use any class name with `@api.external` decorator
- Full Max API access (outlets, inlets, attributes, timing, etc.)
- Lightweight (~815KB) thanks to pocketpy v2.0.8
- No compilation needed - instant iteration
- Built-in code editor with auto-reload on save/close

## Quick Start

### Building from Source

```bash
# Initial setup (clone submodules and create Max package symlinks)
make setup

# Build the external
make build

# Clean build artifacts
make clean
```

After building, the external will be available in Max 8/9 via the automatically created symlinks in your Packages directory.

### Development Workflow

The project uses CMake with Max SDK integration. After `make setup`, the project is symlinked to `~/Documents/Max 8/Packages/pktpy` and/or `~/Documents/Max 9/Packages/pktpy`, allowing Max to load the external directly from the build location.

## Usage in Max

### Using `pktpy` (Python Interpreter)

Create a `pktpy` object and send it messages:

- `bang` - Execute stored Python code
- `eval <expression>` - Evaluate a Python expression and output the result
- `exec <statement>` - Execute Python statements
- `execfile <path>` - Execute a Python file from the filesystem
- `import <module>` - Import a Python module
- Double-click to open the built-in code editor

**Outlets** (right to left):
- Right outlet: Error bang
- Middle outlet: Success bang
- Left outlet: Python output (converted to Max atoms)

**Example:**
```
[eval 2 + 2(  ->  [pktpy]  ->  [print]   # outputs: 4
```

### Using `pyext` (Write Externals in Python)

Create a `pyext` object with a Python script filename:

```
[pyext my_script.py]
```

**Python script structure:**
```python
import api

@api.external
class MyExternal:
    def __init__(self):
        self.inlets = 1
        self.outlets = 1

    def bang(self):
        api.post("Bang received!")
        self._outlets[0].int(42)
```

**Available messages:**
- `reload` - Reload the Python script
- `load <script.py>` - Load a different Python script
- Double-click to open the built-in code editor (auto-reloads on close)

**See `examples/pyext/README.md` for detailed documentation and examples.**

## Project Structure

```
source/
├── projects/
│   ├── pktpy/          # Python interpreter external
│   │   ├── pktpy.c/h   # Max external with message handlers
│   │   ├── pktpy_api.h # Python C API bindings for Max
│   │   └── tests/      # Standalone pocketpy C API examples
│   └── pyext/          # Write-externals-in-Python external
│       └── pyext.c/h   # Max external for Python-based objects
├── thirdparty/
│   └── pocketpy/       # pocketpy v2.0.8 (git submodule)
├── max-sdk-base/       # Max SDK (git submodule)
└── examples/
    └── pyext/          # Example Python scripts for pyext
        ├── README.md   # Complete pyext documentation
        ├── simple.py   # Basic example
        ├── counter.py  # Counter with state
        └── scale.py    # Multi-inlet example
```

## About pocketpy

[pocketpy](https://github.com/pocketpy/pocketpy) is a portable Python 3.x interpreter written in C11. It aims to be an alternative to Lua for embedding scripting capabilities, with elegant syntax, powerful features, and competitive performance. pocketpy has no dependencies other than the C standard library and can be easily integrated into C/C++ projects.

## Requirements

- macOS (arm64/x86_64) or Windows 10 or 11
- Max 8 or Max 9
- CMake 3.19+
- C11-compatible compiler (Clang/GCC)

## License

See LICENSE file for details.
