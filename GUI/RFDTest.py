import serial

RFD = False
COMRFD = "COM4"
   
def SetupRFD(): 
    try:
        SerialRFD = serial.Serial(
            port=COMRFD,
            baudrate=57600,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=None
        )
        
        RFD = True
        
        print("1")
        
    except Exception as e:
        print("0")


def UpdateRFD():
    LatTolerance = 0.01
    LonTolerance = 0.01
    AltTolerance = 100

    if RFD == True:
        try:
            SerialRFD.reset_input_buffer()
            raw_data = SerialRFD.readline()
            decoded_data = raw_data.decode("utf-8")
            data_list = decoded_data.split(",")
            
            print(data_list)

            if len(data_list) >= 30:
                packet, siv, fix, lat, lon, alt, year, month, day, hour, minute, sec, nedN, nedE, nedD, \
                bat, bat33, bat51, bat52, aint, aext, ptemp, dint, dent, pres, ax, ay, az, pitch, roll, \
                yaw = data_list[:31]

                NewLat = round(float(lat) * .0000001, 6)
                NewLon = round(float(lon) * .0000001, 6)
                NewAlt = float(alt) / 1000 * 3.28084
                
                print(pres)
                

                if abs(NewLat - PayloadLatD) <= LatTolerance and \
                   abs(NewLon - PayloadLonD) <= LonTolerance and \
                   abs(NewAlt - PayloadAltF) <= AltTolerance:
                    PayloadLatD = NewLat
                    PayloadLonD = NewLon
                    PayloadAltF = NewAlt
                else:
                    raise ValueError("Bad GPS Data")

        except Exception as e:
            print("0")

i = 1

while not RFD:
    SetupRFD()

while RFD:
    UpdateRFD()