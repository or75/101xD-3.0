#ifndef CTABLEFONT_H
#define CTABLEFONT_H

class cTableFont
{
  public:
    cTableFont();
    void drawString(bool bCenter, int x, int y, int r, int g, int b, const char *pInput, ...); 
    int iGetLength(const char *pInput, ...);
    int iGetWidth(const char *pInput, ...);

  private:
    void pixel(int x, int y, int w, int h, int r, int g, int b, int a, bool bBlend=false);
    void drawChar(const char *pChar);
    bool bParseTableFont(const char *pFileName);
    bool bTable[255][10][15];
};
extern cTableFont gFont;

#endif
