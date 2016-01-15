#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>
#include <InputDebounce.h>
#include <ArduinoJson.h>

const char *ssid PROGMEM = ".......";
const char *password PROGMEM = ".......";
const char *host PROGMEM = "water-heater";

#define RELAY_PIN 4
#define NEOPIXEL_PIN 5
#define BUTTON_PIN 14

#define NUM_PIXELS 4

static ESP8266WebServer server(80);
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

static void handleNotFound(void) {
  server.send(404, "text/plain", "File Not Found");
}

static void setResponseHeaders(void) {
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.sendHeader("Access-Control-Allow-Methods", "GET,PUT,OPTIONS");
}

static void handleOptions(void) {
  setResponseHeaders();
  server.sendHeader("Allow", "GET,PUT,OPTIONS");
  server.send(200);
}

static void handleRelayGet(void) {
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  String res;

  root["state"] = getRelayState() ? "on" : "off";

  root.printTo(res);
  setResponseHeaders();
  server.send(200, "application/json", res);
}

static void handleRelayPut(void) {
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject &json_parsed = jsonBuffer.parseObject(server.arg("plain"));

  if (!json_parsed.success()) {
    server.send(400, "plain/text", "Failed parsing JSON payload");
    return;
  }

  if (!strcmp(json_parsed["state"], "on"))
    setRelayState(true);
  else if (!strcmp(json_parsed["state"], "off"))
    setRelayState(false);
  else {
    server.send(400, "plain/text", "Invalid value for 'state'");
    return;
  }

  handleRelayGet();
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

  /* Web Server */
  server.on("/relay", HTTP_GET, handleRelayGet);
  server.on("/relay", HTTP_PUT, handleRelayPut);
  server.on("/relay", HTTP_OPTIONS, handleOptions);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop() {
  ArduinoOTA.handle();
  button.process(millis());
  server.handleClient();
}
