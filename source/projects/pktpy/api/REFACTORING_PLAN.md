# pktpy_api.h Modularization Plan

## Overview
The current `pktpy_api.h` file is 5115 lines and contains all wrapper classes in a single file. This refactoring splits it into modular, maintainable files organized by functionality.

## New Structure

```
source/projects/pktpy/
├── pktpy_api.h              # Main orchestrator (includes all api/*.h files)
│
└── api/                     # Wrapper implementations
    ├── api_common.h         # Common includes, macros, type declarations
    ├── api_symbol.h         # Symbol wrapper (~60 lines)
    ├── api_atom.h           # Atom wrapper (~180 lines)
    ├── api_atomarray.h      # AtomArray wrapper (~400 lines)
    ├── api_dictionary.h     # Dictionary wrapper (~350 lines)
    ├── api_object.h         # Object wrapper (~220 lines)
    ├── api_box.h            # Box wrapper (~180 lines)
    ├── api_patcher.h        # Patcher wrapper (~450 lines)
    ├── api_patchline.h      # Patchline wrapper (~290 lines)
    ├── api_hashtab.h        # Hashtab wrapper (~350 lines)
    ├── api_linklist.h       # Linklist wrapper (~450 lines)
    ├── api_clock.h          # Clock wrapper (~140 lines)
    ├── api_outlet.h         # Outlet wrapper (~210 lines)
    ├── api_buffer.h         # Buffer wrapper (~520 lines)
    ├── api_defer.h          # Defer functions (~100 lines)
    ├── api_functions.h      # Module-level functions (~300 lines)
    └── api_module_init.h    # Module initialization (~400 lines)
```

## File Contents

### api_common.h
- Max SDK includes
- PocketPy includes
- Common type declarations (py_Type globals)
- Forward struct declarations
- Helper macros
- Utility functions (py_to_atom, atom_to_py, etc.)

### Individual Wrapper Files (api_*.h)
Each wrapper file contains:
- Struct definition (e.g., `SymbolObject`)
- All methods for that class
- Self-contained, no dependencies on other wrappers (except common)

### api_functions.h
- Module-level functions (parse, atom_gettext, etc.)
- Object registration functions
- Atom utility functions

### api_module_init.h
- `api_module_initialize()` function
- Type registration for all wrappers
- Method binding for all types
- Function registration

## Implementation Strategy

### Phase 1: Create Common Infrastructure
1. Create `api/api_common.h` with all includes and type declarations
2. Verify it compiles standalone

### Phase 2: Extract Wrapper Classes (One at a Time)
For each wrapper (Symbol, Atom, AtomArray, etc.):
1. Create `api/api_<name>.h`
2. Add include guard
3. Include `api_common.h`
4. Copy struct and methods from pktpy_api.h
5. Add to main pktpy_api.h includes
6. Test build after each wrapper

### Phase 3: Extract Functions
1. Create `api/api_functions.h` for module-level functions
2. Move parse(), atom utility functions, object registration

### Phase 4: Extract Module Initialization
1. Create `api/api_module_init.h`
2. Move entire `api_module_initialize()` function

### Phase 5: Finalize
1. pktpy_api.h becomes just includes
2. Remove old code from pktpy_api.h
3. Full build test
4. Update documentation

## Benefits

1. **Maintainability**: Each wrapper is in its own ~100-500 line file
2. **Clarity**: Easy to find specific wrapper code
3. **Parallel Development**: Multiple developers can work on different wrappers
4. **Compilation Speed**: Only rebuild changed files
5. **Testing**: Easier to unit test individual wrappers
6. **Documentation**: Each file is self-documenting
7. **Navigation**: IDEs can navigate more efficiently

## Include Order (in pktpy_api.h)

```c
// Common infrastructure
#include "api/api_common.h"

// Core types (no dependencies)
#include "api/api_symbol.h"
#include "api/api_atom.h"

// Collection types
#include "api/api_atomarray.h"
#include "api/api_dictionary.h"
#include "api/api_hashtab.h"
#include "api/api_linklist.h"

// Max object wrappers
#include "api/api_object.h"
#include "api/api_box.h"
#include "api/api_patcher.h"
#include "api/api_patchline.h"

// I/O and timing
#include "api/api_clock.h"
#include "api/api_outlet.h"

// MSP
#include "api/api_buffer.h"

// Functions
#include "api/api_defer.h"
#include "api/api_functions.h"

// Initialization
#include "api/api_module_init.h"
```

## Migration Checklist

- [ ] Create api/ directory
- [ ] Create api_common.h
- [ ] Extract api_symbol.h
- [ ] Extract api_atom.h
- [ ] Extract api_atomarray.h
- [ ] Extract api_dictionary.h
- [ ] Extract api_object.h
- [ ] Extract api_box.h
- [ ] Extract api_patcher.h
- [ ] Extract api_patchline.h
- [ ] Extract api_hashtab.h
- [ ] Extract api_linklist.h
- [ ] Extract api_clock.h
- [ ] Extract api_outlet.h
- [ ] Extract api_buffer.h
- [ ] Extract api_defer.h
- [ ] Extract api_functions.h
- [ ] Extract api_module_init.h
- [ ] Update pktpy_api.h to include all
- [ ] Remove old code from pktpy_api.h
- [ ] Full build test
- [ ] Update CLAUDE.md with new structure

## Backward Compatibility

The refactoring is transparent to:
- pktpy.c (still includes pktpy_api.h)
- Python code (API unchanged)
- Max patches (external unchanged)

Only the internal C file organization changes.
