#include <Wire.h>                 //for I2C
#include <LiquidCrystal_I2C.h>    //for LSD


////Menu Global variables
  // Define debounce time in milliseconds
  #define DEBOUNCE_TIME 50

  // Variables for button debounce
  unsigned long lastDebounceTime = 0;
  bool buttonState = HIGH;
  bool lastButtonState = HIGH;

  // Encoder const
  const int pinCLK = 2;           
  const int pinDT  = 3;           
  const int pinSW  = 4;           

  //Menu's global
  int localParametrMenu = 0;
  int globalParametrMenu = 0;
  int timeConst = 0;

  // LCD display I2C
  LiquidCrystal_I2C lcd(0x27, 20, 4);     

  // Screen resolution
  int res = 12;   

  // Encoder variables
  int poziceEnkod = 0;                
  int stavPred;                       
  int stavCLK;                        
  int stavSW;   
////\Menu

///MatLab Connection Global variables
  int channel;
  int GrowthTime;
  int typeFunction;
  int timeGrowthStartHours;
  int timeGrowthStartMinutes;
  int timeDeclineStartHours;
  int timeDeclineStartMinutes;
  int maxIntensity;
///\MatLab Connection


void setup() {
  Serial.begin(115200);         // Serial speed

////Menu

  res =(int) pow(2,res);        // Resolution 2^res --- 4096 values   

  //Initiation LCD
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
  //\Initiation LCD

  //Encoder initiation
  pinMode(pinCLK, INPUT);               // Encoder
  pinMode(pinDT, INPUT);                // Encoder
  pinMode(pinSW, INPUT_PULLUP);         // Encoder button
  stavPred = digitalRead(pinCLK);       // Start encoder
  drawMenu();                             // Start menu
  //\Encoder initiation
////\Menu

////aA
  //Read sample from MatLAB for growth function
  
////\aA

}



void loop() {

////Menu
  stavCLK = digitalRead(pinCLK);
  stavSW = digitalRead(pinSW);
int array[] = {1,2,3,-1};
  //Encoder rotation
  if (stavCLK != stavPred) {
    if (digitalRead(pinDT) != stavCLK){localParametrMenu++;}
    else{localParametrMenu--;}
    drawMenu();
  }
  stavPred = stavCLK;
  //\Encoder rotation

  // Handle button press (with debouncing)
  int reading = digitalRead(pinSW);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        handleButtonClick(array);
        drawMenu();
      }
    }
  }


  lastButtonState = reading;
  //\Handle button press (with debouncing)
////\Menu

}


void drawMenu() {                                       //LCD - draw Menu
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
      //lcd.print(arrayLen);
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
}                                                     //\LCD - draw Menu


void handleButtonClick(int* forSaving){  //Button click handle
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
          for (int i = 0; i < forSaving[i] != -1; i++){
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

