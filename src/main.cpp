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

#define __SCULPTURE2__ //SCULPTURE1 is Ann, SCULPTURE2 is Soh and Suang Suang

const int NUMDATA1 = 5, NUMDATA2 = 8; //number of data points for each sculpture
const float ann_readings[NUMDATA1] = {2.94, 4.27, 4.09, 0.42, 8.0}; //6 buttons for 6 data points

const int BAND1_1 = 7, BAND1_2 = 4, BAND1_3 = 3, BAND1_4 = 2, BAND1_5 = 2, BAND1_6 = 2; //Sculpture 1: num of pixels per band
const int BAND2_1 = 4, BAND2_2 = 3, BAND2_3 = 3, BAND2_4 = 2, BAND2_5 = 2, BAND2_6 = 2; //Sculpture 2: num of pixels per band

//follow shape of graph. 20 data points
const float sohsuang_readings[NUMDATA2] = {0.13, 0.06, 1.6, 4.38, 7.0, 1.51, 3.8, 0.04 };

const int BAND_DELAY = 500;    //ms delay between each band lightup
const int SLIDER_WAIT = 3000;  //ms idle for slider movement before IDLE_MODE kicks in
#define UPDATES_PER_SECOND 100 //speed of light animation
CHSV cyellow(64, 255, 255);
CHSV cpink(224, 255, 255);

const int buttonPin = 0, sliderPin = 22;

//-------------------- Audio --------------------//

//-------------------- Buttons and Sliders --------------------//

Bounce myButton = Bounce(buttonPin, 15); // 15 = 15 ms debounce time

unsigned int sliderVal;
unsigned int prevSliderVal, currSliderVal;

bool isButtonPressed, isSliderMoved;

//-------------------- Light --------------------//

#define LED_PIN 6

#define LED_TYPE UCS1903
#define COLOR_ORDER GRB //Yes! GRB!

#if defined(__SCULPTURE1__)
const int NUM_LEDS = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5 + BAND1_6;
const int BAND1 = BAND1_1, BAND2 = BAND1_2, BAND3 = BAND1_3, BAND4 = BAND1_4, BAND5 = BAND1_5, BAND6 = BAND1_6;
const int BAND1L = BAND1_1, BAND2L = BAND1_1 + BAND1_2, BAND3L = BAND1_1 + BAND1_2 + BAND1_3, BAND4L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4, BAND5L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5, BAND6L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5 + BAND1_6;
CHSV myColor = cyellow;
const int SCULPTURE_ID = 1;
#elif defined(__SCULPTURE2__)
const int NUM_LEDS = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6;
const int BAND1 = BAND2_1, BAND2 = BAND2_2, BAND3 = BAND2_3, BAND4 = BAND2_4, BAND5 = BAND2_5, BAND6 = BAND2_6;
const int BAND1L = BAND2_1, BAND2L = BAND2_1 + BAND2_2, BAND3L = BAND2_1 + BAND2_2 + BAND2_3, BAND4L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4, BAND5L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5, BAND6L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6;
CHSV myColor = cpink;
const int SCULPTURE_ID = 2;
#else
#error "invalid sculpture ID"
#endif

CRGB leds[NUM_LEDS];
int brightness1, brightness2, brightness3, brightness4, brightness5, brightness6; //band 1 to 6 brightness
int maxBrightLvl = 255;                                                           //variable max brightness
const int IDLE_MODE = 1, BUTTON_MODE = 2, SLIDER_MODE = 3;
unsigned int playMode = IDLE_MODE; //1 - idle mode; 2 - button mode; 3 - slider mode
int activeLedState = 0;            //SCULPTURE 1 : 0 - idle mode, 1 - idle animation has finished dimming, 2 - show brightness according to reading, 3 - has held for a few sec, fade to black and idle
                                   //SCULPTURE 2: 0 - idle mode, 1 - idle animation has finished dimming, show reading animations, 2 - has completed animations, fade to black and idle
unsigned int HOLD_DURATION = 3000; //in msec, how long to hold the active brightness level
bool isMaxBrightness = false;      //to track idle animation direction
elapsedMillis bandms;
unsigned int band_delay = BAND_DELAY;
int readings[20];              //light values for sculpture 2
unsigned int readingsCounter;  //keeps track of indexing the readings array
unsigned int prevVal, currVal; //for comparing prev and current values for dimming and brightening

#include "myfunctions.h"

//-------------------- Setup --------------------//

void setup()
{
  pinMode(sliderPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);

  delay(2000); //power up safety delay

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(255);

  delay(10);
}

//-------------------- Loop --------------------//

void loop()
{
  read_console(); //listen to buttons and sliders

  if (isButtonPressed == true) //process button press
  {
    isButtonPressed = false; //listen again for button presses
    playMode = BUTTON_MODE;

    register_readings(); //register the ann or sohsuang readings into readings[] array. Done once per button press.

    activeLedState = 0; //reset the led if currently active
  }
  else if (isSliderMoved == true)
  {
    isSliderMoved = false; //listen again for slider movement
    playMode = SLIDER_MODE;
  }

  if (playMode = IDLE_MODE)
  {
    fade_animation();
  }
  else if (playMode == BUTTON_MODE) //play in sequence the readings according to bright level
  {
    if (SCULPTURE_ID == 1)
    {
      play_ann_readings();
    }
    else if (SCULPTURE_ID == 2)
    {
      play_sohsuang_readings();
    }
  }
  else if (playMode == SLIDER_MODE)
  {
    if (true)
    {
    }
  }

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}