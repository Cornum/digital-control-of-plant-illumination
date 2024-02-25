#include <Wire.h>                 //pro I2C
#include <LiquidCrystal_I2C.h>    //pro display


////Menu
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
////\Menu

const int  arrayLen = 10;


void setup() {
  Serial.begin(115200);         // rychlost seriove linky

////Menu

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

////\Menu
  Serial.println(arrayLen);

}



void loop() {
int array[] = {2315, 1566, 152, 88, 432, 489, 4897, 123, 321, 222};

////Menu
  stavCLK = digitalRead(pinCLK);
  stavSW = digitalRead(pinSW);

  //Otoceni enkoderem
  if (stavCLK != stavPred) {                  //nastaveni parametru (udelat vyber z menu)
    if (digitalRead(pinDT) != stavCLK){localParametrMenu++;}
    else{localParametrMenu--;}
    drwMenu();
  }
  stavPred = stavCLK;

  // Handle button press (with debouncing)
  int reading = digitalRead(pinSW);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {  //control double click
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        handleButtonClick(array);
        drwMenu();
      }
    }
  }


  lastButtonState = reading;
////\Menu

}


void drwMenu() {                                       //LCD - vypis Menu
  lcd.clear();
  switch(globalParametrMenu){
    case 1:{ //settings menu
      lcd.setCursor(0,0);
      lcd.print("Settings menu: ");
      lcd.setCursor(0,3);
      if(localParametrMenu % 3 == 0){
        lcd.print("Time ->");
      }else if(localParametrMenu % 3 == 1){
        lcd.print("<- Type ->");
      }else{
        lcd.print("<- Done");
      }
      break;}
    case 2:{ //time measurments menu
      lcd.setCursor(0,0);
      lcd.print("Time settings menu: ");
      lcd.setCursor(0, 1);
      lcd.print("Set time [min]:");
      lcd.setCursor(0, 2);
      if (localParametrMenu < 0){
        localParametrMenu = 0;
        timeConst = localParametrMenu;
      }
      else{
        timeConst = localParametrMenu;
      }
      lcd.print(timeConst);
      lcd.setCursor(16, 3);
      lcd.print("Done");
      break;} 
    case 3:{ //type measurments menu
      lcd.setCursor(0,0);
      lcd.print("Type settings menu: ");
      lcd.setCursor(0,3);
      if(localParametrMenu % 3 == 0){
        lcd.print("Block type ->");
      }else if(localParametrMenu % 3 == 1){
        lcd.print("<- Sinus type ->");
      }else{
        lcd.print("<- Day simulation");
      }
      break;} 
    case 10:{  //setted program
      lcd.setCursor(0,0);
      lcd.print("Menu: ");
      lcd.setCursor(0,3);
      if(localParametrMenu % 4 == 0){
        lcd.print("Start final ->");  //setted program
      } else if (localParametrMenu % 4 == 1){
        lcd.print("<- Settings ->");  //set from 0
      } else if (localParametrMenu % 4 == 2){
        lcd.print("<- Save ->");  //Rework
      } else{
        lcd.print("<- EXIT");
      }

      break;
    }
    case -10:{  //saving data
      lcd.setCursor(0,0);
      lcd.print("Saving: ");
      lcd.setCursor(0,3);
      if(localParametrMenu % 2 == 0){
        lcd.print("START ->");
      } else{
        lcd.print("<- STOP");
      }
      break;
    }
    case -1:{ //default program
      lcd.setCursor(0,0);
      lcd.print("Measurment: ");
      lcd.setCursor(0,1);
      lcd.print(arrayLen);
      lcd.setCursor(10,1);
      lcd.print("values");
      lcd.setCursor(0,3);

      if(localParametrMenu % 2 == 0){
        lcd.print("OK ->");
      } else{
        lcd.print("<- STOP");
      }
      break;}

    default:{  //start menu
      lcd.setCursor(0,0);
      lcd.print("Menu: ");
      lcd.setCursor(0,3);
      if(localParametrMenu % 3 == 0){
        lcd.print("Start ->");  //default program from MatLAB
      } else if (localParametrMenu % 3 == 1){
        lcd.print("<- Settings ->");
      } else{
        lcd.print("<- Save");
      }
      break;
    }
  }
}                                                     //LCD - vypis Menu


void handleButtonClick(int* forSaving){
  switch(globalParametrMenu){
    case 1:{  //Settings
      if(localParametrMenu % 3 == 0){
        globalParametrMenu = 2;}  //transfer to Time
      else if(localParametrMenu % 3 == 1){
        globalParametrMenu = 3;} //transfer to Type
      else{
        globalParametrMenu = 10;
      } //transfer to Final Menu
      localParametrMenu = 0;
      break;
    }
    case 2:{ //Time
      localParametrMenu = 0;
      globalParametrMenu = 1; //transfer to Settings Menu
      break;
    }
    case 3:{ //Type
      if(localParametrMenu % 4 == 0){ //transfer to settings with flag
        globalParametrMenu = 1;
      }
      else if(localParametrMenu % 4 == 1){ //transfer to settings with flag
        globalParametrMenu = 1;
      }
      else{ //transfer to settings with flag
        globalParametrMenu = 1;
      }
      localParametrMenu = 0;
      break;
    }
    case 10:{ //Final Menu
      if (localParametrMenu % 4 == 0){
        globalParametrMenu = -2;} //start setted program
      else if (localParametrMenu % 4 == 1){
        globalParametrMenu = 1;} //transfer to settings menu
      else if (localParametrMenu % 4 == 2){
        globalParametrMenu = -10;} //transfer to settings menu
      else{
        globalParametrMenu = 0;}
      localParametrMenu = 0;
      break;
    }
    case -10:{  //saving data
      if (localParametrMenu % 2 == 0){
        lcd.setCursor(0,3);
        lcd.print("Proceeding... ->");
        if(Serial.availableForWrite()){
          Serial.println(0);
          for (int i = 0; i<arrayLen; i++){
            Serial.println(forSaving[i]); // Send value into Serial port
            delay(2000);
            }
        }
      } //Do nothing
      else{
        Serial.println(-1);
        globalParametrMenu = 0;} //transfer to start menu
      localParametrMenu = 0;
      break;
    }
    case -1:{ //default program
      if (localParametrMenu % 2 == 0){} //Do nothing
      else{
        globalParametrMenu = 0;} //transfer to start menu
      localParametrMenu = 0;
      break;
    }
    default:{
      if (localParametrMenu % 3 == 0){
        globalParametrMenu = -1;} //start default
      else if (localParametrMenu % 3 == 1){
        globalParametrMenu = 1;} //transfer to settings menu
      else{
        globalParametrMenu = -10;
      }  //start saving data
      localParametrMenu = 0;
      break;
    }
  }
}

