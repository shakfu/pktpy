# Inlet API

The `api.Inlet` class and related functions provide Python access to Max's inlet creation API, enabling dynamic creation of additional inlets for Max objects.

## Overview

Max provides several methods for creating inlets:

1. **General Purpose**: `inlet_new()` - Receive any or specific messages
2. **Typed Inlets**: `intin()` / `floatin()` - Integer/float inlets with automatic type conversion
3. **Proxy Inlets**: `proxy_new()` / `proxy_getinlet()` - Modern approach for identifying which inlet received a message

Inlets created in Python are symmetric with the existing `api.Outlet` functionality.

## Class: `api.Inlet`

Wrapper for Max inlet objects.

### Constructor

```python
inlet = api.Inlet()
```

Creates an uninitialized inlet wrapper. Use module-level functions to create actual inlets.

### Methods

#### `delete()`
Delete this inlet (if owned by the wrapper).

**Raises:**
- `RuntimeError` if inlet is null or not owned

**Example:**
```python
inlet.delete()
```

---

#### `pointer() -> int`
Get pointer to inlet object.

**Returns:**
- Integer pointer value

---

#### `get_num() -> int`
Get inlet number (for proxies and typed inlets).

**Returns:**
- Inlet number (0 for leftmost, 1+  for additional)

---

#### `is_proxy() -> bool`
Check if this is a proxy inlet.

**Returns:**
- `True` if proxy inlet, `False` otherwise

---

#### `is_null() -> bool`
Check if inlet pointer is null.

**Returns:**
- `True` if null, `False` otherwise

---

#### `__repr__() -> str`
String representation.

**Example:**
```python
print(inlet)  # "Inlet(proxy, num=2, ptr=0x...)"
```

---

## Module-Level Functions

### Creating Inlets

#### `api.inlet_new(owner_ptr: int, message_name: str = None) -> Inlet`
Create a general-purpose inlet.

**Parameters:**
- `owner_ptr`: Pointer to owner object (integer)
- `message_name`: Optional message name to receive, or `None` for any message

**Returns:**
- `Inlet` object

**Raises:**
- `RuntimeError` if creation fails

**Example:**
```python
# Create inlet that receives any message
inlet = api.inlet_new(owner_ptr, None)

# Create inlet that only receives "bang" messages
bang_inlet = api.inlet_new(owner_ptr, "bang")
```

---

#### `api.intin(owner_ptr: int, inlet_number: int) -> Inlet`
Create an integer-typed inlet.

**Parameters:**
- `owner_ptr`: Pointer to owner object
- `inlet_number`: Inlet number (1-9, where 1 is first right of leftmost)

**Returns:**
- `Inlet` object

**Raises:**
- `ValueError` if inlet_number not in range 1-9
- `RuntimeError` if creation fails

**Remarks:**
- Integers sent to inlet N trigger message `inN` (e.g., `in1`, `in2`)
- Create inlets in reverse order (highest number first) for proper visual ordering

**Example:**
```python
# Create two integer inlets
inlet2 = api.intin(owner_ptr, 2)  # Create rightmost first
inlet1 = api.intin(owner_ptr, 1)  # Then create next one
```

---

#### `api.floatin(owner_ptr: int, inlet_number: int) -> Inlet`
Create a float-typed inlet.

**Parameters:**
- `owner_ptr`: Pointer to owner object
- `inlet_number`: Inlet number (1-9)

**Returns:**
- `Inlet` object

**Raises:**
- `ValueError` if inlet_number not in range 1-9
- `RuntimeError` if creation fails

**Remarks:**
- Floats sent to inlet N trigger message `ftN` (e.g., `ft1`, `ft2`)
- Create inlets in reverse order for proper visual ordering

**Example:**
```python
# Create float inlet
inlet = api.floatin(owner_ptr, 1)
```

---

### Proxy Inlets (Modern Approach)

#### `api.proxy_new(owner_ptr: int, inlet_id: int, stuffloc_ptr: int) -> Inlet`
Create a proxy inlet.

**Parameters:**
- `owner_ptr`: Pointer to owner object
- `inlet_id`: ID number for this inlet (typically inlet number)
- `stuffloc_ptr`: Pointer to location where inlet ID will be written

**Returns:**
- `Inlet` object (proxy)

**Raises:**
- `RuntimeError` if creation fails

**Remarks:**
- Proxies are the modern approach for multiple inlets
- Use `proxy_getinlet()` in message handlers to identify which inlet received the message
- Proxies must be freed in object's destructor with `inlet.delete()`

**Example:**
```python
# Create proxy inlets (typically in __init__)
self.inlet_num = 0  # Variable to hold inlet number
inlet1 = api.proxy_new(self.ptr, 1, id(self.inlet_num))
inlet2 = api.proxy_new(self.ptr, 2, id(self.inlet_num))
```

---

#### `api.proxy_getinlet(owner_ptr: int) -> int`
Get the inlet number where current message was received.

**Parameters:**
- `owner_ptr`: Pointer to owner object

**Returns:**
- Inlet number (0 for leftmost, 1+ for additional inlets)

**Remarks:**
- Call this in message handlers to determine which inlet received the message
- Returns 0 for leftmost inlet (which doesn't use proxies)

**Example:**
```python
def handle_bang(self):
    inlet_num = api.proxy_getinlet(self.ptr)
    if inlet_num == 0:
        api.post("Bang in leftmost inlet")
    elif inlet_num == 1:
        api.post("Bang in inlet 1")
```

---

### Inlet Queries

#### `api.inlet_count(owner_ptr: int) -> int`
Get the number of inlets for an object.

**Parameters:**
- `owner_ptr`: Pointer to object

**Returns:**
- Total number of inlets

**Example:**
```python
count = api.inlet_count(self.ptr)
api.post(f"Object has {count} inlets")
```

---

#### `api.inlet_nth(owner_ptr: int, index: int) -> Inlet | None`
Get the nth inlet (0-indexed).

**Parameters:**
- `owner_ptr`: Pointer to object
- `index`: Inlet index (0-based)

**Returns:**
- `Inlet` object wrapping existing inlet, or `None` if index invalid

**Remarks:**
- Returned inlet is not owned by wrapper (don't delete it)
- Useful for inspecting existing inlets

**Example:**
```python
# Get first inlet (leftmost)
inlet0 = api.inlet_nth(self.ptr, 0)
if inlet0:
    api.post(f"First inlet: {inlet0}")
```

---

## Complete Examples

### Basic General-Purpose Inlet

```python
import api

class MyObject:
    def __init__(self, owner_ptr):
        self.ptr = owner_ptr

        # Create inlet that receives any message
        self.extra_inlet = api.inlet_new(self.ptr, None)

    def bang(self):
        api.post("Received bang")
```

### Typed Inlets (Integer and Float)

```python
import api

class Calculator:
    def __init__(self, owner_ptr):
        self.ptr = owner_ptr
        self.a = 0
        self.b = 0

        # Create inlets in reverse order for proper visual layout
        self.inlet_b = api.floatin(self.ptr, 2)  # Rightmost
        self.inlet_a = api.intin(self.ptr, 1)    # Middle
        # Leftmost inlet is automatic

    def in1(self, value):
        """Called when integer arrives in inlet 1"""
        self.a = value
        api.post(f"Set a = {value}")

    def ft2(self, value):
        """Called when float arrives in inlet 2"""
        self.b = value
        api.post(f"Set b = {value}")

    def bang(self):
        """Calculate and output result"""
        result = self.a + self.b
        api.post(f"Result: {self.a} + {self.b} = {result}")
```

### Proxy Inlets (Recommended Modern Approach)

```python
import api

class MultiInlet:
    def __init__(self, owner_ptr):
        self.ptr = owner_ptr
        self.inlet_num = 0  # Storage location for proxy

        # Create 3 additional inlets (4 total including leftmost)
        # Create in reverse order for proper visual layout
        self.inlet3 = api.proxy_new(self.ptr, 3, id(self.inlet_num))
        self.inlet2 = api.proxy_new(self.ptr, 2, id(self.inlet_num))
        self.inlet1 = api.proxy_new(self.ptr, 1, id(self.inlet_num))

    def __del__(self):
        # Clean up proxies
        if hasattr(self, 'inlet1'):
            self.inlet1.delete()
        if hasattr(self, 'inlet2'):
            self.inlet2.delete()
        if hasattr(self, 'inlet3'):
            self.inlet3.delete()

    def bang(self):
        """Handle bang from any inlet"""
        inlet = api.proxy_getinlet(self.ptr)
        api.post(f"Bang received in inlet {inlet}")

    def int_input(self, value):
        """Handle integer from any inlet"""
        inlet = api.proxy_getinlet(self.ptr)
        api.post(f"Got {value} in inlet {inlet}")

        if inlet == 0:
            self.process_left(value)
        elif inlet == 1:
            self.process_inlet1(value)
        elif inlet == 2:
            self.process_inlet2(value)
        elif inlet == 3:
            self.process_inlet3(value)
```

### Query Existing Inlets

```python
import api

def inspect_object(owner_ptr):
    """Inspect an object's inlets"""
    count = api.inlet_count(owner_ptr)
    api.post(f"Object has {count} inlets:")

    for i in range(count):
        inlet = api.inlet_nth(owner_ptr, i)
        if inlet:
            api.post(f"  Inlet {i}: {inlet}")
```

### Message-Specific Inlet

```python
import api

class CustomInlet:
    def __init__(self, owner_ptr):
        self.ptr = owner_ptr

        # Create inlet that only receives "trigger" messages
        self.trigger_inlet = api.inlet_new(self.ptr, "trigger")

    def trigger(self):
        """Called when 'trigger' received in special inlet"""
        api.post("Triggered!")

    def bang(self):
        """Called when 'bang' received in leftmost inlet"""
        api.post("Bang!")
```

## Inlet Creation Order

**Important**: Create inlets in **reverse visual order** (rightmost first) for proper display:

```python
# Correct order - visual layout will be: [0] [1] [2] [3]
inlet3 = api.proxy_new(owner, 3, stuffloc)  # Create rightmost first
inlet2 = api.proxy_new(owner, 2, stuffloc)
inlet1 = api.proxy_new(owner, 1, stuffloc)
# Leftmost inlet (0) is automatic
```

## Message Routing

### intin() and floatin()

When using typed inlets, Max automatically routes to special message handlers:

| Inlet | Integer Handler | Float Handler |
|-------|----------------|---------------|
| 1 | `in1(value)` | `ft1(value)` |
| 2 | `in2(value)` | `ft2(value)` |
| 3 | `in3(value)` | `ft3(value)` |
| ... | ... | ... |
| 9 | `in9(value)` | `ft9(value)` |

### Proxies

With proxies, use `proxy_getinlet()` to determine inlet:

```python
def handle_message(self, value):
    inlet = api.proxy_getinlet(self.ptr)
    # Route based on inlet number
```

## Notes

- Leftmost inlet (inlet 0) is automatic - don't create it
- Inlets are freed automatically when object is destroyed
- For proxies, explicitly call `inlet.delete()` in destructor
- Maximum 9 typed inlets (1-9) for `intin()`/`floatin()`
- Proxies support unlimited inlets
- Always create inlets during object initialization, not dynamically
- Use proxies for new code (more flexible than typed inlets)

## See Also

- `api.Outlet` - Creating outlets
- `api.Object` - Object management
- Max SDK: Inlet documentation
- Tutorial: Creating Multi-Inlet Objects
