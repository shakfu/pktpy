# Path and File I/O API

The `api.Path` class and related module-level functions provide Python access to Max's file system and path management APIs.

## Overview

Max uses **Path IDs** (short integers) to identify file system locations. The Path API provides:
- File location and searching
- Path management and conversion
- File I/O operations (open, read, write, close)
- Path style conversions (Unix, Windows, Max)

## Constants

### Path Styles
```python
api.PATH_STYLE_MAX      # Max platform default
api.PATH_STYLE_NATIVE   # OS native style
api.PATH_STYLE_SLASH    # Unix style ('/')
```

### Path Types
```python
api.PATH_TYPE_ABSOLUTE  # Absolute path
api.PATH_TYPE_RELATIVE  # Relative to search path
api.PATH_TYPE_BOOT      # Boot/root path
api.PATH_TYPE_PATH      # Regular path
```

### File Permissions
```python
api.PATH_READ_PERM      # Read-only (1)
api.PATH_WRITE_PERM     # Write-only (2)
api.PATH_RW_PERM        # Read-write (3)
```

### File Position Modes
```python
api.SYSFILE_ATMARK      # At current position
api.SYSFILE_FROMSTART   # From start of file
api.SYSFILE_FROMLEOF    # From end of file
```

## Module-Level Functions

### Path Management

#### `api.path_getdefault() -> int`
Get the default search path ID.

**Example:**
```python
path_id = api.path_getdefault()
api.post(f"Default path: {path_id}")
```

---

#### `api.path_setdefault(path_id: int, recursive: bool = False)`
Set the default search path.

**Parameters:**
- `path_id`: Path ID to set as default
- `recursive`: Whether to search recursively

**Example:**
```python
api.path_setdefault(1234, recursive=True)
```

---

#### `api.path_getapppath() -> int`
Get the Max application path ID.

**Example:**
```python
app_path = api.path_getapppath()
```

---

### File Location

#### `api.locatefile_extended(filename: str, typelist: list = None) -> tuple | None`
Locate a file in Max's search path.

**Parameters:**
- `filename`: Name of file to find (can be modified by function)
- `typelist`: Optional list of 4-character type codes

**Returns:**
- Tuple `(filename, path_id, filetype)` if found
- `None` if not found

**Example:**
```python
result = api.locatefile_extended("myfile.txt")
if result:
    filename, path_id, filetype = result
    api.post(f"Found: {filename} at path {path_id}")
else:
    api.error("File not found")
```

---

### Path Conversion

#### `api.path_toabsolutesystempath(path_id: int, filename: str) -> str`
Convert Path ID + filename to absolute system path.

**Parameters:**
- `path_id`: Path ID
- `filename`: Filename (can be empty for directory path)

**Returns:**
- Absolute path string

**Example:**
```python
abs_path = api.path_toabsolutesystempath(path_id, "data.json")
api.post(f"Absolute path: {abs_path}")
```

---

#### `api.path_nameconform(src: str, style: int, type: int) -> str`
Convert path string to specified style.

**Parameters:**
- `src`: Source path string
- `style`: Path style constant (e.g., `PATH_STYLE_SLASH`)
- `type`: Path type constant (e.g., `PATH_TYPE_ABSOLUTE`)

**Returns:**
- Converted path string

**Example:**
```python
# Convert to native OS style
native_path = api.path_nameconform(
    "/path/to/file",
    api.PATH_STYLE_NATIVE,
    api.PATH_TYPE_ABSOLUTE
)
```

---

### File Operations

#### `api.path_opensysfile(filename: str, path_id: int, perm: int) -> int`
Open a file for reading/writing.

**Parameters:**
- `filename`: Name of file
- `path_id`: Path ID where file is located
- `perm`: Permission flags (PATH_READ_PERM, PATH_WRITE_PERM, PATH_RW_PERM)

**Returns:**
- File handle (integer)

**Raises:**
- `RuntimeError` if file cannot be opened

**Example:**
```python
fh = api.path_opensysfile("data.txt", path_id, api.PATH_READ_PERM)
```

---

#### `api.path_createsysfile(filename: str, path_id: int, filetype: str) -> int`
Create a new file.

**Parameters:**
- `filename`: Name of file to create
- `path_id`: Path ID where to create
- `filetype`: 4-character type code (e.g., "TEXT")

**Returns:**
- File handle (integer)

**Raises:**
- `RuntimeError` if file cannot be created

**Example:**
```python
fh = api.path_createsysfile("output.txt", path_id, "TEXT")
```

---

#### `api.path_closesysfile(filehandle: int)`
Close an open file.

**Parameters:**
- `filehandle`: File handle from open or create

**Example:**
```python
api.path_closesysfile(fh)
```

---

#### `api.sysfile_read(filehandle: int, count: int) -> str`
Read bytes from file.

**Parameters:**
- `filehandle`: Open file handle
- `count`: Number of bytes to read

**Returns:**
- String containing read data

**Raises:**
- `RuntimeError` on read failure

**Example:**
```python
data = api.sysfile_read(fh, 1024)
api.post(f"Read {len(data)} bytes")
```

---

#### `api.sysfile_write(filehandle: int, data: str) -> int`
Write string/bytes to file.

**Parameters:**
- `filehandle`: Open file handle
- `data`: String data to write

**Returns:**
- Number of bytes actually written

**Raises:**
- `RuntimeError` on write failure

**Example:**
```python
bytes_written = api.sysfile_write(fh, "Hello, Max!")
```

---

#### `api.sysfile_readtextfile(filehandle: int, maxsize: int = 65536) -> str`
Read entire text file into string.

**Parameters:**
- `filehandle`: Open file handle
- `maxsize`: Maximum bytes to read (default 65536)

**Returns:**
- Complete file contents as string

**Example:**
```python
content = api.sysfile_readtextfile(fh)
api.post(content)
```

---

#### `api.sysfile_geteof(filehandle: int) -> int`
Get end-of-file position.

**Returns:**
- File size in bytes

---

#### `api.sysfile_seteof(filehandle: int, pos: int)`
Set end-of-file position (truncate/extend).

---

#### `api.sysfile_getpos(filehandle: int) -> int`
Get current file position.

**Returns:**
- Current position in bytes

---

#### `api.sysfile_setpos(filehandle: int, pos: int, mode: int)`
Set file position (seek).

**Parameters:**
- `filehandle`: Open file handle
- `pos`: Position offset
- `mode`: Position mode (SYSFILE_FROMSTART, etc.)

**Example:**
```python
# Seek to start
api.sysfile_setpos(fh, 0, api.SYSFILE_FROMSTART)

# Seek to end
api.sysfile_setpos(fh, 0, api.SYSFILE_FROMLEOF)
```

---

#### `api.path_deletefile(filename: str, path_id: int)`
Delete a file.

**Parameters:**
- `filename`: Name of file
- `path_id`: Path ID where file is located

**Raises:**
- `RuntimeError` if deletion fails

**Example:**
```python
api.path_deletefile("temp.txt", path_id)
```

---

## Class: `api.Path`

Path object for managing path IDs.

### Constructor

```python
p = api.Path()
```

### Methods

#### `set_from_id(path_id: int)`
Set path from a path ID.

---

#### `get_id() -> int | None`
Get path ID.

**Returns:**
- Path ID integer, or `None` if not set

---

#### `get_path() -> str | None`
Get absolute path string.

**Returns:**
- Path string, or `None` if not set

---

#### `is_set() -> bool`
Check if path is set.

---

## Complete Examples

### Read a Text File

```python
import api

# Find file in search path
result = api.locatefile_extended("config.txt")
if not result:
    api.error("Config file not found")
    return

filename, path_id, filetype = result

# Open file for reading
fh = api.path_opensysfile(filename, path_id, api.PATH_READ_PERM)

# Read entire contents
content = api.sysfile_readtextfile(fh)
api.post(f"Config contents:\n{content}")

# Close file
api.path_closesysfile(fh)
```

### Write a Text File

```python
import api

# Get default path
path_id = api.path_getdefault()

# Create new file
fh = api.path_createsysfile("output.txt", path_id, "TEXT")

# Write data
data = "Hello from Python!\nLine 2\nLine 3"
bytes_written = api.sysfile_write(fh, data)
api.post(f"Wrote {bytes_written} bytes")

# Close file
api.path_closesysfile(fh)
```

### Read File in Chunks

```python
import api

result = api.locatefile_extended("largefile.dat")
if not result:
    return

filename, path_id, _ = result
fh = api.path_opensysfile(filename, path_id, api.PATH_READ_PERM)

# Read in 1KB chunks
chunk_size = 1024
while True:
    chunk = api.sysfile_read(fh, chunk_size)
    if len(chunk) == 0:
        break
    api.post(f"Read chunk: {len(chunk)} bytes")

api.path_closesysfile(fh)
```

### Convert Path Styles

```python
import api

# Mac/Unix style path
unix_path = "/Users/name/Documents/Max 8/file.maxpat"

# Convert to native style for current OS
native_path = api.path_nameconform(
    unix_path,
    api.PATH_STYLE_NATIVE,
    api.PATH_TYPE_ABSOLUTE
)

api.post(f"Native path: {native_path}")
```

### Get Absolute Paths

```python
import api

# Get app path and convert to absolute
app_path_id = api.path_getapppath()
abs_path = api.path_toabsolutesystempath(app_path_id, "")
api.post(f"Max is installed at: {abs_path}")

# Get default path
default_id = api.path_getdefault()
abs_default = api.path_toabsolutesystempath(default_id, "")
api.post(f"Default path: {abs_default}")
```

## Notes

- **Path IDs** are Max's internal file location references
- Always close file handles with `path_closesysfile()`
- File handles are integers (memory addresses cast to int)
- Use context managers in production code to ensure files are closed
- `locatefile_extended()` may modify the filename parameter
- Text files use platform line endings (handled automatically)
- Binary files should use `sysfile_read/write` with explicit byte counts

## See Also

- Max SDK: `ext_path.h`, `ext_sysfile.h`
- `api.Dictionary` for structured file I/O
- Python's built-in `open()` for standard file operations
