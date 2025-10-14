# SysThread and SysMutex API Wrappers

## Overview

The SysThread and SysMutex wrappers provide Python access to Max's thread management API. These allow background thread execution and thread synchronization via mutexes.

**WARNING**: Improper thread management can crash Max. Use with caution!

## Implementation

**Files**: `api_systhread.h`
**Types**: `api.SysThread`, `api.SysMutex`
**C Types**: `t_systhread`, `t_systhread_mutex`

---

# SysThread API

## Usage

### Creating and Running a Thread

```python
import api

def worker_function():
    api.post("Thread is running!")
    return 42

thread = api.SysThread(worker_function)
thread.start()
result = thread.join()  # Wait for completion
api.post(f"Thread returned: {result}")
```

### With User Data

```python
def worker_with_data(data):
    api.post(f"Processing: {data}")
    return len(data)

thread = api.SysThread(worker_with_data, "hello")
thread.start()
result = thread.join()
```

## SysThread Methods

### Constructor

#### `SysThread(callback, user_data=None)`

Creates a new thread object (does not start it).

**Parameters:**
- `callback` (callable): Python function to run in thread
- `user_data` (optional): Data to pass to callback

**Example:**
```python
thread = api.SysThread(my_function)
```

### Instance Methods

#### `start()`

Start the thread execution.

**Raises:** ValueError if thread is already running

**Example:**
```python
thread.start()
```

#### `join() -> int`

Wait for thread to complete and return its result.

**Returns:** Integer return value from thread function

**Raises:** ValueError if thread is not running

**Example:**
```python
result = thread.join()
```

#### `is_running() -> bool`

Check if thread is currently running.

**Returns:** Boolean indicating if thread is active

**Example:**
```python
if thread.is_running():
    api.post("Thread still running...")
```

#### `get_result() -> int`

Get the thread's return value (after it has completed).

**Returns:** Integer result from thread function

**Example:**
```python
result = thread.get_result()
```

### Static Methods

#### `SysThread.sleep(milliseconds)`

Sleep the current thread for specified milliseconds.

**Parameters:**
- `milliseconds` (int): Time to sleep in milliseconds

**Example:**
```python
api.SysThread.sleep(1000)  # Sleep for 1 second
```

## Use Cases

### Background Computation

```python
def compute_intensive():
    api.post("Starting computation...")
    result = 0
    for i in range(1000000):
        result += i
    api.post("Computation done!")
    return result

thread = api.SysThread(compute_intensive)
thread.start()
# Do other work while thread runs
result = thread.join()
```

### File I/O

```python
def load_file():
    # Read large file without blocking Max
    with open("large_file.txt", "r") as f:
        data = f.read()
    return len(data)

thread = api.SysThread(load_file)
thread.start()
```

### Timed Operations

```python
def delayed_action():
    api.SysThread.sleep(5000)  # Wait 5 seconds
    api.post("5 seconds elapsed!")
    return 0

thread = api.SysThread(delayed_action)
thread.start()
```

---

# SysMutex API

## Usage

### Basic Locking

```python
import api

mutex = api.SysMutex()

mutex.lock()
try:
    # Critical section
    shared_data += 1
finally:
    mutex.unlock()
```

### Try-Lock Pattern

```python
mutex = api.SysMutex()

if mutex.trylock():
    try:
        # Got the lock
        process_shared_data()
    finally:
        mutex.unlock()
else:
    api.post("Could not acquire lock")
```

## SysMutex Methods

### Constructor

#### `SysMutex()`

Creates a new mutex object.

**Example:**
```python
mutex = api.SysMutex()
```

### Methods

#### `lock()`

Acquire the mutex (blocks if already locked).

**Raises:** ValueError if mutex is NULL

**Example:**
```python
mutex.lock()
```

#### `unlock()`

Release the mutex.

**Raises:**
- ValueError if mutex is NULL
- ValueError if mutex is not locked

**Example:**
```python
mutex.unlock()
```

#### `trylock() -> bool`

Try to acquire the mutex without blocking.

**Returns:** True if lock acquired, False otherwise

**Example:**
```python
if mutex.trylock():
    # Got lock
    mutex.unlock()
```

#### `is_locked() -> bool`

Check if mutex is currently locked.

**Returns:** Boolean indicating lock state

**Example:**
```python
if mutex.is_locked():
    api.post("Mutex is locked")
```

#### `pointer() -> int`

Get raw pointer to mutex (for advanced use).

**Returns:** Integer representation of pointer

**Example:**
```python
ptr = mutex.pointer()
```

## Thread Synchronization Example

```python
import api

# Shared state
counter = 0
mutex = api.SysMutex()

def worker_thread(thread_id):
    global counter
    for i in range(100):
        mutex.lock()
        try:
            counter += 1
        finally:
            mutex.unlock()
    api.post(f"Thread {thread_id} completed")
    return 0

# Start multiple threads
threads = []
for i in range(5):
    t = api.SysThread(worker_thread, i)
    t.start()
    threads.append(t)

# Wait for all threads
for t in threads:
    t.join()

api.post(f"Final counter: {counter}")  # Should be 500
```

## Best Practices

### 1. Always Use Try-Finally

```python
mutex.lock()
try:
    # Critical section
    pass
finally:
    mutex.unlock()
```

### 2. Keep Critical Sections Short

```python
# Bad - long critical section
mutex.lock()
result = expensive_computation()  # Don't do this!
mutex.unlock()

# Good - compute outside lock
result = expensive_computation()
mutex.lock()
shared_data = result
mutex.unlock()
```

### 3. Avoid Nested Locks

```python
# Bad - can deadlock
mutex1.lock()
mutex2.lock()  # Dangerous!

# Better - single mutex
mutex.lock()
# Work with data
mutex.unlock()
```

### 4. Use Qelem for UI Updates

```python
def worker_thread():
    # Do background work
    result = compute_something()

    # Schedule UI update on main thread
    def update_ui():
        api.post(f"Result: {result}")

    ui_qelem = api.Qelem(update_ui)
    ui_qelem.set()
    return 0
```

## Thread Safety Notes

- Max's audio thread should not be blocked by mutexes
- UI operations must happen on the main thread - use Qelem
- Python's GIL is still active - use mutexes for Max object access
- Thread count should be limited (use CPU core count as guideline)

## Common Pitfalls

1. **Deadlock**: Two threads waiting for each other's locks
2. **Race Condition**: Missing mutex protection for shared data
3. **UI Blocking**: Calling UI operations from worker thread
4. **Memory Corruption**: Accessing Max objects without proper locking

## Implementation Details

### SysThread

- Uses `systhread_create()` to spawn threads
- Callback wrapper handles Python function calls in thread context
- Return values are captured from integer returns
- Thread is automatically joined in destructor if still running

### SysMutex

- Uses `systhread_mutex_new()` for creation
- Lock state is tracked for error checking
- Mutex is automatically freed and unlocked in destructor

### Memory Management

- Callbacks kept alive via `py_setslot`
- User data also protected by `py_setslot`
- No manual reference counting needed
- Destructors handle cleanup

## Performance Considerations

- Thread creation has overhead (~1ms)
- Mutex operations are fast (~100ns)
- Context switching adds latency
- Use thread pools for frequent operations

## See Also

- Qelem API - For main-thread scheduling
- Clock API - For time-based operations
- Max threading documentation
