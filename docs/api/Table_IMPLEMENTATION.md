# Table API Implementation Notes

## Overview

The Table API wrapper provides Python access to Max's `table` objects through the `table_get()` and `table_dirty()` C API functions.

## Architecture

### File Structure

Following the modular API wrapper pattern:

```
source/projects/pktpy/api/
├── api_common.h          # Shared declarations (includes g_table_type)
├── api_table.h           # Table wrapper implementation
└── ...

source/projects/pktpy/
└── pktpy_api.h          # Main header (includes api_table.h, registers type)
```

### C Structure

```c
typedef struct {
    t_symbol* name;      // Table name
    long** handle;       // Pointer to pointer to array
    long size;           // Number of elements
    bool is_bound;       // Binding status
} TableObject;
```

**Key Design Points:**

1. **Handle Indirection**: Max tables use `long**` (pointer to pointer) because the actual array can be reallocated. Always dereference through the handle.

2. **No Ownership**: The wrapper doesn't own the table data - Max manages allocation. The destructor is a no-op.

3. **Binding Pattern**: Tables must be explicitly bound with `bind(name)` before use, unlike Buffer which can create references.

4. **Refresh Semantics**: Since table references can become stale, `refresh()` re-calls `table_get()`.

## Max C API Functions Used

### `table_get()`
```c
short table_get(t_symbol *s, long ***hp, long *sp);
```

- Returns 0 on success, non-zero if table doesn't exist
- Sets `hp` to pointer to handle (pointer to pointer to array)
- Sets `sp` to size in elements
- Must be called each time before accessing table (Max best practice)

### `table_dirty()`
```c
short table_dirty(t_symbol *s);
```

- Marks table as modified
- Called automatically after `set()`, `from_list()`, `fill()`, `copy_from()`
- Returns 0 on success

## Implementation Details

### Binding Flow

1. User calls `t.bind("mytable")`
2. Wrapper stores symbol and calls `table_get()`
3. If successful, stores handle, size, sets `is_bound = true`
4. If failed, clears state, returns `false`

### Array Access

```c
// Correct access pattern:
long value = (*self->handle)[index];

// Why the double indirection:
// - self->handle is long** (pointer to pointer)
// - *self->handle dereferences to long* (pointer to array)
// - (*self->handle)[index] gets the value at index
```

### Safety Mechanisms

1. **Bound Checks**: All access methods check `is_bound` first
2. **Range Checks**: Index validation against `size`
3. **Null Checks**: Handle verified non-NULL before dereference
4. **Type Checks**: `copy_from()` validates source is Table type

### Python Integration

The wrapper exposes both explicit methods and Python operators:

**Explicit Methods:**
- `get(index)`, `set(index, value)` - Explicit access
- `size()` - Explicit size query
- `to_list()`, `from_list()` - Bulk conversion

**Python Operators:**
- `__len__()` enables `len(t)`
- `__getitem__()` enables `t[i]`
- `__setitem__()` enables `t[i] = value`
- `__repr__()` enables `print(t)`

## Performance Characteristics

- **Binding**: O(1) - hash table lookup in Max
- **Get/Set**: O(1) - direct array access
- **to_list()**: O(n) - must copy all elements
- **from_list()**: O(n) - must copy all elements
- **fill()**: O(n) - must set all elements
- **copy_from()**: O(k) where k is count

## Comparison with Buffer

| Feature | Table | Buffer |
|---------|-------|--------|
| Data Type | `long` (int) | `float` |
| Use Case | Control data | Audio samples |
| Size | KB range | MB range |
| Thread Safety | Not thread-safe | Lock/unlock required |
| Overhead | Minimal | Higher (MSP integration) |
| API Complexity | Simple (2 functions) | Complex (20+ functions) |

## Testing

### Unit Tests
`tests/test_table.py` - Mock-based unit tests for Python logic

### Integration Tests
`docs/examples/table_example.py` - Real Max integration examples

**Test Coverage:**
- Binding and unbinding
- Get/set operations
- Index validation
- Python operators (`len`, `[]`)
- List conversions
- Fill operations
- Copy operations
- Error handling

## Known Limitations

1. **Integer Only**: Tables store only `long` integers, no floats
2. **No Create**: Cannot create tables from Python, must exist in Max
3. **No Resize**: Cannot change table size from Python
4. **Stale References**: References can become invalid if table deleted
5. **No Threading**: Table API is not thread-safe
6. **Global Namespace**: All tables share Max's global namespace

## Best Practices

### Always Check Binding
```python
t = api.Table()
if not t.bind("mytable"):
    api.error("Table not found")
    return
```

### Refresh After Operations
```python
# If table might have been deleted/recreated
if not t.refresh():
    api.error("Table no longer exists")
    return
```

### Bulk Operations Over Loops
```python
# Good: Single bulk operation
t.from_list(range(1000))

# Less efficient: Many individual sets
for i in range(1000):
    t.set(i, i)
```

### Use Python Operators
```python
# More Pythonic
for i in range(len(t)):
    t[i] = i * 2

# Less Pythonic
for i in range(t.size()):
    t.set(i, i * 2)
```

## Future Enhancements

Possible additions:
1. Iterator protocol for `for value in table:`
2. Slice support `t[10:20]`
3. Math operations `t1 + t2`
4. Numpy-style operations if needed
5. Context manager for bind/unbind
6. Comparison operators

## References

- Max SDK: `ext_proto.h` - table function declarations
- Max documentation: table object
- pktpy CLAUDE.md: Modular API wrapper pattern
- api_buffer.h: Similar wrapper for audio data
