/*
  Reads button and slider
*/
void read_console()
{
  myButton.update();

  if (myButton.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button pressed");
  }

  currSliderVal = analogRead(sliderPin);

  if (abs(currSliderVal - prevSliderVal) > 20) //some noise
  {
    if (SCULPTURE_ID == 1)
    {
      currSliderPosIndex = int(map(currSliderVal, 0, 1023, 0, NUMDATA1 - 1));
      Serial.print("currSliderVal: ");
      Serial.print(currSliderVal);
      Serial.print("\t currSliderPosIndex: ");
      Serial.println(currSliderPosIndex);
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
      else
      {
        maxBrightLvl = readings2[sliderPosIndex];
      }

      isSliderToggled = true;
    }

    sliderVal = currSliderVal;
    prevSliderVal = currSliderVal;

    bandms = 0; //resets whenever the slider moves
  }
}

/*
  Tracks fade bright levels
*/
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

/*
  Fade in and out from inner to outer bands and vice versaa
*/
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
      int brightlevel = get_brightness(brightness5);
      myColor.val = brightlevel;
      brightness5 = brightlevel;
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
        int brightlevel = get_brightness(brightness4);
        myColor.val = brightlevel;
        brightness4 = brightlevel;
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
        int brightlevel = get_brightness(brightness3);
        myColor.val = brightlevel;
        brightness3 = brightlevel;
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
        int brightlevel = get_brightness(brightness3);
        myColor.val = brightlevel;
        brightness3 = brightlevel;
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
        int brightlevel = get_brightness(brightness4);
        myColor.val = brightlevel;
        brightness4 = brightlevel;
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
        int brightlevel = get_brightness(brightness5);
        myColor.val = brightlevel;
        brightness5 = brightlevel;
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

/*
  Run once during setup to register readings
*/
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

/*
  For button mode
*/
void playback_readings() //light sequence playback according to readings[] array
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
      bandms = 0;
      readingsCounter = 0;
    }
  }
  else if (activeLedState == 1) //finished dimming, show the reading
  {
    if (bandms < BAND_DELAY)
    {
      if (SCULPTURE_ID == 1)
      {
        currBrightVal = readings1[readingsCounter];
      }
      else
      {
        currBrightVal = readings2[readingsCounter];
      }
      if (currBrightVal > prevBrightVal)
      {
        if (myColor.val < currBrightVal)
        {
          myColor.val += 4; //brighten
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = myColor;
        }
      }
      else
      {
        if (myColor.val > currBrightVal)
        {
          myColor.val -= 4; //dim
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = myColor;
        }
      }
    }
    else
    {
      prevBrightVal = currBrightVal;
      readingsCounter++;

      if ((SCULPTURE_ID == 1 && readingsCounter == NUMDATA1) || (SCULPTURE_ID == 2 && readingsCounter == NUMDATA2))
      {
        activeLedState = 2; //go to next state
        bandms = 0;
      }
      bandms = 0;
    }
  }
  else if (activeLedState == 2)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i].fadeToBlackBy(8); //dim by (x/256)% till eventually to black
    }
    if (leds[0].getAverageLight() == 0)
    {
      activeLedState = 0; //go to idle state
      playMode = IDLE_MODE;
      band_delay = BAND_DELAY;
      maxBrightLvl = 255;
      Serial.println("IDLE MODE");
      bandms = 0;
    }
  }
}

/*
  For slider mode
*/
void toggle_readings()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    myColor.val = maxBrightLvl;
    leds[i] = myColor;
  }

  if (bandms > SLIDER_WAIT)
  {
    playMode = IDLE_MODE;
    maxBrightLvl = 255;
    band_delay = BAND_DELAY;
    Serial.println("IDLE MODE");
  }
}

/* 
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