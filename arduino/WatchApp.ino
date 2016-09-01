/*
Intelstellar – Maker :
Stage 1: Ultrasonic sensor to measure the distance.
               Based on the distance, fade LED. If close, make it brighter.
               If distance is less than 25 inches, beep!
*/

// BLE WatchApp
// Runs on Arduino 101 and Arduino boards compatible with https://github.com/sandeepmistry/arduino-BLEPeripheral
#include <CurieBLE.h>

BLEPeripheral blePeripheral;
BLEService service("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEUnsignedLongCharacteristic countCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);  

// Parallax
int signal=11;
int distance;
unsigned long pulseduration=0;
unsigned long cm = 0;
unsigned long inches = 0;

/*
HC-SR04
int trigPin = 11;    //Trig - green Jumper
int echoPin = 12;    //Echo - yellow Jumper
long duration, cm, inches;
*/

int led = 9;          // the PWM pin the LED is attached to
int maxBrightness = 150;
int brightness = 0;   // how bright the LED is
int fadeAmount = 20;  // how many points to fade the LED by

int buzzer = 8;          // the pin BUZZER is attahched to

void setup() {
  //Serial Port begin
  Serial.begin (9600);
  while (!Serial);    // wait for the serial port to open

/*
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  */
  pinMode(signal, OUTPUT);
  
  // declare pin 9 to be an output.
  pinMode(led, OUTPUT);

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
  /*
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
*/

// get the raw measurement data from Ping)))
 measureDistance();
 
 // divide the pulse length by half
 pulseduration=pulseduration/2; 
 
 // now convert to centimetres. We're metric here people...
 cm = int(pulseduration/29);
 inches = int((pulseduration/2)/74);
 
  // convert the time into a distance
 /* cm = (duration/2) / 29.1;
  inches = (duration/2) / 74; 
*/

  Serial.print(inches);
  Serial.print("in, \t");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

/*
  * LED Fade handling based on the distance detection 
  */
  // set the brightness of pin 9:
  analogWrite(led, brightness);

  // change the brightness for next time based on the distance through the loop:
  brightness = maxBrightness - (int) inches/10 * fadeAmount;

/*
* Buzz 
 */
  
  if (inches < 25) {
    tone(buzzer, 1000); // send 1KHz sound signal

    // Notice the distance to the phone connected
    Serial.print("BLE sending...");
    Serial.print(inches);
    Serial.println();
    countCharacteristic.setValue(inches);
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

