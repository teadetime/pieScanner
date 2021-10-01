#include <Servo.h>
Servo panServo;  
Servo tiltServo;  

const byte panPin = 5;         // Digital pin 5
const byte tiltPin = 6;        // Digital Pin on 6
const byte distPin = 0;        // Pinn for the SharpIR Sensor Analog 0

const byte buttonPin = 2;      // the number of the pushbutton pin
const byte ledPin = 13;        // the number of the LED pin

// Variables range for the scan
const byte panZero = 95;  
const int panMin = -0;      // Region to Scan
const int panMax = 0;       // Region to Scan the other way
const byte panStep = 2;
byte panPos = panZero + panMin;// variable to store the Pan servo position
const byte tiltZero = 100;
const int tiltMin = -0;     // Tilt Region to Scan
const int tiltMax = 0;      // Tilt Region to Scan other way
const byte tiltStep = 2;
byte tiltPos = tiltZero + tiltMin;        // variable to store the Tilt servo position

bool panningRight = true;
bool movingUp = false;

// Variables will change:
int ledState = HIGH;                  // the current state of the output pin
int buttonState;                      // the current reading from the input pin
int lastButtonState = LOW;            // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long currTime = 0;
unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers

unsigned long lastStepTime = 0;       // the last time the output pin was toggled
unsigned int stepDelay = 50;         // How long to wait for servo to get to position

// Distance Summation
const byte numReadings = 40;         // number of readings to sum
byte readingCounter = 0; 
const byte readRate = 15;             //Microseconds between analog reads
short distArray[numReadings];
const byte trimFront = 10;            // Trim the top 10 Percent
const byte trimBack = 10;             // Trim the bottom 5 Percent
const byte arrayFront = numReadings/trimFront; //Start position to average in the array
const byte arrayBack = numReadings - (numReadings/trimBack);   //End Pos. in the array

bool moving = false;
unsigned long totalReading = 0;
int avgReading  = 0;

int mode = 0;

short cmpfunc (const void * a, const void * b) {
          return ( *(short*)a - *(short*)b );
}

void setup() {
  Serial.begin(9600);
  panServo.attach(panPin);
  tiltServo.attach(tiltPin);
  panServo.write(panPos);
  tiltServo.write(tiltPos);
  pinMode(buttonPin, INPUT);   // sets the digital pin 13 as output
  delay(2000);
}

void loop() {
  currTime = millis();
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((currTime - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
        ledState = !ledState;
        
        Serial.println("Changing mode!!!");
        mode = (mode + 1) % 3;
        //Do some reset between modes?
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  
  if(mode == 0){
    if(moving){
      // Make sure its been step delay time since moving
      if((currTime - lastStepTime) > stepDelay ){ 
        moving = false;
      }
    }
    else{//NOT MOVING, this means we are reading from sensor!
      if (readingCounter >= numReadings){
        lastStepTime = currTime;
        moving = true;
        qsort(distArray, numReadings, sizeof(short), cmpfunc);
        readingCounter = 0;
        // Now Average
        for (int i=arrayFront; i<(sizeof(distArray) - arrayBack)/sizeof(distArray[0]); i++) {
          readingCounter += 1;
          totalReading += distArray[i];
        }
        avgReading = totalReading/readingCounter;
        Serial.print(panPos-panZero);    Serial.print(",");
        Serial.print(tiltPos-tiltZero);    Serial.print(",");
        Serial.print(avgReading);    Serial.print(",");
        Serial.println("0");
        readingCounter = 0; // Reset the array so that it can be repopulated
        avgReading = 0;
        totalReading = 0;

        //NOW DO THE PANNING
        if(panningRight){
          //Set the servo to next position
          panPos += panStep;
          if(panPos > (panMax+panZero)){
            //Time to move up
            panPos = (panMax+panZero);
            movingUp = true;
            panningRight = !panningRight;
          }
        }
        else{
          panPos -= panStep;
          if(panPos < (panMin+panZero)){
            //Time to move up
            panPos = (panMin+panZero);
            movingUp = true;
            panningRight = !panningRight;
          }
        }
        panServo.write(panPos); 
        //MOVE UP IF Needed
        if(movingUp){
          tiltPos += tiltStep;
          if(tiltPos > (tiltMax+tiltZero)){
            //Serial.println("ALL DONE");     //Reached the top of the scan Stop sending Message so matlab breaks
            // Do cleanup so another scan can be run
            tiltPos = (tiltMin+tiltZero);
            panPos = (panMin+ panZero);
            mode = 2;
          }
          else{
            tiltServo.write(tiltPos);
          }
          movingUp = false;
        }
      }
      // It's not time to move, do another reading
      else{
        distArray[readingCounter] = analogRead(distPin);
        readingCounter += 1;
        delay(readRate); // Don't sum readings all the time because that makes error
      }
    }  
  }
  
  // This mode Prints out 100 readings from the distance sensor
  else if(mode == 1){
    if(readingCounter < numReadings){
      distArray[readingCounter] = analogRead(distPin);
      readingCounter += 1;
      delay(readRate); //Take readings 50 ms apart
    }
    else{
     for(int i=0; i<sizeof(distArray) / sizeof(distArray[0]); i++)
     {
        Serial.print("");
        Serial.print(i);
        Serial.print(",");
        Serial.print(distArray[i]);
        Serial.print(";");
     }
     Serial.println("\n");
     qsort(distArray, numReadings, sizeof(short), cmpfunc);
     readingCounter = 0;
     // Now Average
     for (int i=arrayFront; i<(sizeof(distArray) - arrayBack)/sizeof(distArray[0]); i++) {
       readingCounter += 1;
       totalReading += distArray[i];
//       Serial.print("");
//       Serial.print(i);
//       Serial.print(",");
//       Serial.print(distArray[i]);
//       Serial.print(";");
     }
     Serial.println("\n");
     avgReading = (totalReading/readingCounter);
     Serial.println(avgReading);
        
    readingCounter = 0;
    totalReading = 0;
    delay(2000);
      
    }
  }
}
