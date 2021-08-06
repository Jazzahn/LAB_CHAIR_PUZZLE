
#include <SPI.h>
#include <MFRC522.h>
#include <HX711.h>

#define ROCK_LIGHT 31 
#define HEAD_LIGHT 29
#define VIAL_LIGHT 27
#define ROCK_DOUT 3
#define ROCK_CLK 2
#define RST_PIN 39
#define SS_PIN1 41
#define SS_PIN2 43
#define VIAL_SEND 40
#define HEAD_SEND 42
#define ROCK_SEND 44
#define RESET_PUZZLE 46

HX711 scale(ROCK_DOUT, ROCK_CLK);

MFRC522 headrfid(SS_PIN1, RST_PIN);
MFRC522 vialrfid(SS_PIN2, RST_PIN);


void setup() {

  
  // put your setup code here, to run once:
  Serial.begin(9600);

  SPI.begin();
  
  headrfid.PCD_Init();
  vialrfid.PCD_Init();

  //Enhance the MFRC522 Receiver Gain to maiximum value of some 48db;
  //headrfid.PCD_SetRegisterBitMask(headrfid.RFCfgReg, (0x07<<4));
  //vialrfid.PCD_SetRegisterBitMask(headrfid.RFCfgReg, (0x07<<4));
  
  //Set Scale Zero
  scale.set_scale(-930000);
  scale.tare();


  pinMode(ROCK_LIGHT, OUTPUT);
  pinMode(VIAL_LIGHT, OUTPUT);
  pinMode(HEAD_LIGHT, OUTPUT);
  
  pinMode(ROCK_SEND, OUTPUT);
  pinMode(VIAL_SEND, OUTPUT);
  pinMode(HEAD_SEND, OUTPUT);

  pinMode(RESET_PUZZLE, INPUT);
  //digitalWrite(ROCK_LIGHT, HIGH);


}

void loop() {
  if (digitalRead(RESET_PUZZLE) == HIGH) {
    reset();
  }

  if(headrfid.PICC_IsNewCardPresent()) {
    unsigned long uid = gethID();
    if(uid != -1){
     Serial.print("Card detected, UID: "); Serial.println(uid);
    }
    if (uid == 23874) {
      Serial.println("HEAD CORRECT"); //Placeholder for testing
      digitalWrite(HEAD_SEND, HIGH);
      digitalWrite(HEAD_LIGHT, HIGH);
    }
  } 
  if(vialrfid.PICC_IsNewCardPresent()) {
    unsigned long uid = getvID();
    if(uid != -1){
     Serial.print("Card detected, UID: "); Serial.println(uid);
    }
    if (uid == 23874) {
      Serial.println("VIAL CORRECT"); //Placeholder for testing
      digitalWrite(VIAL_SEND, HIGH);
      digitalWrite(VIAL_LIGHT, HIGH);
    }
  } 
  // put your main code here, to run repeatedly:
  float current_weight=scale.get_units(5);  // get average of 20 scale readings
  float scale_factor=(current_weight/0.184);  // divide the result by a known weight
  if ((current_weight > 0.20) && (current_weight < 0.24))  {
    Serial.println("LIGHT ON");
    digitalWrite(ROCK_LIGHT, HIGH);
    digitalWrite(ROCK_SEND, HIGH);
  } else {
    digitalWrite(ROCK_LIGHT, LOW);
    digitalWrite(ROCK_SEND, LOW);
  }
  Serial.println(scale.get_units(1));  //Print the scale factor to use
  
}

unsigned long gethID(){
  if ( ! headrfid.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return -1;
  }
  unsigned long hex_num;
  hex_num =  headrfid.uid.uidByte[0] << 24;
  hex_num += headrfid.uid.uidByte[1] << 16;
  hex_num += headrfid.uid.uidByte[2] <<  8;
  hex_num += headrfid.uid.uidByte[3];
  headrfid.PICC_HaltA(); // Stop reading
  return hex_num;
}

unsigned long getvID(){
  if ( ! vialrfid.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return -1;
  }
  unsigned long hex_num;
  hex_num =  vialrfid.uid.uidByte[0] << 24;
  hex_num += vialrfid.uid.uidByte[1] << 16;
  hex_num += vialrfid.uid.uidByte[2] <<  8;
  hex_num += vialrfid.uid.uidByte[3];
  vialrfid.PICC_HaltA(); // Stop reading
  return hex_num;
}

void reset() {
  digitalWrite(ROCK_SEND, LOW);
  digitalWrite(VIAL_SEND, LOW);
  digitalWrite(HEAD_SEND, LOW);
  digitalWrite(ROCK_LIGHT, LOW);
  digitalWrite(HEAD_LIGHT, LOW);
  digitalWrite(VIAL_LIGHT, LOW);
}






