#include <Wire.h>                 //for I2C
#include <LiquidCrystal_I2C.h>    //for LCD
#include <math.h>                 //for math
#include <OneButton.h>            //for encoder handling
#include <PCA9685.h>              //for PWM switchers

#define Debounce_Time 25 // time between rotates

// Global variables

  // Encoder variables
  const int PinClock = 2;           
  const int PinDiscreteTime = 3;   
  const int PinShortWave = 4;      
  //int stateBefore;      // Previous state of the encoder clock pin
  //int stateClock;       // Current state of the encoder clock pin

  // Usable constants
  const float ConstPi = 3.1415926535;

  // Time variables
  unsigned long previousChange = 0;             
  unsigned long startTime = 0;                

  // Global settings
  int actualPWM = 0;
  int previousPWM = 0;  //for locating changes
  //int menuQuocient = 1;
  double totalEnergyBlue = 0;
  double totalEnergyWhiteWarm = 0;
  bool isProgramStarted = false;
  bool isDayEnd = false;

  // Modules initialisation
  OneButton encoderButton(PinShortWave, true); // For handling button presses
  LiquidCrystal_I2C lcd(0x27, 20, 4);
  PCA9685 pwmController1(B000000);        // PWM controller I2C


//Methods
  //Button click handle
  void singleClick() {
    startTime = millis();
    totalEnergyBlue = 0;
    totalEnergyWhiteWarm = 0;
    lcd.clear();
    isDayEnd = false;
    Serial.println("Day phase is on");
    isProgramStarted = !isProgramStarted;
  }

  /*void doubleClick() {
  }*/

  /*void longPress() {
    menuQuocient *= 2;
    if(menuQuocient > 64){
      menuQuocient = 1;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Actual quocient:");
    lcd.setCursor(0, 1);
    lcd.print(menuQuocient);
  }*/

void setup() {
  Serial.begin(115200);

  pwmController1.init();
  pwmController1.setPWMFrequency(1000);
  
  lcd.init();
  lcd.setCursor(3,0);
  lcd.print("Hello, starting..");
  for (int i = 0; i < 5; i++) {
    lcd.noBacklight();
    delay(50);
    lcd.backlight();
    delay(50);
  }
  lcd.clear();

  pinMode(PinClock, INPUT);                        // Encoder
  pinMode(PinDiscreteTime, INPUT);                // Encoder
  pinMode(PinShortWave, INPUT_PULLUP);            // Encoder button
  //stateBefore = digitalRead(PinClock);             // Start encoder

  encoderButton.attachClick(singleClick);
  //encoderButton.attachDoubleClick(doubleClick);
  //encoderButton.attachLongPressStart(longPress);
}

void loop() {
  encoderButton.tick();
  
  if(!isProgramStarted){ //Before start menu
    lcd.setCursor(4,1);
    lcd.print("Press button");
    lcd.setCursor(4,2);
    lcd.print("to start...");
  /*
    // Encoder rotation handling
    stateClock = digitalRead(PinClock);
    if (stateClock != stateBefore) {
      if (digitalRead(PinDiscreteTime) != stateClock) {
        actualPWM += menuQuocient;
      } else {
        actualPWM -= menuQuocient;
      }
      if (actualPWM < 0) {
        actualPWM = 4095;
      } else if (actualPWM > 4095) {
        actualPWM = 0;
      }
      lcd.clear();
      lcd.setCursor(0, 1);   //ubrat
      lcd.print(actualPWM);
    }
    stateBefore = stateClock;*/
  }
  else{
    unsigned long elapsedTime = (millis() - startTime)/1000;  //Counting time spend
    
    if(elapsedTime >= 57600){ //Phase changing
      lcd.setCursor(0, 0);
      lcd.print("Day phase is off");
      lcd.setCursor(0, 1);
      lcd.print("Night phase: ");
      lcd.setCursor(0, 2);
      lcd.print(86400-elapsedTime);

      if(!isDayEnd){  //One time write day info into serial
        lcd.clear();
        Serial.println("Day phase is off");
        Serial.print("Total energy on Blue: " );
        Serial.println(totalEnergyBlue, 4);
        Serial.print("Total energy on WhiteWarm: " );
        Serial.println(totalEnergyWhiteWarm, 4);
        Serial.println("_________");
        isDayEnd = !isDayEnd;
      }
    }
    else{
      actualPWM = round(sin(elapsedTime*ConstPi/57600)*4096); //Compute pwm in actual time

      lcd.setCursor(0, 0);
      lcd.print("Actual PWM:");
      lcd.setCursor(12, 0);
      lcd.print(actualPWM);
      
      if (actualPWM < 16) {   //Fan activating
        pwmController1.setChannelPWM(15, 0);
      }
      else if (actualPWM >= 16 && actualPWM <= 1348) {
        pwmController1.setChannelPWM(15, 1348);
      }
      else if (actualPWM > 1348) {
        pwmController1.setChannelPWM(15, actualPWM);
      }


      //Blue 0.0094x-0.3415
      double energyBlue = actualPWM * 0.0094 - 0.3415;
      if(energyBlue < 0){
        energyBlue = 0;
      }
      lcd.setCursor(0,1);
      lcd.print("B: ");
      lcd.setCursor(17,1);
      lcd.print("mkE");
      pwmController1.setChannelPWM(6, actualPWM);

      //WhiteWarm  0.031x-0.5801
      double energyWhiteWarm = actualPWM * 0.031 - 0.5801;
      if(energyWhiteWarm < 0){
        energyWhiteWarm = 0;
      }
      lcd.setCursor(0,2);
      lcd.print("Wh: ");
      lcd.setCursor(17,2);
      lcd.print("mkE");
      pwmController1.setChannelPWM(7, actualPWM);

      //Time on lcd
      lcd.setCursor(0, 3);
      lcd.print("Time: ");
      lcd.setCursor(6, 3);
      lcd.print(elapsedTime);

      if(actualPWM != previousPWM){ //Writing into serial every next pwm changing
        totalEnergyBlue += (elapsedTime - previousChange) * energyBlue;
        totalEnergyWhiteWarm += (elapsedTime - previousChange) * energyWhiteWarm;
        Serial.print("Time: ");
        Serial.println(elapsedTime);
        Serial.print("PWM: " );
        Serial.println(actualPWM);
        Serial.print("Total energy on Blue: " );
        Serial.println(totalEnergyBlue, 4);
        lcd.setCursor(3,1);
        lcd.print(totalEnergyBlue, 4);

        Serial.print("Total energy on WhiteWarm: " );
        Serial.println(totalEnergyWhiteWarm, 4);
        lcd.setCursor(4,2);
        lcd.print(totalEnergyWhiteWarm, 4);
        Serial.println("_________");
        previousPWM = actualPWM;
        previousChange = elapsedTime;
      }
    }
    if(elapsedTime >=86400){  //Last sending into serial
      Serial.println("Program is finished");
      Serial.print("Elapsed time: ");
      Serial.println(elapsedTime);
      Serial.println("_________");
      Serial.println("_________");
      isProgramStarted = !isProgramStarted;
      lcd.clear();
    }
  }
}
