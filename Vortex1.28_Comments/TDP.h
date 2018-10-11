#ifndef TDP_h
#define TDP_h

#include "Arduino.h"

class TDP {
  private:

  public:
    TDP();
    bool on;
    bool endOfSet;
    int hue[8];
    int sat[8];
    int val[8];
    int currentColor;
    int numColors;
    int sectionSize;
    int *ledNum;
    int frame;
    int revertHue[8];
    int revertSat[8];
    int revertVal[8];
    int revertNumColors;
    unsigned long onTime;
    unsigned long offTime;
    unsigned long breakTime;
    unsigned long duration;
    unsigned long prevTime;
    unsigned long mainClock;
    unsigned long revertOnTime;
    unsigned long revertOffTime;
    unsigned long revertBreakTime;
    void setColorHSV(int colorNum, int newHue, int newSat, int newVal);
    void setNumColors(int num);
    void setTiming(unsigned long on, unsigned long off, unsigned long setBreak);
    void storePattern();
    void setSection(int target[], int sectSize);
    void setColorSet(int newHue[], int newSat[], int newVal[]);
    void revertColorSet();
    void revertPattern();

};

#endif
