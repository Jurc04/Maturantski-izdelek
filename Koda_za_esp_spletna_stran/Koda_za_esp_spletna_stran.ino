#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
 
String apiKey = "N3VS2IDIO6KMLFGU"; 
const char* ssid = "HUAWEI P20";
const char* password = "Geslo123";
const char* server = "api.thingspeak.com";
 
#define SS 5
#define RST 14
#define DI0 2
 
#define TX_P 17
#define BAND 433E6
#define ENCRYPT 0x78
 
String device_id;
String temperature;
String pressure;
String altitude;
String humidity;
String dewPoint;
String rainfall;
String lux;
 
WiFiClient client;
 
 
void setup()
{
  Serial.begin(115200);
  Serial.println("LoRa prejemnik");
  LoRa.setTxPower(TX_P);
  LoRa.setSyncWord(ENCRYPT);
 
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND))
  {
    Serial.println("Zagon LoRe je spodletel!");
    while (1);
  }
 
  Serial.println("povezujem se ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi je vzpostavljen!");
  Serial.print("Dobil sem IP ");
  Serial.println(WiFi.localIP());
 
}
 
void loop()
{
  // razčlenimo paket
  int pos1, pos2, pos3, pos4, pos5, pos6, pos7;
 
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    
    Serial.print("prejeti paket:  ");
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData);
    
    while (LoRa.available())
    {
      Serial.print((char)LoRa.read());
    }
    
    Serial.print("' RSSI ");
    Serial.println(LoRa.packetRssi());
 
    pos1 = LoRaData.indexOf('/');
    pos2 = LoRaData.indexOf('&');
    pos3 = LoRaData.indexOf('#');
    pos4 = LoRaData.indexOf('@');
    pos5 = LoRaData.indexOf('$');
    pos6 = LoRaData.indexOf('^');
    pos7 = LoRaData.indexOf('!');
 
    device_id = LoRaData.substring(0, pos1);
    temperature = LoRaData.substring(pos1 + 1, pos2);
    pressure = LoRaData.substring(pos2 + 1, pos3);
    altitude = LoRaData.substring(pos3 + 1, pos4);
    humidity = LoRaData.substring(pos4 + 1, pos5);
    dewPoint = LoRaData.substring(pos5 + 1, pos6);
    rainfall = LoRaData.substring(pos6 + 1, pos7);
    lux = LoRaData.substring(pos7 + 1, LoRaData.length());
 
    Serial.print(F("Device ID = "));
    Serial.println(device_id);
 
    Serial.print(F("Temperatura = "));
    Serial.print(temperature);
    Serial.println(F("*C"));
 
    Serial.print(F("pritisk = "));
    Serial.print(pressure);
    Serial.println(F("hPa"));
 
    Serial.print(F("nadmorska višina = "));
    Serial.print(altitude);
    Serial.println(F("m"));
 
    Serial.print(F("vlažnost = "));
    Serial.print(humidity);
    Serial.println(F("%"));
 
    Serial.print("rosišče = ");
    Serial.print(dewPoint);
    Serial.println(" *C");
 
    Serial.print(F("padavine = "));
    Serial.print(rainfall);
    Serial.println(F("%"));
 
    Serial.print(F("svetlost = "));
    Serial.print(lux);
    Serial.println(F(" lx"));
 
    Serial.println();
 
    if (client.connect(server, 80)) // "184.106.153.149" ali api.thingspeak.com
    {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(temperature);
      postStr += "&field2=";
      postStr += String(pressure);
      postStr += "&field3=";
      postStr += String(altitude);
      postStr += "&field4=";
      postStr += String(humidity);
      postStr += "&field5=";
      postStr += String(dewPoint);
      postStr += "&field6=";
      postStr += String(rainfall);
      postStr += "&field7=";
      postStr += String(lux);
      postStr += "r\n";
 
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);
 
      Serial.println("podatki so poslani na THINGSPEAK!");
      delay(500);
    }
    client.stop();
    Serial.println("Čakam...");
 
  }
}
