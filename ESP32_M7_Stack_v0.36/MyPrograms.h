#ifndef MyPrograms_h
#define MyPrograms_h

#include "ControlsElem.h" 
//void drawSdJpeg(const char *filename, int xpos, int ypos);  // deklaracja renderu JPEG
//void drawJpeg( const char *filename, int xpos, int ypos);

/* --------------- PROGRAMY ---------------- */
//############################################################################## LED
class WSLED
{
public:   //      Pid - generalnie nie uzywane narazie
  WSLED(byte id) : Program_ID(id), LedSelector(0), alreadyDraw(false) {}
  ~WSLED(){}

  void Run()
  {
      // Okno i start (once)
      if (window.alreadyDraw==false) 
      {
        radio.openWritingPipe(addrLed);
        radio.stopListening();
        radio.powerUp();
        
        window.draw("12v 3x Tasmy LED", hsv2hex(156,34,255) );
        this->drawBar_HS (20,120,440,140);
            // LED 1
            tft.fillRect(LS1X,LS1Y,80,40,TFT_NAVY);
            for (byte o=0; o<4; o++)
            { tft.drawRect(LS1X-o,LS1Y-o,80+2*o,40+2*o,TFT_RED); }
            // LED 2
            tft.fillRect(LS2X,LS2Y,80,40,TFT_NAVY);
            for (byte o=0; o<4; o++)
            { tft.drawRect(LS2X-o,LS2Y-o,80+2*o,40+2*o,TFT_NAVY); }
            // LED 3
            tft.fillRect(LS3X,LS3Y,80,40,TFT_NAVY);
            for (byte o=0; o<4; o++)
            { tft.drawRect(LS3X-o,LS3Y-o,80+2*o,40+2*o,TFT_NAVY); }
            
        window.alreadyDraw = true;
        Serial.println("Uruchamiam program o ID: "+String(CURRENT_PROGRAM_ID) );
      }

      // Wcisniety
      if (pressed) 
      {
        if (window.chkIfClosed() )    // Zamkniecie okna ustawia PROG.._ID=0 , RESetuje pulpit
        {
          window.alreadyDraw = false;
        } 
        else this->chkIfPress();
      } // (press==true)
      
      // Run
  }
  
  void drawBar_HS (uint16_t st_x, uint16_t st_y, uint16_t btnW, uint16_t btnH)
  {
    bX=st_x;
    bY=st_y;
    bW=btnW;
    bH=btnH;
    tft.drawRect(bX-1, bY-1, bW+2, bH+2, hsv2hex(0,0,0) );
    xCoef = float(255.f/bW); // 0,57954545
    yCoef = float(255.f/bH); // 1,82142857
    //rysuj bar 2d dla  H i S
    for (uint16_t iY=0; iY<bH; iY++)
    {
      for( uint16_t iX=0; iX<bW; iX++)
      {
        tft.drawPixel(iX+bX, iY+bY, hsv2hex(byte(xCoef*iX), 255-byte(yCoef*iY), 255)  );
      }
    }
    //rysuj bar dla  V
    tft.drawRect(bX-1,bY+bH+9, bW+2, 42, TFT_BLACK );
    for( uint16_t iX=0; iX<bW; iX++)
    {
      tft.drawFastVLine(bX+iX, bY+bH+10,40, hsv2hex(0,255, byte(xCoef*iX)));
    }
  }

  bool chkIfPress()
  {
     // 'H' i 'S' bar
      if (x>bX && x<bX+bW && y>bY && y<bY+bH)
      {
        H = byte((x-bX)*xCoef);
        S = byte(255-(y-bY)*yCoef);
        //V = byte(x-bX)*xCoef;
        for( uint16_t iX=0; iX<bW; iX++)
        tft.drawFastVLine(bX+iX, bY+bH+10,40, hsv2hex(H,S, byte(xCoef*iX)) );

        if (LedSelector==0){
        tft.fillRect(LS1X,LS1Y,80,40,hsv2hex(H,S,V));
        tft.drawRect(LS1X,LS1Y,80,40,TFT_BLACK);
        hsv2rgb3(H,S,V,&L1r,&L1g,&L1b);
        RadioLED[0] = L1r;
        RadioLED[1] = L1g;
        RadioLED[2] = L1b;
        bool report = radio.write(&RadioLED, sizeof(RadioLED));
        }

        else if (LedSelector==1){
        tft.fillRect(LS2X,LS2Y,80,40,hsv2hex(H,S,V));
        tft.drawRect(LS2X,LS2Y,80,40,TFT_BLACK);
        hsv2rgb3(H,S,V,&L2r,&L2g,&L2b);
        RadioLED[3] = L2r;
        RadioLED[4] = L2g;
        RadioLED[5] = L2b;
        bool report = radio.write(&RadioLED, sizeof(RadioLED));        
        }

        else if (LedSelector==2){
        tft.fillRect(LS3X,LS3Y,80,40,hsv2hex(H,S,V));
        tft.drawRect(LS3X,LS3Y,80,40,TFT_BLACK);
        hsv2rgb3(H,S,V,&L3r,&L3g,&L3b);
        RadioLED[6] = L3r;
        RadioLED[7] = L3g;
        RadioLED[8] = L3b;
        bool report = radio.write(&RadioLED, sizeof(RadioLED));
        }
      }
     // 'V' bar bX-1,bY+bH+9, bW+2, 42,
      if (x>bX && x<bX+bW && y>bY+bH+10 && y<bY+bH+10+40)
      {
        V = byte((x-bX)*xCoef); // poprawic

        if (LedSelector==0){
        tft.fillRect(LS1X,LS1Y,80,40,hsv2hex(H,S,V));
        tft.drawRect(LS1X,LS1Y,80,40,TFT_BLACK);
        hsv2rgb3(H,S,V,&L1r,&L1g,&L1b);
        RadioLED[0] = L1r;
        RadioLED[1] = L1g;
        RadioLED[2] = L1b;
        bool report = radio.write(&RadioLED, sizeof(RadioLED)); 
        //Serial.println("L1: (rgb) - "+String(L1r)+" / "+String(L1g)+" / "+String(L1b));
        }

        else if (LedSelector==1){
        tft.fillRect(LS2X,LS2Y,80,40,hsv2hex(H,S,V));
        tft.drawRect(LS2X,LS2Y,80,40,TFT_BLACK);
        hsv2rgb3(H,S,V,&L2r,&L2g,&L2b);
        RadioLED[3] = L2r;
        RadioLED[4] = L2g;
        RadioLED[5] = L2b;
        bool report = radio.write(&RadioLED, sizeof(RadioLED));  
        }

        else if (LedSelector==2){
        tft.fillRect(LS3X,LS3Y,80,40,hsv2hex(H,S,V));
        tft.drawRect(LS3X,LS3Y,80,40,TFT_BLACK);
        hsv2rgb3(H,S,V,&L3r,&L3g,&L3b); 
        RadioLED[6] = L3r;
        RadioLED[7] = L3g;
        RadioLED[8] = L3b;
        bool report = radio.write(&RadioLED, sizeof(RadioLED));
        }
      }
      // Selector
    if (x>LS1X && x<LS1X+80 && y>LS1Y && y<LS1Y+40)
      {
        LedSelector = 0;
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS1X-o,LS1Y-o,80+2*o,40+2*o,TFT_RED); }
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS2X-o,LS2Y-o,80+2*o,40+2*o,TFT_BLACK); }
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS3X-o,LS3Y-o,80+2*o,40+2*o,TFT_BLACK); }
      }
    else if (x>LS2X && x<LS2X+80 && y>LS2Y && y<LS2Y+40)
      {
        LedSelector = 1;
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS1X-o,LS1Y-o,80+2*o,40+2*o,TFT_BLACK); }
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS2X-o,LS2Y-o,80+2*o,40+2*o,TFT_RED); }
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS3X-o,LS3Y-o,80+2*o,40+2*o,TFT_BLACK); }       
      }
    else if (x>LS3X && x<LS3X+80 && y>LS3Y && y<LS3Y+40)
      {
        LedSelector = 2;
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS1X-o,LS1Y-o,80+2*o,40+2*o,TFT_BLACK); }
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS2X-o,LS2Y-o,80+2*o,40+2*o,TFT_BLACK); }
        for (byte o=0; o<4; o++)
        { tft.drawRect(LS3X-o,LS3Y-o,80+2*o,40+2*o,TFT_RED); }       
      }
  }


  // ZMIENNE PUBLICZNE
  bool alreadyDraw;
  
private:
byte Program_ID;
byte LedSelector;
uint16_t LS1X = 20, LS1Y = 70, LS2X = 110, LS2Y = 70, LS3X = 200, LS3Y = 70;
byte L1r, L1g, L1b, L2r, L2g, L2b, L3r, L3g, L3b;
uint16_t bX, bY, bW, bH;
byte H, S, V;
float xCoef;
float yCoef;
  
};

//############################################################################## RF SERVO
class RF_SER
{
public:
  RF_SER(byte ID) : Program_ID(ID) {};
  ~RF_SER(){};

  void Run()
  {
      // Okno
      //  ----------------------------------------------------------------------RAZ PO OTWORZENIU
     if (window.alreadyDraw==false) 
     {
       radio.openWritingPipe(address);
       radio.stopListening();
       radio.powerUp();
  
       window.draw("RF TX Transmitter", hsv2hex(156,34,255) );
       window.alreadyDraw = true;
       Serial.println("Uruchamiam program o ID: "+String(CURRENT_PROGRAM_ID) );

       Ser1m.draw(5,50, 100, 40, "S1-");
       Ser1p.draw(tft.width()-105,50, 100, 40, "S1+");
       
       Ser2m.draw(5,100, 100, 40, "S2-");
       Ser2p.draw(tft.width()-105,100, 100, 40, "S2+");
       
       //Ser3m.draw(5,150, 100, 40, "S3-");
       //Ser3p.draw(tft.width()-105,150, 100, 40, "S3+");
       
       //Ser4m.draw(5,200, 100, 40, "S4-");
       //Ser4p.draw(tft.width()-105,200, 100, 40, "S4+");
                            
       //SerMM.draw(tft.width()/2-60, tft.height()/2, 80, 40, "D-");
       //SerPP.draw(tft.width()/2+60, tft.height()/2, 80, 40, "D+");
       SerSUB.draw(tft.width()/2, tft.height()/2+80, 80, 40, "ADX");

       // testowy toggler
       tog1.draw(tft.width()/2-100, tft.height()/2+80, 80, 40, "RDY", "NOT", 114, 6);

          tft.setTextFont(4);
          tft.setTextColor(TFT_RED);
          
          tft.setCursor(tft.width()/2, 50);
          tft.print(sers[0]);  
          tft.setCursor(tft.width()/2, 100);
          tft.print(sers[1]); 
          tft.setCursor(tft.width()/2, 150);
          tft.print(sers[2]); 
          tft.setCursor(tft.width()/2, 200);
          tft.print(sers[3]);      
     } 
     //  ----------------------------------------------------------------------RAZ PO ZAMKNIECIU
     if (pressed) 
     {
      // WCISNIECIE -> Czy zamykac ?
      if (window.chkIfClosed() )
        { 
          window.alreadyDraw = false; 
        } 
      // WCISNIECIE ---------------------------->
      // (press) BTN 1-  
      if (Ser1m.chkIfPress(x,y) )
      {
        if (sers[0] > 0) { 
          sers[0]-=5;
            if (sers[0]<0) sers[0]=0;
        bool report = radio.write(&sers, sizeof(sers));

        if (RF_DEBUG){
         if (report) {
            Serial.print(F("Transmission successful! "));          // payload was delivered
            Serial.println(sers[0]);                               // print payload sent
                    } 
         else 
          {
            Serial.println(F("Transmission failed or timed out")); // payload was not delivered
          }}          
          }
      }
      // (press) BTN 1+
      if (Ser1p.chkIfPress(x,y) )
      {
        if (sers[0] < 180) {
          sers[0]+=5;
            if (sers[0]>180) sers[0]=180;
        bool report = radio.write(&sers, sizeof(sers));

        if (RF_DEBUG){
         if (report) {
            Serial.print(F("Transmission successful! "));          // payload was delivered
            Serial.println(sers[0]);                               // print payload sent
                    } 
         else 
          {
            Serial.println(F("Transmission failed or timed out")); // payload was not delivered
          }}          
          }
      }           
      // (press) BTN 2-
      if (Ser2m.chkIfPress(x,y) )
      {
        if (sers[1] > 0) { 
          sers[1]-=5;
        bool report = radio.write(&sers, sizeof(sers));

        if (RF_DEBUG){
         if (report) {
            Serial.print(F("Transmission successful! "));          // payload was delivered
            Serial.println(sers[1]);                               // print payload sent
                    } 
         else 
          {
            Serial.println(F("Transmission failed or timed out")); // payload was not delivered
          }}          
          }
      }
      // (press) BTN 2+
      if (Ser2p.chkIfPress(x,y) )
      {
        if (sers[1] < 180) {
          sers[1]+=5;
        bool report = radio.write(&sers, sizeof(sers));

        if (RF_DEBUG){
         if (report) {
            Serial.print(F("Transmission successful! "));          // payload was delivered
            Serial.println(sers[1]);                               // print payload sent
                    } 
         else 
          {
            Serial.println(F("Transmission failed or timed out")); // payload was not delivered
          }}      
          }
      } 
      // (press) SerSUB
      if (SerSUB.chkIfPress(x,y) && subDel.del(500))
      {
          if (iADX==false)
          {
            iADX = true;
            SerSUB.draw(tft.width()/2, tft.height()/2+80, 80, 40, "[ON]");            
          }
          else 
          {
            iADX = false;
            sers[0] = 0;
            sers[1] = 0;
               
            // Try start again
              if (radio.begin()) { SerSUB.draw(tft.width()/2, tft.height()/2+80, 80, 40, "OK"); }
              else { SerSUB.draw(tft.width()/2, tft.height()/2+80, 80, 40, "ERR"); }
              radio.openWritingPipe(address); // na poczatek dla SERV
              delay(50);
              radio.setPALevel(RF24_PA_LOW);
              delay(50);
              radio.stopListening();          // na poczatek dla SERV
              delay(50);
              radio.setRetries(15, 15);
              delay(50);
              radio.powerUp();
              delay(50);
              radio.setDataRate(RF24_250KBPS);
              delay(50);
              radio.enableDynamicPayloads();  
              radio.openWritingPipe(address);
              radio.stopListening();       
          }
      }
      // (press) testowy toggler tog1
      tog1.chkIfToggle();
            
      } // (pressed = true)

      // RUN
      tft.setTextFont(4);
      tft.setTextColor(TFT_RED);
      if (sersPrev[0]!=sers[0])
      {
           tft.fillRect(tft.width()/2, 50 , 60, 40, window.takeBG() ); 
           tft.setCursor(tft.width()/2, 50);
           tft.print(sers[0]);
           sersPrev[0]=sers[0];
      }
      if (sersPrev[1]!=sers[1])
      {
           tft.fillRect(tft.width()/2, 100 , 60, 40, window.takeBG() ); 
           tft.setCursor(tft.width()/2, 100);
           tft.print(sers[1]);
           sersPrev[1]=sers[1];
      }      
      if (sersPrev[2]!=sers[2])
      {
           tft.fillRect(tft.width()/2, 150 , 60, 40, window.takeBG() ); 
           tft.setCursor(tft.width()/2, 150);
           tft.print(sers[2]);
           sersPrev[2]=sers[2];
      } 
      if (sersPrev[3]!=sers[3])
      {
           tft.fillRect(tft.width()/2, 200 , 60, 40, window.takeBG() ); 
           tft.setCursor(tft.width()/2, 200);
           tft.print(sers[3]);
           sersPrev[3]=sers[3];
      }       

      if (iADX)
      {
        accel.getEvent(&ADXLevent);
        fADX = ADXLevent.acceleration.x;
        int IADX = round((fADX+10.f)*10 );
        if (IADX > 200) IADX = 200;
        if (IADX < 0 )  IADX = 0;
        sers[0] = byte(map(IADX,0,200,0,180));
        if (sers[0] > 180) sers[0] = 180;

        fADZ = ADXLevent.acceleration.z;
        int IADZ = round((fADZ+10.f)*10 );
        if (IADZ > 200) IADZ = 200;
        if (IADZ < 0 )  IADZ = 0;
        sers[1] = byte(map(IADZ,0,200,0,180));
        if (sers[1] > 180) sers[1] = 180;
        
        bool report = radio.write(&sers, sizeof(sers));
      }
  } // void.Run()

  
  
  
bool RF_DEBUG = true;
bool iADX = false;
float fADX, fADZ;
private:
byte Program_ID;
};

class CLOCK_SET
{
public:
  CLOCK_SET(byte ID, ValChngObjArrtList VALLIST) : Program_ID(ID), ValList(VALLIST) {};
  ~CLOCK_SET(){};

  void Run()
  {
      // Okno
      //  ----------------------------------------------------------------------RAZ PO OTWORZENIU
     if (window.alreadyDraw==false) 
     {
       window.draw("Clock Settings", hsv2hex(156,34,255) );
       window.alreadyDraw = true;
       Serial.println("Uruchamiam program o ID: "+String(CURRENT_PROGRAM_ID) );

      //            x  y   w
       ValList.draw(40,40,180);
       SetClock.draw(40,200,100,40,"SET");
       
       writeFile(SD, "/zegar.txt", "Poczatek: ");
    
     }
     //  ----------------------------------------------------------------------RAZ PO ZAMKNIECIU
     if (pressed) 
     {
      // WCISNIECIE -> Czy zamykac ?
      if (window.chkIfClosed() )
        { 
          window.alreadyDraw = false; 
        } 
      // WCISNIECIE ---------------------------->
      ValList.chkIfPress();

      if (SetClock.chkIfPress(x,y) )
        {
          rtc.adjust(DateTime(YearRTC, MonthRTC, DayRTC, HourRTC, MinuteRTC, SecondRTC));
          char buff[100];
          sprintf(buff,"Data: %d / %d / %d , Godzina: %d.%d.%d \n",YearRTC, MonthRTC, DayRTC, HourRTC, MinuteRTC, MinuteRTC, SecondRTC);
          appendFile(SD, "/zegar.txt",buff);
        }
      }
  } // void.Run()  

ValChngObjArrtList ValList;

private:
  byte Program_ID;
  //byte SetState=0;
};

//############################################################################## LED WS PANEL
class wLedPanel
{
public:   //      Pid - generalnie nie uzywane narazie
  wLedPanel(byte id, ValChngObjArrtList VALLIST) : ValList(VALLIST), Program_ID(id), alreadyDraw(false), H(255), S(255), V(255) {}
  ~wLedPanel(){}

  void Run()
  {
      // Okno i start (once)
      if (window.alreadyDraw==false) 
      {
        window.draw("WS LED PANEL", hsv2hex(156,34,255) );
        this->loadSettings();
        
        // KOD KOD KOD
        this->drawPanel(6,30,468,104);
        
        this->drawbar_H(80,150,360,40);
        this->drawbar_S(80,200,360,40);
        this->drawbar_V(80,250,360,40);

        this->draw_RGB(5,150,50,40);

        this->scrol.draw(5,200,50,40,"N","Y",hsv2hex(68,255,255), hsv2hex(0,255,255) );
        this->showFFT.draw(5,250,50,40,"F","+",hsv2hex(68,255,255), hsv2hex(0,255,255) );

        window.alreadyDraw = true;
        Serial.println("Uruchamiam program o ID: "+String(CURRENT_PROGRAM_ID) );
      }

      // Wcisniety
      if (pressed) 
      {
        if (window.chkIfClosed() )    // Zamkniecie okna ustawia PROG.._ID=0 , RESetuje pulpit
        {
          this->saveSettings();
          window.alreadyDraw = false;
        } 
        else this->chkIfPress();
      } // (press==true)


      //Przewijaj ekran
      if (isScrolling == true && scrlDel.del(WSLEDScrollSpd))
      {
         for (byte j=0; j<8; j++)
          {
            WLED_buff[j] = WLED[j][0];
          }
        
        for (byte i=0; i<35; i++)
        {
          for (byte j=0; j<8; j++)
          {
            WLED[j][i] = WLED[j][i+1];
          }
        }

         for (byte j=0; j<8; j++)
          {
            WLED[j][35] = WLED_buff[j];
          }

        // Zwieksz HUE
        for (byte i=0; i<36; i++)
        {
          for (byte j=0; j<8; j++)
          {
            WLED[j][i].h += ScrollH;
          }
        }
        
      this->myLed_to_FastLED();
      FastLED.show(); 
      } //PE END-

      // FFT  
      if (FFT_RUN == true){
        // Wylacz pozostale moduly
        if (SENSORS_RUN == true) SENSORS_RUN = false;
        if (VU_RUN == true) VU_RUN = false;
        
        if (IsFALLOFF == false){
        //if (FFTDoneComputing == true) {} -> dokonczyc
        this->clearLedsArray();

        // Rozlozony zakres
        if (compressFFT==false)
        {
        for (int i=0; i<36; i++)
        {
          for (int j=0; j<mapped[bin_reject+i*spectrum_coef]; j++)
          {
            WLED[7-j][i](H+mapped[bin_reject+i*spectrum_coef]*HMirrorOffset, S, V);
          }
        }
        }}// Rozlozony zakres
        
      if (IsFALLOFF == true){
        if (SENSORS_RUN == true) SENSORS_RUN = false;
        //if (FFTDoneComputing == true) {} -> dokonczyc
        this->clearLedsArray();

        // Rozlozony zakres
        if (compressFFT==false)
        {
          
        for (int k=0; k<255; k++)
        {
          if (mapped[k] > mapped_p[k]) {mapped_p[k] = mapped[k]; }
        }
          
        for (int i=0; i<36; i++)
        {
          for (int j=0; j<mapped_p[bin_reject+i*spectrum_coef]; j++)
          {
            WLED[7-j][i](H+mapped_p[bin_reject+i*spectrum_coef]*HMirrorOffset, S, V);
          }
        }

        if (falloff.del(falloffDel)){
        for (int k=0; k<255; k++)
        {
          if (mapped_p[k] > 0) {mapped_p[k]--;}
        }}
        }}// Rozlozony zakres

        // Skompresowany zakres
        if (compressFFT==true)
        {
          int compFFT[36] = {};
          for (int i=0; i<36; i++)
          {
            for (int j=0; j<6; j++)
            {
              compFFT[i] += mapped[i*6+j+bin_reject];
            }
            compFFT[i] /= 6;
            if (compFFT[i]>8) compFFT[i]=8;
          }
        for (int i=0; i<36; i++)
        {
          for (int j=0; j<compFFT[i]; j++)
          {
            WLED[7-j][i](H+compFFT[i]*HMirrorOffset, S, V);
          }
        }
        }// Skompresowany zakres
        
      this->myLed_to_FastLED();
      FastLED.show();
      } // FFT END
      

      // VU
      if (VU_RUN == true)
      {
        // Wylacz moduly
        if (FFT_RUN == true) FFT_RUN = false;
        if (SENSORS_RUN == true) SENSORS_RUN = false;
        
        if (MatChange == true)
        {
          this->clearMAT();
          this->evaluate();
          this->MatChange = false;
        }
        // ZBIERANIE SAMPLI
        if (RMS==false)
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
            VU_MX = map(VU_QX, 0 , 255, 1 , MaxMap);
            if (VU_MX > VU_MX_p) VU_MX_p = VU_MX;
        }

        if (RMS==true)
        {
            VU_QX = 0;
            VU = 0;
            for (int i = 0; i < VU_Samples; i++)
            {
            microsecFFT = micros();
            VU += static_cast<unsigned long>(pow(((analogRead(MIC) - MicOffset)/ADC_PRESCALAR),2));

            while (micros() < microsecFFT+SF){}
            //delayMicroseconds(SF);
            }
            VU /= VU_Samples;
            VU = static_cast<unsigned long>(sqrt(VU));
            if (VU < VU_MinAmp) VU = 0;
            if (VU > VU_MaxAmp) VU = VU_MaxAmp;
            VU = map(VU, 0, VU_MaxAmp, 0 , 255);
            VU_QX = static_cast<byte>(VU);
            VU_MX = map(VU_QX, 0 , 255, 1 , MaxMap);
            if (VU_MX > VU_MX_p) VU_MX_p = VU_MX;
        }
        // ZBIERANIE SAMPLI END

        // VU - mapowanie
        if (VU_MX >= VU_MX_p && IsFALLOFF == true){
          for (int i=0; i<36; i++)
          {
            for (int j=0; j<8; j++)
            {
              WLEDS[j][i](0,0,0);
              if (WLED[j][i].v > 0)
                { 
                  //MAT[j][i] = 1;
                  WLEDS[j][i].v = WLED[j][i].v - VU_MX*4;
                  WLEDS[j][i].h = WLED[j][i].h + VU_MX*2;
                  WLEDS[j][i].s = WLED[j][i].s;
                }
              if (MAT[j][i] <= VU_MX && MAT[j][i] > 1)
                {
                  WLEDS[j][i].h = MAT_C[j][i];
                  WLEDS[j][i].s = 255-MaxMap;
                  WLEDS[j][i].v = MAT_V[j][i]-MaxMap;
                }
            }
          }
        VU_MX_p = VU_MX;
        this->myLed_to_FastLED_Copy();
        FastLED.show();}

        else if(VU_MX_p > VU_MX && falloff.del(falloffDel) && IsFALLOFF == true){
          if (VU_MX_p>0) VU_MX_p -= 1;
          for (int i=0; i<36; i++)
          {
            for (int j=0; j<8; j++)
            {
              WLEDS[j][i](0,0,0);
              if (WLED[j][i].v > 0)
                { 
                  //MAT[j][i] = 1;
                  WLEDS[j][i].v = WLED[j][i].v - VU_MX_p*5;
                  WLEDS[j][i].h = WLED[j][i].h + VU_MX_p*2;
                  WLEDS[j][i].s = WLED[j][i].s;
                }
              if (MAT[j][i] <= VU_MX_p && MAT[j][i] > 1)
                {
                  WLEDS[j][i].h = MAT_C[j][i];
                  WLEDS[j][i].s = 255-MaxMap;
                  WLEDS[j][i].v = MAT_V[j][i]-MaxMap;
                }
            }
          }
        this->myLed_to_FastLED_Copy();
        FastLED.show();}

        // VU - mapowanie, bez falloffu
        if (IsFALLOFF == false){
          for (int i=0; i<36; i++)
          {
            for (int j=0; j<8; j++)
            {
              WLEDS[j][i](0,0,0);
              if (WLED[j][i].v > 0)
                { 
                  //MAT[j][i] = 1;
                  WLEDS[j][i].v = WLED[j][i].v - VU_MX*4;
                  WLEDS[j][i].h = WLED[j][i].h + VU_MX*2;
                  WLEDS[j][i].s = WLED[j][i].s;
                }
              if (MAT[j][i] <= VU_MX && MAT[j][i] > 1)
                {
                  WLEDS[j][i].h = MAT_C[j][i];
                  WLEDS[j][i].s = 255-MaxMap;
                  WLEDS[j][i].v = MAT_V[j][i]-MaxMap;
                }
            }
          }
        this->myLed_to_FastLED_Copy();
        FastLED.show();}
        // VU - mapowanie END
      }
      // VU End
      
      // Run
  }

  void evaluate()
  {
    // 0 - puste
    // 1 - emiter
    // 100 - blokada
    // 2,3,4... iteracje
          // Zapisz gdzie sa emitery (1) i blokady (100)
          for (int i=0; i<36; i++)
          {
            for (int j=0; j<8; j++)
            {
              if (WLED[j][i].v > 0)
              { 
                if (WLED[j][i].s > 192)
                {
                  MAT[j][i] = 1;
                  MAT_C[j][i] = WLED[j][i].h;
                  MAT_V[j][i] = WLED[j][i].v;
                }
                else MAT[j][i] = 100;
              }
            }
          }

          // Expanduj macierz
          for (int k=2; k<40; k++){
          for (int i=0; i<36; i++)
          {
            for (int j=0; j<8; j++)
            {
              if (MAT[j][i]==k-1)
              { 
               if (j-1>=0) {if (MAT[j-1][i]==0) {MAT[j-1][i] = k; MaxMap=k-1; MAT_C[j-1][i] = MAT_C[j][i]+(128/((k-1)*2)); MAT_V[j-1][i] = MAT_V[j][i]-k;}}
               if (j+1<8)  {if (MAT[j+1][i]==0) {MAT[j+1][i] = k; MaxMap=k-1; MAT_C[j+1][i] = MAT_C[j][i]+(128/((k-1)*2)); MAT_V[j+1][i] = MAT_V[j][i]-k;}}
               if (i-1>=0) {if (MAT[j][i-1]==0) {MAT[j][i-1] = k; MaxMap=k-1; MAT_C[j][i-1] = MAT_C[j][i]+(128/((k-1)*2)); MAT_V[j][i-1] = MAT_V[j][i]-k;}}
               if (i+1<36) {if (MAT[j][i+1]==0) {MAT[j][i+1] = k; MaxMap=k-1; MAT_C[j][i+1] = MAT_C[j][i]+(128/((k-1)*2)); MAT_V[j][i+1] = MAT_V[j][i]-k;}}
              }
            }
          }}
  }

  void clearMAT()
  {
          for (int i=0; i<36; i++) {for (int j=0; j<8; j++) {MAT[j][i] = 0; MAT_C[j][i] = 0; MAT_V[j][i] = 0; }}
  }
  
  // | 60 |___360___| 60 | 2x60
  void drawPanel (uint16_t PANEL_SX, uint16_t PANEL_SY, uint16_t PANEL_W, uint16_t PANEL_H)
  {
    // Wstepne przypisanie wartosci
    panel_sx = PANEL_SX;
    panel_sy = PANEL_SY;
    panel_w = PANEL_W; // 360//- 468
    panel_h = PANEL_H; // 80 //- 104
    // Bloki 8x36
    for (byte i=0; i<8; i++)
    {
      for (byte j=0; j<36; j++)
      {
        tft.fillRect(panel_sx+13*j, panel_sy+13*i, 13, 13, hsv2hex(0,0,32));
      }
    }

    // Siatka
    for (byte i=0; i<9; i++)
    {
      tft.drawFastHLine(panel_sx, panel_sy+13*i, PANEL_W, hsv2hex(0,0,255));
    }
    for (byte j=0; j<37; j++)
    {
      tft.drawFastVLine(panel_sx+13*j, panel_sy, PANEL_H, hsv2hex(0,0,255));
    }
  }

  void drawbar_H (uint16_t H_SX, uint16_t H_SY, uint16_t H_W, uint16_t H_H)
  {
    // Wstepne przypisanie wartosci
    hp_sx = H_SX;
    hp_sy = H_SY;
    hp_w  = H_W;
    hp_h  = H_H;
    hp_indi = hp_sx;

    tft.drawRect(hp_sx-1, hp_sy-1, hp_w+2, hp_h+2,TFT_BLACK);
    // Rysuj H bar
    hCoef = float(255./hp_w);
    for (int i=0; i<hp_w; i++)
    {
      tft.drawFastVLine(hp_sx+i, hp_sy, hp_h, hsv2hex(byte(i*hCoef),S,V) );
    }
  }

  void drawbar_S (uint16_t S_SX, uint16_t S_SY, uint16_t S_W, uint16_t S_H)
  {
    // Wstepne przypisanie wartosci
    sp_sx = S_SX;
    sp_sy = S_SY;
    sp_w  = S_W;
    sp_h  = S_H;
    sp_indi = sp_sx;

    tft.drawRect(sp_sx-1, sp_sy-1, sp_w+2, sp_h+2,TFT_BLACK);
    // Rysuj S bar
    sCoef = float(255./sp_w);
    for (int i=0; i<sp_w; i++)
    {
      tft.drawFastVLine(sp_sx+i, sp_sy, sp_h, hsv2hex(H,byte(i*sCoef),V) );
    }
  }

  void drawbar_V (uint16_t V_SX, uint16_t V_SY, uint16_t V_W, uint16_t V_H)
  {
    // Wstepne przypisanie wartosci
    vp_sx = V_SX;
    vp_sy = V_SY;
    vp_w  = V_W;
    vp_h  = V_H;
    vp_indi = vp_sx;

    tft.drawRect(vp_sx-1, vp_sy-1, vp_w+2, vp_h+2,TFT_BLACK);
    // Rysuj V bar
    vCoef = float(255./vp_w);
    for (int i=0; i<vp_w; i++)
    {
      tft.drawFastVLine(vp_sx+i, vp_sy, vp_h, hsv2hex(H,S,byte(i*vCoef)) );
    }
  }

  void draw_RGB(uint16_t rgb_SX, uint16_t rgb_SY, uint16_t rgb_W, uint16_t rgb_H)
  {
    rgb_sx = rgb_SX;
    rgb_sy = rgb_SY;
    rgb_w = rgb_W;
    rgb_h = rgb_H;

    tft.fillRect(rgb_sx, rgb_sy, rgb_w, rgb_h, hsv2hex(H,S,V) );
    tft.drawRect(rgb_sx-1, rgb_sy-1, rgb_w+2, rgb_h+2, TFT_BLACK);
    tft.drawRect(rgb_sx-2, rgb_sy-2, rgb_w+4, rgb_h+4, TFT_WHITE);
  }

  void myLed_to_FastLED()
  {
      int iter = 1;
      for (byte i=0; i<36; i++) { leds[i] = CHSV(WLED[7][i].h, WLED[7][i].s, WLED[7][i].v); }
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLED[6][35-i].h, WLED[6][35-i].s, WLED[6][35-i].v); }
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLED[5][i].h, WLED[5][i].s, WLED[5][i].v); }
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLED[4][35-i].h, WLED[4][35-i].s, WLED[4][35-i].v); }
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLED[3][i].h, WLED[3][i].s, WLED[3][i].v);  }  
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLED[2][35-i].h, WLED[2][35-i].s, WLED[2][35-i].v); } 
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLED[1][i].h, WLED[1][i].s, WLED[1][i].v); }
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLED[0][35-i].h, WLED[0][35-i].s, WLED[0][35-i].v); }    
  }

  void myLed_to_FastLED_Copy()
  {
      int iter = 1;
      for (byte i=0; i<36; i++) { leds[i] = CHSV(WLEDS[7][i].h, WLEDS[7][i].s, WLEDS[7][i].v); }
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLEDS[6][35-i].h, WLEDS[6][35-i].s, WLEDS[6][35-i].v); }
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLEDS[5][i].h, WLEDS[5][i].s, WLEDS[5][i].v); }
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLEDS[4][35-i].h, WLEDS[4][35-i].s, WLEDS[4][35-i].v); }
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLEDS[3][i].h, WLEDS[3][i].s, WLEDS[3][i].v);  }  
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLEDS[2][35-i].h, WLEDS[2][35-i].s, WLEDS[2][35-i].v); } 
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLEDS[1][i].h, WLEDS[1][i].s, WLEDS[1][i].v); }
      iter++;
      for (byte i=0; i<36; i++) { leds[36*iter+i] = CHSV(WLEDS[0][35-i].h, WLEDS[0][35-i].s, WLEDS[0][35-i].v); }    
  }

  void clearLeds()
  {
    for (int i=0; i<8; i++) {for (int j=0; j<36; j++) {WLED[i][j](0,0,0);}}
    this->drawPanel(6,30,468,104);
    this->myLed_to_FastLED();
    FastLED.show();
  }

  void clearLedsArray()
  {
    for (int i=0; i<8; i++) {for (int j=0; j<36; j++) {WLED[i][j](0,0,0);}}
  }  

  void loadSettings()
  {
    readFileToBuff(SD, "/noise_reduction.txt",buff);
    noise_reduction = atoi(buff);

    readFileToBuff(SD, "/fftMinAmp.txt",buff);
    fftMinAmp = atoi(buff);

    readFileToBuff(SD, "/MicOffset.txt",buff);
    MicOffset = atoi(buff);

    readFileToBuff(SD, "/micMaxAmp.txt",buff);
    micMaxAmp = atoi(buff);

    readFileToBuff(SD, "/norm_base.txt",buff);
    norm_base = atof(buff);

    readFileToBuff(SD, "/norm_gain.txt",buff);
    norm_gain = atof(buff);

    readFileToBuff(SD, "/WSLEDScrollSpd.txt",buff);
    WSLEDScrollSpd = atoi(buff);

    readFileToBuff(SD, "/ADC_PRESCALAR.txt",buff);
    ADC_PRESCALAR = atof(buff);

    readFileToBuff(SD, "/bin_reject.txt",buff);
    bin_reject = atoi(buff);

    readFileToBuff(SD, "/spectrum_coef.txt",buff);
    spectrum_coef = atoi(buff);

    readFileToBuff(SD, "/VU_Samples.txt",buff);
    VU_Samples = atoi(buff);

    readFileToBuff(SD, "/VU_MinAmp.txt",buff);
    VU_MinAmp = atoi(buff);

    readFileToBuff(SD, "/VU_MaxAmp.txt",buff);
    VU_MaxAmp = atoi(buff);

    readFileToBuff(SD, "/falloffDel.txt",buff);
    falloffDel = atoi(buff);

    readFileToBuff(SD, "/HMirrorOffset.txt",buff);
    HMirrorOffset = atoi(buff);
  }

  void saveSettings()
  {
        // Zapisz ustawienia
          sprintf(buff,"%d",noise_reduction);
          writeFile(SD, "/noise_reduction.txt",buff);
          
          sprintf(buff,"%d",fftMinAmp);
          writeFile(SD, "/fftMinAmp.txt",buff);
          
          sprintf(buff,"%d",MicOffset);
          writeFile(SD, "/MicOffset.txt",buff);
          
          sprintf(buff,"%d",micMaxAmp);
          writeFile(SD, "/micMaxAmp.txt",buff);
          
          sprintf(buff,"%f",norm_base);
          writeFile(SD, "/norm_base.txt",buff);
          
          sprintf(buff,"%f",norm_gain);
          writeFile(SD, "/norm_gain.txt",buff);
          
          sprintf(buff,"%d",WSLEDScrollSpd);
          writeFile(SD, "/WSLEDScrollSpd.txt",buff);
          
          sprintf(buff,"%f",ADC_PRESCALAR);
          writeFile(SD, "/ADC_PRESCALAR.txt",buff);
          
          sprintf(buff,"%d",bin_reject);
          writeFile(SD, "/bin_reject.txt",buff);
          
          sprintf(buff,"%d",spectrum_coef);
          writeFile(SD, "/spectrum_coef.txt",buff);
          
          sprintf(buff,"%d",VU_Samples);
          writeFile(SD, "/VU_Samples.txt",buff);
          
          sprintf(buff,"%d",VU_MinAmp);
          writeFile(SD, "/VU_MinAmp.txt",buff);
          
          sprintf(buff,"%d",VU_MaxAmp);
          writeFile(SD, "/VU_MaxAmp.txt",buff);
          
          sprintf(buff,"%d",falloffDel);
          writeFile(SD, "/falloffDel.txt",buff);

          sprintf(buff,"%d",HMirrorOffset);
          writeFile(SD, "/HMirrorOffset.txt",buff);
  }
  
  bool chkIfPress()
  {
    // Dotyk na panelu
    if (x>panel_sx && x<panel_sx+panel_w && y>panel_sy && y<panel_sy+panel_h)
    {
      int kdX = int((x-panel_sx)/13);
      int kdY = int((y-panel_sy)/13);
      int MkdX = 0; // mirror x|x
      tft.fillRect(panel_sx+kdX*13, panel_sy+kdY*13, 13, 13, hsv2hex(H,S,V) );
      tft.drawRect(panel_sx+kdX*13, panel_sy+kdY*13, 13, 13, hsv2hex(0,0,255) );
      if (MIRRORING)
      {
        MkdX = 35-kdX;
        tft.fillRect(panel_sx+MkdX*13, panel_sy+kdY*13, 13, 13, hsv2hex(H+HMirrorOffset,S,V) );
        tft.drawRect(panel_sx+MkdX*13, panel_sy+kdY*13, 13, 13, hsv2hex(0,0,255) );
        WLED[kdY][MkdX] = wLed(H+HMirrorOffset,S,V);
      }
      
      //Serial.println("WS LED Kordy x: "+String(kdY)+"   y:"+String(kdX));  
      
      WLED[kdY][kdX] = wLed(H,S,V);
      this->myLed_to_FastLED();
      this->MatChange = true;
      FastLED.show(); 
      
    }// Dotyk na panelu

    // Dotyk na H barze
    if (x>hp_sx && x<hp_sx+hp_w && y>hp_sy && y<hp_sy+hp_h && menuVisible==false )
    {
      H = byte((x-hp_sx)*hCoef);
      tft.fillRect(rgb_sx, rgb_sy, rgb_w, rgb_h, hsv2hex(H,S,V) );

      // Aktualizuj S i V
      for (int i=0; i<sp_w; i++) {tft.drawFastVLine(sp_sx+i, sp_sy, sp_h, hsv2hex(H,byte(i*sCoef),V) );}
      for (int i=0; i<vp_w; i++) {tft.drawFastVLine(vp_sx+i, vp_sy, vp_h, hsv2hex(H,S,byte(i*vCoef)) );}
    }// Dotyk na H barze

    // Dotyk na S barze
    if (x>sp_sx && x<sp_sx+sp_w && y>sp_sy && y<sp_sy+sp_h && menuVisible==false )
    {
      S = byte((x-sp_sx)*sCoef);
      tft.fillRect(rgb_sx, rgb_sy, rgb_w, rgb_h, hsv2hex(H,S,V) );

      // Aktualizuj H i V
      for (int i=0; i<hp_w; i++) {tft.drawFastVLine(hp_sx+i, hp_sy, hp_h, hsv2hex(byte(i*hCoef),S,V) );}
      for (int i=0; i<vp_w; i++) {tft.drawFastVLine(vp_sx+i, vp_sy, vp_h, hsv2hex(H,S,byte(i*vCoef)) );}
    }// Dotyk na S barze

    // Dotyk na V barze
    if (x>vp_sx && x<vp_sx+vp_w && y>vp_sy && y<vp_sy+vp_h && menuVisible==false )
    {
      V = byte((x-vp_sx)*vCoef);
      tft.fillRect(rgb_sx, rgb_sy, rgb_w, rgb_h, hsv2hex(H,S,V) );

      // Aktualizuj H i S
      for (int i=0; i<hp_w; i++) {tft.drawFastVLine(hp_sx+i, hp_sy, hp_h, hsv2hex(byte(i*hCoef),S,V) );}
      for (int i=0; i<sp_w; i++) {tft.drawFastVLine(sp_sx+i, sp_sy, sp_h, hsv2hex(H,byte(i*sCoef),V) );}
    }// Dotyk na V barze

    // Dotyk na RGB - czysc
    if (x>rgb_sx && x<rgb_sx+rgb_w && y>rgb_sy && y<rgb_sy+rgb_h)
    {
      this->clearLeds();
    }// Dotyk na RGB - czysc
        
    scrol.chkIfToggle();
    showFFT.chkIfToggle();

// - - - - - KIEDY MENU WIDOCZNE --->
    if (menuVisible==true) 
    {
      ValList.chkIfPress();
      CompFFT.chkIfToggle();
      showVU.chkIfToggle();
      RMSorNOR.chkIfToggle();
      IsFalloff.chkIfToggle();
      Mirroring.chkIfToggle();
    }

    //Dotyk na dole - settings
    if (x>0 && x<480 && y>300 && y<320 && mvDel.del(500) )
    {
      if (menuVisible == false)
      {
        menuVisible = true;
        tft.fillRect(75,145,405,175,window.takeBG() );
        ValList.draw(80,150,280);
        this->CompFFT.draw(80,240,80,40,"CY","CN",TFT_RED, TFT_BLUE);
        this->showVU.draw(170,240,40,40,"Vo","Vf",TFT_RED, TFT_BLUE);
        this->RMSorNOR.draw(220,240,80,40,"RMS","NOR",TFT_RED, TFT_BLUE);
        this->IsFalloff.draw(310,240,40,40,"FO","QC",TFT_RED, TFT_BLUE);  
        this->Mirroring.draw(360,240,40,40,"M","m",TFT_RED, TFT_BLUE);  
      }
      else
      {
        menuVisible = false;
        tft.fillRect(75,145,405,175,window.takeBG() );
        this->drawbar_H(80,150,360,40);
        this->drawbar_S(80,200,360,40);
        this->drawbar_V(80,250,360,40);
      }
    }
  } // wystapil dotyk


  // ZMIENNE PUBLICZNE
  ValChngObjArrtList ValList;
  bool menuVisible = false;
  bool alreadyDraw = true;
  bool isScrolling = false;
  bool MatChange = true;
  char buff[100];
  
  // Togglery
  Toggler scrol = Toggler(&isScrolling, 200);
  Toggler showFFT = Toggler(&FFT_RUN, 200);
  Toggler CompFFT = Toggler(&compressFFT, 200);
  Toggler showVU = Toggler(&VU_RUN, 200);
  Toggler RMSorNOR = Toggler(&RMS, 200);
  Toggler IsFalloff = Toggler(&IsFALLOFF, 200);
  Toggler Mirroring = Toggler(&MIRRORING, 200);
  
  // Kronosy
  Kronos scrlDel;
  Kronos mvDel;
  Kronos falloff;

  // Macierze pomocnicze
  byte MAT[8][36] = {};
  byte MAT_C[8][36] = {};
  byte MAT_V[8][36] = {};
  
private:
byte Program_ID;
byte L1r, L1g, L1b;

uint16_t panel_sx, panel_sy, panel_w, panel_h;

uint16_t hp_sx, hp_sy, hp_w, hp_h,  hp_indi;
uint16_t sp_sx, sp_sy, sp_w, sp_h,  sp_indi;
uint16_t vp_sx, vp_sy, vp_w, vp_h,  vp_indi;

uint16_t rgb_sx, rgb_sy, rgb_w, rgb_h;

byte H, S, V; // aktualne HSV
float hCoef, sCoef, vCoef;
//float yCoef;
  
};
#endif
