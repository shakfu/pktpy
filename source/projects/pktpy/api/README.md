# API Module Organization

## Purpose

This directory contains modularized wrapper classes for the pktpy Max external's Python API. The original `pktpy_api.h` was 5115 lines in a single file. This structure breaks it into manageable, focused modules.

## Status

**Current State**: Planned but not yet implemented
**Reason**: The refactoring is substantial (extracting 5000+ lines into 17 files) and should be done incrementally with testing at each step.

## How to Implement the Refactoring

### Step-by-Step Process

1. **Create `api_common.h`** - Extract all common infrastructure:
   - All #include statements
   - Global type declarations (g_symbol_type, g_atom_type, etc.)
   - Forward struct declarations if needed
   - Utility functions (py_to_atom, atom_to_py)
   - Helper macros

2. **Extract One Wrapper at a Time** (Start with Symbol as it's smallest):
   - Create `api_symbol.h`
   - Add include guard and `#include "api_common.h"`
   - Copy SymbolObject struct and all Symbol_* functions
   - Add `#include "api/api_symbol.h"` to main pktpy_api.h
   - **BUILD AND TEST** before continuing

3. **Continue with Each Wrapper**:
   - api_atom.h
   - api_atomarray.h
   - api_dictionary.h
   - etc. (see REFACTORING_PLAN.md for full list)

4. **Extract Functions**:
   - api_functions.h - Module-level functions (parse, atom_gettext, object_register, etc.)
   - api_defer.h - Defer callback functions

5. **Extract Initialization**:
   - api_module_init.h - The entire api_module_initialize() function

6. **Final Cleanup**:
   - pktpy_api.h becomes just includes
   - Remove old code
   - Full regression test

### Example: Extracting Symbol Wrapper

**api/api_symbol.h:**
```c
#ifndef API_SYMBOL_H
#define API_SYMBOL_H

#include "api_common.h"

// Symbol wrapper
typedef struct {
    t_symbol* symbol;
} SymbolObject;

static bool Symbol__new__(int argc, py_Ref argv) {
    // ... implementation ...
}

static bool Symbol__init__(int argc, py_Ref argv) {
    // ... implementation ...
}

// ... all other Symbol methods ...

#endif // API_SYMBOL_H
```

**In pktpy_api.h:**
```c
#ifndef PKTPY_API_H
#define PKTPY_API_H

#include "api/api_common.h"
#include "api/api_symbol.h"
// ... other includes ...

#endif // PKTPY_API_H
```

## Benefits of Modular Structure

1. **Maintainability**: Each wrapper is 100-500 lines instead of 5000+
2. **Clarity**: Easy to find specific wrapper code
3. **Parallel Development**: Multiple people can work on different wrappers
4. **Compile Time**: Incremental builds only recompile changed files
5. **Testing**: Easier to test individual components
6. **Documentation**: Each file is self-documenting
7. **Navigation**: Better IDE support

## File Organization Plan

```
api/
├── README.md                    (this file)
├── REFACTORING_PLAN.md         (detailed plan)
├── api_common.h                (shared infrastructure)
│
├── Core Types
│   ├── api_symbol.h            (~60 lines)
│   ├── api_atom.h              (~180 lines)
│   └── api_atomarray.h         (~400 lines)
│
├── Collections
│   ├── api_dictionary.h        (~350 lines)
│   ├── api_hashtab.h           (~350 lines)
│   └── api_linklist.h          (~450 lines)
│
├── Max Objects
│   ├── api_object.h            (~220 lines)
│   ├── api_box.h               (~180 lines)
│   ├── api_patcher.h           (~450 lines)
│   └── api_patchline.h         (~290 lines)
│
├── I/O & Timing
│   ├── api_clock.h             (~140 lines)
│   └── api_outlet.h            (~210 lines)
│
├── Audio
│   └── api_buffer.h            (~520 lines)
│
├── Functions
│   ├── api_functions.h         (~300 lines)
│   └── api_defer.h             (~100 lines)
│
└── Initialization
    └── api_module_init.h       (~400 lines)
```

## Backward Compatibility

The refactoring is completely transparent:
- `pktpy.c` continues to `#include "pktpy_api.h"`
- Python API unchanged
- Max external interface unchanged
- Only internal C organization changes

## Testing Strategy

After each wrapper extraction:
1. Run `make build`
2. Test in Max with example patches
3. Run Python test scripts
4. Verify no regressions

## When to Do This

The refactoring should be done:
- When adding several new wrapper classes
- Before major feature additions
- During a dedicated refactoring sprint
- NOT during critical bug fixes or releases

## Alternative: Keep Single File

If the single file approach is working well, you can also:
- Use editor folding/sections to navigate
- Add clear section markers (already present)
- Use IDE "go to definition" features
- Accept the ~5000 line file as manageable

The refactoring is beneficial but not urgent.
