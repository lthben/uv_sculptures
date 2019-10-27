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

const int NUMDATA1 = 5, NUMDATA2 = 8; //number of data points for each sculpture
const float ann_readings[NUMDATA1] = {2.94, 4.27, 4.09, 0.42, 8.0};
// const float sohsuang_readings[NUMDATA2] = {0.13, 0.06, 1.6, 4.38, 7.0, 1.51, 3.8, 0.04};
const float sohsuang_readings[NUMDATA2] = {1.0, 7.0, 1.0, 7.0, 1.0, 7.0, 1.0, 7.0};

const int BAND1_1 = 4, BAND1_2 = 4, BAND1_3 = 4, BAND1_4 = 4, BAND1_5 = 4, BAND1_6 = 4; //Sculpture 1: num of pixels per band
const int BAND2_1 = 4, BAND2_2 = 4, BAND2_3 = 4, BAND2_4 = 4, BAND2_5 = 4, BAND2_6 = 4; //Sculpture 2: num of pixels per band

const int BAND_DELAY = 500;   //ms delay between each band lightup
const int SLIDER_WAIT = 3000; //ms idle for slider movement before IDLE_MODE kicks in

CHSV cyellow(64, 255, 255);
CHSV cpink(224, 255, 255);


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

const int buttonPin = 0;
const int sliderPin = A6;//A8 does not work for some strange reason. Causes sound to not be heard even when playing.
Bounce myButton = Bounce(buttonPin, 15); // 15 = 15 ms debounce time

unsigned int sliderVal, prevSliderVal, currSliderVal;
unsigned int sliderPosIndex, prevSliderPosIndex, currSliderPosIndex; //0 - 4 for Ann, 0 - 7 for Soh & Suang

bool isButtonPressed, isSliderToggled;

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
const char *idleTrack = "DRONE1.WAV"; const char *activeTrack = "RAYGUN.WAV";
// String idleTrack = "DRONE1.WAV"; String activeTrack = "RAYGUN.WAV";
#elif defined(__SCULPTURE2__)
const int NUM_LEDS = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6;
const int BAND1 = BAND2_1, BAND2 = BAND2_2, BAND3 = BAND2_3, BAND4 = BAND2_4, BAND5 = BAND2_5, BAND6 = BAND2_6;
const int BAND1L = BAND2_1, BAND2L = BAND2_1 + BAND2_2, BAND3L = BAND2_1 + BAND2_2 + BAND2_3, BAND4L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4, BAND5L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5, BAND6L = BAND2_1 + BAND2_2 + BAND2_3 + BAND2_4 + BAND2_5 + BAND2_6;
CHSV myColor = cpink;
const int SCULPTURE_ID = 2;
const char *idleTrack = "DRONE2.WAV"; const char *activeTrack = "TINKLING.WAV";
// String idleTrack = "DRONE2.WAV"; String activeTrack = "TINKLING.WAV";
#else
#error "invalid sculpture ID"
#endif

#define UPDATES_PER_SECOND 100 //speed of light animation

CRGB leds[NUM_LEDS];
int brightness1, brightness2, brightness3, brightness4, brightness5, brightness6; //band 1 to 6 brightness
int maxBrightLvl = 255;                                                           //variable max brightness
const int IDLE_MODE = 1, BUTTON_MODE = 2, SLIDER_MODE = 3;
unsigned int playMode = IDLE_MODE; //1 - idle mode; 2 - button mode; 3 - slider mode
bool hasplayModeChanged; 
int activeLedState = 0;            //SCULPTURE 1 : 0 - idle mode, start fade to black 1 - show brightness according to reading, 2 - has completed animations, fade to black and idle
bool isMaxBrightness = false;      //to track idle animation direction
elapsedMillis bandms;              //multiple use time ellapsed tracker
unsigned int band_delay = BAND_DELAY;
int readings1[NUMDATA1], readings2[NUMDATA2]; //light values translated from UV readings
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

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(255);

  delay(10);

  register_readings(); //translate the ann or sohsuang readings into a readings[] brightness value array.  
}

//-------------------- Loop --------------------//

void loop()
{
  read_console(); //listen to buttons and sliders

  if (isButtonPressed == true) //process button press
  {
    isButtonPressed = false; //listen again for button presses
    playMode = BUTTON_MODE;
    hasplayModeChanged = true;//trigger sound change
    Serial.println("BUTTON MODE");

    activeLedState = 0;          //reset the led if currently active
    band_delay = BAND_DELAY / 4; //speed up the fade animation
  }
  else if (isSliderToggled == true)
  {
    isSliderToggled = false; //listen again for slider movement
    playMode = SLIDER_MODE;
    hasplayModeChanged = true;//trigger sound change
    Serial.println("SLIDER MODE");

    band_delay = BAND_DELAY / 4; //speed up the fade animation
  }

  if (playMode == IDLE_MODE)
  {
    fade_animation();
  }
  else if (playMode == BUTTON_MODE)
  {
    playback_readings(); //play brightness sequence according to readings[] array
  }
  else if (playMode == SLIDER_MODE)
  {
    toggle_readings(); //toggle according to the slider
  }

  if (playMode == IDLE_MODE)
  {
    if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
    }
  }
  else if (playMode == BUTTON_MODE || playMode == SLIDER_MODE)
  {
    if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(activeTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(activeTrack);
    }
  }

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);

  if (playMode == IDLE_MODE)
  {
    if (hasplayModeChanged == true && playSdWav1.isPlaying() == true) 
    {
      playSdWav1.stop();
      playSdWav1.play(idleTrack);
      delay(10);
      // Serial.print("Start playing ");
      // Serial.println(idleTrack);
      hasplayModeChanged = false;
    } 
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(idleTrack);
      delay(10);
      // Serial.print("Start playing ");
      // Serial.println(idleTrack);
    }
  }
  else if ( playMode == BUTTON_MODE || playMode == SLIDER_MODE)
  {
    if (hasplayModeChanged == true && playSdWav1.isPlaying() == true) 
    {
      playSdWav1.stop();
      playSdWav1.play(activeTrack);
      delay(10);
      // Serial.print("Start playing ");
      // Serial.println(activeTrack);
      hasplayModeChanged = false;
    }
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(activeTrack);
      delay(10);
      // Serial.print("Start playing ");
      // Serial.println(activeTrack);
    }
  }
}