void pwmOut(void *parameter) {                                                                   //Handels the PWM writing function
  while (1) {                                                                                    //Infinite loop
    value = (vol * pcmArray[arrayPointer]) + 128;                                                //Multiply the pwm value with the volume calculated by accelerometer sensor
    ledcWrite(0, value );                                                                        //Write the calculted pwm value to the pwm channel
    samplesCount++;                                                                              //Increment the samples per second counter variable
    arrayPointer++;                                                                              //Increment the array pointer
    //Serial.println(value); 
    if (millis() - tim >= 1000) {                                                                //Print the current sample rate on serial monitor
      Serial.println("Samples Rate: " +  String(samplesCount));                                
      samplesCount = 0;
      tim = millis();
      delay(2);                                                                                  //2 milli seconds delay to avoid overflowing watchdog timer
    }
    if (arrayPointer >= datalen) {                                                               //If the pwm array end is reaced then reset the array pointer
      arrayPointer = 0;                                                                      
    }
    //delay(1);
    delayMicroseconds(delayValue);                                                               //This delay is necessay to mimic the sample rate
  }
}

void prepareArray(String filename) {                                                              //Function which prepares the 8bit pwm array
  File file = SPIFFS.open(filename);                                                              //Open the Wav file stored in SPIFFS
  if (!file || file.isDirectory()) {                                                              //Check if file exist
    Serial.println("- failed to open file for reading");                                          //Print the error if file does not exist or SPIFFS not working and terminate the copy
  }
  else {                                                                                          //If file exists then prepare the 8 bit pwm array
    Serial.print(" \n\nFile Opened: ");
    Serial.println(file.name());
    for (int i = 0; i < 44; i++) {                                                                //Copy fist 44 bytes to decode the necessay information like bit rate, resolution, smple rate, etc
      header[i] = file.read();                                                                    //Reading from the SPIFFS
    }
    memcpy(&WavHeader, &header, 44);                                                              // Copy the header part of the wav data into our structure
    DumpWAVHeader(&WavHeader);                                                                    // Dump the header data to serial, optional!
    int32_t tempArray[8] = {0};                                                                   // temporary buffer to store the data from SPIFFS
    int bytesPerSample = bitsPerSample / 8;                                                       // Bytes required per sample
    int bytetoRead = (channels * bytesPerSample);                                                 // Total bytes to read per sample
    uint32_t effectiveSize = 0;                                                                   // Effective size
    arrayPointer = 0;                                                                             // Start the array pointer from 0
    effectiveSize = (dataSize) / (bytetoRead);                                                    // Effective size
    Serial.print("Effective size: "); Serial.println(effectiveSize);                              
    Serial.print("Available space: "); Serial.println(ARRAY_SIZE);
    if (effectiveSize > ARRAY_SIZE) {                                                             // Check if the lenght of Data exceeds the pwm buffer
      datalen = ARRAY_SIZE;                                                                       // If yes then the max length which will be coppied is equivalent to the size of pwm array
    }
    else {                                                                                        // Else
      datalen = effectiveSize;                                                                    // Data size coppied = the actual size of data
    }
    Serial.println("UPPER BOUND OF COPY: " + String(datalen));               
    while ((arrayPointer < datalen)) {                                                            // Check the array pointer value so that it always remain less than the length of pwm array
      for (int i = 0; i < bytetoRead; i++) {                                                      // Read the bytes which represent a single sample
        tempArray[i] = file.read();                                  
      }
      switch (bytetoRead) {                                                                       // Here, different sample rate, resolution and channel data is converted into same rate, 8 bit and single channel data.
        case 1:

          pcmArray[arrayPointer] = tempArray[0] - 128;
          break;

        case 2:
          //Serial.println("CASE2");
          if (channels == 1) {
            pcmArray[arrayPointer] =  ((tempArray[0]) | (tempArray[1] << 8)) / 256;
          }
          else {
            pcmArray[arrayPointer] = tempArray[0] - 128;
          }
          break;

        case 3:
          //Serial.println("CASE3");
          pcmArray[arrayPointer] = ((tempArray[2] << 16) | (tempArray[1] << 8) | (tempArray[0])) / 65536 ;
          break;

        case 4:
          //Serial.println("CASE4");
          if (channels == 1) {
            pcmArray[arrayPointer] = ((tempArray[3] << 24) | (tempArray[2] << 16 ) | (tempArray[1] << 8) | (tempArray[0])) / 16777216 ;
          }
          else {
            pcmArray[arrayPointer] = ((tempArray[1] << 8) | (tempArray[0])) / 256;
          }
          break;

        case 6:
          //Serial.println("CASE6");
          pcmArray[arrayPointer] = ((tempArray[2] << 16) | (tempArray[1] << 8) | (tempArray[0])) / 65536 ;
          break;

        case 8:
          //Serial.println("CASE8");
          pcmArray[arrayPointer] = ((tempArray[3] << 24) | (tempArray[2] << 16 ) | (tempArray[1] << 8) | (tempArray[0])) / 16777216 ;
          break;
      }
      arrayPointer++;
    }
    file.close();                                                                                       // Close the file after reading the data
    Serial.println("Done!! Array created with length= " + String(datalen));                     
    switch (rate) {                                                                                     // As the sample rate can be different for different wav file so we need different delay values to mimic the smaple rate using pwm
      case 8000:                                                                                        // Sample Rate: 8000Hz this sample rate is not recommended
        Serial.println(F("Sample Rate too low!!!! Not recommended"));
        delayValue = delayRateTable[RATE_8000];
        break;

      case 11025:                                                                                       // Sample Rate: 11025Hz Not recommended
        delayValue = delayRateTable[RATE_11025];
        break;

      case 16000:                                                                                       // Sample Rate: 16000Hz lowest recommended sample rate                                                                                   
        delayValue = delayRateTable[RATE_16000];
        break;

      case 22050:                                                                                       // Sample Rate: 22050 Hz
        delayValue = delayRateTable[RATE_22050];
        break;

      case 32000:                                                                                       // Sample Rate: 32000 Hz
        delayValue = delayRateTable[RATE_32000];
        break;

      case 44100:                                                                                       // Sample Rate: 44100 Hz
        delayValue = delayRateTable[RATE_44100];
        break;

      case 48000:                                                                                       // Sample Rate: 48000 Hz
        delayValue = delayRateTable[RATE_48000];
        break;
    }
  }
}
