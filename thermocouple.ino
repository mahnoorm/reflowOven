#include <PID_v1.h>
#include <Adafruit_MAX31855.h>
#include <Keypad.h>


//defining data out, chip select, and clock I/O pins
#define SWITCH 2
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5
#define RelayPin 6

//switch bitch
#define START 23

//Reflow Steps
#define Preheat "PREHEAT"
#define Soak "SOAK"
#define ReflowZone "REFLOW"
#define ReflowZoneRise "REFLOW_RIZE"
#define ReflowZoneConst "REFLOW_CONST"
#define Cooling "COOL"

//Templretue SetPoints
#define preHeatSetPoint 150
#define soakSetPoint 180
#define reflowSetPoint 280
#define cooling 0


//Define Variables we'll be connecting to
double Setpoint, Input, Output;
unsigned long startTimeProgram;
unsigned long startTimeReflow;
String reflowStep;
bool on=false;
long int preheatTime = 90000;
long int soakTime = 180000;
long int reflowRiseTime = 290000;
long int ReflowZoneConstTime = 340000;
long int coolingTime = 380000;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
 {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT);

int WindowSize = 5000;
unsigned long windowStartTime;

void setup() {
  // put your setup code here, to run once:
   pinMode(RelayPin, OUTPUT);
   pinMode(SWITCH, INPUT);
   

  //initialize the variables we're linked to
  Setpoint = 0;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  windowStartTime = millis();
  startTimeProgram=millis();
  reflowStep=Preheat; //step 1 is Preheating
  
  Serial.begin(9600);
  delay(500);

  //set-up keypad
  Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

}

void loop() {
Serial.println(digitalRead(SWITCH));
double c = thermocouple.readCelsius();
startTimeProgram=millis();

if (digitalRead(SWITCH)){
    if (on==false)
      {
        Serial.println("ON BITCH");
        on=true;
        startTimeReflow=startTimeProgram;
        }
  Serial.println("Staying ON BITCH");
  // Serial.print("start time: ");
  long int display_ = (long int)startTimeProgram-(long int)startTimeReflow;
  //Serial.println(display_);

   Input = thermocouple.readCelsius();
   myPID.Compute();

  
  if (reflowStep==Preheat){
    Setpoint=preHeatSetPoint;
    myPID.SetTunings(50,0,0);
    /*
    Serial.print("display: ");
    Serial.print(display_);
    Serial.print(" preheatTime: ");
    Serial.print(preheatTime);
    Serial.print(" ");
    Serial.println(display_>=preheatTime);
    */
    if (display_>=preheatTime){
      reflowStep=Soak;
      }
    }
  
  else if (reflowStep==Soak){
    Setpoint=soakSetPoint;
    if (display_>=soakTime)
      reflowStep=ReflowZoneRise;
    }
    
   //stages we will test afterwards
  else if (reflowStep==ReflowZoneRise){
    Setpoint=reflowSetPoint;
    myPID.SetTunings(50,0,0);
    if (display_>=reflowRiseTime){
      reflowStep=ReflowZoneConst;
    }
  }
  else if(reflowStep==ReflowZoneConst){
  Setpoint = reflowSetPoint;
  myPID.SetTunings(50,0,0);
    if (display_>= ReflowZoneConstTime){
      reflowStep=Cooling;
    }
  }
  else if (reflowStep==Cooling){
    Setpoint=cooling;
 if (isnan(c)&&c<=28.0) {
    reflowStep = Preheat;
    
   }
    }
   double c = thermocouple.readCelsius();
   Serial.print(reflowStep);
 //Serial.print(", startTimeProgram:");
  // Serial.print(startTimeProgram);
  // Serial.print(", startTimeRReflow:");
//   Serial.print(startTimeReflow);
   Serial.print(", displayTime:");
   Serial.print(display_);
   
   Serial.print(", ");
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple!");
   } else {
     Serial.println(c); 
   }

  unsigned long now = millis();
  if (now - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
 
  if (Output > now - windowStartTime) {
    digitalWrite(RelayPin, HIGH);
    }
  else {
    digitalWrite(RelayPin, LOW);
  }
   delay(1000);
  }
  else {
    Serial.println("NOT ON BITCH");
    on=false;
    reflowStep = Preheat;
    startTimeReflow=startTimeProgram;
     digitalWrite(RelayPin, LOW);
    }
}


