"""
Example: Using the Database API in Max/MSP

This demonstrates SQLite database operations via the api module.

The Database API provides persistent storage, complex queries,
and relational data management within Max patches.
"""

import api


# Example 1: Basic CRUD operations
def example_basic_crud():
    """Create, Read, Update, Delete operations"""
    api.post("=== Basic CRUD Example ===")

    db = api.Database()
    db.open("example_users")

    # Create table
    try:
        db.create_table(
            "users",
            3,
            ["id", "name", "email"],
            ["INTEGER PRIMARY KEY", "TEXT", "TEXT"]
        )
        api.post("Created users table")
    except RuntimeError:
        api.post("Table already exists")

    # CREATE
    db.query("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com')")
    db.query("INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com')")
    api.post(f"Created users, last ID: {db.get_last_insert_id()}")

    # READ
    result = db.query("SELECT * FROM users")
    api.post(f"Found {len(result)} users:")
    for i in range(result.numrecords()):
        record = result.get_record(i)
        api.post(f"  {record['id']}: {record['name']} <{record['email']}>")

    # UPDATE
    db.query("UPDATE users SET email = 'alice@newdomain.com' WHERE name = 'Alice'")
    api.post("Updated Alice's email")

    # DELETE
    db.query("DELETE FROM users WHERE name = 'Bob'")
    api.post("Deleted Bob")

    # Verify
    result = db.query("SELECT * FROM users")
    api.post(f"Now have {len(result)} users")

    db.close()


# Example 2: Transactions for performance
def example_transactions():
    """Demonstrate transaction usage for bulk operations"""
    api.post("=== Transaction Example ===")

    db = api.Database()
    db.open("example_measurements")

    # Create table
    try:
        db.create_table(
            "measurements",
            3,
            ["id", "timestamp", "value"],
            ["INTEGER PRIMARY KEY", "INTEGER", "REAL"]
        )
    except RuntimeError:
        pass

    # Insert 1000 records with transaction
    api.post("Inserting 1000 records with transaction...")

    db.transaction_start()

    for i in range(1000):
        db.query(f"INSERT INTO measurements (timestamp, value) VALUES ({i * 1000}, {i * 0.5})")

        # Flush every 100 records to avoid memory buildup
        if i > 0 and i % 100 == 0:
            db.transaction_flush()

    db.transaction_end()
    api.post("Insertion complete!")

    # Verify
    result = db.query("SELECT COUNT(*) as count FROM measurements")
    count = result.get_long(0, 0)
    api.post(f"Total records: {count}")

    db.close()


# Example 3: Query result processing
def example_query_results():
    """Different ways to process query results"""
    api.post("=== Query Results Example ===")

    db = api.Database()
    db.open("example_products")

    # Setup
    try:
        db.create_table(
            "products",
            4,
            ["id", "name", "price", "stock"],
            ["INTEGER PRIMARY KEY", "TEXT", "REAL", "INTEGER"]
        )
        db.query("INSERT INTO products (name, price, stock) VALUES ('Widget', 9.99, 100)")
        db.query("INSERT INTO products (name, price, stock) VALUES ('Gadget', 19.99, 50)")
        db.query("INSERT INTO products (name, price, stock) VALUES ('Doohickey', 14.99, 75)")
    except RuntimeError:
        pass

    # Method 1: Field-by-field access
    api.post("Method 1: Field-by-field access")
    result = db.query("SELECT * FROM products")
    for i in range(result.numrecords()):
        name = result.get_string(i, 1)
        price = result.get_float(i, 2)
        stock = result.get_long(i, 3)
        api.post(f"  {name}: ${price:.2f} (stock: {stock})")

    # Method 2: Record dictionaries
    api.post("Method 2: Record dictionaries")
    result = db.query("SELECT * FROM products WHERE price > 10")
    for i in range(result.numrecords()):
        record = result.get_record(i)
        api.post(f"  {record['name']}: ${record['price']:.2f}")

    # Method 3: Convert to list
    api.post("Method 3: Full list conversion")
    result = db.query("SELECT name, price FROM products ORDER BY price DESC")
    products = result.to_list()
    for p in products:
        api.post(f"  {p['name']}: ${p['price']:.2f}")

    db.close()


# Example 4: Aggregation and grouping
def example_aggregation():
    """SQL aggregation functions"""
    api.post("=== Aggregation Example ===")

    db = api.Database()
    db.open("example_sales")

    # Setup
    try:
        db.create_table(
            "sales",
            4,
            ["id", "product", "quantity", "price"],
            ["INTEGER PRIMARY KEY", "TEXT", "INTEGER", "REAL"]
        )
        # Sample data
        sales_data = [
            ("Widget", 5, 9.99),
            ("Gadget", 3, 19.99),
            ("Widget", 2, 9.99),
            ("Doohickey", 4, 14.99),
            ("Gadget", 1, 19.99),
            ("Widget", 3, 9.99),
        ]
        for product, qty, price in sales_data:
            db.query(f"INSERT INTO sales (product, quantity, price) VALUES ('{product}', {qty}, {price})")
    except RuntimeError:
        pass

    # Aggregate by product
    result = db.query("""
        SELECT
            product,
            COUNT(*) as num_sales,
            SUM(quantity) as total_qty,
            AVG(price) as avg_price
        FROM sales
        GROUP BY product
        ORDER BY total_qty DESC
    """)

    api.post("Sales by product:")
    for i in range(result.numrecords()):
        record = result.get_record(i)
        api.post(f"  {record['product']}: {record['num_sales']} sales, "
                f"{record['total_qty']} units, avg ${record['avg_price']:.2f}")

    # Total revenue
    result = db.query("SELECT SUM(quantity * price) as total_revenue FROM sales")
    revenue = result.get_float(0, 0)
    api.post(f"Total revenue: ${revenue:.2f}")

    db.close()


# Example 5: Dynamic schema evolution
def example_schema_evolution():
    """Add columns to existing tables"""
    api.post("=== Schema Evolution Example ===")

    db = api.Database()
    db.open("example_contacts")

    # Initial table
    try:
        db.create_table(
            "contacts",
            2,
            ["id", "name"],
            ["INTEGER PRIMARY KEY", "TEXT"]
        )
        api.post("Created contacts table with id and name")
    except RuntimeError:
        api.post("Table already exists")

    # Add columns over time
    try:
        db.add_column("contacts", "email", "TEXT")
        api.post("Added email column")
    except RuntimeError:
        api.post("Email column already exists")

    try:
        db.add_column("contacts", "phone", "TEXT")
        api.post("Added phone column")
    except RuntimeError:
        api.post("Phone column already exists")

    # Now can use new schema
    db.query("INSERT INTO contacts (name, email, phone) VALUES "
            "('Charlie', 'charlie@example.com', '555-1234')")

    result = db.query("SELECT * FROM contacts")
    api.post(f"Contacts table now has {result.numfields()} fields:")
    for i in range(result.numfields()):
        api.post(f"  {i}: {result.fieldname(i)}")

    db.close()


# Example 6: Configuration storage pattern
class ConfigStore:
    """Simple key-value configuration storage"""

    def __init__(self, db_name="app_config"):
        self.db = api.Database()
        self.db.open(db_name)
        self._init()

    def _init(self):
        """Initialize config table"""
        try:
            self.db.create_table(
                "config",
                2,
                ["key", "value"],
                ["TEXT PRIMARY KEY", "TEXT"]
            )
        except RuntimeError:
            pass  # Table exists

    def set(self, key, value):
        """Set configuration value"""
        # Use REPLACE for insert-or-update
        safe_value = str(value).replace("'", "''")
        self.db.query(f"REPLACE INTO config (key, value) VALUES ('{key}', '{safe_value}')")

    def get(self, key, default=None):
        """Get configuration value"""
        result = self.db.query(f"SELECT value FROM config WHERE key = '{key}'")
        if len(result) > 0:
            return result.get_string(0, 0)
        return default

    def get_all(self):
        """Get all configuration as dictionary"""
        result = self.db.query("SELECT key, value FROM config")
        return {result.get_string(i, 0): result.get_string(i, 1)
                for i in range(result.numrecords())}

    def delete(self, key):
        """Delete configuration key"""
        self.db.query(f"DELETE FROM config WHERE key = '{key}'")

    def close(self):
        self.db.close()


def example_config_store():
    """Demonstrate configuration storage"""
    api.post("=== Configuration Store Example ===")

    config = ConfigStore()

    # Set values
    config.set("volume", "0.75")
    config.set("mode", "performance")
    config.set("last_file", "/path/to/file.txt")

    # Get values
    volume = config.get("volume", "1.0")
    mode = config.get("mode", "default")
    api.post(f"Volume: {volume}, Mode: {mode}")

    # Get all
    api.post("All configuration:")
    all_config = config.get_all()
    for key, value in all_config.items():
        api.post(f"  {key} = {value}")

    # Delete
    config.delete("last_file")
    api.post("Deleted last_file")

    config.close()


# Example 7: Data logging pattern
class DataLogger:
    """Simple data logger with database backend"""

    def __init__(self, log_name="data_log"):
        self.db = api.Database()
        self.db.open(log_name)
        self._init()

    def _init(self):
        """Initialize log table"""
        try:
            self.db.create_table(
                "logs",
                4,
                ["id", "timestamp", "level", "message"],
                ["INTEGER PRIMARY KEY", "INTEGER", "TEXT", "TEXT"]
            )
        except RuntimeError:
            pass

    def log(self, level, message):
        """Log a message"""
        import time
        timestamp = int(time.time() * 1000)  # milliseconds
        safe_msg = message.replace("'", "''")
        self.db.query(f"INSERT INTO logs (timestamp, level, message) "
                     f"VALUES ({timestamp}, '{level}', '{safe_msg}')")

    def info(self, message):
        self.log("INFO", message)

    def warn(self, message):
        self.log("WARN", message)

    def error(self, message):
        self.log("ERROR", message)

    def get_recent(self, count=10):
        """Get recent log entries"""
        result = self.db.query(f"SELECT * FROM logs ORDER BY id DESC LIMIT {count}")
        return result.to_list()

    def get_by_level(self, level):
        """Get logs by level"""
        result = self.db.query(f"SELECT * FROM logs WHERE level = '{level}' ORDER BY timestamp DESC")
        return result.to_list()

    def clear_old(self, days=7):
        """Clear logs older than N days"""
        import time
        cutoff = int((time.time() - days * 86400) * 1000)
        self.db.query(f"DELETE FROM logs WHERE timestamp < {cutoff}")

    def close(self):
        self.db.close()


def example_data_logger():
    """Demonstrate data logging"""
    api.post("=== Data Logger Example ===")

    logger = DataLogger()

    # Log some messages
    logger.info("Application started")
    logger.info("Processing data...")
    logger.warn("High memory usage detected")
    logger.error("Failed to connect to server")
    logger.info("Retrying connection...")
    logger.info("Connection successful")

    # Get recent logs
    api.post("Recent logs:")
    recent = logger.get_recent(5)
    for log in recent:
        api.post(f"  [{log['level']}] {log['message']}")

    # Get errors only
    api.post("Error logs:")
    errors = logger.get_by_level("ERROR")
    for log in errors:
        api.post(f"  {log['message']}")

    logger.close()


# Example 8: Joins and relationships
def example_joins():
    """Demonstrate table joins"""
    api.post("=== Joins Example ===")

    db = api.Database()
    db.open("example_relations")

    # Create tables
    try:
        db.create_table(
            "authors",
            2,
            ["id", "name"],
            ["INTEGER PRIMARY KEY", "TEXT"]
        )
        db.create_table(
            "books",
            3,
            ["id", "title", "author_id"],
            ["INTEGER PRIMARY KEY", "TEXT", "INTEGER"]
        )

        # Sample data
        db.query("INSERT INTO authors (name) VALUES ('Alice Author')")
        db.query("INSERT INTO authors (name) VALUES ('Bob Writer')")

        author1_id = 1
        author2_id = 2

        db.query(f"INSERT INTO books (title, author_id) VALUES ('Book One', {author1_id})")
        db.query(f"INSERT INTO books (title, author_id) VALUES ('Book Two', {author1_id})")
        db.query(f"INSERT INTO books (title, author_id) VALUES ('Book Three', {author2_id})")
    except RuntimeError:
        pass

    # Join query
    result = db.query("""
        SELECT
            authors.name as author_name,
            books.title as book_title
        FROM books
        JOIN authors ON books.author_id = authors.id
        ORDER BY authors.name, books.title
    """)

    api.post("Books by author:")
    current_author = None
    for i in range(result.numrecords()):
        record = result.get_record(i)
        author = record['author_name']
        book = record['book_title']

        if author != current_author:
            api.post(f"\n{author}:")
            current_author = author

        api.post(f"  - {book}")

    db.close()


# Example 9: Error handling
def example_error_handling():
    """Demonstrate proper error handling"""
    api.post("=== Error Handling Example ===")

    db = api.Database()

    try:
        db.open("example_errors")

        # Try to create duplicate table
        try:
            db.create_table("test", 1, ["id"], ["INTEGER PRIMARY KEY"])
            api.post("Created test table")
        except RuntimeError as e:
            api.post(f"Table creation failed (might exist): {e}")

        # Try invalid query
        try:
            result = db.query("SELECT * FROM nonexistent_table")
        except RuntimeError as e:
            api.error(f"Query failed: {e}")

        # Try invalid column
        try:
            db.query("INSERT INTO test (id, invalid_column) VALUES (1, 'test')")
        except RuntimeError as e:
            api.error(f"Insert failed: {e}")

    finally:
        # Always close database
        if not db.is_null():
            db.close()
            api.post("Database closed")


# Example 10: Performance optimization
def example_performance():
    """Demonstrate performance optimization techniques"""
    api.post("=== Performance Example ===")

    db = api.Database()
    db.open("example_performance")

    # Create table
    try:
        db.create_table(
            "data",
            2,
            ["id", "value"],
            ["INTEGER PRIMARY KEY", "INTEGER"]
        )
    except RuntimeError:
        pass

    # Without transaction (slow)
    api.post("Without transaction: inserting 100 records...")
    # (This would be slow, so we skip it in the example)

    # With transaction (fast)
    api.post("With transaction: inserting 100 records...")
    db.transaction_start()
    for i in range(100):
        db.query(f"INSERT INTO data (value) VALUES ({i})")
    db.transaction_end()
    api.post("Done!")

    # Create index for faster queries
    db.query("CREATE INDEX IF NOT EXISTS idx_value ON data(value)")
    api.post("Created index on value column")

    # Query with index
    result = db.query("SELECT * FROM data WHERE value > 50 ORDER BY value")
    api.post(f"Found {len(result)} records > 50")

    db.close()


def run_all_examples():
    """Run all database examples"""
    api.post("=" * 60)
    api.post("Database API Examples")
    api.post("=" * 60)
    api.post("")

    example_basic_crud()
    api.post("")

    example_transactions()
    api.post("")

    example_query_results()
    api.post("")

    example_aggregation()
    api.post("")

    example_schema_evolution()
    api.post("")

    example_config_store()
    api.post("")

    example_data_logger()
    api.post("")

    example_joins()
    api.post("")

    example_error_handling()
    api.post("")

    example_performance()

    api.post("")
    api.post("=" * 60)
    api.post("Examples complete!")


# Run examples when this file is executed
if __name__ == "__main__":
    run_all_examples()
