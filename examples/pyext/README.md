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

## Message Handlers

Define methods to handle Max messages:
- `bang()` - responds to bang
- `int(n)` - responds to int messages
- `float(f)` - responds to float messages
- `list(*args)` - responds to list messages
- Custom methods - any other method name becomes a Max message

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

## Reloading Scripts

Send the `reload` message to reload the Python script:
```
[reload( -> [pyext simple.py]
```

This allows for live coding and quick iteration.
