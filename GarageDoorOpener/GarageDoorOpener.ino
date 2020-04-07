#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* SSID = "NETZWERKNAME";
const char* PSK = "NETZWERKSCHLÜSSEL";
const char* MQTT_BROKER = "BROKER_IP";
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
 
void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(MQTT_BROKER, 1883);
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
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

 
void reconnect() {
    while (!client.connected()) {
        Serial.print("Reconnecting...");
        if (!client.connect("ESP8266Client")) {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}
void loop() {
 
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
 
    snprintf (msg, 50, "Alive since %ld milliseconds", millis());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("/home/data", "Hello World");
    delay(5000);
}
