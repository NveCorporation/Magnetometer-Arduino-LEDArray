
/**************************************************************************************
Sends the output of an AA-Series analog magnetic sensor to a NeoPixel LED array 
via an Arduino ATmega32U4-based board (pinouts vary with the particular board).
A2 (ADC5) = sensor OUT+; A4 (ADC1) = sensor OUT-; D2 = threshold LED; D3 = LED array;
Three-position switch selects thresholds--SP3T on D5 (Pole), D6, D7, and D8 (throws).
                                                                           Rev. 4/6/18
**************************************************************************************/
#include <Adafruit_NeoPixel.h>
int sensor; //100 mV/V = 256; 3.6 mV/V/Oe for the AAL004==>0.1 Oe/bit; 15 Oe full scale
int threshold = 140;
const int differential = 10;
const int offset = 7;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, 3, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(2, OUTPUT); //Threshold output
  pinMode(5, OUTPUT); //Switch enable
  pinMode(6, INPUT); pinMode(7, INPUT); pinMode(8, INPUT); //Switch inputs
  digitalWrite(6,1); digitalWrite(7,1); digitalWrite(8,1); //Turn on pull-ups
  strip.begin(); //Initialize LED array
  ADCSRA |= 0x80; //Enable ADC (bit 7)
  ADCSRB |= 0x20; //MUX5 (ADCSRB bit 5) = 1 to enable internal differential amplifier
  ADMUX = 0x4D; //Ref=Vcc, right adjusted, differential input, ADC = 10*(ADC5-ADC1)
}
void loop() {
  threshold = 140*(!digitalRead(6))+72*(!digitalRead(7))+15*(!digitalRead(8)); //Read switch 
  ADCSRA |= 0x40; //Start ADC conversion
  while(ADCSRA & 0x40); //Wait for ADC
  sensor = ADC; //Read sensor
  sensor = (sensor-(sensor>512)*1024)/2-offset; //9-bit 2's complement w/offset correction
  if (sensor<0) sensor=0; //Clamp output
  if (sensor>143) sensor=143;
  digitalWrite(2, sensor >= threshold-digitalRead(2)*differential); //Update board LED
  strip.clear(); //Reset the LED array
  strip.setPixelColor(threshold, 255, 255, 0); //Display turn-on threshold (yellow)
  strip.setPixelColor(threshold - differential, 0, 255, 0); //Turn-off threshold (green)
  strip.setPixelColor(sensor, 255, 0, 0); //Display sensor output (red)
  strip.show();
  delay(50); //20 samples/sec.
}
