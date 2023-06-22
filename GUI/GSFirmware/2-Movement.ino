void PanAndTilt(String bytes_in) {
  for (int index = 1; index < bytes_in.length(); index++) {
    if (bytes_in[index] == ',') {
      Azimuth2 = bytes_in.substring(1, index).toFloat();
      Elevation2 = bytes_in.substring(index + 1).toFloat();
      break;
    }
  }

  tic1.exitSafeStart();
  tic2.exitSafeStart();

  int pan = AdjustPan();
  tic1.setTargetPosition(pan);
  CalculatePosition1(pan);
      
  int tilt = AdjustTilt();
  tic2.setTargetPosition(tilt);
  CalculatePosition2(tilt);
      
  ResetDelay(125); 
}

void PanPositive(String bytes_in) {
  tic1.exitSafeStart();

  int NewPosition = bytes_in.substring(1).toFloat() * 119.3333;
  tic1.setTargetPosition(NewPosition);
  
  CalculatePosition1(NewPosition);
}

void PanNegative(String bytes_in) {
  tic1.exitSafeStart();
  
  int NewPosition = bytes_in.substring(1).toFloat() * 119.3333;
  tic1.setTargetPosition(-NewPosition);
  
  CalculatePosition1(-NewPosition);
}

void TiltUp(String bytes_in) {
  tic2.exitSafeStart();

  int NewPosition = bytes_in.substring(1).toFloat() * 119.3333;
  tic2.setTargetPosition(NewPosition);
  
  CalculatePosition2(NewPosition);
}

void TiltDown(String bytes_in) {
  tic2.exitSafeStart();

  int NewPosition = bytes_in.substring(1).toFloat() * 119.3333;
  tic2.setTargetPosition(-NewPosition);
  
  CalculatePosition2(-NewPosition);
}
