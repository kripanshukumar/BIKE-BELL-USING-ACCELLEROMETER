#include "variables.h"
#include "waveheader.h"
#include "accelerometer.h"
#include "pwm.h"
#include "file.h"


#define FORMAT_SPIFFS_IF_FAILED true

void setup() {
  Serial.begin(115200);                                    //Initialize the serial port
  delay(1000);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {            //Initialize the file system
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  listDir(SPIFFS, "/", 0);
  prepareArray("/bell.wav");

  pinMode(22, OUTPUT);
  digitalWrite(22,HIGH);
  ledcSetup(0, freq, resolution);                   //Set the PWM frequency, resolution for LED
  ledcAttachPin(audioPin, 0);                       //Attach the declared led pin to the PWM channel
 
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Wire.setClock(400000);
  delay(10);
  
  xTaskCreatePinnedToCore(
    pwmOut,                    // Function to be called
    "PWM WRITE",                 // Name of task
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
    1);                      // Run on one core for demo purposes (ESP32 only)
}

void loop() {
  return;
  // do nothing as the RTOS will handel each and every task
  //  Serial.println(I2S_CHANNEL_FMT_ONLY_LEFT);
  //  vTaskDelay(1000);
}
