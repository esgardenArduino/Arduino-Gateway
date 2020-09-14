#include <avr/wdt.h> 
#include <SPI.h>
#include <WiFiNINA.h>
#include "RAK811.h"
#include "Lora_conf.h"
#include "ArduinoJson.h"
#include "Firebase_Arduino_WiFiNINA.h"
#include "receive_params.h"
#include "garden_params.h"
#include "firebase_params.h"
#include "SoftwareSerial.h"
#include "MemoryFree.h"

DynamicJsonDocument doc(1024);
int ledPin = 13;
  
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
   StartFirebase();
   LoRaRAK811_setup();
   doc.clear();
   pinMode (LED_BUILTIN, OUTPUT);
   digitalWrite (ledPin, LOW); //on board blinking LED (orange) to check LoRa connectivity
   Serial.println("All OK.");
   wdt_enable(0xFF);   
}

void loop() {
  String recv = "";
  if (RAKLoRa.rk_recvP2PData(1))//Start receiving
  {
    digitalWrite (LED_BUILTIN, HIGH);
    recv = RAKLoRa.rk_recvData();
      if (recv != "") {
      wdt_disable();
      bool sent = SendJSON(GetValueLora(recv));
      delay(100);
      }
  }
  else {
    digitalWrite (LED_BUILTIN, LOW); 
    wdt_disable();
    LoRaRAK811_setup();
  }

  /*String recv = "0309FFFC003C001E000A03000106000A04000A";//Test without RAK reception: Sent number FFFC
  wdt_disable();
  bool sent = SendJSON(GetValueLora(recv));*/

       
  digitalWrite (LED_BUILTIN, LOW); 
  wdt_disable();
  wdt_enable(0xFF);
}


bool SendJSON(String recv){

  JsonObject dataSlot;
  JsonArray slot; 

  int id_node = GetInteger(recv.substring(INI_NODE,FIN_NODE));
  doc["ID_Node"]  =  id_node; 
  doc["timestamp"] = serialized("{\".sv\":\"timestamp\"}");
  
  int index = 0;
  int charsPassed = 0;
  int type = 255;
  while(recv.length() > (charsPassed +4)){   
    String str = "DATASLOT_";
    str.concat(index++);
    int posIni = INI_DATA + charsPassed; 
    int posFin = posIni + LON_TYPE;
    type = GetInteger(recv.substring(posIni,posFin)); 
    charsPassed += LON_TYPE;
    dataSlot = doc.createNestedObject(str);
    dataSlot["Type"] = GetInteger(recv.substring(posIni,posFin));
    slot = dataSlot.createNestedArray("Value");
    for(int i = 0; i<quantityOfVal[type]; i++){
       posIni = posFin;
       posFin = posFin + LON_VALUE;
       slot.add(GetInteger(recv.substring(posIni,posFin)));
       charsPassed += LON_VALUE;
    }    
  }
  bool ok = false;
  String JsonSerialized;
  serializeJson(doc, JsonSerialized);
  
  /*******************************************************************/
  /**** Some packages are erroneously received; implement filtering rules!! ***/
  /******************************************************************/
  
  if ((type < 11) && ((id_node > 0) && (id_node < 11)))
  {
    String route = "";
    route = "/Gardens/"+GARDEN+"/sensorData/"+PLOTLIST[id_node]+"/Data/";
    Serial.println(SendFirebase(route, JsonSerialized));
    delay(100);
    ok = true;
  }
  
  doc.clear();
  return ok;
}


int GetInteger(String from){
   char FIELDCHAR[5];
   from.toCharArray(FIELDCHAR, 5);
   return strtol(FIELDCHAR, NULL, 16);
}
