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
    isButtonPressed = true;
    Serial.println("button0 pressed");
  }

  if (button1.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button1 pressed");
  }

  if (button2.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button2 pressed");
  }

  if (button3.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button3 pressed");
  }

  if (button4.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button4 pressed");
  }

  if (button5.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button5 pressed");
  }
}

int get_brightness(int _brightness)
{
  if (!isMaxBrightness)
  {
    _brightness++;
    if (_brightness > 255)
      _brightness = 255;
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

void idle_animation()
{
  if (!isMaxBrightness)
  {
    for (int i = 0; i < BAND1L; i++)
    {
      int brightlevel = get_brightness(brightness1);
      cyellow.val = brightlevel;
      brightness1 = brightlevel;
      leds[i] = cyellow;
    }
  }
  else
  {
    for (int i = BAND5L; i < BAND6L; i++)
    {
      int brightlevel = get_brightness(brightness5);
      cyellow.val = brightlevel;
      brightness5 = brightlevel;
      leds[i] = cyellow;
    }
  }

  if (bandms > BAND_DELAY)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND1L; i < BAND2L; i++)
      {
        int brightlevel = get_brightness(brightness2);
        cyellow.val = brightlevel;
        brightness2 = brightlevel;
        leds[i] = cyellow;
      }
    }
    else
    {
      for (int i = BAND4L; i < BAND5L; i++)
      {
        int brightlevel = get_brightness(brightness4);
        cyellow.val = brightlevel;
        brightness4 = brightlevel;
        leds[i] = cyellow;
      }
    }
  }

  if (bandms > BAND_DELAY * 2)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND2L; i < BAND3L; i++)
      {
        int brightlevel = get_brightness(brightness3);
        cyellow.val = brightlevel;
        brightness3 = brightlevel;
        leds[i] = cyellow;
      }
    }
    else
    {
      for (int i = BAND3L; i < BAND4L; i++)
      {
        int brightlevel = get_brightness(brightness3);
        cyellow.val = brightlevel;
        brightness3 = brightlevel;
        leds[i] = cyellow;
      }
    }
  }

  if (bandms > BAND_DELAY * 3)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND3L; i < BAND4L; i++)
      {
        int brightlevel = get_brightness(brightness3);
        cyellow.val = brightlevel;
        brightness3 = brightlevel;
        leds[i] = cyellow;
      }
    }
    else
    {
      for (int i = BAND2L; i < BAND3L; i++)
      {
        int brightlevel = get_brightness(brightness3);
        cyellow.val = brightlevel;
        brightness3 = brightlevel;
        leds[i] = cyellow;
      }
    }
  }

  if (bandms > BAND_DELAY * 4)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND4L; i < BAND5L; i++)
      {
        int brightlevel = get_brightness(brightness4);
        cyellow.val = brightlevel;
        brightness4 = brightlevel;
        leds[i] = cyellow;
      }
    }
    else
    {
      for (int i = BAND1L; i < BAND2L; i++)
      {
        int brightlevel = get_brightness(brightness2);
        cyellow.val = brightlevel;
        brightness2 = brightlevel;
        leds[i] = cyellow;
      }
    }
  }

  if (bandms > BAND_DELAY * 5)
  {
    if (!isMaxBrightness)
    {
      for (int i = BAND5L; i < BAND6L; i++)
      {
        int brightlevel = get_brightness(brightness5);
        cyellow.val = brightlevel;
        brightness5 = brightlevel;
        leds[i] = cyellow;
        if (brightlevel == 255)
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
        cyellow.val = brightlevel;
        brightness1 = brightlevel;
        leds[i] = cyellow;
        if (brightlevel == 0)
        {
          isMaxBrightness = false;
          bandms = 0;
        }
      }
    }
  }
}