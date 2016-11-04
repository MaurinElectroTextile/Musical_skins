/*
  E-TEXTILE MATRIX SENSOR : Copyright (c) 2014 Maurin Donneaud http://etextile.org
  Purpose: 16x16 e-textile sensors matrix
  Licence : GNU GENERAL PUBLIC LICENSE
  
  programmed for microcontroller Teensy 3.1 with Arduino IDE
  Install Teensyduino : http://pjrc.com/teensy/teensyduino.html
  Audio lib from pjrc : http://www.pjrc.com/teensy/td_libs_Audio.html

  Settings for Arduino IDE
    Board:           Teensy 3.2 / 3.1
    USB Type:        Serial
    CPU Speed:       72 MHz
    Keyboard Layout: US English
*/


// USB and STANDALONE mode

#include <PacketSerial.h>

PacketSerial serial;

/*
  PACKET SERIAL : Copyright (c) 2012-2014 Christopher Baker <http://christopherbaker.net>
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#define STANDALONE
#define USB_TRANSMIT

#if defined(STANDALONE)
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#endif

#define  BAUD_RATE        230400 // With Teensy, it's always the same native speed. The baud rate setting is ignored (USB speed is always 12 Mbit/sec).

#define  ROW              16
#define  COL              16
#define  DATAS            512
#define  LED_PIN_A        13
#define  LED_PIN_B        37 // 37 = A13 : FIXME change pin on PCB
#define  BUTTON_PIN       32
#define  FRAME_RATE       20

#define  THRESHOLD        160
#define  DEBOUNCE_TIME    100

// Digital pins array
// See the attached PCB (Eagle file) to understand the digital and analog pins mapping
const int rowPins[ROW] = {
  27, 26, 25, 24, 12, 11, 10, 9, 8, 7, 6, 5, 33, 2, 1, 0
};

// Analog pins array
const int columnPins[COL] = {
  A17, A18, A19, A0, A20, A1, A2, A3, A4, A5, A6, A7, A11, A8, A10, A9
};

uint8_t myPacket[DATAS];
boolean scan = false;

int values[ROW][COL];
long lastSensTime[ROW][COL];
boolean keyState[ROW][COL];

boolean DEBUG_MATRIX = false;
boolean DEBUG_SWITCHS = false;
boolean DEBUG_SYNTH = false;
boolean DEBUG_TRANSMIT_USB = false;
boolean DEBUG_MIDI_USB = false;

#if defined(STANDALONE)

#include "Freq.h"

#define  SYNTH    8         // Synthesizers

typedef struct wavedef {
  AudioSynthWaveform *waveform;
  float pulseW;
  float vol;
  float freq;
  short int waveType;
};

typedef struct envdef {
  AudioEffectEnvelope *envelope;
  float att;
  float hol;
  float dec;
  float sus;
  float rel;
};

typedef struct synth {
  boolean state;              // true:free - false:busy
  boolean toggelPlay;         // true:free - false:busy
  int sensorID;               // 0 to 255 (16_16 matrix)
  unsigned int sensorValue;   // 1 to 1024
  int metro;                  // time betwin two notes in reapeat mode
  int timeON;                 // tim of a mote
  unsigned long lastPlayTime; //
  struct wavedef wave;
  struct envdef env;
};

AudioSynthWaveform waveform1, waveform2, waveform3, waveform4;
AudioSynthWaveform waveform5, waveform6, waveform7, waveform8;
AudioEffectEnvelope envelope1, envelope2, envelope3, envelope4;
AudioEffectEnvelope envelope5, envelope6, envelope7, envelope8;

synth allSynth[SYNTH] = {
  { true, true, -1, 0, 2000, 500, 0, { &waveform1, 0.5, 0.25, 440, 0 }, { &envelope1, 10, 2.1, 15, 6, 500 } },
  { true, true, -1, 0, 2000, 500, 0, { &waveform2, 0.5, 0.25, 440, 1 }, { &envelope2, 10, 2.1, 15, 6, 500 } },
  { true, true, -1, 0, 2000, 500, 0, { &waveform3, 0.5, 0.25, 440, 2 }, { &envelope3, 10, 2.1, 15, 6, 500 } },
  { true, true, -1, 0, 2000, 500, 0, { &waveform4, 0.5, 0.25, 440, 3 }, { &envelope4, 10, 2.1, 15, 6, 500 } },
  { true, true, -1, 0, 2000, 500, 0, { &waveform5, 0.5, 0.25, 440, 4 }, { &envelope5, 10, 2.1, 15, 6, 500 } },
  { true, true, -1, 0, 2000, 500, 0, { &waveform6, 0.5, 0.25, 440, 5 }, { &envelope6, 10, 2.1, 15, 6, 500 } },
  { true, true, -1, 0, 2000, 500, 0, { &waveform7, 0.5, 0.25, 440, 0 }, { &envelope7, 10, 2.1, 15, 6, 500 } },
  { true, true, -1, 0, 2000, 500, 0, { &waveform8, 0.5, 0.25, 440, 1 }, { &envelope8, 10, 2.1, 15, 6, 500 } }
};

AudioMixer4  mixer1;
AudioMixer4  mixer2;
AudioMixer4  mixer3;
AudioOutputAnalog  DAC1;

AudioConnection  patchCord1(waveform1, envelope1);
AudioConnection  patchCord2(waveform2, envelope2);
AudioConnection  patchCord3(waveform3, envelope3);
AudioConnection  patchCord4(waveform4, envelope4);
AudioConnection  patchCord5(waveform5, envelope5);
AudioConnection  patchCord6(waveform6, envelope6);
AudioConnection  patchCord7(waveform7, envelope7);
AudioConnection  patchCord8(waveform8, envelope8);

AudioConnection  patchCord9(envelope1, 0, mixer1, 0);
AudioConnection  patchCord10(envelope2, 0, mixer1, 1);
AudioConnection  patchCord11(envelope3, 0, mixer1, 2);
AudioConnection  patchCord12(envelope4, 0, mixer1, 3);
AudioConnection  patchCord13(envelope5, 0, mixer2, 0);
AudioConnection  patchCord14(envelope6, 0, mixer2, 1);
AudioConnection  patchCord15(envelope7, 0, mixer2, 2);
AudioConnection  patchCord16(envelope8, 0, mixer2, 3);

AudioConnection  patchCord17(mixer1, 0, mixer3, 0);
AudioConnection  patchCord18(mixer2, 0, mixer3, 1);
AudioConnection  patchCord19(mixer3, DAC1);

int AudioMemoryVal = 0;

#endif // STANDALONE

volatile boolean communicationState = false;
unsigned long lastFrameTime = 0;

/////////////////////// INITIALISATION
void setup() {

  // We must specify a packet handler method so that
  serial.setPacketHandler(&onPacket);
  serial.begin(BAUD_RATE);

  analogReadRes(10);                               // Set the ADC converteur resolution to 10 bit
  pinMode(LED_PIN_A, OUTPUT);                      // Set rows pins in high-impedance state
  // pinMode(LED_PIN_B, OUTPUT);                   // FIXME change pin on PCB
  // digitalWrite(LED_PIN_B, HIGH);                // FIXME change pin on PCB
  pinMode(BUTTON_PIN, INPUT_PULLUP);               // Set rows pins in high-impedance state
  attachInterrupt(BUTTON_PIN, pushButton, RISING); // interrrupt 1 is data ready

  for ( int i = 0; i < ROW; i++ ) {
    pinMode( rowPins[ROW], INPUT );        // Set rows pins in high-impedance state
  }

#if defined( STANDALONE )
  // Allocate audio memory samples blocks
  AudioMemory(10); // Use emoryUsage to set the value

  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);

  mixer2.gain(0, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);

  mixer3.gain(0, 0.25);
  mixer3.gain(1, 0.25);
  mixer3.gain(2, 0.25);
  mixer3.gain(3, 0.25);

  // Init all synthetizers
  for (int i = 0; i < SYNTH; i++) {
    allSynth[i].timeON = allSynth[i].env.att + allSynth[i].env.hol + allSynth[i].env.dec + allSynth[i].env.sus;
    allSynth[i].wave.waveform -> amplitude( allSynth[i].wave.vol );
    allSynth[i].wave.waveform -> pulseWidth( allSynth[i].wave.pulseW );
    allSynth[i].env.envelope -> attack( allSynth[i].env.att );
    allSynth[i].env.envelope -> hold( allSynth[i].env.hol );
    allSynth[i].env.envelope -> decay( allSynth[i].env.dec );
    allSynth[i].env.envelope -> sustain( allSynth[i].env.sus );
    allSynth[i].env.envelope -> release( allSynth[i].env.rel );
  }
#endif

  bootBlink(9);
  while (!Serial.dtr()); // wait for user to start the serial monitor
  bootBlink(5);
  delay(1000);
}

//////////////////////////////////////////////////////////// BOUCLE PRINCIPALE
void loop() {

#if defined(USB_TRANSMIT)
  UsbTransmit();
#endif

#if defined(STANDALONE)
  scanStandaloneMatrix();
  updateStandaloneSynth();
  // memoryUsage(); // 
#endif

#if defined(MIDI_USB)
  UsbMidiTransmit();
#endif

}

/////////////////////// USB TRANSMIT BYTES
// See : https://www.pjrc.com/teensy/td_serial.html

#if defined(USB_TRANSMIT)
void UsbTransmit() {

  if (scan) {
    for (int row = 0; row < ROW; row++) {
      // Set row pin as output + 3.3V
      pinMode(rowPins[row], OUTPUT);
      digitalWrite(rowPins[row], HIGH);
      for (int column = 0; column < COL; column++) {
        int value = analogRead(columnPins[column]); // Read the sensor value
        int sensorID = row * ROW + column;
        int bytePos = sensorID * 2;
        myPacket[bytePos] = lowByte(value);       // Write lowByte
        myPacket[bytePos + 1] = highByte(value);  // Write highByte
      }
      // Set row pin in high-impedance state
      pinMode(rowPins[row], INPUT);
    }
    scan = false;
  }

  // The update() method attempts to read in
  // any incoming serial data and emits packets via
  // the user's onPacket(const uint8_t* buffer, size_t size)
  // method registered with the setPacketHandler() method.
  // The update() method should be called at the end of the loop().
  serial.update();
}

// This is our packet callback.
// The buffer is delivered already decoded.
void onPacket(const uint8_t* buffer, size_t size) {

  // The send() method will encode the buffer
  // as a packet, set packet markers, etc.
  serial.send(myPacket, DATAS);
  scan = true;
}
#endif // USB_TRANSMIT

/////////// SCAN THE SENSORS MATRIX
#if defined(STANDALONE)

void scanStandaloneMatrix() {

  if (communicationState == false) {
    for (int row = 0; row < ROW; row++) {
      // Set row pin as output + 3.3V
      pinMode(rowPins[row], OUTPUT);    // Set row pin as OUTPUT
      digitalWrite(rowPins[row], HIGH); // Set row pin HIGH (+3V)
      for (int column = 0; column < COL; column++ ) {
        int sensorID = row * ROW + column; // +1 ?
        values[row][column] = analogRead(columnPins[column]); // Read and store the sensor value
        if (DEBUG_MATRIX) Serial.print(values[row][column]), Serial.print("\t");

        // If a sensor is touched
        if (values[row][column] > THRESHOLD && keyState[row][column] == true && ( lastSensTime[row][column] - millis() ) > DEBOUNCE_TIME) {
          lastSensTime[row][column] = millis();
          keyState[row][column] = false;     // Key is pressed
          int sensorID = row * ROW + column; // +1 ?
          if (DEBUG_SWITCHS) Serial.print(sensorID), Serial.print(" "), Serial.println("ON");

          // Select a free synthesizer
          for (int i = 0; i < SYNTH; i++) {
            if (allSynth[i].state == true) {                           // State is true : synth is FREE
              allSynth[i].state = false;                               // State is false : synth is BUSY
              allSynth[i].sensorID = sensorID;                         // Remind the sensor id
              allSynth[i].wave.freq = freq[row][column];               // Set the synthesizer frequancy
              // allSynth[i].wave.waveType = wave[row][column];        // Set the synthesizer wave type
              allSynth[i].wave.waveType = 1;                           // Set the synthesizer wave type
              if (DEBUG_SYNTH) Serial.print(i), Serial.print(" "), Serial.println("BUSY");
              break;
            }
          }
        }
        // Release the synthesizer
        if (values[row][column] <= THRESHOLD && keyState[row][column] == false) {
          keyState[row][column] = true; // key is released

          // Clear the synth
          for (int i = 0; i < SYNTH; i++) {
            if (allSynth[i].sensorID == sensorID) {
              if (DEBUG_SYNTH) Serial.print(i), Serial.print(" "), Serial.println("FREE");
              allSynth[i].sensorID = -1;
              allSynth[i].state = true; // state is true : SYNTH IS FREE
              break;
            }
          }
          if (DEBUG_SWITCHS) Serial.print(sensorID), Serial.print(" "), Serial.println("OFF");
        }
      }
      // Set row pin in high-impedance state
      pinMode(rowPins[row], INPUT);
      if (DEBUG_MATRIX) Serial.println();
    }
  }
}

/////////// STANDALONE SEQ
#if defined(STANDALONE)

void updateStandaloneSynth() {

  if (communicationState == false) {
    for (int synthIndex = 0; synthIndex < SYNTH; synthIndex++) {

      // Volume control // DO NOT WORK : FIXME
      // byte column = ( allSynth[synthIndex].sensorID - 1 ) % 16;
      // byte row = (int) ( ( allSynth[synthIndex].sensorID - 1 ) / 16 );
      // float velocity = values[row][column] / 1024;

      // allSynth[synthIndex].wave.waveform -> amplitude( velocity ); // DO NOT WORK !?

      // PLAY
      // if( ( millis() - allSynth[synthIndex].lastPlayTime ) >= allSynth[synthIndex].metro && allSynth[synthIndex].state == false && allSynth[synthIndex].toggelPlay == false ){
      if ( allSynth[synthIndex].state == false && allSynth[synthIndex].toggelPlay == false ) { // state is false == BUSY_SYNTH
        allSynth[synthIndex].toggelPlay = true;
        allSynth[synthIndex].wave.waveform -> begin( allSynth[synthIndex].wave.vol, allSynth[synthIndex].wave.freq, allSynth[synthIndex].wave.waveType ); // WAVEFORM_SINE
        allSynth[synthIndex].env.envelope -> noteOn();
      }

      // STOP
      // if( ( millis() - allSynth[synthIndex].lastPlayTime ) >= ( allSynth[synthIndex].metro + allSynth[synthIndex].timeON ) &&
      // allSynth[synthIndex].state == true && allSynth[synthIndex].toggelPlay == true ){
      if ( allSynth[synthIndex].state == true && allSynth[synthIndex].toggelPlay == true ) { // state is true == FREE_SYNTH
        allSynth[synthIndex].toggelPlay = false;
        allSynth[synthIndex].env.envelope -> noteOff();
      }
    }
  }
}
#endif


///////////  test audio memory
void  memoryUsage() {

  AudioMemoryVal = AudioMemoryUsageMax();
  Serial.println(AudioMemoryVal);
  delay(1000);
}
#endif

/////////// Called with interrupt triggered with push button attached to I/O pin 32
void  pushButton() {

  cli();
  communicationState = !communicationState;
  digitalWrite( LED_PIN_A, communicationState );
  sei();
}


void bootBlink( int flash ) {

  for ( int i = 0; i < flash; i++ ) {
    digitalWrite( LED_PIN_A, HIGH );
    delay(50);
    digitalWrite( LED_PIN_A, LOW );
    delay(50);
  }

}
