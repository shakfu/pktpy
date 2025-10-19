"""
simple_decorated.py - Example using @external decorator with custom class name

This demonstrates defining an external using the @api.external decorator.
With the decorator, you can use ANY class name you want!
"""

import api

@api.external
class ScaleValue:
    """
    A value scaler external marked with @api.external decorator.
    Note: The class name can be anything when using the decorator.
    """

    def __init__(self):
        # Configure inlets and outlets
        self.inlets = 2
        self.outlets = 1

        # Instance variables
        self.value = 0.0
        self.scale_factor = 1.0

        api.post("ScaleValue: initialized with @external decorator")

    def float(self, f):
        """Scale the incoming float value"""
        self.value = f
        scaled = self.value * self.scale_factor
        api.post(f"Scaled {self.value} by {self.scale_factor} = {scaled}")
        self._outlets[0].float(scaled)

    def int(self, n):
        """Scale the incoming int value"""
        self.float(float(n))

    def set_scale(self, factor):
        """Set the scaling factor"""
        self.scale_factor = factor
        api.post(f"Scale factor set to: {self.scale_factor}")

    def bang(self):
        """Output the last scaled value"""
        scaled = self.value * self.scale_factor
        self._outlets[0].float(scaled)
