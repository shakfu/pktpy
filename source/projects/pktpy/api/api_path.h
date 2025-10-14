// api_path.h
// Path and File I/O wrapper for pktpy API

#ifndef API_PATH_H
#define API_PATH_H

#include "api_common.h"
#include "ext_path.h"
#include "ext_sysfile.h"

// ----------------------------------------------------------------------------
// Path wrapper - File system path and file I/O operations
//
// Max uses Path IDs (short integers) to identify file system locations.
// This wrapper provides Python-friendly access to Max's file API.

typedef struct {
    short path_id;
    char path_string[MAX_PATH_CHARS];
    bool has_path;
} PathObject;

static bool Path__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    PathObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(PathObject));
    wrapper->path_id = 0;
    wrapper->path_string[0] = '\0';
    wrapper->has_path = false;
    return true;
}

static bool Path__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Path__del__(void* self) {
    // No cleanup needed - path IDs are managed by Max
}

static bool Path__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PathObject* self = py_touserdata(py_arg(0));

    char buf[512];
    if (self->has_path) {
        snprintf(buf, sizeof(buf), "Path(id=%d, path='%s')",
                 self->path_id, self->path_string);
    } else {
        snprintf(buf, sizeof(buf), "Path(no path)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// ----------------------------------------------------------------------------
// Module-level path functions

// api.path_getdefault() -> int
// Get default search path ID
static bool api_path_getdefault(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    short path_id = path_getdefault();
    py_newint(py_retval(), path_id);
    return true;
}

// api.path_setdefault(path_id, recursive=False)
// Set default search path
static bool api_path_setdefault(int argc, py_Ref argv) {
    if (argc < 1 || argc > 2) {
        return TypeError("path_setdefault() takes 1-2 arguments, got %d", argc);
    }
    PY_CHECK_ARG_TYPE(0, tp_int);

    short path_id = (short)py_toint(py_arg(0));
    short recursive = 0;

    if (argc >= 2 && py_isbool(py_arg(1))) {
        recursive = py_tobool(py_arg(1)) ? 1 : 0;
    }

    path_setdefault(path_id, recursive);
    py_newnone(py_retval());
    return true;
}

// api.path_getapppath() -> int
// Get Max application path ID
static bool api_path_getapppath(int argc, py_Ref argv) {
    PY_CHECK_ARGC(0);
    short path_id = path_getapppath();
    py_newint(py_retval(), path_id);
    return true;
}

// api.locatefile_extended(filename, typelist=None) -> (path_id, filetype) or None
// Locate a file in Max search path
static bool api_locatefile_extended(int argc, py_Ref argv) {
    if (argc < 1 || argc > 2) {
        return TypeError("locatefile_extended() takes 1-2 arguments, got %d", argc);
    }
    PY_CHECK_ARG_TYPE(0, tp_str);

    // Copy filename to mutable buffer (API may modify it)
    char filename[MAX_FILENAME_CHARS];
    const char* input_name = py_tostr(py_arg(0));
    strncpy_zero(filename, input_name, MAX_FILENAME_CHARS);

    short path_id;
    t_fourcc outtype;
    t_fourcc typelist[TYPELIST_SIZE];
    short numtypes = 0;

    // Parse optional typelist
    if (argc >= 2 && py_islist(py_arg(1))) {
        py_Ref list = py_arg(1);
        int list_len = py_list_len(list);
        numtypes = (list_len > TYPELIST_SIZE) ? TYPELIST_SIZE : list_len;

        for (int i = 0; i < numtypes; i++) {
            py_Ref item = py_list_getitem(list, i);
            if (py_isstr(item)) {
                const char* typestr = py_tostr(item);
                // Convert 4-char string to fourcc
                if (strlen(typestr) == 4) {
                    typelist[i] = ((t_fourcc)typestr[0] << 24) |
                                  ((t_fourcc)typestr[1] << 16) |
                                  ((t_fourcc)typestr[2] << 8) |
                                  ((t_fourcc)typestr[3]);
                } else {
                    typelist[i] = 0;
                }
            } else {
                typelist[i] = 0;
            }
        }
    }

    // Search for file
    short result = locatefile_extended(filename, &path_id, &outtype,
                                       numtypes > 0 ? typelist : NULL, numtypes);

    if (result != 0) {
        // File not found
        py_newnone(py_retval());
        return true;
    }

    // Return tuple (filename, path_id, filetype)
    py_newlistn(py_retval(), 3);
    py_Ref item0 = py_list_getitem(py_retval(), 0);
    py_Ref item1 = py_list_getitem(py_retval(), 1);
    py_Ref item2 = py_list_getitem(py_retval(), 2);

    py_newstr(item0, filename);
    py_newint(item1, path_id);

    // Convert fourcc to string
    char typebuf[5];
    typebuf[0] = (outtype >> 24) & 0xFF;
    typebuf[1] = (outtype >> 16) & 0xFF;
    typebuf[2] = (outtype >> 8) & 0xFF;
    typebuf[3] = outtype & 0xFF;
    typebuf[4] = '\0';
    py_newstr(item2, typebuf);

    return true;
}

// api.path_toabsolutesystempath(path_id, filename) -> str
// Convert path ID + filename to absolute system path
static bool api_path_toabsolutesystempath(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_str);

    short path_id = (short)py_toint(py_arg(0));
    const char* filename = py_tostr(py_arg(1));

    char out_path[MAX_PATH_CHARS];
    t_max_err err = path_toabsolutesystempath(path_id, filename, out_path);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to convert to absolute path");
    }

    py_newstr(py_retval(), out_path);
    return true;
}

// api.path_nameconform(src, style, type) -> str
// Convert path string to specified style
static bool api_path_nameconform(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_int);

    const char* src = py_tostr(py_arg(0));
    long style = (long)py_toint(py_arg(1));
    long type = (long)py_toint(py_arg(2));

    char dst[MAX_PATH_CHARS];
    short result = path_nameconform(src, dst, style, type);

    if (result != 0) {
        return RuntimeError("Failed to conform path name");
    }

    py_newstr(py_retval(), dst);
    return true;
}

// api.path_opensysfile(filename, path_id, perm) -> filehandle (int)
// Open a file for reading/writing
static bool api_path_opensysfile(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_int);

    const char* filename = py_tostr(py_arg(0));
    short path_id = (short)py_toint(py_arg(1));
    short perm = (short)py_toint(py_arg(2));

    t_filehandle fh;
    short result = path_opensysfile(filename, path_id, &fh, perm);

    if (result != 0) {
        return RuntimeError("Failed to open file");
    }

    py_newint(py_retval(), (py_i64)fh);
    return true;
}

// api.path_createsysfile(filename, path_id, filetype) -> filehandle (int)
// Create a new file
static bool api_path_createsysfile(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_str);
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_str);

    const char* filename = py_tostr(py_arg(0));
    short path_id = (short)py_toint(py_arg(1));
    const char* typestr = py_tostr(py_arg(2));

    // Convert 4-char string to fourcc
    t_fourcc filetype = 0;
    if (strlen(typestr) == 4) {
        filetype = ((t_fourcc)typestr[0] << 24) |
                   ((t_fourcc)typestr[1] << 16) |
                   ((t_fourcc)typestr[2] << 8) |
                   ((t_fourcc)typestr[3]);
    }

    t_filehandle fh;
    short result = path_createsysfile(filename, path_id, filetype, &fh);

    if (result != 0) {
        return RuntimeError("Failed to create file");
    }

    py_newint(py_retval(), (py_i64)fh);
    return true;
}

// api.path_closesysfile(filehandle)
// Close an open file
static bool api_path_closesysfile(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);

    t_filehandle fh = (t_filehandle)py_toint(py_arg(0));
    sysfile_close(fh);

    py_newnone(py_retval());
    return true;
}

// api.sysfile_read(filehandle, count) -> bytes
// Read bytes from file
static bool api_sysfile_read(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_int);

    t_filehandle fh = (t_filehandle)py_toint(py_arg(0));
    t_ptr_size count = (t_ptr_size)py_toint(py_arg(1));

    // Allocate buffer
    char* buffer = (char*)sysmem_newptr(count + 1);
    if (!buffer) {
        return RuntimeError("Failed to allocate read buffer");
    }

    t_ptr_size actual = count;
    t_max_err err = sysfile_read(fh, &actual, buffer);

    if (err != MAX_ERR_NONE) {
        sysmem_freeptr(buffer);
        return RuntimeError("Failed to read from file");
    }

    buffer[actual] = '\0';
    py_newstr(py_retval(), buffer);

    sysmem_freeptr(buffer);
    return true;
}

// api.sysfile_write(filehandle, data)
// Write string/bytes to file
static bool api_sysfile_write(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_str);

    t_filehandle fh = (t_filehandle)py_toint(py_arg(0));
    const char* data = py_tostr(py_arg(1));
    t_ptr_size count = strlen(data);

    t_ptr_size actual = count;
    t_max_err err = sysfile_write(fh, &actual, data);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to write to file");
    }

    py_newint(py_retval(), actual);
    return true;
}

// api.sysfile_geteof(filehandle) -> int
// Get end-of-file position
static bool api_sysfile_geteof(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);

    t_filehandle fh = (t_filehandle)py_toint(py_arg(0));
    t_ptr_size eof;

    t_max_err err = sysfile_geteof(fh, &eof);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get EOF position");
    }

    py_newint(py_retval(), eof);
    return true;
}

// api.sysfile_seteof(filehandle, pos)
// Set end-of-file position
static bool api_sysfile_seteof(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_int);

    t_filehandle fh = (t_filehandle)py_toint(py_arg(0));
    t_ptr_size eof = (t_ptr_size)py_toint(py_arg(1));

    t_max_err err = sysfile_seteof(fh, eof);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set EOF position");
    }

    py_newnone(py_retval());
    return true;
}

// api.sysfile_getpos(filehandle) -> int
// Get current file position
static bool api_sysfile_getpos(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PY_CHECK_ARG_TYPE(0, tp_int);

    t_filehandle fh = (t_filehandle)py_toint(py_arg(0));
    t_ptr_size pos;

    t_max_err err = sysfile_getpos(fh, &pos);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get file position");
    }

    py_newint(py_retval(), pos);
    return true;
}

// api.sysfile_setpos(filehandle, pos, mode)
// Set file position
static bool api_sysfile_setpos(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    PY_CHECK_ARG_TYPE(0, tp_int);
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_int);

    t_filehandle fh = (t_filehandle)py_toint(py_arg(0));
    t_ptr_size pos = (t_ptr_size)py_toint(py_arg(1));
    t_sysfile_pos_mode mode = (t_sysfile_pos_mode)py_toint(py_arg(2));

    t_max_err err = sysfile_setpos(fh, mode, pos);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set file position");
    }

    py_newnone(py_retval());
    return true;
}

// api.sysfile_readtextfile(filehandle, maxsize=65536) -> str
// Read entire text file
static bool api_sysfile_readtextfile(int argc, py_Ref argv) {
    if (argc < 1 || argc > 2) {
        return TypeError("sysfile_readtextfile() takes 1-2 arguments, got %d", argc);
    }
    PY_CHECK_ARG_TYPE(0, tp_int);

    t_filehandle fh = (t_filehandle)py_toint(py_arg(0));
    t_ptr_size maxsize = 65536;

    if (argc >= 2 && py_isint(py_arg(1))) {
        maxsize = (t_ptr_size)py_toint(py_arg(1));
    }

    t_handle h = NULL;
    t_max_err err = sysfile_readtextfile(fh, h, maxsize, TEXT_LB_NATIVE);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to read text file");
    }

    if (h == NULL) {
        py_newstr(py_retval(), "");
        return true;
    }

    t_ptr_size size = sysmem_handlesize(h);
    char* text = *h;

    py_newstr(py_retval(), text);

    sysmem_freehandle(h);
    return true;
}

// api.path_deletefile(filename, path_id)
// Delete a file
static bool api_path_deletefile(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(0, tp_str);
    PY_CHECK_ARG_TYPE(1, tp_int);

    const char* filename = py_tostr(py_arg(0));
    short path_id = (short)py_toint(py_arg(1));

    short result = path_deletefile(filename, path_id);

    if (result != 0) {
        return RuntimeError("Failed to delete file");
    }

    py_newnone(py_retval());
    return true;
}

// ----------------------------------------------------------------------------
// Path object methods

// Method: set_from_id(path_id)
// Set path from a path ID
static bool Path_set_from_id(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PathObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    self->path_id = (short)py_toint(py_arg(1));
    self->has_path = true;

    // Convert to absolute path string
    t_max_err err = path_toabsolutesystempath(self->path_id, "", self->path_string);
    if (err != MAX_ERR_NONE) {
        self->path_string[0] = '\0';
    }

    py_newnone(py_retval());
    return true;
}

// Method: get_id() -> int
// Get path ID
static bool Path_get_id(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PathObject* self = py_touserdata(py_arg(0));

    if (!self->has_path) {
        py_newnone(py_retval());
        return true;
    }

    py_newint(py_retval(), self->path_id);
    return true;
}

// Method: get_path() -> str
// Get path string
static bool Path_get_path(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PathObject* self = py_touserdata(py_arg(0));

    if (!self->has_path || self->path_string[0] == '\0') {
        py_newnone(py_retval());
        return true;
    }

    py_newstr(py_retval(), self->path_string);
    return true;
}

// Method: is_set() -> bool
// Check if path is set
static bool Path_is_set(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    PathObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->has_path);
    return true;
}

#endif // API_PATH_H
