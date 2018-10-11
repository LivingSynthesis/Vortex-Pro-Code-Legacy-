/* 
 * Welcome to the Vortex Source Code. This program runs on the Vortex LED-Orbit giving it nearly unlimited 
 * visual customization options. I'll guide you through it and give you the tools you need to make it your own! 
 * Good luck, happy hacking, and remember. YesYouCan(); 
 *  -Shane "Stone" Aronson  
 */

// "Modes.h" and "Buttons.h" contain original code.
#include "Modes.h"
#include "Buttons.h"
// "FastLED.h" and "FlashStorage.h" specialize in LEDs and Storage. 
#include <FastLED.h>
#include <FlashStorage.h>
// These libraries are made by various people to re-use well-tested code instead of starting from scratch.

//  These values are used to describe the way the Vortex is hard-wired do not change them. 
#define NUM_LEDS 28
#define DATA_PIN 4
#define CLOCK_PIN 3

// These special variables are from the Labraries we included before.
CRGB leds[NUM_LEDS];
ColorBank colors;
Modes mode[5];
Buttons button[2];

// This is the save format for FlashStorage.
typedef struct Orbit {
  bool dataIsStored;
  int colorBankHue[16][8];
  int colorBankSat[16][8];
  int colorBankVal[16][8];
  int colorBankSetSize[16];
  int targetAnimation[5];
  int layoutNum[5];
  int hue[5][4][8];
  int sat[5][4][8];
  int val[5][4][8];
  int numColors[5][4];
  unsigned long onTime[5][4];
  unsigned long offTime[5][4];
  unsigned long breakTime[5][4];
};

FlashStorage(saveData, Orbit);

// Variables.
int s, m, modeNum;
int hue, sat, val;
int targetMenu = 0;
int targetSection = 0;
int currentSection = 0;
int targetLayout;
bool changingSection;
bool changingPattern;
int patternNum;
int animRound = 0;
int targetSet = 0;
int targetSlot = 0;
int targetColorBand = 0;
int targetHue = 0;
int targetSat = 0;
int targetVal = 0;
int currentSet = 0;
int currentSlot = 0;
int currentColorBand = 0;
int currentHue = 0;
int currentSat = 0;
int stage;
bool colorIsBlank;


// Everything in the setup function runs one time when the program starts up.
void setup() {
  FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(35);
  button[0].createButton(0);
  button[1].createButton(2);
  setupDefaults();
  loadSave();
}

// The loop runs continuously.
void loop() {
// Every cycle it updates the lights, checks the buttons, and tells FastLED to show the update.
  updateLights();
  checkButton();
  FastLED.show();
}

// The rest of the code is functions, all of the code inside a fuction runs when it's name is called. 

//Checks what menu is currently active and calls that menu's function
void updateLights() {
  m = modeNum;
  int menu = mode[m].menuNum;
  if (menu == 0)playMode(0);
  if (menu == 1)modeOptions();
  if (menu == 2)chooseLayout();
  if (menu == 3)modePreview();
  if (menu == 4)colorMenu();
  if (menu == 5)animations();
  if (menu == 8)menuAnimation();
  if (menu == 9)saveAnimation();
}

// Displays current mode, which has up to 4 sections by default, which each have a pattern and set of colors.
void playMode(int v) {
  //reset's each section's timer, then creates the next step in each section one-by-one.
  for (s = 0; s < mode[m].numOfSections; s++) mode[m].section[s].mainClock = millis();
  for (s = 0; s < mode[m].numOfSections; s++) {
    //gets the next color for the current section
    getColor();
    //checks to see if enough time has passed for the currrent section.
    if (mode[m].section[s].mainClock - mode[m].section[s].prevTime > mode[m].section[s].duration) {
      //checks if the if the LED was last turned off or on.
      if (!mode[m].section[s].on) {
        //if it was off, it sets up the next time duration, sets the color, then prepares the next color. 
        nextDuration(0);
        setSectionColor(v);
        nextColor();
      }
      if (mode[m].section[s].on) {
        //if it was on, it sets up the next duration and turns the light off.
        if (mode[m].section[s].offTime != 0) {
          nextDuration(1);
          setSectionBlank(v);
        }
        //if it was on the last color, it can have a unique durration before starting over
        if (mode[m].section[s].endOfSet) {
          if (mode[m].section[s].breakTime != 0) {
            nextDuration(2);
            setSectionBlank(v);
          }
          mode[m].section[s].endOfSet = false;
        }
      }
      //toggles the lights on/off and resyncs the clock for the next cycle.
      mode[m].section[s].on = !mode[m].section[s].on;
      mode[m].section[s].prevTime = mode[m].section[s].mainClock;
    }
    // v is for displaying a preview of the above mode in other menus.
    if (v == 2) {
      if (colorIsBlank) {
        for (int t = 0; t < mode[m].section[s].sectionSize; t++) {
          int target = mode[m].section[s].ledNum[t];
          leds[target].setHSV(0, 0, 1);
        }
      }
      if (mode[m].target.on) {
        for (int t = 0; t < mode[m].section[targetSection].sectionSize; t++) {
          int target = mode[m].section[targetSection].ledNum[t];
          leds[target].setHSV(0, 0, 0);
        }
      }
      blinkTarget(300);
    }
  }
  //this play's the Animation if it's on, when appropriate.
  if (mode[m].menuNum != 3)if (mode[m].targetAnimation > 0)playAnimation();
}

//looks up a stored color's values.
void getColor() {
  int c = mode[m].section[s].currentColor;
  hue = mode[m].section[s].hue[c];
  sat = mode[m].section[s].sat[c];
  val = mode[m].section[s].val[c];
  if (val == 0) colorIsBlank = true;
  else colorIsBlank = false;
}

//set's all the LEDs in the target section to the current color value.
void setSectionColor(int v) {
  if (v == 0 || v == 2) {
    if (!changingPattern) {
      for (int t = 0; t < mode[m].section[s].sectionSize; t++) {
        int target = mode[m].section[s].ledNum[t];
        leds[target].setHSV(hue, sat, val);
      }
    }
  }
  if (v == 1) {
    leds[7 + s].setHSV(hue, sat, val);
    leds[13 - s].setHSV(hue, sat, val);
  }
}

void setSectionBlank(int v) {
  if (v == 0 || v == 2) {
    for (int t = 0; t < mode[m].section[s].sectionSize; t++) {
      int target = mode[m].section[s].ledNum[t];
      leds[target].setHSV(0, 0, 0);
    }
  }
  if (v == 1) {
    if (mode[m].numOfSections < 4)leds[10].setHSV(0, 0, 0);
    if (mode[m].numOfSections < 3)leds[9].setHSV(0, 0, 0), leds[11].setHSV(0, 0, 0);
    if (mode[m].numOfSections < 2)leds[8].setHSV(0, 0, 0), leds[12].setHSV(0, 0, 0);
    leds[7 + s].setHSV(0, 0, 0);
    leds[13 - s].setHSV(0, 0, 0);
  }
}

//keeps track of current and next color.
void nextColor() {
  if (mode[m].section[s].currentColor < mode[m].section[s].numColors) mode[m].section[s].currentColor++;
  if (mode[m].section[s].currentColor >= mode[m].section[s].numColors) mode[m].section[s].currentColor = 0, mode[m].section[s].endOfSet = true;
}

//updates the next duration to the correct value.
void nextDuration(int next) {
  if (next == 0) mode[m].section[s].duration = mode[m].section[s].onTime;
  if (next == 1) mode[m].section[s].duration = mode[m].section[s].offTime;
  if (next == 2) mode[m].section[s].duration = mode[m].section[s].breakTime;
}

// this is the menu containing the mode settings.
void modeOptions() {
  if (!mode[m].target.on) {
    //these are the 4 parts in the settings menu.
    playSettings(0);
    playMode(1);
    playSettings(1);
    playSettings(2);
  }
  if (mode[m].target.on) {
    if (targetMenu != 0)playSettings(0);
    if (targetMenu != 1)playMode(1);
    if (targetMenu != 2)playSettings(1);
    if (targetMenu != 3)playSettings(2);
    for (int t = 0; t < 7; t++) {
      leds[(targetMenu * 7) + t].setHSV(0, 0, 0);
    }
  }
  blinkTarget(500);
}

//indicates your cursor with a long blink in various menus.
void blinkTarget(unsigned long blinkTime) {
  mode[m].target.mainClock = millis();
  if (mode[m].target.mainClock - mode[m].target.prevTime > blinkTime) {
    mode[m].target.on = !mode[m].target.on;
    mode[m].target.prevTime = mode[m].target.mainClock;
  }
}

// the display animation for the different menue sections.
void playSettings(int v) {
  mode[m].settings[0].duration = 600;
  mode[m].settings[1].duration = 80;
  mode[m].settings[2].duration = 150;
  mode[m].settings[v].mainClock = millis();
  if (mode[m].settings[v].mainClock - mode[m].settings[v].prevTime > mode[m].settings[v].duration) {
    for (int t = 0; t < 7; t++) {
      if (v == 0) {
        if (mode[m].settings[v].on) {
          if (t % 2 == 0)leds[t].setHSV(0, 0, 10);
          if (t % 2 == 1)leds[t].setHSV(0, 0, 200);
        }
        if (!mode[m].settings[v].on) {
          if (t % 2 == 0)leds[t].setHSV(0, 0, 200);
          if (t % 2 == 1)leds[t].setHSV(0, 0, 10);
        }
      }
      if (v == 1) {
        int color = (t + mode[m].settings[v].frame) * 32;
        if (color > 255) color -= 255;
        leds[14 + t].setHue(color);
      }
      if (v == 2) {
        leds[21 + t].setHSV(0, 0, 0);
        if (mode[m].settings[v].frame >= 0 && mode[m].settings[v].frame <= 3) {
          leds[21 + mode[m].settings[v].frame].setHSV(0, 0, 255);
          leds[27 - mode[m].settings[v].frame].setHSV(0, 0, 255);

        }
        if (mode[m].settings[v].frame >= 4 && mode[m].settings[v].frame <= 7) {
          leds[21 + (7 - mode[m].settings[v].frame)].setHSV(0, 0, 255);
          leds[27 - (7 - mode[m].settings[v].frame)].setHSV(0, 0, 255);
        }
      }
    }
    mode[m].settings[v].frame++;
    if (mode[m].settings[v].frame > 7)mode[m].settings[v].frame = 0;
    mode[m].settings[v].on = !mode[m].settings[v].on;
    mode[m].settings[v].prevTime = mode[m].settings[v].mainClock;
  }
}

// you can see and preview your current mode as well as customize it's sections.
void modePreview() {
  if (!changingPattern) {
    if (mode[m].layoutNum != 3 || mode[m].targetAnimation == 0)playMode(2);
    if (mode[m].layoutNum == 3 && mode[m].targetAnimation > 0)showPattern(1);
  }
  if (changingPattern) {
    setPattern(patternNum);
    showPattern(0);
  }
}

//when adding a new color set you will see it displayed on all lights while picking a pattern.
void showPattern(int ver) {
  int sect;
  if (ver == 0) sect = currentSection;
  if (ver == 1) sect = targetSection;
  mode[m].section[sect].mainClock = millis();
  int c = mode[m].section[sect].currentColor;
  hue = mode[m].section[sect].hue[c];
  sat = mode[m].section[sect].sat[c];
  val = mode[m].section[sect].val[c];
  if (mode[m].section[sect].mainClock - mode[m].section[sect].prevTime > mode[m].section[sect].duration) {
    if (!mode[m].section[sect].on) {
      mode[m].section[sect].duration = mode[m].section[sect].onTime;
      for (int a = 0; a < 28; a++) {
        if (ver == 1 && val == 0) leds[a].setHSV(0, 0, 1);
        if (val != 0) leds[a].setHSV(hue, sat, val);
      }
      if (mode[m].section[sect].currentColor < mode[m].section[sect].numColors) mode[m].section[sect].currentColor++;
      if (mode[m].section[sect].currentColor >= mode[m].section[sect].numColors) mode[m].section[sect].currentColor = 0, mode[m].section[sect].endOfSet = true;
    }
    if (mode[m].section[sect].on) {
      if (mode[m].section[sect].offTime != 0) {
        mode[m].section[sect].duration = mode[m].section[sect].offTime;
        for (int a = 0; a < 28; a++) {
          leds[a].setHSV(0, 0, 0);
        }
      }
      if (mode[m].section[sect].endOfSet) {
        if (mode[m].section[sect].breakTime != 0) {
          mode[m].section[sect].duration = mode[m].section[sect].breakTime;
          for (int a = 0; a < 28; a++) {
            leds[a].setHSV(0, 0, 0);
          }
        }
        mode[m].section[sect].endOfSet = false;
      }
    }
    mode[m].section[sect].on = !mode[m].section[sect].on;
    mode[m].section[sect].prevTime = mode[m].section[sect].mainClock;
  }
  if (ver == 1) {
    if (mode[m].target.on) {
      for (int t = 0; t < mode[m].section[sect].sectionSize; t++) {
        int target = mode[m].section[sect].ledNum[t];
        leds[target].setHSV(0, 0, 0);
      }
    }
    blinkTarget(300);
  }
}

//in the color menu you can select different stored colors sets of up to 8 colors and edit them
void colorMenu() {
  if (stage == 0) {
    mode[m].section[s].mainClock = millis();
    if (mode[m].section[s].mainClock - mode[m].section[s].prevTime > 30) {
      int setNum = 0;
      for (int side = 0; side < 4; side++) {
        for (int slot = 0; slot < 4; slot++) {
          int colorNum = colors.currentBankColor[setNum];
          hue = colors.colorBankHue[setNum][colorNum];
          sat = colors.colorBankSat[setNum][colorNum];
          val = colors.colorBankVal[setNum][colorNum];
          leds[3 + (7 * side) + slot].setHSV(hue, sat, val);
          leds[3 + (7 * side) - slot].setHSV(hue, sat, val);
          nextBankColor(setNum);
          setNum++;
        }
      }
      mode[m].section[s].prevTime = mode[m].section[s].mainClock;
    }
    if (mode[m].target.on) {
      int side = targetSet / 4;
      int slot = targetSet % 4;
      leds[3 + (7 * side) + slot].setHSV(0, 0, 0);
      leds[3 + (7 * side) - slot].setHSV(0, 0, 0);
    }
    blinkTarget(300);
  }
  if (stage == 1) {
    int setSize = colors.colorBankSetSize[currentSet];
    for (int a = 0; a < 28; a++) leds[a].setHSV(0, 0, 0);
    for (int colorSlot = 0; colorSlot < 8; colorSlot++) {
      int side = colorSlot / 2;
      leds[colorSlot + (5 * side)].setHSV(0, 0, 1);
      leds[((2 + (3 * side)) * 3) - colorSlot].setHSV(0, 0, 1);
    }
    for (int colorNum = 0; colorNum < setSize; colorNum++) {
      hue = colors.colorBankHue[currentSet][colorNum];
      sat = colors.colorBankSat[currentSet][colorNum];
      val = colors.colorBankVal[currentSet][colorNum];
      int side = colorNum / 2;
      leds[(5 * side) + colorNum].setHSV(hue, sat, val);
      leds[((2 + (3 * side)) * 3) - colorNum].setHSV(hue, sat, val);
    }
    if (targetSlot < setSize) {
      if (mode[m].target.on) {
        int side = targetSlot / 2;
        int blinkVal;
        if (colors.colorBankVal[currentSet][targetSlot] == 0)blinkVal = 5;
        else blinkVal = 0;
        leds[targetSlot + (5 * side)].setHSV(0, 0, blinkVal);
        leds[((2 + (3 * side)) * 3) - targetSlot].setHSV(0, 0, blinkVal);
      }
      blinkTarget(300);
    }
    if (targetSlot == setSize) {
      if (mode[m].target.on) {
        int side = (setSize - 1) / 2;
        leds[(5 * side) + (setSize - 1)].setHSV(0, 0, 1);
        leds[((2 + (3 * side)) * 3) - (setSize - 1)].setHSV(0, 0, 1);
        for (int side = 0; side < 4; side++) {
          leds[2 + (7 * side)].setHSV(0, 0, 1);
          leds[4 + (7 * side)].setHSV(0, 0, 1);
        }
      }
      blinkTarget(60);
    }
    if (targetSlot > setSize) {
      if (mode[m].target.on) {
        int targLed = targetSlot - 1;
        int side = targLed / 2;
        leds[targLed + (5 * side)].setHSV(0, 0, 0);
        leds[((2 + (3 * side)) * 3) - targLed].setHSV(0, 0, 0);
      }
      blinkTarget(300);
    }
  }
  if (stage == 2)colorWheel(0);
  if (stage == 3)colorWheel(1);
  if (stage == 4)colorWheel(2);
  if (stage == 5)colorWheel(3);
}

// manages colors displayed in the color bank
void nextBankColor(int set) {
  colors.currentBankColor[set]++;
  if (colors.currentBankColor[set] >= colors.colorBankSetSize[set])colors.currentBankColor[set] = 0;
}

//mangages color selection from 16,000 H,S,V options.
void colorWheel(int wheelLevel) {
  int side;
  for (int color = 0; color < 16; color++) {
    side = color / 4;
    int hueValue;
    int satValue;
    int valValue;
    if (color >= 0 + 4 * side && color <= 3 + side * 4) {
      if (wheelLevel == 0)hueValue = color * 16;
      if (wheelLevel == 1)hueValue = (color * 4) + (64 * currentColorBand);
      satValue = 255;
      if (wheelLevel == 2) {
        hueValue = (currentHue * 4) + (64 * currentColorBand);
        satValue = 255 - (16 * color);
      }
      valValue = 255;
      if (wheelLevel == 3) {
        hueValue = (currentHue * 4) + (64 * currentColorBand);
        satValue = 255 - (16 * currentSat);
        valValue = 255 - (16 * color);
        if (color == 15) valValue = 0;
      }
      leds[(side * 3) + color].setHSV(hueValue, satValue, valValue);
      leds[((side * 11) + 6) - color].setHSV(hueValue, satValue, valValue);
    }
  }
  if (mode[m].target.on) {
    int theTarget;
    if (wheelLevel == 0)for (int band = 0; band < 7; band++)leds[band + (7 * targetColorBand)].setHSV(0, 0, 0);
    if (wheelLevel > 0) {
      if (wheelLevel == 1)side = targetHue / 4, theTarget = targetHue;
      if (wheelLevel == 2)side = targetSat / 4, theTarget = targetSat;
      if (wheelLevel == 3)side = targetVal / 4, theTarget = targetVal;
      leds[(side * 3) + theTarget].setHSV(0, 0, 0);
      leds[((side * 11) + 6) - theTarget].setHSV(0, 0, 0);
      if (wheelLevel == 3 && theTarget == 15) {
        leds[24].setHSV(0, 0, 1);
      }
    }
  }
  blinkTarget(300);
}

//the layout is how the sections are arranged together.
void chooseLayout() {
  mode[m].setLayout(targetLayout);
  playMode(0);
}

//here are all the patterns which are (on-time,off-time,break-time) format
void setPattern(int patNum) {
  if (patNum == 0)mode[m].section[currentSection].setTiming(1, 0, 0);
  if (patNum == 1)mode[m].section[currentSection].setTiming(5, 0, 0);
  if (patNum == 2)mode[m].section[currentSection].setTiming(10, 0, 0);
  if (patNum == 3)mode[m].section[currentSection].setTiming(20, 0, 0);
  if (patNum == 4)mode[m].section[currentSection].setTiming(0, 1, 0);
  if (patNum == 5)mode[m].section[currentSection].setTiming(0, 10, 0);
  if (patNum == 6)mode[m].section[currentSection].setTiming(0, 20, 0);
  if (patNum == 7)mode[m].section[currentSection].setTiming(5, 5, 0);
  if (patNum == 8)mode[m].section[currentSection].setTiming(5, 15, 0);
  if (patNum == 9)mode[m].section[currentSection].setTiming(10, 5, 0);
  if (patNum == 10)mode[m].section[currentSection].setTiming(10, 15, 0);
  if (patNum == 11)mode[m].section[currentSection].setTiming(20, 5, 0);
  if (patNum == 12)mode[m].section[currentSection].setTiming(20, 25, 0);
  if (patNum == 13)mode[m].section[currentSection].setTiming(0, 0, 1);
  if (patNum == 14)mode[m].section[currentSection].setTiming(0, 0, 10);
  if (patNum == 15)mode[m].section[currentSection].setTiming(0, 0, 20);
  if (patNum == 16)mode[m].section[currentSection].setTiming(5, 0, 10);
  if (patNum == 17)mode[m].section[currentSection].setTiming(5, 0, 30);
  if (patNum == 18)mode[m].section[currentSection].setTiming(10, 0, 10);
  if (patNum == 19)mode[m].section[currentSection].setTiming(10, 0, 30);
  if (patNum == 20)mode[m].section[currentSection].setTiming(3, 3, 10);
  if (patNum == 21)mode[m].section[currentSection].setTiming(3, 3, 20);
}

// this menu toggles the animation on or off.
void animations() {
  if (mode[m].targetAnimation == 1 && mode[m].layoutNum == 3)mode[m].numOfSections = 4;
  if (mode[m].targetAnimation == 0 && mode[m].layoutNum == 3)mode[m].numOfSections = 1;
  playMode(0);
}

// manages the animation swaping different sections' positions.
void playAnimation() {
  mode[m].settings[3].mainClock = millis();
  if (mode[m].settings[3].mainClock - mode[m].settings[3].prevTime > 300) {
    if (mode[m].targetAnimation == 1) {
      mode[m].rotateSections();
      mode[m].settings[3].prevTime = mode[m].settings[3].mainClock;
    }
  }
}

// this is an animation that specifically plays when opening the menu.
void menuAnimation() {
  mode[m].settings[3].mainClock = millis();
  if (mode[m].settings[3].mainClock - mode[m].settings[3].prevTime > 75) {
    for (int a = 0; a < 28; a++) {
      leds[a].setHSV(0, 0, 0);
    }
    for (int side = 0; side < 4; side++) {
      int frame = mode[m].settings[3].frame;
      if (frame >= 0 && frame <= 3) {
        leds[3 + (7 * side) + frame].setHSV(0, 0, 255);
        leds[3 + (7 * side) - frame].setHSV(0, 0, 255);

      }
      if (frame >= 4 && frame <= 6) {
        leds[3 + (7 * side) + (6 - frame)].setHSV(0, 0, 255);
        leds[3 + (7 * side) - (6 - frame)].setHSV(0, 0, 255);
      }
    }
    mode[m].settings[3].frame++;
    if (mode[m].settings[3].frame > 6) mode[m].settings[3].frame = 0;
    mode[m].settings[3].prevTime = mode[m].settings[3].mainClock;
  }
}

// this is an animation that plays specifially when saving.
void saveAnimation () {
  mode[m].settings[3].mainClock = millis();
  if (mode[m].settings[3].mainClock - mode[m].settings[3].prevTime > 100) {
    for (int a = 0; a < 28; a++) {
      leds[a].setHSV(0, 0, 255);
    }
    if (mode[m].settings[3].on) {
      for (int a = 0; a < 28; a++) {
        leds[a].setHSV(0, 0, 0);
      }
    }
    mode[m].settings[3].on = !mode[m].settings[3].on;
    mode[m].settings[3].prevTime = mode[m].settings[3].mainClock;
  }
}

// this outputs your vortex's saved settings as a list of values on the computer.
void printVortex() {
  for (int i = 0; i < 5; i++)mode[i].printMode(i);
  colors.printColors();
}

// checks the buttons to see if and how long they are pressed.
void checkButton() {
  int menu = mode[m].menuNum;
  for (int b = 0; b < 2; b++) {
    button[b].buttonState = digitalRead(button[b].pinNum);
    if (button[b].buttonState == LOW && button[b].lastButtonState == HIGH && (millis() - button[b].pressTime > 200)) {
      button[b].pressTime = millis();
    }
    button[b].holdTime = (millis() - button[b].pressTime);
    if (button[b].holdTime > 50) {
      if (button[b].buttonState == LOW && button[b].holdTime > button[b].prevHoldTime) {
        if (b == 0) {
          if (button[b].holdTime > 2000 && button[b].holdTime < 3000) mode[m].menuNum = 8;
          if (button[b].holdTime > 3000 && mode[m].menuNum == 8) mode[m].menuNum = 1;
        }
        if (b == 1) {
          if (button[b].holdTime > 2000 && button[b].holdTime < 3000) mode[m].menuNum = 9;
          if (button[b].holdTime > 3000 && mode[m].menuNum == 9)saveAll(), printVortex(), mode[m].menuNum = 0;
        }
      }
      if (button[b].buttonState == HIGH && button[b].lastButtonState == LOW && millis() - button[b].prevPressTime > 200) {
        if (button[b].holdTime < 300) {
          if (b == 0) {
            if (menu == 0)modeNum++;
            if (menu == 1)targetMenu++;
            if (menu == 2)targetLayout++;
            if (menu == 3) {
              if (!changingPattern)targetSection++;
              if (changingPattern)patternNum++;
            }
            if (menu == 4) {
              if (stage == 0)targetSet++;
              if (stage == 1)targetSlot++;
              if (stage == 2)targetColorBand++;
              if (stage == 3)targetHue++;
              if (stage == 4)targetSat++;
              if (stage == 5)targetVal++;
            }
            if (menu == 5)mode[m].targetAnimation++;
          }
          if (b == 1) {
            if (menu == 0)modeNum--;
            if (menu == 1)targetMenu--;
            if (menu == 2)targetLayout--;
            if (menu == 3) {
              if (!changingPattern)targetSection--;
              if (changingPattern)patternNum--;
            }
            if (menu == 4) {
              if (stage == 0)targetSet--;
              if (stage == 1)targetSlot--;
              if (stage == 2)targetColorBand--;
              if (stage == 3)targetHue--;
              if (stage == 4)targetSat--;
              if (stage == 5)targetVal--;
            }
            if (menu == 5)mode[m].targetAnimation--;
          }
        }
        if (button[b].holdTime > 400 && button[b].holdTime < 3000) {
          if (b == 0) {
            if (menu == 1)mode[m].menuNum = targetMenu + 2, stage = 0;
            if (menu == 2)mode[m].menuNum = 1, mode[m].targetAnimation = 0, mode[m].restoreOrder();
            if (menu == 3) {
              if (!changingPattern)mode[m].menuNum = 4, currentSection = targetSection, changingSection = true;
              if (changingPattern)mode[m].menuNum = 3, changingPattern = false;
            }
            if (menu == 4) {
              if (!changingSection) {
                if (stage == 0)stage = 1, currentSet = targetSet;
                else if (stage == 1) {
                  int setSize = colors.colorBankSetSize[currentSet];
                  if (targetSlot < setSize)stage = 2, currentSlot = targetSlot;
                  if (targetSlot > setSize)stage = 2, currentSlot = setSize;
                  if (targetSlot == setSize)colors.deleteColor(currentSet);
                }
                else if (stage == 2)stage = 3, currentColorBand = targetColorBand;
                else if (stage == 3)stage = 4, currentHue = targetHue;
                else if (stage == 4)stage = 5, currentSat = targetSat;
                else if (stage == 5) {
                  int saveHue = (currentHue * 4) + (64 * currentColorBand);
                  int saveSat = 255 - (16 * currentSat);
                  int saveVal = 255 - (16 * targetVal);
                  if (targetVal == 15) saveVal = 0;
                  int slotVal = currentSlot;
                  //if (currentSlot >= 2)slotVal = currentSlot - 1;
                  stage = 1, colors.saveColor(currentSet, slotVal, saveHue, saveSat, saveVal);
                  Serial.println(saveHue);
                  Serial.println(saveSat);
                  Serial.println(saveVal);
                }
              }
              if (changingSection) {
                mode[m].section[currentSection].setColorSet(colors.colorBankHue[targetSet], colors.colorBankSat[targetSet], colors.colorBankVal[targetSet]);
                mode[m].section[currentSection].setNumColors(colors.colorBankSetSize[targetSet]);
                mode[m].section[currentSection].storePattern();
                changingSection = false;
                changingPattern = true;
                mode[m].menuNum = 3;
              }
            }
            if (menu == 5)mode[m].menuNum = 1;
          }

          if (b == 1) {
            if (menu == 1)mode[m].menuNum = 0;
            if (menu == 2)mode[m].menuNum = 1;
            if (menu == 3) {
              if (!changingPattern)mode[m].menuNum = 1;
              if (changingPattern) {
                changingSection = true;
                changingPattern = false;
                mode[m].section[currentSection].revertColorSet();
                mode[m].section[currentSection].revertPattern();
                mode[m].menuNum = 4;
              }
            }
            if (menu == 4) {
              if (stage == 0) {
                if (!changingSection)mode[m].menuNum = 1;
                if (changingSection)mode[m].menuNum = 3, changingSection = false;
              }
              if (stage == 1)stage = 0;
              if (stage == 2)stage = 1;
              if (stage == 3)stage = 2;
              if (stage == 4)stage = 3;
              if (stage == 5)stage = 4;
            }
            if (menu == 5)mode[m].menuNum = 1;
          }
        }
        if (button[b].holdTime < 3000 && mode[m].menuNum == 8)mode[m].menuNum = 0;
        if (button[b].holdTime < 3000 && mode[m].menuNum == 9)mode[m].menuNum = 0;
        button[b].prevPressTime = millis();
      }
    }
    // on a button press one these values will change, these are the max and minimum values for each variable.
    if (mode[m].targetAnimation < 0) mode[m].targetAnimation = 1;
    if (mode[m].targetAnimation > 1) mode[m].targetAnimation = 0;
    if (targetLayout < 0)targetLayout = 3;
    if (targetLayout > 3)targetLayout = 0;
    if (patternNum < 0)patternNum = 21;
    if (patternNum > 21)patternNum = 0;
    int maxSlot = colors.colorBankSetSize[currentSet] + 1;
    if (colors.colorBankSetSize[currentSet] == 8)maxSlot = colors.colorBankSetSize[currentSet];
    if (targetVal < 0)targetVal = 15;
    if (targetVal > 15)targetVal = 0;
    if (targetSat < 0)targetSat = 15;
    if (targetSat > 15)targetSat = 0;
    if (targetHue < 0)targetHue = 15;
    if (targetHue > 15)targetHue = 0;
    if (targetColorBand < 0)targetColorBand = 3;
    if (targetColorBand > 3)targetColorBand = 0;
    if (targetSlot < 0)targetSlot = maxSlot;
    if (targetSlot > maxSlot)targetSlot = 0;
    if (targetSet < 0)targetSet = 15;
    if (targetSet > 15)targetSet = 0;
    if (targetSection < 0)targetSection = mode[m].numOfSections - 1;
    if (targetSection > mode[m].numOfSections - 1)targetSection = 0;
    if (targetMenu < 0)targetMenu = 3;
    if (targetMenu > 3)targetMenu = 0;
    if (modeNum < 0)modeNum = 4;
    if (modeNum > 4)modeNum = 0;
    button[b].lastButtonState = button[b].buttonState;
    button[b].prevHoldTime = button[b].holdTime;
  }
}

// this reads the data saved on the Vortex and loads it.
void loadSave() {
  Orbit myOrbit;
  myOrbit = saveData.read();
  if (myOrbit.dataIsStored == true) {
    for (int set = 0; set < 16; set++) {
      colors.colorBankSetSize[set] = myOrbit.colorBankSetSize[set];
      for (int slot = 0; slot < 8; slot ++) {
        colors.colorBankHue[set][slot] = myOrbit.colorBankHue[set][slot];
        colors.colorBankSat[set][slot] = myOrbit.colorBankSat[set][slot];
        colors.colorBankVal[set][slot] = myOrbit.colorBankVal[set][slot];
      }
    }
    for (int z = 0; z < 5; z++) {
      mode[z].targetAnimation = myOrbit.targetAnimation[z];
      mode[z].setLayout(myOrbit.layoutNum[z]);
      for (int sect = 0; sect < 4; sect ++) {
        mode[z].section[sect].numColors = myOrbit.numColors[z][sect];
        mode[z].section[sect].onTime = myOrbit.onTime[z][sect];
        mode[z].section[sect].offTime = myOrbit.offTime[z][sect];
        mode[z].section[sect].breakTime = myOrbit.breakTime[z][sect];
        for (int slot = 0; slot < myOrbit.numColors[z][sect]; slot++) {
          mode[z].section[sect].hue[slot] = myOrbit.hue[z][sect][slot];
          mode[z].section[sect].sat[slot] = myOrbit.sat[z][sect][slot];
          mode[z].section[sect].val[slot] = myOrbit.val[z][sect][slot];
        }
      }
    }
  }
}

// saves the settings.
void saveAll() {
  Orbit myOrbit;
  for (int set = 0; set < 16; set++) {
    myOrbit.colorBankSetSize[set] = colors.colorBankSetSize[set];
    for (int slot = 0; slot < 8; slot ++) {
      myOrbit.colorBankHue[set][slot] = colors.colorBankHue[set][slot];
      myOrbit.colorBankSat[set][slot] = colors.colorBankSat[set][slot];
      myOrbit.colorBankVal[set][slot] = colors.colorBankVal[set][slot];
    }
  }
  for (int z = 0; z < 5; z++) {
    myOrbit.targetAnimation[z] = mode[z].targetAnimation;
    myOrbit.layoutNum[z] = mode[z].layoutNum;
    for (int sect = 0; sect < 4; sect ++) {
      myOrbit.numColors[z][sect] = mode[z].section[sect].numColors;
      myOrbit.onTime[z][sect] = mode[z].section[sect].onTime;
      myOrbit.offTime[z][sect] = mode[z].section[sect].offTime;
      myOrbit.breakTime[z][sect] = mode[z].section[sect].breakTime;
      for (int slot = 0; slot < myOrbit.numColors[z][sect]; slot++) {
        myOrbit.hue[z][sect][slot] = mode[z].section[sect].hue[slot];
        myOrbit.sat[z][sect][slot] = mode[z].section[sect].sat[slot];
        myOrbit.val[z][sect][slot] = mode[z].section[sect].val[slot];
      }
    }
  }
  myOrbit.dataIsStored = true;
  saveData.write(myOrbit);
}

// default settings loaded up before saving.
void setupDefaults() {
  for (int cset = 0; cset < 16; cset++) {
    colors.colorBankSetSize[cset] = 1;
    if (cset < 8) colors.colorBankHue[cset][0] = 0;
    if (cset >= 8) colors.colorBankHue[cset][0] = (cset - 8) * 32;
    colors.colorBankSat[cset][0] = 255;
    colors.colorBankVal[cset][0] = 255;
  }
  for (int sets = 0; sets < 4; sets++) {
    for (int fade = 0; fade < 8; fade++) {
      colors.colorBankHue[sets][fade] = (8 * fade) + (64 * sets);
      colors.colorBankSat[sets][fade] = 255;
      colors.colorBankVal[sets][fade] = 255;
    }
    colors.colorBankSetSize[sets] = 8;
  }
  colors.colorBankHue[4][1] = 96;
  colors.colorBankSat[4][1] = 255;
  colors.colorBankVal[4][1] = 255;
  colors.colorBankHue[4][2] = 160;
  colors.colorBankSat[4][2] = 255;
  colors.colorBankVal[4][2] = 255;
  colors.colorBankSetSize[4] = 3;

  for (int bows = 0; bows < 2; bows++) {
    for (int rain = 0; rain < 8; rain++) {
      colors.colorBankHue[5 + bows][rain] = rain * 32;
      colors.colorBankSat[5 + bows][rain] = 255 - (100 * bows);
      colors.colorBankVal[5 + bows][rain] = 255;
    }
    colors.colorBankSetSize[5 + bows] = 8;
  }
  colors.colorBankVal[7][1] = 0;
  colors.colorBankHue[7][2] = 96;
  colors.colorBankSat[7][2] = 255;
  colors.colorBankVal[7][2] = 255;
  colors.colorBankVal[7][3] = 0;
  colors.colorBankHue[7][4] = 160;
  colors.colorBankSat[7][4] = 255;
  colors.colorBankVal[7][4] = 255;
  colors.colorBankVal[7][5] = 0;
  colors.colorBankSetSize[7] = 6;

  //

  mode[0].setLayout(0);
  mode[0].section[0].setColorHSV(0, 64, 255, 255);
  mode[0].section[0].setColorHSV(1, 0, 255, 255);
  mode[0].section[0].setTiming(0, 0, 10);
  mode[0].section[0].setNumColors(2);

  mode[0].section[1].setColorHSV(0, 190, 255, 255);
  mode[0].section[1].setColorHSV(1, 32, 255, 255);
  mode[0].section[1].setTiming(0, 0, 0);
  mode[0].section[1].setNumColors(2);

  mode[0].section[2].setColorHSV(0, 180, 255, 255);
  mode[0].section[2].setColorHSV(1, 0, 255, 255);
  mode[0].section[2].setTiming(0, 0, 10);
  mode[0].section[2].setNumColors(2);

  mode[0].section[3].setColorHSV(0, 170, 255, 255);
  mode[0].section[3].setColorHSV(1, 64, 255, 255);
  mode[0].section[3].setTiming(0, 0, 20);
  mode[0].section[3].setNumColors(2);
  //
  mode[1].setLayout(1);
  mode[1].section[0].setColorHSV(0, 0 , 255, 255);
  mode[1].section[0].setColorHSV(1, 208 , 255, 255);
  mode[1].section[0].setTiming(0, 1, 0);
  mode[1].section[0].setNumColors(2);

  mode[1].section[1].setColorHSV(0, 160, 255, 255);
  mode[1].section[1].setColorHSV(1, 208 , 255, 255);
  mode[1].section[1].setTiming(0, 1, 0);
  mode[1].section[1].setNumColors(2);

  mode[1].section[2].setColorHSV(0, 96, 255, 255);
  mode[1].section[2].setTiming(0, 0, 10);
  mode[1].section[2].setNumColors(1);
  //
  mode[2].setLayout(2);
  mode[2].section[0].setColorHSV(0, 0, 255, 255);
  mode[2].section[0].setColorHSV(1, 32, 255, 255);
  mode[2].section[0].setTiming(0, 0, 1);
  mode[2].section[0].setNumColors(2);

  mode[2].section[1].setColorHSV(0, 64, 255, 255);
  mode[2].section[1].setColorHSV(1, 96, 255, 255);
  mode[2].section[1].setTiming(0, 0, 1);
  mode[2].section[1].setNumColors(2);

  mode[2].section[2].setColorHSV(0, 128, 255, 255);
  mode[2].section[2].setColorHSV(1, 160, 255, 255);
  mode[2].section[2].setTiming(0, 0, 1);
  mode[2].section[2].setNumColors(2);

  mode[2].section[3].setColorHSV(0, 192, 255, 255);
  mode[2].section[3].setColorHSV(1, 224, 255, 255);
  mode[2].section[3].setTiming(3, 0, 1);
  mode[2].section[3].setNumColors(2);
  //
  mode[3].setLayout(0);
  mode[3].section[0].setColorHSV(0, 128, 255, 255);
  mode[3].section[0].setColorHSV(1, 96, 255, 255);
  mode[3].section[0].setTiming(10, 20, 0);
  mode[3].section[0].setNumColors(2);

  mode[3].section[1].setColorHSV(0, 64, 255, 255);
  mode[3].section[1].setColorHSV(1, 32, 255, 255);
  mode[3].section[1].setTiming(20, 0, 20);
  mode[3].section[1].setNumColors(2);

  mode[3].section[2].setColorHSV(0, 192, 255, 255);
  mode[3].section[2].setColorHSV(1, 160, 255, 255);
  mode[3].section[2].setTiming(6, 1, 0);
  mode[3].section[2].setNumColors(2);

  mode[3].section[3].setColorHSV(0, 0, 255, 255);
  mode[3].section[3].setColorHSV(1, 224, 255, 255);
  mode[3].section[3].setTiming(1, 30, 0);
  mode[3].section[3].setNumColors(2);
  //
  mode[4].setLayout(3);
  for (int rain = 0; rain < 8; rain++) mode[4].section[0].setColorHSV(rain, 32 * rain, 255, 255);
  mode[4].section[0].setTiming(2, 0, 20);
  mode[4].section[0].setNumColors(8);
  for (int sct = 1; sct < 4; sct++) {
    mode[4].section[sct].setColorHSV(0, 0, 255, 255);
    mode[4].section[sct].setColorHSV(1, 96, 255, 255);
    mode[4].section[sct].setColorHSV(2, 160, 255, 255);
    mode[4].section[sct].setNumColors(3);
    mode[4].section[sct].setTiming(1, 1, 20 * sct);
  }
  //
}

