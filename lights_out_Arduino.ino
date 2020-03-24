/*
    @file lights_out_Arduino.ino
    @brief A lights out game that runs on Arduino UNO and MCUFriend 2.4 inch ILI9335 LCD monitor shield.

    @author Kei Takagi
    @date 2020.03.24

    Copyright (c) 2020 Kei Takagi
*/




#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
#include <TouchScreen.h>

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))
#define BOXSIZE       40
#define BOXBLANK      10
#define BOX           (BOXSIZE + BOXBLANK)
#define MATRIX        5

#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

const int XP = 6, XM = A2, YP = A1, YM = 7; //240x320 ID=0x9335
const int TS_TOP = 94, TS_LEFT = 117, TS_RIGHT = 909,  TS_BOTOM = 916;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

byte  LightsGrid[MATRIX][MATRIX];
int16_t PENRADIUS = 1;
uint16_t ID;

uint16_t Color;

#define A1  LightsGrid[0][0]
#define A2  LightsGrid[1][0]
#define A3  LightsGrid[2][0]
#define A4  LightsGrid[3][0]
#define A5  LightsGrid[4][0]

#define B1  LightsGrid[0][1]
#define B2  LightsGrid[1][1]
#define B3  LightsGrid[2][1]
#define B4  LightsGrid[3][1]
#define B5  LightsGrid[4][1]

#define C1  LightsGrid[0][2]
#define C2  LightsGrid[1][2]
#define C3  LightsGrid[2][2]
#define C4  LightsGrid[3][2]
#define C5  LightsGrid[4][2]

#define D1  LightsGrid[0][3]
#define D2  LightsGrid[1][3]
#define D3  LightsGrid[2][3]
#define D4  LightsGrid[3][3]
#define D5  LightsGrid[4][3]

#define E1  LightsGrid[0][4]
#define E2  LightsGrid[1][4]
#define E3  LightsGrid[2][4]
#define E4  LightsGrid[3][4]
#define E5  LightsGrid[4][4]

void setup(void)
{
  randomSeed(analogRead(0));
  tft.reset();
  ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(0);
  tft.fillScreen(BLACK);

  /*
     There are many variations on the 2.4 inch shield of MCUFriend.
     It may not work as it is. So, I added a simple setting mode.
     If you start up while touching the touch panel, you will enter the setting mode.
     Touch the four corners of the screen and change the following values ​​displayed.
     TS_TOP TS_LEFT TS_RIGHT TS_BOTOM
  */

  tp = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if ( MINPRESSURE < tp.z && tp.z < MAXPRESSURE) {
    ledSetup();
  }

  Initialize();
  lcdDraw();
}

void Initialize()
{
  int i, px, py, pattern;
  tft.fillScreen(BLACK);
  for ( i = 0; i < 25 ; i++) {
    px = random(0, 5);
    py = random(0, 5);
    setPixel(px - 1, py);
    setPixel(px + 1, py);
    setPixel(px, py);
    setPixel(px, py - 1);
    setPixel(px, py + 1);
  }

  pattern = random(6 , 15);
  switch (pattern)
  {
    case 0:
      Color = BLUE;
      break;
    case 1:
      Color = RED;
      break;
    case 2:
      Color = GREEN;
      break;
    case 3:
      Color = CYAN;
      break;
    case 4:
      Color = MAGENTA;
      break;
    case 5:
      Color = YELLOW;
      break;
    case 6:
      Color = RGB(random(220, 256), random(220, 256), 0);
      break;
    case 7:
      Color = RGB(random(220, 256), 0, random(220, 256));
      break;
    case 8:
      Color = RGB(0, 0, random(220, 256));
      break;
    case 9:
      Color = RGB(0, random(220, 256), 0);
      break;
    case 10:
      Color = RGB(random(220, 256), 0, 0);
      break;
    default:
      Color = RGB(random(220, 256), random(220, 256), random(220, 256));
      break;
  }
}

void fivePixels(int x, int y)
{
  setPixel(x - 1, y);
  setPixel(x + 1, y);
  setPixel(x, y);
  setPixel(x, y - 1);
  setPixel(x, y + 1);
  lcdDraw();
}

void setPixel(int x, int y)
{
  if (0 <= x && x < MATRIX && 0 <= y && y < MATRIX ) {
    LightsGrid[x][y] = ! LightsGrid[x][y];
  }
}


void lcdDraw()
{
  int x, y;
  int wx, wy;

  for ( x = 0; x < MATRIX ; x++) {
    for ( y = 0; y < MATRIX ; y++) {
      wx = BOX * x ;
      wy = BOX * y ;
      if (LightsGrid[x][y])
        tft.fillRect(wx, wy, BOXSIZE, BOXSIZE, Color);
      else
        tft.fillRect(wx, wy, BOXSIZE, BOXSIZE, BLACK);
      tft.drawRect(wx, wy, BOXSIZE, BOXSIZE, WHITE);
    }
  }

  tft.setTextSize(2);
  tft.setCursor(10, 280);
  tft.print("LIGHTS OUT");
  tft.setCursor(160, 290);
  tft.print("answer");
}


void answer()
{
  int x, y, wx, wy, val, pattern;
  int answerGrid[MATRIX][MATRIX];

  while (1) {
    answerGrid[0][0] = 0;
    answerGrid[1][0] = 0;
    answerGrid[2][0] = (B1 + C1 + B2 + C2 + D2 + A3 + B3 + E3 + B4 + E4 + C5 + D5) % 2;
    answerGrid[3][0] = (A1 + B1 + C1 + A3 + B3 + C3 + B4 + D4 + C5 + D5 + E5) % 2;
    answerGrid[4][0] = (A1 + C1 + D1 + A3 + C3 + D3 + A4 + E4 + B5 + C5 + E5) % 2;

    answerGrid[0][1] = A1 % 2;
    answerGrid[1][1] = (C1 + B2 + C2 + D2 + A3 + B3 + E3 + B4 + E4 + C5 + D5) % 2;
    answerGrid[2][1] = (A1 + C1 + B2 + C2 + D2 + C3 + E3 + D4 + E4 + E5) % 2;
    answerGrid[3][1] = (C1 + B2 + C2 + D2 + A3 + D3 + E3 + A4 + D4 + B5 + C5) % 2;
    answerGrid[4][1] = (A1 + A3 + E3 + A4 + B4 + D4 + E4 + A5 + E5) % 2;

    answerGrid[0][2] = (A1 + C1 + A2 + B2 + C2 + D2 + A3 + B3 + E3 + B4 + E4 + C5 + D5) % 2;
    answerGrid[1][2] = (B2 + A3 + B3 + C3 + B4 + D4 + C5 + D5 + E5) % 2;
    answerGrid[2][2] = (B1 + C1 + E1 + A2 + E2 + A3 + C3 + D3 + C4 + A5 + B5) % 2;
    answerGrid[3][2] = (C1 + D1 + E1 + D2) % 2;
    answerGrid[4][2] = (D1 + B2 + C2 + D2 + E2 + A3 + C3 + A4 + B4 + A5) % 2;

    answerGrid[0][3] = (C1 + A2 + C2 + D2 + A3 + C3 + E3 + D4 + E4 + E5) % 2;
    answerGrid[1][3] = (C1 + D1 + B2 + E2 + A3 + B3 + E3 + B4 + C4 + D4 + C5) % 2;
    answerGrid[2][3] = (A1 + A3 + A4 + B4 + A5) % 2;
    answerGrid[3][3] = (B1 + C1 + A2 + D2 + A3 + D3 + E3 + B4 + C4 + D4 + C5) % 2;
    answerGrid[4][3] = (A2 + B2 + C3 + A4 + C4 + D4 + A5 + C5) % 2;

    answerGrid[0][4] = (A1 + C1 + D1 + E2 + A3 + C3 + E3 + A4 + C4 + D5 + E5) % 2;
    answerGrid[1][4] = (C1 + D1 + E1 + D2 + D4 + C5 + D5 + E5) % 2;
    answerGrid[2][4] = (A1 + C1 + D1 + E1 + B2 + D2 + B3 + C3 + A4 + B4 + B5) % 2;
    answerGrid[3][4] = (A1 + B1 + D1 + E1 + B2 + C3 + D3 + E3 + D4) % 2;
    answerGrid[4][4] = (B1 + C1 + D1 + C2 + E2 + D3 + E3 + E4) % 2;

    pattern = random(0, 4);
    for ( x = 0; x < MATRIX ; x++) {
      for ( y = 0; y < MATRIX ; y++) {
        switch (pattern) {
          case 0:
            wx = x;
            wy = MATRIX - y - 1;
            break;
          case 1:
            wx = MATRIX - x - 1;
            wy = y;
            break;
          case 2:
            wx = MATRIX - x - 1;
            wy = MATRIX - y - 1;
            break;
          case 3:
            wx = x;
            wy = y;
            break;
        }
        if (answerGrid[wx][wy]) {
          fivePixels(wx, wy);
          delay(200);
        }
      }
    }

    val = 0;
    for ( x = 0; x < MATRIX ; x++) {
      for ( y = 0; y < MATRIX ; y++) {
        val += LightsGrid[x][y];
      }
    }
    if (val == 0)break;
  }
}

void ledSetup()
{
  int minx = 1000;
  int miny = 1000;
  int maxx = 0;
  int maxy = 0;
  int xpos, ypos;

  while (1) {
    tp = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    if ( MINPRESSURE < tp.z && tp.z < MAXPRESSURE) {
      xpos = map(tp.x, TS_LEFT, TS_RIGHT, 0, tft.width());
      ypos = map(tp.y, TS_TOP, TS_BOTOM, 0, tft.height());
      if (minx > tp.x)minx = tp.x;
      if (maxx < tp.x)maxx = tp.x;
      if (miny > tp.y)miny = tp.y;
      if (maxy < tp.y)maxy = tp.y;

      tft.fillRect(0, 120, tft.width(), 100 , BLACK);
      tft.setTextSize(2);
      tft.setCursor(0, 120);
      tft.print("x=" + String(tp.x) + " y=" + String(tp.y));
      tft.setCursor(0, 136);
      tft.print("xpos=" + String(xpos) + " ypos=" + String(ypos));

      tft.setCursor(0, 152);
      tft.print(" TS_TOP=" + String(minx));
      tft.setCursor(0, 168);
      tft.print(" TS_LEFT=" + String(miny));
      tft.setCursor(0, 184);
      tft.print(" TS_RIGHT=" + String(maxx));
      tft.setCursor(0, 200);
      tft.print(" TS_BOTOM=" + String(maxy));
    }
  }
}


void loop()
{
  int minx = 1000;
  int miny = 1000;
  int maxx = 0;
  int maxy = 0;

  int xpos, ypos;
  int px, py;

  tp = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if ( MINPRESSURE < tp.z && tp.z < MAXPRESSURE) {
    xpos = map(tp.x, TS_LEFT, TS_RIGHT, 0, tft.width());
    ypos = map(tp.y, TS_TOP, TS_BOTOM, 0, tft.height());

    px = xpos  /  BOX;
    py = ypos  /  BOX;

    if (0 <= px && px < MATRIX && 0 <= py && py < MATRIX ) {
      fivePixels(px, py);
      delay(200);
    }

    if (150 <= xpos && xpos < tft.width() && 290 <= ypos && ypos < tft.height() ) {
      answer();
      delay(1000);
      Initialize();
      lcdDraw();
    }
  }
}
