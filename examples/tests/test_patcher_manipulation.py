"""
Test script for patcher manipulation functions

This script demonstrates usage of the Patcher and Patchline classes for
advanced patcher scripting, including traversing boxes and lines.
Run this inside the pktpy Max external using 'execfile' or 'load'.

NOTE: These tests require a patcher context to run properly.
"""

import api


def test_patcher_info():
    """Test patcher information methods"""
    api.post("=== Testing Patcher Information ===\n")

    api.post("New Patcher methods for file information:\n")
    api.post("\n")

    api.post("1. get_name()\n")
    api.post("   - Get the patcher's name\n")
    api.post("   - Returns string\n")
    api.post("   - Example: name = patcher.get_name()\n")
    api.post("\n")

    api.post("2. get_filepath()\n")
    api.post("   - Get the full file path\n")
    api.post("   - Returns string or None if unsaved\n")
    api.post("   - Example: path = patcher.get_filepath()\n")
    api.post("\n")

    api.post("3. get_filename()\n")
    api.post("   - Get just the filename\n")
    api.post("   - Returns string or None if unsaved\n")
    api.post("   - Example: filename = patcher.get_filename()\n")

    api.post("\n")


def test_get_boxes():
    """Test getting all boxes in a patcher"""
    api.post("=== Testing Patcher.get_boxes() ===\n")

    api.post("Method signature:\n")
    api.post("  boxes = patcher.get_boxes()\n")
    api.post("\n")

    api.post("Returns a Linklist containing all boxes in the patcher.\n")
    api.post("Each item in the list is a box pointer.\n")
    api.post("\n")

    api.post("Usage example:\n")
    api.post("  boxes = patcher.get_boxes()\n")
    api.post("  api.post(f'Found {len(boxes)} boxes\\n')\n")
    api.post("  \n")
    api.post("  for i in range(len(boxes)):\n")
    api.post("      box_ptr = boxes[i]\n")
    api.post("      # Wrap in Box object to query\n")
    api.post("      box = api.Box()\n")
    api.post("      box.wrap(box_ptr)\n")
    api.post("      classname = box.classname()\n")
    api.post("      api.post(f'  [{i}] {classname}\\n')\n")

    api.post("\n")


def test_get_lines():
    """Test getting all patchlines"""
    api.post("=== Testing Patcher.get_lines() ===\n")

    api.post("Method signature:\n")
    api.post("  lines = patcher.get_lines()\n")
    api.post("\n")

    api.post("Returns a Linklist containing all patchlines (patch cords).\n")
    api.post("Each item is a patchline pointer that can be wrapped.\n")
    api.post("\n")

    api.post("Usage example:\n")
    api.post("  lines = patcher.get_lines()\n")
    api.post("  api.post(f'Found {len(lines)} connections\\n')\n")
    api.post("  \n")
    api.post("  for i in range(len(lines)):\n")
    api.post("      line_ptr = lines[i]\n")
    api.post("      # Wrap in Patchline object\n")
    api.post("      line = api.Patchline()\n")
    api.post("      line.wrap(line_ptr)\n")
    api.post("      # Query connection info\n")
    api.post("      outlet = line.get_outletnum()\n")
    api.post("      inlet = line.get_inletnum()\n")
    api.post("      api.post(f'  Connection: outlet {outlet} -> inlet {inlet}\\n')\n")

    api.post("\n")


def test_patchline_class():
    """Test Patchline wrapper class"""
    api.post("=== Testing Patchline Class ===\n")

    api.post("The Patchline class wraps Max patchline (patch cord) objects.\n")
    api.post("\n")

    api.post("Creation:\n")
    api.post("  line = api.Patchline()\n")
    api.post("  line.wrap(line_pointer)\n")
    api.post("\n")

    api.post("Connection Query Methods:\n")
    api.post("\n")

    api.post("1. get_box1()\n")
    api.post("   - Get the source box (where line originates)\n")
    api.post("   - Returns Box object\n")
    api.post("   - Example: source_box = line.get_box1()\n")
    api.post("\n")

    api.post("2. get_box2()\n")
    api.post("   - Get the destination box (where line connects to)\n")
    api.post("   - Returns Box object\n")
    api.post("   - Example: dest_box = line.get_box2()\n")
    api.post("\n")

    api.post("3. get_outletnum()\n")
    api.post("   - Get outlet number of source box\n")
    api.post("   - Returns integer (0-indexed)\n")
    api.post("   - Example: outlet = line.get_outletnum()\n")
    api.post("\n")

    api.post("4. get_inletnum()\n")
    api.post("   - Get inlet number of destination box\n")
    api.post("   - Returns integer (0-indexed)\n")
    api.post("   - Example: inlet = line.get_inletnum()\n")
    api.post("\n")

    api.post("Geometric Methods:\n")
    api.post("\n")

    api.post("5. get_startpoint()\n")
    api.post("   - Get [x, y] coordinates of line start\n")
    api.post("   - Returns list of two floats\n")
    api.post("   - Example: [x, y] = line.get_startpoint()\n")
    api.post("\n")

    api.post("6. get_endpoint()\n")
    api.post("   - Get [x, y] coordinates of line end\n")
    api.post("   - Returns list of two floats\n")
    api.post("   - Example: [x, y] = line.get_endpoint()\n")
    api.post("\n")

    api.post("Visibility Methods:\n")
    api.post("\n")

    api.post("7. get_hidden()\n")
    api.post("   - Check if line is hidden\n")
    api.post("   - Returns boolean\n")
    api.post("   - Example: hidden = line.get_hidden()\n")
    api.post("\n")

    api.post("8. set_hidden(hidden)\n")
    api.post("   - Set line visibility\n")
    api.post("   - Takes boolean argument\n")
    api.post("   - Example: line.set_hidden(True)  # Hide the line\n")
    api.post("\n")

    api.post("Traversal Methods:\n")
    api.post("\n")

    api.post("9. get_nextline()\n")
    api.post("   - Get next patchline in linked list\n")
    api.post("   - Returns Patchline object or None\n")
    api.post("   - Example: next_line = line.get_nextline()\n")

    api.post("\n")


def test_patcher_analysis():
    """Show patcher analysis pattern"""
    api.post("=== Patcher Analysis Pattern ===\n")

    api.post("Example: Analyze patcher structure\n")
    api.post("---\n")
    api.post("def analyze_patcher(patcher):\n")
    api.post("    # Get basic info\n")
    api.post("    name = patcher.get_name()\n")
    api.post("    filename = patcher.get_filename()\n")
    api.post("    \n")
    api.post("    api.post(f'Analyzing: {name}\\n')\n")
    api.post("    if filename:\n")
    api.post("        api.post(f'File: {filename}\\n')\n")
    api.post("    \n")
    api.post("    # Count boxes\n")
    api.post("    boxes = patcher.get_boxes()\n")
    api.post("    api.post(f'Total boxes: {len(boxes)}\\n')\n")
    api.post("    \n")
    api.post("    # Count by class\n")
    api.post("    classes = {}\n")
    api.post("    for i in range(len(boxes)):\n")
    api.post("        box = api.Box()\n")
    api.post("        box.wrap(boxes[i])\n")
    api.post("        classname = box.classname()\n")
    api.post("        classes[classname] = classes.get(classname, 0) + 1\n")
    api.post("    \n")
    api.post("    api.post('Box types:\\n')\n")
    api.post("    for classname, count in classes.items():\n")
    api.post("        api.post(f'  {classname}: {count}\\n')\n")
    api.post("    \n")
    api.post("    # Count connections\n")
    api.post("    lines = patcher.get_lines()\n")
    api.post("    api.post(f'Total connections: {len(lines)}\\n')\n")

    api.post("\n")


def test_connection_analysis():
    """Show connection analysis pattern"""
    api.post("=== Connection Analysis Pattern ===\n")

    api.post("Example: Analyze patch cord connections\n")
    api.post("---\n")
    api.post("def analyze_connections(patcher):\n")
    api.post("    lines = patcher.get_lines()\n")
    api.post("    \n")
    api.post("    api.post(f'Analyzing {len(lines)} connections:\\n\\n')\n")
    api.post("    \n")
    api.post("    for i in range(len(lines)):\n")
    api.post("        line = api.Patchline()\n")
    api.post("        line.wrap(lines[i])\n")
    api.post("        \n")
    api.post("        # Get connection info\n")
    api.post("        box1 = line.get_box1()\n")
    api.post("        box2 = line.get_box2()\n")
    api.post("        outlet = line.get_outletnum()\n")
    api.post("        inlet = line.get_inletnum()\n")
    api.post("        \n")
    api.post("        # Get box names\n")
    api.post("        class1 = box1.classname()\n")
    api.post("        class2 = box2.classname()\n")
    api.post("        \n")
    api.post("        # Check visibility\n")
    api.post("        hidden = line.get_hidden()\n")
    api.post("        visibility = 'hidden' if hidden else 'visible'\n")
    api.post("        \n")
    api.post("        api.post(f'[{i}] {class1}[{outlet}] -> '\n")
    api.post("                f'{class2}[{inlet}] ({visibility})\\n')\n")

    api.post("\n")


def test_line_visibility():
    """Show line visibility control"""
    api.post("=== Line Visibility Control ===\n")

    api.post("Example: Hide/show specific connections\n")
    api.post("---\n")
    api.post("def hide_noisy_connections(patcher):\n")
    api.post("    \"\"\"Hide connections from metro and random objects\"\"\"\n")
    api.post("    lines = patcher.get_lines()\n")
    api.post("    \n")
    api.post("    noisy_classes = ['metro', 'random']\n")
    api.post("    hidden_count = 0\n")
    api.post("    \n")
    api.post("    for i in range(len(lines)):\n")
    api.post("        line = api.Patchline()\n")
    api.post("        line.wrap(lines[i])\n")
    api.post("        \n")
    api.post("        box1 = line.get_box1()\n")
    api.post("        classname = box1.classname()\n")
    api.post("        \n")
    api.post("        if classname in noisy_classes:\n")
    api.post("            line.set_hidden(True)\n")
    api.post("            hidden_count += 1\n")
    api.post("    \n")
    api.post("    api.post(f'Hidden {hidden_count} noisy connections\\n')\n")
    api.post("\n")

    api.post("Example: Show all hidden connections\n")
    api.post("---\n")
    api.post("def show_all_connections(patcher):\n")
    api.post("    lines = patcher.get_lines()\n")
    api.post("    shown_count = 0\n")
    api.post("    \n")
    api.post("    for i in range(len(lines)):\n")
    api.post("        line = api.Patchline()\n")
    api.post("        line.wrap(lines[i])\n")
    api.post("        \n")
    api.post("        if line.get_hidden():\n")
    api.post("            line.set_hidden(False)\n")
    api.post("            shown_count += 1\n")
    api.post("    \n")
    api.post("    api.post(f'Showed {shown_count} hidden connections\\n')\n")

    api.post("\n")


def test_traversal_patterns():
    """Show traversal patterns"""
    api.post("=== Traversal Patterns ===\n")

    api.post("Pattern 1: Using get_boxes() with Linklist\n")
    api.post("---\n")
    api.post("boxes = patcher.get_boxes()\n")
    api.post("for i in range(len(boxes)):\n")
    api.post("    box = api.Box()\n")
    api.post("    box.wrap(boxes[i])\n")
    api.post("    # Process box\n")
    api.post("\n")

    api.post("Pattern 2: Using get_firstobject() (original method)\n")
    api.post("---\n")
    api.post("box = patcher.get_firstobject()\n")
    api.post("while box and not box.is_null():\n")
    api.post("    classname = box.classname()\n")
    api.post("    # Process box\n")
    api.post("    # Note: Need to use object_method to get next\n")
    api.post("\n")

    api.post("Pattern 3: Using get_lines() with Linklist\n")
    api.post("---\n")
    api.post("lines = patcher.get_lines()\n")
    api.post("for i in range(len(lines)):\n")
    api.post("    line = api.Patchline()\n")
    api.post("    line.wrap(lines[i])\n")
    api.post("    # Process line\n")
    api.post("\n")

    api.post("Pattern 4: Using get_nextline() (linked list)\n")
    api.post("---\n")
    api.post("first_line_ptr = patcher.get_firstline()\n")
    api.post("line = api.Patchline()\n")
    api.post("line.wrap(first_line_ptr)\n")
    api.post("\n")
    api.post("while line and not line.is_null():\n")
    api.post("    # Process line\n")
    api.post("    line = line.get_nextline()\n")

    api.post("\n")


def test_use_cases():
    """Show practical use cases"""
    api.post("=== Use Cases for Patcher Manipulation ===\n")

    api.post("1. Patcher Documentation\n")
    api.post("   - Generate documentation of patcher structure\n")
    api.post("   - List all objects and connections\n")
    api.post("   - Export as text or JSON\n")
    api.post("\n")

    api.post("2. Connection Validation\n")
    api.post("   - Check for missing connections\n")
    api.post("   - Validate signal flow\n")
    api.post("   - Find disconnected objects\n")
    api.post("\n")

    api.post("3. Visual Cleanup\n")
    api.post("   - Hide/show specific connection types\n")
    api.post("   - Simplify visual appearance\n")
    api.post("   - Focus on specific signal paths\n")
    api.post("\n")

    api.post("4. Patcher Analysis\n")
    api.post("   - Count objects by type\n")
    api.post("   - Analyze complexity metrics\n")
    api.post("   - Find specific patterns\n")
    api.post("\n")

    api.post("5. Automated Testing\n")
    api.post("   - Verify patcher structure\n")
    api.post("   - Check for required objects\n")
    api.post("   - Validate connections\n")
    api.post("\n")

    api.post("6. Dependency Mapping\n")
    api.post("   - Trace signal paths\n")
    api.post("   - Build dependency graphs\n")
    api.post("   - Find all outputs of an object\n")

    api.post("\n")


def test_complete_example():
    """Show complete practical example"""
    api.post("=== Complete Example: Connection Inspector ===\n")

    api.post("class ConnectionInspector:\n")
    api.post("    \"\"\"Inspect and document patcher connections\"\"\"\n")
    api.post("    \n")
    api.post("    def __init__(self, patcher):\n")
    api.post("        self.patcher = patcher\n")
    api.post("        self.boxes = {}\n")
    api.post("        self.connections = []\n")
    api.post("    \n")
    api.post("    def scan(self):\n")
    api.post("        \"\"\"Scan patcher and build data structures\"\"\"\n")
    api.post("        # Build box index\n")
    api.post("        boxes = self.patcher.get_boxes()\n")
    api.post("        for i in range(len(boxes)):\n")
    api.post("            box = api.Box()\n")
    api.post("            box.wrap(boxes[i])\n")
    api.post("            self.boxes[boxes[i]] = {\n")
    api.post("                'index': i,\n")
    api.post("                'class': box.classname(),\n")
    api.post("                'rect': box.get_rect()\n")
    api.post("            }\n")
    api.post("        \n")
    api.post("        # Build connection list\n")
    api.post("        lines = self.patcher.get_lines()\n")
    api.post("        for i in range(len(lines)):\n")
    api.post("            line = api.Patchline()\n")
    api.post("            line.wrap(lines[i])\n")
    api.post("            \n")
    api.post("            box1_ptr = line.get_box1().pointer()\n")
    api.post("            box2_ptr = line.get_box2().pointer()\n")
    api.post("            \n")
    api.post("            self.connections.append({\n")
    api.post("                'from': self.boxes[box1_ptr]['index'],\n")
    api.post("                'to': self.boxes[box2_ptr]['index'],\n")
    api.post("                'outlet': line.get_outletnum(),\n")
    api.post("                'inlet': line.get_inletnum(),\n")
    api.post("                'hidden': line.get_hidden()\n")
    api.post("            })\n")
    api.post("    \n")
    api.post("    def report(self):\n")
    api.post("        \"\"\"Generate report\"\"\"\n")
    api.post("        api.post(f'Patcher: {self.patcher.get_name()}\\n')\n")
    api.post("        api.post(f'Boxes: {len(self.boxes)}\\n')\n")
    api.post("        api.post(f'Connections: {len(self.connections)}\\n')\n")
    api.post("        \n")
    api.post("        # Show connections\n")
    api.post("        for conn in self.connections:\n")
    api.post("            from_idx = conn['from']\n")
    api.post("            to_idx = conn['to']\n")
    api.post("            api.post(f'  Box{from_idx}[{conn[\"outlet\"]}] -> '\n")
    api.post("                    f'Box{to_idx}[{conn[\"inlet\"]}]\\n')\n")
    api.post("\n")
    api.post("# Usage:\n")
    api.post("# inspector = ConnectionInspector(patcher)\n")
    api.post("# inspector.scan()\n")
    api.post("# inspector.report()\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Patcher Manipulation\n")
    api.post("====================================\n\n")

    try:
        test_patcher_info()
        test_get_boxes()
        test_get_lines()
        test_patchline_class()
        test_patcher_analysis()
        test_connection_analysis()
        test_line_visibility()
        test_traversal_patterns()
        test_use_cases()
        test_complete_example()

        api.post("====================================\n")
        api.post("Documentation completed!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
