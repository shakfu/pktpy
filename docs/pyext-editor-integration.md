# pyext Text Editor Integration

## Overview

The `pyext` external includes integrated text editor support, allowing you to edit Python scripts directly within Max. This provides a workflow similar to Max's `js` object.

## Implementation Date

October 14, 2025 (added after initial pyext implementation)

## Features

### Double-Click to Edit

Double-clicking a `pyext` object opens a text editor window displaying the loaded Python script:

```
[pyext simple.py]  ← double-click here
```

The editor window shows:
- Title: "pyext: scriptname.py"
- Full content of the Python script
- Syntax-aware text editing

### Save Behavior

**Save (Cmd-S):**
- Writes changes to the script file on disk
- Optionally reloads the script (controlled by `run_on_save` attribute)
- Default: save without reload

**Close Window:**
- Writes changes to the script file on disk
- Optionally reloads the script (controlled by `run_on_close` attribute)
- Default: save and reload

### Manual Commands

**Reload:**
```
[reload( -> [pyext simple.py]
```
Manually reload the script from disk, discarding any unsaved editor changes.

**Read:**
```
[read scriptname.py( -> [pyext simple.py]
```
Load a different script file into the editor buffer.

## Architecture

### Data Structure

Added to `t_pyext` struct:
```c
t_object* code_editor;    // Code editor object
char** code;              // Handle to code buffer
long code_size;           // Length of code buffer
long run_on_save;         // Reload script on editor save
long run_on_close;        // Reload script on editor close
```

### Method Handlers

**pyext_dblclick()** - Opens the text editor
- Creates new `jed` (text editor) object if doesn't exist
- Loads script file content via `pyext_doread()`
- Sets editor title to "pyext: scriptname.py"
- Makes editor visible

**pyext_edclose()** - Handles editor close
- Saves text buffer
- Optionally writes to file and reloads script (if `run_on_close == 1`)
- Clears editor object reference

**pyext_edsave()** - Handles editor save (Cmd-S)
- Saves text buffer
- Optionally writes to file and reloads script (if `run_on_save == 1`)
- Editor remains open

**pyext_okclose()** - Configures close behavior
- Sets result to 3 (no dialog on close)

**pyext_read()** - Loads script into editor
- Defers to `pyext_doread()` for actual file reading

**pyext_doread()** - Deferred file read
- Locates file in Max search path
- Reads text content into code buffer
- Used by double-click to populate editor

### Message Registration

In `ext_main()`:
```c
class_addmethod(c, (method)pyext_dblclick, "dblclick", A_CANT, 0);
class_addmethod(c, (method)pyext_edclose,  "edclose",  A_CANT, 0);
class_addmethod(c, (method)pyext_edsave,   "edsave",   A_CANT, 0);
class_addmethod(c, (method)pyext_okclose,  "okclose",  A_CANT, 0);
class_addmethod(c, (method)pyext_read,     "read",     A_DEFSYM, 0);
```

The `A_CANT` flag indicates these are internal methods called by Max, not user-callable.

## Workflow Comparison

### Similar to js Object

| Feature | js Object | pyext Object |
|---------|-----------|--------------|
| Double-click to edit | Yes | Yes |
| Live code editing | Yes | Yes |
| Auto-reload on close | Yes (default) | Yes (default) |
| Save without reload | Yes (Cmd-S) | Yes (Cmd-S) |
| Script title in editor | Yes | Yes |
| Syntax highlighting | JavaScript | Plain text |

### Different from pktpy Object

The `pktpy` object has a text editor for arbitrary Python code execution, while `pyext` has a text editor for editing the loaded script file. Key differences:

| Feature | pktpy | pyext |
|---------|-------|-------|
| Editor purpose | Scratch pad for code execution | Script file editor |
| File binding | None (scratch buffer) | Bound to script file |
| Save behavior | Saves to scratch buffer | Saves to script file |
| Default on close | Execute code | Reload script |
| Use case | Interactive REPL-like | Script development |

## File Operations

### Save Flow

1. User edits in text editor
2. User saves (Cmd-S) or closes window
3. `pyext_edsave()` or `pyext_edclose()` called
4. Text copied to internal buffer (`x->code`)
5. Text written to file via `path_createsysfile()` and `sysfile_write()`
6. If `run_on_save`/`run_on_close`, `pyext_reload()` is called
7. Script reloaded and Python object re-instantiated

### Read Flow

1. User double-clicks object or sends `read` message
2. `pyext_doread()` locates file via `locatefile_extended()`
3. File read via `path_opensysfile()` and `sysfile_readtextfile()`
4. Content stored in `x->code` buffer
5. If editor open, `settext` method updates editor content

## Memory Management

- **code buffer**: Allocated with `sysmem_newhandle()`, freed in `pyext_free()`
- **editor object**: Created on demand, NULL when closed
- **file handles**: Opened and closed within same function scope
- No memory leaks on object destruction

## Future Enhancements

### Phase 1 - Attributes
Add Inspector-visible attributes:
- `run_on_save` - Boolean to control reload behavior on save
- `run_on_close` - Boolean to control reload behavior on close
- Currently hardcoded (save=0, close=1)

### Phase 2 - Syntax Highlighting
- Integrate Python syntax highlighting in editor
- Requires custom text editor or jed extensions
- May need external editor integration

### Phase 3 - Error Annotations
- Show Python errors inline in editor
- Highlight line with error
- Display exception traceback
- Similar to Max's error highlighting in gen~ code

### Phase 4 - Auto-reload on File Change
- Watch script file for external changes
- Automatically reload when modified by external editor
- Use file system notifications
- Useful for IDE-based development

### Phase 5 - Multiple Scripts
- Load multiple scripts per object
- Tab-based editor for multiple files
- Module management
- Import path configuration

## Testing

### Manual Tests

1. **Basic editing:**
   - Create `[pyext simple.py]`
   - Double-click to open editor
   - Verify script content loads
   - Edit and close
   - Verify script reloads

2. **Save behavior:**
   - Open editor
   - Edit script
   - Press Cmd-S
   - Verify file saved but script not reloaded (default)
   - Edit again and close
   - Verify script reloads

3. **Multiple instances:**
   - Create two `[pyext simple.py]` objects
   - Edit one
   - Verify both reload (they share the file)

4. **Error handling:**
   - Open editor
   - Introduce syntax error
   - Close editor
   - Verify error shown in Max console
   - Verify object still functional

5. **Read command:**
   - Create `[pyext simple.py]`
   - Send `[read counter.py(`
   - Double-click
   - Verify counter.py content loaded

## Known Limitations

1. **No syntax highlighting** - Editor shows plain text
2. **No undo/redo across sessions** - Editor undo buffer cleared on close
3. **No line numbers** - Can't jump to specific line number
4. **Attributes not exposed** - Can't change `run_on_save`/`run_on_close` behavior
5. **Single file only** - Can't edit imported modules

## Conclusion

The text editor integration makes `pyext` practical for real-world development by enabling:
- Quick iteration on Python scripts
- Visual development workflow within Max
- No need to switch between Max and external editor (though you still can)
- Familiar workflow for Max users (similar to `js` object)

This brings `pyext` to feature parity with Max's built-in scriptable objects in terms of editing experience, while providing the power of Python and full Max API access.
