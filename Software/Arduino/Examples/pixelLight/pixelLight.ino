

/*

   MY-BUTLER Smart Switch/Plug Program for ESP8266

  This program uses MQTT to interface with a three-way switch
  using a NC/NO relay (standard 3-pole) and a specially setup
  breakout board with the appropriate pullup/pulldown hardware.

  The program is meant to be used with an ESP-01 module, but if
  using another format, simply replace pin numbers as needed and
  remove sections that perform pullup/pulldown functions.

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WS2812FX.h>

// Define GPIO Ports
#define STATE_PIN 0
#define RELAY_PIN 2
#define WS2812B_PIN 3
#define LED_COUNT 12
// Set Wi-Fi Paramaters
const char* WIFI_SSID = "MY-BUTLER";
const char* WIFI_PASS = "ABCDE12345";

// Set MQTT Parameters
const char* MQTT_BROKER = "192.168.1.200";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";
const char* L_RED_TOPIC = "/RGBCANS/2/Red";
const char* L_GRN_TOPIC = "/RGBCANS/2/Green";
const char* L_BLU_TOPIC = "/RGBCANS/2/Blue";
const char* L_WHT_TOPIC = "/RGBCANS/2/White/c";
const char* L_STA_TOPIC = "/RGBCANS/2/White/s";
const int MESSAGE_LIMIT = 3;
char* INC_MESSAGE = "012";
char* OUT_MESSAGE = "0";

// State variables
bool CURRENT_STATE = true;
unsigned int RED_VALUE = 0;
unsigned int GRN_VALUE = 0;
unsigned int BLU_VALUE = 0;

// Heartbeat variables
unsigned long PREV_HB_TIME = 0;
int HB_INTERVAL = 1000;

WS2812FX ws2812fx = WS2812FX(LED_COUNT, WS2812B_PIN, NEO_GRB + NEO_KHZ800);

void handleIncomingMessage(char* _topic) {

  // Handle setting of RED value
  if (strcmp(L_RED_TOPIC, _topic) == 0) {
    RED_VALUE = atof(INC_MESSAGE);
  }
  // Handle setting of GRN value
  else if (strcmp(L_GRN_TOPIC, _topic) == 0) {
    GRN_VALUE = atof(INC_MESSAGE);
  }
  // Handle setting of BLU value
  else if (strcmp(L_BLU_TOPIC, _topic) == 0) {
    BLU_VALUE = atof(INC_MESSAGE);
  }
  // Handle setting of BLU value
  else if (strcmp(L_WHT_TOPIC, _topic) == 0) {
    
    // Get a boolean request to compare against the current state
    bool request = false;
    int message = atoi(INC_MESSAGE);
    // Convert the incoming char into a bool
    if (message == 1) {
        request = true;
    }
    else{
      request = false;
    }
    // If the incoming request is different than the current state...
    if (request != CURRENT_STATE) {
        // Flip the relay, and hope it changes the system state
        digitalWrite(RELAY_PIN, request);
    }
  
  }

}
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

  // Update LEDs
  ws2812fx.setColor(RED_VALUE, GRN_VALUE, BLU_VALUE);
}

// Call this when a message is received - MQTT unpacking and message handling
void messageHandler(char * _topic, byte * _payload, unsigned int _length) {
  // We handle depending on which topic is the source
  int length = (MESSAGE_LIMIT < _length) ? MESSAGE_LIMIT : _length;
  for (int i = 0; i < length; i++) {
    // Dispatch incoming char to handler
    INC_MESSAGE[i] = _payload[i];
    INC_MESSAGE[i + 1] = NULL;
    // Extend next heartbeat
  }
  handleIncomingMessage(_topic);

  PREV_HB_TIME = millis() + 1000;
  
}

// Create Wi-Fi and MQTT clients
WiFiClient WIFI_CLIENT;
PubSubClient MQTT_CLIENT(MQTT_BROKER, MQTT_PORT, messageHandler, WIFI_CLIENT);

// Heartbeat function
int heartbeat() {
	
	// Convert CURRENT_STATE bool into char for outgoing message
    OUT_MESSAGE[0] = '0';
    if (CURRENT_STATE) {
        OUT_MESSAGE[0] = '1';
    }
	
	// Attempt to publish the REPORT_VALUE on the STATUS_TOPIC and return success
    if (MQTT_CLIENT.publish(L_STA_TOPIC, OUT_MESSAGE)) {
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
    if ( MQTT_CLIENT.connect("canlight1") ) {
      // When connected, subscribe to CONTROL_TOPIC
      MQTT_CLIENT.subscribe(L_RED_TOPIC);
      MQTT_CLIENT.subscribe(L_GRN_TOPIC);
      MQTT_CLIENT.subscribe(L_BLU_TOPIC);
      MQTT_CLIENT.subscribe(L_WHT_TOPIC);
    }
	
    // If connection failed, alert and retry after cooldown
    else {
      delay(2000);
    }
  }

}

void setup() {

  pinMode(STATE_PIN, OUTPUT);
  digitalWrite(STATE_PIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  delay(100);

  while ( WiFi.status() != WL_CONNECTED ) {
	delay(500);
  }

  ws2812fx.init();
  ws2812fx.setBrightness(100);
  ws2812fx.setSpeed(200);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();

}

void loop() {
  
  ws2812fx.service();
  
  // Grab the current time and update the sensors, etc
  unsigned long cTime = millis();
  setState();

  // Connect to the MQTT Broker if needed
  if ( !MQTT_CLIENT.connected() ) {
    reconnect();
  }
  // Handle MQTT Pub/Sub back-end functions
  MQTT_CLIENT.loop();

  // If hearbeat timer is exceeded, perform needed actions and update timer
  if ( cTime > (PREV_HB_TIME + HB_INTERVAL)) {
    heartbeat();
    PREV_HB_TIME = cTime;
  }

}
