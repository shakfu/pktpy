"""
Example: Using the Path and File I/O API in Max/MSP

This demonstrates how to use file operations and path management
through the api module.

Setup in Max:
1. Create a [pktpy] object
2. Load or execute this code
3. Check Max console for output
"""

import api

def example_locate_file():
    """Find a file in Max's search path"""
    api.post("=== Locate File Example ===")

    # Search for a Max patcher file
    result = api.locatefile_extended("init.txt")

    if result:
        filename, path_id, filetype = result
        api.post(f"Found file: {filename}")
        api.post(f"Path ID: {path_id}")
        api.post(f"File type: {filetype}")

        # Convert to absolute path
        abs_path = api.path_toabsolutesystempath(path_id, filename)
        api.post(f"Absolute path: {abs_path}")
    else:
        api.post("File not found")


def example_read_textfile():
    """Read a text file"""
    api.post("=== Read Text File Example ===")

    # Try to find and read a file
    result = api.locatefile_extended("README.md")

    if not result:
        api.post("README.md not found")
        return

    filename, path_id, _ = result

    try:
        # Open file for reading
        fh = api.path_opensysfile(filename, path_id, api.PATH_READ_PERM)

        # Read entire file
        content = api.sysfile_readtextfile(fh)

        # Show first 200 characters
        preview = content[:200] if len(content) > 200 else content
        api.post(f"File contents (first 200 chars):\n{preview}")

        # Close file
        api.path_closesysfile(fh)

    except Exception as e:
        api.error(f"Error reading file: {e}")


def example_write_textfile():
    """Write a text file"""
    api.post("=== Write Text File Example ===")

    # Get default path
    path_id = api.path_getdefault()
    api.post(f"Using path ID: {path_id}")

    filename = "pktpy_test_output.txt"

    try:
        # Create new file
        fh = api.path_createsysfile(filename, path_id, "TEXT")

        # Write some data
        lines = [
            "Hello from pktpy!",
            "This file was created by Python code in Max.",
            f"Path ID: {path_id}",
            "End of file."
        ]

        data = "\n".join(lines)
        bytes_written = api.sysfile_write(fh, data)

        api.post(f"Wrote {bytes_written} bytes to {filename}")

        # Close file
        api.path_closesysfile(fh)

        # Show absolute path
        abs_path = api.path_toabsolutesystempath(path_id, filename)
        api.post(f"File created at: {abs_path}")

    except Exception as e:
        api.error(f"Error writing file: {e}")


def example_file_chunks():
    """Read file in chunks"""
    api.post("=== Read Chunks Example ===")

    result = api.locatefile_extended("README.md")
    if not result:
        return

    filename, path_id, _ = result

    try:
        fh = api.path_opensysfile(filename, path_id, api.PATH_READ_PERM)

        # Get file size
        file_size = api.sysfile_geteof(fh)
        api.post(f"File size: {file_size} bytes")

        # Read in chunks
        chunk_size = 100
        chunks_read = 0
        total_read = 0

        while True:
            chunk = api.sysfile_read(fh, chunk_size)
            if len(chunk) == 0:
                break
            chunks_read += 1
            total_read += len(chunk)

        api.post(f"Read {chunks_read} chunks, {total_read} total bytes")

        api.path_closesysfile(fh)

    except Exception as e:
        api.error(f"Error: {e}")


def example_file_seeking():
    """Demonstrate file seeking"""
    api.post("=== File Seeking Example ===")

    result = api.locatefile_extended("README.md")
    if not result:
        return

    filename, path_id, _ = result

    try:
        fh = api.path_opensysfile(filename, path_id, api.PATH_READ_PERM)

        # Read from start
        api.sysfile_setpos(fh, 0, api.SYSFILE_FROMSTART)
        first_10 = api.sysfile_read(fh, 10)
        api.post(f"First 10 bytes: {first_10}")

        # Get current position
        pos = api.sysfile_getpos(fh)
        api.post(f"Current position: {pos}")

        # Seek to position 100
        api.sysfile_setpos(fh, 100, api.SYSFILE_FROMSTART)
        middle = api.sysfile_read(fh, 20)
        api.post(f"20 bytes from pos 100: {middle}")

        # Seek to end minus 20
        api.sysfile_setpos(fh, -20, api.SYSFILE_FROMLEOF)
        last_20 = api.sysfile_read(fh, 20)
        api.post(f"Last 20 bytes: {last_20}")

        api.path_closesysfile(fh)

    except Exception as e:
        api.error(f"Error: {e}")


def example_path_conversion():
    """Convert between path styles"""
    api.post("=== Path Conversion Example ===")

    # Example paths
    paths = [
        "/Users/name/Documents/file.txt",
        "C:\\Users\\name\\Documents\\file.txt",
        "Macintosh HD:Users:name:Documents:file.txt"
    ]

    for path in paths:
        try:
            # Convert to slash style
            slash = api.path_nameconform(
                path,
                api.PATH_STYLE_SLASH,
                api.PATH_TYPE_ABSOLUTE
            )
            api.post(f"Original: {path}")
            api.post(f"Slash:    {slash}")

            # Convert to native
            native = api.path_nameconform(
                path,
                api.PATH_STYLE_NATIVE,
                api.PATH_TYPE_ABSOLUTE
            )
            api.post(f"Native:   {native}")
            api.post("")

        except Exception as e:
            api.post(f"Error converting {path}: {e}")


def example_path_info():
    """Show path information"""
    api.post("=== Path Information Example ===")

    # Default path
    default_id = api.path_getdefault()
    default_path = api.path_toabsolutesystempath(default_id, "")
    api.post(f"Default path ID: {default_id}")
    api.post(f"Default path: {default_path}")

    # App path
    app_id = api.path_getapppath()
    app_path = api.path_toabsolutesystempath(app_id, "")
    api.post(f"Max app path ID: {app_id}")
    api.post(f"Max app path: {app_path}")

    # Use Path object
    p = api.Path()
    p.set_from_id(default_id)
    api.post(f"Path object ID: {p.get_id()}")
    api.post(f"Path object path: {p.get_path()}")


def example_write_and_read():
    """Write then read back a file"""
    api.post("=== Write and Read Back Example ===")

    path_id = api.path_getdefault()
    filename = "pktpy_roundtrip_test.txt"

    # Write data
    try:
        fh = api.path_createsysfile(filename, path_id, "TEXT")

        write_data = "Test data 1234567890\nSecond line\nThird line"
        bytes_written = api.sysfile_write(fh, write_data)
        api.post(f"Wrote {bytes_written} bytes")

        api.path_closesysfile(fh)

    except Exception as e:
        api.error(f"Write error: {e}")
        return

    # Read it back
    try:
        fh = api.path_opensysfile(filename, path_id, api.PATH_READ_PERM)

        read_data = api.sysfile_readtextfile(fh)
        api.post(f"Read back {len(read_data)} bytes")
        api.post(f"Content matches: {read_data == write_data}")

        api.path_closesysfile(fh)

    except Exception as e:
        api.error(f"Read error: {e}")
        return

    # Clean up
    try:
        api.path_deletefile(filename, path_id)
        api.post(f"Deleted {filename}")
    except Exception as e:
        api.post(f"Could not delete file: {e}")


def run_all_examples():
    """Run all examples"""
    api.post("=" * 60)
    api.post("Path and File I/O Examples")
    api.post("=" * 60)
    api.post("")

    example_path_info()
    api.post("")

    example_locate_file()
    api.post("")

    example_path_conversion()
    api.post("")

    example_write_textfile()
    api.post("")

    example_write_and_read()
    api.post("")

    # Optional: requires README.md in search path
    # example_read_textfile()
    # example_file_chunks()
    # example_file_seeking()

    api.post("=" * 60)
    api.post("Examples complete!")


# Run examples when this file is executed
if __name__ == "__main__":
    run_all_examples()
