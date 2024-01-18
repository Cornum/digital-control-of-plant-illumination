/*  
 *  ---------------------------------------------
 *  Pripojene knihovny
 */
#include <Wire.h>                 //pro I2C
#include <LiquidCrystal_I2C.h>    //pro display
#include <OneWire.h>            //pro komunikace po jednom vodici - pro teplotni cidlo
#include <DallasTemperature.h>  //pro teplotni cidlo
#include "PCA9685.h"              //PWM kontroler
/* 
 *  Pripojene knihovny
 *  ---------------------------------------------
 */


/*
 * --------------------------------------------- 
 * Cisla pinu pripojenych zarizeni
 */
const int buttonPin1 = 12;      // tlacitko 1 (mikrospinac)
const int buttonPin2 = 13;      // tlacitko 2 (mikrospinac)
const int DSPin = 8;            // teplotni cidlo

const int pinCLK = 2;           // otocny ovladac (enkoder)
const int pinDT  = 3;           //
const int pinSW  = 4;           // otocny ovladac (enkoder)
/* 
 * Cisla pinu pripojenych zarizeni
 * ---------------------------------------------
 */



/*
 * ---------------------------------------------
 * Iniciace pripojenych zarizeni
 */
LiquidCrystal_I2C lcd(0x27, 20, 4);     // LCD display pres I2C


OneWire oneWireDS(DSPin);                   // teplotni cidlo komunikace pres OneWire
DallasTemperature senzoryDS(&oneWireDS);    // teplotni cidlo


PCA9685 pwmController1(B000000);        // PWM kontroler1 pres I2C - Library using B000000 (A5-A0) i2c address, and default Wire @400kHz
PCA9685 pwmController2(B000001);        // PWM kontroler2 pres I2C - Library using B000001 (A5-A0) i2c address, and default Wire @400kHz
PCA9685 pwmControllerAll(PCA9685_I2C_DEF_ALLCALL_PROXYADR); //PWM vsechny kontrolery
/* 
 * Iniciace pripojenych zarizeni
 * ---------------------------------------------
 */



/*
 * ---------------------------------------------
 * Definice promennych a konstant
 */
int res = 12;                       //PWM bit bitova hloubka (2^8=255 | 2^10=1023 | 2^12=4095)

int buttonState1 = 0;               // stav tlačítka 1 (mikrospinac)
int buttonState2 = 0;               // stav tlačítka 2 (mikrospinac)

unsigned long previousMillis1 = 0;  // citac = casova zakladna Arduina - kontrola behu programu
unsigned long previousMillis2 = 0;  // citac = casova zakladna Arduina - kontrola behu programu

int led = 0;                        // hodnota PWM pro led panel
int pled = 0;                       //
int multi = 32;                     // nasobic hodnoty
int pmulti = 32;                    // 

int poziceEnkod = 0;                // pro enkoder
int stavPred;                       //
int stavCLK;                        //
int stavSW;                         // pro enkoder

int parametrHigh = 0;               // pro rust intenzity
int parametrLow = 0;                // pro klesani intenzity
int parametrHalf = 0;               // pro nastaveni maximalni intenzity

/*
 * *Definice promennych a konstant
 * ---------------------------------------------
 */



/*
 * ---------------------------------------------
 * Iniciace
 */
void setup()  {  
  Serial.begin(115200);         // rychlost seriove linky
  Wire.begin();                 // iniciace seriove komunikace
    
  pinMode(buttonPin1, INPUT);   // iniciace tlacitka1
  pinMode(buttonPin2, INPUT);   // iniciace tlacitka2
  
  
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

    
 
  senzoryDS.begin();                  // iniciace tepltoniho cidla


  pwmControllerAll.resetDevices();      // Resets vsech PCA9685 zarizeni na i2c lince
  pwmController1.init();                // Iniciace PWM kontroleru1
  pwmController2.init();                // Iniciace PWM kontroleru2
  pwmController1.setPWMFrequency(500);  // Nastaveni frekvence PWM (defaultne 200Hz, lze 24Hz – 1526Hz)
  pwmController2.setPWMFrequency(500);  // Nastaveni frekvence PWM (defaultne 200Hz, lze 24Hz – 1526Hz)
  

  pinMode(pinCLK, INPUT);               // Otocny ovladac
  pinMode(pinDT, INPUT);                // Otocny ovladac
  pinMode(pinSW, INPUT_PULLUP);         // Otocny ovladac - tlacitko  
  stavPred = digitalRead(pinCLK);       // Pocatecni stav otocneho ovladace

  drwtemp();                            // Pocatecni vypsani displaye   
  drwbit();                             // Pocatecni vypsani displaye
}
/*
 * *Iniciace
 * ---------------------------------------------
 */







//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// HLAVNI SMYCKA
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void loop()  {
stavCLK = digitalRead(pinCLK);
stavSW = digitalRead(pinSW);
buttonState1 = digitalRead(buttonPin1);
buttonState2 = digitalRead(buttonPin2);
unsigned long currentMillis = millis();

  //Menu
  //*Menu

  //----------------------------------------------------------------------------------------
  //Otoceni enkoderem
  if (stavCLK != stavPred) {                      // kdyz se otocilo enkoderem
    if (digitalRead(pinDT) != stavCLK) {          // Serial.print("Rotace vpravo => | ");
      if (led<res) {led=led+multi;                 
        if (led>res){led=res;}
      }      
    }
    else {                                        // Serial.print("Rotace vlevo  <= | ");        
      if (led>0) {led=led-multi;                  
        if (led<0){led=0;}      
      }
    }    
    drwbit();                                     // prepsat display    
  }
  stavPred = stavCLK;                           // aktualizuj hodnotu enkoderu
  //*Otoceni enkoderem
  if (stavCLK != stavPred) {                  //nastaveni parametru (udelat vyber z menu)
    if (digitalRead(pinDT) != stavCLK){parametrHigh++;}
    else{parametrHigh--;}
    drwbit();
  }
  stavPred = stavCLK;
  //----------------------------------------------------------------------------------------



  //----------------------------------------------------------------------------------------
  //Po 10s vycte a prepise teplotu

    if ((unsigned long)(currentMillis - previousMillis1) >= 10000) {    
    drwtemp();
    Serial.println("TEMP");
    previousMillis1 = currentMillis;
    } 

  //----------------------------------------------------------------------------------------


  //----------------------------------------------------------------------------------------
  //Reakce na macknuti tlacitka
  if ((unsigned long)(currentMillis - previousMillis2) >= 300) {  // Kazdych 300ms kontrola macknuti tlacitek
    if (buttonState1>0 && multi>1)                                // nasobic -1 
    {
      multi--;
    }
    if (buttonState2>0 && multi<32)                               // Nasobic +1
    {
      multi++;
    }
    if (buttonState1>0  && buttonState2>0) {multi=1;}             // Obe najednou => reset nasobice na 1
        
    if (stavSW == 0) {                                            // Zmacknuti enkoderu => reset hodnoty PWM do 0
    // Serial.println("Stisknuto tlacitko enkoderu!");
    // //led = 0; //kdyz neni breathing
    // led = 0; setAllPWM(); breathing(1);
    rustBrightness(parametrHigh, parametrHalf);
    }
    
    if (pled!=led || pmulti!=multi){                              // Kdyz se zmeni jedna z hodnot prepsat display a predchozi hodnoty a nastavit PWM    
      drwbit();
      pled=led;
      pmulti=multi;
      
      // pwmController1.setChannelPWM(0, led);                    // Nastaveni hodnoty PWM - jen jeden kanal
      setAllPWM();                                                // Nastaveni hodnoty PWM - pro vsechny kanaly
    }                                                             // Kdyz se zmeni jedna z hodnot prepsat display a predchozi hodnoty          
    previousMillis2 = millis();                                   // Reset citace

  
  //Serial.println(led);
  
  /*                             // Testovani tlacitek
  Serial.print("Tlacitko 1: ");
  Serial.println(buttonState1);
  Serial.print("Tlacitko 2: ");
  Serial.println(buttonState2);
  //Serial.print(previousMillis);Serial.print("---");Serial.println(currentMillis);
 */                             // Testovani tlacitek
  }

  
  //----------------------------------------------------------------------------------------
 
 


}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// *HLAVNI SMYCKA
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------






//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// Procedury
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void drwbit() {                                       //LCD - vypis hodnoty LED
  //Serial.print(" Intensity: ");
  //Serial.print(led);
  //Serial.println(" bit");

  lcd.setCursor(0,3);
  lcd.print("Value: ");
  lcd.print(led);lcd.print("    ");
  lcd.setCursor(17,3);
  lcd.print("*");
  lcd.print(multi);
  if(multi<10) {lcd.print(" ");}
}                                                     //LCD - vypis hodnoty LED

void drwtemp() {                                      //LCD - vypis teploty
  lcd.setCursor(19,2);lcd.print("!");
  
  senzoryDS.requestTemperatures();

  lcd.setCursor(0,2);
  lcd.print("Temp: ");
  lcd.print(senzoryDS.getTempCByIndex(0));
  lcd.print(" degC");
  lcd.setCursor(19,2);lcd.print(" ");

  Serial.print("Teplota cidla DS18B20: ");
  Serial.print(senzoryDS.getTempCByIndex(0));
  Serial.println(" stupnu Celsia");  
}                                                     //LCD - vypis teploty

void setAllPWM() {                                    //PWM - nastav vsechny stejne
  for (int i=0; i<=15; i++) {
    pwmController1.setChannelPWM(i, led); 
    pwmController2.setChannelPWM(i, led);
  } 
}                                                     //PWM - nastav vsechny stejne


void breathing(int n) {
  if (n<1) {n=1;}
  
  for (int t=1; t<=n; t++) {
    
  for (int i=0; i<=5; i++) {
    for (int j=0; j<=4095; j++){
      pwmController1.setChannelPWM(i, j);           
    }
    for (int j=4095; j>=0; j--){
      pwmController1.setChannelPWM(i, j);           
    }    
  }
    
  for (int i=0; i<=5; i++) {
    for (int j=0; j<=4095; j++){
      pwmController2.setChannelPWM(i, j);           
    }    
    for (int j=4095; j>=0; j--){
      pwmController2.setChannelPWM(i, j);           
    }
  }  
  }
}

void rustBrightness(int parametrRust, int parametrBright){
  for(int i =0; i<5; i++){ 
    for(int brightness = 0; brightness <= 4096 * parametrBright; brightness += 50*sin(parametrRust)){
      pwmController1.setChannelPWM(i, brightness);
      delay(50);
    }
  }
}

void klesBrightness(int parametrKles, int parametrBright){
  for(int i =0; i<5; i++){ 
    for(int brightness = 4096 * parametrBright; brightness >= 0; brightness -= 50*sin(parametrKles)){
      pwmController1.setChannelPWM(i, brightness);
      delay(50);
    }
  }
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// *Procedury
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
