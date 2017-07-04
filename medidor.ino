//Programa : Medidor de energia elétrica com Arduino e SCT-013
//Autor : rict4
 
//Baseado no programa exemplo da biblioteca EmonLib
 
//Carrega as bibliotecas
#include <SPI.h>
#include "EmonLib.h" 
#include <Ethernet.h>
#include <PubSubClient.h>

// Update this to either the MAC address found on the sticker on your ethernet shield (newer shields)
// or a different random hexadecimal value (change at least the last four bytes)
byte mac[]    = {0xDE, 0xEB, 0xBC, 0xFE, 0xFE, 0xEA };
char macstr[] = "deebbcfefeea";
// Note this next value is only used if you intend to test against a local MQTT server
byte localserver[] = {192, 168, 1, 103};
// Update this value to an appropriate open IP on your local network
byte ip[]     = {192, 168, 25, 99};

//char servername[]="quickstart.messaging.internetofthings.ibmcloud.com";
//String clientName = String("d:quickstart:arduino:") + macstr;
//String topicName = String("iot-2/evt/status/fmt/json");

//char servername[]="7o4gsc.messaging.internetofthings.ibmcloud.com";
char servername[]="7o4gsc.messaging.internetofthings.ibmcloud.com";
String clientName = String("d:7o4gsc:Sensor:") + macstr;
String topicName = String("iot-2/evt/status/fmt/json");
char username[]="use-token-auth";
char password[]="jXczme(B2dEZa8G7f3";

EthernetClient ethClient;
EnergyMonitor emon1;
// Uncomment this next line and comment out the line after it to test against a local MQTT server
//PubSubClient client(localserver, 1883, 0, ethClient);
PubSubClient client(servername, 1883, 0, ethClient);

//Tensao da rede eletrica
int rede = 220.0;
//Pino do sensor SCT
int pino_sct = 8;
double irms;
char clientStr[34];
char topicStr[26];
 
void setup() 
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);   //2000/390 = 5.12
  //Pino, calibracao - Cur Const= Ratio/BurdenR. 1800/62 = 29. 
  emon1.current(pino_sct, 5.12);
  Ethernet.begin(mac, ip);
  client.setCallback(callback);
  clientName.toCharArray(clientStr,34);
  
  topicName.toCharArray(topicStr,26);
  digitalWrite(13, HIGH);
  if (!client.connected()) {
    digitalWrite(13, LOW);
    Serial.print("Trying to connect to: ");
    Serial.println(clientStr);
//    client.connect(clientStr);
    client.connect(clientStr, username, password);
  }
  Serial.print(".");
} 
  
void loop() 
{ 
  Serial.print(".");
  getData();
  Serial.print(".");
   if (client.connected() ) {
    Serial.print(".");
    digitalWrite(13, HIGH);
    String json = buildJson();
    char jsonStr[200];
    json.toCharArray(jsonStr,200);
    
    boolean pubresult = client.publish(topicStr,jsonStr);
    client.subscribe("topic/subs");
    
    Serial.print("attempt to send ");
    Serial.println(jsonStr);
    Serial.print("to ");
    Serial.println(topicStr);
//    
////    if (pubresult)
////      Serial.println("successfully sent");
////    else
////      Serial.println("unsuccessfully sent");
  }else {
    digitalWrite(13, LOW);
    Serial.print("Trying to connect to: ");
    Serial.println(clientStr);
//    client.connect(clientStr);
    client.connect(clientStr, username, password);
  }
  
  client.loop();
  delay(5000);
}

String buildJson() {
  String data = "{";
  data+="\n";
  data+= "\"d\": {";
  data+="\n";
  data+="\"myName\": \"Arduino Energy Monitor\",";
  data+="\n";
  data+="\"Corrente (A)\": ";
  data+=irms;
  data+="\n";
  data+="}";
  data+="\n";
  data+="}";
  return data;
}

void getData() {
//  Serial.println("Read OK"); 
  //Calcula a corrente  
  irms = emon1.calcIrms(1480);
  
  //Mostra o valor da corrente
  Serial.print("Corrente : ");
  Serial.print(irms); // Irms

  //Calcula e mostra o valor da potencia
  Serial.print(" Potencia : ");
  Serial.println(irms*rede);

  delay(1500);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}