"""
custom_counter.py - Example with custom class name using @api.external

This shows that you can use ANY class name when decorating with @api.external.
"""

import api

@api.external
class MyAwesomeCounter:
    """
    A counter that demonstrates custom class naming.
    """

    def __init__(self):
        self.inlets = 1
        self.outlets = 2
        self.count = 0
        api.post("MyAwesomeCounter: ready to count!")

    def bang(self):
        """Increment and output count"""
        self.count += 1
        api.post(f"Count: {self.count}")
        self._outlets[0].int(self.count)

    def reset(self):
        """Reset counter to zero"""
        self.count = 0
        api.post("Counter reset to 0")
        self._outlets[0].int(0)

    def set(self, value):
        """Set counter to specific value"""
        self.count = value
        api.post(f"Counter set to: {self.count}")
        self._outlets[0].int(self.count)

    def increment(self, step):
        """Increment by specific step"""
        self.count += step
        api.post(f"Incremented by {step}, now: {self.count}")
        self._outlets[0].int(self.count)
