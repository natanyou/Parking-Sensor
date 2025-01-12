/*
This code is used to run a parking sensor. The sensor includes a buzzer and LEDS which are operated
accordingly to the distance of an object to the sensor, a mute button, an LCD displaying the distance
of an object from the sensor, and 2 potentiometers to control the buzzer volume and the LCD brightness.

Circuit connection:

Buzzer - connected between pin 2 and GND. A potentiometer is connected between GND and VDD, and the
middle leg is connected to the line between pin 2 and the buzzer. 

Ultrasonic Sensor - VDD and GND accordingly. Trigger to pin 3 and echo to pin 4.

LEDS - 
Green led: VDD -> 220 Ohm resistor -> green LED -> GND.
Red led: pin 13 -> 220 Ohm resistor -> red LED -> GND.
yellow led: pin 12 -> 220 Ohm resistor -> yellow LED -> GND.

LCD Screen - 
VSS -> GND; VDD -> VDD ;V0 -> middle leg of potentiometer. Other legs are at VDD and GND accordingly.
RS -> pin 10; RW -> GND; E -> pin 9.
D4 -> pin 8; D5 -> pin 7; D6 -> pin 6; D7 -> pin 5.
A -> 220 Ohm resistor -> VDD; K -> GND.

Mute Button - upper leg to VDD, lower Leg to 10k Ohm resistor and than GND. Pin 11 in parallel to
the resistor.
*/

// include the liquid crystal library code
#include <LiquidCrystal.h>

// Initialize pins
const int buzzer = 2, trig = 3, echo = 4, d7 = 5, d6 = 6, d5 = 7, d4 = 8, en = 9, rs = 10, mute = 11, redLED = 12, yellowLED = 13;

// initialize the library by associating any needed LCD interface pin with the arduino pin number it is connected to
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Set sensor parameters
int freq = 400, onTime = 500;
int offTime[] = { 500, 300, 200, 70, 1 };  // Closer distance -> smaller off time
long duration;                             // Duration of pulse
float distance;                            // Measured distance
float prox[] = { 50, 40, 30, 20, 10 };     // Important proximites that trigger the buzzer
int numOfProx = sizeof(prox) / sizeof(prox[0]);
void buzzerOn(int delayTime, int freq);  // Buzzer activation function
bool isMuted = true;
int lastButtonState = LOW;  // Tracks the previous button state
int debounceDelay = 50;     // Debounce delay for the button

void setup() {
  Serial.begin(9600);  // Begin serial communication
  lcd.begin(16, 2);    // Set up the LCD's number of columns and rows:
  pinMode(buzzer, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(mute, INPUT);
}

void loop() {
  // Button edge detection for toggling mute
  int currentButtonState = digitalRead(mute);                  // Read the current button state
  if (currentButtonState == HIGH && lastButtonState == LOW) {  // Detect button press
    delay(debounceDelay);                                      // Debounce delay
    isMuted = !isMuted;                                        // Toggle the mute state
    Serial.println(isMuted ? "Muted" : "Unmuted");
  }
  lastButtonState = currentButtonState;  // Update the last button state

  if (!isMuted) {
    // Measure distance
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    duration = pulseIn(echo, HIGH);
    distance = duration * 0.0344 / 2;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Set buzzer and LEDs according to distance
    for (int i = 0; i <= numOfProx - 2; i++) {
      if (distance <= prox[i] && distance >= prox[i + 1]) {
        lcd.print(distance);
        digitalWrite(yellowLED, HIGH);
        digitalWrite(redLED, LOW);
        buzzerOn(offTime[i], freq);
        lcd.clear();
      }
    }
    if (distance <= prox[numOfProx - 1]) {
      lcd.print(distance);
      digitalWrite(yellowLED, HIGH);
      digitalWrite(redLED, HIGH);
      buzzerOn(offTime[numOfProx - 1], freq);
      lcd.clear();
    } else if (distance >= prox[0]) {
      lcd.print(distance);
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      delay(1000);
      lcd.clear();
    }
  }
}

// Define a function to turn on the buzzer at set rate and frequency
void buzzerOn(int rate, int freq) {
  tone(buzzer, freq);
  delay(onTime);
  noTone(buzzer);
  delay(rate);
}
