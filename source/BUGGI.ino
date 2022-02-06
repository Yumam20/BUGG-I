#include <Wire.h>

#define BTN_ADDR 0x6F
#define LCD_ADDR 0x72

int dsec = 0;
int sector = 0;
int sectorTime = 0;
int sectorTimes[] = {0,0,0,0,0,0,0};
int benchmark = 0;
bool timerCont = false;
bool btnClicked = false;
int flash = 0;
int flashRed = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  clearLCD();
}

void loop() {
  delay(100); 
  if (timerCont)
    dsec++;

  if (sector == 7 && dsec < 1200)
    flash++;

  sectorTime = dsec - benchmark;

  clearLCD();
  if (sector != 7) // sector 7 is final report
    displayTime(dsec, "Overall: ");

  // for displaying sector time
  if (sector == 0) // roll starting
    displayMsg("Press to start");
  else if (1 <= sector && sector <= 6)
    displayHillTime(sectorTime, sector);
  else if (sector == 7) { // roll ending
    displayAllTime();
    if (0 < flash && flash <= 31){
      displayMsg("O");
      if (flashRed)
        setRGBBacklight(LCD_ADDR, 0xFF, 0x00, 0x00);
      else
        setRGBBacklight(LCD_ADDR, 0xFF, 0xFF, 0xFF);
        
      if (flash % 5 == 1)
        flashRed = !flashRed;
    }
  } else { // sector = 8, restarting
    clearLCD();
  }

  bool btnState = getBtnState(BTN_ADDR);
    
  if (btnState && !btnClicked) { // button is clicked
    if (sector == 0) {
      startTimer();
      sector++;
    } else if (1 <= sector && sector <= 5) {
      sectorTimes[sector] = sectorTime;
      benchmark = dsec;
      sectorTime = 0;
      sector++;
    } else if (sector == 6) {
      sectorTimes[sector] = sectorTime;
      stopTimer();
      sector++;
    } else { // sector = 7, reset
      sector = 0;
      dsec = 0;
      sector = 0;
      sectorTime = 0;
      flash = 0;
      flashRed = false;
      for (int i = 0; i < 7; i++)
        sectorTimes[i] = 0; 
      benchmark = 0;
      timerCont = false;
      btnClicked = false;
    }
      
  }

  if (btnState)  // to differentiate button clicked and pressed
    btnClicked = true;
  else
    btnClicked = false;
}

void startTimer() {
  timerCont = true;
}

void stopTimer() {
  timerCont = false;
}

void clearLCD() {
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(0x7c);
  Wire.write(0x2D);
  Wire.endTransmission();
}

void displayMsg(String msg) {
  Wire.beginTransmission(LCD_ADDR);
  Wire.println(msg);
  Wire.endTransmission();
}

void displayTime(int dsec, String msg) {
  Wire.beginTransmission(LCD_ADDR);
  Wire.print(msg);
  Wire.print(dsec / 10) ;
  Wire.print(".");
  Wire.print(dsec % 10);
  Wire.println();
  Wire.endTransmission();
}

void displayHillTime(int dsec, int hill) {
  Wire.beginTransmission(LCD_ADDR);
  if (hill < 3){
     Wire.print("Hill ");
     Wire.print(hill);
     Wire.print(": ");
  } else if (hill == 3) {
    Wire.print("Free Roll: ");
  } else { // hill >= 4 so shift down
    Wire.print("Hill");
    Wire.print(" ");
    Wire.print(hill - 1);
    Wire.print(": ");
  }

  
  Wire.print(dsec / 10) ;
  Wire.print(".");
  Wire.print(dsec % 10);
  Wire.println();
  Wire.endTransmission();
}

void displayAllTime() {
  // display overall first
  Wire.beginTransmission(LCD_ADDR);
  Wire.print("O: ");
  Wire.print(dsec / 10) ;
  Wire.print(".");
  Wire.print(dsec % 10);
  Wire.print(",");

  Wire.print(sectorTimes[1] / 10) ;
  Wire.print(",");

  Wire.print(sectorTimes[2] / 10) ;

  Wire.println();

  Wire.print("FR: ") ;
  Wire.print(sectorTimes[3] / 10);
  Wire.print(",");

  Wire.print(sectorTimes[4] / 10) ;
  Wire.print(",");

  Wire.print(sectorTimes[5] / 10) ;
  Wire.print(",");

  Wire.print(sectorTimes[6] / 10) ;
  
  Wire.println();
  
  Wire.endTransmission();    
}

bool getBtnState(byte address) {
  // write your code here! (part 3)
  Wire.beginTransmission(address);
  Wire.write(0x03);
  Wire.endTransmission();
  
  Wire.requestFrom(address, 1);
  int register_data = Wire.read();
  int state = register_data & 0x04;
  return state;
}

void setRGBBacklight(byte address, byte r, byte g, byte b) {
  Wire.beginTransmission(address);
  Wire.write(0x7c);
  Wire.write(0x2B);
  Wire.write(r);
  Wire.write(g);
  Wire.write(b);
  Wire.endTransmission();
  return;
 }
