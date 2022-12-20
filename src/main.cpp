#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <stdlib.h>
// Replace with your network credentials
const char* ssid = "";
const char* password = "";
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
const char* mqttServer = "broker.emqx.io";
int mqttPort = 1883;
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);

  // set the callback function
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/swa/commands");
      }
      
  }
}

MFRC522DriverPinSimple ss_pin(5); // Configurable, see typical pin layout above.

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver.
MFRC522 mfrc522{driver}; // Create MFRC522 instance.

String tagContent = "";

#define LED 12

#define Red 27
#define Blue 26
#define Green 25

#define BUZZ 13


 WiFiManager wm;
void setup()
{
  
  pinMode (LED,OUTPUT);
  pinMode (BUZZ,OUTPUT);
  pinMode (Red,OUTPUT);
  pinMode (Green,OUTPUT);
  pinMode (Blue,OUTPUT);

  Serial.begin(115200); // Initialize serial communications with the PC for debugging.

digitalWrite(Red, LOW);
digitalWrite(Green, HIGH);
digitalWrite(Blue, HIGH);


 
  bool res;

  res = wm.autoConnect("ESP32","00000000"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
  }
  else {
      digitalWrite(Green, LOW);
  } 


  // // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
  setupMQTT();
  
  // initWebSocket();

  // Start server
  // server.begin();
                                             // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  mfrc522.PCD_Init();                                     // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial); // Show details of PCD - MFRC522 Card Reader details.
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop()
{
  // return if wifi is not connected
  if (WiFi.status() != WL_CONNECTED) {
  digitalWrite(Red, LOW);
  digitalWrite(Green, HIGH);
  digitalWrite(Blue, HIGH);
  bool res = wm.autoConnect("ESP32","00000000"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      return;
  }
  else {
      digitalWrite(Green, LOW);
  } 

 
    // return;
  }
    // ws.cleanupClients();
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if (!mqttClient.connected())
    reconnect();
  mqttClient.loop();
  digitalWrite(Red, HIGH);
  digitalWrite(Green, LOW);
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards.
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called.
  // MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &(mfrc522.uid));

  Serial.println("Printing only the Card ID:");
  digitalWrite(LED, HIGH);
  digitalWrite(BUZZ, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  digitalWrite(BUZZ, LOW);

  Serial.println("--------------------------");
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    tagContent.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : ""));
    tagContent.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  tagContent.toUpperCase();

  Serial.println(tagContent);
  // notifyClients();
  
  Serial.println(mqttClient.connected());
  String message = "{ \"msg\": \"" + tagContent + "\" }";
  Serial.println(message.c_str());
  mqttClient.publish("MKOSJVMJGJ/orange", message.c_str());
  tagContent = "";
  delay(1000);
}
