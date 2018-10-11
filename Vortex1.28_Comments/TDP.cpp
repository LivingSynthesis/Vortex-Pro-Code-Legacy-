#include "TDP.h"

TDP::TDP() {
  bool on;
  bool endOfSet;
  int hue[8];
  int sat[8];
  int val[8];
  int currentColor = 0;
  int numColors = 1;
  int *ledAddresses;
  int sectionSize;
  int frame;
  unsigned long prevTime;
  unsigned long mainClock;
  unsigned long onTime;
  unsigned long offTime;
  unsigned long breakTime;
  unsigned long duration;
}

void TDP::setColorHSV(int colorNum, int newHue, int newSat, int newVal) {
  hue[colorNum] = newHue;
  sat[colorNum] = newSat;
  val[colorNum] = newVal;
}

void TDP::setNumColors(int num) {
  numColors = num;
}

void TDP::setTiming(unsigned long on, unsigned long off, unsigned long setBreak) {
  onTime = on;
  offTime = off;
  breakTime = setBreak;
}

void TDP::storePattern() {
  revertOnTime = onTime;
  revertOffTime = offTime;
  revertBreakTime = breakTime;
}

void TDP::setSection(int target[], int sectSize) {
  ledNum = target;
  sectionSize = sectSize;
}

void TDP::setColorSet(int newHue[], int newSat[], int newVal[]) {
  revertNumColors = numColors;
  for (int c = 0; c < 8; c++) {
    revertHue[c] = hue [c];
    revertSat[c] = sat [c];
    revertVal[c] = val [c];
    hue[c] = newHue[c];
    sat[c] = newSat[c];
    val[c] = newVal[c];
  }
}

void TDP::revertColorSet() {
  numColors = revertNumColors;
  for (int c = 0; c < 8; c++) {
    hue[c] = revertHue[c];
    sat[c] = revertSat[c];
    val[c] = revertVal[c];
  }
}

void TDP::revertPattern() {
  onTime = revertOnTime;
  offTime = revertOffTime;
  breakTime = revertBreakTime;
}


