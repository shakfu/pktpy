# pktpy Quick Reference

## Import

```python
import api
```

## Console Output

```python
api.post("message")           # Print to console
api.error("error message")    # Print error (red)
```

## Symbols

```python
# Create
s = api.Symbol("name")
s = api.gensym("name")

# Properties
s.name                          # Get string value

# Compare
s1 == s2                        # Compare symbols
s == "string"                   # Compare with string
```

## Atoms

```python
# Create
a = api.Atom()                  # long(0)
a = api.Atom(42)                # long
a = api.Atom(3.14)              # float
a = api.Atom("text")            # symbol
a = api.Atom(sym)               # symbol

# Properties
a.type                          # "long", "float", "symbol"
a.value                         # Get value
a.value = 100                   # Set value (auto-converts)

# Type checks
a.is_long()
a.is_float()
a.is_symbol()

# Pythonic conversions (recommended)
int(a)                          # Convert to int (__int__)
float(a)                        # Convert to float (__float__)
str(a)                          # Convert to string (__str__)

# Explicit Max conversion methods
a.getlong()                     # Get as long (truncates float)
a.getfloat()                    # Get as float (converts long)
a.getsym()                      # Get as Symbol object

# Module-level conversions
api.atom_getlong(a)
api.atom_getfloat(a)
api.atom_getsym(a)

# Representation
repr(a)                         # Atom(value) format
```

## AtomArrays

```python
# Create
arr = api.AtomArray()               # Empty
arr = api.AtomArray([1, 2, 3])      # From list

# Access
arr[0]                              # Get Atom at index
arr[-1]                             # Negative indexing
len(arr)                            # Length

# Modify
arr[0] = 100                        # Set value
arr.append(42)                      # Append
arr.clear()                         # Clear all

# Convert
py_list = arr.to_list()             # To Python list
arr2 = arr.duplicate()              # Copy

# Iterate
for i in range(len(arr)):
    print(arr[i])
```

## Common Patterns

### Message Router

```python
msg_type = api.Symbol(message)

if msg_type == api.Symbol("bang"):
    handle_bang()
elif msg_type == api.Symbol("float"):
    handle_float(value)
```

### Atom Processor

```python
def process(atom):
    if atom.is_long():
        return atom.value * 2
    elif atom.is_float():
        return atom.value + 0.5
    elif atom.is_symbol():
        return f"Symbol: {atom.value}"
```

### Type Conversion

```python
# Pythonic conversions (recommended)
a = api.Atom(3.7)
i = int(a)                      # 3 (truncated)
f = float(a)                    # 3.7
s = str(a)                      # "3.7"

# Use in expressions
result = int(api.Atom(10)) * 2  # 20
total = float(api.Atom(5)) + 1.5  # 6.5

# Symbol extraction
a = api.Atom("test")
sym = a.getsym()                # Symbol object
print(sym.name)                 # "test"
print(str(a))                   # "test" (string)
```

### Working with Lists

```python
atoms = [api.Atom(x) for x in [1, 2, 3]]

for a in atoms:
    api.post(f"{a.type}: {a.value}\n")
```

## Full Example

```python
import api

def main():
    # Create symbol
    sym = api.gensym("my_message")
    api.post(f"Created: {sym}\n")

    # Create atoms
    atoms = [
        api.Atom(42),
        api.Atom(3.14),
        api.Atom("test")
    ]

    # Process
    for i, atom in enumerate(atoms):
        api.post(f"[{i}] {atom} - type: {atom.type}\n")

    # Convert types
    atoms[0].value = 100.5  # Now float
    api.post(f"Converted: {atoms[0]}\n")

if __name__ == "__main__":
    main()
```
