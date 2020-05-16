#include "LiquidCrystal_I2C.h"
#include <SPI.h> 
#include <Wire.h> 
#include <Thermocouple.h>
#include <MAX6675_Thermocouple.h>
#include <AverageThermocouple.h>
#include <SD.h> 
//#define Button_PIN 2

//thermal
#define Thermal_SCK_PIN 3
#define Thermal_CS_PIN 4
#define Thermal_SO_PIN 5
#define READINGS_NUMBER 30
#define DELAY_TIME 10
Thermocouple* thermocouple_Ave = NULL;

//SD
#define SD_CS_PIN 7
File myTemp;

//LCD
//#define LCD_SDA_PIN A4
//#define LCD_SCL_PIN A5
LiquidCrystal_I2C lcd(0x27, 16, 2);// 0x27 is the I2C bus address for an unmodified backpack

void setup() {
  // put your setup code here, to run once:
  // pinMode(2,INPUT_PULLUP);//2号引脚接开关OUT，控制程序开始
  // while(digitalRead(2)){
  //  Serial.print("WAITING");
  //  }//按下按键前，2为高电平，返回1，执行空死循环
  //LCD Initialization
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("HELLO");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("FROM HEU KIPA");
  delay(500);
  
  //PC Initialization
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  
  //SD Initialization
  pinMode(10,OUTPUT);//SS引脚(UNO=10),保持输出，SD卡工作
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Initialization failled!");
    while (1);//初始化失败，执行空死循环，不进入loop循环
  }
  Serial.println("Initialization done.");
  
  //SD_File Initialization 打开文件，且一次只能打开一个文件,若需打开另一文件，须先关闭当前文件
  myTemp = SD.open("TempRec.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myTemp) {
    Serial.print("Writing to TempRec.txt...");
    myTemp.println("Temperature records are about to begin.");
    // close the file:
    myTemp.close();
    Serial.println("Temperature records are about to begin.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error opening File in setup");
    lcd.setCursor(0, 1);
    lcd.print("Setup File Error");
  }
  //MAX6675 Initialization
  Serial.print("Read data from MAX6675: ");
  lcd.setCursor(0, 0);
  lcd.print("MAX6675 Rec");
  Thermocouple* originThermocouple = new MAX6675_Thermocouple(Thermal_SCK_PIN, Thermal_CS_PIN, Thermal_SO_PIN);
  thermocouple_Ave = new AverageThermocouple(
    originThermocouple,
    READINGS_NUMBER,
    DELAY_TIME
  );
}
void loop() {
  // put your main code here, to run repeatedly:
  // Reads temperature
  const double Ave_celsius = thermocouple_Ave->readCelsius();
  Serial.println("Read data and write data");
  myTemp = SD.open("TempRec.txt", FILE_WRITE);
  if(myTemp){
    myTemp.print(Ave_celsius);
    myTemp.println("");
    Serial.println("File Writen");
    myTemp.close();
  }
  else{
    Serial.println("Error opening File");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MAX6675 Rec(C)");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(Ave_celsius);  
  // Output of information
  Serial.print("Temperature: ");
  Serial.print("Ave_Temperature: ");
  Serial.print(Ave_celsius);
  Serial.println(" C.");

  delay(500); // optionally, only to delay the output of information in the example.
}
