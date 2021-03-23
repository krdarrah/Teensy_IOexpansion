
void writeLED(byte channel, byte LED, bool state) {
  byte LEDWriteByte = 0;
  bitWrite(LEDWriteByte, LED, 1);
  bitWrite(LEDWriteByte, LED + 4, state);//both LED indicator and state in one byte!
  // if writing led 1 HIGH, looks like this 00010001, or LOW would be 00000001, top 4 is for status, bottom is the LED number

  Wire.beginTransmission(channel);
  Wire.write(LEDWriteByte);
  Wire.endTransmission();
}

void scanButtons() {
  for (int i = 1; i <= numberOfChannels; i++) {// loop through all I2C addresses, note starting at 1
    byte newButtonState = 0;//whatever comes back each time we request
    Wire.requestFrom(i, 1);    // addr, numbytes
    while (Wire.available()) {
      newButtonState = Wire.read(); // current button status from this address
    }
    for (int  j = 0; j < 4; j++) {//now let's loop trhough bits of this status to see if anything has changed
      if (bitRead(newButtonState, j) != bitRead(buttonStates[i - 1], j)) {//comparing to last read bits to see if change
        //in here only if change
        //*** This code here re-maps what MIDI channel we're going to use ***
        //we need to divide our I2C addresses in half, so that
        // I2C   MIDI
        //  1     1
        //  2     1
        //  3     2
        //  4     2
        //  5     3
        //  5     3
        // and so on... 
        byte mappedChannel = i;//new MIDI channel we're going to use, so just set for I2C address
        bool useTop4Notes = false;// used to use top 4 notes from the noteMap[] array for second I2C address on same MIDI channel
        if (mappedChannel % 2 == 0) { //even
          //simple check to see number is even, so if it is, we'll divide by two to get MIDI channel we're going to use
          // example would be 4, that would need to be MIDI channel 2, and use top 4 notes
          mappedChannel = mappedChannel / 2;
          useTop4Notes = true;
        } else { //odd
          //this would be ODD, so just add one and divide by two to keep integer math and set our MIDI channel, 
          // same example if I2C address was 3, then we would get our MIDI channel of 2 and the bool to use top notes would be false
          mappedChannel = mappedChannel + 1;
          mappedChannel = mappedChannel / 2;
        }
        //*************************

        //now see if bit went from LOW to HIGH or other way around... we only want to send this message once on change
        if (bitRead(newButtonState, j)) {//PRESSED so turn note on, and based on if top notes should be used or not, use appropriate notes
          if (useTop4Notes)
            usbMIDI.sendNoteOn(noteMap[j + 4], 100, mappedChannel);//note, velocity,channel
          else
            usbMIDI.sendNoteOn(noteMap[j], 100, mappedChannel);//note, velocity,channel
        } else { //RELEASED, send note off
          if (useTop4Notes)
            usbMIDI.sendNoteOff(noteMap[j + 4], 100, mappedChannel);//note, velocity,channel
          else
            usbMIDI.sendNoteOff(noteMap[j], 100, mappedChannel);//note, velocity,channel
        }
      }
    }
    buttonStates[i - 1] = newButtonState;// this is key for this to work, need to now save current status
  }
}
