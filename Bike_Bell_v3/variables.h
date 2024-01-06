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

// I2S configuations and variables
static const i2s_port_t i2s_num = I2S_NUM_0;  // i2s port number
int32_t idx = 0;
int increment = 0;
unsigned char vs[1024];
unsigned char* TheData;
uint32_t filesize = 0;
char dt[4] = {0, 0, 0, 0};
uint32_t upper = 0;
int16_t value = 0;

//accelerometer variables
#define samples 40
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int32_t netAcc = 0;
int32_t accBuff[samples];
int32_t avgAcc = 0;
int bufferpostion = 0;
int32_t variance = 0;
float vol = 0;

//Audio file
String filename = "/bell8.wav";

//========================================================================================================================
// I2S configuration structures

static const i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
  .sample_rate = 44100,                            // Note, this will be changed later
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,       // high interrupt priority
  .dma_buf_count = 8,                             // 8 buffers
  .dma_buf_len = 1024,                            // 1K per buffer, so 8K of buffer space
  .use_apll = 0,
  .tx_desc_auto_clear = true,
  .fixed_mclk = -1
};

static const i2s_pin_config_t pin_config = {
  .bck_io_num = 26,                                 // The bit clock connectiom, goes to pin 27 of ESP32
  .ws_io_num = 27,                                  // Word select, also known as word select or left right clock
  .data_out_num = 25,                               // Data out from the ESP32, connect to DIN on 38357A
  .data_in_num = I2S_PIN_NO_CHANGE                  // we are not interested in I2S data into the ESP32
};

//========================================================================================================================

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
