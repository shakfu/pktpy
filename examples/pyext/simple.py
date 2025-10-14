"""
simple.py - Simple pyext example

This demonstrates the basic structure of a Python external for Max.
"""

import api

class External:
    """
    A simple external that responds to bang and stores a value.
    """

    def __init__(self):
        # Configure inlets and outlets
        self.inlets = 2
        self.outlets = 2

        # Instance variables
        self.value = 0

        api.post("simple.py: External initialized")

    def bang(self):
        """Respond to bang message - output stored value"""
        api.post(f"Bang received! Value is {self.value}")

    def int(self, n):
        """Respond to int message - store the value"""
        self.value = n
        api.post(f"Stored value: {self.value}")

    def float(self, f):
        """Respond to float message"""
        api.post(f"Received float: {f}")

    def set_value(self, n):
        """Custom message handler - set value without output"""
        self.value = n
        api.post(f"Value set to: {self.value}")
