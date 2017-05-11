/*
 * Monitor Plug
 * Script created by: Cameron Jones
 * 
 * Inputs:
 * 1. Pin to track monitor line
 * 2. Channel with incoming plug enable/disable commands
 * Outputs:
 * 1. Pin to enable/disable plug
 * 2. Heartbeat channel with status of monitor line
 * 3. Heartbeat channel with status of plug enable/disable (LOW is enable, N/C relay state)
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
 
// Connect to the WiFi
const char* ssid = "ESP-Prog";
const char* password = "";
const char* mqtt_server = "192.168.1.147"; //  Server's IP address
uint16_t mqtt_port = 1883;
 
WiFiClient espClient;
PubSubClient client(espClient);

// Heartbeat and state variables
unsigned long previousHB = 0;
long HB_interval = 1000;
char lineState = '0';
char plugState = '0';
const int linePin = 2; //  Pin to track monitor line
const int plugPin = 3; //  Pin to enable/disable plug

// Send a heartbeat if it is time
int heartbeat () {
  if (client.publish("/device/item/lineStatus", &lineState) && client.publish("device/item/plugStatus", &plugState)) {
    Serial.print("\nHB:");
    Serial.print(lineState);
    Serial.print(plugState);
    return 1;
  }
  else {
    Serial.print("\nHB failed:");
    Serial.print(lineState);
    Serial.print(plugState);
    return 0;
  }
}

void checkPlugState () {
  if (plugState == '0') {
    digitalWrite(plugPin, HIGH);
  }
  else {
    digitalWrite(plugPin, LOW);
  }
}

// Calls this when a message is received
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);
  if (receivedChar == '0')
    plugState = '0';
  if (receivedChar == '1')
    plugState = '1';
  }
  Serial.println();
}
 
// Reconnect if disconnected
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("mqttClientID")) {
  Serial.println("connected");
  // ... and subscribe to topic
  client.subscribe("/device/item/control"); //  Topic
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}
 
void setup()
{
 Serial.begin(9600);

 Wifi.mode(WIFI_STA);

 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);

 pinMode(linePin, INPUT);
 pinMode(plugPin, OUTPUT);
 digitalWrite(plugPin, HIGH);
}
 
void loop()
{
 // Reconnect if needed
 if (!client.connected()) {
  reconnect();
 }
 client.loop();

 // Send HB if it is time
 unsigned long currentHB = millis();
 if (currentHB - previousHB >= HB_interval) {
  heartbeat();
  previousHB = currentHB;
 }

  checkPlugState();
}
