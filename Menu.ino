#include <Wire.h>                 //pro I2C
#include <LiquidCrystal_I2C.h>    //pro display

// Define debounce time in milliseconds
#define DEBOUNCE_TIME 50

// Variables for button debounce
unsigned long lastDebounceTime = 0;
bool buttonState = HIGH;
bool lastButtonState = HIGH;


const int pinCLK = 2;           // otocny ovladac (enkoder)
const int pinDT  = 3;           //
const int pinSW  = 4;           // otocny ovladac (enkoder)

LiquidCrystal_I2C lcd(0x27, 20, 4);     // LCD display pres I2C

int localParametrMenu = 0;
int globalParametrMenu = 0;
int timeConst = 0;

int res = 12; 
int poziceEnkod = 0;                // pro enkoder
int stavPred;                       
int stavCLK;                        
int stavSW;   

void setup() {
  Serial.begin(115200);         // rychlost seriove linky

  res =(int) pow(2,res);        // rozliseni 2^res --- tj. 4096 hodnot   


  //iniciace LCD displae + hlaska
  lcd.init();  
  lcd.setCursor(3,0);
  lcd.print("Hello, starting..");
  for (int i=0 ; i<5; i++)
  {
    lcd.noBacklight();    
    delay(50);
    lcd.backlight();
    delay(50);    
  }  
  //iniciace LCD displae + hlaska

  pinMode(pinCLK, INPUT);               // Otocny ovladac
  pinMode(pinDT, INPUT);                // Otocny ovladac
  pinMode(pinSW, INPUT_PULLUP);         // Otocny ovladac - tlacitko  
  stavPred = digitalRead(pinCLK);       // Pocatecni stav otocneho ovladace
  drwMenu();                             // Pocatecni vypsani displaye
}



void loop() {
  stavCLK = digitalRead(pinCLK);
  stavSW = digitalRead(pinSW);

  //Otoceni enkoderem
  if (stavCLK != stavPred) {                  //nastaveni parametru (udelat vyber z menu)
    if (digitalRead(pinDT) != stavCLK){localParametrMenu++;}
    else{localParametrMenu--;}
    drwMenu();
  }
  stavPred = stavCLK;
//

// Handle button press (with debouncing)
  int reading = digitalRead(pinSW);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        handleButtonClick();
        drwMenu();
      }
    }
  }

  lastButtonState = reading;
}


void drwMenu() {                                       //LCD - vypis Menu
  lcd.clear();
  switch(globalParametrMenu){
    case 1:{
      if(localParametrMenu % 3 == 0){
        lcd.setCursor(0,0);
        lcd.print("Settings menu: ");
        lcd.setCursor(0,3);
        lcd.print("Time ->");
      }else if(localParametrMenu % 3 == 1){
        lcd.setCursor(0,0);
        lcd.print("Settings menu: ");
        lcd.setCursor(0,3);
        lcd.print("Type ->");
      }else{
        lcd.setCursor(0,0);
        lcd.print("Settings menu: ");
        lcd.setCursor(0,3);
        lcd.print("Done ->");
      }
      break;} //settings menu
    case 2:{
      lcd.setCursor(0,0);
      lcd.print("Time settings menu: ");
      lcd.setCursor(0, 2);
      lcd.print("Set time [min]:");
      lcd.setCursor(0, 3);

      if (localParametrMenu < 0){
        localParametrMenu = 0;
        timeConst = localParametrMenu;
      }
      else{
        timeConst = localParametrMenu;
      }
      lcd.print(timeConst);
      break;} //time measurments menu
    case 3:{
        if(localParametrMenu % 4 == 0){
        lcd.setCursor(0,0);
        lcd.print("Type settings menu: ");
        lcd.setCursor(0,3);
        lcd.print("Block type ->");
      }else if(localParametrMenu % 4 == 1){
        lcd.setCursor(0,0);
        lcd.print("Type settings menu: ");
        lcd.setCursor(0,3);
        lcd.print("Sinus type ->");
      }else if(localParametrMenu % 4 == 2){
        lcd.setCursor(0,0);
        lcd.print("Type settings menu: ");
        lcd.setCursor(0,3);
        lcd.print("Day simulation ->");
      }else{
        lcd.setCursor(0,0);
        lcd.print("Type settings menu: ");
        lcd.setCursor(0,3);
        lcd.print("Done ->");
      }
      break;} //type measurments menu
    case 10:{
      if(localParametrMenu % 2 == 0){
        lcd.setCursor(0,0);
        lcd.print("Menu: ");
        lcd.setCursor(0,3);
        lcd.print("Start final ->");  //default program from MatLAB
      } else{
        lcd.setCursor(0,0);
        lcd.print("Menu: ");
        lcd.setCursor(0,3);
        lcd.print("<-Settings");
      }
      break;
    }
    case -2:{
      lcd.setCursor(0,0);
        lcd.print("Measurment: ");
        lcd.setCursor(0,1);
        lcd.print("Proceeding...");
      if(localParametrMenu % 2 == 0){
        lcd.setCursor(0,3);
        lcd.print("OK. ->");
      } else{
        lcd.setCursor(0,3);
        lcd.print("<-STOP");
      }
      break;
    }
    case -1:{
      //DO program
      break;}
    default:{  //start menu
      if(localParametrMenu % 2 == 0){
        lcd.setCursor(0,0);
        lcd.print("Menu: ");
        lcd.setCursor(0,3);
        lcd.print("Start ->");  //default program from MatLAB
      } else{
        lcd.setCursor(0,0);
        lcd.print("Menu: ");
        lcd.setCursor(0,3);
        lcd.print("<-Settings");
      }
      break;
    }
  }
}                                                     //LCD - vypis Menu


void handleButtonClick(){
  switch(globalParametrMenu){
    case 1:{  //Settings
      if(localParametrMenu % 3 == 0){
        localParametrMenu = 0;
        globalParametrMenu = 2;}  //transfer to Time
      else if(localParametrMenu % 3 == 1){
        localParametrMenu = 0;
        globalParametrMenu = 3;} //transfer to Type
      else{
        localParametrMenu = 0;
        globalParametrMenu = 10;
      } //transfer to Final Menu
      break;
    }
    case 2:{ //Time
      localParametrMenu = 0;
      globalParametrMenu = 1; //transfer to Settings Menu
      break;
    }
    case 3:{ //Type
      if(localParametrMenu % 4 == 0){ //transfer to settings with flag
        localParametrMenu = 0;
        globalParametrMenu = 1;
      }
      else if(localParametrMenu % 4 == 1){ //transfer to settings with flag
        localParametrMenu = 0;
        globalParametrMenu = 1;
      }
      else if(localParametrMenu % 4 == 2){ //transfer to settings with flag
        localParametrMenu = 0;
        globalParametrMenu = 1;
      }
      else{
        localParametrMenu = 0;
        globalParametrMenu = 1;} //transfer to Settings Menu
      break;
    }
    case 10:{ //Final Menu
      if (localParametrMenu % 2 == 0){
        localParametrMenu = 0;
        globalParametrMenu = -2;} //start setted program
      else{
        localParametrMenu = 0;
        globalParametrMenu = 1;} //transfer to settings menu
      break;
    }
    case -2:{
      if (localParametrMenu % 2 == 0){
        localParametrMenu = 0;} //Do nothing
      else{
        localParametrMenu = 0;
        globalParametrMenu = 0;} //transfer to start menu
      break;
    }
    default:{
      if (localParametrMenu % 2 == 0){
        localParametrMenu = 0;
        globalParametrMenu = -1;} //start default
      else{
        localParametrMenu = 0;
        globalParametrMenu = 1;} //transfer to settings menu
      break;
    }
  }
}