#ifndef CONTROLSELEM_H
#define CONTROLSELEM_H

// Moja klasa WS28 ledow
class wLed
{
public:
  wLed() : h(0), s(0), v(0) {}
  wLed(byte H, byte S, byte V)
  {
    this->h = H; this->s = S; this->v = V;
  }
  wLed(const wLed &cpy)
  {
    this->h = cpy.h; this->s = cpy.s; this->v = cpy.v;
  }

  ~wLed(){}

  byte h,s,v;

  void operator=(const wLed &cpy)
  {
    this->h = cpy.h; this->s = cpy.s; this->v = cpy.v;
  }

  void operator() (byte hh, byte ss, byte vv)
  {
    this->h = hh; this->s = ss; this->v = vv;
  }
  
  
    
};

class Slider
{
public:
  Slider(): alreadyDraw(false) {};
  ~Slider(){};


  void draw(uint16_t sldX, uint16_t sldY, uint16_t sldW, uint16_t sldH, word cB, word cAS)
  {
    
    // Zapisanie wymiarow
    sX = sldX; sY = sldY; sW = sldW; sH = sldH;
    colBTN = cB; colASld = cAS;
    // Rysowanie okienka slidera
    tft.fillRect(sX,sY,sW,sH, hsv2hex(0,0,120) );
    tft.drawRect(sX,sY,sW,sH, hsv2hex(0,0,60) );
    //tft.drawRect(sX+1,sY+1,sW-2,sH-2, hsv2hex(0,0,75) );
    // Guidline
    //tft.fillRect(sX+1, sY+sH/2-4, sX+sW, sY+sH/2+4, hsv2hex(0,0,90) ); 
    // Button
    tft.fillRoundRect(sX+3, sY+3, sH-6, sH-6, 8, hsv2hex(30,180,210) );
    //tft.drawRect(sX+5, sY+5, sH-10, sH-10, hsv2hex(30,180,210) );
    
  }


  void upd(byte *var)
  {
    if (x>sX+1 && x<sX+sW-1 && y>sY+1 && y<sY+sH-1)
    {
      // Update okna
      //tft.fillRect(sX,sY,sW,sH, hsv2hex(0,0,120) );
      //tft.drawRect(sX,sY,sW,sH, hsv2hex(0,0,60) );
      
      // Zmien aktywny guidline
      tft.fillRect(sX+1, sY+sH/2-8, x-sX, 8, hsv2hex(80,240,180) ); 
      // Zmien pasywny guidline
      tft.fillRect(x, sY+sH/2-8, sW-(x-sX), 8, hsv2hex(0,0,90) ); 
      
      // Zmien polozenie buttona
        // nadpisz stare polozenie
      tft.fillRect(btnPrevX-sX+3, sY+3, sH-6, sH-6, hsv2hex(0,0,120) );
        // narysuj nowy
      tft.fillRoundRect(x-sX+3, sY+3, sH-6, sH-6, 8, hsv2hex(30,180,210) );
      btnPrevX = x;
      this->currVal = 255.f/(sW-sH)*(x-sX);
      (*var)= byte(this->currVal);
      tft.setCursor(sX,sY+sH);
      tft.setTextColor(hsv2hex(130,250,250), hsv2hex(250,250,250));
      tft.print("V: "+String(*var));
    }
  }

  int upd(int var)
  {
    
  }

  float upd(float var)
  {
    
  }

  bool alreadyDraw;
private:
  uint16_t sX, sY, sW, sH;
  uint16_t btnPrevX, btnPrevY;
  word colBTN, colASld; 
  float currVal;
  
};

// w produkcji...
class SimpleSlider
{
public:
  SimpleSlider(){};
  ~SimpleSlider(){};

  void draw(uint16_t sldX, uint16_t sldY, uint16_t sldW, uint16_t sldH, word cB, word cAS)
  {
    
  }

private:
  uint16_t sX, sY, sW, sH;
  uint16_t btnPrevX;
  word colBTN, colASld; 
  float currVal;
};

class ProgBar
{
public:
  ProgBar() : Mode('r'), cbg(0x000000), cbar(0xFFDD00) {};
  ~ProgBar(){};

  void draw(uint16_t pbarX, uint16_t pbarY, uint16_t pbarW, uint16_t pbarH, char typ, word cBG, word cBar)
  {
     //przypisz
     sx = pbarX; sy = pbarY; bw = pbarW; bh = pbarH; cbg = cBG; cbar = cBar; Mode = typ;
     //rysuj square
     switch (Mode)
     {
      case 's':
        {
          tft.fillRect(sx,sy,bw,bh,cbg);
          for (byte i=1; i<4; i++)
          {
            tft.drawRect(sx-i, sy-i, bw+2*i, bh+2*i, hsv2hex(0,0,0+5*i) );
          }
          break;
          
        }
      case 'r':
        {
          tft.fillRoundRect(sx,sy,bw,bh, 10,cbg);
          for (byte i=1; i<4; i++)
          {
            tft.drawRoundRect(sx-i, sy-i, bw+2*i, bh+2*i, 10, hsv2hex(0,0,0+5*i) );
          }
          break;
        }
      default: {break;}
     }
       
  }

  void upd(int imin, int imax, int ival)
  {
     switch (Mode)
     {
      case 's':
        {
          tft.fillRect(sx,sy, map(ival,imin,imax,0,bw) ,bh, cbar);
          break;
        }
      case 'r':
        {
          tft.fillRoundRect(sx,sy, map(ival,imin,imax,0,bw) ,bh, 10, cbar);
          break;
        }
      default: {break;}
     }    
  }
  
private:
uint16_t sx, sy, bw, bh;
word cbg, cbar;
char Mode;
};

// val changer STRUCT approach
class ValElem
{
public:
  ValElem(int *val, int vmin, int vmax, int INC, const char *NAZWA) : iVAL(val) , VMIN(vmin), VMAX(vmax), vType('i'), iINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(byte *val, int vmin, int vmax, int INC, const char *NAZWA) : bVAL(val) , VMIN(vmin), VMAX(vmax), vType('b'), iINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(word *val, int vmin, int vmax, int INC, const char *NAZWA) : wVAL(val) , VMIN(vmin), VMAX(vmax), vType('w'), iINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(uint16_t *val, int vmin, int vmax, int INC, const char *NAZWA) : u16VAL(val) , VMIN(vmin), VMAX(vmax), vType('l'), iINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(float *val, float vmin, float vmax, float INC, const char *NAZWA) : fVAL(val) , fVMIN(vmin), fVMAX(vmax), vType('f'), fINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(double *val, float vmin, float vmax, float INC, const char *NAZWA) : dVAL(val) , fVMIN(vmin), fVMAX(vmax), vType('d'), fINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }
  ~ValElem(){}

char *nazwa;
char vType = 'i';

int *iVAL = NULL;
byte *bVAL = NULL;
float *fVAL = NULL;
word *wVAL = NULL;
uint16_t *u16VAL = NULL;
double *dVAL = NULL;

int iINC = 1;
float fINC = 0.1;

int VMIN=0, VMAX=100;  
float fVMIN=0.0, fVMAX=100.0;  
private:

};

class ValChngObjArrtList
{
public:
  ValChngObjArrtList(ValElem **tab, byte ELEM ) : vElem(tab), elements(ELEM), listIter(0) {}
  ~ValChngObjArrtList(){}

  Kronos ValChngDel;
  int VCDdel = 25;
  void draw(uint16_t vX, uint16_t vY, uint16_t textW)
  {
    sx = vX; sy = vY; t_width = textW;
    // Gorne okno
    tft.setTextColor(TFT_WHITE);
    tft.fillRect(sx,sy,40,40,hsv2hex(140,240,255) );
      tft.drawCentreString("<", sx+20, sy+6, 4);
    tft.fillRect(sx+40,sy, t_width, 40,TFT_BLACK );
    tft.fillRect(sx+40+t_width,sy,40,40,hsv2hex(5,240,255) );
      tft.drawCentreString(">", sx+60+t_width, sy+6, 4);
    
    // Dolne okno
    tft.fillRect(sx,sy+40,40,40,hsv2hex(140,120,255) );
      tft.drawCentreString("-", sx+20, sy+46, 4);
    tft.fillRect(sx+40,sy+40, t_width ,40,hsv2hex(0,0,64) );
    tft.fillRect(sx+40+t_width ,sy+40,40,40,hsv2hex(5,120,255) );
      tft.drawCentreString("+", sx+60+t_width, sy+46, 4);

    tft.setTextFont(4);
    tft.drawCentreString(vElem[0]->nazwa, sx+((80+t_width)/2), sy+6, 4);
    switch(vElem[0]->vType)
    {
      case 'i': {tft.drawCentreString(String(*vElem[0]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'b': {tft.drawCentreString(String(*vElem[0]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'w': {tft.drawCentreString(String(*vElem[0]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'l': {tft.drawCentreString(String(*vElem[0]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }

      case 'f': { tft.drawFloat((*vElem[0]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
      case 'd': { tft.drawFloat((*vElem[0]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
      //case 'f': {tft.drawCentreString(String(*vElem[0]->fVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      //case 'd': {tft.drawCentreString(String(*vElem[0]->dVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      default: break;
    }
    
  }

  void chkIfPress()
  {
    // Typ wartosci ' - '
    if (x>sx && x<sx+40 && y>sy && y<sy+40 && listIter > 0 && ValChngDel.del(VCDdel*4))
    {
      listIter--;
      tft.fillRect(sx+40,sy, t_width, 40,TFT_BLACK );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      tft.drawCentreString(vElem[listIter]->nazwa, sx+((80+t_width)/2), sy+6, 4);  

      tft.fillRect(sx+40,sy+40,t_width,40,hsv2hex(0,0,64) );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      //tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4);
        switch(vElem[listIter]->vType)
        {
          case 'i': {tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          //case 'f': {tft.drawCentreString(String(*vElem[listIter]->fVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'b': {tft.drawCentreString(String(*vElem[listIter]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'w': {tft.drawCentreString(String(*vElem[listIter]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'l': {tft.drawCentreString(String(*vElem[listIter]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
          //case 'd': {tft.drawCentreString(String(*vElem[listIter]->dVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          
          case 'f': { tft.drawFloat((*vElem[listIter]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
          case 'd': { tft.drawFloat((*vElem[listIter]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
          default: break;
        }
    }
    // Typ wartosci ' + '
    else if (x>sx+40+t_width && x<sx+80+t_width && y>sy && y<sy+40 && listIter < elements-1 && ValChngDel.del(VCDdel*4))
    {
      listIter++;
      tft.fillRect(sx+40,sy,t_width,40,TFT_BLACK );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      tft.drawCentreString(vElem[listIter]->nazwa, sx+((80+t_width)/2), sy+6, 4); 

      tft.fillRect(sx+40,sy+40,t_width,40,hsv2hex(0,0,64) );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      //tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4);
        switch(vElem[listIter]->vType)
        {
          case 'i': {tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          //case 'f': {tft.drawCentreString(String(*vElem[listIter]->fVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'b': {tft.drawCentreString(String(*vElem[listIter]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'w': {tft.drawCentreString(String(*vElem[listIter]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'l': {tft.drawCentreString(String(*vElem[listIter]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
          //case 'd': {tft.drawCentreString(String(*vElem[listIter]->dVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          
          case 'f': { tft.drawFloat((*vElem[listIter]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
          case 'd': { tft.drawFloat((*vElem[listIter]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
          default: break;
        }      
    }
    // Val --
    else if (x>sx && x<sx+40 && y>sy+40 && y<sy+80 && ValChngDel.del(VCDdel))
    {
      switch(vElem[listIter]->vType)
      {
      case 'i': { if ((*vElem[listIter]->iVAL) > vElem[listIter]->VMIN) {(*vElem[listIter]->iVAL)-= (vElem[listIter]->iINC);} break; }
      case 'f': { if ((*vElem[listIter]->fVAL) > vElem[listIter]->fVMIN) {(*vElem[listIter]->fVAL)-= (vElem[listIter]->fINC);} break; }
      case 'b': { if ((*vElem[listIter]->bVAL) > vElem[listIter]->VMIN) {(*vElem[listIter]->bVAL)-= (vElem[listIter]->iINC);} break; }
      case 'w': { if ((*vElem[listIter]->wVAL) > vElem[listIter]->VMIN) {(*vElem[listIter]->wVAL)-= (vElem[listIter]->iINC);} break; }
      case 'l': { if ((*vElem[listIter]->u16VAL) > vElem[listIter]->VMIN) {(*vElem[listIter]->u16VAL)-= (vElem[listIter]->iINC);} break; }
      case 'd': { if ((*vElem[listIter]->dVAL) > vElem[listIter]->fVMIN) {(*vElem[listIter]->dVAL)-= (vElem[listIter]->fINC);} break; }
      default: break;
      }
      //(*vElem[listIter]->iVAL)--;
      tft.fillRect(sx+40,sy+40,t_width,40,hsv2hex(0,0,64) );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      switch(vElem[listIter]->vType)
      {
      case 'i': {tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      //case 'f': {tft.drawCentreString(String(*vElem[listIter]->fVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'b': {tft.drawCentreString(String(*vElem[listIter]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'w': {tft.drawCentreString(String(*vElem[listIter]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'l': {tft.drawCentreString(String(*vElem[listIter]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
      //case 'd': {tft.drawCentreString(String(*vElem[listIter]->dVAL), sx+((80+t_width)/2), sy+46, 4); break; }

      case 'f': { tft.drawFloat((*vElem[listIter]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
      case 'd': { tft.drawFloat((*vElem[listIter]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
      default: break;
      }
      //tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4);
    }
    // Val ++
    else if (x>sx+40+t_width && x<sx+80+t_width && y>sy+40 && y<sy+80 && ValChngDel.del(VCDdel))
    {
      switch(vElem[listIter]->vType)
      {
      case 'i': { if((*vElem[listIter]->iVAL) < vElem[listIter]->VMAX) {(*vElem[listIter]->iVAL)+= (vElem[listIter]->iINC);} break; }
      case 'f': { if((*vElem[listIter]->fVAL) < vElem[listIter]->fVMAX) {(*vElem[listIter]->fVAL)+= (vElem[listIter]->fINC);} break; }
      case 'b': { if((*vElem[listIter]->bVAL) < vElem[listIter]->VMAX) {(*vElem[listIter]->bVAL)+= (vElem[listIter]->iINC);} break; }
      case 'w': { if((*vElem[listIter]->wVAL) < vElem[listIter]->VMAX) {(*vElem[listIter]->wVAL)+= (vElem[listIter]->iINC);} break; }
      case 'l': { if((*vElem[listIter]->u16VAL) < vElem[listIter]->VMAX) {(*vElem[listIter]->u16VAL)+= (vElem[listIter]->iINC);} break; }
      case 'd': { if((*vElem[listIter]->dVAL) < vElem[listIter]->fVMAX) {(*vElem[listIter]->dVAL)+= (vElem[listIter]->fINC);} break; }
      default: break;
      }
      //(*vElem[listIter]->iVAL)++;
      tft.fillRect(sx+40,sy+40,t_width,40,hsv2hex(0,0,64) );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      switch(vElem[listIter]->vType)
      {
      case 'i': {tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      //case 'f': {tft.drawCentreString(String(*vElem[listIter]->fVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'b': {tft.drawCentreString(String(*vElem[listIter]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'w': {tft.drawCentreString(String(*vElem[listIter]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'l': {tft.drawCentreString(String(*vElem[listIter]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
      //case 'd': {tft.drawCentreString(String(*vElem[listIter]->dVAL), sx+((80+t_width)/2), sy+46, 4); break; }

      case 'f': { tft.drawFloat((*vElem[listIter]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
      case 'd': { tft.drawFloat((*vElem[listIter]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
      default: break;
      }
      //tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4);      
    }
  }
  // iterowanie po **arr
  // (*tab)++ < ELEM

private:
  uint16_t sx, sy, t_width;
  ValElem **vElem;
  int elements;
  word bg1, bg2;
  byte listIter;

};

#endif
