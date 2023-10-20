void Calibration(String bytes_in) {
  for (int index = 1; index < bytes_in.length(); index++) {
    if (bytes_in[index] == ',') {
      Azimuth1 = bytes_in.substring(1, index).toFloat();
      Elevation1 = bytes_in.substring(index + 1).toFloat();
      break;
    }
  } 
  return;
}

void ResetDelay(uint32_t ms) {
  uint32_t start = millis();
  do {
    ResetTimeout();
  } while ((uint32_t)(millis() - start) <= ms);
}

void ResetTimeout() {
  tic1.ResetTimeout();
  tic2.ResetTimeout();
}

void GPSData() { 
  if(GPS.fix) {
    GPSString = "" + String(GPS.latitudeDegrees, 4) + GPS.lat + "," + String(GPS.longitudeDegrees, 4) + GPS.lon + "," + String(GPS.altitude);
    Serial.println(GPSString);
  }
}

void EmergencyStop(String bytes_in) {
  tic1.setTargetPosition(tic1.getCurrentPosition());
  tic2.setTargetPosition(tic1.getCurrentPosition());
}
