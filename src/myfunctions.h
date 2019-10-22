void read_pushbuttons()
{
  button0.update();
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();

  if (button0.fallingEdge())
  {
    isButtonPressed = true; buttonNum = 0;
    Serial.println("button0 pressed");
  }

  if (button1.fallingEdge())
  {
    isButtonPressed = true; buttonNum = 1;
    Serial.println("button1 pressed");
  }

  if (button2.fallingEdge())
  {
    isButtonPressed = true; buttonNum = 2;
    Serial.println("button2 pressed");
  }

  if (button3.fallingEdge())
  {
    isButtonPressed = true; buttonNum = 3;
    Serial.println("button3 pressed");
  }

  if (button4.fallingEdge())
  {
    isButtonPressed = true; buttonNum = 4;
    Serial.println("button4 pressed");
  }

  if (button5.fallingEdge())
  {
    isButtonPressed = true; buttonNum = 5;
    Serial.println("button5 pressed");
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