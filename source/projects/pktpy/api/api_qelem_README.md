# Qelem API Wrapper

## Overview

The Qelem wrapper provides Python access to Max's queue-based defer mechanism. Qelems are an alternative to the defer mechanism that offer more control over deferred execution, particularly useful for UI updates and scheduled callbacks.

## Implementation

**File**: `api_qelem.h`
**Type**: `api.Qelem`
**C Type**: `t_qelem*`

## Usage

### Creating a Qelem

```python
import api

def my_callback():
    api.post("Qelem callback executed!")

q = api.Qelem(my_callback)
```

### With User Data

```python
def my_callback_with_data(data):
    api.post(f"Received: {data}")

q = api.Qelem(my_callback_with_data, "my_data")
```

## Methods

### Constructor

#### `Qelem(callback, user_data=None)`

Creates a new Qelem object.

**Parameters:**
- `callback` (callable): Python function to be called when qelem executes
- `user_data` (optional): Optional data to pass to the callback

**Example:**
```python
q = api.Qelem(lambda: api.post("Hello!"))
```

### Methods

#### `set()`

Schedule the qelem for execution. The callback will be called on the next scheduler tick.

**Example:**
```python
q.set()
```

#### `unset()`

Cancel a scheduled qelem execution.

**Example:**
```python
q.unset()
```

#### `front()`

Schedule the qelem for execution at the front of the queue (higher priority).

**Example:**
```python
q.front()
```

#### `is_set() -> bool`

Check if the qelem is currently scheduled for execution.

**Returns:** Boolean indicating if qelem is set

**Example:**
```python
if q.is_set():
    api.post("Qelem is scheduled")
```

#### `is_null() -> bool`

Check if the internal qelem pointer is NULL.

**Returns:** Boolean indicating if qelem is null

**Example:**
```python
if not q.is_null():
    api.post("Qelem is valid")
```

#### `pointer() -> int`

Get the raw pointer to the t_qelem object (for advanced use).

**Returns:** Integer representation of pointer

**Example:**
```python
ptr = q.pointer()
api.post(f"Qelem pointer: 0x{ptr:x}")
```

## Use Cases

### Deferred UI Updates

```python
def update_ui():
    # Update Max UI elements
    api.post("UI updated")

q = api.Qelem(update_ui)
q.set()  # Schedule UI update
```

### Periodic Callbacks

```python
def periodic_task():
    api.post("Periodic task running")
    # Reschedule for next execution
    q.set()

q = api.Qelem(periodic_task)
q.set()  # Start periodic execution
```

### One-Shot Deferred Execution

```python
def one_shot():
    api.post("One-shot execution")

q = api.Qelem(one_shot)
q.set()
```

## Comparison with Defer

| Feature | Qelem | Defer |
|---------|-------|-------|
| Control | High - can unset, use front() | Low - fire and forget |
| Reusability | Can be reused | One-time use |
| Priority | Can set priority with front() | Normal priority only |
| Overhead | Slightly higher (object creation) | Lower |

## Thread Safety

Qelems are designed to be set from any thread but execute in the main scheduler thread. This makes them safe for cross-thread communication.

```python
def worker_thread():
    # Do some work
    result = compute_something()

    # Schedule UI update from worker thread
    ui_qelem.set()

thread = api.SysThread(worker_thread)
thread.start()
```

## Notes

- The callback is executed in Max's main scheduler thread
- Callbacks should be quick to avoid blocking the scheduler
- User data is kept alive via `py_setslot` - no manual reference counting needed
- The qelem is automatically freed when the Python object is garbage collected
- Setting an already-set qelem is safe (no-op)

## Implementation Details

The wrapper uses:
- `qelem_new()` - Create qelem with bridge function
- `qelem_set()` - Schedule execution
- `qelem_unset()` - Cancel execution
- `qelem_front()` - Priority execution
- `qelem_free()` - Cleanup (in destructor)

The callback wrapper bridges C to Python:
1. Pushes Python callback onto stack
2. Pushes user_data if provided
3. Calls via `py_vectorcall()`
4. Handles errors with `py_printexc()`

## See Also

- Clock API - For time-based scheduling
- SysThread API - For background thread execution
- Max documentation on qelems
