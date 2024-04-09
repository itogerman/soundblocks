void OSC_reset() {
  for (int i = 0; i < 128; i++) {
    noteOUT = i;
    velocityOUT = 0;

    OSCMessage msg_send("/SEND");
    msg_send.add(id);
    msg_send.add(noteOUT);
    msg_send.add(velocityOUT);
    for (int i = 0; i < nESP; i++) {
      outIp[3] = array_ids[i];
      Udp.beginPacket(outIp, outPort);
      msg_send.send(Udp);
      Udp.endPacket();
    }
    msg_send.empty();
  }
}

void showmsg(OSCMessage &msg_received) {
  id_received = msg_received.getInt(0);
  if (id_received != id) {
    noteIN = msg_received.getInt(1);
    velocityIN = msg_received.getInt(2);

    //resonance = random(0, 255);

    if ((id_received == 101) && (velocityIN != 0)) {
      int nro_track = random(1, 10);
      mp3.playTrack(nro_track);    //play track #1, don’t copy 0003.mp3 and then 0001.mp3, because 0003.mp3 will be played firts
    }

    if (id_received == 106) {
      mod = false;

      modulation = map(noteIN, noteMin, noteMax, -15, 15);
      modulation = constrain(modulation, -15, 15);

      frecuencyBase();
    }
    //else{
    //mod = true;
    //}

    Serial.print("ID: ");
    Serial.print(id_received);
    Serial.print(" - Received note ");
    Serial.print(noteIN);
    Serial.print(" - velocity ");
    Serial.println(velocityIN);
  }
}

void OSC_send() {
  if (touchValue < threshold) {
    digitalWrite(ledPin, HIGH);
    noteOUT = map(averageX, -200, -500, noteMin, noteMax);
    noteOUT = constrain(noteOUT, noteMin, noteMax);
    velocityOUT = map(averageY, -1200, -300, velMin, velMax); // x: -1200-+500, y: -1000-+0, z: +500-+2000,
    velocityOUT = constrain(velocityOUT, velMin, velMax);
  }
  else {
    digitalWrite(ledPin, LOW);
    if (noteON == true) {
      noteOUT = random(1, 97);
      velocityOUT = random(1, 128);
      //      noteOUT = map(averageX, -200, -500, noteMin, noteMax);
      //      noteOUT = constrain(noteOUT, noteMin, noteMax);
      //      velocityOUT = map(averageY, -1200, -300, velMin, velMax); // x: -1200-+500, y: -1000-+0, z: +500-+2000,
      //      velocityOUT = constrain(velocityOUT, velMin, velMax);
      noteON = false;
    } else {
      velocityOUT = 0;
      noteON = true;
    }
  }

  OSCMessage msg_send("/SEND");
  msg_send.add(id);
  msg_send.add(noteOUT);
  msg_send.add(velocityOUT);
  for (int i = 0; i < nESP; i++) {
    outIp[3] = array_ids[i];
    Udp.beginPacket(outIp, outPort);
    msg_send.send(Udp);
    Udp.endPacket();
  }
  msg_send.empty();
  Serial.print("Send note ");
  Serial.print(noteOUT);
  Serial.print(" - velocity ");
  Serial.println(velocityOUT);
}
