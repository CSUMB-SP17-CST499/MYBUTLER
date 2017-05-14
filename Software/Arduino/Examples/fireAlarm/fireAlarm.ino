/*
*
*	MY-BUTLER Fire Alarm Program for ESP8266
*	
*	This sketch uses MQTT to interface with a fire alarm and a specially 
*	setup breakout board with the appropriate pullup/pulldown hardware.
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
#define BUTTON_PIN 4
#define LED_PIN 16

// Set Wi-Fi Paramaters
const char* WIFI_SSID = "YOUR SSID HERE";
const char* WIFI_PASS = "YOUR PASS HERE";

// Set MQTT Parameters
const char* MQTT_BROKER = "192.168.1.200";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";
const char* STATUS_TOPIC = "/firealarm/status";
const char* CONTROL_TOPIC = "/firealarm/control";

// Heartbeat and State Tracking Variables
unsigned long PREV_HB_TIME = 0;
int HB_INTERVAL = 1000;
int debounceCounter = 0;
bool BUTTON_STATE = false;
bool ALARM_STATE = false;
char* REPORT_VALUE = "0";

void handleStateChar(char _input) {
  
    // Convert the incoming char into a bool
	ALARM_STATE = false;
    if (_input == '1') {
        ALARM_STATE = true;
    }
	
}

// State Machine updating based on physical system
void setState() {

	// Simple debounce to translate BUTTON_PIN into BUTTON_STATE
    if (digitalRead(BUTTON_PIN)) {
        if(debounceCounter == 10){
			BUTTON_STATE = true;
		}
		else{
			debounceCounter++;
		}
    } else {
        if(debounceCounter == 0){
			BUTTON_STATE = false;
		}
		else{
			debounceCounter--;
		}
    }
  

  
}

void updateLED(){
	if( ((millis()%100)>50) && ALARM_STATE ){
		digitalWrite(LED_PIN, LOW); // ON
	}
	else{
		digitalWrite(LED_PIN, HIGH); // OFF
	}
}

// Call this when a message is received - MQTT unpacking and message handling
void messageHandler(char * _topic, byte * _payload, unsigned int _length) {

    // Print out the details of the incoming message
    Serial.print("\nMessage arrived on: ");
    Serial.println(_topic);

    // We handle depending on which topic is the source
    if (strcmp(_topic, CONTROL_TOPIC) == 0) {
        for (int i = 0; i < _length; i++) {
            // Dispatch incoming char to handler
            handleStateChar(_payload[i]);
            // Extend next heartbeat and update CURRENT_STATE
            PREV_HB_TIME = millis() + 1000;
            delay(50);
            setState();
        }
    }
  
}

// Create Wi-Fi and MQTT clients
WiFiClient WIFI_CLIENT;
PubSubClient MQTT_CLIENT(MQTT_BROKER, MQTT_PORT, messageHandler, WIFI_CLIENT);

// Heartbeat function
int heartbeat() {
	
	// Convert CURRENT_STATE bool into char for outgoing message
    REPORT_VALUE[0] = '0';
    if (BUTTON_STATE) {
        REPORT_VALUE[0] = '1';
    }
	
	// Attempt to publish the REPORT_VALUE on the STATUS_TOPIC and return success
    if (MQTT_CLIENT.publish(STATUS_TOPIC, REPORT_VALUE)) {
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
        if ( MQTT_CLIENT.connect("fire") ) {
            Serial.println("connected!");
            // When connected, subscribe to CONTROL_TOPIC
            MQTT_CLIENT.subscribe(CONTROL_TOPIC);
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
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // OFF
    pinMode(BUTTON_PIN, INPUT);

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
        heartbeat();
        PREV_HB_TIME = millis();
    }
	
    setState();
	updateLED();

}
