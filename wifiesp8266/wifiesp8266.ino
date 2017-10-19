/*
  Basic ESP8266 MQTT example


  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h"
// Update these with values suitable for your network.添加中文

const char* ssid = "CSU-GeekSpace";
const char* password = "147258369";
const char* mqtt_server = "119.29.67.126";
SSD1306  display(0x3c, D3, D5);
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int data;
String oneLineData = "";
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    //delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  display.clear();
  char signaltype;
  String port;
  String keyvalue;
  //////////////////////////////////////////////
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    if (i == 0) {
      signaltype = (char)payload[0];
      Serial.println(signaltype);
    } else if (i == 1) {
      port += (char)payload[1];
      Serial.println(port);
    } else if ( i == 2) {
      port += (char)payload[2];
      Serial.println(port);
    } else {
      keyvalue += (char)payload[i];
      Serial.println(keyvalue);
    }
    // Print to the screen
    display.print((char)payload[i]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display();
    //delay(500);
    display.clear();
  }

  // we can now get the signaltype and port and keyvalue
  int keyvalueInt = keyvalue.toInt();
  if (signaltype == '0') {
    // indicates this is turn-on-off value
    // now keyvalueInt is 0 or 1
    Serial.println("get signaltype");
    if (keyvalueInt==0) {
       Serial.println("kv=0");
      digitalWrite(D4, LOW);
    } else {
       Serial.println("kv=1");
      digitalWrite(D4, HIGH);
      delay(1000);
    }
  } else {
    Serial.println("else");
    // now keyvalueInt is 21 or 32
    // do something
  }
  Serial.println();
  display.setLogBuffer(5, 30);

  // Some test data
  const char* test[] = {
    "turn on",
    "turn off" ,
  };

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    //digitalWrite(D4, HIGH);
    display.clear();
    // Print to the screen
    display.print(test[0]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display();
  } else {
   //digitalWrite(D4, LOW);
    display.clear();
    // Print to the screen
    display.print(test[1]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display();
    // delay(500);// // Turn the LED off by making the voltage HIGH
    display.clear();
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("manager/0004000001");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}


void setup() {
  display.init();
  display.setContrast(255);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(D4, OUTPUT);// Initialize the BUILTIN_LED pin as an output
  digitalWrite(D4, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(D4, LOW);    // turn the LED off by making the voltage LOW
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    while (Serial.available() > 0)
    {
      oneLineData += char(Serial.read());

    }
    int sensorValue = analogRead(A0);
    Serial.println(sensorValue);
    String tem = String(sensorValue);
    client.publish("chat/bob", tem.c_str());
    tem = "";
    // memset(ret, 0, 100);

    snprintf (msg, 75, "wocha", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);

  }
}
