#include <Tic.h>
#include <SoftwareSerial.h>

TicI2C tic1(14);
TicI2C tic2(15);

SoftwareSerial SerialGPS(8, 7);

float TweakPan = 0;
float TweakTilt = 0;

float CurrentAnglePan = TweakPan;
float CurrentAngleTilt = TweakTilt;

float TargetAnglePan = 0;
float TargetAngleTilt = 0;

float ReferenceAnglePan = 0;
float ReferenceAngleTilt = 0;

const float StepConstant = 127.2889;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.setTimeout(5);

  SerialGPS.begin(9600);

  Wire.begin();
  delay(20);

  tic1.exitSafeStart();
  tic2.exitSafeStart();

  tic1.clearDriverError();
  tic2.clearDriverError();

  tic1.haltAndHold();
  tic2.haltAndHold();

  ReferenceAnglePan = CurrentAnglePan;
  ReferenceAngleTilt = CurrentAngleTilt;
}

void loop() {
  if (Serial.available() > 0) {

    float Var1, Var2;
    if (SerialInput(Var1, Var2)) {
      TargetAnglePan = Var1 + TweakPan;
      TargetAngleTilt = Var2 + TweakTilt;

      ShortestAngle();

      CurrentAnglePan = TargetAnglePan;
      CurrentAngleTilt = TargetAngleTilt;

      TrackerPan(CurrentAnglePan);
      TrackerTilt(CurrentAngleTilt);
    }
  }
}

bool SerialInput(float& Var1, float& Var2) {
  String Command = Serial.readStringUntil('\n');

  if (Command == "t+") {
    TweakTilt += 5;
  } else if (Command == "t-") {
    TweakTilt -= 5;
  } else if (Command == "p+") {
    TweakPan += 5;
  } else if (Command == "p-") {
    TweakPan -= 5;
  } else {
    int Index = Command.indexOf(',');
    if (Index != -1) {
      Var1 = Command.substring(0, Index).toFloat();
      Var2 = Command.substring(Index + 1).toFloat();

      return true;
    }

    return false;
  }
}

void ShortestAngle() {
  float Delta = TargetAnglePan - fmod(CurrentAnglePan - ReferenceAnglePan, 361);
  bool Clockwise = Delta <= 180;

  if (Clockwise) {
    TargetAnglePan = CurrentAnglePan + Delta;
  } else {
    TargetAnglePan = CurrentAnglePan + Delta-360;
  }

  if (TargetAngleTilt < -20)
    TargetAngleTilt = -10;
  if (TargetAngleTilt > 90)
    TargetAngleTilt = 90;
}

void TrackerPan(float CurrentAnglePan1) {
  tic1.exitSafeStart();
  int long TargetPosition = -StepConstant * CurrentAnglePan1;
  tic1.setTargetPosition(TargetPosition);
  DelayPos1(TargetPosition);
}

void TrackerTilt(float CurrentAngleTilt) {
  tic2.exitSafeStart();
  int TargetPosition = static_cast<int>(StepConstant * CurrentAngleTilt);
  tic2.setTargetPosition(TargetPosition);
  DelayPos2(TargetPosition);
}

void DelayPos1(int TargetPosition) {
  while (tic1.getCurrentPosition() != TargetPosition) {
    resetCommandTimeout();
  }
  tic1.haltAndHold();
}

void DelayPos2(int TargetPosition) {
  while (tic2.getCurrentPosition() != TargetPosition) {
    resetCommandTimeout();
  }
  tic2.haltAndHold();
}

void resetCommandTimeout() {
  tic1.resetCommandTimeout();
  tic2.resetCommandTimeout();
}