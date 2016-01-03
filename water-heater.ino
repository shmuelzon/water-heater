#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <InputDebounce.h>

const char *ssid PROGMEM = ".......";
const char *password PROGMEM = ".......";
const char *host PROGMEM = "water-heater";

#define RELAY_PIN 4
#define BUTTON_PIN 14

static InputDebounce button;

static bool getRelayState(void) {
  return digitalRead(RELAY_PIN) == HIGH;
}

static bool setRelayState(bool on) {
  digitalWrite(RELAY_PIN, on ? HIGH : LOW);
}

static void toggleRelay(void) {
  setRelayState(!getRelayState());
}

void setup() {
  /* GPIOs */
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

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

  /* Button */
  button.setup(BUTTON_PIN);
  button.registerCallbacks(toggleRelay, NULL, NULL);
}

void loop() {
  ArduinoOTA.handle();
  button.process(millis());
}
