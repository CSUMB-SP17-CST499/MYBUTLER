/*
*
*	MY-BUTLER Motion Sensor Program for ESP8266
*	
*	This program uses MQTT to interface with a motion sensor
*	and broadcasts the result to the broker.
*
*	The program is meant to be used with an ESP-01 module, but if
*	using another format, simply replace pin numbers as needed and
*	remove sections that perform pullup/pulldown functions.
*
*/

#include <EEPROM.h> 
#include <ESP8266WiFi.h> 
#include <PubSubClient.h> 
#include <Wire.h>

// Define GPIO Ports
#define MOTION_PIN 0
#define LED_RED_PIN 1 // Serial pin - cannot use Serial in sketch!
#define LED_GRN_PIN 3 // Serial pin - cannot use Serial in sketch!

// Set Wi-Fi Paramaters
const char* WIFI_SSID = "YOUR SSID HERE";
const char* WIFI_PASS = "YOUR PASS HERE";

// Set MQTT Parameters
const char* MQTT_BROKER = "192.168.1.200";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";
const char* STATUS_TOPIC = "/motion/status";

// Heartbeat and State Tracking Variables
unsigned long PREV_HB_TIME = 0;
int HB_INTERVAL = 1000;
bool CURRENT_STATE = false;
char* REPORT_VALUE = "0";

// State Machine updating based on physical system
void setState() {

  pinMode(MOTION_PIN, INPUT);
  delay(10);
  
	// Set CURRENT_STATE according to actual state of BUTTON_PIN
    if (digitalRead(MOTION_PIN)) {
        CURRENT_STATE = true;
        REPORT_VALUE[0] = '1';
    } else {
        CURRENT_STATE = false;
        REPORT_VALUE[0] = '0';
    }
	digitalWrite(LED_RED_PIN, !CURRENT_STATE);
	digitalWrite(LED_GRN_PIN, CURRENT_STATE);

  pinMode(MOTION_PIN, OUTPUT);
  digitalWrite(MOTION_PIN, LOW);
  
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
        if ( MQTT_CLIENT.connect("motion") ) {
            // Good to go!
        } 
		// If connection failed, alert and retry after cooldown
		else {
            delay(2000);
        }
    }
  
}

void setup() {

    // Set LED pins
    pinMode(LED_GRN_PIN, OUTPUT);
    digitalWrite(LED_GRN_PIN, LOW);
    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_RED_PIN, LOW);
	
	// Set button pin
    pinMode(MOTION_PIN, OUTPUT); // PULLUP/PULLDOWN FUNCTION - use as input when not programming ESP-01 breakout
    digitalWrite(MOTION_PIN, LOW); // PULLUP/PULLDOWN FUNCTION - delete if using STATE_PIN as input

    // Setup Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Try to connect to Wi-Fi until successful
    while ( WiFi.status() != WL_CONNECTED ) {
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
