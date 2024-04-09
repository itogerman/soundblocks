void initI2C(){
    // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif
}

void initDFPlayer() {
  mp3Serial.begin(MP3_SERIAL_SPEED, SWSERIAL_8N1, MP3_RX_PIN, MP3_TX_PIN, false, MP3_SERIAL_BUFFER_SIZE, 0); //false=signal not inverted, 0=ISR/RX buffer size (shared with serial TX buffer)
  mp3.begin(mp3Serial, MP3_SERIAL_TIMEOUT, DFPLAYER_HW_247A, false); //"DFPLAYER_HW_247A" see NOTE, false=no feedback from module after the command

  mp3.stop();                             //if player was runing during ESP8266 reboot
  mp3.reset();                            //reset all setting to default

  mp3.setSource(2);                       //1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash
  mp3.setEQ(0);                           //0=Off, 1=Pop, 2=Rock, 3=Jazz, 4=Classic, 5=Bass
  mp3.setVolume(30);                      //0..30, module persists volume on power failure

  mp3.sleep();                            //inter sleep mode, 24mA
  mp3.wakeup(2);                          //exit sleep mode & initialize source 1=USB-Disk, 2=TF-Card, 3=Aux, 5=NOR Flash
  mp3Serial.enableRx(true);               //enable interrupts on RX-pin for better response detection, less overhead than mp3Serial.listen()

  Serial.print("DFPlayer Status: ");
  Serial.print(mp3.getStatus());        //0=stop, 1=playing, 2=pause, 3=sleep or standby, 4=communication error, 5=unknown state
  Serial.print( " - Volume: ");
  Serial.print(mp3.getVolume());        //0..30
  Serial.print(" - Command: ");
  Serial.println(mp3.getCommandStatus()); //1=module busy, 2=module sleep, 3=request not fully received, 4=checksum not match, 5=requested folder/track out of range,
  //6=requested folder/track not found, 7=advert available while track is playing, 8=SD card not found, 9=???, 10=module sleep
  //11=OK command accepted, 12=OK playback completed, 13=OK module ready after reboot
  mp3Serial.enableRx(false);              //disable interrupts on RX-pin, less overhead than mp3Serial.listen()
  //  //mp3.playMP3Folder(1); //1=track, folder name must be "mp3" or "MP3" & files in folder must start with 4 decimal digits with leading zeros
  //  //mp3.playFolder(1, 2); //1=folder/2=track, folder name must be 01..99 & files in folder must start with 3 decimal digits with leading zeros
  //  mp3.pause();
}
