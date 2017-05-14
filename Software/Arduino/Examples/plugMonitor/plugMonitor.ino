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
#include <Wire.h>

// Define GPIO Ports
#define STATE_PIN 0
#define RELAY_PIN 2

// Set Wi-Fi Paramaters
const char* WIFI_SSID = "REPLACE ME WITH YOUR SSID";
const char* WIFI_PASS = "REPLACE ME WITH YOUR PASS";

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
  
  // ESP-01 breakout uses hardware pullup/pulldown @ ~15% of line voltage
  // Put pin in input mode and pause for voltage stabilization
    pinMode(STATE_PIN, INPUT); // PULLUP/PULLDOWN FUNCTION
    delay(50);
  
  // Set CURRENT_STATE according to actual state of STATE_PIN
    if (digitalRead(STATE_PIN)) {
        CURRENT_STATE = true;
    } else {
        CURRENT_STATE = false;
    }
  
  // Set STATE_PIN back into "drain" mode - float out ~15% depending on hardware
    pinMode(STATE_PIN, OUTPUT); // PULLUP/PULLDOWN FUNCTION
    digitalWrite(STATE_PIN, LOW); // PULLUP/PULLDOWN FUNCTION
  
}

// Call this when a message is received - MQTT unpacking and message handling
void messageHandler(char * _topic, byte * _payload, unsigned int _length) {

    // Print out the details of the incoming message
    Serial.print("\nMessage arrived on: ");
    Serial.println(_topic);
  
}

// Create Wi-Fi and MQTT clients
WiFiClient WIFI_CLIENT;
PubSubClient MQTT_CLIENT(MQTT_BROKER, MQTT_PORT, messageHandler, WIFI_CLIENT);

// Heartbeat function
int heartbeat() {

  // Attempt to publish the REPORT_VALUE on the STATUS_TOPIC and return success
    if (MQTT_CLIENT.publish(STATUS_TOPIC, &REPORT_VALUE)) {
        Serial.print("HB, State: ");
        Serial.println(REPORT_VALUE);
        return 1;
    } else {
        Serial.print("HB failed:");
        Serial.println(REPORT_VALUE);
        return 0;
    }
  
}

// Reconnect to the MQTT Broker
void reconnect() {
  
    // Loop until we're reconnected
    while (!MQTT_CLIENT.connected()) {
        Serial.print("Connecting to MQTT Broker...");
        // Attempt to connect
        if ( MQTT_CLIENT.connect("mqttClientID") ) {
            Serial.println("connected!");
        } 
        // If connection failed, alert and retry after cooldown
        else {
            Serial.print("failed, rc=");
            Serial.print(MQTT_CLIENT.state());
            Serial.println(", try again in 2 seconds...");
            delay(2000);
        }
    }
  
}

void setup() {
  
    // Initialize Hardware Serial
    Serial.begin(115200);
    delay(10);
    Serial.println();

    // Initialize Pin Directions/Levels
    pinMode(STATE_PIN, OUTPUT); // PULLUP/PULLDOWN FUNCTION - use as input when not programming ESP-01 breakout
    digitalWrite(STATE_PIN, LOW); // PULLUP/PULLDOWN FUNCTION - delete if using STATE_PIN as input

    // Setup Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Try to connect to Wi-Fi until successful
    Serial.print("WiFi connecting");
    while ( WiFi.status() != WL_CONNECTED ) {
        Serial.print(".");
        delay(500);
    }

    // Print connection info and exit setup
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  
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
