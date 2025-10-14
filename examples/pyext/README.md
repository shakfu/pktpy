# pyext Examples

This directory contains example Python scripts for the `pyext` Max external.

## What is pyext?

`pyext` is a Max external that allows you to write Max object behavior in Python using the pocketpy interpreter. It's similar to the `js` object but for Python code.

## Basic Structure

Every pyext script must define an `External` class:

```python
import api

class External:
    def __init__(self):
        # Configure inlets and outlets
        self.inlets = 1
        self.outlets = 1

    def bang(self):
        """Respond to bang message"""
        api.post("Bang received!")
```

## Configuration

In the `__init__` method, set:
- `self.inlets` - number of inlets (default: 1)
- `self.outlets` - number of outlets (default: 1)

After initialization, your Python object has access to:
- `self._outlets` - list of outlet objects for sending data to Max

## Message Handlers

Define methods to handle Max messages:
- `bang()` - responds to bang
- `int(n)` - responds to int messages
- `float(f)` - responds to float messages
- `list(*args)` - responds to list messages
- Custom methods - any other method name becomes a Max message

## Sending Output

Use the `self._outlets` list to send data:

```python
def bang(self):
    # Send integer to outlet 0
    self._outlets[0].int(42)

    # Send float to outlet 1 (if it exists)
    if len(self._outlets) > 1:
        self._outlets[1].float(3.14)

    # Send bang
    self._outlets[0].bang()
```

Available outlet methods:
- `.bang()` - Send bang
- `.int(n)` - Send integer
- `.float(f)` - Send float
- `.list(atomarray)` - Send list (requires AtomArray)
- `.anything(symbol, atomarray)` - Send message

## Using in Max

1. Create a `pyext` object with the script name:
   ```
   [pyext simple.py]
   ```

2. Send messages to the object:
   ```
   [bang] -> [pyext simple.py]
   [42] -> [pyext simple.py]
   [set_value 100] -> [pyext simple.py]
   ```

## Examples

- `simple.py` - Basic structure with value storage
- `counter.py` - Counter that increments on bang
- `scale.py` - Math operations with multiple inlets

## API Module

The `api` module provides access to Max functionality:

- `api.post(msg)` - Print to Max console
- `api.error(msg)` - Print error to Max console
- More functions available (see pktpy_api.h for full list)

## Editing Scripts

### Text Editor

Double-click the `pyext` object to open a built-in text editor showing the Python script content. You can edit the script directly in Max:

1. Double-click the object
2. Edit the Python code
3. Close the editor - script automatically reloads (default behavior)

### Editor Behavior

By default:
- **Save** (Cmd-S) - Saves to file but doesn't reload
- **Close** - Saves to file and reloads the script

You can customize this behavior with attributes (future feature).

### Manual Reloading

Send the `reload` message to manually reload the Python script:
```
[reload( -> [pyext simple.py]
```

### Read Script

Load a script file into the editor buffer:
```
[read scriptname.py( -> [pyext]
```

This workflow enables:
- Live coding directly in Max
- Quick iteration without leaving Max
- Visual feedback of Python code
- Similar workflow to the `js` object
