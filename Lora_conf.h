#include "RAK811.h"
#include "SoftwareSerial.h"

#define WORK_MODE LoRaP2P 
#define TXpin 11
#define RXpin 10
#define RESET_PIN 12
#define DebugSerial Serial

SoftwareSerial RAKSerial(RXpin, TXpin);
RAK811 RAKLoRa(RAKSerial, DebugSerial);
void StartLoraReceiver(void);
bool initP2P(void);
void LoRaRAK811_setup(void);

String FREQ = "868100000";  // frequency     range:(860000000 - 1020000000)

void LoRaRAK811_setup() {

  Serial.begin(9600);
  DebugSerial.println("STARTING. Starting LoRa transceiver...");
  StartLoraReceiver();
  Serial.println("STARTED.  LoRa Transceiver Started.\n\n");
  while (!initP2P());
}

bool initP2P() {
  RAKLoRa.rk_setWorkingMode(WORK_MODE);
  Serial.println("You set Working mode: is OK!");
  if (RAKLoRa.rk_initP2P(FREQ, 7, 0, 1, 8, 14)) //FREQ,SF: 7,BW: 0,CR: 1,PRlen: 8,PWR: 14
  {
    Serial.println("You init P2P parameter: is OK!");
    return true;
  }
  return false;
}


void StartLoraReceiver() {
  pinMode(RESET_PIN, OUTPUT);
  //Setup Debug Serial on USB Port
  Serial.begin(9600);
  Serial.println("Serial StartUP");
  delay(200);
  digitalWrite(RESET_PIN, LOW);   // turn the pin low to Reset
  delay(400);
  digitalWrite(RESET_PIN, HIGH);    // then high to enable
  delay(400);
  while (DebugSerial.read() >= 0) {}
  while (!DebugSerial);
  //Print debug info
  Serial.println("Serial Reset");
  //Reset the RAK Module
  RAKSerial.begin(9600); // Arduino Shield
  RAKSerial.println("at+uart=9600,8,0,0,0");
  delay(100);
  digitalWrite(RESET_PIN, LOW);   // turn the pin low to Reset
  delay(400);
  digitalWrite(RESET_PIN, HIGH);    // then high to enable
  delay(400);
  Serial.println("RAK Reset Success");
  delay(100);
  Serial.println(RAKLoRa.rk_getVersion());
  delay(100);
  Serial.println(RAKLoRa.rk_getBand());
  delay(100);
  Serial.println(RAKLoRa.rk_getP2PConfig());
  delay (100);
}

String GetValueLora(String from) {
  String str = from + " ";
  char str_array[str.length() + 2];
  str.toCharArray(str_array, str.length());
  char* token = strtok(str_array, ",");
  int n = 5;
  while (n > 0) {
    token = strtok(0, ",");
    n--;
  }
  return token;
}
