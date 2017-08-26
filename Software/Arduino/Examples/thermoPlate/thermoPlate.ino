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
#include <Wire.h>
#include "SSD1306.h"
#include "images.h"
#include "Adafruit_Si7021.h"

// Define GPIO Ports
#define HEARTBEAT_PIN 2

// Set Wi-Fi Paramaters
const char* WIFI_SSID = "YOUR SSID HERE";
const char* WIFI_PASS = "YOUR SSID HERE";

// Set MQTT Parameters
const char* MQTT_BROKER = "192.168.1.200";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";
const char* CTEMP_TOPIC = "/thermostat/ctemp";
const char* TTEMP_TOPIC = "/thermostat/ttemp";
const char* CHUMD_TOPIC = "/thermostat/chumd";
const char* CMODE_TOPIC = "/thermostat/cmode";
const char* TMODE_TOPIC = "/thermostat/tmode";
const int MESSAGE_LIMIT = 5;
char* INC_MESSAGE = "     ";
char* OUT_MESSAGE = "     ";

// Define "proxy enum" for state control
const char MODE_OFF = '0';
const char MODE_HEAT = '1';
const char MODE_COOL = '2';
const char MODE_AUTO = '3';

// State variables
char CURRENT_OPRT = '0';
char CURRENT_MODE = '0';
float TARGET_TEMP = 65.00;
float CURRENT_TEMP = 65.00;
float CURRENT_HUMD = 50.00;

// Heartbeat variables
unsigned long PREV_HB_TIME = 0;
int HB_INTERVAL = 1000;

// Setup Screen
SSD1306  LCD_DISPLAY(0x3c, 4, 5);

// Setup Temp/Humidity Sensor
Adafruit_Si7021 ENV_SENSOR = Adafruit_Si7021();

void handleIncomingMessage(char* _topic) {

  Serial.println(INC_MESSAGE);
  // Handle setting of Target Temperature value
  if (strcmp(TTEMP_TOPIC, _topic) == 0) {
    TARGET_TEMP = atof(INC_MESSAGE);
  }
  // Handle setting of Current Operating Mode
  else if (strcmp(TMODE_TOPIC, _topic) == 0) {
    CURRENT_MODE = (char)INC_MESSAGE[0];
  }

}
void updateSensors() {
  CURRENT_TEMP = CURRENT_TEMP * 0.9;
  CURRENT_TEMP += ((ENV_SENSOR.readTemperature() * 9.0 / 5.0) + 32.0) / 10.0;
  CURRENT_HUMD = CURRENT_HUMD * 0.9;
  CURRENT_HUMD += ENV_SENSOR.readHumidity() / 10.0;
}
void updateDisplay() {

  LCD_DISPLAY.clear();
  if((millis()/1000)%10>2){
    LCD_DISPLAY.setTextAlignment(TEXT_ALIGN_LEFT);
    LCD_DISPLAY.drawString(0, 0, "M:");
    LCD_DISPLAY.drawString(0, 48, "S:");
    CURRENT_OPRT = '0';
    if (CURRENT_MODE == MODE_OFF) {
    LCD_DISPLAY.drawString(32, 0, "OFF");
    LCD_DISPLAY.drawString(32, 48, "OFF");
    }
    else if (CURRENT_MODE == MODE_HEAT) {
    LCD_DISPLAY.drawString(32, 0, "HEAT");
    if ((int)CURRENT_TEMP > (int)TARGET_TEMP) {
      LCD_DISPLAY.drawString(32, 48, "OFF");
    }
    else {
      LCD_DISPLAY.drawString(32, 48, "ON");
      CURRENT_OPRT = '1';
    }
    }
    else if (CURRENT_MODE == MODE_COOL) {
    LCD_DISPLAY.drawString(32, 0, "COOL");
    if ((int)CURRENT_TEMP > (int)TARGET_TEMP) {
      LCD_DISPLAY.drawString(32, 48, "ON");
      CURRENT_OPRT = '1';
    }
    else {
      LCD_DISPLAY.drawString(32, 48, "OFF");
    }
    }
    else if (CURRENT_MODE == MODE_AUTO) {
    LCD_DISPLAY.drawString(32, 0, "AUTO");
    if ((int)CURRENT_TEMP-1 > (int)TARGET_TEMP) {
      LCD_DISPLAY.drawString(32, 48, "COOL");
      CURRENT_OPRT = '1';
    }
    else if ((int)CURRENT_TEMP+1 < (int)TARGET_TEMP) {
      LCD_DISPLAY.drawString(32, 48, "HEAT");
      CURRENT_OPRT = '1';
    }
    else {
      LCD_DISPLAY.drawString(32, 48, "OFF");
    }
    }
    LCD_DISPLAY.drawString(0, 16, "T:");
    dtostrf(TARGET_TEMP, 4, 2, OUT_MESSAGE);
    LCD_DISPLAY.drawString(32, 16, OUT_MESSAGE);
    LCD_DISPLAY.drawString(0, 32, "C:");
    dtostrf(CURRENT_TEMP, 4, 2, OUT_MESSAGE);
    LCD_DISPLAY.drawString(32, 32, OUT_MESSAGE);
  }
  else{
    LCD_DISPLAY.setTextAlignment(TEXT_ALIGN_CENTER);
    LCD_DISPLAY.drawString(64, 0, "Welcome to:");
    LCD_DISPLAY.drawString(64, 16, "MY-BUTLER");
    LCD_DISPLAY.drawString(64, 32, "CSUMB C/S");
    LCD_DISPLAY.drawString(64, 48, "Capstone 2017");
  }
  LCD_DISPLAY.display();

}
// State Machine updating based on physical system
void setState() {
  updateSensors();
  updateDisplay();
}

// Call this when a message is received - MQTT unpacking and message handling
void messageHandler(char * _topic, byte * _payload, unsigned int _length) {

  // Print out the details of the incoming message
  Serial.print("\nMessage arrived on: ");
  Serial.println(_topic);

  // We handle depending on which topic is the source
  int length = (MESSAGE_LIMIT < _length) ? MESSAGE_LIMIT : _length;
  for (int i = 0; i < length; i++) {
    // Dispatch incoming char to handler
    INC_MESSAGE[i] = _payload[i];
    INC_MESSAGE[i + 1] = NULL;
    // Extend next heartbeat
    PREV_HB_TIME = millis() + 1000;
  }
  handleIncomingMessage(_topic);

}

// Create Wi-Fi and MQTT clients
WiFiClient WIFI_CLIENT;
PubSubClient MQTT_CLIENT(MQTT_BROKER, MQTT_PORT, messageHandler, WIFI_CLIENT);

// MQTT Heartbeat function
int mqttHeartbeat() {
  int success = 1;

  // Attempt to publish the temperature
  dtostrf(CURRENT_TEMP, 4, 2, OUT_MESSAGE);
  if (MQTT_CLIENT.publish(CTEMP_TOPIC, OUT_MESSAGE)) {
    Serial.println("Temp ok.");
  } else {
    Serial.println("Temp failed.");
    success = 0;
  }

  // Attempt to publish the humidity
  dtostrf(CURRENT_HUMD, 4, 2, OUT_MESSAGE);
  if (MQTT_CLIENT.publish(CHUMD_TOPIC, OUT_MESSAGE)) {
    Serial.println("Humid ok.");
  } else {
    Serial.println("Humid failed.");
    success = 0;
  }

  // Attempt to publish the mode
  OUT_MESSAGE[0] = CURRENT_MODE;
  OUT_MESSAGE[1] = NULL;
  if (MQTT_CLIENT.publish(CMODE_TOPIC, OUT_MESSAGE)) {
    Serial.println("Mode ok.");
  } else {
    Serial.println("Mode failed.");
    success = 0;
  }

  return success;
}

// Reconnect to the MQTT Broker
void reconnect() {

  // Loop until we're reconnected
  while (!MQTT_CLIENT.connected()) {
    Serial.print("Connecting to MQTT Broker...");
    // Attempt to connect
    if ( MQTT_CLIENT.connect("thermostat") ) {
      Serial.println("connected!");
      // When connected, subscribe to CONTROL_TOPIC
      MQTT_CLIENT.subscribe(TTEMP_TOPIC);
      MQTT_CLIENT.subscribe(TMODE_TOPIC);
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

  Wire.begin();

  // Initialize Heartbeat Pin Direction/Level
  pinMode(HEARTBEAT_PIN, OUTPUT);
  digitalWrite(HEARTBEAT_PIN, HIGH);

  // Setup SSD1306 Screen
  LCD_DISPLAY.init();

  LCD_DISPLAY.flipScreenVertically();
  LCD_DISPLAY.setFont(ArialMT_Plain_16);
  LCD_DISPLAY.clear();
  LCD_DISPLAY.drawString(0, 0, "Boot...");
  LCD_DISPLAY.display();
  delay(100);

  // Setup Wi-Fi
  LCD_DISPLAY.clear();
  LCD_DISPLAY.drawString(0, 0, "Connect...");
  LCD_DISPLAY.display();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  delay(100);

  // Try to connect to Wi-Fi until successful
  int progress = 0;
  while ( WiFi.status() != WL_CONNECTED ) {
    Serial.print(".");
    digitalWrite(HEARTBEAT_PIN, !digitalRead(HEARTBEAT_PIN));
    for (int i = 0; i < 100; i++) {
      unsigned long cTime = millis();
      progress = ((int)cTime / 5) % 100;
      LCD_DISPLAY.clear();
      LCD_DISPLAY.drawString(0, 0, "Connect...");
      LCD_DISPLAY.drawProgressBar(0, 32, 120, 10, progress);
      LCD_DISPLAY.display();
      delay(5);
    }
  }

  // Confirm Wi-Fi is connected
  LCD_DISPLAY.clear();
  LCD_DISPLAY.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  LCD_DISPLAY.display();
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(2000);

}

void loop() {

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
    mqttHeartbeat();
    PREV_HB_TIME = cTime;
    Serial.print("Mode: ");
    Serial.println(CURRENT_MODE);
    Serial.print("Status: ");
    Serial.println(CURRENT_OPRT);
  }

  // Don't forget to blink!
  digitalWrite(HEARTBEAT_PIN, cTime % 1000 > 250);

}
