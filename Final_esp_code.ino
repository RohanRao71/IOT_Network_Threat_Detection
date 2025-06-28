#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#define NODE_ID "ESP32_001"

const char* WIFI_SSID = "YourSSID";
const char* WIFI_PASSWORD = "YourPassword";

const char* WEBSOCKET_HOST = "192.168.1.100";  // Replace with your server IP
const uint16_t WEBSOCKET_PORT = 8000;
const char* WEBSOCKET_PATH = "/ws";

WebSocketsClient webSocket;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 200;  // in milliseconds (0.2 sec)

// Random float between min and max
float randFloat(float min, float max) {
  return min + static_cast<float>(random(0, 10001)) / 10000 * (max - min);
}

// Random integer between min and max
int randInt(int min, int max) {
  return random(min, max + 1);
}

void sendSimulatedLog() {
  StaticJsonDocument<1024> doc;

  // Set values
  doc["node_id"] = NODE_ID;
  doc["timestamp"] = millis();

  doc["duration"] = randInt(0, 10000);
  doc["protocol_type"] = "tcp";  // simplify for now
  doc["service"] = "http";
  doc["flag"] = "SF";
  doc["src_bytes"] = randInt(0, 100000);
  doc["dst_bytes"] = randInt(0, 100000);
  doc["land"] = randInt(0, 1);
  doc["wrong_fragment"] = randInt(0, 3);
  doc["urgent"] = randInt(0, 3);
  doc["hot"] = randInt(0, 5);
  doc["num_failed_logins"] = randInt(0, 5);
  doc["logged_in"] = randInt(0, 1);
  doc["num_compromised"] = randInt(0, 10);
  doc["root_shell"] = randInt(0, 1);
  doc["su_attempted"] = randInt(0, 1);
  doc["num_root"] = randInt(0, 10);
  doc["num_file_creations"] = randInt(0, 5);
  doc["num_shells"] = randInt(0, 2);
  doc["num_access_files"] = randInt(0, 5);
  doc["num_outbound_cmds"] = 0;
  doc["is_host_login"] = randInt(0, 1);
  doc["is_guest_login"] = randInt(0, 1);
  doc["count"] = randInt(0, 500);
  doc["srv_count"] = randInt(0, 500);
  doc["serror_rate"] = randFloat(0, 1);
  doc["srv_serror_rate"] = randFloat(0, 1);
  doc["rerror_rate"] = randFloat(0, 1);
  doc["srv_rerror_rate"] = randFloat(0, 1);
  doc["same_srv_rate"] = randFloat(0, 1);
  doc["diff_srv_rate"] = randFloat(0, 1);
  doc["srv_diff_host_rate"] = randFloat(0, 1);
  doc["dst_host_count"] = randInt(0, 255);
  doc["dst_host_srv_count"] = randInt(0, 255);
  doc["dst_host_same_srv_rate"] = randFloat(0, 1);
  doc["dst_host_diff_srv_rate"] = randFloat(0, 1);
  doc["dst_host_same_src_port_rate"] = randFloat(0, 1);
  doc["dst_host_srv_diff_host_rate"] = randFloat(0, 1);
  doc["dst_host_serror_rate"] = randFloat(0, 1);
  doc["dst_host_srv_serror_rate"] = randFloat(0, 1);
  doc["dst_host_rerror_rate"] = randFloat(0, 1);
  doc["dst_host_srv_rerror_rate"] = randFloat(0, 1);
  doc["difficulty_level"] = randFloat(0, 1);

  String jsonString;
  serializeJson(doc, jsonString);
  webSocket.sendTXT(jsonString);
}

void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.println("Connected to WebSocket");
  } else if (type == WStype_DISCONNECTED) {
    Serial.println("Disconnected from WebSocket");
  } else if (type == WStype_TEXT) {
    Serial.printf("Received: %s\n", payload);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());

  webSocket.begin(WEBSOCKET_HOST, WEBSOCKET_PORT, WEBSOCKET_PATH);
  webSocket.onEvent(onWebSocketEvent);
  webSocket.setReconnectInterval(2000);
}

void loop() {
  webSocket.loop();
  if (millis() - lastSendTime > sendInterval) {
    sendSimulatedLog();
    lastSendTime = millis();
  }
}