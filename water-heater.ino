#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>
#include <InputDebounce.h>

const char *ssid PROGMEM = ".......";
const char *password PROGMEM = ".......";
const char *host PROGMEM = "water-heater";

#define RELAY_PIN 4
#define NEOPIXEL_PIN 5
#define BUTTON_PIN 14

#define NUM_PIXELS 4

static Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
static InputDebounce button;

static bool getRelayState(void) {
  return digitalRead(RELAY_PIN) == HIGH;
}

static bool setRelayState(bool on) {
  uint32_t color = on ? 0xFF0000 : 0x000000;

  digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  for (int i = 0; i < NUM_PIXELS; i++)
    pixels.setPixelColor(i, color);
  pixels.show();
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

  /* NeoPixles */
  pixels.begin();
  pixels.setBrightness(16);
  pixels.show();
}

void loop() {
  ArduinoOTA.handle();
  button.process(millis());
}
