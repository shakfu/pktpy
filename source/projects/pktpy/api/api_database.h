// api_database.h
// Database (SQLite) wrapper for pktpy API

#ifndef API_DATABASE_H
#define API_DATABASE_H

#include "api_common.h"
#include "ext_database.h"

// ----------------------------------------------------------------------------
// Database wrapper - SQLite database access from Python
//
// Max provides a SQLite database API for persistent storage and queries.
// Databases can be file-based or in-memory.

typedef struct {
    t_database* db;
    t_symbol* dbname;
    bool owns_db;
} DatabaseObject;

typedef struct {
    t_db_result* result;
    bool owns_result;
} DBResultObject;

typedef struct {
    t_db_view* view;
    t_database* parent_db;
    bool owns_view;
} DBViewObject;

// ----------------------------------------------------------------------------
// Database type

static bool Database__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    DatabaseObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(DatabaseObject));
    wrapper->db = NULL;
    wrapper->dbname = NULL;
    wrapper->owns_db = false;
    return true;
}

static bool Database__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void Database__del__(void* self) {
    DatabaseObject* wrapper = (DatabaseObject*)self;
    if (wrapper->owns_db && wrapper->db) {
        db_close(&wrapper->db);
    }
    wrapper->db = NULL;
}

static bool Database__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DatabaseObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->db && self->dbname) {
        snprintf(buf, sizeof(buf), "Database(name='%s', ptr=%p)",
                 self->dbname->s_name, self->db);
    } else {
        snprintf(buf, sizeof(buf), "Database(closed)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Method: open(name, filepath=None)
// Open or create a database
static bool Database_open(int argc, py_Ref argv) {
    if (argc < 2 || argc > 3) {
        return TypeError("open() takes 1-2 arguments, got %d", argc - 1);
    }
    DatabaseObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    const char* name_str = py_tostr(py_arg(1));
    const char* filepath = NULL;

    // Optional filepath
    if (argc >= 3 && py_isstr(py_arg(2)) && !py_isnone(py_arg(2))) {
        filepath = py_tostr(py_arg(2));
    }

    // Close existing if we own it
    if (self->owns_db && self->db) {
        db_close(&self->db);
    }

    self->dbname = gensym(name_str);
    t_max_err err = db_open(self->dbname, filepath, &self->db);

    if (err != MAX_ERR_NONE) {
        self->db = NULL;
        self->owns_db = false;
        return RuntimeError("Failed to open database");
    }

    self->owns_db = true;
    py_newnone(py_retval());
    return true;
}

// Method: close()
// Close the database
static bool Database_close(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DatabaseObject* self = py_touserdata(py_arg(0));

    if (!self->db) {
        py_newnone(py_retval());
        return true;
    }

    if (self->owns_db) {
        t_max_err err = db_close(&self->db);
        if (err != MAX_ERR_NONE) {
            return RuntimeError("Failed to close database");
        }
    }

    self->db = NULL;
    self->owns_db = false;

    py_newnone(py_retval());
    return true;
}

// Method: query(sql) -> DBResult
// Execute a SQL query
static bool Database_query(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DatabaseObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->db) {
        return RuntimeError("Database not open");
    }

    const char* sql = py_tostr(py_arg(1));
    t_db_result* result = NULL;

    t_max_err err = db_query_direct(self->db, &result, sql);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Query failed");
    }

    // Create DBResult wrapper
    DBResultObject* result_wrapper = py_newobject(py_retval(), g_dbresult_type, 0, sizeof(DBResultObject));
    result_wrapper->result = result;
    result_wrapper->owns_result = true;

    return true;
}

// Method: transaction_start()
// Begin a transaction
static bool Database_transaction_start(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DatabaseObject* self = py_touserdata(py_arg(0));

    if (!self->db) {
        return RuntimeError("Database not open");
    }

    t_max_err err = db_transaction_start(self->db);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to start transaction");
    }

    py_newnone(py_retval());
    return true;
}

// Method: transaction_end()
// Commit a transaction
static bool Database_transaction_end(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DatabaseObject* self = py_touserdata(py_arg(0));

    if (!self->db) {
        return RuntimeError("Database not open");
    }

    t_max_err err = db_transaction_end(self->db);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to end transaction");
    }

    py_newnone(py_retval());
    return true;
}

// Method: transaction_flush()
// Force flush all transactions
static bool Database_transaction_flush(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DatabaseObject* self = py_touserdata(py_arg(0));

    if (!self->db) {
        return RuntimeError("Database not open");
    }

    t_max_err err = db_transaction_flush(self->db);
    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to flush transactions");
    }

    py_newnone(py_retval());
    return true;
}

// Method: get_last_insert_id() -> int
// Get the ID of the last INSERT
static bool Database_get_last_insert_id(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DatabaseObject* self = py_touserdata(py_arg(0));

    if (!self->db) {
        return RuntimeError("Database not open");
    }

    long id;
    t_max_err err = db_query_getlastinsertid(self->db, &id);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to get last insert ID");
    }

    py_newint(py_retval(), id);
    return true;
}

// Method: create_table(tablename)
// Create a new table with primary key
static bool Database_create_table(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DatabaseObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);

    if (!self->db) {
        return RuntimeError("Database not open");
    }

    const char* tablename = py_tostr(py_arg(1));
    t_max_err err = db_query_table_new(self->db, tablename);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to create table");
    }

    py_newnone(py_retval());
    return true;
}

// Method: add_column(tablename, columnname, columntype, flags=None)
// Add a column to existing table
static bool Database_add_column(int argc, py_Ref argv) {
    if (argc < 4 || argc > 5) {
        return TypeError("add_column() takes 3-4 arguments, got %d", argc - 1);
    }
    DatabaseObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_str);
    PY_CHECK_ARG_TYPE(2, tp_str);
    PY_CHECK_ARG_TYPE(3, tp_str);

    if (!self->db) {
        return RuntimeError("Database not open");
    }

    const char* tablename = py_tostr(py_arg(1));
    const char* columnname = py_tostr(py_arg(2));
    const char* columntype = py_tostr(py_arg(3));
    const char* flags = NULL;

    if (argc >= 5 && py_isstr(py_arg(4))) {
        flags = py_tostr(py_arg(4));
    }

    t_max_err err = db_query_table_addcolumn(self->db, tablename, columnname, columntype, flags);

    if (err != MAX_ERR_NONE) {
        return RuntimeError("Failed to add column");
    }

    py_newnone(py_retval());
    return true;
}

// Method: is_open() -> bool
// Check if database is open
static bool Database_is_open(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DatabaseObject* self = py_touserdata(py_arg(0));
    py_newbool(py_retval(), self->db != NULL);
    return true;
}

// Method: pointer() -> int
// Get database pointer
static bool Database_pointer(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DatabaseObject* self = py_touserdata(py_arg(0));
    py_newint(py_retval(), (py_i64)self->db);
    return true;
}

// ----------------------------------------------------------------------------
// DBResult type

static bool DBResult__new__(int argc, py_Ref argv) {
    py_Type cls = py_totype(argv);
    DBResultObject* wrapper = py_newobject(py_retval(), cls, 0, sizeof(DBResultObject));
    wrapper->result = NULL;
    wrapper->owns_result = false;
    return true;
}

static bool DBResult__init__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_newnone(py_retval());
    return true;
}

static void DBResult__del__(void* self) {
    DBResultObject* wrapper = (DBResultObject*)self;
    if (wrapper->owns_result && wrapper->result) {
        object_free((t_object*)wrapper->result);
    }
    wrapper->result = NULL;
}

static bool DBResult__repr__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DBResultObject* self = py_touserdata(py_arg(0));

    char buf[256];
    if (self->result) {
        long numrecords = db_result_numrecords(self->result);
        long numfields = db_result_numfields(self->result);
        snprintf(buf, sizeof(buf), "DBResult(%ld records, %ld fields)",
                 numrecords, numfields);
    } else {
        snprintf(buf, sizeof(buf), "DBResult(null)");
    }
    py_newstr(py_retval(), buf);
    return true;
}

// Method: numrecords() -> int
// Get number of records
static bool DBResult_numrecords(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DBResultObject* self = py_touserdata(py_arg(0));

    if (!self->result) {
        py_newint(py_retval(), 0);
        return true;
    }

    long count = db_result_numrecords(self->result);
    py_newint(py_retval(), count);
    return true;
}

// Method: numfields() -> int
// Get number of fields
static bool DBResult_numfields(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DBResultObject* self = py_touserdata(py_arg(0));

    if (!self->result) {
        py_newint(py_retval(), 0);
        return true;
    }

    long count = db_result_numfields(self->result);
    py_newint(py_retval(), count);
    return true;
}

// Method: fieldname(index) -> str
// Get field name by index
static bool DBResult_fieldname(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DBResultObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->result) {
        return RuntimeError("Result is null");
    }

    long index = (long)py_toint(py_arg(1));
    char* name = db_result_fieldname(self->result, index);

    if (name) {
        py_newstr(py_retval(), name);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Method: get_string(record, field) -> str
// Get string value
static bool DBResult_get_string(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    DBResultObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_int);

    if (!self->result) {
        return RuntimeError("Result is null");
    }

    long record = (long)py_toint(py_arg(1));
    long field = (long)py_toint(py_arg(2));

    char* value = db_result_string(self->result, record, field);

    if (value) {
        py_newstr(py_retval(), value);
    } else {
        py_newnone(py_retval());
    }

    return true;
}

// Method: get_long(record, field) -> int
// Get integer value
static bool DBResult_get_long(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    DBResultObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_int);

    if (!self->result) {
        return RuntimeError("Result is null");
    }

    long record = (long)py_toint(py_arg(1));
    long field = (long)py_toint(py_arg(2));

    long value = db_result_long(self->result, record, field);
    py_newint(py_retval(), value);
    return true;
}

// Method: get_float(record, field) -> float
// Get float value
static bool DBResult_get_float(int argc, py_Ref argv) {
    PY_CHECK_ARGC(3);
    DBResultObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);
    PY_CHECK_ARG_TYPE(2, tp_int);

    if (!self->result) {
        return RuntimeError("Result is null");
    }

    long record = (long)py_toint(py_arg(1));
    long field = (long)py_toint(py_arg(2));

    float value = db_result_float(self->result, record, field);
    py_newfloat(py_retval(), value);
    return true;
}

// Method: get_record(index) -> list
// Get entire record as list of strings
static bool DBResult_get_record(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    DBResultObject* self = py_touserdata(py_arg(0));
    PY_CHECK_ARG_TYPE(1, tp_int);

    if (!self->result) {
        return RuntimeError("Result is null");
    }

    long record_index = (long)py_toint(py_arg(1));
    long numfields = db_result_numfields(self->result);

    // Create list
    py_newlistn(py_retval(), (int)numfields);

    for (long i = 0; i < numfields; i++) {
        py_Ref item = py_list_getitem(py_retval(), (int)i);
        char* value = db_result_string(self->result, record_index, i);
        if (value) {
            py_newstr(item, value);
        } else {
            py_newnone(item);
        }
    }

    return true;
}

// Method: to_list() -> list
// Convert all results to list of lists
static bool DBResult_to_list(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DBResultObject* self = py_touserdata(py_arg(0));

    if (!self->result) {
        py_newlistn(py_retval(), 0);
        return true;
    }

    long numrecords = db_result_numrecords(self->result);
    long numfields = db_result_numfields(self->result);

    // Create outer list
    py_newlistn(py_retval(), (int)numrecords);

    for (long r = 0; r < numrecords; r++) {
        py_Ref record = py_list_getitem(py_retval(), (int)r);
        py_newlistn(record, (int)numfields);

        for (long f = 0; f < numfields; f++) {
            py_Ref item = py_list_getitem(record, (int)f);
            char* value = db_result_string(self->result, r, f);
            if (value) {
                py_newstr(item, value);
            } else {
                py_newnone(item);
            }
        }
    }

    return true;
}

// Method: reset()
// Reset iterator to beginning
static bool DBResult_reset(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DBResultObject* self = py_touserdata(py_arg(0));

    if (!self->result) {
        return RuntimeError("Result is null");
    }

    db_result_reset(self->result);
    py_newnone(py_retval());
    return true;
}

// Method: clear()
// Clear the result
static bool DBResult_clear(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    DBResultObject* self = py_touserdata(py_arg(0));

    if (!self->result) {
        py_newnone(py_retval());
        return true;
    }

    db_result_clear(self->result);
    py_newnone(py_retval());
    return true;
}

// Method: __len__() -> int
// Get number of records
static bool DBResult__len__(int argc, py_Ref argv) {
    return DBResult_numrecords(argc, argv);
}

#endif // API_DATABASE_H
