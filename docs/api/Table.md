# Table API

The `api.Table` class provides Python access to Max's table objects - named global arrays of integer values.

## Overview

Tables in Max are lightweight integer arrays stored globally by name. They're simpler and more efficient than buffer~ objects for non-audio data, making them ideal for:
- Lookup tables
- Breakpoint envelopes
- Step sequences
- Integer data storage
- Control data arrays

Tables store `long` integers and are accessed by name through the Max global namespace.

## Class: `api.Table`

### Constructor

```python
t = api.Table()
```

Creates a new Table wrapper object. The table must be bound to a named Max table before use.

### Methods

#### `bind(name: str) -> bool`
Bind to a named table object in Max.

**Parameters:**
- `name`: Name of the table object (string)

**Returns:**
- `True` if binding succeeded, `False` if table doesn't exist

**Example:**
```python
t = api.Table()
if t.bind("mytable"):
    api.post("Successfully bound to mytable")
else:
    api.error("Table 'mytable' not found")
```

---

#### `refresh() -> bool`
Refresh the table reference. Call this if the table might have been deleted and recreated.

**Returns:**
- `True` if table still exists, `False` otherwise

**Example:**
```python
if t.refresh():
    api.post("Table still valid")
```

---

#### `get(index: int) -> int`
Get value at specified index.

**Parameters:**
- `index`: Array index (0-based)

**Returns:**
- Integer value at index

**Raises:**
- `RuntimeError` if table not bound
- `IndexError` if index out of range

**Example:**
```python
value = t.get(0)
api.post(f"First element: {value}")
```

---

#### `set(index: int, value: int) -> None`
Set value at specified index and mark table as dirty.

**Parameters:**
- `index`: Array index (0-based)
- `value`: Integer value to store

**Raises:**
- `RuntimeError` if table not bound
- `IndexError` if index out of range

**Example:**
```python
t.set(0, 42)
```

---

#### `size() -> int`
Get the size of the table.

**Returns:**
- Number of elements in table, or 0 if not bound

**Example:**
```python
length = t.size()
api.post(f"Table has {length} elements")
```

---

#### `is_bound() -> bool`
Check if table is currently bound.

**Returns:**
- `True` if bound, `False` otherwise

---

#### `name() -> str | None`
Get the name of the bound table.

**Returns:**
- Table name string, or `None` if not bound

---

#### `to_list() -> list[int]`
Convert entire table to Python list.

**Returns:**
- List of integer values

**Raises:**
- `RuntimeError` if table not bound

**Example:**
```python
values = t.to_list()
api.post(f"Table values: {values}")
```

---

#### `from_list(values: list) -> int`
Set table contents from Python list. Copies as many values as will fit.

**Parameters:**
- `values`: List of numeric values (ints or floats)

**Returns:**
- Number of values copied

**Raises:**
- `RuntimeError` if table not bound

**Example:**
```python
ramp = list(range(100))
count = t.from_list(ramp)
api.post(f"Wrote {count} values")
```

---

#### `fill(value: int) -> None`
Fill entire table with a single value.

**Parameters:**
- `value`: Integer value to fill with

**Raises:**
- `RuntimeError` if table not bound

**Example:**
```python
t.fill(0)  # Clear table to zeros
```

---

#### `copy_from(source: Table, src_offset=0, dst_offset=0, count=-1) -> int`
Copy data from another table.

**Parameters:**
- `source`: Source Table object
- `src_offset`: Starting index in source (default: 0)
- `dst_offset`: Starting index in destination (default: 0)
- `count`: Number of elements to copy, or -1 for maximum (default: -1)

**Returns:**
- Number of elements actually copied

**Raises:**
- `RuntimeError` if source or destination not bound
- `IndexError` if offsets out of range

**Example:**
```python
src = api.Table()
src.bind("source_table")

dst = api.Table()
dst.bind("dest_table")

# Copy 20 values from src[10:] to dst[0:]
count = dst.copy_from(src, src_offset=10, count=20)
```

---

#### `pointer() -> int`
Get pointer to the table handle (for advanced use).

**Returns:**
- Integer pointer value, or 0 if not bound

---

### Python Operators

The Table class supports Python sequence operations:

#### `__len__() -> int`
Get table size using Python's `len()` function.

```python
size = len(t)
```

---

#### `__getitem__(index: int) -> int`
Get value using Python array syntax.

```python
value = t[5]
```

---

#### `__setitem__(index: int, value: int) -> None`
Set value using Python array syntax.

```python
t[5] = 100
```

---

#### `__repr__() -> str`
String representation of the table.

```python
print(t)  # "Table(name='mytable', size=100)"
```

---

## Complete Example

```python
import api

# Create and bind table
t = api.Table()
if not t.bind("lookup"):
    api.error("Table 'lookup' not found")
    return

# Get table info
api.post(f"Bound to: {t.name()}")
api.post(f"Size: {len(t)}")

# Fill with zeros
t.fill(0)

# Set individual values
t[0] = 100
t[1] = 200
t[2] = 300

# Use Python array syntax
for i in range(10):
    t[i] = i * 10

# Read values
values = [t[i] for i in range(10)]
api.post(f"Values: {values}")

# Bulk operations
t.from_list(list(range(50)))
all_values = t.to_list()

# Generate waveform
import math
for i in range(len(t)):
    phase = (i / len(t)) * 2 * math.pi
    value = int((math.sin(phase) * 500) + 500)
    t[i] = value
```

## Notes

- Tables must exist in Max before binding (create with `[table name size]`)
- Tables store only integer values (longs)
- Values are automatically clamped/converted from floats
- Tables are global - all patches see the same named table
- Always call `refresh()` if the table might have been recreated
- Setting values automatically marks the table as dirty
- Table data is owned by Max, not by the Python wrapper

## See Also

- Max `[table]` object documentation
- `api.Buffer` for audio-rate float arrays
- `api.AtomArray` for mixed-type arrays
