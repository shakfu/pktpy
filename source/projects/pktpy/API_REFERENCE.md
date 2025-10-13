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
- `from_parse(string)`: Class method - parse string into AtomArray

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

#### Parsing Strings

```python
# Parse string into AtomArray (module-level function)
arr = api.parse("hello world 42 3.14")
print(len(arr))          # 4
print(arr[0])            # Atom('hello')
print(arr[1])            # Atom('world')
print(int(arr[2]))       # 42
print(float(arr[3]))     # 3.14

# Using class method
arr = api.AtomArray.from_parse("bang 100")
print(len(arr))          # 2
print(str(arr[0]))       # "bang"
print(int(arr[1]))       # 100

# Parse empty string
arr = api.parse("")
print(len(arr))          # 0

# Parse numbers
arr = api.parse("1 2 3 4.5 6.7")
for i in range(len(arr)):
    print(f"{i}: {arr[i].type} = {arr[i].value}")
# Output:
#   0: long = 1
#   1: long = 2
#   2: long = 3
#   3: float = 4.5
#   4: float = 6.7

# Parse symbols
arr = api.parse("metro sine list")
py_list = arr.to_list()
print(py_list)          # ['metro', 'sine', 'list']

# Modify parsed atoms
arr = api.parse("10 20 30")
arr[0] = 100
arr.append(40)
print(arr.to_list())    # [100, 20, 30, 40]
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

### api.Object

Wrapper for Max's `t_object*` type. Provides generic access to any Max object, allowing you to wrap existing objects, create new ones, access attributes, and call methods.

#### Constructor

```python
Object()     # Create empty wrapper (null object)
```

#### Methods

**Object Management:**
- `create(classname, *args)`: Create new Max object (takes ownership)
- `wrap(pointer)`: Wrap existing object pointer (no ownership)
- `free()`: Manually free owned object
- `is_null()`: Returns True if object pointer is null

**Object Information:**
- `classname()`: Returns class name as string
- `pointer()`: Returns raw pointer value as integer

**Attribute Access:**
- `getattr(name)`: Get attribute value (returns single value or list)
- `setattr(name, value)`: Set attribute value
- `attrnames()`: Returns list of all attribute names

**Method Calling:**
- `method(name, *args)`: Call object method with optional arguments

#### Ownership Rules

1. **Created objects** (via `create()`) → Owned, will be freed
2. **Wrapped objects** (via `wrap()`) → Not owned, won't be freed
3. **Destructor** → Automatically frees owned objects
4. **Manual free** → Use `free()` to free before destructor

#### Examples

**Wrapping Existing Objects:**

```python
import api

# Wrap an existing Max object (pointer from Max context)
obj = api.Object()
obj.wrap(max_object_pointer)

# Check if valid
if not obj.is_null():
    name = obj.classname()
    api.post(f"Wrapped {name} object\n")
```

**Attribute Access:**

```python
# Get object attributes
obj = api.Object()
obj.wrap(some_buffer_object)

# Single attribute
size = obj.getattr("size")
api.post(f"Buffer size: {size}\n")

# Set attribute
obj.setattr("size", 2048)

# List all attributes
attrs = obj.attrnames()
for attr in attrs:
    try:
        value = obj.getattr(attr)
        api.post(f"{attr}: {value}\n")
    except:
        pass
```

**Calling Methods:**

```python
# Call methods on Max objects
obj = api.Object()
obj.wrap(some_max_object)

# No arguments
obj.method("clear")

# With arguments
result = obj.method("getvalue", 100)

# With multiple arguments
obj.method("set", 10, 20, 30)
```

**Creating New Objects:**

```python
# Create new Max object (requires registered class)
obj = api.Object()

try:
    # Create with classname and arguments
    obj.create("buffer~", "mybuffer", 1000)
    api.post(f"Created: {obj.classname()}\n")
except Exception as e:
    api.error(f"Failed to create: {e}\n")
```

**Ownership Example:**

```python
# Owned object (will be freed)
obj1 = api.Object()
obj1.create("someclass")  # obj1 owns this

# Wrapped object (won't be freed)
obj2 = api.Object()
obj2.wrap(external_pointer)  # obj2 doesn't own this

# Manual cleanup
obj1.free()  # Explicitly free
# obj1 is now null

# obj2 won't be freed when it goes out of scope
```

**Pointer Management:**

```python
# Get pointer value
obj = api.Object()
obj.wrap(some_object)

ptr = obj.pointer()
api.post(f"Object pointer: 0x{ptr:x}\n")

# Pass pointer to another wrapper
obj2 = api.Object()
obj2.wrap(ptr)
```

**Working with Buffer Objects:**

```python
# Example: Working with buffer~ objects
# (Requires actual buffer~ object from Max context)

buffer_obj = api.Object()
buffer_obj.wrap(buffer_pointer)

# Get buffer properties
size_ms = buffer_obj.getattr("size_ms")
channels = buffer_obj.getattr("channels")

api.post(f"Buffer: {size_ms}ms, {channels} channels\n")

# Modify buffer
buffer_obj.setattr("size_ms", 500.0)
buffer_obj.method("clear")
```

**Patcher Object Example:**

```python
# Example: Accessing patcher objects
# (Requires patcher object from Max context)

patcher = api.Object()
patcher.wrap(patcher_pointer)

# Get patcher info
name = patcher.getattr("name")
api.post(f"Patcher: {name}\n")

# Query patcher
box_count = patcher.method("count")
api.post(f"Number of objects: {box_count}\n")
```

#### Error Handling

```python
obj = api.Object()

try:
    # Attempt to create object
    obj.create("unknown_class")
except RuntimeError as e:
    api.error(f"Creation failed: {e}\n")

try:
    # Attempt to access attribute
    value = obj.getattr("nonexistent_attr")
except RuntimeError as e:
    api.error(f"Attribute access failed: {e}\n")
```

#### Use Cases

1. **Dynamic Object Creation**: Create Max objects at runtime
2. **Object Introspection**: Query object properties and capabilities
3. **Attribute Manipulation**: Get/set object attributes programmatically
4. **Method Invocation**: Call object methods from Python
5. **Integration**: Bridge between Python scripts and Max objects

#### Limitations

- Objects must be registered with Max to use `create()`
- Method calling uses `object_method_typed()` (may not work for all methods)
- Attribute access requires proper Max attribute implementation
- Pointer management requires understanding of Max object lifetimes

---

### api.Hashtab

Wrapper for Max's `t_hashtab*` type. Hashtables provide fast key-value storage using symbol keys.

#### Constructor

```python
Hashtab()     # Create new empty hashtab
```

#### Magic Methods

- `__len__()`: Returns number of entries (enables `len(h)`)
- `__contains__(key)`: Check if key exists (enables `key in h`)
- `__getitem__(key)`: Get value by key (enables `h[key]`)
- `__setitem__(key, value)`: Set value by key (enables `h[key] = value`)
- `__repr__()`: Returns string representation like `Hashtab(entries=N)`

#### Methods

**Object Management:**
- `wrap(pointer)`: Wrap existing hashtab pointer (no ownership)
- `is_null()`: Returns True if hashtab pointer is null
- `pointer()`: Returns raw pointer value as integer

**Key-Value Operations:**
- `store(key, value)`: Store value (int, float, or string) with key
- `lookup(key)`: Lookup value by key (returns value or None)
- `delete(key)`: Remove entry by key
- `clear()`: Remove all entries

**Query:**
- `keys()`: Returns list of all keys (as strings)
- `has_key(key)`: Returns True if key exists
- `getsize()`: Returns number of entries

#### Supported Value Types

Hashtab values can be:
- `int` → stored as long
- `float` → stored as float
- `str` → stored as symbol

#### Examples

**Basic Usage:**

```python
import api

# Create hashtab
h = api.Hashtab()

# Store values
h["count"] = 10
h["ratio"] = 0.5
h["name"] = "test"

# Access values
print(h["count"])       # 10
print(h["ratio"])       # 0.5
print(h["name"])        # "test"

# Check membership
print("count" in h)     # True
print("missing" in h)   # False

# Get size
print(len(h))           # 3
```

**Using Methods:**

```python
h = api.Hashtab()

# Store using method
h.store("value", 42)
h.store("pi", 3.14159)
h.store("label", "demo")

# Lookup using method
val = h.lookup("value")     # 42
pi = h.lookup("pi")          # 3.14159
missing = h.lookup("missing") # None

# Get all keys
all_keys = h.keys()
print(all_keys)  # ['value', 'pi', 'label']

# Iterate
for key in h.keys():
    print(f"{key}: {h[key]}")
```

**Deletion:**

```python
h = api.Hashtab()
h["a"] = 1
h["b"] = 2
h["c"] = 3

# Delete single entry
h.delete("b")
print(len(h))  # 2

# Clear all
h.clear()
print(len(h))  # 0
```

**Wrapping Existing Hashtabs:**

```python
# Wrap hashtab from Max context
h = api.Hashtab()
h.wrap(hashtab_pointer)

if not h.is_null():
    # Access existing hashtab data
    for key in h.keys():
        value = h[key]
        api.post(f"{key} = {value}\n")
```

#### Ownership Notes

- New hashtabs (via constructor) are owned and will be freed
- Wrapped hashtabs (via `wrap()`) are not owned

---

### api.Linklist

Wrapper for Max's `t_linklist*` type. Linklists are doubly-linked lists of object pointers.

#### Constructor

```python
Linklist()     # Create new empty linklist
```

#### Magic Methods

- `__len__()`: Returns number of items (enables `len(lst)`)
- `__getitem__(index)`: Get pointer at index (enables `lst[i]`)
- `__repr__()`: Returns string representation like `Linklist(size=N)`

#### Methods

**Object Management:**
- `wrap(pointer)`: Wrap existing linklist pointer (no ownership)
- `is_null()`: Returns True if linklist pointer is null
- `pointer()`: Returns raw pointer value as integer

**Adding Items:**
- `append(pointer)`: Append object pointer to end
- `insertindex(index, pointer)`: Insert pointer at index

**Accessing Items:**
- `getindex(index)`: Get pointer at index (returns integer)
- `getsize()`: Returns number of items

**Removing Items:**
- `chuckindex(index)`: Remove item at index
- `deleteindex(index)`: Delete item at index (same as chuckindex)
- `clear()`: Remove all items

**Reordering:**
- `reverse()`: Reverse order of items
- `rotate(n)`: Rotate items by n positions
- `shuffle()`: Randomly shuffle items
- `swap(i, j)`: Swap items at indices i and j

#### Examples

**Basic Usage:**

```python
import api

# Create linklist
lst = api.Linklist()

# Add some object pointers
obj1_ptr = 0x1000  # Example pointer values
obj2_ptr = 0x2000
obj3_ptr = 0x3000

lst.append(obj1_ptr)
lst.append(obj2_ptr)
lst.append(obj3_ptr)

print(len(lst))  # 3
```

**Accessing Items:**

```python
# Get items by index
first = lst[0]
second = lst[1]
last = lst[-1]  # Negative indexing

# Or use method
ptr = lst.getindex(0)

# Iterate
for i in range(len(lst)):
    ptr = lst[i]
    api.post(f"Item {i}: 0x{ptr:x}\n")
```

**Inserting and Removing:**

```python
lst = api.Linklist()
lst.append(0x1000)
lst.append(0x2000)
lst.append(0x3000)

# Insert at position
lst.insertindex(1, 0x1500)  # Insert between first and second

# Remove by index
lst.chuckindex(2)  # Remove third item

# Clear all
lst.clear()
print(len(lst))  # 0
```

**Reordering:**

```python
lst = api.Linklist()
for i in range(5):
    lst.append(i * 0x1000)

# Reverse
lst.reverse()
print(lst[0])  # Last item now first

# Rotate
lst.rotate(2)  # Rotate 2 positions

# Shuffle
lst.shuffle()  # Random order

# Swap
lst.swap(0, 4)  # Swap first and last
```

**Working with Max Objects:**

```python
# Linklist stores object pointers
lst = api.Linklist()

# Add object pointers from Max context
lst.append(box1_pointer)
lst.append(box2_pointer)

# Wrap pointers with Object wrapper
for i in range(len(lst)):
    ptr = lst[i]
    obj = api.Object()
    obj.wrap(ptr)

    if not obj.is_null():
        name = obj.classname()
        api.post(f"Object {i}: {name}\n")
```

**Wrapping Existing Linklists:**

```python
# Wrap linklist from Max (e.g., patcher box list)
lst = api.Linklist()
lst.wrap(linklist_pointer)

if not lst.is_null():
    api.post(f"List has {len(lst)} items\n")

    for i in range(len(lst)):
        ptr = lst[i]
        api.post(f"  [{i}] 0x{ptr:x}\n")
```

#### Ownership Notes

- New linklists (via constructor) are owned and will be freed
- Wrapped linklists (via `wrap()`) are not owned
- Linklist does NOT own the objects it points to
- Only stores pointers, doesn't manage object lifetimes

---

### api.Patcher

Wrapper for Max's `t_object*` (patcher) type. Provides access to patcher objects for querying and manipulating patch structure.

#### Constructor

```python
Patcher()     # Create empty wrapper (null patcher)
```

#### Methods

**Object Management:**
- `wrap(pointer)`: Wrap existing patcher pointer (no ownership)
- `is_null()`: Returns True if patcher pointer is null
- `pointer()`: Returns raw pointer value as integer

**Navigation:**
- `get_firstobject()`: Get pointer to first box in patcher
- `get_lastobject()`: Get pointer to last box in patcher
- `get_parentpatcher()`: Get pointer to parent patcher
- `get_toppatcher()`: Get pointer to top-level patcher

**Object Management:**
- `newobject(classname)`: Create new object in patcher (returns box pointer)
- `deleteobj(box_pointer)`: Delete object from patcher
- `count()`: Returns number of objects in patcher

**Properties:**
- `get_title()`: Get patcher window title
- `set_title(title)`: Set patcher window title
- `get_rect()`: Get patcher window rect as [x, y, width, height]
- `set_rect(x, y, w, h)`: Set patcher window rect

**State:**
- `set_locked(locked)`: Lock (True) or unlock (False) patcher
- `set_dirty(dirty)`: Mark patcher as modified (True) or clean (False)

#### Examples

**Basic Usage:**

```python
import api

# Wrap patcher from Max context
p = api.Patcher()
p.wrap(patcher_pointer)

if not p.is_null():
    # Get patcher info
    title = p.get_title()
    count = p.count()
    api.post(f"Patcher '{title}' has {count} objects\n")
```

**Navigating Objects:**

```python
p = api.Patcher()
p.wrap(patcher_pointer)

# Get first box
box_ptr = p.get_firstobject()
if box_ptr != 0:
    box = api.Box()
    box.wrap(box_ptr)
    name = box.classname()
    api.post(f"First object: {name}\n")

# Count objects
total = p.count()
api.post(f"Total objects: {total}\n")
```

**Creating and Deleting Objects:**

```python
p = api.Patcher()
p.wrap(patcher_pointer)

# Create new object
box_ptr = p.newobject("gate")
if box_ptr != 0:
    api.post("Created gate object\n")

    # Delete it
    p.deleteobj(box_ptr)
    api.post("Deleted gate object\n")
```

**Patcher Hierarchy:**

```python
p = api.Patcher()
p.wrap(patcher_pointer)

# Get parent patcher
parent_ptr = p.get_parentpatcher()
if parent_ptr != 0:
    parent = api.Patcher()
    parent.wrap(parent_ptr)
    title = parent.get_title()
    api.post(f"Parent: {title}\n")

# Get top patcher
top_ptr = p.get_toppatcher()
if top_ptr != 0:
    top = api.Patcher()
    top.wrap(top_ptr)
    title = top.get_title()
    api.post(f"Top: {title}\n")
```

**Window Management:**

```python
p = api.Patcher()
p.wrap(patcher_pointer)

# Get window rect
rect = p.get_rect()  # [x, y, width, height]
api.post(f"Position: ({rect[0]}, {rect[1]})\n")
api.post(f"Size: {rect[2]} x {rect[3]}\n")

# Set window position and size
p.set_rect(100, 100, 800, 600)

# Get/set title
old_title = p.get_title()
p.set_title("My Patcher")
api.post(f"Renamed '{old_title}' to 'My Patcher'\n")
```

**Lock and Dirty State:**

```python
p = api.Patcher()
p.wrap(patcher_pointer)

# Lock patcher
p.set_locked(True)
api.post("Patcher locked\n")

# Unlock patcher
p.set_locked(False)
api.post("Patcher unlocked\n")

# Mark as modified
p.set_dirty(True)
```

#### Use Cases

1. **Patcher Introspection**: Query patcher structure and contents
2. **Dynamic Object Creation**: Create objects programmatically
3. **Window Management**: Control patcher window size and position
4. **Navigation**: Traverse patcher hierarchy
5. **Automation**: Script patcher modifications

---

### api.Box

Wrapper for Max's `t_object*` (box) type. Represents UI boxes in a patcher.

#### Constructor

```python
Box()     # Create empty wrapper (null box)
```

#### Methods

**Object Management:**
- `wrap(pointer)`: Wrap existing box pointer (no ownership)
- `is_null()`: Returns True if box pointer is null
- `pointer()`: Returns raw pointer value as integer

**Box Information:**
- `classname()`: Returns box class name as string
- `get_object()`: Get underlying t_object pointer (as integer)

**Position and Size:**
- `get_rect()`: Get box rect as [x, y, width, height]
- `set_rect(x, y, w, h)`: Set box position and size

#### Examples

**Basic Usage:**

```python
import api

# Wrap box from Max context
b = api.Box()
b.wrap(box_pointer)

if not b.is_null():
    # Get box info
    name = b.classname()
    api.post(f"Box class: {name}\n")
```

**Getting Object:**

```python
b = api.Box()
b.wrap(box_pointer)

# Get underlying object
obj_ptr = b.get_object()
if obj_ptr != 0:
    obj = api.Object()
    obj.wrap(obj_ptr)

    # Access object attributes
    attrs = obj.attrnames()
    api.post(f"Object has {len(attrs)} attributes\n")
```

**Position and Size:**

```python
b = api.Box()
b.wrap(box_pointer)

# Get current position
rect = b.get_rect()  # [x, y, width, height]
api.post(f"Box at ({rect[0]}, {rect[1]})\n")
api.post(f"Size: {rect[2]} x {rect[3]}\n")

# Move box
new_x = rect[0] + 50
new_y = rect[1] + 50
b.set_rect(new_x, new_y, rect[2], rect[3])

# Resize box
b.set_rect(rect[0], rect[1], 200, 100)
```

**Iterating Patcher Boxes:**

```python
# Get all boxes in patcher
p = api.Patcher()
p.wrap(patcher_pointer)

box_ptr = p.get_firstobject()
while box_ptr != 0:
    box = api.Box()
    box.wrap(box_ptr)

    if not box.is_null():
        name = box.classname()
        rect = box.get_rect()
        api.post(f"{name} at ({rect[0]}, {rect[1]})\n")

    # Note: To iterate properly, you'd need next_object()
    # which isn't currently wrapped
    break
```

**Working with Object:**

```python
# Box wrapper gives access to the visual box
b = api.Box()
b.wrap(box_pointer)

# Object wrapper gives access to the underlying Max object
obj_ptr = b.get_object()
obj = api.Object()
obj.wrap(obj_ptr)

# Access box properties
box_name = b.classname()
rect = b.get_rect()

# Access object properties
attrs = obj.attrnames()
for attr in attrs:
    try:
        value = obj.getattr(attr)
        api.post(f"  {attr}: {value}\n")
    except:
        pass
```

#### Use Cases

1. **Box Introspection**: Query box class and properties
2. **Layout Management**: Position and size boxes programmatically
3. **Visual Scripting**: Manipulate patch appearance
4. **Object Access**: Bridge between visual box and underlying object

#### Relationship to Object

- **Box** represents the visual UI element in the patcher
- **Object** represents the underlying Max object
- Use `box.get_object()` to get the object pointer from a box
- Use object wrapper for attributes/methods, box wrapper for position/size

---

## Parsing Functions

### api.parse(str)

Parse a string into an AtomArray. The string is parsed according to Max's atom syntax rules, automatically determining types (long, float, or symbol).

**Parameters:**
- `str`: String to parse

**Returns:** AtomArray containing the parsed atoms

**Type Detection Rules:**
- Integer numbers → long atoms (e.g., "42")
- Floating-point numbers → float atoms (e.g., "3.14")
- Everything else → symbol atoms (e.g., "hello")

```python
import api

# Parse mixed values
arr = api.parse("hello 42 3.14 world")
print(len(arr))              # 4
print(str(arr[0]))           # "hello" (symbol)
print(int(arr[1]))           # 42 (long)
print(float(arr[2]))         # 3.14 (float)
print(str(arr[3]))           # "world" (symbol)

# Parse numbers only
arr = api.parse("1 2 3 4 5")
total = sum(int(arr[i]) for i in range(len(arr)))
print(total)                 # 15

# Parse symbols only
arr = api.parse("bang float list")
for i in range(len(arr)):
    api.post(f"{str(arr[i])}\n")

# Empty string
arr = api.parse("")
print(len(arr))              # 0

# Single value
arr = api.parse("42")
print(len(arr))              # 1
print(int(arr[0]))           # 42
```

**Use Cases:**
- Converting user input strings to atoms
- Parsing message content
- Quick atom array creation
- Command parsing

**Note:** This function uses Max's `atom_setparse()` C function internally, which follows Max's standard parsing rules.

---

## Object Registration and Notification Functions

Max provides a registration system for objects to communicate with each other. Objects can register themselves in namespaces, and other objects can attach to them to receive notifications.

### api.object_register(namespace, name, pointer)

Register an object in a namespace.

**Parameters:**
- `namespace` (str): Namespace for registration (e.g., "global", "box", "nobox", or custom)
- `name` (str): Name of object in namespace
- `pointer` (int): Pointer to object to register

**Returns:** int - Pointer to registered object (may be different from input)

**Important:** The returned pointer may be different from the input pointer, as Max may duplicate the object. Always use the returned pointer for unregistration.

```python
import api

# Get object pointer (from Object wrapper or elsewhere)
obj = api.Object()
# ... wrap or create object ...
ptr = obj.pointer()

# Register in custom namespace
reg_ptr = api.object_register("myspace", "myobject", ptr)

# Store reg_ptr for later unregistration
```

---

### api.object_unregister(pointer)

Unregister an object from its namespace.

**Parameters:**
- `pointer` (int): Pointer returned from `object_register()`

**Returns:** None

```python
# Unregister using pointer from registration
api.object_unregister(reg_ptr)
```

---

### api.object_findregistered(namespace, name)

Find a registered object by its namespace and name.

**Parameters:**
- `namespace` (str): Namespace to search
- `name` (str): Name of object in namespace

**Returns:** int - Pointer to registered object, or None if not found

```python
# Find registered object
ptr = api.object_findregistered("myspace", "myobject")
if ptr:
    # Wrap and use it
    obj = api.Object()
    obj.wrap(ptr)
    api.post(f"Found: {obj.classname()}\n")
```

---

### api.object_findregisteredbyptr(pointer)

Find the namespace and name of a registered object given its pointer.

**Parameters:**
- `pointer` (int): Pointer to registered object

**Returns:** list - `[namespace, name]` or None if not registered

```python
# Get registration info from pointer
info = api.object_findregisteredbyptr(ptr)
if info:
    namespace, name = info
    api.post(f"Registered as {namespace}::{name}\n")
```

---

### api.object_attach(namespace, name, client_pointer)

Attach a client object to a registered object to receive notifications.

**Parameters:**
- `namespace` (str): Namespace of registered object
- `name` (str): Name of registered object
- `client_pointer` (int): Pointer to client object (the one attaching)

**Returns:** int - Pointer to registered object, or None if not found

```python
# Client attaches to registered object
client_ptr = my_client_obj.pointer()
registered = api.object_attach("myspace", "myobject", client_ptr)

if registered:
    api.post("Successfully attached\n")
```

---

### api.object_detach(namespace, name, client_pointer)

Detach a client object from a registered object.

**Parameters:**
- `namespace` (str): Namespace of registered object
- `name` (str): Name of registered object
- `client_pointer` (int): Pointer to client object to detach

**Returns:** None

```python
# Detach client from registered object
api.object_detach("myspace", "myobject", client_ptr)
```

---

### api.object_attach_byptr(client_pointer, registered_pointer)

Attach to a registered object using direct pointers.

**Parameters:**
- `client_pointer` (int): Pointer to client object
- `registered_pointer` (int): Pointer to registered object

**Returns:** None

```python
# Attach using pointers directly
api.object_attach_byptr(client_ptr, registered_ptr)
```

---

### api.object_detach_byptr(client_pointer, registered_pointer)

Detach from a registered object using direct pointers.

**Parameters:**
- `client_pointer` (int): Pointer to client object
- `registered_pointer` (int): Pointer to registered object

**Returns:** None

```python
# Detach using pointers directly
api.object_detach_byptr(client_ptr, registered_ptr)
```

---

### api.object_notify(object_pointer, message, data_pointer)

Send a notification from a registered object to all attached clients.

**Parameters:**
- `object_pointer` (int): Pointer to object sending notification
- `message` (str): Message/event name (e.g., "modified", "changed", "deleted")
- `data_pointer` (int): Optional pointer to data (use 0 for none)

**Returns:** None

```python
# Notify all attached clients
api.object_notify(reg_ptr, "modified", 0)

# Notify with data pointer
api.object_notify(reg_ptr, "changed", data_ptr)
```

---

### Complete Example

```python
import api

# === Provider Object ===
# Create and register a provider object
provider = api.Object()
# ... create or wrap provider object ...
provider_ptr = provider.pointer()

# Register it
reg_ptr = api.object_register("myapp", "datasource", provider_ptr)
api.post("Provider registered\n")

# === Client Objects ===
# Create client objects that want to listen
client1 = api.Object()
client2 = api.Object()
# ... create or wrap client objects ...

# Clients attach to provider
api.object_attach("myapp", "datasource", client1.pointer())
api.object_attach("myapp", "datasource", client2.pointer())
api.post("Clients attached\n")

# === Notification ===
# Provider notifies all clients of changes
api.object_notify(reg_ptr, "data_changed", 0)
api.post("Notification sent to all clients\n")

# === Cleanup ===
# Clients detach
api.object_detach("myapp", "datasource", client1.pointer())
api.object_detach("myapp", "datasource", client2.pointer())

# Unregister provider
api.object_unregister(reg_ptr)
api.post("Provider unregistered\n")
```

---

### Use Cases

**1. Shared Resources**
```python
# Register a shared buffer
buffer_ptr = api.object_register("buffers", "shared_audio", buf_ptr)

# Multiple processors can find and use it
processor1_accesses = api.object_findregistered("buffers", "shared_audio")
processor2_accesses = api.object_findregistered("buffers", "shared_audio")
```

**2. Event Broadcasting**
```python
# Register event broadcaster
broadcaster_ptr = api.object_register("events", "transport", obj_ptr)

# Listeners attach
api.object_attach("events", "transport", listener1_ptr)
api.object_attach("events", "transport", listener2_ptr)

# Broadcast events
api.object_notify(broadcaster_ptr, "play", 0)
api.object_notify(broadcaster_ptr, "stop", 0)
```

**3. State Synchronization**
```python
# Register state holder
state_ptr = api.object_register("state", "project", state_obj_ptr)

# UI elements attach to stay in sync
api.object_attach("state", "project", ui_slider_ptr)
api.object_attach("state", "project", ui_display_ptr)

# Notify on state changes
api.object_notify(state_ptr, "value_changed", value_ptr)
```

---

### Important Notes

**Pointer Lifetime:**
- Ensure objects exist before registering
- Unregister before freeing objects
- Don't use invalid pointers

**Return Value:**
- `object_register()` may return a different pointer than provided
- Always use the returned pointer for unregistration

**UI Objects:**
- Don't manually register UI objects (they auto-register)
- Don't attach UI objects to themselves

**Namespaces:**
- Common: "global", "box", "nobox"
- Use custom namespaces to avoid conflicts
- Choose meaningful names

**Notification:**
- Clients must implement a `notify` method to receive notifications
- Not all objects support notification
- Message names are arbitrary strings

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
