#!/usr/bin/env python
"""
Balloon Tracking Software for MSGC Ground Station X

Author:	Larson Dean Brandstetter, CpE
Modified: Ronnel Walton and Mathew Clutter
Based on the Satellite Tracking Method from eutelsat and the University of Munich.

"""

import math
import numpy as np


class trackMath:

    re = 6371000  # average radius of the earth

    def __init__(self, gslon, gslat, gsalt, tlon, tlat, talt):    # GPS information, gs is ground station, t is target
                                                            # Inputs are in degrees, converted to radians for calc (Why do this conversion?)
                                                            # Alt is in meters

        self.gslon = math.radians(gslon)
        self.gslat = math.radians(gslat)
        self.gsalt = gsalt
        self.tlon = math.radians(tlon)
        self.tlat = math.radians(tlat)
        self.talt = talt
        self.rgeo = trackMath.re + self.talt # radius of the earth plus altitude of target
        self.rgs = trackMath.re + self.gsalt # radius of the earth plus gs altitude
        self.w = self.gslon - self.tlon

        self.distance = self.distance()

    def distance(self):
        # calculates distance from ground station to the balloon (in m)
        a = math.pow((self.rgeo*math.cos(self.tlat)*math.cos(self.w)-self.rgs*math.cos(self.gslat)),2)
        b = math.pow(self.rgeo,2)*math.pow(math.cos(self.tlat),2)*math.pow(math.sin(self.w),2)
        c = math.pow((self.rgeo*math.sin(self.tlat)-self.rgs*math.sin(self.gslat)),2)
        self.distance = math.sqrt(a+b+c)
        return self.distance

    def elevation(self):
        # calculates the elevation from the ground station to the ballon (degrees)
        a = math.cos(self.gslat)*self.rgeo*math.cos(self.tlat)*math.cos(self.w)
        b = self.rgeo*math.sin(self.gslat)*math.sin(self.tlat)
        self.elev = -math.asin(-((a+b-self.rgs)/self.distance))

        self.elev = np.rad2deg(self.elev)

        if self.elev > 90:
            return 90
        elif self.elev < 0:
            return 0
        else:
            return self.elev

    def azimuth(self):
        # calculates the azimuth from the ground station to the balloon (compass bearing degrees)
        a = -(self.rgeo*math.cos(self.tlat)*math.sin(self.w))/(self.distance*math.cos(np.deg2rad(self.elev)))
        b = -(self.rgeo*((math.sin(self.gslat)*math.cos(self.tlat)*math.cos(self.w))-
                         (math.cos(self.gslat)*math.sin(self.tlat))))/(self.distance*math.cos(np.deg2rad(self.elev)))
        self.az = np.rad2deg(math.atan2(a, b))

        while self.az < 0:
            self.az += 360

        while self.az > 360:
            self.az -= 360

        return self.az
