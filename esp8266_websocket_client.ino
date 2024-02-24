#include <LiquidCrystal_I2C.h>
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
using namespace websockets;
const char* ssid = "SSID";      //Enter SSID
const char* password = "pass";  //Enter Password
int connection = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
String senderTop = "";
String messageTop = "";
StaticJsonDocument<200> doc;
void onMessageCallback(WebsocketsMessage message) {
  DeserializationError error = deserializeJson(doc, message.data());
  if (senderTop != String(doc["sender"]) || messageTop != String(doc["message"])) {
    lcd.clear();
  }
  senderTop = String(doc["sender"]);
  messageTop = String(doc["message"]);
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    connection = 0;
    Serial.println("Connnection Closed");
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

WebsocketsClient client;


void connectServer() {
  while (!connection) {
    connection = client.connect("ws://ip_addr:port/Esp");  // or use wss
    delay(500);
  }
}

void setup() {
  lcd.begin();
  Serial.begin(115200);
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(1000);
  }

  // run callback when messages are received
  client.onMessage(onMessageCallback);
  client.addHeader("Device", "esp8266");  // Add header info for websocket connection
  // run callback when events are occuring
  client.onEvent(onEventsCallback);
  // Connect to server
  connectServer();
}

void loop() {
  client.poll();
  const char* mission = doc["mission"];
  if (String(mission) == "message") {
    lcd.setCursor(0, 0);
    auto sender = "Sender:" + String(doc["sender"]);
    auto payload = String(doc["message"]);
    lcd.print(sender);
    int textLength = messageTop.length();
    lcd.setCursor(0, 1);
    if (textLength > 16) {
      // Print the text and scroll it
      for (int i = 0; i <= textLength - 16; i++) {
        lcd.setCursor(0, 1);
        lcd.print(messageTop.substring(i, i + 16));
        delay(500);
      }
    } else {
      lcd.setCursor(0, 1);
      lcd.print(messageTop);
      delay(1000);
    }
  }
  if (String(mission) == "lightOn") {
    lcd.blink_on();
  }
  if (String(mission) == "lightOff") {
    lcd.blink_off();
  }
  if (!connection) {
    connectServer();
  }
}