void mpu_read(void *parameter) {
  while (1) {                                           // Infinite While loop
    Wire.beginTransmission(MPU_addr);                   // Start communicaiton with the Accelerometer module
    Wire.write(0x3B);                                   // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 6, true);                // request a total of 14 registers
    AcX = Wire.read() << 8 | Wire.read();               // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read();               // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read();               // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    netAcc = pow(((pow(AcX, 2)) +                       //
                  (pow(AcY, 2)) +                       /// Calculate the average acceleration value 
                  (pow(AcZ, 2))), 0.5);                 //
    if (netAcc >= 10000) {                              // Avoids false data
      avgAcc = avgAcc - accBuff[bufferpostion];         // Removes the previous data from buffer
      avgAcc += netAcc;                                 // Add the new data
      accBuff[bufferpostion] = netAcc;                  // Store the new data in the buffer
      variance = abs(((avgAcc / samples) - netAcc));    // Calculate the variance (absolute value) to get a value for the amout of vibration
      if (variance >= 800) {                            // threshold for the lower limit of the variace ( basically sets the sensitivity level)
        vol = 1;//float(variance) / 5000;                   // Calculate the volume from the vibration gathered from the accelerometer
        if (vol >= 1) {                                 // If volume is greater than 1 then set the volume to 1
          vol = 1;
        }
        Serial.println(vol);
      }
      else {                                            // If no vibration then set the volume to 0
        vol = 0;       
      }
      bufferpostion++;                                  // variable to remember the position of acceleration buffer whose value is to be updated
      if (bufferpostion >= samples) {
        bufferpostion = 0;
      }
    }
    delay(2);
  }
}
