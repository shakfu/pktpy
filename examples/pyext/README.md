# pyext - Python Externals for Max/MSP

Write Max/MSP externals in Python without compiling C code!

## Overview

`pyext` is a Max external that allows you to write Max object behavior entirely in Python using the embedded pocketpy interpreter. Think of it as the Python equivalent of Max's `js` object - rapid development, hot reloading, and full access to the Max API, but with Python's clean syntax and extensive ecosystem.

**Key Features:**
- Write Max externals in pure Python
- Hot reload - edit scripts while Max is running
- Use any class name with `@api.external` decorator
- Full Max API access (outlets, inlets, attributes, timing, etc.)
- Lightweight (~815KB) thanks to pocketpy v2.0.8
- No compilation needed - instant iteration
- Similar workflow to Max's built-in `js` object

## Quick Start

### Method 1: Using @api.external decorator (Recommended)

With the `@api.external` decorator, you can use **any class name** you want:

```python
import api

@api.external
class MyCustomExternal:
    """Your class can have any name when using the decorator!"""

    def __init__(self):
        # Configure your external
        self.inlets = 1    # Number of inlets
        self.outlets = 1   # Number of outlets
        self.count = 0     # Instance variables

        api.post("MyCustomExternal: ready!")

    def bang(self):
        """Handle bang messages"""
        self.count += 1
        api.post(f"Bang #{self.count}")
        self._outlets[0].int(self.count)

    def int(self, n):
        """Handle integer input"""
        api.post(f"Received: {n}")
        self._outlets[0].int(n * 2)
```

**Why use the decorator?**
- [x] Use descriptive, meaningful class names
- [x] Multiple pyext scripts can coexist without naming conflicts
- [x] More Pythonic and modern
- [x] Future-proof for advanced features

### Method 2: Class named "External" (Legacy)

For backward compatibility, you can define a class named `External` without the decorator:

```python
import api

class External:
    """Must be named 'External' when not using decorator"""

    def __init__(self):
        self.inlets = 1
        self.outlets = 1

    def bang(self):
        api.post("Bang received!")
```

> **Note:** While this method works, using `@api.external` is recommended for new scripts.

## Core Concepts

### Configuring Inlets and Outlets

Set the number of inlets and outlets in your `__init__` method:

```python
def __init__(self):
    self.inlets = 3    # 3 inlets (leftmost is default)
    self.outlets = 2   # 2 outlets (left to right)
```

After initialization, access outlets via `self._outlets` list:
- `self._outlets[0]` - leftmost outlet
- `self._outlets[1]` - second outlet from left
- etc.

### Message Handlers

Define methods to handle Max messages. Method names map directly to Max messages:

| Method | Max Message | Description |
|--------|-------------|-------------|
| `bang()` | `bang` | Responds to bang |
| `int(n)` | Integer number | Receives integers |
| `float(f)` | Float number | Receives floats |
| `list(*args)` | List of values | Receives lists |
| `anything(msg, *args)` | Any message | Catch-all handler |
| `my_custom_method(arg)` | `my_custom_method` | Custom messages |

**Example:**
```python
@api.external
class MessageHandler:
    def __init__(self):
        self.inlets = 1
        self.outlets = 1

    def bang(self):
        """Triggered when receiving bang"""
        api.post("Bang!")

    def int(self, n):
        """Triggered when receiving an integer"""
        api.post(f"Got integer: {n}")

    def float(self, f):
        """Triggered when receiving a float"""
        api.post(f"Got float: {f}")

    def custom_message(self, value):
        """Triggered by [custom_message 42]"""
        api.post(f"Custom: {value}")
```

### Sending Output

Send data through outlets using the `self._outlets` list:

```python
@api.external
class OutputExample:
    def __init__(self):
        self.inlets = 1
        self.outlets = 3

    def bang(self):
        # Send to different outlets
        self._outlets[0].int(42)          # Integer to left outlet
        self._outlets[1].float(3.14159)   # Float to middle outlet
        self._outlets[2].bang()           # Bang to right outlet
```

**Available outlet methods:**

| Method | Description | Example |
|--------|-------------|---------|
| `.bang()` | Send bang | `self._outlets[0].bang()` |
| `.int(n)` | Send integer | `self._outlets[0].int(42)` |
| `.float(f)` | Send float | `self._outlets[0].float(3.14)` |

> **Note:** Additional outlet methods for lists and messages will be added in future versions.

## Using pyext in Max

### Creating a pyext Object

In your Max patch, create a `pyext` object with your script filename:

```
[pyext custom_counter.py]
```

The script will be loaded from Max's search path (same as `js` object behavior).

### Sending Messages

Connect Max objects to send messages to your Python external:

```
[bang]                    → [pyext custom_counter.py] → [print]
[42]                      → [pyext custom_counter.py] → [number]
[set 100]                 → [pyext custom_counter.py]
[custom_message 5.5]      → [pyext custom_counter.py]
```

### File Location

Place your Python scripts in:
- Same directory as your Max patch
- Max's search path (Preferences → File Preferences)
- `~/Documents/Max 8/Library/` (or Max 9)

### Complete Example

**Python script (my_osc.py):**
```python
import api

@api.external
class SimpleOscillator:
    def __init__(self):
        self.inlets = 1
        self.outlets = 1
        self.frequency = 440.0

    def set_freq(self, f):
        self.frequency = f
        api.post(f"Frequency: {self.frequency} Hz")
        self._outlets[0].float(self.frequency)

    def bang(self):
        self._outlets[0].float(self.frequency)
```

**Max patch:**
```
[440.]
|
[set_freq $1]
|
[pyext my_osc.py]
|
[flonum]
```

## Example Scripts

This directory contains several example scripts demonstrating different features:

### Modern Style (Using @api.external)

| Script | Class Name | Description |
|--------|------------|-------------|
| `simple_decorated.py` | `ScaleValue` | Value scaling with custom class name |
| `custom_counter.py` | `MyAwesomeCounter` | Counter demonstrating naming freedom |

### Legacy Style (Named "External")

| Script | Description |
|--------|-------------|
| `simple.py` | Basic structure with value storage and multiple message handlers |
| `counter.py` | Counter that increments on bang with reset functionality |
| `scale.py` | Math operations demonstrating multiple inlets |

**Recommendation:** Start with `simple_decorated.py` or `custom_counter.py` to see the modern decorator-based approach.

## API Module Reference

The `api` module provides access to Max functionality. Import it in every pyext script:

```python
import api
```

### Console Output

| Function | Description | Example |
|----------|-------------|---------|
| `api.post(msg)` | Print to Max console | `api.post("Debug info")` |
| `api.error(msg)` | Print error message | `api.error("Something went wrong")` |

### Decorator

| Decorator | Purpose |
|-----------|---------|
| `@api.external` | Mark any class as external (enables custom class names) |

### Full API

The `api` module provides extensive Max API bindings including:
- **Timing**: Clock, ITM (tempo/timing)
- **Data structures**: AtomArray, Dictionary, Hashtab, Linklist
- **Audio**: Buffer access
- **Threading**: SysThread, SysMutex
- **Database**: Query operations
- **Messaging**: Send messages between objects
- **And more...**

> For complete API documentation, see `source/projects/pktpy/pktpy_api.h`

## Live Coding & Development

### Built-in Text Editor

Double-click any `pyext` object to open the built-in code editor:

1. **Double-click** the `pyext` object in your patch
2. **Edit** your Python code directly in the editor
3. **Save** (Cmd-S / Ctrl-S) to save changes
4. **Close** the editor - script automatically reloads

This provides immediate feedback for rapid prototyping!

### Hot Reload

Send the `reload` message to manually reload your script without closing the editor:

```
[reload( → [pyext my_script.py]
```

**Use cases:**
- Testing changes without closing the editor
- Reloading after editing in external editor (VS Code, PyCharm, etc.)
- Resetting the Python object state

### Loading Different Scripts

Load a different script file at runtime:

```
[load other_script.py( → [pyext]
```

This allows dynamic script switching without recreating the object.

### Development Workflow

**Recommended workflow for rapid development:**

1. **In Max**: Create `[pyext my_script.py]` object
2. **Double-click** to open built-in editor
3. **Write code** with immediate console feedback via `api.post()`
4. **Close editor** to reload and test
5. **Iterate** quickly!

**Alternative workflow (external editor):**

1. Edit Python files in your favorite IDE (VS Code, PyCharm, Sublime)
2. Send `reload` message to pyext object
3. Test changes in Max

> **Tip:** Use `api.post()` liberally for debugging - it's like `console.log()` for Max!

## Advanced Features

### Instance Variables

Store state between message calls using instance variables:

```python
@api.external
class Accumulator:
    def __init__(self):
        self.inlets = 1
        self.outlets = 1
        self.total = 0
        self.history = []

    def int(self, n):
        self.total += n
        self.history.append(n)
        api.post(f"Total: {self.total}, History: {self.history}")
        self._outlets[0].int(self.total)

    def reset(self):
        self.total = 0
        self.history = []
        api.post("Reset!")
```

### Error Handling

Python exceptions are caught and displayed in the Max console:

```python
@api.external
class SafeDivider:
    def __init__(self):
        self.inlets = 2
        self.outlets = 1

    def divide(self, numerator, denominator):
        try:
            result = numerator / denominator
            self._outlets[0].float(result)
        except ZeroDivisionError:
            api.error("Cannot divide by zero!")
        except Exception as e:
            api.error(f"Error: {e}")
```

## Comparison with js Object

| Feature | pyext | js |
|---------|-------|-----|
| Language | Python (pocketpy) | JavaScript (V8) |
| File size | ~815KB | ~2-3MB |
| Hot reload | [x] Yes | [x] Yes |
| Custom class names | [x] Yes (with decorator) | Fixed structure |
| Built-in editor | [x] Yes | [x] Yes |
| Syntax | Python 3.x | JavaScript ES6+ |
| API access | Full Max API | Via jsextension |
| Learning curve | Python-friendly | JavaScript-friendly |

## Troubleshooting

### Script won't load

**Problem:** Error message "script must define an external class"

**Solutions:**
- If using decorator: Ensure `@api.external` is present
- If not using decorator: Class must be named `External`
- Check for syntax errors in your Python code
- Verify `import api` is at the top of your script

### Outlets not working

**Problem:** Nothing outputs when calling outlet methods

**Solutions:**
- Ensure `self.outlets` is set in `__init__`
- Check outlet index: `self._outlets[0]`, `self._outlets[1]`, etc.
- Verify you're calling methods on `self._outlets`, not just `outlets`

### Changes not taking effect

**Problem:** Code changes don't appear after saving

**Solutions:**
- Close the editor window (triggers auto-reload)
- Send `reload` message manually
- Check Max console for error messages

## Python Limitations

pyext uses **pocketpy v2.0.8**, a lightweight Python interpreter. Be aware of these limitations:

- [x] **Supported**: Core Python 3.x syntax, classes, functions, standard types
- [x] **Supported**: Basic standard library (math, random, etc.)
- [ ] **Not available**: pip packages, numpy, pandas, requests, etc.
- [ ] **Not available**: C extensions
- [/] **Limited**: Some advanced Python features

For most Max/MSP use cases (data processing, algorithmic composition, control logic), pocketpy provides everything you need!

## Resources

- **Full API Reference**: `source/projects/pktpy/pktpy_api.h`
- **Implementation Details**: `docs/pyext-implementation-summary.md`
- **pocketpy Documentation**: [https://pocketpy.dev](https://pocketpy.dev)

## Contributing

Found a bug or want to contribute? Check out the main project repository for contribution guidelines.

