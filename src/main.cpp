#include <Arduino.h>
#include <SPI.h>
#include <string.h>
#include "LCDWIKI_GUI.h" //Core graphics library
#include "LCDWIKI_KBV.h" //Hardware-specific library
#include <EEPROM.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>

//DEFINES
#define DHTPIN 22     // Digital pin connected to the DHT sensor 
#define DHTTYPE DHT11     // DHT 11
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//VARIABLES
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
LCDWIKI_KBV mylcd(ILI9486, A3 , A2, A1, A0, A4); //model,cs,cd,wr,rd,reset
int screenWidth, screenHeight;
float tempF, tempC, humidity;
int mode, addrMode = 0; //72 = (H)eat; 67 = (C)ool
int heatTemp, addrSetHeatTemp = 1;
int coolTemp, addrSetCoolTemp = 2;
bool stopReading = false;

void setup() {

  Serial.begin(250000);
  mylcd.Init_LCD();
  Serial.println(mylcd.Read_ID(), HEX);
  mylcd.Fill_Screen(BLACK);
  mylcd.Set_Text_Mode(0);
  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_Size(2);
  mylcd.Set_Rotation(1);
  dht.begin();

  delayMS = 1000;
  //Retrieve stored values from EEPROM
  mode = EEPROM.read(addrMode);
  if(mode != 67 || mode != 72)
  {
    //67 & 72 are ASCII values for C & H
    //If current read value is not one of these then default to (H)eat
    mode = 72;
  }
  heatTemp = EEPROM.read(addrSetHeatTemp);
  coolTemp = EEPROM.read(addrSetCoolTemp);
}

void loop() {

//Continue to update variables


  screenWidth = mylcd.Get_Display_Width();
  screenHeight = mylcd.Get_Display_Height();

  mylcd.Set_Text_Size(3.5);

  // Delay between measurements.
  delay(delayMS);

  // Get temperature event and print its value.
  sensors_event_t event;

  dht.temperature().getEvent(&event);

  if (!isnan(event.temperature)) {
    mylcd.Print_String("Temp: " + String(round(((event.temperature * 9) / 5) + 32)) + "F" , 0, 40);
    mylcd.Print_String("Temp: " + String(round(event.temperature)) + "C" , 0, 80);
    mylcd.Print_String("Current mode: " + String((char) mode), 0, 160);
    mylcd.Print_String("Current set heat temp: " + String(heatTemp), 0, 200);
    mylcd.Print_String("Current set cool temp: " + String(coolTemp), 0, 240);
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);

  if (!isnan(event.relative_humidity)) {
    mylcd.Print_String("Humidity: " + String(round(event.relative_humidity)) + "%", 0, 120);
  }
}