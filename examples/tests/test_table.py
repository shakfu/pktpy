"""
Example: Using the Table API wrapper in Max/MSP

This demonstrates how to use the api.Table class to work with Max table objects.
Tables are named integer arrays that are lighter than buffer~ objects.

Setup in Max:
1. Create a [table mytable 100] object
2. Create a [pktpy] object
3. Load or execute this code

Usage:
- Tables must be bound before use with table.bind(name)
- Use refresh() if the table might have been recreated
- Access values with get()/set() or Python array syntax []
- Tables are integer-only (longs)
"""

import api

def table_basic_example():
    """Basic table operations"""
    api.post("=== Basic Table Example ===")

    # Create and bind to a table
    t = api.Table()

    # Bind to a table named 'mytable' (must exist in Max)
    if not t.bind("mytable"):
        api.error("Could not bind to table 'mytable'")
        return

    api.post(f"Bound to table: {t.name()}")
    api.post(f"Table size: {t.size()}")

    # Set some values
    t.set(0, 100)
    t.set(1, 200)
    t.set(2, 300)

    # Get values
    api.post(f"t[0] = {t.get(0)}")
    api.post(f"t[1] = {t.get(1)}")
    api.post(f"t[2] = {t.get(2)}")

    # Python array syntax also works
    t[5] = 500
    api.post(f"t[5] = {t[5]}")


def table_fill_example():
    """Fill table with pattern"""
    api.post("=== Fill Table Example ===")

    t = api.Table()
    if not t.bind("mytable"):
        api.error("Could not bind to table 'mytable'")
        return

    # Fill entire table with zeros
    t.fill(0)
    api.post("Filled table with zeros")

    # Create a ramp pattern using from_list
    ramp = list(range(50))
    count = t.from_list(ramp)
    api.post(f"Wrote {count} values from list")

    # Read back as list
    values = t.to_list()
    api.post(f"First 10 values: {values[:10]}")


def table_waveform_example():
    """Generate a simple waveform in a table"""
    api.post("=== Waveform Example ===")

    import math

    t = api.Table()
    if not t.bind("mytable"):
        api.error("Could not bind to table 'mytable'")
        return

    size = t.size()

    # Generate a sine wave (scaled to 0-1000)
    for i in range(size):
        phase = (i / size) * 2 * math.pi
        value = int((math.sin(phase) * 500) + 500)
        t[i] = value

    api.post(f"Generated sine wave with {size} samples")


def table_copy_example():
    """Copy data between tables"""
    api.post("=== Copy Table Example ===")

    # Create two tables
    src = api.Table()
    dst = api.Table()

    # Bind to source and destination
    if not src.bind("table_src"):
        api.error("Could not bind to table 'table_src'")
        return

    if not dst.bind("table_dst"):
        api.error("Could not bind to table 'table_dst'")
        return

    # Fill source with a pattern
    src.from_list(list(range(100)))

    # Copy a portion to destination
    # Copy 20 values from src[30:] to dst[10:]
    count = dst.copy_from(src, src_offset=30, dst_offset=10, count=20)
    api.post(f"Copied {count} values between tables")

    # Verify
    api.post(f"dst[10] = {dst[10]} (should be 30)")
    api.post(f"dst[29] = {dst[29]} (should be 49)")


def table_iteration_example():
    """Iterate over table values"""
    api.post("=== Iteration Example ===")

    t = api.Table()
    if not t.bind("mytable"):
        api.error("Could not bind to table 'mytable'")
        return

    # Set some values
    t.fill(0)
    for i in range(10):
        t[i] = i * 10

    # Read and process values
    values = []
    for i in range(10):
        values.append(t[i])

    api.post(f"Values: {values}")

    # Calculate statistics
    sum_val = sum(values)
    avg_val = sum_val / len(values)
    max_val = max(values)

    api.post(f"Sum: {sum_val}, Avg: {avg_val}, Max: {max_val}")


def table_length_example():
    """Using len() with tables"""
    api.post("=== Length Example ===")

    t = api.Table()
    if not t.bind("mytable"):
        api.error("Could not bind to table 'mytable'")
        return

    # Python len() works
    size = len(t)
    api.post(f"Table length: {size}")

    # Fill all elements using len()
    for i in range(len(t)):
        t[i] = i % 256


def run_all_examples():
    """Run all examples"""
    api.post("=" * 60)
    api.post("Table API Examples")
    api.post("=" * 60)

    table_basic_example()
    api.post("")

    table_fill_example()
    api.post("")

    table_waveform_example()
    api.post("")

    table_length_example()
    api.post("")

    table_iteration_example()
    api.post("")

    # Copy example requires two tables
    # table_copy_example()

    api.post("=" * 60)
    api.post("Examples complete!")


# Run examples when this file is executed
if __name__ == "__main__":
    run_all_examples()
