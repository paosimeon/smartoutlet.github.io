#include <Arduino.h>
#include <WiFi.h>               //we are using the ESP32
//#include <ESP8266WiFi.h>      // uncomment this line if you are using esp8266 and comment the line above
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#include <PZEM004Tv30.h>

// Define a HardwareSerial instance for communication with PZEM004Tv30
HardwareSerial pzemSerial(1);  // Use Serial1 for ESP32

PZEM004Tv30 pzem(&pzemSerial, 16, 17);  // RX: 8, TX: 9

// Insert your network credentials
#define WIFI_SSID "PLDTHOMEFIBR2a2e0"
#define WIFI_PASSWORD "PLDTWIFIyxnp5"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAzceWGaVeJkB9xU3iMQ9Ym1LcLTgXIXHQ"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://volcheck-ccb7b-default-rtdb.asia-southeast1.firebasedatabase.app" 

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "paolosatuito18@gmail.com"
#define USER_PASSWORD "pao122002Lo"

// Define Firebase Data object
FirebaseData fbdo1;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

const int ledPin = 15;

void setup()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo1.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo1.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
}

void loop()
{
  // Firebase.ready() should be called repeatedly to handle authentication tasks.
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

  int ledState;
   if(Firebase.RTDB.getInt(&fbdo1, "/led/state", &ledState)){
    digitalWrite(ledPin, ledState);
   }else{
    Serial.println(fbdo1.errorReason().c_str());
   }
  }
}
