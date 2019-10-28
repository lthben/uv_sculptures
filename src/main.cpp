/*
  Author: Benjamin Low (Lthben@gmail.com)
  Date: Oct 2019
  Description: 
      Teensy 3.2 with audio shield. 
      Sculpture 1 and 2 have one button and one slider each.
      The button is playback mode for the light readings. The slider is toggle mode for the different readings. 
      Both have bands of neon flex leds to show the UV levels in the form of light. 
      There are sounds associated with all light animations, including a pulsing idle animation. 
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

//band 1 is inner most centre where the idle fade animation starts from
const int BAND1_1 = 5, BAND1_2 = 10, BAND1_3 = 10, BAND1_4 = 40, BAND1_5 = 60, BAND1_6 = 100; //Sculpture 1: num of pixels per band. 6 bands.
const int BAND2_1 = 30, BAND2_2 = 30, BAND2_3 = 30, BAND2_4 = 20, BAND2_5 = 20, BAND2_6 = 20, BAND2_7 = 20;//Sculpture 2: num of pixels per band. 7 bands.

const int NUMDATA1 = 5, NUMDATA2 = 8; //number of data points for each sculpture
// const float ann_readings[NUMDATA1] = {2.94, 4.27, 4.09, 0.42, 8.0}; //actual values
const float ann_readings[NUMDATA1] = {1.0, 8.0, 1.0, 8.0, 1.0}; //test values

// const float sohsuang_readings[NUMDATA2] = {0.13, 0.06, 1.6, 4.38, 7.0, 1.51, 3.8, 0.04}; //actual values
const float sohsuang_readings[NUMDATA2] = {1.0, 7.0, 1.0, 7.0, 1.0, 7.0, 1.0, 7.0}; //test values

const int BAND_DELAY = 500;   //ms delay between each band lightup
const int SLIDER_WAIT = 3000; //ms idle for slider movement before IDLE_MODE kicks in

CHSV cyellow(64, 255, 255); //sculpture 1 colour
CHSV cpink(224, 255, 255); //sculpture 2 colour


//-------------------- Audio --------------------//

// GUItool: begin automatically generated code. See https://www.pjrc.com/teensy/gui/index.html
AudioPlaySdWav playSdWav1; //xy=416,186
AudioOutputI2S i2s1;       //xy=821,189
AudioConnection patchCord1(playSdWav1, 0, i2s1, 0);
AudioConnection patchCord2(playSdWav1, 1, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1; //xy=615,336
// GUItool: end automatically generated code

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

float vol = 0.5; //master volume gain 0.0 - 1.0

//-------------------- Buttons and Sliders --------------------//

const int buttonPin = 8;
const int sliderPin = A6;//A8 does not work for some strange reason. Causes sound to not be heard even when playing.
Bounce myButton = Bounce(buttonPin, 15); // 15 = 15 ms debounce time

unsigned int sliderVal, prevSliderVal, currSliderVal;
unsigned int sliderPosIndex, prevSliderPosIndex, currSliderPosIndex; //0 - 4 for Ann, 0 - 7 for Soh & Suang

bool isButtonPressed, isSliderToggled;

//-------------------- Light --------------------//
const int LEDPIN0 = 0, LEDPIN1 = 1, LEDPIN2 = 2, LEDPIN3 = 3, LEDPIN4 = 4, LEDPIN5 = 5, LEDPIN6 = 6;

#define LED_TYPE UCS1903
#define COLOR_ORDER GRB //Yes! GRB!

#if defined(__SCULPTURE1__)
// const int NUM_LEDS = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5 + BAND1_6;
const int BAND1 = BAND1_1, BAND2 = BAND1_2, BAND3 = BAND1_3, BAND4 = BAND1_4, BAND5 = BAND1_5, BAND6 = BAND1_6, BAND7 = 0;
// const int BAND1L = BAND1_1, BAND2L = BAND1_1 + BAND1_2, BAND3L = BAND1_1 + BAND1_2 + BAND1_3, BAND4L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4, BAND5L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5, BAND6L = BAND1_1 + BAND1_2 + BAND1_3 + BAND1_4 + BAND1_5 + BAND1_6;
CHSV myColor = cyellow;
const int SCULPTURE_ID = 1;
const char *idleTrack = "DRONE1.WAV"; const char *activeTrack = "RAYGUN.WAV";
CRGB leds0[BAND1_1], leds1[BAND1_2], leds2[BAND1_3], leds3[BAND1_4], leds4[BAND1_5], leds5[BAND1_6], leds6[0];

#elif defined(__SCULPTURE2__)
// const int NUM_LEDS = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6 + BAND2_7;
const int BAND1 = BAND2_1, BAND2 = BAND2_2, BAND3 = BAND2_3, BAND4 = BAND2_4, BAND5 = BAND2_5, BAND6 = BAND2_6, BAND7 = BAND2_7;
// const int BAND1L = BAND2_1, BAND2L = BAND2_1 + BAND2_2, BAND3L = BAND2_1 + BAND2_2 + BAND2_3, BAND4L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4, BAND5L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5, BAND6L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6, BAND7L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6 + BAND2_7;
CHSV myColor = cpink;
const int SCULPTURE_ID = 2;
const char *idleTrack = "DRONE2.WAV"; const char *activeTrack = "TINKLING.WAV";
CRGB leds0[BAND2_1], leds1[BAND2_2], leds2[BAND2_3], leds3[BAND2_4], leds4[BAND2_5], leds5[BAND2_6], leds6[BAND2_7];

#else
#error "invalid sculpture ID"
#endif

#define UPDATES_PER_SECOND 100 //speed of light animation

int brightness1, brightness2, brightness3, brightness4, brightness5, brightness6, brightness7; //band 1 to 6 brightness
int maxBrightLvl = 255;                                                           //variable max brightness
const int IDLE_MODE = 1, BUTTON_MODE = 2, SLIDER_MODE = 3;
unsigned int playMode = IDLE_MODE; 
bool hasplayModeChanged; //for audio track changes
int activeLedState = 0;            //to track led animaton states, e.g. 0 - idle mode, start fade to black 1 - show brightness according to reading, 2 - has completed animations, fade to black and idle
bool isMaxBrightness = false;      //to track idle animation direction
elapsedMillis bandms;              //multiple use time ellapsed counter
unsigned int band_delay = BAND_DELAY; //speed of fade animation
int readings1[NUMDATA1], readings2[NUMDATA2]; //brightness values translated from UV readings
unsigned int readingsCounter;                 //keeps track of indexing the readings array
unsigned int prevBrightVal, currBrightVal;    //for comparing prev and current values for dimming and brightening

#include "myfunctions.h"

//-------------------- Setup --------------------//

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(sliderPin, INPUT);

  Serial.begin(9600);

  AudioMemory(8);

  sgtl5000_1.enable();
  sgtl5000_1.volume(vol);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN)))
  {
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  delay(2000); //power up safety delay

  if (SCULPTURE_ID == 1)
  {
    FastLED.addLeds<LED_TYPE, LEDPIN0, COLOR_ORDER>(leds0, BAND1);
    FastLED.addLeds<LED_TYPE, LEDPIN1, COLOR_ORDER>(leds1, BAND2);
    FastLED.addLeds<LED_TYPE, LEDPIN2, COLOR_ORDER>(leds2, BAND3);
    FastLED.addLeds<LED_TYPE, LEDPIN3, COLOR_ORDER>(leds3, BAND4);
    FastLED.addLeds<LED_TYPE, LEDPIN4, COLOR_ORDER>(leds4, BAND5);
    FastLED.addLeds<LED_TYPE, LEDPIN5, COLOR_ORDER>(leds5, BAND6);
  }
  else if (SCULPTURE_ID == 2)   
  {
    FastLED.addLeds<LED_TYPE, LEDPIN0, COLOR_ORDER>(leds0, BAND1);
    FastLED.addLeds<LED_TYPE, LEDPIN1, COLOR_ORDER>(leds1, BAND2);
    FastLED.addLeds<LED_TYPE, LEDPIN2, COLOR_ORDER>(leds2, BAND3);
    FastLED.addLeds<LED_TYPE, LEDPIN3, COLOR_ORDER>(leds3, BAND4);
    FastLED.addLeds<LED_TYPE, LEDPIN4, COLOR_ORDER>(leds4, BAND5);
    FastLED.addLeds<LED_TYPE, LEDPIN5, COLOR_ORDER>(leds5, BAND6);
    FastLED.addLeds<LED_TYPE, LEDPIN6, COLOR_ORDER>(leds6, BAND7);
  }

  FastLED.setBrightness(255);

  delay(10);

  register_readings(); //translate the ann or sohsuang readings into a readings[] brightness value array.  
}

//-------------------- Loop --------------------//

void loop()
{
  read_console(); //listen to buttons and sliders

  check_playMode();

  if (playMode == IDLE_MODE)
  {
    if (SCULPTURE_ID == 1) sixband_fade_animation();
    else sevenband_fade_animation();
  }
  else if (playMode == BUTTON_MODE)
  {
    playback_readings(); //play brightness sequence according to readings[] array
  }
  else if (playMode == SLIDER_MODE)
  {
    toggle_readings(); //toggle according to the slider
  }

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);

  play_audio(); 
}