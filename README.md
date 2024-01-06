# Description:
This device makes use of ESP32 along with an accelerometer and I2S-based Audio Amplifier. A small sample
WAV file containing the ring bell sound is uploaded to the ESP32 flash storage. The motion is captured by
an IMU sensor. The bell audio is amplitude-modulated with the total acceleration captured by the IMU
sensor minus the acceleration due to gravity.

# Components Used:

*  ESP32 32-bit dual-core MCU clocked at 240Mhz with 2MB of Flash storage (SPI).
*  MPU6050 Inertial Measurement Unit.
*  MAX626 PCM Class D Amplifier with Class AB Performance.
*  Speaker 4W Speaker

# Operation:

*  The ESP32 MCU serves as the main processing unit, with a clock speed of 240 MHz and a flash
storage capacity of 2MB.
*  The ESP32 communicates with the MAX626 audio amplifier using the I2S protocol.
*  Sample WAV file containing the bell sound is uploaded to the ESP32 flash storage.
*  To overcome the slow read operations from the SPIFFS file system, the WAV sample is read into the
main memory and stored as an array.
*  The IMU sensor captures acceleration data, including the acceleration due to gravity, using the
I2C protocol.
*  The maximum range of acceleration is set to 4g (39.2 m/s²), and the volume level is mapped to
the 0-4g range of acceleration values.
*  Two separate tasks are created using FreeRTOS: one for reading acceleration data from the IMU
sensor and another for writing amplitude-modulated data to the I2S direct memory access
(DMA) buffer.
*  Acceleration data is read every 2 ms from the IMU sensor to regenerate the volume variable.
*  The remaining time after reading accelerometer data is utilized to scale the amplitude values of
the bell audio according to the volume variable, which is then fed to the I2S DMA buffer.
*  The previous two steps are repeated continuously to create the bell effect.

# Learnings/ Skills gained:
The development of the Digital Bike Bell involves the utilization of the following skills:
*  DMA and DMA interrupts: Direct Memory Access (DMA) is used to facilitate efficient data
transfer between memory and peripherals, and DMA interrupts are utilized to handle DMA
transfer events.
*  FreeRTOS: The Real-Time Operating System is employed to create tasks that run on the
scheduler, enabling concurrent execution of multiple operations.
*  I2S Protocol: The Inter-Integrated Sound protocol is used for audio data transmission between
the ESP32 and the MAX626 audio amplifier.
*  I2C Protocol: The Inter-Integrated Circuit protocol is utilized for communication with the IMU
sensor.
*  C++ Programming: Reading WAV files is implemented using C++.

# Schematics:

<img src="https://github.com/kripanshukumar/BIKE-BELL-USING-ACCELLEROMETER/blob/main/Schematic/Schematic.png" width=100% height=100%>
