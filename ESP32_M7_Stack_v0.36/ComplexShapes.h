#ifndef COMPLEXSHAPES_H
#define COMPLEXSHAPES_H

class ExitBtn
{
public:
  ExitBtn(){};
  ~ExitBtn(){};

private:
};

class BatIndicator
{
public:
  BatIndicator(){};
  ~BatIndicator(){};

  void draw(uint16_t sX, uint16_t sY, uint16_t bW)
  {
    sx = sX; sy = sY; bw = bW;
    tft.fillRect(sx,sy,bW,20,TFT_WHITE);
    tft.fillRect(sx+bW,sy+6,5,8,hsv2hex(0,0,190) );
    tft.drawRect(sx,sy,bW,20,hsv2hex(0,0,190));  
  }

  void bUpd(uint16_t bVal, uint16_t bMin, uint16_t bMax)
  {
    if (bVal < 100)
    {
      if (power == false)
      {
        tft.fillRect(sx+1,sy+1,bw-2,18,hsv2hex(0,0,190));
        tft.fillRect(sx+2,sy+2,bw-4,16,TFT_BLUE);
        power = true;   

      tft.setTextSize(1);
      tft.setCursor(sx,sy+bh+2);
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextFont(2);
      tft.print("USB");
      } 
    }
    else
    {
      byte pCent = byte(map(bVal,bMin,bMax,0,bw-4));
      tft.fillRect(sx+1,sy+1,bw-2,18,hsv2hex(0,0,190));
      tft.fillRect(sx+2,sy+2,pCent,16,TFT_GREEN);

      tft.setTextSize(1);
      tft.setCursor(sx,sy+bh+2);
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextFont(2);
      byte pCent100 = byte(map(bVal,bMin,bMax,0,100) );
      tft.println(String(pCent100)+"%");
    }
  }

  bool power = false;
  
private:
uint16_t sx, sy, bw, bh;

};
#endif
