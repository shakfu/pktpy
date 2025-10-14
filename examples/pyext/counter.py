"""
counter.py - Counter external example

Demonstrates a simple counter that increments on bang.
"""

import api

class External:
    """
    A counter that increments on bang and can be reset.
    """

    def __init__(self):
        self.inlets = 1
        self.outlets = 1
        self.count = 0
        api.post("counter.py: Counter initialized")

    def bang(self):
        """Increment counter and output"""
        self.count += 1
        api.post(f"Count: {self.count}")

    def reset(self):
        """Reset counter to zero"""
        self.count = 0
        api.post("Counter reset")

    def set(self, n):
        """Set counter to specific value"""
        self.count = n
        api.post(f"Counter set to {self.count}")
