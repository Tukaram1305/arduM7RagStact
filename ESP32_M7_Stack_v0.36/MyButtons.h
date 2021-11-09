#ifndef MYBUTTONS_H
#define MYBUTTONS_H

void drawSdJpeg(const char *filename, int xpos, int ypos);  // deklaracja renderu JPEG
void drawJpeg( const char *filename, int xpos, int ypos);

//*********************************************************************************************************************** -> Btn
class Btn
{
  public:
   Btn(){};
  ~Btn(){};

  // @ DRAW ( START_X , START_Y , WIDTH , HEIGHT , LABEL )
  void draw(uint16_t bx, uint16_t by,uint16_t bw, uint16_t bh, const char *label)
  {
    lab = new char[strlen(label)];
    strcpy(lab,label);
    x = bx; y = by; w = bw; h = bh;
    
    //tft.fillRect(x,y,w,h,hsv2hex(162,255,255) );
    for (int i=0; i<h; i++)
    {
       tft.drawFastHLine(x,y+i,w,hsv2hex(140,255-2*i,255-4*i) );
    }
 
    tft.drawRect(x,y,w,h,hsv2hex(0,220,255) );
    tft.drawRect(x+1,y+1,w-2,h-2,hsv2hex(0,0,40) );
    tft.drawFastHLine(x+1,y+1,w-2,hsv2hex(0,0,160) );
    tft.drawFastVLine(x+1,y+1,h-2,hsv2hex(0,0,180) );

    //tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(FSSB18);
    //tft.setTextFont(4);
    //tft.setTextSize(1);
    //tft.setCursor( x+(w/2), y+4 );
    //tft.print(lab);
    //tft.drawString(lab,x,y,4);

    tft.drawCentreString(lab, x+w/2, y+6, GFXFF);
  }

  bool chkIfPress(uint16_t tx, uint16_t ty)
  {
    if (tx>x && tx<x+w && ty>y && ty<y+h) 
    {
      //Serial.println(lab);
      return true;
    }
    else return false;
  }

  private:
  uint16_t x,y,w,h;
  char* lab;
};

//*********************************************************************************************************************** -> Toggler
class Toggler
{
public:
  //     VALUE PTR        DEL
  Toggler(){}
  Toggler(bool *VAL, uint16_t DELX): val(VAL), delx(DELX) {};
  ~Toggler(){};

  // @ DRAW (   START_X ,    START_Y ,     WIDTH ,     HEIGHT ,             LAB ON,              LAB OFF,     COLOR ON,     COLOR OFF)
  void draw(uint16_t bx, uint16_t by,uint16_t bW, uint16_t bH, const char *labelON, const char *labelOFF, byte colorON, byte colorOFF)
  {
    labON = new char[strlen(labelON)];
    strcpy(labON,labelON);
    labOFF = new char[strlen(labelOFF)];
    strcpy(labOFF,labelOFF);
        
    sx = bx; sy = by; bw = bW; bh = bH; colON = colorON; colOFF = colorOFF;
    
    if ((*val)==true)
    {
      for (int i=0; i<bh; i++)
      { tft.drawFastHLine(sx,sy+i,bw,hsv2hex(colON,255-2*i,255-4*i) ); }      
    }
    else
    {
      for (int i=0; i<bh; i++)
      { tft.drawFastHLine(sx,sy+i,bw,hsv2hex(colOFF,255-2*i,255-4*i) ); }
    }

 
    tft.drawRect(sx,sy,bw,bh,hsv2hex(0,220,255) );
    tft.drawRect(sx+1,sy+1,bw-2,bh-2,hsv2hex(0,0,40) );

    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(FSSB18);

    if ((*val)==true) { tft.drawCentreString(labON, sx+bw/2, sy+6, GFXFF); }
    else              { tft.drawCentreString(labOFF, sx+bw/2, sy+6, GFXFF); }
  }

  void chkIfToggle()
  {
    if (x>sx && x<sx+bw && y>sy && y<sy+bh && (millis() > this->milli+this->delx) ) 
    {
      if ((*val) == true)
      {
        (*val) = false;
        soundID = 2;
        for (int i=0; i<bh; i++)
          { tft.drawFastHLine(sx,sy+i,bw,hsv2hex(colOFF,255-2*i,255-4*i) ); }
        tft.drawRect(sx,sy,bw,bh,hsv2hex(0,220,255) );
        tft.drawRect(sx+1,sy+1,bw-2,bh-2,hsv2hex(0,0,40) );
        tft.setTextColor(TFT_WHITE);
        tft.setFreeFont(FSSB18);
        tft.drawCentreString(labOFF, sx+bw/2, sy+6, GFXFF);
        this->milli = millis();
      }
      else if ((*val) == false)
      {
        (*val) = true;
        soundID = 1;
        for (int i=0; i<bh; i++)
          { tft.drawFastHLine(sx,sy+i,bw,hsv2hex(colON,255-2*i,255-4*i) ); }
        tft.drawRect(sx,sy,bw,bh,hsv2hex(0,220,255) );
        tft.drawRect(sx+1,sy+1,bw-2,bh-2,hsv2hex(0,0,40) );
        tft.setTextColor(TFT_WHITE);
        tft.setFreeFont(FSSB18);
        tft.drawCentreString(labON, sx+bw/2, sy+6, GFXFF);
        this->milli = millis();
      }      
    }
  }
  
  bool *val;
  
private:
  uint16_t sx, sy, bw, bh;
  char* labON;
  char* labOFF;
  byte colON, colOFF;
  
  unsigned long milli;
  uint16_t delx;
};

//*********************************************************************************************************************** -> Window
class Window
{
public:
  Window(uint16_t xp, uint16_t yp, uint16_t xw, uint16_t yh )
  : wX(xp), wY(yp), wW(xw), wH(yh) // konstruktor o wymiarach
  {}
  Window() : wX(0), wY(0), wW(480), wH(320)       // konstruktor bazowy (caly ekran)
  {}
  virtual ~Window(){};

  virtual void draw(const char* lab, word background)
  {
    //label = new char[strlen(lab)];
    //strcpy(label,lab);
    bg = background;
    // okno
    tft.fillRect(wX,wY,wW,wH,bg);
    for (byte n=0; n<30; n++)
    {
      tft.drawFastHLine(wX,wY+n,wW,hsv2hex(138,240-3*n,250-4*n));
    }
    tft.drawRect(wX,wY,wW,wH,hsv2hex(0,0,30));
    tft.drawRect(wX+1,wY+1,wW-1,wH-1,hsv2hex(0,0,45));
    //tft.fillRect(wX,wY,wW,30,hsv); // nazwa okna (pomarancz)
    //tft.drawFastHLine(wX,wY,wW,0x8c8c8c);
    //tft.drawFastHLine(wX,wH,wW,0x8c8c8c);
    //tft.drawFastVLine(wX,wY,wH,0x8c8c8c);
    //tft.drawFastVLine(wW,wY,wH,0x8c8c8c);
    // etykieta
    tft.setCursor(wX+12, wY+4);
    tft.setTextColor(TFT_WHITE);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.print(lab);
    // close button
    tft.drawRect(wW-41,wY+1,40,28,0x2e2e2e);
    tft.fillRect(wW-40,wY+2,38,26,0xb80000);
    tft.drawLine(wW-40,wY+2,wW-2,wY+28,TFT_WHITE);
    tft.drawLine(wW-40,wY+28,wW-2,wY+2,TFT_WHITE);

    this->alreadyDraw = true;
    
  }

  bool chkIfDraw()
  {
    return this->alreadyDraw;
  }
  
  bool chkIfClosed()
  {
    if (x>wW-40 && x<wW && y>wY && y<wY+30)
    {
      this->isActive = false;
      CURRENT_PROGRAM_ID = 0;
      DrawDesk.RESET();
      return true;
    }
    else return false;
  }

  word takeBG() {return this->bg;}
  bool isActive = false;
  bool alreadyDraw = false;
  
private:
  uint16_t wX, wY, wW, wH;
  word bg;
  //char* label;
protected:

};

//*********************************************************************************************************************** -> Tabs
class Tabs : public Window
{
public:
  Tabs() : wX(0), wY(0), wW(tft.width()), wH(tft.height()), isActive(false), ActiveTab(0)
  {}

  virtual void draw (const char** arr, word background)
  {
    // zbierz ilosc elementow
    this->NumOfTabs = 0;
    for (byte i=0; arr[i] != NULL; i++)
    {
      this->NumOfTabs++;
    }
    tft.fillScreen(bg);
    
  }

bool isActive;

private:
  uint16_t wX, wY, wW, wH;
  byte NumOfTabs;
  byte ActiveTab;
  word bg;
};

//*********************************************************************************************************************** -> Labs
class Labs : public Window
{
public:
  Labs(){}

private:

};

//*********************************************************************************************************************** -> DesktopItem
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ BAZOWA KLASA DLA OBIEKTOW EKRANU
class DesktopItem
{
public:
  static byte           ITEMS_PLACEMENT[4][6]; // nieuzywane narazie
  //static unsigned long  PRESS_TIMER;
  //static uint16_t       PRESS_TIME;

   DesktopItem(){};
   virtual ~DesktopItem(){};
   
   virtual byte draw (uint16_t pGx, uint16_t pGy) = 0;
   virtual byte chkIfPress() = 0;
   
protected:
   byte spaceTakenX;
   byte spaceTakenY;
private:
   byte PRESS_STATE;
   byte Press_Action;
};

byte DesktopItem::ITEMS_PLACEMENT[4][6] = {};
//unsigned long DesktopItem::PRESS_TIMER = 0;
//uint16_t DesktopItem::PRESS_TIME = 0;

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ BAZOWA KLASA

//*********************************************************************************************************************** -> Shortcut
class Shortcut : public DesktopItem
{
  public:
  /*
   * @ Konstruktor ( FilePatch, Name, ProgramID, WIDTH, HEIGHT )
   */
  Shortcut(const char* iFile, const char* iName, const byte pID, byte iW, byte iH) : Program_ID(pID), i_w(iW), i_h(iH), PRESS_STATE(0), Press_Action(0)
  {
    uint16_t len = strlen(iFile);
    iconFile = new char [len];
    strcpy(iconFile,iFile);
    len = strlen(iName);
    Name = new char [len];
    strcpy(Name,iName);

    spaceTakenX = 1;
    spaceTakenY = 1;
    
  };
  ~Shortcut(){};

  virtual byte draw (uint16_t pGx, uint16_t pGy)
  {
    pos_x = 10+pGx*(i_w+20);
    pos_y = 10+pGy*(i_h+20);
    drawSdJpeg(this->iconFile, pos_x , pos_y );
    Serial.print("RYSUJE IKONE: ");
    Serial.println(this->Name);

    //#### Obramowka selektora (default) ####
    for (byte i=1; i<5; i++)
    tft.drawRect(pos_x-i , pos_y-i , i_w+2*i , i_h+2*i , hsv2hex(0,0,220) );
    
    return Program_ID;            // zwroc ProgID do debugu
  }

  virtual byte chkIfPress() // dostanie globalne zmienne z TOUCH (bool pressed, x, y, P_TIMER, P_TIME, wasReleased, LONG_PRESS
  {
    if (x>pos_x-10 && x<pos_x+i_w+10 && y>pos_y-10 && y<pos_y+i_h+10 && PRESS_STATE==0)
    {
      Serial.print("Wcisnieto: ");
      Serial.println(this->Name);
      this->PRESS_STATE = 1;
      this->Press_Action = 0;
      P_TIMER = millis();
       // #### Obramowka selektora (default) # # # #
       for (byte i=1; i<5; i++)
       tft.drawRect(pos_x-i , pos_y-i , i_w+2*i , i_h+2*i , hsv2hex(0,0,40) );
       // #### Obramowka selektora (default) # # END
    }
    
    else if (x>pos_x-10 && x<pos_x+i_w+10 && y>pos_y-10 && y<pos_y+i_h+10 && PRESS_STATE==1 && wasReleased==0)
    {
      P_TIME = millis() - P_TIMER;
      if (P_TIME > LONG_PRESS && this->Press_Action == 0)
      {
        // #### Obramowka selektora (default) # # # #
        for (byte i=1; i<5; i++)
        tft.drawRect(pos_x-i , pos_y-i , i_w+2*i , i_h+2*i , hsv2hex(0,240,220) );
        // #### Obramowka selektora (default) # # END        
        this->Press_Action = 1;
      }
    }
    
    else if (x>tft.width() && y>tft.height() && wasReleased==1 && this->PRESS_STATE == 1) // WYKONA SIE RAZ PO PUSZCZENIU
    {
      Serial.print("Puszczono: ");
      Serial.print(this->Name);
      Serial.println(", czas: "+String(P_TIME)+", akcja: "+String(this->Press_Action) );
      this->PRESS_STATE = 0;
       // #### Obramowka selektora (default) # # # #
       for (byte i=1; i<5; i++)
       tft.drawRect(pos_x-i , pos_y-i , i_w+2*i , i_h+2*i , hsv2hex(0,0,220) );
       // #### Obramowka selektora (default) # # END

       // ============================ GLOWNE WYKONANIE AKCJI PO KLIKNIECIU
       // CURRENT_PROGRAM_ID
       // 0   = (default) nic nie rob
       // 10+ = uruchom program
       // 50+ = long press na programie
       if      (this->Press_Action==0) {CURRENT_PROGRAM_ID = this->Program_ID;      Serial.println("Checker ikon ID: "+String(this->Program_ID)); }
       else if (this->Press_Action==1) {CURRENT_PROGRAM_ID = this->Program_ID+50;   Serial.println("Checker ikon ID: "+String(this->Program_ID)); }
    }
    
    else  // sprawdzenie bez akcji
    {
      return 0;
    }
 
  }

  private:
  // wypelni konstruktor (wlasne)
  char* iconFile;
  char* Name;
  byte Program_ID;
  byte i_w, i_h;
  
  // wypelniaja metody
  uint16_t pos_x, pos_y;
  byte PRESS_STATE;
  byte Press_Action;      // nic nie rob (0), uruchom app (ID), long press (ID+50)

  //wlasne
  unsigned long T_TIMER;
  unsigned long T_TIME;

/*
   0 - default
   1 - wcisniety
   2 - zwolniony
*/
 
};

#endif
