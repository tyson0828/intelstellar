
/*
Intelstellar – Maker :
Stage 1: Ultrasonic sensor to measure the distance.
         Based on the distance, fade LED. If close, make it brighter.
         If distance is less than 25 inches, beep!
Stage 2: Added one more Ultrasonic sensor to detect object on the back.
Stage 3: Send info to to Phone to alert. Vibration & Pop-up alarm.
Stage 4: Added shock detection.
Stage 5[Planning]: Need to send e-mail to family
Stage 6[Planning]: Need to provide different alert image based on the direction(front/back)

*/

// BLE WatchApp
// Runs on Arduino 101 and Arduino boards compatible with https://github.com/sandeepmistry/arduino-BLEPeripheral
#include <CurieBLE.h>
#include "CurieIMU.h"
#include <NewPing.h>
#define PING_PIN 13
#define MAX_DISTANCE 200
#define US_ROUNDTRIP_INCH 74
#define US_ROUNDTRIP_CM 29
unsigned long echo = 0;
unsigned long ultrasoundValue = 0;

NewPing sonar(PING_PIN, PING_PIN, MAX_DISTANCE);

BLEPeripheral blePeripheral;
BLEService service("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEUnsignedLongCharacteristic countCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);  

// Parallax
int signal=13;
int distance;
unsigned long pulseduration=0;
unsigned long cm = 0;
unsigned long inches = 0;


//HC-SR04
int trigPin = 12;    //Trig
int echoPin = 11;    //Echo 
long duration;
long cm2, inches2;


int greenled = 3;          // the PWM pin the LED is attached to
int redled = 9;
int maxBrightness = 100;
int brightness = 0;   // how bright the LED is
int greenBrightness = 0;
int fadeAmount = 30;  // how many points to fade the LED by

int buzzer = 4;          // the pin BUZZER is attahched to

void setup() {
  //Serial Port begin
  Serial.begin (9600);
  //while (!Serial);    // wait for the serial port to open

  /* Initialise the IMU */
  CurieIMU.begin();
  CurieIMU.attachInterrupt(eventCallback);

  /* Enable Shock Detection */
  CurieIMU.setDetectionThreshold(CURIE_IMU_SHOCK, 1500); // 1.5g = 1500 mg
  CurieIMU.setDetectionDuration(CURIE_IMU_SHOCK, 50);   // 50ms
  CurieIMU.interrupts(CURIE_IMU_SHOCK);


  //
  //Define inputs and outputs
  //
  
  // HC-SR04 ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Parallax ultrasonic sensor
  pinMode(signal, OUTPUT);
  
  // declare greenpin 3 to be an output.
  pinMode(greenled, OUTPUT);
  // declare redpin 9 to be an output.
  pinMode(redled, OUTPUT);
  
  // declare pin 8 to be an output.
  pinMode(buzzer, OUTPUT);
  
  blePeripheral.setLocalName("IntelStellar");
  blePeripheral.setDeviceName("IntelStellar");
  blePeripheral.setAdvertisedServiceUuid(service.uuid());
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(countCharacteristic);
  
  blePeripheral.setEventHandler(BLEConnected, onConnect);
  blePeripheral.setEventHandler(BLEDisconnected, onDisconnect);
  
  countCharacteristic.setEventHandler(BLESubscribed, onSubscribe);
  countCharacteristic.setEventHandler(BLEUnsubscribed, onUnsubscribe);
  
  blePeripheral.begin();
  Serial.println(("Bluetooth device active, waiting for connections..."));    
}

void measureDistance()
{
  // set pin as output so we can send a pulse
  pinMode(signal, OUTPUT);

  // set output to LOW
  digitalWrite(signal, LOW);
  delayMicroseconds(5);
 
  // now send the 5uS pulse out to activate Ping)))
  digitalWrite(signal, HIGH);
  delayMicroseconds(5);
  digitalWrite(signal, LOW);
  
  // now we need to change the digital pin
  // to input to read the incoming pulse
  pinMode(signal, INPUT);
 
  // finally, measure the length of the incoming pulse
  pulseduration=pulseIn(signal, HIGH);
}

void loop()
{
  /*
  * Ultrasonic sensor handling
  */
  
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  //delayMicroseconds(5);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  //delayMicroseconds(10);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  //pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  //cm2 = (duration/2) / 29.1;
  cm2 = (duration/2) / US_ROUNDTRIP_CM;
  inches2 = (duration/2) / US_ROUNDTRIP_INCH; 

  //  Serial.print("HC-SR04 : ");
  //  Serial.print(inches2);
  //  Serial.print("in, \t");
  //  Serial.print(cm2);
  //  Serial.println("cm");
  //  delay(200);
  
  /*
  int x = ping();
  Serial.println(x);
  delay(250);
  */
  
  
  // get the raw measurement data from Ping)))
  measureDistance();
 
  // divide the pulse length by half
  pulseduration=pulseduration/2; 
 
  // now convert to centimetres. We're metric here people...
  cm = int(pulseduration/US_ROUNDTRIP_CM);
  inches = int((pulseduration/2)/US_ROUNDTRIP_INCH);

  //  Serial.print("PING : ");
  //  Serial.print(inches);
  //  Serial.print("in, \t");
  //  Serial.print(cm);
  //  Serial.println("cm");
  
    
  /*
    delay(50);
    unsigned int us = sonar.ping();
    Serial.print("Ping: " );
    Serial.println((us/2) / US_ROUNDTRIP_INCH);
    Serial.println(sonar.ping_in());
  */

  /*
  * LED Fade handling based on the distance detection 
  */
 
  // change the brightness for next time based on the distance through the loop:
  if (inches < 10) {
    brightness = 200;
  } else if (inches < 20) {
    brightness = 80;
  } else if (inches < 25) {
    brightness = 20;
  } else {
    brightness = 0;
  }  
  greenBrightness = maxBrightness - brightness;
  
  // set the brightness of pin 9:
  analogWrite(redled, brightness);
  // set the brightness of pin 3:
  analogWrite(greenled,greenBrightness);
  
  // Serial.println(brightness);
  // Serial.println(greenBrightness);
  
  /*
  * Buzz 
  */
  
  if (inches < 25) {
    tone(buzzer, 1000); // send 1KHz sound signal

    // Notice the distance to the phone connected
    Serial.print("BLE sending from Parallax...");
    Serial.println(inches);
    countCharacteristic.setValue(inches);
  } else if (inches2 < 25) {
    tone(buzzer, 500); // send .5KHz sound signal

    // Notice the distance to the phone connected
    Serial.print("BLE sending from HC-SR04...");
    Serial.println(inches2);
    countCharacteristic.setValue(inches2);
  } else {
    noTone(buzzer);
  }
  
  delay(200);
}

void onConnect(BLECentral& central) {
  Serial.print("Central ");
  Serial.print(central.address());
  Serial.println(" connected");
}

void onDisconnect(BLECentral& central) {
  Serial.print("Central ");
  Serial.print(central.address());
  Serial.println(" disconnected");
}

void onSubscribe(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(central.address());
  Serial.print(" subscribed to characteristic ");
  Serial.println(characteristic.uuid());
}

void onUnsubscribe(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(central.address());
  Serial.print(" unsubscribed from characteristic ");
  Serial.println(characteristic.uuid());
}

unsigned long ping()
{ 
  pinMode(signal, OUTPUT); // Switch signalpin to output
  digitalWrite(signal, LOW); // Send low pulse 
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(signal, HIGH); // Send high pulse
  delayMicroseconds(5); // Wait for 5 microseconds
  digitalWrite(signal, LOW); // Holdoff
  pinMode(signal, INPUT); // Switch signalpin to input
  digitalWrite(signal, HIGH); // Turn on pullup resistor
  // please note that pulseIn has a 1sec timeout, which may
  // not be desirable. Depending on your sensor specs, you
  // can likely bound the time like this -- marcmerlin
  // echo = pulseIn(signal, HIGH, 38000)
  echo = pulseIn(signal, HIGH); //Listen for echo
  ultrasoundValue = (echo / 58.138) * .39; //convert to CM then to inches
  return ultrasoundValue;
}

static void eventCallback(void)
{
  if (CurieIMU.getInterruptStatus(CURIE_IMU_SHOCK)) {
    if (CurieIMU.shockDetected(X_AXIS, POSITIVE))
      Serial.println("Negative shock detected on X-axis");
    if (CurieIMU.shockDetected(X_AXIS, NEGATIVE))
      Serial.println("Positive shock detected on X-axis");
    if (CurieIMU.shockDetected(Y_AXIS, POSITIVE))
      Serial.println("Negative shock detected on Y-axis");
    if (CurieIMU.shockDetected(Y_AXIS, NEGATIVE))
      Serial.println("Positive shock detected on Y-axis");
    if (CurieIMU.shockDetected(Z_AXIS, POSITIVE))
      Serial.println("Negative shock detected on Z-axis");
    if (CurieIMU.shockDetected(Z_AXIS, NEGATIVE))
      Serial.println("Positive shock detected on Z-axis");
  }
}
