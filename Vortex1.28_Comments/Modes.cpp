#include "Modes.h"

Modes::Modes() {
  TDP section[4];
  TDP settings[4];
  TDP target;
}

void Modes::setNumOfSections(int num) {
  numOfSections = num;
}

void Modes::setLayout(int lay) {
  layoutNum = lay;
  if (layoutNum == 0) {
    section[0].setSection(ring0, 8);
    section[1].setSection(ring1, 8);
    section[2].setSection(ring2, 8);
    section[3].setSection(ring3, 4);
    numOfSections = 4;
  }
  if (layoutNum == 1) {
    section[0].setSection(side1, 12);
    section[1].setSection(side2, 12);
    section[2].setSection(ring3, 4);
    numOfSections = 3;
  }
  if (layoutNum == 2) {
    section[0].setSection(quarter1, 7);
    section[1].setSection(quarter2, 7);
    section[2].setSection(quarter3, 7);
    section[3].setSection(quarter4, 7);
    numOfSections = 4;
  }
  if (layoutNum == 3) {
    section[0].setSection(all, 28);
    numOfSections = 1;
  }
}

void Modes::rotateSections() {
  int temp = animate[0];
  for (int i = 0; i < numOfSections; i++)animate[i] = animate[i + 1];
  animate[numOfSections - 1] = temp;
  if (layoutNum == 0) {
    section[animate[0]].setSection(ring0, 8);
    section[animate[1]].setSection(ring1, 8);
    section[animate[2]].setSection(ring2, 8);
    section[animate[3]].setSection(ring3, 4);
  }
  if (layoutNum == 1) {
    section[animate[0]].setSection(side1, 12);
    section[animate[1]].setSection(side2, 12);
    section[animate[2]].setSection(ring3, 4);
  }
  if (layoutNum == 2) {
    section[animate[0]].setSection(quarter1, 7);
    section[animate[1]].setSection(quarter2, 7);
    section[animate[2]].setSection(quarter3, 7);
    section[animate[3]].setSection(quarter4, 7);
  }
  if (layoutNum == 3) {
    section[animate[0]].setSection(all, 28);
    section[animate[1]].setSection(none, 0);
    section[animate[2]].setSection(none, 0);
    section[animate[3]].setSection(none, 0);
  }
}

void Modes::restoreOrder() {
  for (int i = 0; i < 4; i++)animate[i] = i;
}

void Modes::printMode(int num) {
  Serial.print("Mode "), Serial.println(num);
  for (int p = 0; p < numOfSections; p++) {
    Serial.print("Section "), Serial.print(p), Serial.print(": ");
    Serial.print(section[p].onTime), Serial.print(",");
    Serial.print(section[p].offTime), Serial.print(",");
    Serial.println(section[p].breakTime);
    for (int slot = 0; slot < section[p].numColors; slot ++) {
      Serial.print(section[p].hue[slot]), Serial.print(" ");
      Serial.print(section[p].sat[slot]), Serial.print(" ");
      Serial.println(section[p].val[slot]);
    }
  }
  Serial.print("Layout"), Serial.print(": ");
  Serial.print(layoutNum), Serial.print(", ");
  Serial.print(targetAnimation), Serial.println();
  Serial.println();
}

ColorBank::ColorBank() {

}

void ColorBank::saveColor(int set, int slot, int hue, int sat, int val) {
  colorBankHue[set][slot] = hue;
  colorBankSat[set][slot] = sat;
  colorBankVal[set][slot] = val;
  if (slot == colorBankSetSize[set])colorBankSetSize[set]++;
}

void ColorBank::deleteColor(int set) {
  if (colorBankSetSize[set] > 1)colorBankSetSize[set]--;
}

void ColorBank::printColors() {
  for (int set = 0; set < 16; set++) {
    Serial.print("set "), Serial.println(set);
    for (int slot = 0; slot < colorBankSetSize[set]; slot ++) {
      Serial.print(colorBankHue[set][slot]), Serial.print(" ");
      Serial.print(colorBankSat[set][slot]), Serial.print(" ");
      Serial.println(colorBankVal[set][slot]);
    }
    Serial.println();
  }
}


