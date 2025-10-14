"""
Example: Using the Inlet API in Max/MSP

This demonstrates how to create additional inlets using the api module.

Note: This is示例代码 showing the API usage patterns. In practice, inlets
should be created during object initialization in the pktpy C code.
These examples show the Python API calls that would be used.
"""

import api

# Example 1: Query existing inlets
def example_query_inlets(owner_ptr):
    """Query and inspect object's inlets"""
    api.post("=== Query Inlets Example ===")

    # Get inlet count
    count = api.inlet_count(owner_ptr)
    api.post(f"Object has {count} inlets")

    # Inspect each inlet
    for i in range(count):
        inlet = api.inlet_nth(owner_ptr, i)
        if inlet:
            api.post(f"Inlet {i}: {inlet}")
            api.post(f"  Pointer: {inlet.pointer()}")
            api.post(f"  Is proxy: {inlet.is_proxy()}")


# Example 2: Understanding proxy_getinlet()
def example_proxy_identification(owner_ptr):
    """Demonstrate identifying which inlet received a message"""
    api.post("=== Proxy Identification Example ===")

    # This would be called in a message handler
    inlet_num = api.proxy_getinlet(owner_ptr)

    if inlet_num == 0:
        api.post("Message received in leftmost inlet")
    elif inlet_num == 1:
        api.post("Message received in inlet 1")
    elif inlet_num == 2:
        api.post("Message received in inlet 2")
    else:
        api.post(f"Message received in inlet {inlet_num}")


# Example 3: Simulated multi-inlet object with proxies
class SimulatedMultiInletObject:
    """
    Example showing how a multi-inlet object would be structured.

    In practice, this would be integrated into the pktpy C code,
    but this shows the Python API patterns.
    """

    def __init__(self, owner_ptr):
        self.owner_ptr = owner_ptr
        self.inlet_num = 0  # Storage for proxy system

        api.post("Creating multi-inlet object with 3 inlets")

        # Note: In real usage, these would be created in C code
        # during object instantiation
        try:
            # Create proxies in reverse order
            self.proxy3 = api.proxy_new(owner_ptr, 3, id(self.inlet_num))
            self.proxy2 = api.proxy_new(owner_ptr, 2, id(self.inlet_num))
            self.proxy1 = api.proxy_new(owner_ptr, 1, id(self.inlet_num))

            api.post("Created 3 proxy inlets successfully")
        except Exception as e:
            api.error(f"Failed to create inlets: {e}")

        # Storage for values from each inlet
        self.values = [0, 0, 0, 0]  # Index 0-3 for each inlet

    def handle_int(self, value):
        """Handle integer input from any inlet"""
        inlet = api.proxy_getinlet(self.owner_ptr)
        self.values[inlet] = value
        api.post(f"Inlet {inlet} received: {value}")
        api.post(f"Current values: {self.values}")

    def handle_bang(self):
        """Handle bang from any inlet"""
        inlet = api.proxy_getinlet(self.owner_ptr)
        api.post(f"Bang in inlet {inlet}")

        if inlet == 0:
            # Leftmost inlet - output sum
            total = sum(self.values)
            api.post(f"Sum: {total}")
        else:
            # Other inlets - show stored value
            api.post(f"Stored value: {self.values[inlet]}")

    def cleanup(self):
        """Clean up proxies"""
        if hasattr(self, 'proxy1'):
            self.proxy1.delete()
        if hasattr(self, 'proxy2'):
            self.proxy2.delete()
        if hasattr(self, 'proxy3'):
            self.proxy3.delete()
        api.post("Cleaned up proxy inlets")


# Example 4: Typed inlet patterns
def example_typed_inlet_pattern():
    """
    Show the pattern for typed inlets.

    Note: These create actual inlets and would need to be created
    during object initialization in C code.
    """
    api.post("=== Typed Inlet Pattern ===")
    api.post("")
    api.post("Pattern for integer inlets (intin):")
    api.post("  - Creates inlet that sends 'inN' messages")
    api.post("  - Inlet 1 -> in1() method called")
    api.post("  - Inlet 2 -> in2() method called")
    api.post("  - etc.")
    api.post("")
    api.post("Pattern for float inlets (floatin):")
    api.post("  - Creates inlet that sends 'ftN' messages")
    api.post("  - Inlet 1 -> ft1() method called")
    api.post("  - Inlet 2 -> ft2() method called")
    api.post("  - etc.")
    api.post("")
    api.post("Example handlers in Python:")
    api.post("""
    def in1(self, value):
        '''Called when int arrives in inlet 1'''
        api.post(f"in1: {value}")

    def ft2(self, value):
        '''Called when float arrives in inlet 2'''
        api.post(f"ft2: {value}")
    """)


# Example 5: General-purpose inlet pattern
def example_general_inlet_pattern():
    """Show general-purpose inlet pattern"""
    api.post("=== General-Purpose Inlet Pattern ===")
    api.post("")
    api.post("inlet_new(owner_ptr, None):")
    api.post("  - Receives ANY message")
    api.post("  - Use for flexible inlet behavior")
    api.post("")
    api.post("inlet_new(owner_ptr, 'bang'):")
    api.post("  - Only receives 'bang' messages")
    api.post("  - Use for specific message types")
    api.post("")
    api.post("inlet_new(owner_ptr, 'trigger'):")
    api.post("  - Only receives 'trigger' messages")
    api.post("  - Custom message handlers")


# Example 6: Inlet method demonstration
def example_inlet_methods(owner_ptr):
    """Demonstrate Inlet object methods"""
    api.post("=== Inlet Methods Example ===")

    # Get an existing inlet
    inlet = api.inlet_nth(owner_ptr, 0)
    if not inlet:
        api.post("No inlet found")
        return

    # Query methods
    api.post(f"Inlet representation: {inlet}")
    api.post(f"Pointer: {inlet.pointer()}")
    api.post(f"Is null: {inlet.is_null()}")
    api.post(f"Is proxy: {inlet.is_proxy()}")
    api.post(f"Inlet number: {inlet.get_num()}")


# Example 7: Creating inlets with error handling
def example_inlet_creation_safe(owner_ptr):
    """Show safe inlet creation with error handling"""
    api.post("=== Safe Inlet Creation Example ===")

    inlets = []

    try:
        # Try to create 3 inlets
        for i in range(3, 0, -1):  # Reverse order
            api.post(f"Creating inlet {i}...")
            inlet = api.intin(owner_ptr, i)
            inlets.append(inlet)
            api.post(f"  Success!")

        api.post(f"Created {len(inlets)} inlets successfully")

    except ValueError as e:
        api.error(f"Invalid inlet number: {e}")
    except RuntimeError as e:
        api.error(f"Failed to create inlet: {e}")
    except Exception as e:
        api.error(f"Unexpected error: {e}")


# Example 8: Advanced proxy pattern with state
class AdvancedProxyObject:
    """Advanced example with per-inlet state"""

    def __init__(self, owner_ptr):
        self.owner = owner_ptr
        self.inlet_num = 0

        # Per-inlet configuration
        self.inlet_configs = {
            0: {"name": "trigger", "multiplier": 1.0},
            1: {"name": "input_a", "multiplier": 2.0},
            2: {"name": "input_b", "multiplier": 3.0},
        }

        # Create proxies
        self.proxies = []
        for i in range(2, 0, -1):  # Create 1 and 2 (0 is automatic)
            try:
                proxy = api.proxy_new(owner_ptr, i, id(self.inlet_num))
                self.proxies.append(proxy)
            except Exception as e:
                api.error(f"Failed to create proxy {i}: {e}")

    def handle_float(self, value):
        """Handle float with inlet-specific processing"""
        inlet = api.proxy_getinlet(self.owner)
        config = self.inlet_configs.get(inlet, {})

        name = config.get("name", f"inlet_{inlet}")
        multiplier = config.get("multiplier", 1.0)

        result = value * multiplier
        api.post(f"{name}: {value} × {multiplier} = {result}")

        return result

    def handle_list(self, values):
        """Handle list with inlet identification"""
        inlet = api.proxy_getinlet(self.owner)
        api.post(f"List in inlet {inlet}: {values}")


def run_all_examples():
    """Run all examples (with mock owner_ptr)"""
    api.post("=" * 60)
    api.post("Inlet API Examples")
    api.post("=" * 60)
    api.post("")

    # Note: These examples use 0 as owner_ptr for demonstration
    # In real usage, you would pass an actual object pointer
    mock_owner = 0

    api.post("Note: These examples demonstrate the API patterns.")
    api.post("In practice, inlets are created during object initialization.")
    api.post("")

    example_typed_inlet_pattern()
    api.post("")

    example_general_inlet_pattern()
    api.post("")

    # The following would work with a real object pointer:
    # example_query_inlets(real_owner_ptr)
    # example_proxy_identification(real_owner_ptr)
    # example_inlet_methods(real_owner_ptr)
    # example_inlet_creation_safe(real_owner_ptr)

    api.post("=" * 60)
    api.post("Examples complete!")
    api.post("")
    api.post("For working examples, inlets must be created in")
    api.post("the pktpy C code during object initialization.")


# Run examples when this file is executed
if __name__ == "__main__":
    run_all_examples()
