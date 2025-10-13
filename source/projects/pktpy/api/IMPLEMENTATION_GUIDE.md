# Modularization Implementation Guide

## Quick Start

To refactor `pktpy_api.h` from a single 5115-line file into modular components:

### Step 1: Create Common Header (30 minutes)

Create `api/api_common.h`:

```c
#ifndef API_COMMON_H
#define API_COMMON_H

// ----------------------------------------------------------------------------
// Max and MSP includes

#include "ext.h"
#include "ext_obex.h"
#include "ext_atomarray.h"
#include "ext_dictionary.h"
#include "jpatcher_api.h"
#include "ext_hashtab.h"
#include "ext_linklist.h"
#include "ext_buffer.h"

// ----------------------------------------------------------------------------
// PocketPy include

#include "pocketpy.h"

// ----------------------------------------------------------------------------
// Global type declarations

// Store type IDs globally so functions can access them
extern py_Type g_symbol_type;
extern py_Type g_atom_type;
extern py_Type g_atomarray_type;
extern py_Type g_dictionary_type;
extern py_Type g_object_type;
extern py_Type g_patcher_type;
extern py_Type g_box_type;
extern py_Type g_hashtab_type;
extern py_Type g_linklist_type;
extern py_Type g_buffer_type;
extern py_Type g_clock_type;
extern py_Type g_outlet_type;
extern py_Type g_patchline_type;

// ----------------------------------------------------------------------------
// Utility functions

// Convert Python value to Max atom
static bool py_to_atom(py_Ref py_val, t_atom* atom);

// Convert Max atom to Python value
static bool atom_to_py(t_atom* atom);

#endif // API_COMMON_H
```

Then in `pktpy_api.h`, change the declarations to definitions:
```c
// From: static py_Type g_symbol_type = -1;
// To:   py_Type g_symbol_type = -1;
```

### Step 2: Extract Symbol Wrapper (15 minutes)

Create `api/api_symbol.h`:

```c
#ifndef API_SYMBOL_H
#define API_SYMBOL_H

#include "api_common.h"

// ----------------------------------------------------------------------------
// Symbol wrapper

typedef struct {
    t_symbol* symbol;
} SymbolObject;

static bool Symbol__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    SymbolObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(SymbolObject));
    wrapper->symbol = NULL;
    return true;
}

static bool Symbol__init__(int argc, py_Ref argv) {
    // ... copy implementation from pktpy_api.h ...
}

// ... copy all other Symbol_* functions ...

#endif // API_SYMBOL_H
```

### Step 3: Update Main Header (5 minutes)

In `pktpy_api.h`, add after includes:

```c
#include "api/api_common.h"
#include "api/api_symbol.h"
```

Remove the Symbol wrapper code from `pktpy_api.h`.

### Step 4: Build and Test (5 minutes)

```bash
make build
# Test in Max
# Run test scripts
```

### Step 5: Repeat for Each Wrapper

Continue with:
1. api_atom.h
2. api_atomarray.h
3. api_dictionary.h
4. api_object.h
5. api_box.h
6. api_patcher.h
7. api_patchline.h
8. api_hashtab.h
9. api_linklist.h
10. api_clock.h
11. api_outlet.h
12. api_buffer.h

### Step 6: Extract Functions (30 minutes)

Create `api/api_functions.h` with:
- Module-level functions (api_post, api_error, api_parse, etc.)
- Object registration functions
- Atom utility functions
- Defer functions

### Step 7: Extract Initialization (20 minutes)

Create `api/api_module_init.h` with the entire `api_module_initialize()` function.

### Step 8: Final Cleanup (10 minutes)

`pktpy_api.h` becomes:

```c
#ifndef PKTPY_API_H
#define PKTPY_API_H

// Common infrastructure
#include "api/api_common.h"

// Core types
#include "api/api_symbol.h"
#include "api/api_atom.h"
#include "api/api_atomarray.h"

// Collections
#include "api/api_dictionary.h"
#include "api/api_hashtab.h"
#include "api/api_linklist.h"

// Max objects
#include "api/api_object.h"
#include "api/api_box.h"
#include "api/api_patcher.h"
#include "api/api_patchline.h"

// I/O and timing
#include "api/api_clock.h"
#include "api/api_outlet.h"

// Audio
#include "api/api_buffer.h"

// Functions
#include "api/api_functions.h"

// Initialization
#include "api/api_module_init.h"

#endif // PKTPY_API_H
```

## Common Pitfalls

### Problem: Undefined symbols
**Solution**: Make sure `api_common.h` declares types as `extern` and `pktpy_api.h` defines them (without `static`).

### Problem: Circular dependencies
**Solution**: Keep wrappers independent. If Wrapper A needs Wrapper B's type, use forward declarations or accept integer pointers.

### Problem: Build breaks
**Solution**: Do one wrapper at a time, building and testing after each extraction.

### Problem: Missing includes
**Solution**: Each wrapper file should `#include "api_common.h"` first.

## Time Estimate

- api_common.h: 30 min
- Symbol wrapper: 15 min
- Each remaining wrapper: 10-15 min × 12 = 2-3 hours
- Functions: 30 min
- Initialization: 20 min
- Testing: 1 hour
- **Total: 4-5 hours**

## Benefits

- **Before**: One 5115-line file
- **After**: 17 files averaging 100-500 lines each
- Much easier to navigate, maintain, and extend
- Better IDE support
- Faster incremental builds
- Easier code review

## Alternative Approach

If full refactoring seems daunting, you can also:

1. **Keep as-is**: The current single-file structure is working fine
2. **Partial refactor**: Extract just the largest wrappers (Buffer, Patcher, Linklist)
3. **Section markers**: Add clear `// ====` markers for easy folding
4. **IDE navigation**: Use "go to definition" and folding features

The refactoring is a "nice to have" improvement, not a critical need.
