#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const int highFreq = 5000;
const int lowFreq = 2000;
const int soundDuration = 200;

const int PIEZO = 11;
int led2 = 12;
int but1 = 10;
int but2 = 9;

int o = 0;
const int BUTTON_PIN = 2;
const int LED_PIN    = 8;
int ledState = LOW;   
int lastButtonState;
int currentButtonState;


static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

SoftwareSerial mySerial(5, 6);
String cmd = "";

int value;

String msgTxt = "" ;
String ReceivedSMS = "" ;


TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);

  Serial.println(F("GPS TRACKER ALARM"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Makati Group"));
  Serial.println();

  mySerial.begin(9600);
  Serial.println("Initializing SMS...");
  delay(1000);

  mySerial.println("AT");                 // Sends an ATTENTION command, reply should be OK
  updateSerial();
  updateSMS();
  mySerial.println("AT+CMGF=1");          // Configuration for sending SMS
  updateSerial();
  updateSMS();
  mySerial.println("AT+CNMI=1,2,0,0,0");  // Configuration for receiving SMS
  updateSerial();
  updateSMS();


  pinMode(led2, OUTPUT);
  pinMode(but1, INPUT_PULLUP);
  pinMode(but2, INPUT_PULLUP);

 
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  currentButtonState = digitalRead(BUTTON_PIN);
}

void loop() {
  //check if gps available
  while (ss.available() > 0)
    if (gps.encode(ss.read()))

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  int state1 = digitalRead(but1);
  int state2 = digitalRead(but2);
  if (state1 == 0){
    tone(PIEZO, highFreq);  
    delay(soundDuration);
    tone(PIEZO, lowFreq);   
    delay(soundDuration);
    Serial.println("Speaker on");
    }
    else{
       noTone(PIEZO);
      }
   
   if (state2 == 0){
    digitalWrite(led2,HIGH);
    Serial.println("LED ON");
    }
    else{
      digitalWrite(led2,LOW);
      }

  lastButtonState    = currentButtonState;                
  currentButtonState = digitalRead(BUTTON_PIN); 
  if(lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.print("The button is pressed: ");
    if(ledState == LOW) {
       ledState = HIGH;  
       Serial.println("Turning LED on");
       boom();
       int o = o++;
    }
    else {
      ledState = LOW;
      Serial.println("Turning LED off");
      sendGPS();
    }
    digitalWrite(LED_PIN, ledState); 
  }
    
   updateSerial();

}

//GPS Location
void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();

  msgTxt = gps.location.lat() + gps.location.lng() ;

  
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {

    cmd+=(char)Serial.read();
 
    if(cmd!="")
    {
      cmd.trim();  // Remove added LF in transmit
      if (cmd.equals("A")) {
        sendSMS();
      }

      if (cmd.equals("B")) {
        displayInfo();
      }

      if (cmd.equals("C")) {
        sendGPS();
      }
      
      else 
      {
        mySerial.print(cmd);
        mySerial.println("");
      }
    }

     mySerial.write(Serial.read());
  }
  
  
} 

void updateSMS()
{

  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());
  }
}

void sendSMS(){
  mySerial.println("AT+CMGF=1");
  delay(500);
  mySerial.println("AT+CMGS=\"+639762752773\"\r");
  delay(500);
  mySerial.print("Hi! This is Arduino sent by Chan!");
  delay(500);
  mySerial.write(26);
}

void sendGPS(){
  mySerial.println("AT+CMGF=1");
  delay(500);
  mySerial.println("AT+CMGS=\"+639762752773\"\r");
  delay(500);
  mySerial.print("Hi Here is my location  " );
  delay(500);
  mySerial.print(msgTxt);
  delay(500);
  mySerial.write(26);
}

void boom(){
    if ( o == 0){
  for (int i = 1; i <= 5; i++ ){
  tone(PIEZO, highFreq);  
  delay(soundDuration);
  tone(PIEZO, lowFreq);   
  delay(soundDuration);
  digitalWrite(led2,HIGH);
  delay(50);
  digitalWrite(led2,LOW);
  delay(50);
  Serial.println(i);
  o++;
      }
    }
    else{
      Serial.println("Sending message");
   
      }
  }
