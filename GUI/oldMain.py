"""
-------------------------------------------------------------------------------
MIT License
Copyright (c) 2021 Mathew Clutter and Ronnel Walton
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-------------------------------------------------------------------------------
"""

from Balloon_Coordinates import Balloon_Coordinates
from satelliteTrackingMath import trackMath
from Ground_Station_Arduino import Ground_Station_Arduino
import serial.tools.list_ports
import time


timer = time.time()

IMEIList = Balloon_Coordinates.list_IMEI()

for index in range(len(IMEIList)):
    print("[", index, "] ", IMEIList[index])

Balloon = Balloon_Coordinates(IMEIList[int(input("Choose IMEI: "))])
Balloon.print_info()

ports = serial.tools.list_ports.comports()
portNames = []

index = 0
for port, desc, hwid in sorted(ports):
    print("[{}] {}: {}".format(index, port, desc))
    portNames.append("{}".format(port))
    index += 1

user_port = int(input("Choose Port: "))
print("{}".format(portNames[user_port]))

GSMotors = Ground_Station_Arduino(portNames[user_port], 9600)

# Initialize First Coordinate

GSMotors.warm_start()
time.sleep(1)

GSCoords = GSMotors.req_GPS()
GSMotors.print_GPS()
groundStationLat = GSCoords[0]
groundStationLong = GSCoords[1]
groundStationAltitude = GSCoords[2]

adjusting = True
while adjusting:
    moveDir = input("enter w a s d to adjust pan/tilt, q to exit: ").strip().lower()
    if moveDir == "w":
        GSMotors.adjustTiltUp(1)
    elif moveDir == "s":
        GSMotors.adjustTiltDown(1)
    elif moveDir == "a":
        GSMotors.adjustPanPositive(1)
    elif moveDir == "d":
        GSMotors.adjustPanNegative(1)
    elif moveDir == "q":
        adjusting = False
        break
    else:
        print("Invalid input")

# for now, these must be looked up using: https://gml.noaa.gov/grad/solcalc/ and the gps coords of ground station
oldAzimuth = float(input("Enter initial azimuth: "))
oldElevation = float(input("Enter initial elevation: "))

"""
groundStationLat = float(input("Enter the ground station latitude (decimal degrees) : "))
groundStationLong = float(input("Enter the ground station longitude (decimal degrees) : "))
groundStationAltitude = float(input("Enter the ground station altitude (m) : "))
"""

GSMotors.calibrate(oldAzimuth, oldElevation)


while True:
    if (time.time() - timer) > 1:
        timer = time.time()

        Balloon_Coor = Balloon.get_coor_alt()

        # note that trackMath takes arguments as long, lat, altitude
        Tracking_Calc = trackMath(groundStationLong, groundStationLat, groundStationAltitude, Balloon_Coor[1], Balloon_Coor[0], Balloon_Coor[2])

        distance = Tracking_Calc.distance
        newElevation = Tracking_Calc.elevation()
        newAzimuth = Tracking_Calc.azimuth()

        print("Distance " + str(distance) + " Azimuth: " + str(newAzimuth) + ", Elevation: " + str(newElevation))

        GSMotors.move_position(newAzimuth, newElevation)
