#ifndef Modes_h
#define Modes_h

#include "Arduino.h"
#include "TDP.h"


class Modes {
  private:
  
    int all[28] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
    int ring0 [8] = {0, 6, 7, 13, 14, 20, 21, 27};
    int ring1 [8] = {1, 5, 8, 12, 15, 19, 22, 26};
    int ring2 [8] = {2, 4, 9, 11, 16, 18, 23, 25};
    int ring3 [4] = {3, 10, 17, 24};
    int quarter1[7] = {0, 1, 2, 3, 4, 5, 6};
    int quarter2[7] = {7, 8, 9, 10, 11, 12, 13};
    int quarter3[7] = {14, 15, 16, 17, 18, 19, 20};
    int quarter4[7] = {21, 22, 23, 24, 25, 26, 27};
    int side1 [12] = {0, 1, 2, 11, 12, 13, 14, 15, 16, 25, 26, 27};
    int side2 [12] = {4, 5, 6, 7, 8, 9, 18, 19, 20, 21, 22, 23};
    int none[1]= {0};


  public:
    Modes();
    TDP section[4];
    TDP settings[4];
    TDP target;
    int numOfSections = 4;
    int menuNum;
    int animate[4] = {0, 1, 2, 3};
    int targetAnimation = 0;
    int layoutNum;
    void setNumOfSections(int num);
    void setLayout(int lay);
    void rotateSections();
    void restoreOrder();
    void printMode(int num);
};

class ColorBank {
  private:

  public:
    ColorBank();
    int colorBankHue[16][8];
    int colorBankSat[16][8];
    int colorBankVal[16][8];
    int colorBankSetSize[16];
    int currentBankColor[16];
    int blankBlink[8];
    void saveColor(int set, int slot, int hue, int sat, int val);
    void deleteColor(int set);
    void printColors();
};

#endif
