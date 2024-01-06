#include "variables.h"
#include "accelerometer.h"
#include "i2s.h"
#include "file.h"
#include"waveheader.h"

#define FORMAT_SPIFFS_IF_FAILED true

unsigned char WavData[44];

void setup() {
  Serial.begin(500000);                                    //Initialize the serial port
  delay(1000);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {            //Initialize the file system
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  //listDir(SPIFFS, "/", 0);

  i2s_driver_install(i2s_num, &i2s_config, 0, NULL);        // ESP32 will allocated resources to run I2S
  i2s_set_pin(i2s_num, &pin_config);                        // Tell it the pins you will be using
  //  i2s_set_pin(i2s_num, &pin_config);                        // Tell it the pins you will be using
  //i2s_set_clk(i2s_num, 44100,I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

  File file = SPIFFS.open(filename);                      // Opent the audio file from memory
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }
  int tmp = 0;
  while (file.available()) {
    WavData[tmp] = file.read();
    Serial.println(WavData[tmp]);
    tmp++;
    if (tmp >= 44) {
      file.close();
      break;
    }
  }


  memcpy(&WavHeader, &WavData, 44);                   // Copy the header part of the wav data into our structure
  DumpWAVHeader(&WavHeader);                          // Dump the header data to serial, optional!
  //  if(ValidWavData(&WavHeader))
  //  {
  //      i2s_driver_install(i2s_num, &i2s_config, 0, NULL);        // ESP32 will allocated resources to run I2S
  //      i2s_set_pin(i2s_num, &pin_config);                        // Tell it the pins you will be using
  //      i2s_set_sample_rates(i2s_num, WavHeader.SampleRate);      //set sample rate
  //      TheData=WavData;                                          // set to start of data
  //      TheData+=44;

  //    }

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Wire.setClock(400000);
  delay(1000);
  xTaskCreatePinnedToCore(
    writeI2S,                    // Function to be called
    "I2S WRITE",                 // Name of task
    8192,                        // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,                        // Parameter to pass to function
    configMAX_PRIORITIES - 1,    // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,                        // Task handle
    0);                          // Run on one core for demo purposes (ESP32 only)

  xTaskCreatePinnedToCore(
    mpu_read,                      // Function to be called
    "Accelero",                    // Name of task
    1024,                          // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,                          // Parameter to pass to function
    configMAX_PRIORITIES - 1,      // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,                          // Task handle
    app_cpu);                      // Run on one core for demo purposes (ESP32 only)
}

void loop() {
  // do nothing as the RTOS will handel each and every task
  //  Serial.println(I2S_CHANNEL_FMT_ONLY_LEFT);
  //  vTaskDelay(1000);
}
