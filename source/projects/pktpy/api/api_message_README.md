# Message Sending and Preset API

## Overview

The message sending API provides Python access to Max's message dispatch system via `typedmess()` and convenience wrappers. This allows Python code to send messages directly to Max objects, enabling deep integration with the Max patcher environment.

## Implementation

**File**: `api_message.h`, `api_preset.h`
**No classes** - All module-level functions

## Message Sending Functions

### Core Function

#### `typedmess(obj_ptr: int, message: str, *args) -> int|None`

Send a typed message to a Max object.

**Parameters:**
- `obj_ptr` (int): Pointer to target Max object
- `message` (str): Message selector
- `*args`: Variable arguments (int, float, or str)

**Returns:** Result pointer as integer, or None

**Example:**
```python
import api

# Get object pointer (from patcher, etc.)
obj_ptr = some_object.pointer()

# Send various messages
api.typedmess(obj_ptr, "bang")
api.typedmess(obj_ptr, "set", 42)
api.typedmess(obj_ptr, "anything", 1, 2.5, "symbol")
```

### Convenience Functions

#### `send_bang(obj_ptr: int) -> int|None`

Send a bang message.

**Example:**
```python
api.send_bang(obj_ptr)
```

#### `send_int(obj_ptr: int, value: int) -> int|None`

Send an integer value.

**Example:**
```python
api.send_int(obj_ptr, 42)
```

#### `send_float(obj_ptr: int, value: float) -> int|None`

Send a float value.

**Example:**
```python
api.send_float(obj_ptr, 3.14159)
```

#### `send_symbol(obj_ptr: int, symbol: str) -> int|None`

Send a symbol.

**Example:**
```python
api.send_symbol(obj_ptr, "hello")
```

#### `send_list(obj_ptr: int, items: list) -> int|None`

Send a list message.

**Parameters:**
- `obj_ptr` (int): Target object pointer
- `items` (list): List of int, float, or str values

**Example:**
```python
api.send_list(obj_ptr, [1, 2, 3, 4.5, "symbol"])
```

#### `send_anything(obj_ptr: int, selector: str, *args) -> int|None`

Send an arbitrary message with arguments.

**Parameters:**
- `obj_ptr` (int): Target object pointer
- `selector` (str): Message selector
- `*args`: Variable arguments

**Example:**
```python
api.send_anything(obj_ptr, "custom_message", 1, 2, "arg3")
```

#### `send_message(obj_ptr: int, message: str, atomarray: AtomArray) -> int|None`

Send a message with an AtomArray.

**Parameters:**
- `obj_ptr` (int): Target object pointer
- `message` (str): Message selector
- `atomarray` (AtomArray): Arguments as AtomArray

**Example:**
```python
aa = api.AtomArray([1, 2, 3])
api.send_message(obj_ptr, "list", aa)
```

## Preset Functions

### `preset_store(fmt: str)`

Store preset data.

**Parameters:**
- `fmt` (str): Format string for preset data

**Example:**
```python
api.preset_store("preset_data")
```

### `preset_set(obj_ptr: int, value: int)`

Set preset value for an object.

**Parameters:**
- `obj_ptr` (int): Object pointer
- `value` (int): Preset number

**Example:**
```python
api.preset_set(obj_ptr, 1)
```

### `preset_int(obj_ptr: int, value: int)`

Send preset int to object.

**Parameters:**
- `obj_ptr` (int): Object pointer
- `value` (int): Integer value

**Example:**
```python
api.preset_int(obj_ptr, 42)
```

### `preset_get_data_symbol() -> str`

Get the preset data symbol name.

**Returns:** String "preset_data"

**Example:**
```python
symbol = api.preset_get_data_symbol()
```

## Common Use Cases

### Controlling UI Objects

```python
import api

# Get UI object from patcher
patcher = api.Patcher()
box = patcher.get_named_box("my_slider")
if box:
    obj = box.get_object()
    obj_ptr = obj.pointer()

    # Set slider value
    api.send_float(obj_ptr, 0.75)

    # Or use set message (doesn't output)
    api.typedmess(obj_ptr, "set", 0.75)
```

### Sending Messages to Named Objects

```python
import api

def send_to_named(name, *args):
    """Send message to named object in patcher"""
    patcher = api.Patcher()
    box = patcher.get_named_box(name)
    if box:
        obj = box.get_object()
        if args:
            api.send_anything(obj.pointer(), "list", *args)
        else:
            api.send_bang(obj.pointer())

# Usage
send_to_named("output_number", 42)
send_to_named("trigger_button")
```

### Dynamic Patcher Control

```python
import api

def create_and_connect():
    """Create objects and connect them"""
    patcher = api.Patcher()
    patcher_ptr = patcher.pointer()

    # Create number box
    api.typedmess(patcher_ptr, "newobject", "number")

    # Create print object
    api.typedmess(patcher_ptr, "newobject", "print", "output")
```

### Querying Object State

```python
import api

def get_object_value(obj_ptr):
    """Get current value from object"""
    # Some objects respond to "get" message
    result_ptr = api.typedmess(obj_ptr, "get")

    # Process result if needed
    if result_ptr:
        # Wrap result in appropriate type
        pass

    return result_ptr
```

### Batch Message Sending

```python
import api

def update_many_objects(objects, values):
    """Update multiple objects efficiently"""
    for obj_ptr, value in zip(objects, values):
        if isinstance(value, int):
            api.send_int(obj_ptr, value)
        elif isinstance(value, float):
            api.send_float(obj_ptr, value)
        elif isinstance(value, list):
            api.send_list(obj_ptr, value)
```

### Message Routing

```python
import api

def route_message(obj_ptr, msg_type, value):
    """Route messages based on type"""
    handlers = {
        "bang": lambda: api.send_bang(obj_ptr),
        "int": lambda: api.send_int(obj_ptr, value),
        "float": lambda: api.send_float(obj_ptr, value),
        "list": lambda: api.send_list(obj_ptr, value),
    }

    handler = handlers.get(msg_type)
    if handler:
        handler()
```

### Working with AtomArrays

```python
import api

def send_complex_message(obj_ptr):
    """Send complex message using AtomArray"""
    # Build argument list
    aa = api.AtomArray()
    aa.append(api.Atom(42))
    aa.append(api.Atom(3.14))
    aa.append(api.Atom(api.gensym("symbol")))

    # Send as list
    api.send_message(obj_ptr, "list", aa)
```

### Preset Integration

```python
import api

class PresetManager:
    """Manage preset storage and recall"""

    def __init__(self):
        self.objects = {}

    def register(self, name, obj_ptr):
        """Register object for preset management"""
        self.objects[name] = obj_ptr

    def store_preset(self, preset_num):
        """Store current state as preset"""
        api.preset_store(f"preset_{preset_num}")
        api.post(f"Stored preset {preset_num}")

    def recall_preset(self, preset_num):
        """Recall preset for all objects"""
        for name, obj_ptr in self.objects.items():
            api.preset_set(obj_ptr, preset_num)

# Usage
pm = PresetManager()
pm.register("slider", slider_ptr)
pm.register("number", number_ptr)
pm.store_preset(1)
```

## Error Handling

```python
import api

def safe_send(obj_ptr, message, *args):
    """Safely send message with error handling"""
    try:
        result = api.typedmess(obj_ptr, message, *args)
        return result
    except TypeError as e:
        api.error(f"Type error: {e}")
    except ValueError as e:
        api.error(f"Value error: {e}")
    except Exception as e:
        api.error(f"Unexpected error: {e}")
    return None

# Usage
result = safe_send(obj_ptr, "set", 42)
```

## Message Types

| Function | Max Message | Arguments |
|----------|-------------|-----------|
| `send_bang` | bang | None |
| `send_int` | int | Integer |
| `send_float` | float | Float |
| `send_symbol` | symbol | Symbol string |
| `send_list` | list | Mixed list |
| `send_anything` | anything | Custom selector + args |

## Important Notes

- **Object Pointers**: Must be valid Max object pointers (use `.pointer()` method)
- **Type Checking**: Arguments are checked and converted to Max atoms
- **Return Values**: Some messages return pointers, most return None
- **Thread Safety**: Messages should be sent from main thread (use Qelem for thread safety)
- **Memory**: Arguments are copied, temporary memory is freed automatically

## Best Practices

1. **Validate Pointers**:
```python
if obj_ptr and obj_ptr != 0:
    api.send_int(obj_ptr, 42)
```

2. **Use Appropriate Function**:
```python
# Preferred - clear intent
api.send_float(obj_ptr, 3.14)

# Also works but less clear
api.typedmess(obj_ptr, "float", 3.14)
```

3. **Check Return Values**:
```python
result = api.typedmess(obj_ptr, "getitem", 0)
if result:
    # Process result
    pass
```

4. **Use Defer for Thread Safety**:
```python
def worker_thread():
    # Don't send messages directly from thread!
    result = compute_something()

    # Use Qelem to defer to main thread
    def send_result():
        api.send_int(obj_ptr, result)

    q = api.Qelem(send_result)
    q.set()
```

## Performance Considerations

- Message sending is relatively fast (~1-10 microseconds)
- `typedmess` has argument conversion overhead
- Convenience functions are slightly faster (pre-converted types)
- Batch messages in loops when possible
- Avoid sending messages at audio rate

## Common Pitfalls

1. **Invalid Pointers**: Always validate object pointers
2. **Wrong Thread**: Don't send UI messages from threads
3. **Type Mismatches**: Ensure arguments match what object expects
4. **Memory Leaks**: None - automatic cleanup is handled
5. **Object Lifetime**: Don't keep pointers to deleted objects

## Debugging

```python
import api

def debug_message(obj_ptr, message, *args):
    """Send message with debug output"""
    api.post(f"Sending {message} to 0x{obj_ptr:x}")
    api.post(f"Args: {args}")

    result = api.typedmess(obj_ptr, message, *args)

    api.post(f"Result: {result}")
    return result
```

## See Also

- Object API - For object creation and management
- Patcher API - For patcher manipulation
- Outlet API - For output from external
- Qelem API - For thread-safe message sending
