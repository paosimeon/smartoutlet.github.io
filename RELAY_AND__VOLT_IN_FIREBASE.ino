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
#define WIFI_SSID "sabater5G"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAzceWGaVeJkB9xU3iMQ9Ym1LcLTgXIXHQ"

    // Insert RTDB URLefine the RTDB URL */ 
#define DATABASE_URL "https://volcheck-ccb7b-default-rtdb.asia-southeast1.firebasedatabase.app" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseData fbdo1;
 
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;                     //since we are doing an anonymous sign in 
const int ledPin = 15;
void setup(){
  Serial.begin(115200);
  pzemSerial.begin(9600);
    pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  fbdo1.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo1.setResponseSize(2048);
    fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
}

void loop(){
  //voltage and current
  float voltage = pzem.voltage();
  float energy = pzem.energy();
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "VOLCHECK/Voltage", voltage)){
     Serial.print(voltage);
    }
    else {
      Serial.println("FAILED TO READ FROM SENSOR");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "VOLCHECK/Energy", energy)){
     Serial.print(energy);
    }
    else {
      Serial.println("FAILED TO READ FROM SENSOR");
      Serial.println("REASON: " + fbdo.errorReason());
    }
     int ledState;
   if(Firebase.RTDB.getInt(&fbdo1, "/led/state", &ledState)){
    digitalWrite(ledPin, ledState);
   }else{
    Serial.println(fbdo1.errorReason().c_str());
   }
  }
  
  }
