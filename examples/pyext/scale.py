"""
scale.py - Value scaler example

Demonstrates math operations and multiple inlets/outlets.
"""

import api

class External:
    """
    Scales incoming numbers by a factor.
    Inlet 0: input value
    Inlet 1: scale factor
    """

    def __init__(self):
        self.inlets = 2
        self.outlets = 1
        self.scale_factor = 1.0
        api.post("scale.py: Scaler initialized")

    def int(self, n):
        """Scale integer input"""
        result = n * self.scale_factor
        api.post(f"Scaled {n} by {self.scale_factor} = {result}")
        self._outlets[0].float(result)

    def float(self, f):
        """Scale float input"""
        result = f * self.scale_factor
        api.post(f"Scaled {f} by {self.scale_factor} = {result}")
        self._outlets[0].float(result)

    def set_scale(self, factor):
        """Set the scale factor"""
        self.scale_factor = factor
        api.post(f"Scale factor set to {self.scale_factor}")
