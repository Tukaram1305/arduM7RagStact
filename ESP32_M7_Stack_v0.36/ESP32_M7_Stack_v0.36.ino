/*
0db for a full-scale voltage of 1.1V (default)
2.5db for a full-scale voltage of 1.5V
6db for a full-scale voltage of 2.2V
11db for a full-scale voltage of 3.9V
*/
 
// FastLED PIN 13
// Ext DS (temp) PIN 25
// Battery % SVN/39
// BUZZER 33
TaskHandle_t Task1;

#include "ColorConv.h"
#include "RagClss.h"

//Komunikacja
#include <SPI.h>
#include <Wire.h>

//Magazyny
#include <FS.h>
#include <SD.h>
#include "SPIFFS.h"

#include "SDSPIFFSM.h"
//

// BMP Pressure n Temp
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

// ADXL345 Akcelerometr
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// VL53LOX TOF
#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// TFT i dekodery
#include <JPEGDecoder.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#include "Free_Fonts.h" // Include the header file attached to this sketch

//FastLedWS
#include <FastLED.h>
#define NUM_LEDS 288 
#define DATA_PIN 13
CRGB leds[NUM_LEDS];

// Radio NRF
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(26, 27); // using pin 7 for the CE pin, and pin 8 for the CSN pin
const byte address[6] = "00001";
const byte addrLed[6] = "00002";

byte pos = 0;
byte posPrev = 0;
//------ Radio NRF

#include <AHT10.h>
uint8_t readStatus = 0;
AHT10 aht10(AHT10_ADDRESS_0X38);
//------

#include "RTClib.h"
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
uint16_t stDel = 50;

// -- FFT
#include "arduinoFFT_f.h" // dla pojedynczej precyzji
#define SAMPLES 512
#define SAMPLING_FREQUENCY 38000
#define ALIAS 2 // 0 brak, 2 nowy
#define samples_per_cycle 16

arduinoFFT FFT = arduinoFFT();
  // MIC Offset / VOLT - ADC Read - AttenDb
  //             1.25v - 2326.7    ( 6db - 2.2v ) - mic max 2v = 3722.7
  //             1.25v - 1312.5    ( 11db- 3.9v )
float         vReal[SAMPLES];
float         vImag[SAMPLES];

unsigned long  milliFFT = 0;
unsigned long  microsecFFT = 0x00000000;

unsigned int   SF = round(1000000/SAMPLING_FREQUENCY); // 26 us = ~38 000 Hz
int            sum; // wartosc z AnalogRead
byte           mapped[SAMPLES / 2];
byte           mapped_p[SAMPLES / 2] = {};

float          ADC_PRESCALAR = 1.0;
uint16_t       noise_reduction = 50;
uint16_t       MicOffset = 2457;
//uint16_t       micMax = 3723;

uint16_t       micMaxAmp = 5000;
uint16_t       fftMinAmp = 1200;

int            WSLEDScrollSpd = 25;
byte           ScrollH = 1;
//Kronos FFTDel;

// SPECTRUM //
bool compressFFT = false;
bool FFTDoneComputing = false;
float norm_base = 0.76;
byte bin_reject = 2;
byte spectrum_coef = 6; // nie wiekszy niz SAMPLES/2/36 + bin_reject (7 = 245)
float normalizer = norm_base;
float norm_gain = 0.00125;  // base (0.00125) 
                          // max  (0.08) 

// VU Meter
uint16_t VU_Samples = 50;
unsigned long VU =0;
uint16_t VU_MinAmp = 50;
uint16_t VU_MaxAmp = 1800;
uint16_t MaxMap = 0; //(po przekatnej 36,8...)
bool VU_Done = false;
byte VU_QX = 0;
byte VU_MX = 0;
byte VU_MX_p = 0;
bool RMS = true;
uint16_t falloffDel = 25;
byte HMirrorOffset = 0;
bool IsFALLOFF = true;
bool MIRRORING = false;

Kronos k1;
Kronos DrawDesk;
Kronos adxl;
Kronos ds13;
Kronos baseIcons;
Kronos batDel;

 bool radio_OK = true;
 bool rtc_OK = true;
 bool adxl_OK = true;
 bool bmp_OK = true;
 bool aht_OK = true;
 bool tof_OK = true;
// # # # # # # # # # # # # #  #  #  #  #   G L O B A L N E
#define TFT_LED 4
#define BUZZ 33
byte TFT_LED_V = 128;
byte TFT_LED_PERCENT = 25;
#define MIC 36
#define BAT 39
byte LAST_BTN = 0;

byte RadioLED[9]={};

byte soundID = 0;
// # # # # # # # # # # # # #  #  #  #  #   T O U C H (To store the touch coordinates)
uint16_t x = 0, y = 0;
boolean pressed;
unsigned long P_TIMER = 0;
unsigned long P_TIME = 0;
unsigned int LONG_PRESS = 1000;
byte wasReleased = 0;
  
// # # # # # # # # # # # # #  #  #  #  #   D E S K T O P
byte TFT_ORIENT = 3;
byte ICON_W = 60, ICON_H = 60;
const byte GRID_X = 6, GRID_Y = 4;
// Glowna zmienna dla odpalonego poragmu (0=pulpit)
byte CURRENT_PROGRAM_ID;

// ZMIENNE DLA CZUJNIKOW
VL53L0X_RangingMeasurementData_t TOFdist;
sensors_event_t ADXLevent; 
float adX, adY, adZ;
DateTime  nowT;
uint16_t tofR;
bool tofOOR = false;
float ahtT, ahtH;
float bmpP;
// Sprawdzac czujniki ?, przeprowadzac FFT ?
bool SENSORS_RUN = true;
bool FFT_RUN = false;
bool VU_RUN = false;

// Afirmacja czujnikow
bool TOFDone=false, ADXDone=false, RTCDone=false, AHTDone=false, BMPDone=false;
bool TOFReq=false, ADXReq=false, RTCReq=false, AHTReq=false, BMPReq=false;

// chwilowe do testow + - + - + -
byte tmp1, tmp2;
bool test33 = true;

// Zmienne czasu RTC
int YearRTC=2020, MonthRTC=1, DayRTC=1, HourRTC=12, MinuteRTC=0, SecondRTC=0;

//==================================================== MOJE KLASY (RagClss.h inc.)
#include "ComplexShapes.h"
#include "MyButtons.h"    // (moje klasy, buttony, okna, ikony)
         Window window; // glowne okno programow

       // Obiekty RF
       Btn Ser1m; Btn Ser1p; Btn Ser2m; Btn Ser2p; Btn Ser3m; Btn Ser3p; Btn Ser4m; Btn Ser4p;  Btn SerSUB;
       Toggler tog1(&test33, 250);
       Kronos subDel;

       // Setclock
       Btn SetClock;

       byte sers[4] = {};
       byte sersPrev[4] = {};
       
#include "ControlsElem.h"  
wLed WLRow1[36] = wLed(0,0,0);
wLed WLRow2[36] = wLed(0,0,0);
wLed WLRow3[36] = wLed(0,0,0);
wLed WLRow4[36] = wLed(0,0,0);
wLed WLRow5[36] = wLed(0,0,0);
wLed WLRow6[36] = wLed(0,0,0);
wLed WLRow7[36] = wLed(0,0,0);
wLed WLRow8[36] = wLed(0,0,0);
wLed* WLED[8];    
//bufor dla kolumny WSLed do przesowania
wLed WLED_buff[8] = wLed(0,0,0);

wLed WLSRow1[36] = wLed(0,0,0);
wLed WLSRow2[36] = wLed(0,0,0);
wLed WLSRow3[36] = wLed(0,0,0);
wLed WLSRow4[36] = wLed(0,0,0);
wLed WLSRow5[36] = wLed(0,0,0);
wLed WLSRow6[36] = wLed(0,0,0);
wLed WLSRow7[36] = wLed(0,0,0);
wLed WLSRow8[36] = wLed(0,0,0);
wLed* WLEDS[8];  
#include "MyPrograms.h"


  // // // Dwa przayciski do brighteness + i - (wskazniki)
  Btn* menuBtns[2];


// Adresy ValElementow RTC
ValElem *RTCobejcts[6];
//                 int *val, int vmin,   int vmax,   INCREMENT(i/f)  const char *NAZWA
ValElem YearObj  ( &YearRTC,     2010,       2050,         1,       "Czas: Rok");
ValElem MonthObj ( &MonthRTC,       1,         12,         1,       "Czas: Miesiac");
ValElem DayObj   ( &DayRTC,         1,         31,         1,       "Czas: Dzien");
ValElem HourObj  ( &HourRTC,        0,         24,         1,       "Czas: Godzina");
ValElem MinuteObj( &MinuteRTC,      0,         60,         1,       "Czas: Minuta");
ValElem SecondObj( &SecondRTC,      0,         60,         1,       "Czas: Sekunda");

// Adresy ValElementow FFT
ValElem *FFTObejcts[16];
//                    int *val,        i/f vmin,   i/f vmax,     inc      const char *NAZWA
ValElem FFTNR      ( &noise_reduction,     0,       1500,         2,       "ADC: Noise");
ValElem FFTAMPNR   ( &fftMinAmp,           0,       6000,        25,       "FFT: Noise");
ValElem FFToffset  ( &MicOffset,        1000,       4000,         1,       "FFT: Offset");
ValElem FFTAmp     ( &micMaxAmp,         500,      16000,       100,       "FFT: MaxAmp");
ValElem FFTNorBase ( &norm_base,         0.0,        2.5,      0.025,      "Norm Base");
ValElem FFTNorGain ( &norm_gain,     0.00025,        0.1,    0.00015,      "Norm Gain");
ValElem WSScrlSpd  ( &WSLEDScrollSpd,   -200,        200,          2,      "Scroll Speed");
ValElem WScrollH   ( &ScrollH,             0,        255,          1,      "Scroll HUE++");
ValElem ADCPreScal ( &ADC_PRESCALAR,     1.0,       10.0,        0.1,      "ADC Prescalar");
ValElem BinReject  ( &bin_reject,          0,         10,          1,      "Bin Reject");
ValElem SpectrCoef ( &spectrum_coef,       1,          7,          1,      "FFT Width <1-7>");
ValElem VU_SAMPLEX ( &VU_Samples,         10,        250,          5,      "VU: Samples N");
ValElem VU_MINAMP  ( &VU_MinAmp,           0,       2000,         25,      "VU: MinAmp");
ValElem VU_MAXAMP  ( &VU_MaxAmp,         500,       4500,        100,      "VU: MaxAmp");
ValElem VU_FODEL   ( &falloffDel,          0,        250,          1,      "VU/FFT: Falloff MS");
ValElem MIRR_OFSET ( &HMirrorOffset,       0,        255,          1,      "H Offset FFT/Mirr.");

//==================================================== MOJE KLASY END


//#define CALIBRATION_FILE "/TouchCalData2"

//####################################################################################################
// Zmienne globalne
//####################################################################################################
  // ############### sciezki do tapet (portret)
  char* fpatch[] = {"/sd1.jpg","/sd2.jpg","/sd3.jpg","/sd4.jpg"};
  char* fpatchBMP[] = {"/sd1.bmp","/sd2.bmp","/sd3.bmp","/sd4.bmp"};

  //############# Sciezki do ikon       OPIS               INDEX
  char* ipatch[] = {"/bb1.jpg",     // opcje (gear)           0
                    "/bb2.jpg",     // drawer (lista)         1
                    "/bb3.jpg",     // pilot                  2
                    "/bb4.jpg",     // WS LED                 3
                    "/bb5.jpg",     // kalkulator             4
                    "/bb6.jpg",     // zegar                  5
                    "/bb7.jpg",     // zdjecia                6
                    "/bb8.jpg",     // notatnik               7
                    "/bb9.jpg",     // wykresy (bary)         8
                    "/bb10.jpg",    // pogoda                 9
                    "/bb11.jpg",    // PWM suwaki             10
                    "/bb12.jpg",    // slonce/brtness+        11           
                    "/bb13.jpg",    // slonce/brtness-        12   
                    "/bb14.jpg",    // wykresy(red) arr2      13           
                    "/bb15.jpg",    // 3xcolor wheels         14     
                    "/bb16.jpg",    // kolory/pedzel          15
                    "/bb17.jpg",    // gauge(wht bg)          16
                    "/bb18.jpg",    // signal (orange)        17
                    "/bb19.jpg",    // folder                 18
                    "/bb20.jpg",    // fft/mic (bars)         19
                    "/bb21.jpg",    // "i" info               20
                    "/bb22.jpg",    // DS czujnik             21
                    "/bb23.jpg",    // servo icon             22
                    "/bb24.jpg",    // Arrow-L                23
                    "/bb25.jpg",    // Arrow-R                24
                    "/bb26.jpg",    // STAT(PC) nice          25
                    "/bb27.jpg",    // Gyro icon              26
                    "/bb28.jpg",    // Gauges(3)multi         27
                    "/bb29.jpg",    // Termometr              28
                    "/bb30.jpg",    // Graph(wht bg)          29 
                    "/bb31.jpg",    // bars(3) another        30
                    "/bb32.jpg",    // Gauge#1                31
                    "/bb33.jpg",    // Gauge#2                32
                    "/bb34.jpg",    // Gauge#3                33
                    "/bb35.jpg",    // Osciloscope            34
                    };  


  //################ Sciezki do background (landscape)
  char* bgpatch[] = {"/bg1.jpg",
                     "/bg2.jpg",
                     "/bg3.jpg",
                     "/bg4.jpg",
                     "/bg5.jpg",
                     "/bg6.jpg",
                     "/bg7.jpg",
                     "/bg8.jpg",
                     "/bg9.jpg",
                     "/bg10.jpg",
                     "/bg11.jpg",}; 
                     

 /*
  byte TFT_ORIENT = 3;
  byte ICON_W = 60, ICON_H = 60;
  const byte GRID_X = 6, GRID_Y = 4;
 */
 
DesktopItem* D_GRID[GRID_Y][GRID_X];
/*
 * GRID EKRANU 6x4 (icony xy 60+20)
 * 
 * p19  p20   p21   p22   p23   p24
 * p13  p14   p15   p16   p17   p18
 * p7   p8    p9    p10   p11   p12
 * p1   p2    p3    p4    p5    p6
 */

byte GRID_TOUCH[GRID_Y][GRID_X] = 
{
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0}
};
 //   OBIEKTY Z BAZOWYMI IKONAMI ( APLIKACJE )
//klasa  nazwa(obj) sciezka(sd)     Nazwa      ID     W       H
Shortcut sSettings( ipatch[0],     "Opcje",    10, ICON_W, ICON_H);
Shortcut sDrawer(   ipatch[1],     "Drawer",   11, ICON_W, ICON_H);
Shortcut sPilot(    ipatch[2],     "Pilot",    12, ICON_W, ICON_H);
Shortcut sWS28LED(  ipatch[3],     "LED",      13, ICON_W, ICON_H);
Shortcut sCalc(     ipatch[4],     "Calc",     14, ICON_W, ICON_H);
Shortcut sClock(    ipatch[5],     "Clock",    15, ICON_W, ICON_H);
Shortcut sImages(   ipatch[6],     "Zdjecia",  16, ICON_W, ICON_H);
Shortcut sNotes(    ipatch[7],     "Notes",    18, ICON_W, ICON_H);
Shortcut sCharts(   ipatch[8],     "Wykresy",  19, ICON_W, ICON_H);
Shortcut sWeather(  ipatch[9],     "Pogoda",   20, ICON_W, ICON_H);

Shortcut sWSDraw(  ipatch[15], "WS_Rysowanie", 21, ICON_W, ICON_H);

 //   GLOWNE METODY ZARZADZANIA DESKTOPem
 bool addShortcut(DesktopItem *item)    // dodaj wksaznik do skrotu w kolejnym wolnym miejscu GRIDu
 {
  for (byte Y=GRID_Y-1; Y>=0; Y--)
  {
    for (byte X=0; X<GRID_X; X++)
    {
      if (D_GRID[Y][X]==NULL)
      {
        D_GRID[Y][X] = item;
        return true;
      }
    }
  }
  return false;
 }

 byte popIcons()  // renderuj ikony na podstawie czy jest jakis wskaznik w GRIDzie
 {
  byte iconCounter = 0;
  for (byte Y=0; Y<GRID_Y; Y++)
  {
    for (byte X=0; X<GRID_X; X++)
     {
       if (D_GRID[Y][X] != NULL)
       {
         D_GRID[Y][X]->draw(X,Y);
         iconCounter++;
       }
     }
    }
  return iconCounter;
  }

 void gridReadTouch(uint16_t x, uint16_t y) // NARAZIE ZOSTAWIC
 {
  for (byte yy=0; yy<GRID_Y; yy++)
  {
    for (byte xx=0; xx<GRID_X; xx++)
    {
      
    }
  }
 }

 void gridIconTouch()
 {
  for (byte gY=0; gY<GRID_Y; gY++)
  {
    for (byte gX=0; gX<GRID_X; gX++)
    {
      if ( D_GRID[gY][gX]!=NULL )
      {
        D_GRID[gY][gX]->chkIfPress(); // sprawdz dotyk tylko tam gdzie jest
                                      // wskaznik na klase pochodna z "DesktopItem"
      }
    } //xx
  } //yy
 }

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }
 
//############################################################################################################################################# SETUP
// Setup
//############################################################################################################################################# SETUP
void setup() {

  Serial.begin(115200);
    Serial.println("Serial utworzony, startuje Wire...");
  Wire.begin (21, 22);   // sda= GPIO_21 /scl= GPIO_22
    Serial.println("Wire wystartowalo.");
  delay(stDel*2);

  // Radio prestart
  if (radio.begin()) { radio_OK = true; }
  else { radio_OK = false; }
  radio.openWritingPipe(address); delay(50); // na poczatek dla SERV
  radio.setPALevel(RF24_PA_LOW); delay(50);
  radio.stopListening(); delay(50);
  radio.setRetries(15, 15); delay(50);
  radio.powerUp(); delay(50);
  radio.setDataRate(RF24_250KBPS); delay(50);
  radio.enableDynamicPayloads();
  delay(stDel);

  tft.begin();
  tft.setRotation(TFT_ORIENT);
  
  Serial.println("Startuje i2c scanner...");
  Scanner();
  delay(stDel);
  ScannerTFT();
  delay(stDel);
  
  tft.fillScreen(TFT_BLUE);
  tft.setCursor(0,0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextFont(4);
  tft.println("BOOTING...");
  Serial.println("Koncze start TFT");
  delay(stDel);
  
  Serial.println("I2c skac ukonczono.");
  delay(stDel);
  // Set all chip selects high to avoid bus contention during initialisation of each peripheral
  digitalWrite(15, HIGH); // Touch controller chip select (if used)
  digitalWrite(16, HIGH); // TFT screen chip select
  digitalWrite( 5, HIGH); // SD card chips select, must use GPIO 5 (ESP32 SS)
  digitalWrite( 26, HIGH); // radio SC
  digitalWrite( 27, HIGH); // radio CSC
  
  //FastLED
  Serial.println("FastLED --- tworze...");
  LEDS.addLeds<WS2812B,DATA_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(128);
  
  tft.println("P Set HIGH: 15,16,5,26,27");
  Serial.println("i2c start skonczony, piny ustawione.");
  delay(stDel);   
   
  //=============================Task dla rdzenia (0) - main loop() jest na core 1
  xTaskCreatePinnedToCore(
                    coreTask,   /* Function to implement the task */
                    "Task1",    /* Name of the task */
                    10000,      /* Stack size in words */
                    NULL,       /* Task input parameter */
                    0,          /* Priority of the task */
                    &Task1,     /* Task handle. */
                    0);         /* Core where the task should run */
                    
//=============================Task dla rdzenia (0)
  Serial.println("Task na rdzeniu 0 utwprzony.");
  tft.println("Task started at core 0");
  delay(stDel);

  // Wyzeruj GRID dla ikon
  for (byte j=0; j<GRID_Y; j++)
  {
    for (byte i=0; i<GRID_X; i++)
    {
      D_GRID[j][i] = NULL;
    }
  }

  // INFO, INFO , INFO ... ... ...
  Serial.println("Task created...");
  Serial.println(F("Sensor inicialization: . . ."));
  tft.println("SENSORT INITALIZATION:");

  // RADIO
  if (!radio_OK) { Serial.println(F("radio hardware is not responding!!")); tft.setTextColor(TFT_WHITE,TFT_RED); tft.println("RADIO not responding!"); }
  else {Serial.println(F("Radio go to go!")); tft.setTextColor(TFT_WHITE); tft.println("RADIO --- OK"); }

  // RTC
  if (! rtc.begin())
  {
    Serial.println(F("Couldn't find RTC")); 
    tft.setTextColor(TFT_WHITE,TFT_RED);
    tft.println("RTC not responding!");
    rtc_OK = false;
  }
  if (rtc.isrunning()) {Serial.println(F("RTC is go to go!")); tft.setTextColor(TFT_WHITE); tft.println("RTC --- OK"); }
  else Serial.println(F("RTC is NOT running - Err"));
    //                      data i czas kompilacji
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //                      y    m   d  h  m  s
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  delay(stDel);

  // ACCEL ADXL
  if(!accel.begin()) 
  { 
    Serial.println(F("No ADXL345 detected ... Check your wiring!")); 
    tft.setTextColor(TFT_WHITE,TFT_RED);
    tft.println("ADXL not responding!");
    adxl_OK = false;
  }
  else {Serial.println(F("ADXL OK")); tft.setTextColor(TFT_WHITE); tft.println("ADXL --- OK"); }
  accel.setRange(ADXL345_RANGE_16_G); // (RANGE_16,8,4,2
  delay(stDel);

  // BMP
  if (!bmp.begin()) 
  { 
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.println("BMP not responding!");
    bmp_OK = false;
  }
  else {Serial.println("BMP180 OK"); tft.setTextColor(TFT_WHITE); tft.println("BMP --- OK"); }
  delay(stDel);

  // AHT10
  if (!aht10.begin()) 
  { 
    Serial.println(F("AHT10 not connected or fail to load calibration coefficient"));
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.println("AHT10 not responding!");
    aht_OK = false;
  }
  else {Serial.println(F("AHT10 OK")); tft.setTextColor(TFT_WHITE); tft.println("AHT10 --- OK"); }
  delay(stDel);

  // TOF LOX  
  if (!lox.begin()) 
  {
  Serial.println(F("Failed to boot VL53L0X"));
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.println("TOF not responding!");
  tof_OK = false;
  }
  else {  Serial.println(F("TOF LOX OK")); tft.setTextColor(TFT_WHITE); tft.println("TOF --- OK"); }
  delay(stDel);
    
  Serial.println("Koncze init sensorow.");
  delay(stDel*2);

  // Konfiguracja mikrofonu
  analogReadResolution(12);
  analogSetCycles(samples_per_cycle);
  adcAttachPin(MIC);
  analogSetPinAttenuation(MIC, ADC_6db);
  // MIC Offset / VOLT - ADC Read - AttenDb
  //             1.25v - 2326.7    ( 6db - 2.2v ) - mic max 2v = 3722.7
  //             1.25v - 1312.5    ( 11db- 3.9v )
  
  // I ADC dla baterii LiPo
  adcAttachPin(BAT);
  analogSetPinAttenuation(BAT, ADC_11db);
  
  // Konfiguracja PWM dla poedswietlenia TFT  kanal - 1, res - 8 bit
  ledcAttachPin(TFT_LED, 1);
  ledcSetup(1, 120, 8);
  ledcWrite(1, TFT_LED_V);

  // Konf PWM dla BUZZERA na kanale (2), na pinie (33), rozdzielczosc (8) bit
  pinMode(BUZZ, OUTPUT);
  ledcAttachPin(BUZZ, 2);
  ledcSetup(2, 0, 8);
 
  tft.fillScreen(TFT_BLUE);
  tft.setCursor(0,0);

  tft.setTextColor(TFT_WHITE);
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    tft.println("SD - card mout failed!");
  }
  else tft.println("SD OK and Mounted");
  
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    tft.println("No SD card attached");
  }

//======================================================= KALIBRACJA
 uint16_t calDataP0[5] = { 285, 3510, 253, 3618, 4 }; // 0 (portret 1 - bazowy od strony kabla(gora))
 uint16_t calDataL1[5] = { 284, 3619, 235, 3554, 7 }; // 1 (landscape 1 - odwrocony)
 uint16_t calDataP2[5] = { 228, 3584, 277, 3648, 2 }; // 2 (portret 2)
 uint16_t calDataL3[5] = { 239, 3661, 215, 3564, 1 }; // 3 (landscape 2 - wlasciwy)
 
  switch (TFT_ORIENT)
  {
    case 0:{tft.setTouch(calDataP0); break;}
    case 1:{tft.setTouch(calDataL1); break;}
    case 2:{tft.setTouch(calDataP2); break;}
    case 3:{tft.setTouch(calDataL3); break;}
    default:{tft.setTouch(calDataP0); break;}
  }
//======================================================= KALIBRACJA END  

  Serial.print("SD Card Type: ");
  tft.println("SD Card Type: ");

  if (cardType == CARD_MMC) {
    Serial.println("MMC");           tft.println("MMC");
  } else if (cardType == CARD_SD){
    Serial.println("SDSC");          tft.println("SDSC");
  } else if (cardType == CARD_SDHC){
    Serial.println("SDHC");          tft.println("SDHC");
  } else {
    Serial.println("UNKNOWN");       tft.println("UNKOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  uint32_t cardSmall = cardSize;
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  tft.println("SD Size: "+String(cardSmall) + " MB" );
  
  Serial.println("initialisation done.");

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
  delay(stDel*2);

// Lists the files so you can see what is in the SPIFFS  
Serial.println("Listuje co jest na SPIFFS..........................");
//listFiles(SPIFFS, "/", 0);
Serial.println("Listuje co jest na SD CARD..........................");
//listFiles(SD, "/", 0);

RTCobejcts[0] = &YearObj;
RTCobejcts[1] = &MonthObj;
RTCobejcts[2] = &DayObj;
RTCobejcts[3] = &HourObj;
RTCobejcts[4] = &MinuteObj;
RTCobejcts[5] = &SecondObj;

FFTObejcts[0] = &FFTNR;
FFTObejcts[1] = &FFTAMPNR;
FFTObejcts[2] = &FFTAmp;
FFTObejcts[3] = &FFTNorBase;
FFTObejcts[4] = &FFTNorGain;
FFTObejcts[5] = &WSScrlSpd;
FFTObejcts[6] = &ADCPreScal;
FFTObejcts[7] = &BinReject;
FFTObejcts[8] = &SpectrCoef;
FFTObejcts[9] = &FFToffset;
FFTObejcts[10] = &VU_SAMPLEX;
FFTObejcts[11] = &VU_MINAMP;
FFTObejcts[12] = &VU_MAXAMP;
FFTObejcts[13] = &VU_FODEL;
FFTObejcts[14] = &MIRR_OFSET;
FFTObejcts[15] = &WScrollH;


  menuBtns[0] = new Btn;
  menuBtns[1] = new Btn;

WLED[0] = WLRow1;
WLED[1] = WLRow2;
WLED[2] = WLRow3;
WLED[3] = WLRow4;
WLED[4] = WLRow5;
WLED[5] = WLRow6;
WLED[6] = WLRow7;
WLED[7] = WLRow8;

WLEDS[0] = WLSRow1;
WLEDS[1] = WLSRow2;
WLEDS[2] = WLSRow3;
WLEDS[3] = WLSRow4;
WLEDS[4] = WLSRow5;
WLEDS[5] = WLSRow6;
WLEDS[6] = WLSRow7;
WLEDS[7] = WLSRow8;
  Serial.println("Koncze setup, startuje Desktop...");
  
} /// SETUP END ====================================================================================================

//====================================== Task przypiety do rdzenia (0, main loop jest na 1)

Kronos Core1Del;
void coreTask( void * pvParameters ){ //########################################################################################## CORE 0
 
    //String taskMessage = "Odzywa sie rdzen nr.: ";
    //taskMessage = taskMessage + xPortGetCoreID();

    //################################ GLOWNA PETLA DLA DRUGIEGO (1) RDZENIA
    for(;;){

        // ODCZYTY Z CZUJNIKOW --------------------------------------------------------------------------------------------------------
        if (SENSORS_RUN==true) // 5 czujnikow na i2c - 1.ADXL 2.TOF 3.RTC 4.AHT 5.BMP
        {
          if (Core1Del.del(10)){
            // Zbierz z akcelerometru
            if (adxl_OK==true && ADXReq==true)
            { //ADXDone = false
              accel.getEvent(&ADXLevent);  
              adX = ADXLevent.acceleration.x;
              adY = ADXLevent.acceleration.y;
              adZ = ADXLevent.acceleration.z;
              
              //ADXDone = true;
              ADXReq = false;}
            // Zbierz z TOF
            if (tof_OK==true && TOFReq==true)
              {
              lox.rangingTest(&TOFdist, false);
               if (TOFdist.RangeStatus != 4)
                {tofR = TOFdist.RangeMilliMeter; TOFReq = false; tofOOR = false; }
               else {tofOOR=true; TOFReq = false; }
              }
            // Zbierz RTC
            if (rtc_OK==true && RTCReq==true) {nowT = rtc.now(); RTCReq = false;}
            // Zbierz AHT
            if (aht_OK==true && AHTReq==true) {ahtT = aht10.readTemperature(); ahtH = aht10.readHumidity(); AHTReq = false;}
            // Zbierz BMP
            if (bmp_OK==true && BMPReq==true) {bmpP = bmp.readPressure()/100.f; BMPReq = false;}
        }}
        
        // ------- FFT
        if (FFT_RUN == true)
        {
            for (int i = 0; i < SAMPLES; i++)
            {
            microsecFFT = micros();
            sum = int((analogRead(MIC) - MicOffset)/ADC_PRESCALAR);
            if (sum > -noise_reduction && sum < noise_reduction) {
              sum = 0;
            }
            //Serial.println(sum);
            vReal[i] = sum;
            vImag[i] = 0;
    
            while (micros() < microsecFFT+SF){}
            //delayMicroseconds(SF);
            }
          /*FFT*/
          FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
          FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
          FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

          #if ALIAS == 2 /// NORMALIZER 
                normalizer = norm_base;
                for (int i = 0; i < SAMPLES / 2; i++)
                {
                  normalizer+=norm_gain;
                  vReal[i] = vReal[i]*normalizer;
                }
          #endif
          
          
          /* ------------- MAPOWANIE NA mapped[256] ------------- */
            for (int i = 0; i < SAMPLES / 2; i++)
            {
              vReal[i] = round(vReal[i]);
              if (vReal[i] > micMaxAmp) vReal[i] = micMaxAmp;
              if (vReal[i] < fftMinAmp) vReal[i] = fftMinAmp;
              mapped[i] = map(int(vReal[i]), fftMinAmp, micMaxAmp, 0, 8);
            }
         //FFTDoneComputing = true;
        }
// --------------------------------------------------------------------------- FFT END

/*
// ---- VU METER
        if (VU_RUN == true)
        {
            VU_QX = 0;
            VU = 0;
            for (int i = 0; i < VU_Samples; i++)
            {
            microsecFFT = micros();
            VU += static_cast<unsigned long>(abs((analogRead(MIC) - MicOffset)/ADC_PRESCALAR));

            while (micros() < microsecFFT+SF){}
            //delayMicroseconds(SF);
            }
            VU /= VU_Samples;
            if (VU < VU_MinAmp) VU = 0;
            if (VU > VU_MaxAmp) VU = VU_MaxAmp;
            VU = map(VU, 0, VU_MaxAmp, 0 , 255);
            VU_QX = static_cast<byte>(VU);
            //Serial.println(VU);
            //delay(500);
        }
// -- VU END
*/
        // BUZZER I DZWIEKI -------------------------------------------
        if (soundID == 1)
        {
          double snd = 500;
          for (int i=0; i<25; i++)
          {
            ledcWriteTone(2, snd);
            snd*=1.25;
            delay(10);
          } 
          soundID = 0;   
          ledcWriteTone(2, 0);  
        }

        if (soundID == 2)
        {
          double snd = 15625;
          for (int i=0; i<25; i++)
          {
            ledcWriteTone(2, snd);
            snd/=1.25;
            delay(10);
          } 
          soundID = 0;   
          ledcWriteTone(2, 0);     
        }
        // BUZZER I DZWIEKI -------------------------------------------
        
    vTaskDelay(1);
    }//######################### GLOWNA PETLA DLA DRUGIEGO (0) RDZENIA [END]
 
}//################################################################################################################################# CORE 1

// Values changer object        **ptr     int num
ValChngObjArrtList RTCSettings(RTCobejcts, 6);
ValChngObjArrtList FFTSettings(FFTObejcts, 16);

/// OBIEKTY PROGRAMOW --- id tu nie jest narazie potrzebne ///
WSLED wsled(13);
RF_SER rfser(11);
CLOCK_SET clock_set(15, RTCSettings);
wLedPanel led_panel(21, FFTSettings);

// chwilowe testy
Slider sld1, sld2;
BatIndicator BatIndi;


void showSensorsLCD() // ======================================================================= SHOW SENSORS L C D ! ! !
{
  // SERIAL
  // Skaner I2C
  Serial.println("Aktualny Prog ID: "+String(CURRENT_PROGRAM_ID) );
  /*
  //AHT10 temp i humidity
  Serial.println(F("ATH10 test ==============================="));
  Serial.print(F("Temperature: ")); Serial.print(aht10.readTemperature()); Serial.println(F(" +-0.3C")); //by default "AHT10_FORCE_READ_DATA"
  Serial.print(F("Humidity...: ")); Serial.print(aht10.readHumidity());    Serial.println(F(" +-2%"));   //by default "AHT10_FORCE_READ_DATA"  
  // BMP180 Pressure n temp
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  //Serial.print("Altitude = ");
  //Serial.print(bmp.readAltitude());
  //Serial.println(" meters");
  // Data z RTC (na Serial)
  printDate();
  
  accel.getEvent(&ADXLevent);
  Serial.print("ADXL: - X: "); Serial.print(ADXLevent.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(ADXLevent.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(ADXLevent.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");  
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&TOFdist, false); // pass in 'true' to get debug data printout!

  if (TOFdist.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(TOFdist.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }
*/
  //========================== LCD

    //tft.setFreeFont(FSI12);

    tft.setTextDatum(TR_DATUM);
    tft.setTextSize(1);
    tft.setCursor(0,10);
    tft.setTextColor(TFT_WHITE,TFT_BLUE);
    //tft.setTextFont(7); cyfrowa (tylko liczby)
    tft.setTextFont(2);
    //tft.setTextSize(0);
    if (aht_OK) tft.println("Temp: "+String(ahtT)+" / Hum: "+String(ahtH));
    else tft.println("AHT Failed!");

    if (bmp_OK)  tft.println("Prs: "+String(bmpP)+" hPa");
    //tft.println("Alt: "+String(bmp.readAltitude())+"m");
    else tft.println("BMP Failed!");
    
    if (rtc_OK)
    {
    
    tft.setTextColor(TFT_WHITE,hsv2hex(218,255,220) );
    tft.println(String(nowT.year())+" / "+String(nowT.month())+" / "+String(nowT.day()));
    tft.setTextColor(TFT_GREEN,TFT_BLACK);
    tft.println(String(nowT.hour())+" : "+String(nowT.minute())+" : "+String(nowT.second()));
    }
    else tft.println("RTC Failed!");
    
    if (adxl_OK)
    {
    tft.setTextColor(TFT_WHITE,TFT_RED);
    tft.println("Akcelerometr:");     
    tft.setTextColor(TFT_GREEN,TFT_BLACK);
    tft.setTextFont(2);
    tft.println("X: "+String(adX)+" Y: "+String(adY)+" Z: "+String(adZ));
    }
    else tft.println("ADXL Failed!");
    
    if (tof_OK)
    {
      if (tofOOR==false)
    {tft.println("TOF Dist: "+String(tofR)+" mm");}
    else { tft.println("TOF: Out of range."); }
    }
    else tft.println("TOF Failed!");

    //Zarzadaj danych z czujnikow:
    RTCReq = true;
    AHTReq = true;
    BMPReq = true;
    ADXReq = true;
    TOFReq = true;
    
    // BATERIA
      float batRaw = 0;
  for (byte i=0; i<25; i++)
  {
    batRaw+=analogRead(BAT);
  }
  batRaw/=25;
  uint16_t batRawInt = static_cast<uint16_t>(batRaw);

  float batVolt = 3.9/4095.*batRaw*2.f; // 3.9v dla atten 11db

  if (batVolt>2) tft.println("RawVal: "+String(batRaw,2)+", Vcc: "+String(batVolt,4));
  // Szczyty
  // MAX Bat -- 4.2v -- 2205
  // MIN Bat -- 3.2v -- 1680
  // DIFF    -- 1v   -- 525   - 1.9mV / 1 ar
  
  // Ekranowy indicator
  BatIndi.bUpd(batRawInt,1680,2205);

} // showSensorsLCD()


//############################################################################################################################################# C0 LOOP
// Main loop
//############################################################################################################################################# C0 LOOP
void loop() {

if (baseIcons.ONCE())
{
Serial.println("########### DODAJE BAZOWE IKONY");
//addShortcut(&sSettings);    // id_prog 10
addShortcut(&sDrawer);      // id_prog 11
addShortcut(&sPilot);       // id_prog 12
addShortcut(&sWS28LED);     // id_prog 13
//addShortcut(&sCalc);        // id_prog 14
addShortcut(&sClock);       // id_prog 15
addShortcut(&sWSDraw);       // id_prog 21
//addShortcut(&sImages);      // id_prog 16
//addShortcut(&sNotes);       // id_prog 17
Serial.println("########### KONCZE DODAWAC");  
}

if (DrawDesk.ONCE())
{
  byte randWpp = random(0,11);
  drawSdJpeg(bgpatch[randWpp], 0 , 0);
  menuBtns[0]->draw(210,10,80,40,"L+");
  menuBtns[1]->draw(300,10,80,40,"L-");
  Serial.println("Stworzylem: "+String(popIcons())+" ikon." );
  BatIndi.power = false;
  BatIndi.draw(400,10,60);
  SENSORS_RUN = true;
  // CURRENT_PROGRAM_ID = 0; // Resetuj program (juz robi to samo zamkniecie okna)
}

if (adxl.del(500) && CURRENT_PROGRAM_ID==0) showSensorsLCD(); // ==== Dane z czujnikow -> EKRAN

   
  ///////////////////////////////////// GLOWNE SPRAWDZANIE DOTYKU 
pressed = tft.getTouch(&x, &y);

if (pressed == true && CURRENT_PROGRAM_ID==0 )
  {
    wasReleased = 0;
    //Serial.println("WCISNIETY X: "+String(x) +"  Y: "+String(y) );
    gridIconTouch();

    for (byte i=0; i<2; i++)
    {
     if (menuBtns[i]->chkIfPress(x,y))
      {
        LAST_BTN = i+1;
        break;
      }
    }
  } // touch
  
  if (pressed == false && wasReleased==0 && CURRENT_PROGRAM_ID==0 )
  {
    wasReleased = 1;
    x=500; y=500;     // zmien kordynaty dotyku za ekran
    gridIconTouch();  // Wykona sie raz po puszczeniu dla IKON
  }
  
  ///////////////////////////////////// GLOWNE URUCHAMIANIE APLIKACJI I AKCJI PO DOTYKU
  //SWICH
  pressed = tft.getTouch(&x, &y);
  switch(CURRENT_PROGRAM_ID)
  {
    case 13:
    {
      // 3x Led stripy
      wsled.Run();
      break;
    }
    case 15:
    {
      // Clock set
      clock_set.Run();
      break;
    }
    case 12:
    {
      // TESTY
      if (window.alreadyDraw==false) 
      {
        window.draw("Testy", hsv2hex(156,34,255) );
        window.alreadyDraw = true;
        Serial.println("Uruchamiam program o ID: "+String(CURRENT_PROGRAM_ID) );
      }
      if (pressed) 
      {
        if (window.chkIfClosed() )
        {
          window.alreadyDraw = false;
          sld1.alreadyDraw==false;
          sld2.alreadyDraw==false;
        }
        sld1.upd(&tmp1);
        sld2.upd(&tmp2);
      }

      if ( sld1.alreadyDraw==false && sld2.alreadyDraw==false)
      {
        sld1.draw(10,50,460,30,0xFFFF,0xFFFF);
        sld1.alreadyDraw = true;
       
        sld2.draw(50,140,380,60,0xFFFF,0xFFFF);
        sld2.alreadyDraw = true;
      } 
      break;
    }
    case 11:
    {
      // RF Serva Radio
      rfser.Run();
      break;
    }
    case 21:
    {
      // WS LED Panel
      led_panel.Run();
      break;
    }
    default:
    {
      CURRENT_PROGRAM_ID = 0;
    }
  }
  //SWICH END

  ///////////////////////////////////// LED BACKLIGHT
 if (LAST_BTN!=0)
  {
    if (LAST_BTN==1 && TFT_LED_PERCENT<100)
    {
      TFT_LED_PERCENT++;
      LAST_BTN=0;
    }
    
    else if (LAST_BTN==2 && TFT_LED_PERCENT>0)
    {
      TFT_LED_PERCENT--;
      LAST_BTN=0;
    }

    else LAST_BTN = 0;

    TFT_LED_V = byte(255-(2.5*TFT_LED_PERCENT));
    Serial.println("TFT_LED PWM val: "+String(TFT_LED_V)+" / "+String(TFT_LED_PERCENT)+"%");
    ledcWrite(1, TFT_LED_V);
  }

/*  //  TOUCH 2 GEN
  pressed = tft.getTouch(&x, &y);
  if (pressed)
  {

  }

 
*/
  
  /*
  tft.setRotation(2);  // portrait
  tft.fillScreen(random(0xFFFF));

  tft.setRotation(2);  // portrait
  drawSdJpeg("/sd1.jpg", 0, 0);     // This draws a jpeg pulled off the SD Card
  delay(2000);

  tft.setRotation(2);  // portrait
  tft.fillScreen(random(0xFFFF));
  drawSdJpeg("/sd2.jpg", 0, 0);     // This draws a jpeg pulled off the SD Card
  delay(2000);

  tft.setRotation(2);  // portrait
  tft.fillScreen(random(0xFFFF));
  drawSdJpeg("/sd3.jpg", 0, 0);     // This draws a jpeg pulled off the SD Card
  delay(2000);

  tft.setRotation(2);  // landscape
  tft.fillScreen(random(0xFFFF));
  drawSdJpeg("/sd4.jpg", 0, 0);     // This draws a jpeg pulled off the SD Card

  delay(2000);
*/
  //while(1); // Wait here
}
