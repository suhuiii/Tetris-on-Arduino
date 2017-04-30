
#include "LedControl.h"
#define DEVICES 2
#define ROWS 16

//connections
int CS = 14;
int DIN = 15;
int CLK = 16;
int buttonRotate = 6 ;
int buttonLeft =  7;
int buttonDown =  8;
int buttonRight = 9;

int lastRotateState = HIGH;
int lastRightState = HIGH;
int lastLeftState = HIGH;


//game controls
int x = 0;
int y = 0;
int rotation = 0;
int delayCounter = 0;
int removed = 0;

LedControl matrixLED = LedControl(DIN, CLK, CS, DEVICES); // LedControl(dataPin,clockPin,csPin,numDevices)

byte matrix[ROWS] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

word shapes[7][4] = {{0xCC00, 0xCC00, 0xCC00, 0xCC00}, // O
  {0x4444, 0x0F00, 0x2222, 0x0F00}, // I
  {0x4E00, 0x4640, 0x0E40, 0x4C40}, // T
  {0x4460, 0x0E80, 0xC440, 0x2E00}, // L
  {0x44C0, 0x8E00, 0x6440, 0x0E20}, // J
  {0x4C80, 0xC600, 0x4C80, 0xC600}, // Z
  {0x8C40, 0x6C00, 0x8C40, 0x6C00} // S
}; 

byte digit[10][5] = {
  {2, 5, 5, 5, 2},  // 0
  {1, 1, 1, 1, 1},  // 1
  {6, 1, 2, 4, 7},  // 2
  {7, 1, 2, 1, 6},  // 3
  {4, 5, 7, 1, 1},  // 4
  {7, 4, 7, 1, 6},  // 5
  {3, 4, 7, 5, 2},  // 6
  {7, 1, 2, 4, 4},  // 7
  {7, 5, 2, 5, 7},  // 8
  {2, 5, 7, 1, 6},  // 9
};

word* currentShape;

boolean gameEnd = false;

void setup() {
  randomSeed(analogRead(0));
  
  for (int index = 0; index < matrixLED.getDeviceCount(); index++) {
    matrixLED.shutdown(index, false);
  }
  matrixLED.setIntensity(0, 6);
  matrixLED.setIntensity(1, 6);

  pinMode(17, OUTPUT);
  pinMode(buttonRotate, INPUT_PULLUP); // Rotate
  pinMode(buttonRight, INPUT_PULLUP);  // Right
  pinMode(buttonLeft, INPUT_PULLUP);   // Left
  pinMode(buttonDown, INPUT_PULLUP);   // Down

  newGame();

}

void loop() {
  if(!gameEnd){
   
    byte dispMatrix[ROWS];
  
    addShapeToMatrix(dispMatrix, currentShape[rotation], x, y);
    renderMatrix(dispMatrix);
    
    int btn = resolveButtonPushes();

    switch(btn){
      case 1:       rotation = rotate(rotation); break;
      case 2:       moveLeft(); break;
      case 3:       moveRight(); break;
      case 4:       moveDown(); break;
    }

    if (delayCounter++ >= 50 - removed) {
      if (collideNext(currentShape[rotation], x, y + 1) || y + 4 - bottomOffset(currentShape[rotation]) > 15) {
        if( y < 0){
          gameOver();
          return;
        }
        addShapeToMatrix(matrix, currentShape[rotation], x, y);
        spawnRandomShape();
    }
      removed += removeFullLines();
      y++;
      delayCounter = 0;
    }
    
  }else{   
    if(resolveButtonPushes() != 0){
      newGame();
    }
  }

}

//game play methods
void newGame(){
  gameEnd = false;
  rotation = 0;
  delayCounter = 0;
  removed = 0;

  //clear game board
  for(int i = 0; i < ROWS; i++){
    matrix[i] = 0x0;
  }
  
  spawnRandomShape();
}

void gameOver(){

  //waterfall
  for (int i = 0; i < ROWS; i++) {
    if (i < 8) {
      matrixLED.setRow(0, i, 0xFF);
    } else {
      matrixLED.setRow(1, i - 8, 0xFF);
    }
    delay(75);
  }
  
  for (int i = 0; i < ROWS; i++) {
    if (i < 8) {
      matrixLED.setRow(0, i, 0x00);
    } else {
      matrixLED.setRow(1, i - 8, 0x00);
    }
    delay(75);
  }

  //sad face
  matrixLED.setRow(0, 2, 0x24);
  matrixLED.setRow(0, 4, 0x18);
  matrixLED.setRow(0, 5, 0x24);


  //score
  int tens = removed / 10;
  int ones = removed % 10;

  for(int i = 0; i < 5; i++){
    matrixLED.setRow(1, i + 2, (digit[tens][i] << 5 | (digit[ones][i])));
  }

  gameEnd = true;
 
}

void spawnRandomShape(){
      currentShape = shapes[random(7)];
      rotation =  random(3);
      x = 2 + (rightOffset(currentShape[rotation]) / 2);
      y = -4 + bottomOffset(currentShape[rotation]);
}


int removeFullLines(){
  int removed = 0;

  for(int i = 0; i< ROWS; i++){
    if(matrix[i] == 0xff){
      shiftLinesDownFrom(i);
      removed++;
    }
  }
  return removed;
}

void shiftLinesDownFrom(int row){
  for(int i = row; i > 0; i--){
    matrix[i] = matrix[i - 1];
  }
}

boolean collideNext(word shape, int testX, int testY) {
   for(int i = 3; i >= 0 ; i--){
    if(testY + i < 0)
      continue;

    if((matrix[testY + i] & getRowValue(shape, i, testX)) > 0){
      return true;
    }
  }
  return false;
}

//navigation methods
void moveLeft(){
  int spaceLeft = x + leftOffset(currentShape[rotation]) - 1;
   if( spaceLeft >= 0 && !collideNext(currentShape[rotation], x - 1, y)){
     x--;
   }
}

void moveRight(){
  if(x - rightOffset(currentShape[rotation]) + 4 <= 7 && !collideNext(currentShape[rotation], x + 1, y)){
    x++;  
  }
}

void moveDown(){
  if(!collideNext(currentShape[rotation], x, y + 1) && y + 4 - bottomOffset(currentShape[rotation]) <= 15 && !(y < 0)){
    y++;
  }
  
}

int rotate(int currRotation){
  int newRotate = currRotation + 1;
  if(newRotate > 3){
    newRotate = 0;
  }
  
  if(x - rightOffset(currentShape[newRotate]) + 4 > 8 || collideNext(currentShape[newRotate], x, y))
    return currRotation;
  
  return newRotate; 
}

int resolveButtonPushes(){
  int rotateState = digitalRead(buttonRotate);
  
  if((rotateState != lastRotateState)){
    lastRotateState = rotateState;

    if(rotateState == LOW){
      digitalWrite(17, LOW);
      return 1;
    }
  }
  
  int leftState = digitalRead(buttonLeft);
  if((leftState != lastLeftState)){
    lastLeftState = leftState;
    
    if(leftState == LOW){
      digitalWrite(17, LOW);
      return 2;
    }
  }

  int rightState = digitalRead(buttonRight);
  if((rightState != lastRightState)){
    lastRightState = rightState;
    
    if(rightState == LOW){
      digitalWrite(17, LOW);
      return 3;
    }
  }

  int downState = digitalRead(buttonDown);
  if(downState == LOW){
      digitalWrite(17, LOW);
      return 4;
  }
  
  digitalWrite(17, HIGH);
  return 0;
}

//display methods

void renderMatrix(byte* dispMatrix) {
  for (int i = 0; i < ROWS; i++) {
    if (i < 8) {
      matrixLED.setRow(0, i, dispMatrix[i]);
    } else {
      matrixLED.setRow(1, i - 8, dispMatrix[i]);
    }
  }
}

void addShapeToMatrix(byte *tempMatrix, word shape, int x, int y) {
  copyMatrix(matrix, tempMatrix);

  for(int i = 0; i < 4 ; i++){
    if(y + i < 0)
      continue;
      
    tempMatrix[y + i] = matrix[y + i] | getRowValue(shape, i, x);
  }
  return;
}

void copyMatrix(byte *original, byte *destination) {

  for (int i = 0; i < ROWS; i++) {
    destination[i] = original[i];
  }

  return;
}

//helper methods
int bottomOffset(word shape) {
  for (int i = 3; i >= 0; i--) {
    if (getRowValue(shape, i, x) > 0) {
      return 3 - i;
    }
  }
}

int leftOffset(word shape){
  for(int offset = 3; offset >= 0; offset--){
    for(int j = 0; j < 4 ; j++){
      byte row = (shape >> (j * 4)) & 0xf;
      if(bitRead(row, offset) == HIGH){
        return 3 - offset;
      }
    }
  }
  return -1;
}

int rightOffset(word shape){
  for(int offset = 0; offset < 3; offset++){
    for(int j = 0; j < 4 ; j++){
      byte row = (shape >> (j * 4)) & 0xf;
      if(bitRead(row, offset) == HIGH){
        return offset;
      }
    }
  }
  return -1;
}

byte getRowValue(word shape, int row, int testX){
  int shift = (3 - row) * 4;
  byte rowValue = (shape >> shift) & 0xf;
  int loffset = leftOffset(shape);

  return rowValue << (4 + loffset) >> (testX + loffset);
}

