from satelliteTrackingMath import trackMath
from Ground_Station_Arduino import Ground_Station_Arduino
import serial.tools.list_ports
import time

# this whole file is a bodge, used when the website decided to go down :(


def calcs():
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

    while True:

        startingAzimuth = float(input("starting azimuth; "))
        startingElevation = float(input("starting elevation: "))
        GSMotors.calibrate(startingAzimuth, startingElevation)

        GSLat = float(input("enter gs lat: "))
        GSLong = float(input("enter gs long: "))
        GSAlt = float(input("enter GS alt: "))

        BalloonLat = float(input("Enter balloon lat: "))
        BalloonLong = float(input("Enter balloon long: "))
        BalloonAlt = float(input("Enter balloon alt: "))

        Tracking_Calc = trackMath(GSLong, GSLat, GSAlt, BalloonLong, BalloonLat, BalloonAlt)

        distance = Tracking_Calc.distance
        newElevation = Tracking_Calc.elevation()
        newAzimuth = Tracking_Calc.azimuth()

        print(" Distance " + str(distance) + " Azimuth: " + str(newAzimuth) + ", Elevation: " + str(newElevation))

        time.sleep(1)

        GSMotors.move_position(newAzimuth, newElevation)


calcs()
