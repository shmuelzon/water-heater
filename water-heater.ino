#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

const char *ssid PROGMEM = ".......";
const char *password PROGMEM = ".......";
const char *host PROGMEM = "water-heater";

void setup() {
  /* WiFi */
  WiFi.mode(WIFI_STA);
  WiFi.hostname(host);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
  }

  /* OTA */
  ArduinoOTA.setHostname(host);
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
}
