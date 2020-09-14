#include "Firebase_Arduino_WiFiNINA.h"
#include <WiFiNINA.h>

//CONFIGURATION 1: NEEDED TO WORK WITH WIFI
char ssid[] = "...";        
char pass[] = "...";  
//END CONFIGURATION 1

//CONFIGURATION 2: NEEDED TO WORK WITH FIREBASE
#define FIREBASE_HOST "....firebaseio.com"//Your database
#define FIREBASE_AUTH "..." //The secret
//END CONFIGURATION 2

FirebaseData firebaseData;
int status = WL_IDLE_STATUS;

void StartFirebase(){
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, ssid, pass);
  Firebase.reconnectWiFi(true);
}

String SendFirebase(String Path, String JsonSerialized){   
   Firebase.reconnectWiFi (true);
   if (!firebaseData.bufferOverflow()) {
              
          if (!Firebase.pushJSON(firebaseData, Path, JsonSerialized)) {
            
             status = WiFi.status ();
             Serial.print("Error Wifi: ");
             Serial.println(status);
             return firebaseData.errorReason();
          } else {
            return "sent";
          }
   
  } else {
    return "error overflow";
  }
}
