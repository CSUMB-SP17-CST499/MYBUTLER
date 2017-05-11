/*
 * Button
 * Script created by: Cameron Jones
 * 
 * Inputs:
 * 1. Pin to track button pressing
 * 2. (OPTIONAL) CHANNEL COMMANDS
 * Outputs:
 * 1. Heartbeat channel with status of button (after filtering)
 * 2. (OPTIONAL) GPIO OUTPUT
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
//#include <Adafruit_INA219.h>
 
// Connect to the WiFi
const char* ssid = "ESP-Prog";
const char* password = "";
const char* mqtt_server = "192.168.1.146"; //  Server's IP address
uint16_t mqtt_port = 1883;
 
WiFiClient espClient;
PubSubClient client(espClient);

 const byte buttonPin = 0;
 int buttonState = 0;
 //  OPTIONAL - GPIO OUTPUT
//const byte outputPin = 1; // Pin for output

unsigned long previousHB = 0;
long HB_interval = 100;
char* state = "0";
int heartbeat () {
  if (client.publish("/device/item/status", state)) {
    Serial.print("Sent HB\n");
    return 1;
  }
  else {
    Serial.print("HB failed\n");
    return 0;
  }
}

void checkButton() {
 //  Read button
 buttonState = digitalRead(buttonPin);
 //  Uncomment below to print button state
 /*Serial.print("buttonState: ");
 Serial.print(buttonState);
 Serial.print("\n");*/
 if (buttonState == LOW) {
    //  Change state when button is pushed
    state = "1";
 } else {
    state = "0";
 }
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);
  //  OPTIONAL - CHANNEL COMMANDS
  /*if (receivedChar == '0')
    state = '0';
  if (receivedChar == '1')
    state = '1';*/
  }
  Serial.println();
}
 
// Reconnect if desconnected
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("mgttClientID")) {
  Serial.println("connected");
  // ... and subscribe to topic
  //  OPTIONAL - CHANNEL COMMANDS
  //client.subscribe("/device/item/control");//  Topic to subscribe to
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

 pinMode(buttonPin, INPUT);
 //  OPTIONAL - GPIO OUTPUT
 //pinMode(outputPin, OUTPUT);
}
 
void loop()
{
 if (!client.connected()) {
  reconnect();
 }
 client.loop();

 checkButton();
 unsigned long currentHB = millis();
 if (currentHB - previousHB >= HB_interval) {
  heartbeat();
  previousHB = currentHB;
 }

  //  OPTIONAL - GPIO OUTPUT
 // HIGH turns pin off and LOW turns pin on
 /*if (state == '0') {
  digitalWrite(relayPin, HIGH);
 } else {
  digitalWrite(relayPin, LOW);
 }*/
}
