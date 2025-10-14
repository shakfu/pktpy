# Database API

The `api.Database` and `api.DBResult` classes provide Python access to Max's SQLite database functionality, enabling persistent data storage and retrieval.

## Overview

Max's database API wraps SQLite, providing:

1. **Database Management**: Open/close databases, transactions, schema operations
2. **Query Execution**: Execute SQL queries with results returned as DBResult objects
3. **Result Iteration**: Access query results as records with field access
4. **Type Conversion**: Automatic conversion between SQLite types and Python types

The Database API is useful for:
- Persistent storage of configuration data
- Caching computed results between Max sessions
- Data logging and analysis
- Complex data relationships via SQL queries

## Class: `api.Database`

Wrapper for Max database objects (SQLite).

### Constructor

```python
db = api.Database()
```

Creates an uninitialized database wrapper.

### Methods

#### `open(name: str, filepath: str = None) -> None`
Open or create a database.

**Parameters:**
- `name`: Database name (symbol)
- `filepath`: Optional path to database file. If None, uses default location.

**Raises:**
- `RuntimeError` if database open fails

**Example:**
```python
db = api.Database()
db.open("mydata")  # Opens in default location
# or
db.open("mydata", "/path/to/mydata.db")  # Custom location
```

---

#### `close() -> None`
Close the database.

**Raises:**
- `RuntimeError` if close fails

**Example:**
```python
db.close()
```

---

#### `query(sql: str) -> DBResult`
Execute SQL query and return results.

**Parameters:**
- `sql`: SQL query string

**Returns:**
- `DBResult` object containing query results

**Raises:**
- `RuntimeError` if query execution fails

**Example:**
```python
result = db.query("SELECT * FROM users WHERE age > 21")
api.post(f"Found {len(result)} records")
```

---

#### `transaction_start() -> None`
Begin a database transaction.

**Raises:**
- `RuntimeError` if transaction start fails

**Remarks:**
- Transactions improve performance for bulk operations
- Must call `transaction_end()` or `transaction_flush()` to commit

**Example:**
```python
db.transaction_start()
# ... multiple inserts/updates
db.transaction_end()
```

---

#### `transaction_end() -> None`
End and commit current transaction.

**Raises:**
- `RuntimeError` if commit fails

**Example:**
```python
db.transaction_start()
for i in range(1000):
    db.query(f"INSERT INTO data VALUES ({i})")
db.transaction_end()  # Commit all at once
```

---

#### `transaction_flush() -> None`
Flush (commit) current transaction without ending it.

**Raises:**
- `RuntimeError` if flush fails

**Remarks:**
- Allows periodic commits within a long transaction

**Example:**
```python
db.transaction_start()
for i in range(10000):
    db.query(f"INSERT INTO data VALUES ({i})")
    if i % 1000 == 0:
        db.transaction_flush()  # Commit every 1000 rows
db.transaction_end()
```

---

#### `get_last_insert_id() -> int`
Get the rowid of the last INSERT operation.

**Returns:**
- Integer rowid of last inserted row

**Example:**
```python
db.query("INSERT INTO users (name) VALUES ('Alice')")
user_id = db.get_last_insert_id()
api.post(f"New user ID: {user_id}")
```

---

#### `create_table(tablename: str, num_fields: int, fieldnames: list, fieldtypes: list) -> None`
Create a new table with specified schema.

**Parameters:**
- `tablename`: Name of table to create
- `num_fields`: Number of fields (must match length of fieldnames and fieldtypes)
- `fieldnames`: List of field name strings
- `fieldtypes`: List of field type strings ("INTEGER", "REAL", "TEXT", "BLOB")

**Raises:**
- `ValueError` if argument types or counts don't match
- `RuntimeError` if table creation fails

**Example:**
```python
db.create_table(
    "users",
    3,
    ["id", "name", "age"],
    ["INTEGER PRIMARY KEY", "TEXT", "INTEGER"]
)
```

---

#### `add_column(tablename: str, fieldname: str, fieldtype: str) -> None`
Add a column to an existing table.

**Parameters:**
- `tablename`: Name of existing table
- `fieldname`: Name of new column
- `fieldtype`: Type of new column ("INTEGER", "REAL", "TEXT", "BLOB")

**Raises:**
- `RuntimeError` if column addition fails

**Example:**
```python
db.add_column("users", "email", "TEXT")
```

---

#### `pointer() -> int`
Get pointer to database object.

**Returns:**
- Integer pointer value

---

#### `is_null() -> bool`
Check if database pointer is null.

**Returns:**
- `True` if null, `False` otherwise

---

#### `__repr__() -> str`
String representation.

**Example:**
```python
print(db)  # "Database(ptr=0x...)"
```

---

## Class: `api.DBResult`

Wrapper for database query result sets.

### Constructor

```python
result = api.DBResult()
```

Creates an uninitialized result wrapper. Typically obtained from `Database.query()`.

### Methods

#### `numrecords() -> int`
Get number of records in result.

**Returns:**
- Integer record count

**Example:**
```python
result = db.query("SELECT * FROM users")
api.post(f"Found {result.numrecords()} users")
```

---

#### `numfields() -> int`
Get number of fields per record.

**Returns:**
- Integer field count

**Example:**
```python
api.post(f"Each record has {result.numfields()} fields")
```

---

#### `fieldname(index: int) -> str`
Get name of field at index.

**Parameters:**
- `index`: Field index (0-based)

**Returns:**
- Field name string

**Raises:**
- `RuntimeError` if index invalid

**Example:**
```python
for i in range(result.numfields()):
    api.post(f"Field {i}: {result.fieldname(i)}")
```

---

#### `get_string(record: int, field: int) -> str`
Get string value from result.

**Parameters:**
- `record`: Record index (0-based)
- `field`: Field index (0-based)

**Returns:**
- String value

**Example:**
```python
name = result.get_string(0, 1)  # First record, second field
```

---

#### `get_long(record: int, field: int) -> int`
Get integer value from result.

**Parameters:**
- `record`: Record index
- `field`: Field index

**Returns:**
- Integer value

---

#### `get_float(record: int, field: int) -> float`
Get float value from result.

**Parameters:**
- `record`: Record index
- `field`: Field index

**Returns:**
- Float value

---

#### `get_record(record: int) -> dict`
Get complete record as dictionary.

**Parameters:**
- `record`: Record index (0-based)

**Returns:**
- Dictionary mapping field names to values

**Example:**
```python
record = result.get_record(0)
api.post(f"Name: {record['name']}, Age: {record['age']}")
```

---

#### `to_list() -> list`
Convert all results to list of dictionaries.

**Returns:**
- List of dictionaries, one per record

**Example:**
```python
results = db.query("SELECT * FROM users").to_list()
for user in results:
    api.post(f"{user['name']}: {user['age']}")
```

---

#### `reset() -> None`
Reset result to beginning.

**Example:**
```python
result.reset()
# Can iterate again from start
```

---

#### `clear() -> None`
Clear result data.

**Example:**
```python
result.clear()
```

---

#### `pointer() -> int`
Get pointer to result object.

**Returns:**
- Integer pointer value

---

#### `is_null() -> bool`
Check if result pointer is null.

**Returns:**
- `True` if null, `False` otherwise

---

#### `__len__() -> int`
Get number of records (Python operator).

**Returns:**
- Record count

**Example:**
```python
result = db.query("SELECT * FROM users")
api.post(f"Found {len(result)} records")
```

---

#### `__repr__() -> str`
String representation.

**Example:**
```python
print(result)  # "DBResult(records=5, fields=3, ptr=0x...)"
```

---

## Complete Examples

### Basic Database Operations

```python
import api

# Create and open database
db = api.Database()
db.open("test_db")

# Create table
db.create_table(
    "users",
    3,
    ["id", "name", "age"],
    ["INTEGER PRIMARY KEY", "TEXT", "INTEGER"]
)

# Insert data
db.query("INSERT INTO users (name, age) VALUES ('Alice', 30)")
db.query("INSERT INTO users (name, age) VALUES ('Bob', 25)")

# Get last insert ID
user_id = db.get_last_insert_id()
api.post(f"Last inserted ID: {user_id}")

# Query data
result = db.query("SELECT * FROM users WHERE age > 20")
api.post(f"Found {len(result)} users over 20")

# Access results
for i in range(result.numrecords()):
    record = result.get_record(i)
    api.post(f"User: {record['name']}, Age: {record['age']}")

# Close database
db.close()
```

### Transactions for Bulk Operations

```python
import api

db = api.Database()
db.open("bulk_data")

# Create table
db.create_table(
    "measurements",
    2,
    ["timestamp", "value"],
    ["INTEGER", "REAL"]
)

# Use transaction for better performance
db.transaction_start()

for i in range(1000):
    db.query(f"INSERT INTO measurements VALUES ({i}, {i * 0.5})")

    # Periodic flush to avoid memory buildup
    if i % 100 == 0:
        db.transaction_flush()

db.transaction_end()
api.post("Inserted 1000 measurements in transaction")

db.close()
```

### Query Results as Python Data

```python
import api

db = api.Database()
db.open("analytics")

# Query with aggregation
result = db.query("""
    SELECT
        category,
        COUNT(*) as count,
        AVG(value) as avg_value
    FROM data
    GROUP BY category
    ORDER BY count DESC
""")

# Convert to Python list
data = result.to_list()

# Process results
for row in data:
    api.post(f"{row['category']}: {row['count']} items, avg={row['avg_value']:.2f}")

db.close()
```

### Dynamic Schema Modification

```python
import api

db = api.Database()
db.open("evolving_schema")

# Create initial table
db.create_table(
    "products",
    2,
    ["id", "name"],
    ["INTEGER PRIMARY KEY", "TEXT"]
)

# Add columns as needed
db.add_column("products", "price", "REAL")
db.add_column("products", "stock", "INTEGER")

# Now insert with new schema
db.query("INSERT INTO products (name, price, stock) VALUES ('Widget', 9.99, 100)")

db.close()
```

### Error Handling

```python
import api

db = api.Database()

try:
    db.open("my_database")

    try:
        # This might fail if table exists
        db.create_table("data", 2, ["id", "value"], ["INTEGER", "REAL"])
    except RuntimeError as e:
        api.post(f"Table might already exist: {e}")

    # Try query
    try:
        result = db.query("SELECT * FROM nonexistent_table")
    except RuntimeError as e:
        api.error(f"Query failed: {e}")

finally:
    if not db.is_null():
        db.close()
```

### Data Export Pattern

```python
import api

def export_to_python_dict(db, tablename):
    """Export entire table to Python dictionary"""
    result = db.query(f"SELECT * FROM {tablename}")

    # Convert to list of dicts
    records = result.to_list()

    return {
        "table": tablename,
        "count": len(records),
        "fields": [result.fieldname(i) for i in range(result.numfields())],
        "data": records
    }

# Usage
db = api.Database()
db.open("my_data")

exported = export_to_python_dict(db, "users")
api.post(f"Exported {exported['count']} records from {exported['table']}")

db.close()
```

### Persistent Configuration Storage

```python
import api

class Config:
    """Simple configuration storage using SQLite"""

    def __init__(self, db_name="app_config"):
        self.db = api.Database()
        self.db.open(db_name)
        self._init_table()

    def _init_table(self):
        try:
            self.db.create_table(
                "config",
                2,
                ["key", "value"],
                ["TEXT PRIMARY KEY", "TEXT"]
            )
        except RuntimeError:
            pass  # Table already exists

    def set(self, key, value):
        """Set configuration value"""
        # Use REPLACE to insert or update
        self.db.query(f"REPLACE INTO config (key, value) VALUES ('{key}', '{value}')")

    def get(self, key, default=None):
        """Get configuration value"""
        result = self.db.query(f"SELECT value FROM config WHERE key = '{key}'")
        if len(result) > 0:
            return result.get_string(0, 0)
        return default

    def close(self):
        self.db.close()

# Usage
config = Config()
config.set("volume", "0.75")
config.set("mode", "performance")

volume = config.get("volume", "1.0")
api.post(f"Volume: {volume}")

config.close()
```

## SQL Injection Protection

When building queries with user input, use parameterized queries or proper escaping:

```python
# UNSAFE - Don't do this
user_input = "'; DROP TABLE users; --"
db.query(f"SELECT * FROM users WHERE name = '{user_input}'")

# SAFER - Escape single quotes
def escape_sql_string(s):
    return s.replace("'", "''")

safe_input = escape_sql_string(user_input)
db.query(f"SELECT * FROM users WHERE name = '{safe_input}'")

# BEST - Use application-level validation
def sanitize_username(name):
    # Only allow alphanumeric and underscore
    return ''.join(c for c in name if c.isalnum() or c == '_')

clean_name = sanitize_username(user_input)
db.query(f"SELECT * FROM users WHERE name = '{clean_name}'")
```

## SQLite Type Mapping

| SQLite Type | Python Type | Access Method |
|-------------|-------------|---------------|
| INTEGER | int | `get_long()` |
| REAL | float | `get_float()` |
| TEXT | str | `get_string()` |
| BLOB | str | `get_string()` |
| NULL | None | - |

## Performance Tips

1. **Use transactions for bulk operations**:
   ```python
   db.transaction_start()
   # ... many inserts
   db.transaction_end()
   ```

2. **Create indexes for frequently queried columns**:
   ```python
   db.query("CREATE INDEX idx_name ON users(name)")
   ```

3. **Use proper data types**:
   ```python
   # INTEGER for IDs and counts
   # REAL for measurements
   # TEXT for strings
   ```

4. **Limit result sets**:
   ```python
   result = db.query("SELECT * FROM large_table LIMIT 100")
   ```

5. **Use EXPLAIN QUERY PLAN to optimize**:
   ```python
   plan = db.query("EXPLAIN QUERY PLAN SELECT * FROM users WHERE age > 21")
   ```

## Limitations

- Database files persist between Max sessions
- All queries execute synchronously (blocking)
- No prepared statement API exposed
- Result sets loaded entirely into memory
- Max's database API wraps SQLite 3.x
- Thread safety depends on Max's implementation

## See Also

- SQLite documentation: https://www.sqlite.org/docs.html
- Max SDK: Database documentation
- `api.Dictionary` - Alternative for simple key-value storage
- `api.Table` - Array storage for numeric data

## Notes

- Always call `db.close()` when done to release file handles
- Database files stored in Max search path by default
- Use `transaction_start/end` for bulk operations (much faster)
- Query results remain valid until next query or database close
- Empty result sets have `len(result) == 0`
- Field and record indices are 0-based
