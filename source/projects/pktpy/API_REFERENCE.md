# pktpy API Reference

This document describes the Python API available within the `pktpy` Max external through the `api` module.

## Core Types

### api.Symbol

Wrapper for Max's `t_symbol` type. Symbols are interned strings used throughout Max for efficient string handling.

#### Constructor

```python
Symbol()          # Create empty symbol
Symbol(str)       # Create symbol from string
```

#### Properties

- `name` (read-only): Returns the symbol's string value

#### Methods

- `__str__()`: Returns the symbol's string value
- `__repr__()`: Returns string representation like `Symbol('name')`
- `__eq__(other)`: Compare with another Symbol or string

#### Examples

```python
import api

# Create symbols
s1 = api.Symbol("hello")
s2 = api.Symbol("world")

# Access name
print(s1.name)  # "hello"

# Compare symbols
s3 = api.Symbol("hello")
print(s1 == s3)        # True (same interned symbol)
print(s1 == "hello")   # True (compare with string)
print(s1 == s2)        # False
```

---

### api.Atom

Wrapper for Max's `t_atom` type. Atoms are tagged unions that can hold int (long), float, or symbol values.

#### Constructor

```python
Atom()           # Create atom with long value 0
Atom(int)        # Create atom with long value
Atom(float)      # Create atom with float value
Atom(str)        # Create atom with symbol value
Atom(Symbol)     # Create atom with symbol value
```

#### Properties

- `type` (read-only): Returns `"long"`, `"float"`, or `"symbol"`
- `value` (read/write): Get or set the atom's value (type changes automatically)

#### Methods

- `is_long()`: Returns True if atom contains a long
- `is_float()`: Returns True if atom contains a float
- `is_symbol()`: Returns True if atom contains a symbol
- `getlong()`: Get value as long (truncates floats, returns 0 for symbols)
- `getfloat()`: Get value as float (converts longs, returns 0.0 for symbols)
- `getsym()`: Get value as Symbol (returns Symbol object)
- `__str__()`: String conversion via `atom_getsym()->s_name`
- `__int__()`: Integer conversion via `atom_getlong()` (enables `int(atom)`)
- `__float__()`: Float conversion via `atom_getfloat()` (enables `float(atom)`)
- `__repr__()`: Returns string representation like `Atom(value)`

#### Examples

```python
import api

# Create atoms
a1 = api.Atom(42)
a2 = api.Atom(3.14)
a3 = api.Atom("test")

# Check types
print(a1.type)        # "long"
print(a1.is_long())   # True
print(a2.is_float())  # True
print(a3.is_symbol()) # True

# Get values
print(a1.value)  # 42
print(a2.value)  # 3.14
print(a3.value)  # "test"

# Modify values (type changes automatically)
a1.value = 2.5
print(a1.type)   # "float"
print(a1.value)  # 2.5

a1.value = "changed"
print(a1.type)   # "symbol"
print(a1.value)  # "changed"

# Conversion methods (Max atom_get* functions)
a = api.Atom(3.7)
print(a.getlong())   # 3 (truncated)
print(a.getfloat())  # 3.7

a = api.Atom("test")
sym = a.getsym()     # Returns Symbol object
print(sym.name)      # "test"

# Pythonic magic methods (preferred)
a = api.Atom(3.7)
print(int(a))        # 3 (calls __int__)
print(float(a))      # 3.7 (calls __float__)
print(str(a))        # "3.7" (calls __str__)

a = api.Atom("hello")
print(str(a))        # "hello"
s = a.getsym()       # Symbol('hello')

# Use in expressions
result = int(api.Atom(10)) * 2        # 20
total = float(api.Atom(3.5)) + 1.5    # 5.0
msg = "Value: " + str(api.Atom(42))   # "Value: 42"
```

#### Conversion Behavior

The conversion methods and magic methods use Max's `atom_getlong()`, `atom_getfloat()`, and `atom_getsym()` C functions:

| Python | Atom Method | C Function | Description |
|--------|-------------|------------|-------------|
| `int(a)` | `a.getlong()` | `atom_getlong()` | Extract as integer |
| `float(a)` | `a.getfloat()` | `atom_getfloat()` | Extract as float |
| `str(a)` | - | `atom_getsym()` | Get string representation |
| - | `a.getsym()` | `atom_getsym()` | Get Symbol object |

**Conversion Rules:**

- **`getlong()`**:
  - Long atoms: returns the value
  - Float atoms: truncates to integer (3.7 → 3)
  - Symbol atoms: returns 0

- **`getfloat()`**:
  - Long atoms: converts to float (42 → 42.0)
  - Float atoms: returns the value
  - Symbol atoms: returns 0.0

- **`getsym()`**:
  - Long atoms: returns Symbol("0")
  - Float atoms: returns Symbol("0.")
  - Symbol atoms: returns the Symbol object
```

---

### api.AtomArray

Wrapper for Max's `t_atomarray` type. A dynamically resizable array of atoms.

#### Constructor

```python
AtomArray()              # Create empty array
AtomArray([...])         # Create from Python list
```

#### Properties

None (use methods to access data).

#### Methods

- `__len__()`: Returns length of array (enables `len(arr)`)
- `__getitem__(index)`: Get Atom at index (enables `arr[i]`)
- `__setitem__(index, value)`: Set value at index (enables `arr[i] = val`)
- `__repr__()`: Returns string representation
- `getsize()`: Returns length of array
- `append(value)`: Append value to end of array
- `clear()`: Remove all elements
- `to_list()`: Convert to Python list
- `duplicate()`: Create a copy of the array

#### Examples

```python
import api

# Create empty array
arr = api.AtomArray()
arr.append(1)
arr.append(2)
arr.append(3)
print(len(arr))  # 3

# Create from list
arr = api.AtomArray([1, 2.5, "hello"])
print(arr[0])    # Atom(1)
print(arr[1])    # Atom(2.5)
print(arr[2])    # Atom('hello')

# Modify elements
arr[0] = 100
print(int(arr[0]))  # 100

# Negative indexing
print(arr[-1])   # Last element

# Convert to Python list
py_list = arr.to_list()
print(py_list)   # [100, 2.5, 'hello']

# Iterate
for i in range(len(arr)):
    print(f"arr[{i}] = {arr[i]}")

# Duplicate
arr2 = arr.duplicate()
arr2.append(42)  # Only affects arr2

# Clear
arr.clear()
print(len(arr))  # 0
```

#### Working with Atom Objects

```python
# Append Atom objects
a1 = api.Atom(42)
a2 = api.Atom(3.14)
arr = api.AtomArray()
arr.append(a1)
arr.append(a2)

# Get returns Atom objects
atom = arr[0]
print(int(atom))     # 42
print(float(atom))   # 42.0
```

#### Memory Management

- AtomArray owns its internal atom storage
- Automatically freed when Python object is garbage collected
- `duplicate()` creates independent copy with separate storage
- `to_list()` creates Python list (atoms converted to Python types)

---

### api.Dictionary

Wrapper for Max's `t_dictionary` type. Dictionaries store key-value pairs where keys are strings and values can be any supported type (int, float, string, Atom, AtomArray, or nested Dictionary).

#### Constructor

```python
Dictionary()     # Create empty dictionary
```

#### Magic Methods

- `__len__()`: Returns number of entries (enables `len(dict)`)
- `__getitem__(key)`: Get value by key (enables `dict[key]`)
- `__setitem__(key, value)`: Set value by key (enables `dict[key] = value`)
- `__contains__(key)`: Check if key exists (enables `key in dict`)
- `__repr__()`: Returns string representation like `Dictionary(entries=N)`

#### Methods

**Key Operations:**
- `keys()`: Returns list of all keys
- `has_key(key)`: Returns True if key exists
- `get(key, default=None)`: Get value with optional default

**Typed Getters:**
- `getlong(key, default=0)`: Get value as long integer
- `getfloat(key, default=0.0)`: Get value as float
- `getstring(key, default="")`: Get value as string

**Modification:**
- `delete(key)`: Remove entry by key
- `clear()`: Remove all entries

**File I/O:**
- `read(filename, path)`: Load dictionary from JSON file
- `write(filename, path)`: Save dictionary to JSON file

**Debug:**
- `dump(recurse=True, console=False)`: Print dictionary contents to Max console

#### Supported Value Types

Dictionary values can be:
- `int` → stored as long
- `float` → stored as float
- `str` → stored as string
- `list` → stored as AtomArray (of convertible items)
- `api.Atom` → stored as atom
- `api.Symbol` → stored as symbol
- `api.AtomArray` → stored as atomarray (dictionary takes ownership)
- `api.Dictionary` → stored as nested dictionary (parent takes ownership)

#### Examples

**Basic Usage:**

```python
import api

# Create and populate
d = api.Dictionary()
d["name"] = "oscillator"
d["frequency"] = 440.0
d["amplitude"] = 0.5
d["enabled"] = 1

# Access values
print(d["frequency"])           # 440.0
print(len(d))                   # 4
print("name" in d)              # True

# With defaults
amp = d.get("amplitude", 1.0)   # 0.5
gain = d.get("gain", 0.8)       # 0.8 (not in dict)

# Typed getters
freq = d.getfloat("frequency")  # 440.0
enabled = d.getlong("enabled")  # 1
name = d.getstring("name")      # "oscillator"
```

**Nested Dictionaries:**

```python
# Create nested structure
config = api.Dictionary()

audio = api.Dictionary()
audio["sample_rate"] = 44100
audio["buffer_size"] = 512
config["audio"] = audio

midi = api.Dictionary()
midi["channel"] = 1
midi["velocity"] = 100
config["midi"] = midi

# Access nested values
sr = config["audio"]["sample_rate"]      # 44100
channel = config["midi"]["channel"]       # 1

# Iterate keys
for key in config.keys():
    api.post(f"Section: {key}\n")
```

**Lists and Arrays:**

```python
d = api.Dictionary()

# Store Python list (becomes AtomArray internally)
d["values"] = [1, 2, 3, 4, 5]
d["mixed"] = [10, 3.14, "text"]

# Store AtomArray explicitly
arr = api.AtomArray([10, 20, 30])
d["array"] = arr.duplicate()  # Duplicate first, as dict takes ownership
```

**File Persistence:**

```python
# Create configuration
settings = api.Dictionary()
settings["theme"] = "dark"
settings["font_size"] = 12
settings["auto_save"] = 1

# Save to file (path=0 for default Max search path)
settings.write("my_settings.json", 0)

# Load from file
loaded = api.Dictionary()
loaded.read("my_settings.json", 0)

theme = loaded.getstring("theme")  # "dark"
```

**Iteration:**

```python
config = api.Dictionary()
config["a"] = 1
config["b"] = 2.5
config["c"] = "text"

# Iterate over keys
for key in config.keys():
    value = config[key]
    api.post(f"{key}: {value}\n")

# Check and delete
if config.has_key("b"):
    config.delete("b")

# Clear all
config.clear()
print(len(config))  # 0
```

**Debug Output:**

```python
d = api.Dictionary()
d["int"] = 42
d["float"] = 3.14
d["string"] = "hello"

# Print to Max console
d.dump()

# Print recursively to system console
d.dump(recurse=True, console=True)
```

#### Ownership Notes

When storing AtomArray or nested Dictionary objects, the parent dictionary takes ownership:

```python
arr = api.AtomArray([1, 2, 3])
d = api.Dictionary()

# After this, d owns arr - don't free arr separately
d["data"] = arr

# To keep using arr, duplicate it first:
arr = api.AtomArray([1, 2, 3])
d["data"] = arr.duplicate()
# Now you can still use arr
```

---

## Console Functions

### api.post(str)

Print a message to the Max console (standard output).

```python
api.post("Hello from Python!\n")
api.post(f"Value: {42}\n")
```

### api.error(str)

Print an error message to the Max console (highlighted in red).

```python
api.error("Something went wrong!\n")
```

**Note:** Remember to include `\n` at the end of messages for proper line breaks.

---

## Symbol Functions

### api.gensym(str)

Create or retrieve an interned symbol. Equivalent to Max's `gensym()` function.

```python
sym = api.gensym("my_symbol")
print(sym.name)  # "my_symbol"

# gensym always returns the same object for the same string
s1 = api.gensym("test")
s2 = api.gensym("test")
print(s1 == s2)  # True
```

---

## Atom Conversion Functions

Module-level functions that mirror Max's C API for atom type conversions.

### api.atom_getlong(atom)

Extract long value from an Atom object. Equivalent to Max's `atom_getlong()`.

```python
a = api.Atom(42)
val = api.atom_getlong(a)  # 42

a = api.Atom(3.7)
val = api.atom_getlong(a)  # 3 (truncated)
```

### api.atom_getfloat(atom)

Extract float value from an Atom object. Equivalent to Max's `atom_getfloat()`.

```python
a = api.Atom(3.14)
val = api.atom_getfloat(a)  # 3.14

a = api.Atom(42)
val = api.atom_getfloat(a)  # 42.0 (converted)
```

### api.atom_getsym(atom)

Extract Symbol from an Atom object. Equivalent to Max's `atom_getsym()`.

```python
a = api.Atom("test")
sym = api.atom_getsym(a)  # Symbol object
print(sym.name)           # "test"
```

**Note**: These module-level functions are equivalent to calling the methods on Atom objects:
- `api.atom_getlong(a)` ≡ `a.getlong()`
- `api.atom_getfloat(a)` ≡ `a.getfloat()`
- `api.atom_getsym(a)` ≡ `a.getsym()`

---

## Utility Functions (Internal)

### py_to_atom(py_val, atom_ptr)

Internal C utility to convert Python value to Max atom. Not directly accessible from Python.

### atom_to_py(atom_ptr)

Internal C utility to convert Max atom to Python value. Not directly accessible from Python.

---

## Demo Types

### api.Person

Demo class showing how to wrap custom C structures. Not for production use.

```python
p = api.Person(1, 25)
print(p.id)   # 1
print(p.age)  # 25
p.age = 30
```

---

## Usage Examples

### Basic Console Output

```python
import api

api.post("Starting calculation...\n")
result = 42 * 2
api.post(f"Result: {result}\n")
```

### Working with Symbols

```python
import api

# Create symbols for message routing
bang_sym = api.Symbol("bang")
float_sym = api.Symbol("float")

# Check message types
if received_sym == bang_sym:
    api.post("Received bang\n")
elif received_sym == float_sym:
    api.post("Received float\n")
```

### Working with Atoms

```python
import api

# Process incoming atoms
def process_atom(atom):
    if atom.is_long():
        api.post(f"Integer: {atom.value}\n")
    elif atom.is_float():
        api.post(f"Float: {atom.value:.2f}\n")
    elif atom.is_symbol():
        api.post(f"Symbol: {atom.value}\n")

# Create atom list
atoms = [
    api.Atom(10),
    api.Atom(3.14),
    api.Atom("test")
]

for a in atoms:
    process_atom(a)
```

### Type Conversion

```python
import api

# Convert between types
a = api.Atom(42)
api.post(f"Start: {a} (type: {a.type})\n")

# Change to float
a.value = float(a.value) + 0.5
api.post(f"After: {a} (type: {a.type})\n")

# Create symbol from atom value
sym = api.Symbol(str(a.value))
api.post(f"Symbol: {sym}\n")
```

---

## Implementation Notes

### Memory Management

- **Symbols**: Max symbols are globally interned and never freed. The Python `Symbol` wrapper holds only a pointer to the Max symbol table entry.
- **Atoms**: The Python `Atom` wrapper contains an actual `t_atom` struct (16 bytes on most platforms). The atom's lifetime is managed by Python's garbage collector.

### Type Safety

All wrapper types perform type checking:
- `Symbol.__init__()` requires a string argument
- `Atom.value` setter accepts int, float, or string
- Methods validate argument counts and types

### Performance

- Symbol comparisons are pointer comparisons (O(1))
- Atom type checking is a simple enum comparison
- No dynamic memory allocation for individual atoms or symbols

### Integration with pocketpy

These wrappers follow pocketpy v2's Lua-like C API patterns:
- `__new__` allocates user data with `py_newobject()`
- `__init__` initializes the user data
- Properties use `py_bindproperty()` for getter/setter
- All functions return `bool` indicating success/failure
