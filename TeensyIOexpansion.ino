//Teensyduino
// Teensy 4.1
// USB mode USB+MIDI
// 600MHz


#include <Wire.h>// I2C

const int numberOfChannels = 24;//number of ATtiny1614 boards connected

byte buttonStates[numberOfChannels];//keeps current status of all buttons

//MIDI mapping each channel will have 8 notes
const byte noteMap[] = {60, 61, 62, 63, 64, 65, 66, 67}; //0,1,2,3.. 0,1,2,3 LEDs mapped to these notes, can make whatever notes you want
//note reference https://computermusicresource.com/midikeys.html

//functions

//IO stuff
void writeLED(byte channel, byte LED, bool state);
void scanButtons();

//MIDI stuff
void printBytes(const byte *data, unsigned int size);
void processMIDI(void);

void setup() {
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  scanButtons();//scans all buttons

  if (usbMIDI.read()) {//for MIDI messages to control LEDs
    processMIDI();
  }

}
