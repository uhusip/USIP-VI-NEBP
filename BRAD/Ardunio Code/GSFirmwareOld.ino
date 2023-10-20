/*
-------------------------------------------------------------------------------
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
-------------------------------------------------------------------------------
*/

#include <Tic.h>
#include <SoftwareSerial.h>

TicI2C tic1(14);
TicI2C tic2(15);

SoftwareSerial mySerial(8, 7);

float oldAzimuth, newAzimuth = 0;
float oldElevation, newElevation = 0;

// these are used for finding the shortest path to a new point
bool negativeOldAzimuth, negativeNewAzimuth = false;

String bytes_in = "";

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.setTimeout(5);

  Wire.begin();
  delay(20);

  tic1.exitSafeStart();
  tic2.exitSafeStart();

  // this will hopefully clear any errors present
  // if one motor mysteriously stops working,
  // try plugging into the tic controller and clearing any errors from the pololu tic controller software
  tic1.clearDriverError();
  tic2.clearDriverError();

  tic1.haltAndSetPosition(0);
  tic2.haltAndSetPosition(0);

  // end of motor setup
}

void resetCommandTimeout() {
  tic1.resetCommandTimeout();
  tic2.resetCommandTimeout();
}


void delayWhileResettingCommandTimeout(uint32_t ms) {
  uint32_t start = millis();
  do {
    resetCommandTimeout();
  } while ((uint32_t)(millis() - start) <= ms);
}


void waitForPosition1(int32_t targetPosition) {
  do {
    resetCommandTimeout();
  } while (tic1.getCurrentPosition() != targetPosition);

  if(negativeOldAzimuth == true) {
    oldAzimuth = oldAzimuth + 360; // move back to 0-360 range
    negativeOldAzimuth = false;
  }

  if(negativeNewAzimuth == true) {
    newAzimuth = newAzimuth + 360; // move back to 0-360 range
    negativeNewAzimuth = false;
  }

  oldAzimuth = newAzimuth;
  tic1.haltAndSetPosition(0);
}


void waitForPosition2(int32_t targetPosition) {
  do {
    resetCommandTimeout();
  } while (tic2.getCurrentPosition() != targetPosition);

  oldElevation = newElevation;
  tic2.haltAndSetPosition(0);
}


void movePanAndTilt(String bytes_in) {
 for (int index = 1; index < bytes_in.length(); index++)
      {
        if (bytes_in[index] == ',')
        {
          newAzimuth = bytes_in.substring(1, index).toFloat();
          newElevation = bytes_in.substring(index + 1).toFloat();
          break;
        }
      }

      tic1.exitSafeStart();
      tic2.exitSafeStart(); // these seem to prevent safe start error
      // the exitSafeStart only clears flag for 200ms

      int pan = movePan();
      tic1.setTargetPosition(pan);
      waitForPosition1(pan);
      
      int tilt = moveTilt();
      tic2.setTargetPosition(tilt);
      waitForPosition2(tilt);
      
      delayWhileResettingCommandTimeout(125); 
}

int movePan() {
  tic1.exitSafeStart();

  if(abs(newAzimuth - (oldAzimuth - 360)) < abs(newAzimuth - oldAzimuth)) // check for shortest path to positions
  {
    oldAzimuth = oldAzimuth - 360;
    negativeOldAzimuth = true;
  }
  else if(abs((newAzimuth - 360) - oldAzimuth) < abs(newAzimuth - oldAzimuth))
  {
    newAzimuth = newAzimuth - 360;
    negativeNewAzimuth = true;
  }

  float delta = newAzimuth - oldAzimuth;

  //return int(-119.33333 * delta); // needs to be negative due to wiring
  return int(-127.2889 * delta); // needs to be negative due to wiring
}

int moveTilt() {
  tic2.exitSafeStart();

  float delta = newElevation - oldElevation;

  return int(119.33333 * delta); 
}

void calibrate(String bytes_in) {
 for (int index = 1; index < bytes_in.length(); index++)
      {
        if (bytes_in[index] == ',')
        {
          oldAzimuth = bytes_in.substring(1, index).toFloat();
          oldElevation = bytes_in.substring(index + 1).toFloat();
          break;
        }
      } 
      return;
}

void adjustPanPos() {
  tic1.exitSafeStart();

  //int adjustedPos = bytes_in.substring(1).toFloat() * 119.3333;
  int adjustedPos = bytes_in.substring(1).toFloat() * 127.2889;
  tic1.setTargetPosition(adjustedPos);
  waitForPosition1(adjustedPos);
}

void adjustPanNeg() {
  tic1.exitSafeStart();
  
  // int adjustedPos = bytes_in.substring(1).toFloat() * 119.3333;
  int adjustedPos = bytes_in.substring(1).toFloat() * 127.2889;
  tic1.setTargetPosition(-adjustedPos);
  waitForPosition1(-adjustedPos);
}

void adjustTiltUp(String bytes_in) {
  tic2.exitSafeStart();

  // int adjustedPos = bytes_in.substring(1).toFloat() * 119.3333;
  int adjustedPos = bytes_in.substring(1).toFloat() * 127.2889;
  tic2.setTargetPosition(adjustedPos);
  waitForPosition2(adjustedPos);
}

void adjustTiltDown() {
  tic2.exitSafeStart();

  //int adjustedPos = bytes_in.substring(1).toFloat() * 119.3333;
  int adjustedPos = bytes_in.substring(1).toFloat() * 127.2889;
  tic2.setTargetPosition(-adjustedPos);
  waitForPosition2(-adjustedPos);
}

void loop() {
  // internal gear ratio is 26.85:1 and external gear ratio is 8:1
  // this gives 180 degree rotation of 21480 microsteps
  // each degree is 119.3333 microsteps
  // each microstep is .00838 degrees
  // both gear ratios should be the same for pan and tilt, thus this should be consistent
  //
  //update 2-9-2023 for new 15 tooth gear (old gear 16 tooth not avaiable)
  //external gear ratio is new 128 teeth / 15 teeth = 8.533:1 (old ration 128 teeth / 16 teeth = 8:1)  
  //26.85*8.533*100 = 127.2889 (old 26.85*8*100 = 119.3333)  Each degree is 127.2889 microsteps
  //Change this throughout program for both pan and tilt.  Gear ratios are the same.

  if (Serial.available() > 0) {} // request for something to happen
    bytes_in = Serial.readString();
    
    if (bytes_in[0] == 'M') { // note move is 'M' for now
      movePanAndTilt(bytes_in);
    }
    else if(bytes_in[0] == 'C') {
      calibrate(bytes_in);
    }
    else if(bytes_in[0] == 'W') {
      adjustTiltUp(bytes_in);
    }
    else if(bytes_in[0] == 'S') {
      adjustTiltDown();
    }
    else if(bytes_in[0] == 'A') {
      adjustPanPos();
    }
    else if(bytes_in[0] == 'D') {
      adjustPanNeg();
    }
    else {
      return;
    }
  }
}