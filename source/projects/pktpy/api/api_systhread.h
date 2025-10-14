// api_systhread.h
// Systhread wrapper for pktpy API
//
// Thread management for Max/MSP
// WARNING: Use with caution - improper thread management can crash Max

#ifndef API_SYSTHREAD_H
#define API_SYSTHREAD_H

#include "api_common.h"
#include "ext_systhread.h"

// ----------------------------------------------------------------------------
// SysThread wrapper

typedef struct {
    t_systhread thread;
    py_Ref callback;    // Python callback function
    py_Ref user_data;   // Optional user data
    bool is_running;
    unsigned int result;
} SysThreadObject;

// ----------------------------------------------------------------------------
// SysMutex wrapper

typedef struct {
    t_systhread_mutex mutex;
    bool is_locked;
} SysMutexObject;

// ----------------------------------------------------------------------------
// Thread callback wrapper

static void* systhread_callback_wrapper(void* arg) {
    SysThreadObject* obj = (SysThreadObject*)arg;

    if (obj == NULL || obj->callback == NULL) {
        return NULL;
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

    // Get return value if it's an integer
    if (py_isint(py_retval())) {
        obj->result = (unsigned int)py_toint(py_retval());
    }

    obj->is_running = false;
    return NULL;
}

// ----------------------------------------------------------------------------
// SysThread destructor

static void SysThread__del__(void* self) {
    SysThreadObject* obj = (SysThreadObject*)self;

    // Wait for thread to finish if still running
    if (obj->is_running && obj->thread) {
        unsigned int ret;
        systhread_join(obj->thread, &ret);
    }
}

// ----------------------------------------------------------------------------
// SysThread constructor

static bool SysThread__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    SysThreadObject* obj = py_newobject(py_retval(), cls, 0, sizeof(SysThreadObject));
    obj->thread = NULL;
    obj->callback = NULL;
    obj->user_data = NULL;
    obj->is_running = false;
    obj->result = 0;
    return true;
}

static bool SysThread__init__(int argc, py_Ref argv) {
    SysThreadObject* self = py_touserdata(py_arg(0));

    if (argc < 2) {
        return TypeError("SysThread() requires at least 1 argument (callback), got %d", argc - 1);
    }

    // First argument is the callback function
    py_Ref callback = py_arg(1);
    if (!py_callable(callback)) {
        return TypeError("SysThread(): callback must be callable");
    }

    // Optional second argument is user data
    py_Ref user_data = NULL;
    if (argc >= 3) {
        user_data = py_arg(2);
    }

    // Store callback and user data using py_setslot
    self->callback = callback;
    py_setslot(py_arg(0), 0, callback);  // Keep callback alive

    if (user_data != NULL) {
        self->user_data = user_data;
        py_setslot(py_arg(0), 1, user_data);  // Keep user_data alive
    }

    py_newnone(py_retval());
    return true;
}

// ----------------------------------------------------------------------------
// SysThread string representation

static bool SysThread__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysThreadObject* self = py_touserdata(py_arg(0));

    char buf[256];
    snprintf(buf, sizeof(buf), "SysThread(running=%s, result=%u)",
             self->is_running ? "True" : "False", self->result);

    py_newstr(py_retval(), buf);
    return true;
}

// ----------------------------------------------------------------------------
// SysThread methods

// Start the thread
static bool SysThread_start(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysThreadObject* self = py_touserdata(py_arg(0));

    if (self->is_running) {
        return ValueError("SysThread: thread is already running");
    }

    if (self->callback == NULL) {
        return ValueError("SysThread: no callback function set");
    }

    // Create and start thread
    t_max_err err = systhread_create(
        (method)systhread_callback_wrapper,
        self,
        0,  // stacksize (0 = default)
        0,  // priority (0 = default)
        0,  // flags
        &self->thread
    );

    if (err != MAX_ERR_NONE) {
        return ValueError("Failed to create thread");
    }

    self->is_running = true;
    py_newnone(py_retval());
    return true;
}

// Join the thread (wait for completion)
static bool SysThread_join(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysThreadObject* self = py_touserdata(py_arg(0));

    if (!self->is_running || self->thread == NULL) {
        return ValueError("SysThread: thread is not running");
    }

    unsigned int ret;
    t_max_err err = systhread_join(self->thread, &ret);

    if (err != MAX_ERR_NONE) {
        return ValueError("Failed to join thread");
    }

    self->result = ret;
    self->is_running = false;

    py_newint(py_retval(), ret);
    return true;
}

// Check if thread is running
static bool SysThread_is_running(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysThreadObject* self = py_touserdata(py_arg(0));

    py_newbool(py_retval(), self->is_running);
    return true;
}

// Get result
static bool SysThread_get_result(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysThreadObject* self = py_touserdata(py_arg(0));

    py_newint(py_retval(), self->result);
    return true;
}

// Sleep
static bool SysThread_sleep(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    PY_CHECK_ARG_TYPE(1, tp_int);

    unsigned int milliseconds = (unsigned int)py_toint(py_arg(1));
    systhread_sleep(milliseconds);

    py_newnone(py_retval());
    return true;
}

// ----------------------------------------------------------------------------
// SysMutex destructor

static void SysMutex__del__(void* self) {
    SysMutexObject* obj = (SysMutexObject*)self;

    if (obj->mutex) {
        // Unlock if locked
        if (obj->is_locked) {
            systhread_mutex_unlock(obj->mutex);
        }
        systhread_mutex_free(obj->mutex);
        obj->mutex = NULL;
    }
}

// ----------------------------------------------------------------------------
// SysMutex constructor

static bool SysMutex__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    SysMutexObject* obj = py_newobject(py_retval(), cls, 0, sizeof(SysMutexObject));
    obj->mutex = NULL;
    obj->is_locked = false;
    return true;
}

static bool SysMutex__init__(int argc, py_Ref argv) {
    SysMutexObject* self = py_touserdata(py_arg(0));

    // Create mutex
    t_max_err err = systhread_mutex_new(&self->mutex, 0);
    if (err != MAX_ERR_NONE || self->mutex == NULL) {
        return ValueError("Failed to create mutex");
    }

    self->is_locked = false;

    py_newnone(py_retval());
    return true;
}

// ----------------------------------------------------------------------------
// SysMutex string representation

static bool SysMutex__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysMutexObject* self = py_touserdata(py_arg(0));

    char buf[256];
    snprintf(buf, sizeof(buf), "SysMutex(%p, locked=%s)",
             (void*)self->mutex, self->is_locked ? "True" : "False");

    py_newstr(py_retval(), buf);
    return true;
}

// ----------------------------------------------------------------------------
// SysMutex methods

// Lock mutex
static bool SysMutex_lock(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysMutexObject* self = py_touserdata(py_arg(0));

    if (self->mutex == NULL) {
        return ValueError("SysMutex: mutex is NULL");
    }

    t_max_err err = systhread_mutex_lock(self->mutex);
    if (err != MAX_ERR_NONE) {
        return ValueError("Failed to lock mutex");
    }

    self->is_locked = true;
    py_newnone(py_retval());
    return true;
}

// Unlock mutex
static bool SysMutex_unlock(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysMutexObject* self = py_touserdata(py_arg(0));

    if (self->mutex == NULL) {
        return ValueError("SysMutex: mutex is NULL");
    }

    if (!self->is_locked) {
        return ValueError("SysMutex: mutex is not locked");
    }

    t_max_err err = systhread_mutex_unlock(self->mutex);
    if (err != MAX_ERR_NONE) {
        return ValueError("Failed to unlock mutex");
    }

    self->is_locked = false;
    py_newnone(py_retval());
    return true;
}

// Try lock (non-blocking)
static bool SysMutex_trylock(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysMutexObject* self = py_touserdata(py_arg(0));

    if (self->mutex == NULL) {
        return ValueError("SysMutex: mutex is NULL");
    }

    t_max_err err = systhread_mutex_trylock(self->mutex);
    bool success = (err == MAX_ERR_NONE);

    if (success) {
        self->is_locked = true;
    }

    py_newbool(py_retval(), success);
    return true;
}

// Check if mutex is locked
static bool SysMutex_is_locked(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysMutexObject* self = py_touserdata(py_arg(0));

    py_newbool(py_retval(), self->is_locked);
    return true;
}

// Get pointer (for advanced use)
static bool SysMutex_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    SysMutexObject* self = py_touserdata(py_arg(0));

    py_newint(py_retval(), (py_i64)self->mutex);
    return true;
}

#endif // API_SYSTHREAD_H
