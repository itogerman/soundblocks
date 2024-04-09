void initCompass() {
  // initialize device
  Serial.println();
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  compass.init();
  //compass.setMode(0x00, 0x0C, 0x00, 0xC0);
  compass.setSmoothing(10, true);
}

void readCompass() {
  compass.read();

  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  a = compass.getAzimuth();

  b = compass.getBearing(a);

  compass.getDirection(myArray, a);

  compassSmoothing();
}

void compassSmoothing() {
  totalX = totalX - readingsX[readIndexX]; // subtract the last reading
  readingsX[readIndexX] = x;   // read from the sensor
  totalX = totalX + readingsX[readIndexX];  // add the reading to the total
  readIndexX = readIndexX + 1; // advance to the next position in the array

  // if we're at the end of the array...
  if (readIndexX >= numReadings) {
    readIndexX = 0;   // ...wrap around to the beginning
  }

  averageX = totalX / numReadings;   // calculate the average

  //Serial.print(averageX);

  totalY = totalY - readingsY[readIndexY];
  readingsY[readIndexY] = y;
  totalY = totalY + readingsY[readIndexY];
  readIndexY = readIndexY + 1;

  if (readIndexY >= numReadings) {
    readIndexY = 0;
  }

  averageY = totalY / numReadings;

  totalZ = totalZ - readingsZ[readIndexZ];
  readingsZ[readIndexZ] = z;
  totalZ = totalZ + readingsZ[readIndexZ];
  readIndexZ = readIndexZ + 1;

  if (readIndexZ >= numReadings) {
    readIndexZ = 0;
  }

  averageZ = totalZ / numReadings;

  Serial.print("avgG-X: ");
  Serial.print(averageX);
  Serial.print(" - avgG-Y: ");
  Serial.print(averageY);
  Serial.print(" - avgG-Z: ");
  Serial.println(averageZ);

  if (mod = true) {
    modulation = map(averageX, -200, -500, -10, 10);
    modulation = constrain(modulation, -10, 10);

    frecuencyBase();
  }
}
