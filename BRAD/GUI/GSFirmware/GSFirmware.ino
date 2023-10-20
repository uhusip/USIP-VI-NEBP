/*
   _____ ____  _______     _______  _____ _____ _    _ _______ 
  / ____/ __ \|  __ \ \   / /  __ \|_   _/ ____| |  | |__   __|
 | |   | |  | | |__) \ \_/ /| |__) | | || |  __| |__| |  | |   
 | |   | |  | |  ___/ \   / |  _  /  | || | |_ |  __  |  | |   
 | |___| |__| | |      | |  | | \ \ _| || |__| | |  | |  | |   
  \_____\____/|_|      |_|  |_|  \_\_____\_____|_|  |_|  |_|   

*/

/*

MIT License
Copyright (c) 2021 Mathew Clutter
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

*/

/*
  _    _  _____         _____ ______   _   _  ____ _______ ______  _____ 
 | |  | |/ ____|  /\   / ____|  ____| | \ | |/ __ \__   __|  ____|/ ____|
 | |  | | (___   /  \ | |  __| |__    |  \| | |  | | | |  | |__  | (___  
 | |  | |\___ \ / /\ \| | |_ |  __|   | . ` | |  | | | |  |  __|  \___ \ 
 | |__| |____) / ____ \ |__| | |____  | |\  | |__| | | |  | |____ ____) |
  \____/|_____/_/    \_\_____|______| |_| \_|\____/  |_|  |______|_____/ 

*/

/*

This is a fork of the dedicated firmware written by the MSU BOREALIS team.

The internal gear ratio of the ground station is 26.85 : 1.
The external gear ratio is 8 : 1.

This gives an 180 degree rotation of 21480 microsteps.
Each degree is 119.3333 microsteps.
Each microstep is 0.00838 degrees.

Both gear ratios should be the same for both pan and tilt.

If one of the motors stops working, plug it into the tic controller.
Clear any errors from the pololu tic controller software.

*/

/*
  _      _____ ____  _____            _____  _____ ______  _____ 
 | |    |_   _|  _ \|  __ \     /\   |  __ \|_   _|  ____|/ ____|
 | |      | | | |_) | |__) |   /  \  | |__) | | | | |__  | (___  
 | |      | | |  _ <|  _  /   / /\ \ |  _  /  | | |  __|  \___ \ 
 | |____ _| |_| |_) | | \ \  / ____ \| | \ \ _| |_| |____ ____) |
 |______|_____|____/|_|  \_\/_/    \_\_|  \_\_____|______|_____/ 
 
*/

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Tic.h>

/*
   _____ _      ____  ____          _       __      __     _____  _____          ____  _      ______  _____ 
  / ____| |    / __ \|  _ \   /\   | |      \ \    / /\   |  __ \|_   _|   /\   |  _ \| |    |  ____|/ ____|
 | |  __| |   | |  | | |_) | /  \  | |       \ \  / /  \  | |__) | | |    /  \  | |_) | |    | |__  | (___  
 | | |_ | |   | |  | |  _ < / /\ \ | |        \ \/ / /\ \ |  _  /  | |   / /\ \ |  _ <| |    |  __|  \___ \ 
 | |__| | |___| |__| | |_) / ____ \| |____     \  / ____ \| | \ \ _| |_ / ____ \| |_) | |____| |____ ____) |
  \_____|______\____/|____/_/    \_\______|     \/_/    \_\_|  \_\_____/_/    \_\____/|______|______|_____/ 
  
*/

TicI2C tic1(14);
TicI2C tic2(15);

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

String bytes_in = "";
String GPSString = "";

float Azimuth1 = 0;
float Azimuth2 = 0;

float Elevation1 = 0;
float Elevation2 = 0;

bool NegativeAzimuth1 = false;
bool NegativeAzimuth2 = false;

#define GPSECHO false

/*
  ______ _    _ _   _  _____ _______ _____ ____  _   _  _____ 
 |  ____| |  | | \ | |/ ____|__   __|_   _/ __ \| \ | |/ ____|
 | |__  | |  | |  \| | |       | |    | || |  | |  \| | (___  
 |  __| | |  | | . ` | |       | |    | || |  | | . ` |\___ \ 
 | |    | |__| | |\  | |____   | |   _| || |__| | |\  |____) |
 |_|     \____/|_| \_|\_____|  |_|  |_____\____/|_| \_|_____/ 
                                                                          
*/

// Tab 1 - Calculations
void CalculatePosition1(int32_t targetPosition);
void CalculatePosition2(int32_t targetPosition);

int AdjustPan();
int AdjustTilt();

// Tab 2 - Movement
void PanAndTilt(String bytes_in);
void PanPositive(String bytes_in);
void PanNegative(String bytes_in);
void TiltUp(String bytes_in);
void TiltDown(String bytes_in);

// Tab 3 - Miscellaneous
void Calibration(String bytes_in);
void ResetDelay(uint32_t ms);
void ResetTimeout();
void GPSData();
void EmergencyStop();

/*
   _____ ______ _______ _    _ _____    _      ____   ____  _____  
  / ____|  ____|__   __| |  | |  __ \  | |    / __ \ / __ \|  __ \ 
 | (___ | |__     | |  | |  | | |__) | | |   | |  | | |  | | |__) |
  \___ \|  __|    | |  | |  | |  ___/  | |   | |  | | |  | |  ___/ 
  ____) | |____   | |  | |__| | |      | |___| |__| | |__| | |     
 |_____/|______|  |_|   \____/|_|      |______\____/ \____/|_|     
 
*/

void setup() {
  Serial.begin(9600);
  
  while (!Serial);
    Serial.setTimeout(5);

  Wire.begin();
  delay(20);

  tic1.exitSafeStart();
  tic2.exitSafeStart();

  tic1.clearDriverError();
  tic2.clearDriverError();

  tic1.haltAndSetPosition(0);
  tic2.haltAndSetPosition(0);

  GPS.begin(9600);

  // 5 Hz Update Rate and Fix Update Rate
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);

  // Request Firmware Version
  while(!GPS.LOCUS_ReadStatus());
    mySerial.println(PMTK_Q_RELEASE);
}

/*
  __  __          _____ _   _   _      ____   ____  _____  
 |  \/  |   /\   |_   _| \ | | | |    / __ \ / __ \|  __ \ 
 | \  / |  /  \    | | |  \| | | |   | |  | | |  | | |__) |
 | |\/| | / /\ \   | | | . ` | | |   | |  | | |  | |  ___/ 
 | |  | |/ ____ \ _| |_| |\  | | |___| |__| | |__| | |     
 |_|  |_/_/    \_\_____|_| \_| |______\____/ \____/|_|     
 
*/

void loop() {
  if (Serial.available() > 0) {
    bytes_in = Serial.readString();

    if (bytes_in[0] == 'C')
      Calibration(bytes_in);

    else if (bytes_in[0] == 'M')
      PanAndTilt(bytes_in);

    else if (bytes_in[0] == 'W')
      TiltUp(bytes_in);

    else if (bytes_in[0] == 'S')
      TiltDown(bytes_in);

    else if (bytes_in[0] == 'A')
      PanPositive(bytes_in);

    else if (bytes_in[0] == 'D')
      PanNegative(bytes_in);

    else if (bytes_in[0] == 'E')
      EmergencyStop(bytes_in);

    else if (bytes_in[0] == 'G' && GPS.fix)
      GPSData();

    else
      return;
  }

  char c = GPS.read();

  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
}
  
