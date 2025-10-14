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
        self.inlets = 1
        self.outlets = 2

        # Instance variables
        self.value = 0

        api.post("simple.py: External initialized")

    def bang(self):
        """Respond to bang message - output stored value"""
        api.post(f"Bang received! Value is {self.value}")
        self._outlets[0].int(self.value)

    def int(self, n):
        """Respond to int message - store and output the value"""
        self.value = n
        api.post(f"Stored value: {self.value}")
        self._outlets[0].int(n)

    def float(self, f):
        """Respond to float message - output to second outlet"""
        api.post(f"Received float: {f}")
        if len(self._outlets) > 1:
            self._outlets[1].float(f)

    def set_value(self, n):
        """Custom message handler - set value without output"""
        self.value = n
        api.post(f"Value set to: {self.value}")
