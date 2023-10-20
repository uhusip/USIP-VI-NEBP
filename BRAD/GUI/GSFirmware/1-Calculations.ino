void CalculatePosition1(int32_t targetPosition) {
  do {
    ResetTimeout();
  } while (tic1.getCurrentPosition() != targetPosition && Serial.peek() != 'E');

  if(NegativeAzimuth1 == true) {
    Azimuth1 = Azimuth1 + 360; // move back to 0-360 range
    NegativeAzimuth1 = false;
  }
  
  if(NegativeAzimuth2 == true) {
    Azimuth2 = Azimuth2 + 360; // move back to 0-360 range
    NegativeAzimuth2 = false;
  }
  
  Azimuth1 = Azimuth2;
  tic1.haltAndSetPosition(0);
}

void CalculatePosition2(int32_t targetPosition) {
  do {
    ResetTimeout();
  } while (tic2.getCurrentPosition() != targetPosition && Serial.peek() != 'E');

  Elevation1 = Elevation2;
  tic2.haltAndSetPosition(0);
}

int AdjustPan() {
  tic1.exitSafeStart();

  if(abs(Azimuth2 - (Azimuth1 - 360)) < abs(Azimuth2 - Azimuth1)) {
    Azimuth1 = Azimuth1 - 360;
    NegativeAzimuth1 = true;
  }

  else if(abs((Azimuth2 - 360) - Azimuth1) < abs(Azimuth2 - Azimuth1)) {
    Azimuth2 = Azimuth2 - 360;
    NegativeAzimuth2 = true;
  }

  return int(-119.33333 * (Azimuth2 - Azimuth1));
}

int AdjustTilt() {
  tic2.exitSafeStart();
  
  return int(119.33333 * (Elevation2 - Elevation1)); 
}
