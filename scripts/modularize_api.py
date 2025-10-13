#!/usr/bin/env python3
"""
Modularize pktpy_api.h into separate files per wrapper class.

This script extracts each wrapper class and related code from the monolithic
pktpy_api.h file into separate, maintainable header files in api/ directory.
"""

import re
import os
from pathlib import Path

# Base directory
BASE_DIR = Path(__file__).parent.parent / "source" / "projects" / "pktpy"
API_DIR = BASE_DIR / "api"
ORIGINAL_FILE = BASE_DIR / "pktpy_api.h"
BACKUP_FILE = BASE_DIR / "pktpy_api.h.backup"

# Wrapper definitions to extract
WRAPPERS = [
    "Symbol",
    "Atom",
    "AtomArray",
    "Dictionary",
    "Object",
    "Box",
    "Patcher",
    "Patchline",
    "Hashtab",
    "Linklist",
    "Clock",
    "Outlet",
    "Buffer",
]

def read_file(filepath):
    """Read entire file content."""
    with open(filepath, 'r', encoding='utf-8') as f:
        return f.read()

def write_file(filepath, content):
    """Write content to file."""
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)

def extract_section(content, start_marker, end_marker):
    """Extract content between two markers."""
    start_idx = content.find(start_marker)
    if start_idx == -1:
        return None, content

    end_idx = content.find(end_marker, start_idx + len(start_marker))
    if end_idx == -1:
        return None, content

    extracted = content[start_idx:end_idx]
    remaining = content[:start_idx] + content[end_idx:]

    return extracted, remaining

def extract_wrapper(content, wrapper_name):
    """Extract a wrapper class and its methods."""
    # Find the wrapper section
    start_marker = f"// {wrapper_name} wrapper"

    # Find next wrapper or major section
    start_idx = content.find(start_marker)
    if start_idx == -1:
        print(f"Warning: Could not find wrapper: {wrapper_name}")
        return None, content

    # Find the end (next major section marker)
    end_markers = [
        "\n// ----------------------------------------------------------------------------\n// ",
        "\n\n// ----------------------------------------------------------------------------\n// ",
    ]

    end_idx = -1
    for end_marker in end_markers:
        potential_end = content.find(end_marker, start_idx + len(start_marker))
        if potential_end != -1:
            if end_idx == -1 or potential_end < end_idx:
                end_idx = potential_end

    if end_idx == -1:
        end_idx = len(content)

    extracted = content[start_idx:end_idx]
    remaining = content[:start_idx] + content[end_idx:]

    return extracted, remaining

def create_common_header():
    """Create api_common.h with shared infrastructure."""
    content = """// api_common.h
// Common infrastructure for pktpy API wrappers

#ifndef API_COMMON_H
#define API_COMMON_H

// ----------------------------------------------------------------------------
// Max and MSP includes

#include "ext.h"
#include "ext_obex.h"
#include "ext_atomarray.h"
#include "ext_dictionary.h"
#include "jpatcher_api.h"
#include "ext_hashtab.h"
#include "ext_linklist.h"
#include "ext_buffer.h"

// ----------------------------------------------------------------------------
// PocketPy include

#include "pocketpy.h"

// ----------------------------------------------------------------------------
// Global type declarations

// Store type IDs globally so functions can access them
// Note: Declared as extern here, defined in pktpy_api.h
extern py_Type g_symbol_type;
extern py_Type g_atom_type;
extern py_Type g_atomarray_type;
extern py_Type g_dictionary_type;
extern py_Type g_object_type;
extern py_Type g_patcher_type;
extern py_Type g_box_type;
extern py_Type g_hashtab_type;
extern py_Type g_linklist_type;
extern py_Type g_buffer_type;
extern py_Type g_clock_type;
extern py_Type g_outlet_type;
extern py_Type g_patchline_type;

// ----------------------------------------------------------------------------
// Forward declarations for utility functions

static bool py_to_atom(py_Ref py_val, t_atom* atom);
static bool atom_to_py(t_atom* atom);

#endif // API_COMMON_H
"""
    return content

def create_wrapper_header(wrapper_name, content):
    """Create a wrapper header file."""
    guard_name = f"API_{wrapper_name.upper()}_H"
    filename = f"api_{wrapper_name.lower()}.h"

    header = f"""// {filename}
// {wrapper_name} wrapper for pktpy API

#ifndef {guard_name}
#define {guard_name}

#include "api_common.h"

{content}

#endif // {guard_name}
"""
    return header

def main():
    print("Starting pktpy_api.h modularization...")
    print(f"Reading from: {ORIGINAL_FILE}")

    # Backup original file
    if not BACKUP_FILE.exists():
        print(f"Creating backup: {BACKUP_FILE}")
        import shutil
        shutil.copy(ORIGINAL_FILE, BACKUP_FILE)

    # Read original content
    content = read_file(ORIGINAL_FILE)
    original_lines = len(content.split('\n'))
    print(f"Original file: {original_lines} lines")

    # Create api_common.h
    print("\nCreating api_common.h...")
    common_content = create_common_header()
    write_file(API_DIR / "api_common.h", common_content)
    print(f"  Created: api/api_common.h")

    # Extract each wrapper
    print("\nExtracting wrappers...")
    remaining_content = content
    extracted_wrappers = []

    for wrapper in WRAPPERS:
        print(f"  Extracting {wrapper}...")
        wrapper_content, remaining_content = extract_wrapper(remaining_content, wrapper)

        if wrapper_content:
            # Create wrapper header
            header_content = create_wrapper_header(wrapper, wrapper_content)
            filename = f"api_{wrapper.lower()}.h"
            write_file(API_DIR / filename, header_content)

            wrapper_lines = len(wrapper_content.split('\n'))
            print(f"    Created: api/{filename} ({wrapper_lines} lines)")
            extracted_wrappers.append(wrapper)
        else:
            print(f"    Warning: Could not extract {wrapper}")

    print(f"\nSuccessfully extracted {len(extracted_wrappers)} wrappers")
    print("\nTo complete the refactoring:")
    print("1. Review generated files in api/ directory")
    print("2. Extract remaining functions (api_functions.h)")
    print("3. Extract module initialization (api_module_init.h)")
    print("4. Update pktpy_api.h to include all new headers")
    print("5. Run 'make build' to test")
    print(f"\nOriginal backup saved at: {BACKUP_FILE}")

if __name__ == "__main__":
    main()
