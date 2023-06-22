import csv
import time
from datetime import datetime


def getTimeDiffs():
    flightData = open('flightData.csv', "r")

    csvReader = csv.reader(flightData)
    next(csvReader)

    firstLine = next(csvReader)
    # print(firstLine[1])
    oldTime = datetime.strptime(firstLine[1], "%Y-%m-%dT%H:%M:%SZ")
    # oldTime = 0
    print(oldTime)

    i = 0
    fullTimeDiff = []

    for line in csvReader:
        uglyDate = line[1]
        currTime = datetime.strptime(uglyDate, "%Y-%m-%dT%H:%M:%SZ")
        # print((currTime - oldTime).total_seconds())
        fullTimeDiff.append( (currTime - oldTime).total_seconds() )

        oldTime = currTime
        i += 1

        print(i)

    flightData.close()

    return fullTimeDiff


def getLats():
    flightData = open('flightData.csv', "r")

    csvReader = csv.reader(flightData)
    next(csvReader)

    latList = []
    for line in csvReader:
        print(line[2])
        latList.append(line[2])

    flightData.close()

    return latList

def getLongs():
    flightData = open('flightData.csv', "r")

    csvReader = csv.reader(flightData)
    next(csvReader)

    longList = []
    for line in csvReader:
        print(line[3])
        longList.append(line[3])

    flightData.close()

    return longList

def getAlts():
    flightData = open('flightData.csv', "r")

    csvReader = csv.reader(flightData)
    next(csvReader)

    altList = []
    for line in csvReader:
        print(line[4])
        altList.append(line[4])

    flightData.close()

    return altList
