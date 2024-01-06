#include "FS.h"
#include "SPIFFS.h"
#include <Wire.h>
#include "driver/i2s.h"                       // Library of I2S routines, comes with ESP32 standard install

//============================================Variables==================================================//

#define FORMAT_SPIFFS_IF_FAILED true

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// pwm Configurations and Variables
#define ARRAY_SIZE 100000
#define audioPin 19
uint32_t t = 0;
uint8_t value = 0;
int delayValue = 0;
const int freq = 150000;
const int resolution = 8;
uint32_t samplesCount = 0, tim  = 0;
int8_t pcmArray[ARRAY_SIZE] = {0};
uint32_t arrayPointer = 0, datalen = 0;
uint16_t delayRateTable[7] = {115, 82, 54, 36, 22, 15, 12};


//accelerometer variables
#define samples 10
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int32_t netAcc = 0;
int32_t accBuff[samples];
int32_t avgAcc = 0;
int bufferpostion = 0;
int32_t variance = 0;
float vol = 0;
int sensitivity = 400;

// WAV Header Variables
#define RATE_8000 0
#define RATE_11025 1
#define RATE_16000 2
#define RATE_22050 3
#define RATE_32000 4
#define RATE_44100 5
#define RATE_48000 6

uint32_t rate = 0;
uint32_t channels = 0;
uint32_t dataSize = 0;
uint8_t header[44] = {0};
uint32_t bitsPerSample = 0;

//Audio file
String audioFile = "/bell3.wav";

//========================================================================================================================
// WAV HEADER STRUCTURE
struct WavHeader_Struct
{
  //   RIFF Section
  char RIFFSectionID[4];      // Letters "RIFF"
  uint32_t Size;              // Size of entire file less 8
  char RiffFormat[4];         // Letters "WAVE"

  //   Format Section
  char FormatSectionID[4];    // letters "fmt"
  uint32_t FormatSize;        // Size of format section less 8
  uint16_t FormatID;          // 1=uncompressed PCM
  uint16_t NumChannels;       // 1=mono,2=stereo
  uint32_t SampleRate;        // 44100, 16000, 8000 etc.
  uint32_t ByteRate;          // =SampleRate * Channels * (BitsPerSample/8)
  uint16_t BlockAlign;        // =Channels * (BitsPerSample/8)
  uint16_t BitsPerSample;     // 8,16,24 or 32

  // Data Section
  char DataSectionID[4];      // The letters "data"
  uint32_t DataSize;          // Size of the data that follows
} WavHeader;

//========================================================================================================================
