#include <Wire.h>                 //for I2C
#include <LiquidCrystal_I2C.h>    //for LSD
#include <math.h>
#include <DS1307RTC.h>            //for time
#include <TimeLib.h>
#include "PCA9685.h"


////Menu Global variables
  // Define debounce time in milliseconds
    #define DEBOUNCE_TIME 25
  

  // Variables for button debounce
    unsigned long lastDebounceTime = 0;
    bool buttonState = HIGH;
    bool lastButtonState = HIGH;

  // Encoder const
    const int pinCLK = 2;           
    const int pinDT  = 3;           
    const int pinSW  = 4;           
    int pinGlobal = 6;            //find this pin!!!!

  //Menu's global
    int localParametrMenu = 0;
    int globalParametrMenu = 0;
    int timeChooser = 0;

  // LCD display I2C
    LiquidCrystal_I2C lcd(0x27, 20, 4);     

  // Screen resolution
    int res = 12;   

  // Encoder variables
    int poziceEnkod = 0;                
    int stateBefore;                       
    int stateCLK;                        
    int stateSW;   

  ///Global variables for settings
    struct ChannelTypePair{
      const char* channel;      //channel number
      int typeFunction;       //block type / sinus type / day simulation type 
    };
    //int pins[2] = {1,1};      //for experimental only 2 pins which both are on
    const int dictSize = 2;
    ChannelTypePair channels[dictSize] = {{"0",0}, {"1",0}};    //{channel number, block type function}
    int growthTime = 60;  //in min
    int timeGrowthStartHours = 8;
    int timeGrowthStartMinutes = 0;
    int timeDeclineStartHours = 23;
    int timeDeclineStartMinutes = 0;
    int maxIntensity = 2048;  //up to 4095
  ///\Global variables
////\Menu

  ///Einstein data
    const float AvogadroConstant = 6.02214076;
    const float PlanckConstant = 6.62607015;
    const float PiConstant = 3.14159265358979323846;
    const float VoltageMax = 4095.0;
    int tenPow = -11;
  ///\Einstein data

  tmElements_t tm;          //time element DS1307

  PCA9685 pwmController1(B000000);        //pwm controller I2C
  PCA9685 pwmController2(B000001);        //pwm controller I2C
  PCA9685 pwmControllerAll(PCA9685_I2C_DEF_ALLCALL_PROXYADR);   //all pwm controllers I2C


void setup() {
  Serial.begin(115200);         // Serial speed
////Menu
  res =(int) pow(2,res);        // Resolution 2^res --- 4096 values   
  //PWM setup
  pwmControllerAll.resetDevices(); 
  pwmController1.init(); 
  pwmController2.init(); 
  pwmController1.setPWMFrequency(500);
  pwmController2.setPWMFrequency(500);
  //\PWM setup

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
  stateBefore = digitalRead(pinCLK);       // Start encoder
  drawMenu();                             // Start menu
  //\Encoder initiation

////\Menu
}



void loop() {
////Menu
  stateCLK = digitalRead(pinCLK);
  stateSW = digitalRead(pinSW);

  //Encoder rotation
  if (stateCLK != stateBefore) {
    if (digitalRead(pinDT) != stateCLK){localParametrMenu++;}
    else{localParametrMenu--;}
    drawMenu();
  }
  stateBefore = stateCLK;
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
        handleButtonClick();
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
      switch(localParametrMenu){
        case 1:{
          lcd.print("<- Type ->");
          break;
        }
        case 2:{
          lcd.print("<- Intensity ->");
          break;
        }
        case 3:{
          lcd.print("<- Start time ->");
          break;
        }
        case 4:{
          lcd.print("<- Final time ->");
          break;
        }
        case 5:{
          lcd.print("<- Done");
          break;
        }
        default:{
          localParametrMenu = 0;
          lcd.print("Growth time ->");
          break;
        }
      }
      break;
    }
    case 2:{ //Growth time measurments menu
      lcd.setCursor(0,0);
      lcd.print("Time settings menu: ");
      lcd.setCursor(0, 1);
      lcd.print("Set time [min]:");
      lcd.setCursor(0, 3);
      if (localParametrMenu < 0){
        localParametrMenu = 0;
        growthTime = localParametrMenu;
      }
      else{
        growthTime = localParametrMenu;
      }
      lcd.print(growthTime);
      lcd.setCursor(16, 3);
      lcd.print("Done");
      break;
    } 
    case 3:{ //type measurments menu
      lcd.setCursor(0,0);
      lcd.print("Type settings menu: ");
      lcd.setCursor(0,3);
      switch(localParametrMenu){
        case 1:{
          lcd.print("<- Sinus type ->");
          break;
        }
        case 2:{
          lcd.print("<- Day simulation");
          break;
        }
        default:{
          localParametrMenu = 0;
          lcd.print("Block type ->");
          break;
        }
      }
      break;
    }
    case 4:{  //Intensity
      lcd.setCursor(0,0);
      lcd.print("Max intensity menu:");
      lcd.setCursor(0, 1);
      lcd.print("Set intensity x32:");
      lcd.setCursor(0, 3);
      if (localParametrMenu * 32 < 0){
        localParametrMenu = 0;
      }
      else if(localParametrMenu * 32 > 4096){
        localParametrMenu = 4096 / 32;
      }
      maxIntensity = localParametrMenu * 32;
      lcd.print(maxIntensity);
      lcd.setCursor(16, 3);
      lcd.print("Done");
      break;
    }
    case 5:{    //Start time menu
      lcd.setCursor(0,0);
      lcd.print("Start time menu:");
      lcd.setCursor(0, 1);
      lcd.print("Set time:");

      if(timeChooser == 0){   //hours

        if(localParametrMenu < 0){
          localParametrMenu = 23;
        }
        else if(localParametrMenu > 23){
          localParametrMenu = 0;
        }
        timeGrowthStartHours = localParametrMenu;

      }
      else{   //minutes

        if(localParametrMenu < 0){
          localParametrMenu = 59;
        }
        else if(localParametrMenu > 59){
          localParametrMenu = 0;
        }
        timeGrowthStartMinutes = localParametrMenu;

      }
      //if else for lcd print
      if(timeGrowthStartHours < 10){
        lcd.setCursor(0, 3);
        lcd.print(0);
        lcd.setCursor(1, 3);
        lcd.print(timeGrowthStartHours);
      } else{
        lcd.setCursor(0, 3);
        lcd.print(timeGrowthStartHours);
      }
      lcd.setCursor(2, 3);
      lcd.print(":");
      if(timeGrowthStartMinutes < 10){
        lcd.setCursor(3, 3);
        lcd.print(0);
        lcd.setCursor(4, 3);
        lcd.print(timeGrowthStartMinutes);
      } else{
        lcd.setCursor(3, 3);
        lcd.print(timeGrowthStartMinutes);
      }

      lcd.setCursor(16, 3);
      lcd.print("Done");
      break;
    }
    case 6:{    //Final time menu
      lcd.setCursor(0,0);
      lcd.print("Final time menu:");
      lcd.setCursor(0, 1);
      lcd.print("Set time:");

      if(timeChooser == 0){   //hours

        if(localParametrMenu < 0){
          localParametrMenu = 23;
        }
        else if(localParametrMenu > 23){
          localParametrMenu = 0;
        }
        timeDeclineStartHours = localParametrMenu;

      }
      else{   //minutes

        if(localParametrMenu < 0){
          localParametrMenu = 59;
        }
        else if(localParametrMenu > 59){
          localParametrMenu = 0;
        }
        timeDeclineStartMinutes = localParametrMenu;

      }
      //if else for lcd print
      if(timeDeclineStartHours < 10){
        lcd.setCursor(0, 3);
        lcd.print(0);
        lcd.setCursor(1, 3);
        lcd.print(timeDeclineStartHours);
      } else{
        lcd.setCursor(0, 3);
        lcd.print(timeDeclineStartHours);
      }
      lcd.setCursor(2, 3);
      lcd.print(":");
      if(timeDeclineStartMinutes < 10){
        lcd.setCursor(3, 3);
        lcd.print(0);
        lcd.setCursor(4, 3);
        lcd.print(timeDeclineStartMinutes);
      } else{
        lcd.setCursor(3, 3);
        lcd.print(timeDeclineStartMinutes);
      }

      lcd.setCursor(16, 3);
      lcd.print("Done");
      break;
    }
    case 10:{  //setted program
      lcd.setCursor(0,0);
      lcd.print("Menu: ");
      lcd.setCursor(0,3);
      switch(localParametrMenu){
        case 1:{  //set all from 0
          lcd.print("<- Settings ->");
          break;
        }
        case 2:{
          lcd.print("<- EXIT");
          break;
        }
        default:{ //setted program
          localParametrMenu = 0;
          lcd.print("Start final ->");
          break;
        }
      }
      break;
    }
    case -1:{ //default program
      lcd.setCursor(0,0);
      lcd.print("Measurment: ");
      lcd.setCursor(0,3);
      switch(localParametrMenu){
        case 1:{
          lcd.print("<- STOP");
          break;
        }
        default:{
          localParametrMenu = 0;
          lcd.print("OK ->");
          break;
        }
      }
      break;
    }
    case -2:{//FINAL setted program
      lcd.setCursor(0,0);
      lcd.print("Measurment: ");
      lcd.setCursor(0,3);
      switch(localParametrMenu){
        case 1:{
          lcd.print("<- STOP");
          break;
        }
        default:{
          localParametrMenu = 0;
          lcd.print("OK ->");
          break;
        }
      }
      break;
    }

    default:{  //start menu
      lcd.setCursor(0,0);
      lcd.print("Menu: ");
      lcd.setCursor(0,3);
      switch(localParametrMenu){
        case 1:{
          lcd.print("<- Settings");
          break;
        }
        default:{ //default program from MatLAB
          localParametrMenu = 0;
          lcd.print("Start ->");
          break;
        }
      }
      break;
    }
  }
}                                                     //\LCD - draw Menu


void handleButtonClick(){  //Button click handle
  switch(globalParametrMenu){
    case 1:{  //Settings
      switch(localParametrMenu){
        case 1:{  //transfer to Type
          globalParametrMenu = 3;
          break;
        }
        case 2:{  //transfer to Intensity
          globalParametrMenu = 4;
          break;
        }
        case 3:{  //transfer to Start time
          globalParametrMenu = 5;
          break;
        }
        case 4:{  //transfer to Final time
          globalParametrMenu = 6;
          break;
        }
        case 5:{  //DONE transfer to Final Menu
          globalParametrMenu = 10;
          break;
        }
        default:{ //transfer to Growth time
          localParametrMenu = 0;
          globalParametrMenu = 2;
          break;
        }
      }
      localParametrMenu = 0;
      break;
    }
    case 2:
    case 4:{ //Growth time & Intensity
      localParametrMenu = 0;
      globalParametrMenu = 1; //transfer to Settings Menu
      break;
    }
    case 3:{ //Type
      switch(localParametrMenu){
        case 1:{    //sinus type
          setTypeFunction("0", 1);
          globalParametrMenu = 1;
          break;
        }
        case 2:{    //day simulation
          setTypeFunction("0", 2);
          globalParametrMenu = 1;
          break;
        }
        default:{   //block type
          setTypeFunction("0", 0);
          localParametrMenu = 0;
          globalParametrMenu = 1;
          break;
        }
      }
      localParametrMenu = 0;
      break;
    }
    case 5:
    case 6:{   //Start, Final time
      if(timeChooser == 0){
        localParametrMenu = 0;
        timeChooser++;
      }
      else{
        timeChooser = 0;
        localParametrMenu = 0;
        globalParametrMenu = 1; //transfer to Settings Menu
      }
      break;
    }
    case 10:{ //Final Menu

      switch(localParametrMenu){
        case 1:{    //transfer to settings menu
           globalParametrMenu = 1;
           break;
        }
        case 2:{    //transfer to START MENU
          globalParametrMenu = 0;
          break;
        }
        default:{   //start setted program
          localParametrMenu = 0;
          globalParametrMenu = -2;
          break;
        }
      }
      localParametrMenu = 0;
      break;
    }
    case -1:{ //default program
      switch(localParametrMenu){
        case 1:{  //transfer to start menu
          globalParametrMenu = 0;
          break;
        }
        default:{ //do default program
          lcd.setCursor(0, 3);
          lcd.print("Starting...");
          // if(time.hour == timeGrowthStartHours) && (time.min == timeGrowthStartMinutes) && (time.sec == 0){
            for(int i = 0; i < growthTime*60; i++){
              for (int j = 0; j < 2; j++){
                growthFunctionBlock(i, pinGlobal, j);
              }
              delay(1); //change to 1000 or chain with real time
            }
          // }
          // lcd.clear();
          lcd.setCursor(0, 3);
          lcd.print("Proceed...");
          //until time for decline
          delay(3000); //for see, that Proceed is on screen
          break;
        }
      }
      localParametrMenu = 0;
      break;
    }
    case -2:{ //START FINAL setted program
      switch(localParametrMenu){
        case 1:{  //transfer to start menu
          globalParametrMenu = 0;
          break;
        }
        default:{ //do setted program
          localParametrMenu = 0;
          lcd.setCursor(0, 1);
          lcd.print("Starting...");
          delay(100); //for see, that Starting is on screen
          for(int i = 0; i < growthTime*60; i++){ //change growthTime to settedTime
            for (int j = 0; j < 2; j++){
              growthFunctionBlock(i, pinGlobal, j);
            }
          }
          lcd.setCursor(0, 1);
          lcd.print("Proceed...");
          break;
        }
      }
      localParametrMenu = 0;
      break;
    }
    default:{
      switch(localParametrMenu){
        case 1:{    //transfer to settings menu
          globalParametrMenu = 1;
          break;
        }
        case 2:{    //start saving data
          globalParametrMenu = -10;
          break;
        }
        default:{   //start default
          localParametrMenu = 0;
          globalParametrMenu = -1;
          break;
        }
      }
      localParametrMenu = 0;
      break;
    }
  }
}

void growthFunctionBlock(int time, int pin, int channel){ //time - sec from cycle starts
  double angleStep = (PiConstant / 2) / (growthTime * 60);
  int bright = sin(angleStep * time) * maxIntensity;

  if (channel == 1){
    pwmController1.setChannelPWM(pin, bright); 
  }
  else{
    pwmController2.setChannelPWM(pin, bright);
  }

  printEinstein({time},bright);     //chain with time from all works time
}

void declineFunctionBlock(int time, int pin, int channel){ //time - sec from cycle starts
  double angleStep = (PiConstant / 2) / (growthTime * 60);
  int bright = sin((angleStep * time) + (PiConstant / 2)) * maxIntensity;
  
  if (channel == 1){
    pwmController1.setChannelPWM(pin, bright); 
  }
  else{
    pwmController2.setChannelPWM(pin, bright);
  }

  printEinstein({time},bright);     //chain with time from all works time
}

void printEinstein (int time, int voltage){   //return data in einsteins for saving in file
  float einstein = (1000000 * AvogadroConstant * PlanckConstant * asin(voltage/VoltageMax)) / (2 * PiConstant * time);  //1000000 is value for value > 0 in float
  Serial.print(einstein);
  Serial.println(" * 10^-11 mkE");
}

int getTypeFunction(const char* channel){
  for (int i = 0; i < dictSize; i++){
    if(strcmp(channel, channels[i].channel) == 0){ //find definition strcmp
      return channels[i].typeFunction;
    }
  }
  return -1;     //channel isn't found
}

void setTypeFunction(const char* channel, int newType){
  for (int i = 0; i < dictSize; i++){
    if(strcmp(channel, channels[i].channel) == 0){
      channels[i].typeFunction = newType;
      return;
    }
  }
}