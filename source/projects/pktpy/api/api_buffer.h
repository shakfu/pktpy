// api_buffer.h
// Buffer wrapper for pktpy API

#ifndef API_BUFFER_H
#define API_BUFFER_H

#include "api_common.h"


// ----------------------------------------------------------------------------
// Buffer wrapper - Wrapper for MSP buffer~ objects

typedef struct {
    t_buffer_ref* buffer_ref;
    bool owns_ref;
} BufferObject;

static bool Buffer__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    BufferObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(BufferObject));
    wrapper->buffer_ref = NULL;
    wrapper->owns_ref = false;
    return true;
}

static bool Buffer__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Buffer__del__(void* self) {
    BufferObject* wrapper = (BufferObject*)self;
    if (wrapper->owns_ref && wrapper->buffer_ref) {
        object_free(wrapper->buffer_ref);
        wrapper->buffer_ref = NULL;
    }
}

static bool Buffer__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->buffer_ref) {
        t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
        if (obj) {
            t_buffer_info info;
            buffer_getinfo(obj, &info);
            snprintf(buf, sizeof(buf), "Buffer(name='%s', frames=%ld, channels=%ld)",
                     info.b_name ? info.b_name->s_name : "unknown",
                     info.b_frames, info.b_nchans);
        } else {
            snprintf(buf, sizeof(buf), "Buffer(no object)");
        }
    } else {
        snprintf(buf, sizeof(buf), "Buffer(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Method: ref_new(owner_ptr, name)
// Create a new buffer reference
static bool Buffer_ref_new(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    BufferObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);   // owner pointer
    PY_CHECK_ARG_TYPE(2, tp_str);   // buffer name

    py_i64 owner_ptr = py_toint(py_arg(1));
    const char* name_str = py_tostr(py_arg(2));

    t_object* owner = (t_object*)owner_ptr;
    t_symbol* name = gensym(name_str);

    // Free existing ref if we own it
    if (self->owns_ref && self->buffer_ref) {
        object_free(self->buffer_ref);
    }

    self->buffer_ref = buffer_ref_new(owner, name);
    self->owns_ref = true;

    py_newnone(py_retval());
    return true;
}

// Method: ref_set(name)
// Change buffer reference to different buffer
static bool Buffer_ref_set(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    BufferObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null - call ref_new() first");
    }

    const char* name_str = py_tostr(py_arg(1));
    t_symbol* name = gensym(name_str);

    buffer_ref_set(self->buffer_ref, name);

    py_newnone(py_retval());
    return true;
}

// Method: exists()
// Check if buffer exists
static bool Buffer_exists(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        py_newbool(py_retval(), false);
        return true;
    }

    t_atom_long exists = buffer_ref_exists(self->buffer_ref);
    py_newbool(py_retval(), exists != 0);
    return true;
}

// Method: getobject()
// Get buffer object pointer
static bool Buffer_getobject(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);

    if (!obj) {
        py_newnone(py_retval());
        return true;
    }

    py_newint(py_retval(), (py_i64)obj);
    return true;
}

// Method: getinfo()
// Get buffer information as dictionary
static bool Buffer_getinfo(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    t_buffer_info info;
    t_max_err err = buffer_getinfo(obj, &info);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get buffer info");
    }

    // Return as Python dict (using list for simplicity)
    py_newlistn(py_retval(), 10);

    py_Ref item0 = py_list_getitem(py_retval(), 0);
    py_newstr(item0, info.b_name ? info.b_name->s_name : "");

    py_Ref item1 = py_list_getitem(py_retval(), 1);
    py_newint(item1, (py_i64)info.b_samples);

    py_Ref item2 = py_list_getitem(py_retval(), 2);
    py_newint(item2, info.b_frames);

    py_Ref item3 = py_list_getitem(py_retval(), 3);
    py_newint(item3, info.b_nchans);

    py_Ref item4 = py_list_getitem(py_retval(), 4);
    py_newint(item4, info.b_size);

    py_Ref item5 = py_list_getitem(py_retval(), 5);
    py_newfloat(item5, info.b_sr);

    py_Ref item6 = py_list_getitem(py_retval(), 6);
    py_newint(item6, info.b_modtime);

    // Add convenience fields
    py_Ref item7 = py_list_getitem(py_retval(), 7);
    py_newstr(item7, "name");

    py_Ref item8 = py_list_getitem(py_retval(), 8);
    py_newstr(item8, "frames");

    py_Ref item9 = py_list_getitem(py_retval(), 9);
    py_newstr(item9, "channels");

    return true;
}

// Method: lock()
// Lock buffer for safe access
static bool Buffer_lock(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    t_max_err err = buffer_lock(obj);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to lock buffer");
    }

    py_newnone(py_retval());
    return true;
}

// Method: unlock()
// Unlock buffer
static bool Buffer_unlock(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    t_max_err err = buffer_unlock(obj);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to unlock buffer");
    }

    py_newnone(py_retval());
    return true;
}

// Method: locksamples()
// Lock and get pointer to samples
static bool Buffer_locksamples(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    float* samples = buffer_locksamples(obj);

    if (!samples) {
        py_newnone(py_retval());
        return true;
    }

    py_newint(py_retval(), (py_i64)samples);
    return true;
}

// Method: unlocksamples()
// Unlock samples
static bool Buffer_unlocksamples(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    buffer_unlocksamples(obj);

    py_newnone(py_retval());
    return true;
}

// Method: getchannelcount()
static bool Buffer_getchannelcount(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    t_atom_long count = buffer_getchannelcount(obj);
    py_newint(py_retval(), count);
    return true;
}

// Method: getframecount()
static bool Buffer_getframecount(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    t_atom_long count = buffer_getframecount(obj);
    py_newint(py_retval(), count);
    return true;
}

// Method: getsamplerate()
static bool Buffer_getsamplerate(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    t_atom_float sr = buffer_getsamplerate(obj);
    py_newfloat(py_retval(), sr);
    return true;
}

// Method: setdirty()
static bool Buffer_setdirty(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    t_max_err err = buffer_setdirty(obj);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to set dirty flag");
    }

    py_newnone(py_retval());
    return true;
}

// Method: getfilename()
static bool Buffer_getfilename(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    t_symbol* filename = buffer_getfilename(obj);

    if (!filename || filename == gensym("")) {
        py_newnone(py_retval());
        return true;
    }

    py_newstr(py_retval(), filename->s_name);
    return true;
}

// Method: peek(frame, channel)
// Read a single sample value
static bool Buffer_peek(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    BufferObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);  // frame
    PY_CHECK_ARG_TYPE(2, tp_int);  // channel

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    long frame = (long)py_toint(py_arg(1));
    long channel = (long)py_toint(py_arg(2));

    // Get buffer info
    t_buffer_info info;
    buffer_getinfo(obj, &info);

    // Validate indices
    if (frame < 0 || frame >= info.b_frames) {
        return IndexError("Frame index out of range");
    }
    if (channel < 0 || channel >= info.b_nchans) {
        return IndexError("Channel index out of range");
    }

    // Lock and read
    float* samples = buffer_locksamples(obj);
    if (!samples) {
        return RuntimeError("Failed to lock buffer samples");
    }

    // Calculate index (samples are interleaved)
    long index = frame * info.b_nchans + channel;
    float value = samples[index];

    buffer_unlocksamples(obj);

    py_newfloat(py_retval(), value);
    return true;
}

// Method: poke(frame, channel, value)
// Write a single sample value
static bool Buffer_poke(int argc, py_Ref argv) {
    PY_CHECK_ARGC(4);
    BufferObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);    // frame
    PY_CHECK_ARG_TYPE(2, tp_int);    // channel
    PY_CHECK_ARG_TYPE(3, tp_float);  // value

    if (!self->buffer_ref) {
        return RuntimeError("Buffer reference is null");
    }

    t_buffer_obj* obj = buffer_ref_getobject(self->buffer_ref);
    if (!obj) {
        return RuntimeError("Buffer object does not exist");
    }

    long frame = (long)py_toint(py_arg(1));
    long channel = (long)py_toint(py_arg(2));
    float value = (float)py_tofloat(py_arg(3));

    // Get buffer info
    t_buffer_info info;
    buffer_getinfo(obj, &info);

    // Validate indices
    if (frame < 0 || frame >= info.b_frames) {
        return IndexError("Frame index out of range");
    }
    if (channel < 0 || channel >= info.b_nchans) {
        return IndexError("Channel index out of range");
    }

    // Lock and write
    float* samples = buffer_locksamples(obj);
    if (!samples) {
        return RuntimeError("Failed to lock buffer samples");
    }

    // Calculate index (samples are interleaved)
    long index = frame * info.b_nchans + channel;
    samples[index] = value;

    buffer_unlocksamples(obj);
    buffer_setdirty(obj);

    py_newnone(py_retval());
    return true;
}

// Method: is_null()
static bool Buffer_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->buffer_ref == NULL);
    return true;
}

// Method: pointer()
static bool Buffer_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    BufferObject* self = py_touserdata(py_arg(0));
    py_newint(py_retval(), (py_i64)self->buffer_ref);
    return true;
}



#endif // API_BUFFER_H
