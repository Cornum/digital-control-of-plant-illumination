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
  int channel = 0;  //default value
  int GrowthTime = 60;  //in min
  int typeFunction = 0;  //block type
  int timeGrowthStartHours = 8;
  int timeGrowthStartMinutes = 0;
  int timeDeclineStartHours = 23;
  int timeDeclineStartMinutes = 0;
  int maxIntensity = 2048;  //up to 4095
  
  
  // int readDataArray[8];
  
  // const byte MAX_BUFFER_SIZE = 64; 
  // char buffer[MAX_BUFFER_SIZE];    
  // byte index = 0;   
///\MatLab Connection

////Growth function
  ///Data for saving
    const float AvogadroConstant = 6.02214076e23;
    const float PlanckConstant = 6.62607015e-34;
    const float PiConstant = 3.1415926;
    const int MeasureTime = 10; //chain with time?

    float einsteinArray[MeasureTime];
  ///\Data for saving
  
////\Growth function


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
////Variables for function
  /*
    //Read sample from MatLAB for growth function
    for(int i = 0; i > 7; i++){
      if (Serial.available()) {
        char incomingChar = Serial.read(); // Считываем входной символ из Serial порта
        if (incomingChar != '\n') {        // Проверяем, не является ли символ завершающим
          buffer[index++] = incomingChar;  // Записываем символ в буфер
          if (index >= MAX_BUFFER_SIZE) {  // Проверяем на переполнение буфера
            // Обработка переполнения буфера
            index = 0;                      // Сбрасываем индекс
          }
        } else {
          buffer[index] = '\0';             // Добавляем завершающий символ строки
          readDataArray[i] = atoi(buffer);   // Преобразуем строку в целое число
          // Делаем что-то с полученным целым числом
          // Сбрасываем индекс для следующей строки
          index = 0;
        }
      }
    }
    channel = readDataArray[0];
    GrowthTime = readDataArray[1];
    typeFunction = readDataArray[2];
    timeGrowthStartHours = readDataArray[3];
    timeGrowthStartMinutes = readDataArray[4];
    timeDeclineStartHours = readDataArray[5];
    timeDeclineStartMinutes = readDataArray[6];
    maxIntensity = readDataArray[7];
    
  */


////\Variables for function
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
      lcd.print(maxIntensity);
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

void growthFunction(){}

float calculateEinstein (int time, int voltage, const int voltageMax){
  float einstein = (AvogadroConstant * PlanckConstant * asin(voltage/voltageMax)) / (2 * PiConstant * time);
  return einstein;
}