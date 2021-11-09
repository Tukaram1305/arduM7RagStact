#ifndef RAGCLSS_H
#define RAGCLSS_H

/*  WYROWNYWANIE TEKSTU 
 *   
 *   tft.drawCentreString("69", x, y, 8);
 *   tft.setFreeFont(FF18);
 *   
TL_DATUM = 0 = Top left
TC_DATUM = 1 = Top centre
TR_DATUM = 2 = Top right
ML_DATUM = 3 = Middle left
MC_DATUM = 4 = Middle centre
MR_DATUM = 5 = Middle right
BL_DATUM = 6 = Bottom left
BC_DATUM = 7 = Bottom centre
BR_DATUM = 8 = Bottom right

L_BASELINE =  9 = Left character baseline (Line the 'A' character would sit on)
C_BASELINE = 10 = Centre character baseline
R_BASELINE = 11 = Right character baseline

So you can use lines to position text like:
tft.setTextDatum(BC_DATUM); // Set datum to bottom centre
 */
 
class Kronos
{
  /*
   * Metody         Zwraca              Opis
   * del(x)       - bool(true)        - delay milisekundy
   * delMicro(x)  - bool(true)        - delay mikrosekundy
   * startDel     - bool(true)        - opozniony start (milisekundy)
   * 
   * CNT          - void              - zacznij odliczanie
   * STP          - czas (mili)       - stop odliczanie
   * 
   * ONCE         - bool (true)       - true jezeli raz wykonalo
   * ONCE_RES     - void              - resetuj ONCE
   */
   
  public:
  Kronos() : mili(0), rev(true), already(false) {};
  ~Kronos(){};
  bool del (unsigned int delx)
  {
    if (millis()>mili+delx)
    {
      this->mili = millis();
      return true;
    }
    else return false;
  }
  bool delMicro (unsigned int delx)
  {
    if (micros()>mili+delx)
    {
      this->mili = micros();
      return true;
    }
    else return false;
  }
  bool startDel (unsigned int delx)
  {
    if (rev == true)
    {
      this->mili = millis();
      this->rev = false;
      return false;
    }
    if (this->rev == false)
    {
      if (millis()<mili+delx) return false;
      else
      {
        this->rev = true;
        return true;
      }
    }
  }

  void CNT ()
  {
    this->mili = millis();
  }
  unsigned long STP ()
  {
    this->miliNew = millis();
    return (this->miliNew-(this->mili));
  }
  
  bool ONCE ()
  {
	  if (this->already == true) return false;
	  else 
	  {
		  this->already = true;
      return true;
	  }
  }

  void RESET ()
  {
    if (this->already == true) this->already = false;
  }
  private:
  bool already = false;
  bool rev = true;
  unsigned long mili;
  unsigned long miliNew;
};

#endif
