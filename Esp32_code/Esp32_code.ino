// Lib to be include
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ESP32Servo.h>


// pin mapinggggggggggggggggg
//------Ultrasonic_sensors-----------
#define TRIG1 19
#define ECHO1 18
#define TRIG2 23
#define ECHO2 17
#define TRIG3 16
#define ECHO3 4
#define TRIG4 25
#define ECHO4 27
//------IR-----------
#define IR1 35
#define IR2 32
//------servo---------
#define servoPin 13
//--------------Wifi_config------------
#define WIFI_SSID "Akhil's M35"
#define WIFI_PASSWORD "AK@18255"
//---------------FireBase--------------
#define DATABASE_URL "smartparkingsystem-b6eea-default-rtdb.asia-southeast1.firebasedatabase.app" 
#define DATABASE_SECRET "NoSlzwEO3IUOLWN1v8O4SdJLb0SsNJLMCOQKDDMD"


// Variables
// treshh the hold
const int threshold = 10; // in centi
//-----------servo_var----------------
int pos = 0; 
//-------garb--------------
  String ps1 = "FREE";
  String ps2 = "FREE";
  String ps3 = "FREE";
  String ps4 = "FREE";
//------Slots-----------
int Avalableslots = 4;
int preAvb = Avalableslots;

// Config
//------------Firebase_config----------
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
//-------------servo_config----------------
Servo myservo;
//----------LCD_display--------------
LiquidCrystal_I2C lcd(0x27,16,2);


//---------------functions------------------
float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    Serial.print("no fricking input from this guy -> ");

    Serial.println(echoPin);
    return -1;
  }
  return duration * 0.034 / 2;
}



void setup() {

  //------------LCD_display----------
  lcd.init();
  lcd.backlight();

  //-----sserial output-------
  Serial.begin(115200);
  Serial.println("Smart Parking System Starting...");
  lcd.setCursor(0,0);
  lcd.print("Smart Parking");
  lcd.setCursor(0, 1);
  lcd.print("System");

  Serial.println("Initializing Ultrasonic Sensor Pins");
  //-----pinmoeds--------
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT); pinMode(ECHO3, INPUT);
  pinMode(TRIG4, OUTPUT); pinMode(ECHO4, INPUT);
  Serial.println("Done.....");

  Serial.println("Initializing IR Pins");
  //-----------IR_setup------------
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  Serial.println("Done.....");

  Serial.println("Initializing Servo Motor");
  //-------servo---------
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50); 
	myservo.attach(servoPin, 10, 5000);
  myservo.write(0);
  Serial.println("Done.....");

  //-------wifi-----------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());


  Serial.println("Initializing FireBase");
  //-----------firebase-----------
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(1024 , 512);
  Firebase.begin(&config, &auth);
  Serial.println("Firebase Initialized");
  //----------Val-rest----------------
  Firebase.setString(fbdo,"/location/1011/Slot/slot1", "FREE");
  Firebase.setString(fbdo,"/location/1011/Slot/slot2", "FREE");
  Firebase.setString(fbdo,"/location/1011/Slot/slot3", "FREE");
  Firebase.setString(fbdo,"/location/1011/Slot/slot4", "FREE");

  //-----------Clean_up---------------
  lcd.clear();

}



void loop() {

  //----------LCD_print------------
  lcd.setCursor(0,0);
  lcd.print("Available slots");

  if(preAvb != Avalableslots){
    lcd.setCursor(3, 1);
    lcd.print("  "); 
    lcd.setCursor(3, 1);
    lcd.print(Avalableslots);
  }
  


  //----------Distance-------------
  float d1 = getDistance(TRIG1, ECHO1);
  delay(10);
  float d2 = getDistance(TRIG2, ECHO2);
  delay(10);
  float d3 = getDistance(TRIG3, ECHO3);
  delay(10);
  float d4 = getDistance(TRIG4, ECHO4);
  delay(10);
  //-------des--------
  String s1 = (d1 > 0 && d1 < threshold) ? "OCCUPIED" : "FREE";
  String s2 = (d2 > 0 && d2 < threshold) ? "OCCUPIED" : "FREE";
  String s3 = (d3 > 0 && d3 < threshold) ? "OCCUPIED" : "FREE";
  String s4 = (d4 > 0 && d4 < threshold) ? "OCCUPIED" : "FREE";



  //-------Parking status---------
  Serial.println("------ Parking Status ------");

  Serial.print("Slot 1: "); Serial.print(s1);
  Serial.print(" | "); Serial.print(d1); Serial.println(" cm");
  if(s1 != ps1) {
    Firebase.setString(fbdo,"/location/1011/Slot/slot1", s1);
  }

  Serial.print("Slot 2: "); Serial.print(s2);
  Serial.print(" | "); Serial.print(d2); Serial.println(" cm");
  if(s2 != ps2) {
    Firebase.setString(fbdo,"/location/1011/Slot/slot2", s2);
  }
  
  Serial.print("Slot 3: "); Serial.print(s3);
  Serial.print(" | "); Serial.print(d3); Serial.println(" cm");
  if(s3 != ps3) {
    Firebase.setString(fbdo,"/location/1011/Slot/slot3", s3);
  }
  
  Serial.print("Slot 4: "); Serial.print(s4);
  Serial.print(" | "); Serial.print(d4); Serial.println(" cm");
  if(s4 != ps4) {
    Firebase.setString(fbdo,"/location/1011/Slot/slot4", s4);
  }
  ps1 = s1;
  ps2 = s2;
  ps3 = s3;
  ps4 = s4;
  Serial.println("----------------------------\n");
  delay(500);




//-----------gate--------------
// ENTRY: car approaches from outside (IR1)
while (digitalRead(IR1) == LOW) {
  if (Avalableslots <= 0) {
    Serial.println("Parking is full");
    lcd.clear();
    lcd.print("Parking is full!");
    delay(2000);
  } else {
    myservo.write(90);
    while (digitalRead(IR2) != LOW);
    delay(1000);
    myservo.write(0);
    Avalableslots--;
    Firebase.setInt(fbdo, "/location/1011/Slot/AvbSlots", Avalableslots);
  }
}

// EXIT
while (digitalRead(IR2) == LOW) {
  myservo.write(90);
  while (digitalRead(IR1) != LOW);
  delay(1000);
  myservo.write(0);
  if (Avalableslots < 4) Avalableslots++;
  Firebase.setInt(fbdo, "/location/1011/Slot/AvbSlots", Avalableslots);
}
  // Lib to be include
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ESP32Servo.h>


// pin mapinggggggggggggggggg
//------Ultrasonic_sensors-----------
#define TRIG1 19
#define ECHO1 18
#define TRIG2 23
#define ECHO2 17
#define TRIG3 16
#define ECHO3 4
#define TRIG4 25
#define ECHO4 27
//------IR-----------
#define IR1 35
#define IR2 32
//------servo---------
#define servoPin 13
//--------------Wifi_config------------
#define WIFI_SSID "Akhil's M35"
#define WIFI_PASSWORD "AK@18255"
//---------------FireBase--------------
#define DATABASE_URL "smartparkingsystem-b6eea-default-rtdb.asia-southeast1.firebasedatabase.app" 
#define DATABASE_SECRET "NoSlzwEO3IUOLWN1v8O4SdJLb0SsNJLMCOQKDDMD"


// Variables
// treshh the hold
const int threshold = 10; // in centi
//-----------servo_var----------------
int pos = 0; 
//-------garb--------------
  String ps1 = "FREE";
  String ps2 = "FREE";
  String ps3 = "FREE";
  String ps4 = "FREE";
//------Slots-----------
int Avalableslots = 4;
int preAvb = Avalableslots;

// Config
//------------Firebase_config----------
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
//-------------servo_config----------------
Servo myservo;
//----------LCD_display--------------
LiquidCrystal_I2C lcd(0x27,16,2);


//---------------functions------------------
float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    Serial.print("no fricking input from this guy -> ");

    Serial.println(echoPin);
    return -1;
  }
  return duration * 0.034 / 2;
}



void setup() {

  //------------LCD_display----------
  lcd.init();
  lcd.backlight();

  //-----sserial output-------
  Serial.begin(115200);
  Serial.println("Smart Parking System Starting...");
  lcd.setCursor(0,0);
  lcd.print("Smart Parking");
  lcd.setCursor(0, 1);
  lcd.print("System");

  Serial.println("Initializing Ultrasonic Sensor Pins");
  //-----pinmoeds--------
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT); pinMode(ECHO3, INPUT);
  pinMode(TRIG4, OUTPUT); pinMode(ECHO4, INPUT);
  Serial.println("Done.....");

  Serial.println("Initializing IR Pins");
  //-----------IR_setup------------
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  Serial.println("Done.....");

  Serial.println("Initializing Servo Motor");
  //-------servo---------
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50); 
	myservo.attach(servoPin, 10, 5000);
  myservo.write(0);
  Serial.println("Done.....");

  //-------wifi-----------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());


  Serial.println("Initializing FireBase");
  //-----------firebase-----------
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(1024 , 512);
  Firebase.begin(&config, &auth);
  Serial.println("Firebase Initialized");
  //----------Val-rest----------------
  Firebase.setString(fbdo,"/location/1011/Slot/slot1", "FREE");
  Firebase.setString(fbdo,"/location/1011/Slot/slot2", "FREE");
  Firebase.setString(fbdo,"/location/1011/Slot/slot3", "FREE");
  Firebase.setString(fbdo,"/location/1011/Slot/slot4", "FREE");

  //-----------Clean_up---------------
  lcd.clear();

}



void loop() {

  //----------LCD_print------------
  lcd.setCursor(0,0);
  lcd.print("Available slots");

  if(preAvb != Avalableslots){
    lcd.setCursor(3, 1);
    lcd.print("  "); 
    lcd.setCursor(3, 1);
    lcd.print(Avalableslots);
  }
  


  //----------Distance-------------
  float d1 = getDistance(TRIG1, ECHO1);
  delay(10);
  float d2 = getDistance(TRIG2, ECHO2);
  delay(10);
  float d3 = getDistance(TRIG3, ECHO3);
  delay(10);
  float d4 = getDistance(TRIG4, ECHO4);
  delay(10);
  //-------des--------
  String s1 = (d1 > 0 && d1 < threshold) ? "OCCUPIED" : "FREE";
  String s2 = (d2 > 0 && d2 < threshold) ? "OCCUPIED" : "FREE";
  String s3 = (d3 > 0 && d3 < threshold) ? "OCCUPIED" : "FREE";
  String s4 = (d4 > 0 && d4 < threshold) ? "OCCUPIED" : "FREE";



  //-------Parking status---------
  Serial.println("------ Parking Status ------");

  Serial.print("Slot 1: "); Serial.print(s1);
  Serial.print(" | "); Serial.print(d1); Serial.println(" cm");
  if(s1 != ps1) {
    Firebase.setString(fbdo,"/location/1011/Slot/slot1", s1);
  }

  Serial.print("Slot 2: "); Serial.print(s2);
  Serial.print(" | "); Serial.print(d2); Serial.println(" cm");
  if(s2 != ps2) {
    Firebase.setString(fbdo,"/location/1011/Slot/slot2", s2);
  }
  
  Serial.print("Slot 3: "); Serial.print(s3);
  Serial.print(" | "); Serial.print(d3); Serial.println(" cm");
  if(s3 != ps3) {
    Firebase.setString(fbdo,"/location/1011/Slot/slot3", s3);
  }
  
  Serial.print("Slot 4: "); Serial.print(s4);
  Serial.print(" | "); Serial.print(d4); Serial.println(" cm");
  if(s4 != ps4) {
    Firebase.setString(fbdo,"/location/1011/Slot/slot4", s4);
  }
  ps1 = s1;
  ps2 = s2;
  ps3 = s3;
  ps4 = s4;
  Serial.println("----------------------------\n");
  delay(500);




//-----------gate--------------
// ENTRY: car approaches from outside (IR1)
while (digitalRead(IR1) == LOW) {
 if (Avalableslots <= 0) {
  lcd.setCursor(0, 0);
  lcd.print("Parking Full!   ");
  lcd.setCursor(0, 1);
  lcd.print("No slots left!  ");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("Available slots ");  // restore
}else {
    myservo.write(90);
    while (digitalRead(IR2) != LOW);
    delay(1000);
    myservo.write(0);
    Avalableslots--;
    Firebase.setInt(fbdo, "/location/1011/Slot/AvbSlots", Avalableslots);
  }
}

// EXIT
while (digitalRead(IR2) == LOW) {
  myservo.write(90);
  while (digitalRead(IR1) != LOW);
  delay(1000);
  myservo.write(0);
  if (Avalableslots < 4) Avalableslots++;
  Firebase.setInt(fbdo, "/location/1011/Slot/AvbSlots", Avalableslots);
}
  preAvb = Avalableslots;
}
}