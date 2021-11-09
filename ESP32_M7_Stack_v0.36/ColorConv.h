#ifndef COLORCONV_H
#define COLORCONV_H

double min(double a, double b)
{
  if (a<b) return a;
  else return b;
}
double max(double a, double b)
{
  if (a>b) return a;
  else return b;
}

void hsv2rgb2(float h, float s, float v, byte *r, byte *g, byte *b)
{
  int i;
  float f,p,q,t;
  
  h = max(0.0, min(360.0, h));
  s = max(0.0, min(100.0, s));
  v = max(0.0, min(100.0, v));
  
  s /= 100;
  v /= 100;
  
  if(s == 0) {
    // Achromatic (grey)
    *r = *g = *b = round(v*255);
    return;
  }

  h /= 60; // sector 0 to 5
  i = floor(h);
  f = h - i; // factorial part of h
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));
  switch(i) {
    case 0:
      *r = round(255*v);
      *g = round(255*t);
      *b = round(255*p);
      break;
    case 1:
      *r = round(255*q);
      *g = round(255*v);
      *b = round(255*p);
      break;
    case 2:
      *r = round(255*p);
      *g = round(255*v);
      *b = round(255*t);
      break;
    case 3:
      *r = round(255*p);
      *g = round(255*q);
      *b = round(255*v);
      break;
    case 4:
      *r = round(255*t);
      *g = round(255*p);
      *b = round(255*v);
      break;
    default: // case 5:
      *r = round(255*v);
      *g = round(255*p);
      *b = round(255*q);
    }
}

void hsv2rgb3(byte h, byte s, byte v, byte *r, byte *g, byte *b )
{
    double H , S , V ,
            P, Q, T,
            RC,GC,BC,
            fract;
            
    H = 1.4117647058823529411764705882353*h; // mapuj 0-360 -> 0-255
    S = 0.0039215686274509803921568627451*s; //       0-1   -> 0-255
    V = 0.0039215686274509803921568627451*v; //       0-1   -> 0-255

    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);

    P = V*(1. - S);
    Q = V*(1. - S*fract);
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
    { RC = V; GC = T; BC = P; }
    else if (1. <= H && H < 2.)
    { RC = Q; GC = V; BC = P; }
    else if (2. <= H && H < 3.)
    { RC = P; GC = V; BC = T; }
    else if (3. <= H && H < 4.)
    { RC = P; GC = Q; BC = V; }
    else if (4. <= H && H < 5.)
    { RC = T; GC = P; BC = V; }
    else if (5. <= H && H < 6.)
    { RC = V; GC = P; BC = Q; }
    else
    { RC = 0; GC = 0; BC = 0; }

    (*r) = byte(RC*255); (*g) = byte(GC*255); (*b) = byte(BC*255);
}

typedef struct {
    double r;       // ∈ [0, 1]
    double g;       // ∈ [0, 1]
    double b;       // ∈ [0, 1]
} rgb;

typedef struct {
    double h;       // ∈ [0, 360]
    double s;       // ∈ [0, 1]
    double v;       // ∈ [0, 1]
} hsv;

rgb hsv2rgb(hsv HSV)
{
    rgb RGB;
    double H = HSV.h, S = HSV.s, V = HSV.v,
            P, Q, T,
            fract;

    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);

    P = V*(1. - S);
    Q = V*(1. - S*fract);
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
        RGB = (rgb){.r = V, .g = T, .b = P};
    else if (1. <= H && H < 2.)
        RGB = (rgb){.r = Q, .g = V, .b = P};
    else if (2. <= H && H < 3.)
        RGB = (rgb){.r = P, .g = V, .b = T};
    else if (3. <= H && H < 4.)
        RGB = (rgb){.r = P, .g = Q, .b = V};
    else if (4. <= H && H < 5.)
        RGB = (rgb){.r = T, .g = P, .b = V};
    else if (5. <= H && H < 6.)
        RGB = (rgb){.r = V, .g = P, .b = Q};
    else
        RGB = (rgb){.r = 0., .g = 0., .b = 0.};

    return RGB;
}

word hsv2hex(byte h, byte s, byte v)
{
    double  H, S, V,
            P, Q, T,
            RC,GC,BC,
            fract;


    H = 1.4117647058823529411764705882353*h; // mapuj 0-360 -> 0-255
    S = 0.0039215686274509803921568627451*s; //       0-1   -> 0-255
    V = 0.0039215686274509803921568627451*v; //       0-1   -> 0-255

    byte  bR, bG, bB;

    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);

    P = V*(1. - S);
    Q = V*(1. - S*fract);
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
    {
        RC = V;
        GC = T;
        BC = P;
    }
    else if (1. <= H && H < 2.)
    {
        RC = Q;
        GC = V;
        BC = P;
    }
    else if (2. <= H && H < 3.)
    {
        RC = P;
        GC = V;
        BC = T;
    }
    else if (3. <= H && H < 4.)
    {
        RC = P;
        GC = Q;
        BC = V;
    }
    else if (4. <= H && H < 5.)
    {
        RC = T;
        GC = P;
        BC = V;
    }
    else if (5. <= H && H < 6.)
    {
        RC = V;
        GC = P;
        BC = Q;
    }
    else
    {
        RC = 0;
        GC = 0;
        BC = 0;
    }

    bR = byte(RC*255); bG = byte(GC*255); bB = byte(BC*255);
    
    return ( ((bR & 0xF8) << 8) | ((bG & 0xFC) << 3) | (bB >> 3) );
}

word rgb2hex( byte R, byte G, byte B)
{
  return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );
}

#endif
