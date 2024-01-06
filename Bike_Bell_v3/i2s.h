void writeI2S(void *parameter) {                                // Function to send data to the I2S board
  TheData = &vs[0];                                             // Set the date pointer to the begning of the char array
  size_t BytesWritten;                                          // Returns the no of written bytes
  int diff = 0;                                                 // Variable to store the remainder of the division between filesize and one time data array length
  while (1) {                                                   // Infinite While loop
    File file = SPIFFS.open(filename);                      // Opent the audio file from memory
    if (!file || file.isDirectory()) {                          
      Serial.println("- failed to open file for reading");
      return;
    }
    increment = 1024;                                           // No of bytes written in one iteration
    idx = 0;                                                    // Packet counter (no of bytes processed from the storage)
 
    while (file.available()) {                                  // check for the availability of files
      if (idx > 43) {                                           // If the counter is more then 43 then the data arrives in the wav file sequence, the rest is header
        if (idx >= upper) {
          increment = diff;
        }
        for (int i = 0; i < increment; i += 2) {                // Prepare Data which will be written to the DMA buffer of I2S
          value = 0;                                            //
          value = file.read();                                  //
          value = value | (file.read() << 8);                   //
          value = value *1; //vol;                                  // Adjust the data according to the output from accelerometer 
          vs[i] = lowByte(value);
          vs[i + 1] = highByte(value);
          //file.read();
          //file.read();
          idx += 2;
        }
        if (idx <= (filesize)) {
          i2s_write(i2s_num, TheData, increment, &BytesWritten, portMAX_DELAY); // Write the data to the DMA buffer of I2S
        }
        else {
          file.close();
          break;
        }
      }
      else {                                                                        //
        if (idx > 39) {                                                             ///
          dt[idx - 40] = file.read();                                               ////
        }                                                                           /////
        else {                                                                      //////
          char temp = file.read();                                                  /////// Gathers the required Data from the wav header
        }                                                                           //////
        if (idx == 43) {                                                            /////
          filesize = dt[0] + (dt[1] * 256) + (dt[2] * 65536) + (dt[3] * 16777216);  ////
          diff = filesize % 1024;                                                   ///
          upper = filesize - diff;                                                  //
        }
        idx++;
      }
      //vTaskDelay(1);
    }
  }
}
