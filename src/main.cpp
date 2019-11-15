/*
  Author: Benjamin Low (Lthben@gmail.com)
  Date: Nov 2019
  Description: UV Sculpture 2 - Soh / Suang

      Teensy 3.2 with audio shield. 
      Each sculpture has one button and one slider each.
      The button is playback mode for the light readings. The slider is toggle mode for the different readings. 
      Both have bands of neon flex leds to show the UV levels in the form of light. 
      There are sounds associated with all light animations, including a pulsing idle animation. 
      
      SCULPTURE1 (curved light strips) is Ann, SCULPTURE2 (straight light strips) is Soh and Suang Suang
      SCULPTURE 1 has 10 led strips. SCULPTURE 2 has 9.
      SCULPTURE 1 uses a slider. 2 uses a knob  
*/

#include <Arduino.h>
#include <FastLED.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

//-------------------- PIN ASSIGNMENT --------------------//
/* Mapping for 11 data pins. Pins 9, 11, 12, 15 cannot be used as sound will not play. 
  TBD: can 13, 16, 17, 18, 21 to 23 be used? -> 13 cannot light up led
  HACK: LEDPIN9 is coupled to LEDPIN8. Animation is slow enough not to notice that LEDPIN8  and LEDPIN9 are actually have the same brightness
  7, 10 and 14 used by SD card
  19 and 20 used by pot and button
*/
const int LEDPIN0 = 0, LEDPIN1 = 1, LEDPIN2 = 2, LEDPIN3 = 3, LEDPIN4 = 4, LEDPIN5 = 5, LEDPIN6 = 6, LEDPIN7 = 8, LEDPIN8 = 13, LEDPIN9 = 16, LEDPIN10 = 17; 

//-------------------- USER DEFINED SETTINGS --------------------//

/* CHECK IF CHECK_CONSOLE() IS COMMENTED OUT!!!!!!!!!
*/

// #define __SCULPTURE1__  //Ann
#define __SCULPTURE2__  //Soh & Suang

const int MAXBRIGHTLVL = 160; //conserve the leds

//band 1 is inner most centre where the idle fade animation starts from
//number of pixels (every 10cm) for each data pin controlled led strip
//number system is NX_BX_SX where NX is SCULPTURE ID number, BX is band number and SX is the individual strip number within the band
const int p1_1_x = 3, p1_2_x = 6, p1_3_x = 10, p1_4_1 = 14, p1_4_2 = 14, p1_5_1 = 18, p1_5_2 = 18, p1_6_1 = 22, p1_6_2 = 22, p1_6_3 = 23; //SCULPTURE1. 6 bands. 10 strips.
const int p2_1_1 = 9, p2_1_2 = 9, p2_1_3 = 9, p2_2_x = 20, p2_3_x = 22, p2_4_x = 11, p2_5_x = 11, p2_6_x = 11, p2_7_x = 11;//Sculpture 2. 7 bands. 9 strips.

const int NUMDATA1 = 5, NUMDATA2 = 8; //number of data points for each sculpture
const float ann_readings[NUMDATA1] = {2.94, 4.27, 4.09, 0.42, 8.0}; //actual values
// const float ann_readings[NUMDATA1] = {1.0, 8.0, 1.0, 8.0, 1.0}; //test values

const float sohsuang_readings[NUMDATA2] = {0.13, 0.06, 1.6, 4.38, 7.0, 1.51, 3.8, 0.04}; //actual values
// const float sohsuang_readings[NUMDATA2] = {1.0, 7.0, 1.0, 7.0, 1.0, 7.0, 1.0, 7.0}; //test values

const int BAND_DELAY = 500;   //ms delay between each band lightup
const int SLIDER_WAIT = 3000; //ms idle for slider movement before IDLE_MODE kicks in

CHSV cyellow(64, 255, 255); //sculpture 1 idle mode colour
CHSV cpink(224, 255, 255); //sculpture 2 idle mode colour
CHSV corange(32, 255, 255);
CHSV cred(0, 255, 255);

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

//-------------------- Buttons and Sliders --------------------//

const int buttonPin = 19;
const int sliderPin = A6;//A6 is 20. A8 or 22 does not work for some strange reason. Causes sound to not be heard even when playing.
Bounce myButton = Bounce(buttonPin, 15); // 15 = 15 ms debounce time

unsigned int sliderVal, prevSliderVal, currSliderVal;
unsigned int sliderPosIndex, prevSliderPosIndex, currSliderPosIndex; //0 - 4 for Ann, 0 - 7 for Soh & Suang

bool isButtonPressed, isSliderToggled;

//-------------------- Light --------------------//

#define LED_TYPE WS2812 //Not UCS1903
#define COLOR_ORDER GRB //Yes! GRB!

#if defined(__SCULPTURE1__)
CHSV myIdleColor = cyellow;
const int SCULPTURE_ID = 1;
const char *idleTrack = "DRONE1.WAV"; const char *activeTrack = "RAYGUN.WAV";
CRGB leds0[p1_1_x], leds1[p1_2_x], leds2[p1_3_x], leds3[p1_4_1], leds4[p1_4_2], leds5[p1_5_1], leds6[p1_5_2], leds7[p1_6_1], leds8[p1_6_2], leds9[p1_6_3];
const int n1 = p1_1_x, n2 = p1_2_x, n3 = p1_3_x, n4 = p1_4_1, n5 = p1_4_2, n6 = p1_5_1, n7 = p1_5_2, n8 = p1_6_1, n9 = p1_6_2, n10 = p1_6_3; //for common code for both sculptures
float vol = 0.6; //hard to access the vol knob on the amp, so adjust in software

#elif defined(__SCULPTURE2__)
CHSV myIdleColor = cpink;
const int SCULPTURE_ID = 2;
const char *idleTrack = "DRONE2.WAV"; const char *activeTrack = "TINKLING.WAV";
CRGB leds0[p2_1_1], leds1[p2_1_2], leds2[p2_1_3], leds3[p2_2_x], leds4[p2_3_x], leds5[p2_4_x], leds6[p2_5_x], leds7[p2_6_x], leds8[p2_7_x], leds9[0]; //need to define "leds9[]" else cannot compile
const int n1 = p2_1_1, n2 = p2_1_2, n3 = p2_1_3, n4 = p2_2_x, n5 = p2_3_x, n6 = p2_4_x, n7 = p2_5_x, n8 = p2_6_x, n9 = p2_7_x, n10 = 0; //for common code for both sculptures
float vol = 0.7; //master volume gain 0.0 - 1.0

#else
#error "invalid sculpture ID"
#endif

#define UPDATES_PER_SECOND 100 //speed of light animation

int brightness1, brightness2, brightness3, brightness4, brightness5, brightness6, brightness7; //band 1 to 7 brightness
int maxBrightLvl = MAXBRIGHTLVL;                                                           //variable max brightness
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
CHSV activeColor1 = cyellow; CHSV activeColor2 = corange; CHSV activeColor3 = cred;

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
    FastLED.addLeds<LED_TYPE, LEDPIN0, COLOR_ORDER>(leds0, p1_1_x);
    FastLED.addLeds<LED_TYPE, LEDPIN1, COLOR_ORDER>(leds1, p1_2_x);
    FastLED.addLeds<LED_TYPE, LEDPIN2, COLOR_ORDER>(leds2, p1_3_x);
    FastLED.addLeds<LED_TYPE, LEDPIN3, COLOR_ORDER>(leds3, p1_4_1);
    FastLED.addLeds<LED_TYPE, LEDPIN4, COLOR_ORDER>(leds4, p1_4_2);
    FastLED.addLeds<LED_TYPE, LEDPIN5, COLOR_ORDER>(leds5, p1_5_1);
    FastLED.addLeds<LED_TYPE, LEDPIN6, COLOR_ORDER>(leds6, p1_5_2);
    FastLED.addLeds<LED_TYPE, LEDPIN7, COLOR_ORDER>(leds7, p1_6_1);
    FastLED.addLeds<LED_TYPE, LEDPIN8, COLOR_ORDER>(leds8, p1_6_2);
    FastLED.addLeds<LED_TYPE, LEDPIN9, COLOR_ORDER>(leds9, p1_6_3);
  }
  else if (SCULPTURE_ID == 2)   
  {
    FastLED.addLeds<LED_TYPE, LEDPIN0, COLOR_ORDER>(leds0, p2_1_1);
    FastLED.addLeds<LED_TYPE, LEDPIN1, COLOR_ORDER>(leds1, p2_1_2);
    FastLED.addLeds<LED_TYPE, LEDPIN2, COLOR_ORDER>(leds2, p2_1_3);
    FastLED.addLeds<LED_TYPE, LEDPIN3, COLOR_ORDER>(leds3, p2_2_x);
    FastLED.addLeds<LED_TYPE, LEDPIN4, COLOR_ORDER>(leds4, p2_3_x);
    FastLED.addLeds<LED_TYPE, LEDPIN5, COLOR_ORDER>(leds5, p2_4_x);
    FastLED.addLeds<LED_TYPE, LEDPIN6, COLOR_ORDER>(leds6, p2_5_x);
    FastLED.addLeds<LED_TYPE, LEDPIN7, COLOR_ORDER>(leds7, p2_6_x);
    FastLED.addLeds<LED_TYPE, LEDPIN8, COLOR_ORDER>(leds8, p2_7_x);
    FastLED.addLeds<LED_TYPE, LEDPIN8, COLOR_ORDER>(leds9, 0); //need to define, else cannot compile
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
