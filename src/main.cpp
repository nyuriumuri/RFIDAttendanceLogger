#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiManager.h>
// Replace with your network credentials
const char* ssid = "";
const char* password = "";

MFRC522DriverPinSimple ss_pin(5); // Configurable, see typical pin layout above.

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver.
// MFRC522DriverI2C driver{}; // Create I2C driver.
MFRC522 mfrc522{driver}; // Create MFRC522 instance.

String tagContent = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#define LED 12

#define BUZZ 13

void notifyClients() {
    Serial.printf("Notifying: %s\n", tagContent);
  ws.textAll(String(tagContent));
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      Serial.printf("Got message: %s", (char *) data);
    //   handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
void setup()
{
  
  pinMode (LED,OUTPUT);
  pinMode (BUZZ,OUTPUT);
  Serial.begin(115200); // Initialize serial communications with the PC for debugging.
//   delay(400);

 WiFiManager wm;

 
    wm.resetSettings();
    bool res;

    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
    } 



  // WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Start server
  server.begin();
  while (!Serial)
    ;                                                     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  mfrc522.PCD_Init();                                     // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial); // Show details of PCD - MFRC522 Card Reader details.
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop()
{
    ws.cleanupClients();
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
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
  delay(1000);
  digitalWrite(LED, LOW);
  digitalWrite(BUZZ, LOW);

  Serial.println("--------------------------");
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    tagContent.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tagContent.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  tagContent.toUpperCase();

  Serial.println(tagContent);
  notifyClients();
  
  tagContent = "";
  delay(1000);
}