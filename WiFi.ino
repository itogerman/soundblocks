void initWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // se podrá reducir el tiempo???
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif
  Serial.println();
}
