/*
 * Monitor Plug
 * Script created by: Cameron Jones
 * 
 * Inputs:
 * 1. Pin to track monitor line
 * Outputs:
 * 1. Heartbeat channel with status of monitor line
 */
 
#include <EEPROM.h> 
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

// Define GPIO Ports
#define OPTO_PIN 3

// Set Wi-Fi Paramaters
const char* WIFI_SSID = "MY-BUTLER";
const char* WIFI_PASS = "ABCDE12345";

// Set MQTT Parameters
const char* MQTT_BROKER = "192.168.1.200";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";
const char* STATUS_TOPIC = "/plugmonitor/status";

// Heartbeat and State Tracking Variables
unsigned long PREV_HB_TIME = 0;
int HB_INTERVAL = 1000;
bool CURRENT_STATE = false;

// State Machine updating based on physical system
void setState() {

  pinMode(OPTO_PIN, OUTPUT);
  digitalWrite(OPTO_PIN, LOW);
  delay(10);
  pinMode(OPTO_PIN, INPUT);
  delay(90);
  
  // Set CURRENT_STATE according to actual state of STATE_PIN
    if (digitalRead(OPTO_PIN)) {
        CURRENT_STATE = true;
    } else {
        CURRENT_STATE = false;
    }

  
}

// Call this when a message is received - MQTT unpacking and message handling
void messageHandler(char * _topic, byte * _payload, unsigned int _length) {

  // This should never happen!
  
}

// Create Wi-Fi and MQTT clients
WiFiClient WIFI_CLIENT;
PubSubClient MQTT_CLIENT(MQTT_BROKER, MQTT_PORT, messageHandler, WIFI_CLIENT);

// Heartbeat function
int heartbeat() {

  char* REPORT_VALUE = "0";
  if(CURRENT_STATE){
    REPORT_VALUE[0] = '1';
  }
  else{
    REPORT_VALUE[0] = '0';
  }
  // Attempt to publish the REPORT_VALUE on the STATUS_TOPIC and return success
    if (MQTT_CLIENT.publish(STATUS_TOPIC, REPORT_VALUE)) {
        return 1;
    } else {
        return 0;
    }
  
}

// Reconnect to the MQTT Broker
void reconnect() {
  
    // Loop until we're reconnected
    while (!MQTT_CLIENT.connected()) {
        // Attempt to connect
        if ( MQTT_CLIENT.connect("plugmonitor") ) {
            // Good to go!
        } 
        // If connection failed, alert and retry after cooldown
        else {
            delay(2000);
        }
    }
  
}

void setup() {
  
 

    // Initialize Pin Directions/Levels
    pinMode(OPTO_PIN, INPUT);
    
    // Setup Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Try to connect to Wi-Fi until successful
    while ( WiFi.status() != WL_CONNECTED ) {
        Serial.print(".");
        delay(500);
    }

  
}

void loop() {
  
    // Connect to the MQTT Broker if needed
    if ( !MQTT_CLIENT.connected() ) {
        reconnect();
    }
    // Handle MQTT Pub/Sub back-end functions
    MQTT_CLIENT.loop();

    // If hearbeat timer is exceeded, perform needed actions and update timer
    if ( millis() > (PREV_HB_TIME + HB_INTERVAL)) {
        setState();
        heartbeat();
        PREV_HB_TIME = millis();
    }

}
