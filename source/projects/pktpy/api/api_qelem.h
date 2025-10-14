// api_qelem.h
// Qelem wrapper for pktpy API
//
// Queue-based defer alternative for UI updates and deferred execution

#ifndef API_QELEM_H
#define API_QELEM_H

#include "api_common.h"

// ----------------------------------------------------------------------------
// Qelem wrapper

typedef struct {
    t_qelem* qelem;
    py_Ref callback;    // Python callback function
    py_Ref user_data;   // Optional user data
    bool is_set;
} QelemObject;

// ----------------------------------------------------------------------------
// Callback function

static void qelem_callback_wrapper(QelemObject* obj) {
    if (obj == NULL || obj->callback == NULL) {
        return;
    }

    // Call Python callback
    py_push(obj->callback);
    py_pushnil();  // self (no self for module function)

    // Push user data if available
    if (obj->user_data != NULL) {
        py_push(obj->user_data);
        bool ok = py_vectorcall(1, 0);
        if (!ok) {
            py_printexc();
        }
    } else {
        bool ok = py_vectorcall(0, 0);
        if (!ok) {
            py_printexc();
        }
    }

    // Clear the set flag after execution
    obj->is_set = false;
}

// ----------------------------------------------------------------------------
// Destructor

static void Qelem__del__(void* self) {
    QelemObject* obj = (QelemObject*)self;

    if (obj->qelem) {
        qelem_unset(obj->qelem);
        qelem_free(obj->qelem);
        obj->qelem = NULL;
    }
}

// ----------------------------------------------------------------------------
// Constructor

static bool Qelem__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    QelemObject* obj = py_newobject(py_retval(), cls, 0, sizeof(QelemObject));
    obj->qelem = NULL;
    obj->callback = NULL;
    obj->user_data = NULL;
    obj->is_set = false;
    return true;
}

static bool Qelem__init__(int argc, py_Ref argv) {
    QelemObject* self = py_touserdata(py_arg(0));

    if (argc < 2) {
        return TypeError("Qelem() requires at least 1 argument (callback), got %d", argc - 1);
    }

    // First argument is the callback function
    py_Ref callback = py_arg(1);
    if (!py_callable(callback)) {
        return TypeError("Qelem(): callback must be callable");
    }

    // Optional second argument is user data
    py_Ref user_data = NULL;
    if (argc >= 3) {
        user_data = py_arg(2);
    }

    // Create qelem
    self->qelem = qelem_new(self, (method)qelem_callback_wrapper);
    if (self->qelem == NULL) {
        return ValueError("Failed to create qelem");
    }

    // Store callback and user data using py_setslot to keep references
    self->callback = callback;
    py_setslot(py_arg(0), 0, callback);  // Keep callback alive

    if (user_data != NULL) {
        self->user_data = user_data;
        py_setslot(py_arg(0), 1, user_data);  // Keep user_data alive
    }
    self->is_set = false;

    py_newnone(py_retval());
    return true;
}

// ----------------------------------------------------------------------------
// String representation

static bool Qelem__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    QelemObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->qelem) {
        snprintf(buf, sizeof(buf), "Qelem(%p, set=%s)",
                 (void*)self->qelem, self->is_set ? "True" : "False");
    } else {
        snprintf(buf, sizeof(buf), "Qelem(None)");
    }

    py_newstr(py_retval(), buf);
    return true;
}

// ----------------------------------------------------------------------------
// Methods

// Set qelem (schedule for execution)
static bool Qelem_set(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    QelemObject* self = py_touserdata(py_arg(0));

    if (self->qelem == NULL) {
        return ValueError("Qelem: qelem is NULL");
    }

    qelem_set(self->qelem);
    self->is_set = true;

    py_newnone(py_retval());
    return true;
}

// Unset qelem (cancel scheduled execution)
static bool Qelem_unset(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    QelemObject* self = py_touserdata(py_arg(0));

    if (self->qelem == NULL) {
        return ValueError("Qelem: qelem is NULL");
    }

    qelem_unset(self->qelem);
    self->is_set = false;

    py_newnone(py_retval());
    return true;
}

// Check if qelem is set
static bool Qelem_is_set(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    QelemObject* self = py_touserdata(py_arg(0));

    py_newbool(py_retval(), self->is_set);
    return true;
}

// Check if qelem is null
static bool Qelem_is_null(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    QelemObject* self = py_touserdata(py_arg(0));

    py_newbool(py_retval(), self->qelem == NULL);
    return true;
}

// Get pointer (for advanced use)
static bool Qelem_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    QelemObject* self = py_touserdata(py_arg(0));

    py_newint(py_retval(), (py_i64)self->qelem);
    return true;
}

// Front - execute qelem in front of queue
static bool Qelem_front(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    QelemObject* self = py_touserdata(py_arg(0));

    if (self->qelem == NULL) {
        return ValueError("Qelem: qelem is NULL");
    }

    qelem_front(self->qelem);
    self->is_set = true;

    py_newnone(py_retval());
    return true;
}

#endif // API_QELEM_H
