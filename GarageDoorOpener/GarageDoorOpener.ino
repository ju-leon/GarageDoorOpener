#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

const char* SSID = WIFI_SSID;
const char* PSK = PASSWORD;
const char* MQTT_BROKER = BROKER_IP;
 
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
char msg[50];
int value = 0;

const int RELAY_PIN = D1;

bool ledState = false;

void setup() {
    pinMode(RELAY_PIN, OUTPUT);

    pinMode(LED_BUILTIN, OUTPUT);
  
    Serial.begin(115200);
    setup_wifi();
    client.setServer(MQTT_BROKER, 1883);
    client.setCallback(callback);
}
 
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);
 
    WiFi.begin(SSID, PSK);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(RELAY_PIN, HIGH);   
    delay(200);
    digitalWrite(RELAY_PIN, LOW);

    client.publish("openhab/garage/events", "Garage door: switched garage");
  }

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "GarageDoor-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("openhab/garage/state2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
 
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    //Now will owerflow after 50 days. Should not be an issue.
    unsigned long now = millis();
    if (abs(now - lastMsg) > 2000) {
      lastMsg = now;
      
      snprintf (msg, 50, "Garage door: online, since %ld.000ms", now);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("openhab/garage/events", msg);

      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
      
    }
}
