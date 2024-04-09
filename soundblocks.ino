#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

char ssid[] = "soundblocks";        // Network SSID (name)
char pass[] = "soundblocks";                     // Network password

WiFiUDP Udp;                                    // A UDP instance to let us send and receive packets over UDP
int array_ids[] = {101, 106, 107, 108};
int nESP = 4;
IPAddress outIp(192, 168, 10, 101); // remote IP of your computer
const unsigned int outPort = 12000;  // remote port to receive OSC
const unsigned int localPort = 12000;  // local port to listen for OSC packets (actually not used for sending)

int id = 105;
IPAddress ip(192, 168, 10, id);  //ip que va a tener la placa
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);

OSCErrorCode error;

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high
//MPU6050 accelgyro(0x68, &Wire1); // <-- use for AD0 low, but 2nd Wire (TWI/I2C) object

int16_t ax, ay, az;
int16_t gx, gy, gz;

#include <QMC5883LCompass.h>

QMC5883LCompass compass;

int x, y, z, a, b;
char myArray[3];

int MODE = 0x00; //0x00 = Standby, 0x01 = Continuous
int ODR = 0x00; // OUTPUT DATA RATE, 0x00 = 10Hz, 0x04 = 50Hz, 0x08 = 100Hz, 0x0C = 200Hz
int RNG = 0x10; // FULL SCALE 0x00 = 2G, 0x10 = 8G
int OSR = 0xC0; // OVER SAMPLE RATIO, 0xC0 = 64,0x80 = 128, 0x40 = 256, 0x00 = 512

const int numReadings = 100;
float readingsX[numReadings];
float readingsY[numReadings];
float readingsZ[numReadings];
int readIndexX = 0;
int readIndexY = 0;
int readIndexZ = 0;
int totalX = 0;
int totalY = 0;
int totalZ = 0;
float averageX = 0;
float averageY = 0;
float averageZ = 0;

const int touchPin = 4; 
const int ledPin = 5;

const int threshold = 15;
int touchValue;

#define MP3_RX_PIN              16     //GPIO4/D2 to DFPlayer Mini TX
#define MP3_TX_PIN              17     //GPIO5/D1 to DFPlayer Mini RX
#define MP3_SERIAL_SPEED        9600  //DFPlayer Mini suport only 9600-baud
#define MP3_SERIAL_BUFFER_SIZE  32    //software serial buffer size in bytes, to send 8-bytes you need 11-bytes buffer (start byte+8-data bytes+parity-byte+stop-byte=11-bytes)
#define MP3_SERIAL_TIMEOUT      350   //average DFPlayer response timeout for GD3200B chip 350msec..500msec

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <mozzi_fixmath.h> // for Q16n16 fixed-point fractional number type

#include <SoftwareSerial.h>
#include <DFPlayer.h>

#define CONTROL_RATE 256
#define NUM_VOICES 8
#define THRESHOLD 10
// harmonics
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6(COS8192_DATA);
//Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7(COS8192_DATA); // used to work smoothly in Arduino 1.05

// duplicates but slightly off frequency for adding to originals
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6b(COS8192_DATA);
//Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7b(COS8192_DATA);

// base pitch frequencies in Q16n16 fixed int format (for speed later)
Q16n16 f1, f2, f3, f4, f5, f6; //,f7;

Q16n16 variation() {
  // 32 random bits & with 524287 (b111 1111 1111 1111 1111)
  // gives between 0-8 in Q16n16 format
  return  (Q16n16) (xorshift96() & 524287UL);
}

boolean mod = true;
int modulation = 0;

SoftwareSerial mp3Serial;
DFPlayer       mp3;

int noteIN, velocityIN, id_received;
int noteOUT, velocityOUT;

int noteMin = 36;
int noteMax = 96;
int velMin = 64;
int velMax = 127;

boolean noteON = true;

unsigned long refTime = millis();
long interval;

long minRandom = 20000;
long maxRandom = 50000;


void setup() {
  initI2C();
  Serial.begin(115200);
  initWiFi();
  randomSeed(analogRead(34));
  OSC_reset();
  initCompass();
  intervaler();
  //resonance = random(0, 255);
  pinMode(ledPin, OUTPUT);
  initMozzi();
  //initDFPlayer();
}

void loop() {
  audioHook();
}

void intervaler() {
  interval = random(minRandom, maxRandom);
  Serial.print("ID: ");
  Serial.print(id);
  Serial.print(" - Next msgSend: ");
  Serial.println(interval);
}

void updateControl() {
  touchValue = touchRead(touchPin);
  Serial.println(touchValue);
  
  readCompass();

  //if (millis() - refTime >= interval) {
    //intervaler();
    OSC_send();
    //refTime = millis();
  //}

  OSCMessage msg_received;
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msg_received.fill(Udp.read());
    }
    if (!msg_received.hasError()) {
      msg_received.dispatch("/SEND", showmsg);
    } else {
      error = msg_received.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
  
  // todo: choose a nice scale or progression and make a table for it
  // or add a very slow gliss for f1-f7, like shephard tones

  // change frequencies of the b oscillators
  switch (lowByte(xorshift96()) & 7) { // 7 is 0111

    case 0:
      aCos1b.setFreq_Q16n16(f1 + variation());
      break;

    case 1:
      aCos2b.setFreq_Q16n16(f2 + variation());
      break;

    case 2:
      aCos3b.setFreq_Q16n16(f3 + variation());
      break;

    case 3:
      aCos4b.setFreq_Q16n16(f4 + variation());
      break;

    case 4:
      aCos5b.setFreq_Q16n16(f5 + variation());
      break;

    case 5:
      aCos6b.setFreq_Q16n16(f6 + variation());
      break;
      /*
        case 6:
         aCos7b.setFreq_Q16n16(f7+variation());
        break;
      */
  }
}

AudioOutput_t updateAudio() {

  int asig =
    aCos1.next() + aCos1b.next() +
    aCos2.next() + aCos2b.next() +
    aCos3.next() + aCos3b.next() +
    aCos4.next() + aCos4b.next() +
    aCos5.next() + aCos5b.next() +
    aCos6.next() + aCos6b.next();// +
  // aCos7.next() + aCos7b.next();

  return MonoOutput::fromAlmostNBit(12, asig);
}
