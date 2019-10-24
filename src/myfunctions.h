void read_console()
{
  myButton.update();

  if (myButton.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button pressed");
  }

  currSliderVal = analogRead(sliderPin);
  if (currSliderVal != prevSliderVal)
  {
    isSliderMoved = true;
    sliderVal = currSliderVal;
    prevSliderVal = currSliderVal;
    bandms = 0;
  }
  else if (bandms > SLIDER_WAIT)
  {
    playMode = IDLE_MODE;
  }
}

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

void play_sohsuang_readings()
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
      currVal = readings[readingsCounter];

      if (currVal > prevVal)
      {
        if (myColor.val < currVal)
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
        if (myColor.val > currVal)
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
      prevVal = currVal;
      readingsCounter++;

      if (readingsCounter == 20)
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
      activeLedState = 0; //go to next state
      bandms = 0;
      playMode = IDLE_MODE;
    }
  }
}

void register_readings()
{
  if (SCULPTURE_ID == 1)
  {
    for (int i = 0; i < NUMDATA1; i++)
    {
      readings[i] = int(map(ann_readings[i], 0.0, 8.0, 31.0, 255.0));
    }
  }
  else if (SCULPTURE_ID == 2) //get the readings once
  {
    for (int i = 0; i < NUMDATA2; i++)
    {
      readings[i] = int(map(sohsuang_readings[i], 0.0, 8.0, 31.0, 255.0));
    }
  }
}
