
/*--------------------------------------------------------------------------------
  Reads button and slider
--------------------------------------------------------------------------------*/

void read_console()
{
  myButton.update();

  if (myButton.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button pressed");
  }

  if (playMode != BUTTON_MODE) //don't interrupt the button mode with slider
  {
    currSliderVal = analogRead(sliderPin);
    // Serial.print("val: "); Serial.println(currSliderVal);
  }

  if (abs(currSliderVal - prevSliderVal) > 50) //to ignore noise
  {
    if (SCULPTURE_ID == 1)
    {
      currSliderPosIndex = int(map(currSliderVal, 0, 1023, 0, NUMDATA1 - 1));
    }
    else //sculpture 2
    {
      currSliderPosIndex = int(map(currSliderVal, 0, 1023, 0, NUMDATA2 - 1));
    }

    if (currSliderPosIndex != prevSliderPosIndex)
    {
      sliderPosIndex = currSliderPosIndex;
      prevSliderPosIndex = currSliderPosIndex;

      if (SCULPTURE_ID == 1)
      {
        maxBrightLvl = readings1[sliderPosIndex];
      }
      else //sculpture 2
      {
        maxBrightLvl = readings2[sliderPosIndex];
      }

      isSliderToggled = true;
      activeLedState = 0;

      Serial.print("currSliderVal: ");
      Serial.print(currSliderVal);
      Serial.print("\t currSliderPosIndex: ");
      Serial.println(currSliderPosIndex);
    }

    sliderVal = currSliderVal;
    prevSliderVal = currSliderVal;

    bandms = 0; //resets whenever the slider moves
  }
}

/*--------------------------------------------------------------------------------
  Tracks fade animation bright levels
--------------------------------------------------------------------------------*/
int get_brightness(int _brightness)
{
  if (!isMaxBrightness)
  {
    _brightness++;
    if (_brightness > maxBrightLvl)
      _brightness = maxBrightLvl;
    return _brightness;
  }
  else //reached max brightness
  {
    _brightness--;
    if (_brightness < 0)
      _brightness = 0;
    return _brightness;
  }
}

/*--------------------------------------------------------------------------------
  Fade in and out from inner to outer bands and vice versaa
--------------------------------------------------------------------------------*/
void fade_animation()
{
  if (!isMaxBrightness)
  {
    for (int i = 0; i < BAND1L; i++)
    {
      int brightlevel = get_brightness(brightness1);
      myColor.val = brightlevel;
      brightness1 = brightlevel;
      leds[i] = myColor;
    }
  }
  else
  {
    for (int i = BAND5L; i < BAND6L; i++)
    {
      int brightlevel = get_brightness(brightness6);
      myColor.val = brightlevel;
      brightness6 = brightlevel;
      leds[i] = myColor;
    }
  }

  if (bandms > band_delay)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND1L; i < BAND2L; i++)
      {
        int brightlevel = get_brightness(brightness2);
        myColor.val = brightlevel;
        brightness2 = brightlevel;
        leds[i] = myColor;
      }
    }
    else
    {
      for (int i = BAND4L; i < BAND5L; i++)
      {
        int brightlevel = get_brightness(brightness5);
        myColor.val = brightlevel;
        brightness5 = brightlevel;
        leds[i] = myColor;
      }
    }
  }

  if (bandms > band_delay * 2)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND2L; i < BAND3L; i++)
      {
        int brightlevel = get_brightness(brightness3);
        myColor.val = brightlevel;
        brightness3 = brightlevel;
        leds[i] = myColor;
      }
    }
    else
    {
      for (int i = BAND3L; i < BAND4L; i++)
      {
        int brightlevel = get_brightness(brightness4);
        myColor.val = brightlevel;
        brightness4 = brightlevel;
        leds[i] = myColor;
      }
    }
  }

  if (bandms > band_delay * 3)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND3L; i < BAND4L; i++)
      {
        int brightlevel = get_brightness(brightness4);
        myColor.val = brightlevel;
        brightness4 = brightlevel;
        leds[i] = myColor;
      }
    }
    else
    {
      for (int i = BAND2L; i < BAND3L; i++)
      {
        int brightlevel = get_brightness(brightness3);
        myColor.val = brightlevel;
        brightness3 = brightlevel;
        leds[i] = myColor;
      }
    }
  }

  if (bandms > band_delay * 4)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND4L; i < BAND5L; i++)
      {
        int brightlevel = get_brightness(brightness5);
        myColor.val = brightlevel;
        brightness5 = brightlevel;
        leds[i] = myColor;
      }
    }
    else
    {
      for (int i = BAND1L; i < BAND2L; i++)
      {
        int brightlevel = get_brightness(brightness2);
        myColor.val = brightlevel;
        brightness2 = brightlevel;
        leds[i] = myColor;
      }
    }
  }

  if (bandms > band_delay * 5)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND5L; i < BAND6L; i++)
      {
        int brightlevel = get_brightness(brightness6);
        myColor.val = brightlevel;
        brightness6 = brightlevel;
        leds[i] = myColor;
        if (brightlevel == maxBrightLvl)
        {
          isMaxBrightness = true;
          bandms = 0;
        }
      }
    }
    else
    {
      for (int i = 0; i < BAND1L; i++)
      {
        int brightlevel = get_brightness(brightness1);
        myColor.val = brightlevel;
        brightness1 = brightlevel;
        leds[i] = myColor;
        if (brightlevel == 0)
        {
          isMaxBrightness = false;
          bandms = 0;
        }
      }
    }
  }
}

/*--------------------------------------------------------------------------------
  Run once during setup to register brightness readings
--------------------------------------------------------------------------------*/
void register_readings()
{ //get the readings once, translate data to a light brightness value
  if (SCULPTURE_ID == 1)
  {
    for (int i = 0; i < NUMDATA1; i++)
    {
      readings1[i] = int(map(ann_readings[i], 0.0, 8.0, 31.0, 255.0));
    }
  }
  else if (SCULPTURE_ID == 2)
  {
    for (int i = 0; i < NUMDATA2; i++)
    {
      readings2[i] = int(map(sohsuang_readings[i], 0.0, 7.0, 31.0, 255.0));
    }
  }
}

/*--------------------------------------------------------------------------------
  Prep to go to idle mode
--------------------------------------------------------------------------------*/
void go_idle()
{
  activeLedState = 0; //go to idle state
  playMode = IDLE_MODE;
  hasplayModeChanged = true;//trigger sound change
  band_delay = BAND_DELAY;
  maxBrightLvl = 255;
  Serial.println("IDLE MODE");
  isMaxBrightness = false;
  brightness1 = brightness2 = brightness3 = brightness4 = brightness5 = brightness6 = 0;
  bandms = 0;
  sgtl5000_1.volume(0.5);
}

/*--------------------------------------------------------------------------------
  For button mode
--------------------------------------------------------------------------------*/
void playback_readings() //light sequence playback according to readings[] array
{
  if (activeLedState == 0) //dim the lights
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i].fadeToBlackBy(8); //dim by (x/256)% till eventually to black
    }
    if (leds[0].getAverageLight() == 0 && leds[NUM_LEDS - 1].getAverageLight() == 0 && leds[int(NUM_LEDS / 2)].getAverageLight() == 0)
    {
      activeLedState = 1; //go to next state
      bandms = 0;
      readingsCounter = 0;
      currBrightVal = prevBrightVal = 0;
      myColor.val = 0;
    }
  }
  else if (activeLedState == 1) //finished dimming, show the reading
  {
    if (bandms < BAND_DELAY * 2) //control the speed of the fade animation here 
    {
      if (SCULPTURE_ID == 1)
      {
        currBrightVal = readings1[readingsCounter];
      }
      else //sculpture 2
      {
        currBrightVal = readings2[readingsCounter];
      }

      if (currBrightVal > prevBrightVal)
      {
        if (myColor.val < currBrightVal)
        {
          myColor.val += 2; //brighten
        }
      }
      else //curr is < prev bright val
      {
        if (myColor.val > currBrightVal)
        {
          myColor.val -= 2; //dim
        }
      }

      //change vol here
      vol = map(float(myColor.val), 0.0, 255.0, 0.3, 0.7);
      sgtl5000_1.volume(vol);
      // Serial.print("vol: "); Serial.println(vol);

      //change led brightness here
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = myColor;
      }
    }
    else //go to next bright value
    {
      prevBrightVal = currBrightVal;
      readingsCounter++; 

      Serial.print("readingsCounter: ");
      Serial.print(readingsCounter);
      Serial.print("\t currBrightVal: ");
      Serial.println(currBrightVal);

      bandms = 0; //need to reset here

      if ((SCULPTURE_ID == 1 && readingsCounter == NUMDATA1) || (SCULPTURE_ID == 2 && readingsCounter == NUMDATA2))
      {
        activeLedState = 2; //go to next state
      }
    }
  }
  else if (activeLedState == 2)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i].fadeToBlackBy(8); //dim by (x/256)% till eventually to black
    }
    if (leds[0].getAverageLight() == 0 && leds[NUM_LEDS - 1].getAverageLight() == 0 && leds[int(NUM_LEDS / 2)].getAverageLight() == 0)
    {
      go_idle();
    }
  }
}

/*--------------------------------------------------------------------------------
  For slider mode
--------------------------------------------------------------------------------*/
void toggle_readings()
{
  if (activeLedState == 0)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      myColor.val = maxBrightLvl;
      leds[i] = myColor;
    }

    //change vol here
    vol = map(float(myColor.val), 0.0, 255.0, 0.3, 0.7);
    sgtl5000_1.volume(vol);
    // Serial.print("vol: "); Serial.println(vol);

    if (bandms > SLIDER_WAIT)
    {
      activeLedState = 1; //start fade before idling
    }
  }
  else if (activeLedState == 1)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i].fadeToBlackBy(8); //dim by (x/256)% till eventually to black
    }
    if (leds[0].getAverageLight() == 0 && leds[NUM_LEDS - 1].getAverageLight() == 0 && leds[int(NUM_LEDS / 2)].getAverageLight() == 0)
    {
      activeLedState = 2; //go to next state
    }
  }
  else if (activeLedState == 2)
  {
    go_idle();
  }
}

/* --------------------------------------------------------------------------------
//This animation is no longer used
void play_ann_readings()
{
  if (activeLedState == 0) //dim the lights
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i].fadeToBlackBy(8); //dim by (x/256)% till eventually to black
    }
    if (leds[0].getAverageLight() == 0)
    {
      activeLedState = 1; //go to next state
      brightness1 = brightness2 = brightness3 = brightness4 = brightness5 = brightness6 = 0;
      isMaxBrightness = false;
      bandms = 0;
    }
  }
  else if (activeLedState == 1) //finished dimming, show the reading
  {
    //show reading
    fade_animation();
    //check whether max bright reached
    if (isMaxBrightness == true)
    {
      activeLedState = 2;
      bandms = 0;
    }
  }
  else if (activeLedState == 2) //has reached max brightness, hold for a few sec
  {
    if (bandms > HOLD_DURATION)
    {
      activeLedState = 3;
      bandms = 0;
    }
  }
  else if (activeLedState == 3) //has held for a few sec
  {
    //dim
    fade_animation();
    //check whether dim finished
    if (isMaxBrightness == false)
    {
      playMode = IDLE_MODE;
      maxBrightLvl = 255;
      activeLedState = 0;
      bandms = 0;
      band_delay = BAND_DELAY;
    }
  }
}
*/