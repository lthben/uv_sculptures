
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
      if (currSliderVal >= 180 && currSliderVal < 300) currSliderPosIndex = 0;
      else if (currSliderVal >= 300 && currSliderVal < 520) currSliderPosIndex = 1;
      else if (currSliderVal >= 520 && currSliderVal < 720) currSliderPosIndex = 2;
      else if (currSliderVal >= 720 && currSliderVal < 920) currSliderPosIndex = 3;
      else if (currSliderVal >= 920 && currSliderVal < 1024) currSliderPosIndex = 4;  
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
  Fade all leds
--------------------------------------------------------------------------------*/
void fade_all()
{
  for (int i = 0; i < n1; i++)
  {
    leds0[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n2; i++)
  {
    leds1[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n3; i++)
  {
    leds2[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n4; i++)
  {
    leds3[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n5; i++)
  {
    leds4[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n6; i++)
  {
    leds5[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n7; i++)
  {
    leds6[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n8; i++)
  {
    leds7[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n9; i++)
  {
    leds8[i].fadeToBlackBy(8);
  }
  for (int i = 0; i < n10; i++)
  {
    leds9[i].fadeToBlackBy(8);
  }
}

/*--------------------------------------------------------------------------------
  Set all leds to a certain brightness
--------------------------------------------------------------------------------*/
void set_all_to_brightLevel(int brightLvl)
{
  if (playMode == IDLE_MODE)
  {
    myIdleColor.val = brightLvl;

    for (int i = 0; i < n1; i++)
    {
      leds0[i] = myIdleColor;
    }
    for (int i = 0; i < n2; i++)
    {
      leds1[i] = myIdleColor;
    }
    for (int i = 0; i < n3; i++)
    {
      leds2[i] = myIdleColor;
    }
    for (int i = 0; i < n4; i++)
    {
      leds3[i] = myIdleColor;
    }
    for (int i = 0; i < n5; i++)
    {
      leds4[i] = myIdleColor;
    }
    for (int i = 0; i < n6; i++)
    {
      leds5[i] = myIdleColor;
    }
    for (int i = 0; i < n7; i++)
    {
      leds6[i] = myIdleColor;
    }
    for (int i = 0; i < n8; i++)
    {
      leds7[i] = myIdleColor;
    }
    for (int i = 0; i < n9; i++)
    {
      leds8[i] = myIdleColor;
    }
    for (int i = 0; i < n10; i++)
    {
      leds9[i] = myIdleColor;
    }
  }
  else //other playmodes use active colors
  {
    activeColor1.val = activeColor2.val = activeColor3.val = brightLvl;

    for (int i = 0; i < n1; i++)
    {
      leds0[i] = activeColor1;
    }
    for (int i = 0; i < n2; i++)
    {
      leds1[i] = activeColor1;
    }
    for (int i = 0; i < n3; i++)
    {
      leds2[i] = activeColor1;
    }
    for (int i = 0; i < n4; i++)
    {
      leds3[i] = activeColor2;
    }
    for (int i = 0; i < n5; i++)
    {
      leds4[i] = activeColor2;
    }
    for (int i = 0; i < n6; i++)
    {
      leds5[i] = activeColor2;
    }
    for (int i = 0; i < n7; i++)
    {
      leds6[i] = activeColor2;
    }
    for (int i = 0; i < n8; i++)
    {
      leds7[i] = activeColor3;
    }
    for (int i = 0; i < n9; i++)
    {
      leds8[i] = activeColor3;
    }
    for (int i = 0; i < n10; i++)
    {
      leds9[i] = activeColor3;
    }
  }
}

/*--------------------------------------------------------------------------------
  Checks to see if all the leds have faded out
--------------------------------------------------------------------------------*/
bool has_all_fade()
{
  if (leds0[0].getAverageLight() == 0)
  {
    if (leds2[0].getAverageLight() == 0)
    {
      if (leds4[0].getAverageLight() == 0)
      {
        if (leds6[0].getAverageLight() == 0)
        {
          if (leds8[0].getAverageLight() == 0)
          {
            return true;
          }
          return false;
        }
        return false;
      }
      return false;
    }
    return false;
  }
  return false;
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
  Sculpture 1. Fade in and out from inner to outer bands and vice versa.
--------------------------------------------------------------------------------*/
void sixband_fade_animation()
{
  if (!isMaxBrightness)
  {
    int brightlevel = get_brightness(brightness1);
    myIdleColor.val = brightness1 = brightlevel;
    for (int i = 0; i < n1; i++)
    {
      leds0[i] = myIdleColor;
    }
  }
  else
  {
    int brightlevel = get_brightness(brightness6);
    myIdleColor.val = brightness6 = brightlevel;
    for (int i = 0; i < n8; i++)
    {
      leds7[i] = myIdleColor;
    }
    for (int i = 0; i < n9; i++)
    {
      leds8[i] = myIdleColor;
    }
    for (int i = 0; i < n10; i++)
    {
      leds9[i] = myIdleColor;
    }
  }

  if (bandms > band_delay)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness2);
      myIdleColor.val = brightness2 = brightlevel;
      for (int i = 0; i < n2; i++)
      {
        leds1[i] = myIdleColor;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness5);
      myIdleColor.val = brightness5 = brightlevel;
      for (int i = 0; i < n6; i++)
      {
        leds5[i] = myIdleColor;
      }
      for (int i = 0; i < n7; i++)
      {
        leds6[i] = myIdleColor;
      }
    }
  }

  if (bandms > band_delay * 2)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness3);
      myIdleColor.val = brightness3 = brightlevel;
      for (int i = 0; i < n3; i++)
      {
        leds2[i] = myIdleColor;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness4);
      myIdleColor.val = brightness4 = brightlevel;
      for (int i = 0; i < n4; i++)
      {
        leds3[i] = myIdleColor;
      }
      for (int i = 0; i < n5; i++)
      {
        leds4[i] = myIdleColor;
      }
    }
  }

  if (bandms > band_delay * 3)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness4);
      myIdleColor.val = brightness4 = brightlevel;
      for (int i = 0; i < n4; i++)
      {
        leds3[i] = myIdleColor;
      }
      for (int i = 0; i < n5; i++)
      {
        leds4[i] = myIdleColor;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness3);
      myIdleColor.val = brightness3 = brightlevel;
      for (int i = 0; i < n3; i++)
      {
        leds2[i] = myIdleColor;
      }
    }
  }

  if (bandms > band_delay * 4)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness5);
      myIdleColor.val = brightness5 = brightlevel;
      for (int i = 0; i < n6; i++)
      {
        leds5[i] = myIdleColor;
      }
      for (int i = 0; i < n7; i++)
      {
        leds6[i] = myIdleColor;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness2);
      myIdleColor.val = brightness2 = brightlevel;
      for (int i = 0; i < n2; i++)
      {
        leds1[i] = myIdleColor;
      }
    }
  }

  if (bandms > band_delay * 5)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness6);
      myIdleColor.val = brightness6 = brightlevel;
      for (int i = 0; i < n8; i++)
      {
        leds7[i] = myIdleColor;
      }
      for (int i = 0; i < n9; i++)
      {
        leds8[i] = myIdleColor;
      }
      for (int i = 0; i < n10; i++)
      {
        leds9[i] = myIdleColor;
      }
      if (brightlevel == maxBrightLvl)
      {
        isMaxBrightness = true;
        bandms = 0;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness1);
      myIdleColor.val = brightness1 = brightlevel;
      for (int i = 0; i < n1; i++)
      {
        leds0[i] = myIdleColor;
      }
      if (brightlevel == 0)
      {
        isMaxBrightness = false;
        bandms = 0;
      }
    }
  }
}

/*--------------------------------------------------------------------------------
  Sculpture 2. Fade in and out from inner to outer bands and vice versa.
--------------------------------------------------------------------------------*/
void sevenband_fade_animation()
{
  if (!isMaxBrightness)
  {
    int brightlevel = get_brightness(brightness1);
    myIdleColor.val = brightness1 = brightlevel;
    for (int i = 0; i < n1; i++)
    {
      leds0[i] = myIdleColor;
    }
    for (int i = 0; i < n2; i++)
    {
      leds1[i] = myIdleColor;
    }
    for (int i = 0; i < n3; i++)
    {
      leds2[i] = myIdleColor;
    }
  }
  else
  {
    int brightlevel = get_brightness(brightness7);
    myIdleColor.val = brightness7 = brightlevel;
    for (int i = 0; i < n9; i++)
    {
      leds8[i] = myIdleColor;
    }
  }

  if (bandms > band_delay)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness2);
      myIdleColor.val = brightness2 = brightlevel;
      for (int i = 0; i < n4; i++)
      {
        leds3[i] = myIdleColor;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness6);
      myIdleColor.val = brightness6 = brightlevel;
      for (int i = 0; i < n8; i++)
      {
        leds7[i] = myIdleColor;
      }
    }
  }

  if (bandms > band_delay * 2)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness3);
      myIdleColor.val = brightness3 = brightlevel;
      for (int i = 0; i < n5; i++)
      {
        leds4[i] = myIdleColor;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness5);
      myIdleColor.val = brightness5 = brightlevel;
      for (int i = 0; i < n7; i++)
      {
        leds6[i] = myIdleColor;
      }
    }
  }

  if (bandms > band_delay * 3)
  { //both directions are band 4
    int brightlevel = get_brightness(brightness4);
    myIdleColor.val = brightness4 = brightlevel;
    for (int i = 0; i < n6; i++)
    {
      leds5[i] = myIdleColor;
    }
  }

  if (bandms > band_delay * 4)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness5);
      myIdleColor.val = brightness5 = brightlevel;
      for (int i = 0; i < n7; i++)
      {
        leds6[i] = myIdleColor;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness3);
      myIdleColor.val = brightness3 = brightlevel;
      for (int i = 0; i < n5; i++)
      {
        leds4[i] = myIdleColor;
      }
    }
  }

  if (bandms > band_delay * 5)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness6);
      myIdleColor.val = brightness6 = brightlevel;
      for (int i = 0; i < n8; i++)
      {
        leds7[i] = myIdleColor;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness2);
      myIdleColor.val = brightness2 = brightlevel;
      for (int i = 0; i < n4; i++)
      {
        leds3[i] = myIdleColor;
      }
    }
  }

  if (bandms > band_delay * 6)
  {
    if (!isMaxBrightness)
    {
      int brightlevel = get_brightness(brightness7);
      myIdleColor.val = brightness7 = brightlevel;
      for (int i = 0; i < n9; i++)
      {
        leds8[i] = myIdleColor;
      }
      if (brightlevel == maxBrightLvl)
      {
        isMaxBrightness = true;
        bandms = 0;
      }
    }
    else
    {
      int brightlevel = get_brightness(brightness1);
      myIdleColor.val = brightness1 = brightlevel;
      for (int i = 0; i < n1; i++)
      {
        leds0[i] = myIdleColor;
      }
      for (int i = 0; i < n2; i++)
      {
        leds1[i] = myIdleColor;
      }
      for (int i = 0; i < n3; i++)
      {
        leds2[i] = myIdleColor;
      }
      if (brightlevel == 0)
      {
        isMaxBrightness = false;
        bandms = 0;
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
      readings1[i] = int(map(ann_readings[i], 0.0, 8.0, 80.0, 255.0));
    }
  }
  else if (SCULPTURE_ID == 2)
  {
    for (int i = 0; i < NUMDATA2; i++)
    {
      readings2[i] = int(map(sohsuang_readings[i], 0.0, 7.0, 80.0, 255.0));
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
  hasplayModeChanged = true; //trigger sound change
  band_delay = BAND_DELAY;
  maxBrightLvl = 255;
  Serial.println("IDLE MODE");
  isMaxBrightness = false;
  brightness1 = brightness2 = brightness3 = brightness4 = brightness5 = brightness6 = brightness7 = 0;
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
    fade_all();

    if (has_all_fade() == true)
    {
      activeLedState = 1; //go to next state
      bandms = 0;
      readingsCounter = 0;
      currBrightVal = prevBrightVal = 0;
      myIdleColor.val = activeColor1.val = activeColor2.val = activeColor3.val = 0;
    }
  }
  else if (activeLedState == 1) //finished dimming, show the reading
  {
    if (bandms < BAND_DELAY * 4) //control the speed of the fade animation here
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
        if (activeColor1.val < currBrightVal)
        { //brighten
          activeColor1.val += 5;
          activeColor2.val += 5;
          activeColor3.val += 5;
        }
      }
      else //curr is < prev bright val
      {
        if (activeColor1.val > currBrightVal)
        { //dim
          activeColor1.val -= 5;
          activeColor2.val -= 5;
          activeColor3.val -= 5;
        }
      }

      //change vol here
      vol = map(float(activeColor1.val), 0.0, 255.0, 0.3, 0.7);
      sgtl5000_1.volume(vol);
      // Serial.print("vol: "); Serial.println(vol);

      //change led brightness here
      set_all_to_brightLevel(int(activeColor1.val));
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
    fade_all();

    if (has_all_fade() == true)
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
    set_all_to_brightLevel(maxBrightLvl);

    //change vol here
    vol = map(float(activeColor1.val), 0.0, 255.0, 0.3, 0.7);
    sgtl5000_1.volume(vol);
    // Serial.print("vol: "); Serial.println(vol);

    if (bandms > SLIDER_WAIT)
    {
      activeLedState = 1; //start fade before idling
    }
  }
  else if (activeLedState == 1)
  {
    fade_all();

    if (has_all_fade() == true)
    {
      activeLedState = 2; //go to next state
    }
  }
  else if (activeLedState == 2)
  {
    go_idle();
  }
}

/*--------------------------------------------------------------------------------
  Selects the tracks according to playMode
--------------------------------------------------------------------------------*/
void play_audio()
{
  if (playMode == IDLE_MODE)
  {
    if (hasplayModeChanged == true && playSdWav1.isPlaying() == true)
    {
      playSdWav1.stop();
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
      hasplayModeChanged = false;
    }
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
    }
  }
  else if (playMode == BUTTON_MODE || playMode == SLIDER_MODE)
  {
    if (hasplayModeChanged == true && playSdWav1.isPlaying() == true)
    {
      playSdWav1.stop();
      playSdWav1.play(activeTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(activeTrack);
      hasplayModeChanged = false;
    }
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(activeTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(activeTrack);
    }
  }
}

/*--------------------------------------------------------------------------------
  Selects the playMode according to user console input
--------------------------------------------------------------------------------*/
void check_playMode()
{
  if (isButtonPressed == true) //process button press
  {
    isButtonPressed = false; //listen again for button presses
    playMode = BUTTON_MODE;
    hasplayModeChanged = true; //trigger sound change
    Serial.println("BUTTON MODE");

    activeLedState = 0;          //reset the led if currently active
    band_delay = BAND_DELAY / 4; //speed up the fade animation
  }
  else if (isSliderToggled == true)
  {
    isSliderToggled = false; //listen again for slider movement
    playMode = SLIDER_MODE;
    hasplayModeChanged = true; //trigger sound change
    Serial.println("SLIDER MODE");

    band_delay = BAND_DELAY / 4; //speed up the fade animation
  }
}