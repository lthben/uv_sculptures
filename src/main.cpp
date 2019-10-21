/*
  Author: Benjamin Low (Lthben@gmail.com)
  Date: Oct 2019
  Description: 
      Teensy 3.2 with audio shield. 
      Sculpture 1 has 6 buttons. Sculpture 2 has 2 buttons and 2 sliders. 
      Both have bands of neon flex leds to show the UV levels in the form of light. 
      There are sounds associated with all light animations, including a pulsing idle animation. 
      Sculpture 1 shows UV levels in 6 diff places. 
      Sculpture 2 has a playback mode for the buttons and sliders to show UV levels across time at a location. 
*/

#include <Arduino.h>
#include <FastLED.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

//-------------------- USER DEFINED SETTINGS --------------------//

#define __SCULPTURE1__ //SCULPTURE1 is Ann, SCULPTURE2 is Soh and Suang Suang

//Deck near Sunrise Bridge. Sep 18, 2019. 6.45pm -> 0.15
//Sip Garden. Sep 24, 2019. 10.30am -> 6.27
//Sunrise Bridge. Sep 25, 2019. 10.41am -> 0.42
//Sip Garden. Sep 26, 2019. 1.35pm -> 8.03
//Sip Garden. Oct 01, 2019. 10.41am -> 0.98
//Sip Garden. Oct 04, 2019. 1.20pm -> 7.61
const float ann_readings[6] = {0.15, 6.27, 0.42, 8.03, 0.98, 7.61}; //6 buttons for 6 data points

const int BAND1_1 = 7, BAND1_2 = 4, BAND1_3 = 3, BAND1_4 = 2, BAND1_5 = 2, BAND1_6 = 2; //Sculpture 1: num of pixels per band
const int BAND2_1 = 4, BAND2_2 = 3, BAND2_3 = 3, BAND2_4 = 2, BAND2_5 = 2, BAND2_6 = 2; //Sculpture 2: num of pixels per band

//follow shape of graph. 20 data points
const float soh_readings[20] = {0.04, 1.6, 0.03, 3.9, 0.78, 3.8, 0.05, 1.45, 0.04, 7.04, 0.06, 0.57, 0.94, 0.03, 0.75, 0.43, 0.93, 0.04, 0.53, 0.11};
const float suang_readings[20] = {0.04, 0.59, 0.13, 0, 1.6, 0.05, 3.8, 0.09, 0.03, 0.43, 0.02, 0.06, 0.57, 0.94, 0.65, 0.04, 0.75, 0.44, 0.82, 0.04};

const int BAND_DELAY = 500;    //ms delay between each band lightup
#define UPDATES_PER_SECOND 100 //speed of light animation
CHSV cyellow(64, 255, 255);
CHSV cpink(224, 255, 255);

//-------------------- Audio --------------------//

//-------------------- Buttons --------------------//

Bounce button0 = Bounce(0, 15); // 15 = 15 ms debounce time
Bounce button1 = Bounce(1, 15);
Bounce button2 = Bounce(2, 15);
Bounce button3 = Bounce(3, 15);
Bounce button4 = Bounce(4, 15);
Bounce button5 = Bounce(5, 15);

const int slider0 = 22; //slider pin
const int slider1 = 23;

bool isButtonPressed;

//-------------------- Light --------------------//

#define LED_PIN 6

#define LED_TYPE UCS1903
#define COLOR_ORDER GRB //Yes! GRB!

#if defined(__SCULPTURE1__)
const int NUM_LEDS = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5 + BAND1_6;
const int BAND1 = BAND1_1, BAND2 = BAND1_2, BAND3 = BAND1_3, BAND4 = BAND1_4, BAND5 = BAND1_5, BAND6 = BAND1_6;
const int BAND1L = BAND1_1, BAND2L = BAND1_1 + BAND1_2, BAND3L = BAND1_1 + BAND1_2 + BAND1_3, BAND4L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4, BAND5L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5, BAND6L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5 + BAND1_6;
const int SCULPTURE_ID = 1;
#elif defined(__SCULPTURE2__)
const int NUM_LEDS = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6;
const int BAND1 = BAND2_1, BAND2 = BAND2_2, BAND3 = BAND2_3, BAND4 = BAND2_4, BAND5 = BAND2_5, BAND6 = BAND2_6;
const int BAND1L = BAND2_1, BAND2L = BAND2_1 + BAND2_2, BAND3L = BAND2_1 + BAND2_2 + BAND2_3, BAND4L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4, BAND5L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5, BAND6L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6;
const int SCULPTURE_ID = 2;
#else
#error "invalid sculpture ID"
#endif

CRGB leds[NUM_LEDS];
int brightness1, brightness2, brightness3, brightness4, brightness5, brightness6;
bool isMaxBrightness = false; //to track idle animation direction
//isMaxBrightness1, isMaxBrightness2, isMaxBrightness3, isMaxBrightness4, isMaxBrightness5, isMaxBrightness6;
elapsedMillis bandms;

#include "myfunctions.h"

//-------------------- Setup --------------------//

void setup()
{
  pinMode(slider0, INPUT);
  pinMode(slider1, INPUT);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  Serial.begin(9600);

  delay(2000); //power up safety delay

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(255);

  delay(10);
}

//-------------------- Loop --------------------//

void loop()
{

  read_pushbuttons();
  
  idle_animation();

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}