# pktpy Test Suite

## Overview

This directory contains tests for the pktpy Max/MSP external. The tests follow the pattern used by the [py external](https://github.com/shakfu/py) for consistency and maintainability.

## Test Pattern

Tests follow these conventions (based on py external tests):

### Structure
- Test functions use `test_` prefix
- Each test focuses on ONE specific behavior
- Tests use assertions (`assert`) for validation
- Minimal logging - only what's essential
- Concise docstrings describe what's being tested
- No `run_all_tests()` wrapper functions
- No `if __name__ == "__main__":` patterns

### Example

**Good (py-style):**
```python
def test_atom_creation():
    """Test Atom creation with different types"""
    a_long = api.Atom(42)
    a_float = api.Atom(3.14)

    assert a_long.value == 42
    assert abs(a_float.value - 3.14) < 0.01
```

**Avoid (old pktpy-style):**
```python
def test_atom_creation():
    """Test Atom creation"""
    api.post("=== Testing Atom Creation ===\n")

    a_long = api.Atom(42)
    api.post(f"Created atom with value: {a_long.value}\n")

    a_float = api.Atom(3.14)
    api.post(f"Created float atom: {a_float.value}\n")

    api.post("Test completed successfully!\n")
```

## Running Tests

Tests are designed to be run inside Max/MSP using the pktpy external:

1. **Load a test file:**
   - Send `execfile examples/tests/test_api_types.py` to pktpy object
   - Or use `load` message with the filename

2. **Run specific tests:**
   - Call individual test functions from Max patches
   - Example: Send `exec test_atom_creation()` to pktpy

3. **Check results:**
   - Assertions will raise errors if tests fail
   - Success indicated by no error messages
   - Use Max console to view output

## Test Files

**Total: 15 files | 146 test functions**

All test files now use the `test_` prefix for test functions.

### ✅ Fully Refactored (py-style)
These files follow the py external test pattern with focused tests and assertions:

- **test_api_types.py** (16 tests) - Atom and Symbol types
- **test_atomarray.py** (12 tests) - AtomArray wrapper
- **test_dictionary.py** (16 tests) - Dictionary wrapper
- **test_parse.py** (12 tests) - Parsing functions

### ✅ Naming Standardized
These files have correct `test_` prefixes but still use verbose style:

- **test_database.py** (10 tests) - Database operations
- **test_inlet.py** (6 tests) - Inlet handling
- **test_path.py** (8 tests) - File path operations
- **test_table.py** (6 tests) - Table operations

### ⚠️ Pending Full Refactoring
These files still use the old verbose documentation style:

- test_atom_utils.py
- test_buffer.py
- test_clock.py
- test_object_registration.py
- test_object.py
- test_outlet.py
- test_patcher_manipulation.py

## Key Differences: pktpy vs py Tests

### py external tests
- Standalone Python tests in `/examples/tests`
- Can be run with pytest
- Have pytest.ini configuration
- Tests are in `/source/projects/py/tests` for unit testing

### pktpy external tests
- Tests run inside Max/MSP via pktpy external
- No pytest infrastructure (yet)
- Tests import `api` module (pktpy's Python API)
- Validate Max/MSP integration directly

## Benefits of New Pattern

1. **Focused tests** - One behavior per test
2. **Clear validation** - Assertions show what's expected
3. **Less noise** - Minimal logging, easier to debug
4. **Consistent style** - Follows py external conventions
5. **Better maintainability** - Easier to understand and modify

## Future Work

1. **Complete refactoring** - Apply pattern to remaining 11 test files
2. **Test runner** - Consider adding automated test runner for Max
3. **CI integration** - Explore automated testing in Max environment
4. **Documentation** - Add more examples and usage patterns
