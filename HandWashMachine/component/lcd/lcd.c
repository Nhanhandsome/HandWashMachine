/*
 * lcd.c
 *
 *  Created on: Mar 10, 2023
 *      Author: Admin
 */

#include "lcd.h"
#include "board.h"
#include "i2c/i2c.h"
#include "stdio.h"
#include "string.h"
#include <lcd/font.h>



static uint8_t ssd1306_buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF
#if (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH > 96*16)
,
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#if (SSD1306_LCDHEIGHT == 64)
,
0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
#endif
};
uint8_t _i2caddr, _vccstate, x_pos, y_pos, text_size;
bool wrap = true;

void ssd1306_command(uint8_t c)
{
    uint8_t data[2] = {0x00,c};   // Co = 0, D/C = 0
    i2c_write(_i2caddr, data, 2);
}
void SSD1306_Begin(uint8_t vccstate, uint8_t i2caddr)
{
	  _vccstate = vccstate;
	  _i2caddr  = i2caddr;
	  delay_ms(10);

	  #ifdef SSD1306_RST
	    output_low(SSD1306_RST);
	    output_drive(SSD1306_RST);
	    delay_ms(10);
	    output_high(SSD1306_RST);
	  #endif

	  // Init sequence
	  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
	  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	  ssd1306_command(0x80);                                  // the suggested ratio 0x80

	  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
	  ssd1306_command(SSD1306_LCDHEIGHT - 1);

	  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	  ssd1306_command(0x0);                                   // no offset
	  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
	  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
	  if (vccstate == SSD1306_EXTERNALVCC)
	    { ssd1306_command(0x10); }
	  else
	    { ssd1306_command(0x14); }
	  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
	  ssd1306_command(0x00);                                  // 0x0 act like ks0108
	  ssd1306_command(SSD1306_SEGREMAP | 0x1);
	  ssd1306_command(SSD1306_COMSCANDEC);

	 #if defined SSD1306_128_32
	  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	  ssd1306_command(0x02);
	  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
	  ssd1306_command(0x8F);

	#elif defined SSD1306_128_64
	  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	  ssd1306_command(0x12);
	  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
	  if (vccstate == SSD1306_EXTERNALVCC)
	    { ssd1306_command(0x9F); }
	  else
	    { ssd1306_command(0xCF); }

	#elif defined SSD1306_96_16
	  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	  ssd1306_command(0x2);   //ada x12
	  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
	  if (vccstate == SSD1306_EXTERNALVCC)
	    { ssd1306_command(0x10); }
	  else
	    { ssd1306_command(0xAF); }

	#endif

	  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
	  if (vccstate == SSD1306_EXTERNALVCC)
	    { ssd1306_command(0x22); }
	  else
	    { ssd1306_command(0xF1); }
	  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
	  ssd1306_command(0x40);
	  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

	  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

	  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel

	  // set cursor to (0, 0)
	  x_pos = 0;
	  y_pos = 0;
	  // set text size to 1
	  text_size = 1;
}
void SSD1306_DrawPixel(uint8_t x, uint8_t y, bool color)
{
	if ((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
		return;
	if (color)
		ssd1306_buffer[x + (uint16_t) (y / 8) * SSD1306_LCDWIDTH] |= (1
				<< (y & 7));
	else
		ssd1306_buffer[x + (uint16_t) (y / 8) * SSD1306_LCDWIDTH] &= ~(1
				<< (y & 7));
}
void SSD1306_StartScrollRight(uint8_t start, uint8_t stop)
{
	  ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	  ssd1306_command(0X00);
	  ssd1306_command(start);
	  ssd1306_command(0X00);
	  ssd1306_command(stop);
	  ssd1306_command(0X00);
	  ssd1306_command(0XFF);
	  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}
void SSD1306_StartScrollLeft(uint8_t start, uint8_t stop)
{
	  ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
	  ssd1306_command(0X00);
	  ssd1306_command(start);
	  ssd1306_command(0X00);
	  ssd1306_command(stop);
	  ssd1306_command(0X00);
	  ssd1306_command(0XFF);
	  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}
void SSD1306_StartScrollDiagRight(uint8_t start, uint8_t stop)
{
	  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	  ssd1306_command(0X00);
	  ssd1306_command(SSD1306_LCDHEIGHT);
	  ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	  ssd1306_command(0X00);
	  ssd1306_command(start);
	  ssd1306_command(0X00);
	  ssd1306_command(stop);
	  ssd1306_command(0X01);
	  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}
void SSD1306_StartScrollDiagLeft(uint8_t start, uint8_t stop)
{
	  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	  ssd1306_command(0X00);
	  ssd1306_command(SSD1306_LCDHEIGHT);
	  ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	  ssd1306_command(0X00);
	  ssd1306_command(start);
	  ssd1306_command(0X00);
	  ssd1306_command(stop);
	  ssd1306_command(0X01);
	  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}
void SSD1306_StopScroll(void)
{
	  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}
void SSD1306_Dim(bool dim)
{
	  uint8_t contrast;
	  if (dim)
	    contrast = 0; // Dimmed display
	  else {
	    if (_vccstate == SSD1306_EXTERNALVCC)
	      contrast = 0x9F;
	    else
	      contrast = 0xCF;
	  }
	  // the range of contrast to too small to be really useful
	  // it is useful to dim the display
	  ssd1306_command(SSD1306_SETCONTRAST);
	  ssd1306_command(contrast);
}
void SSD1306_Display(void)
{
	  ssd1306_command(SSD1306_COLUMNADDR);
	  ssd1306_command(0);   // Column start address (0 = reset)
	  ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

	  ssd1306_command(SSD1306_PAGEADDR);
	  ssd1306_command(0); // Page start address (0 = reset)
	  #if SSD1306_LCDHEIGHT == 64
	    ssd1306_command(7); // Page end address
	  #endif
	  #if SSD1306_LCDHEIGHT == 32
	    ssd1306_command(3); // Page end address
	  #endif
	  #if SSD1306_LCDHEIGHT == 16
	    ssd1306_command(1); // Page end address
	  #endif
	  char buff[17] = {0};
	  for (uint16_t i = 0; i < (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT / 8); i++) {
	      // send a bunch of data in one xmission
	      buff[0] = 0x40;
	      memcpy(buff+1,ssd1306_buffer,16);
	      i2c_write(_i2caddr,(uint8_t*) buff,17);
	      i+= 16;
	      while(!i2c_writeed());
	      i--;
	    }
}
void SSD1306_ClearDisplay(void)
{
	  for (uint16_t i = 0; i < (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT / 8); i++)
	    ssd1306_buffer[i] = 0;
}
void SSD1306_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		bool color)
{
	bool steep;
	int8_t ystep;
	uint8_t dx, dy;
	int16_t err;
	steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		ssd1306_swap(x0, y0);
		ssd1306_swap(x1, y1);
	}
	if (x0 > x1)
	{
		ssd1306_swap(x0, x1);
		ssd1306_swap(y0, y1);
	}
	dx = x1 - x0;
	dy = abs(y1 - y0);

	err = dx / 2;
	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	for (; x0 <= x1; x0++)
	{
		if (steep)
		{
			if (color)
				SSD1306_DrawPixel(y0, x0,true);
			else
				SSD1306_DrawPixel(y0, x0, false);
		}
		else
		{
			if (color)
				SSD1306_DrawPixel(x0, y0,true);
			else
				SSD1306_DrawPixel(x0, y0, false);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}
void SSD1306_DrawFastHLine(uint8_t x, uint8_t y, uint8_t w, bool color)
{
	   SSD1306_DrawLine(x, y, x + w - 1, y, color);
}
void SSD1306_DrawFastVLine(uint8_t x, uint8_t y, uint8_t h, bool color)
{
	SSD1306_DrawLine(x, y, x, y + h - 1, color);
}
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color)
{
	for (int16_t i = x; i < x + w; i++)
	    SSD1306_DrawFastVLine(i, y, h, color);
}
void SSD1306_FillScreen(bool color)
{
	SSD1306_FillRect(0, 0, SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT, color);
}
void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r)
{
	  int16_t f = 1 - r;
	  int16_t ddF_x = 1;
	  int16_t ddF_y = -2 * r;
	  int16_t x = 0;
	  int16_t y = r;

	  SSD1306_DrawPixel(x0  , y0 + r,true);
	  SSD1306_DrawPixel(x0  , y0 - r,true);
	  SSD1306_DrawPixel(x0 + r, y0,true);
	  SSD1306_DrawPixel(x0 - r, y0,true);

	  while (x < y) {
	    if (f >= 0) {
	      y--;
	      ddF_y += 2;
	      f += ddF_y;
	    }
	    x++;
	    ddF_x += 2;
	    f += ddF_x;

	    SSD1306_DrawPixel(x0 + x, y0 + y,true);
	    SSD1306_DrawPixel(x0 - x, y0 + y,true);
	    SSD1306_DrawPixel(x0 + x, y0 - y,true);
	    SSD1306_DrawPixel(x0 - x, y0 - y,true);
	    SSD1306_DrawPixel(x0 + y, y0 + x,true);
	    SSD1306_DrawPixel(x0 - y, y0 + x,true);
	    SSD1306_DrawPixel(x0 + y, y0 - x,true);
	    SSD1306_DrawPixel(x0 - y, y0 - x,true);
	  }

}
void SSD1306_DrawCircleHelper(int16_t x0, int16_t y0, int16_t r,
		uint8_t cornername)
{
	  int16_t f     = 1 - r;
	  int16_t ddF_x = 1;
	  int16_t ddF_y = -2 * r;
	  int16_t x     = 0;
	  int16_t y     = r;

	  while (x < y) {
	    if (f >= 0) {
	      y--;
	      ddF_y += 2;
	      f     += ddF_y;
	    }
	    x++;
	    ddF_x += 2;
	    f     += ddF_x;
	    if (cornername & 0x4) {
	      SSD1306_DrawPixel(x0 + x, y0 + y,true);
	      SSD1306_DrawPixel(x0 + y, y0 + x,true);
	    }
	    if (cornername & 0x2) {
	      SSD1306_DrawPixel(x0 + x, y0 - y,true);
	      SSD1306_DrawPixel(x0 + y, y0 - x,true);
	    }
	    if (cornername & 0x8) {
	      SSD1306_DrawPixel(x0 - y, y0 + x,true);
	      SSD1306_DrawPixel(x0 - x, y0 + y,true);
	    }
	    if (cornername & 0x1) {
	      SSD1306_DrawPixel(x0 - y, y0 - x,true);
	      SSD1306_DrawPixel(x0 - x, y0 - y,true);
	    }
	  }

}
void SSD1306_FillCircle(int16_t x0, int16_t y0, int16_t r, bool color)
{
	  SSD1306_DrawFastVLine(x0, y0 - r, 2 * r + 1, color);
	  SSD1306_FillCircleHelper(x0, y0, r, 3, 0, color);
}
void SSD1306_FillCircleHelper(int16_t x0, int16_t y0, int16_t r,
		uint8_t cornername, int16_t delta, bool color)
{
	  int16_t f     = 1 - r;
	  int16_t ddF_x = 1;
	  int16_t ddF_y = -2 * r;
	  int16_t x     = 0;
	  int16_t y     = r;

	  while (x < y) {
	    if (f >= 0) {
	      y--;
	      ddF_y += 2;
	      f     += ddF_y;
	    }
	    x++;
	    ddF_x += 2;
	    f     += ddF_x;

	    if (cornername & 0x01) {
	      SSD1306_DrawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
	      SSD1306_DrawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
	    }
	    if (cornername & 0x02) {
	      SSD1306_DrawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
	      SSD1306_DrawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
	    }
	  }
}
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	  SSD1306_DrawFastHLine(x, y, w,true);
	  SSD1306_DrawFastHLine(x, y + h - 1, w,true);
	  SSD1306_DrawFastVLine(x, y, h,true);
	  SSD1306_DrawFastVLine(x + w - 1, y, h,true);
}
void SSD1306_DrawRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
		uint8_t r)
{
	  SSD1306_DrawFastHLine(x + r, y, w - 2 * r,true); // Top
	  SSD1306_DrawFastHLine(x + r, y + h - 1, w - 2 * r,true); // Bottom
	  SSD1306_DrawFastVLine(x, y + r, h - 2 * r,true); // Left
	  SSD1306_DrawFastVLine(x + w - 1, y + r, h - 2 * r,true); // Right
	  // draw four corners
	  SSD1306_DrawCircleHelper(x + r, y + r, r, 1);
	  SSD1306_DrawCircleHelper(x + w - r - 1, y + r, r, 2);
	  SSD1306_DrawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4);
	  SSD1306_DrawCircleHelper(x + r, y + h - r - 1, r, 8);
}
void SSD1306_FillRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
		uint8_t r, bool color)
{
	  // smarter version
	  SSD1306_FillRect(x + r, y, w - 2 * r, h, color);
	  // draw four corners
	  SSD1306_FillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	  SSD1306_FillCircleHelper(x + r        , y + r, r, 2, h - 2 * r - 1, color);
}
void SSD1306_DrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		uint8_t x2, uint8_t y2)
{
	  SSD1306_DrawLine(x0, y0, x1, y1,true);
	  SSD1306_DrawLine(x1, y1, x2, y2,true);
	  SSD1306_DrawLine(x2, y2, x0, y0,true);
}
void SSD1306_FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		int16_t x2, int16_t y2, bool color)
{
	  int16_t a, b, y, last;
	  // Sort coordinates by Y order (y2 >= y1 >= y0)
	  if (y0 > y1) {
	    ssd1306_swap(y0, y1); ssd1306_swap(x0, x1);
	  }
	  if (y1 > y2) {
	    ssd1306_swap(y2, y1); ssd1306_swap(x2, x1);
	  }
	  if (y0 > y1) {
	    ssd1306_swap(y0, y1); ssd1306_swap(x0, x1);
	  }

	  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
	    a = b = x0;
	    if(x1 < a)      a = x1;
	    else if(x1 > b) b = x1;
	    if(x2 < a)      a = x2;
	    else if(x2 > b) b = x2;
	    SSD1306_DrawFastHLine(a, y0, b - a + 1, color);
	    return;
	  }

	  int16_t
	  dx01 = x1 - x0,
	  dy01 = y1 - y0,
	  dx02 = x2 - x0,
	  dy02 = y2 - y0,
	  dx12 = x2 - x1,
	  dy12 = y2 - y1;
	  int32_t  sa   = 0, sb   = 0;

	  // For upper part of triangle, find scanline crossings for segments
	  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	  // is included here (and second loop will be skipped, avoiding a /0
	  // error there), otherwise scanline y1 is skipped here and handled
	  // in the second loop...which also avoids a /0 error here if y0=y1
	  // (flat-topped triangle).
	  if(y1 == y2) last = y1;   // Include y1 scanline
	  else         last = y1 - 1; // Skip it

	  for(y = y0; y <= last; y++) {
	    a   = x0 + sa / dy01;
	    b   = x0 + sb / dy02;
	    sa += dx01;
	    sb += dx02;
	    /* longhand:
	    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
	    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
	    */
	    if(a > b) ssd1306_swap(a, b);
	    SSD1306_DrawFastHLine(a, y, b - a + 1, color);
	  }

	  // For lower part of triangle, find scanline crossings for segments
	  // 0-2 and 1-2.  This loop is skipped if y1=y2.
	  sa = dx12 * (y - y1);
	  sb = dx02 * (y - y0);
	  for(; y <= y2; y++) {
	    a   = x1 + sa / dy12;
	    b   = x0 + sb / dy02;
	    sa += dx12;
	    sb += dx02;
	    /* longhand:
	    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
	    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
	    */
	    if(a > b) ssd1306_swap(a, b);
	    SSD1306_DrawFastHLine(a, y, b - a + 1, color);
	  }
}
void SSD1306_DrawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t size)
{
	  SSD1306_GotoXY(x, y);
	  SSD1306_TextSize(size);
	  SSD1306_Print(c);
}
void SSD1306_DrawText(uint8_t x, uint8_t y, char *_text, uint8_t size)
{
	  SSD1306_GotoXY(x, y);
	  SSD1306_TextSize(size);
	  while(*_text != '\0')
	    SSD1306_Print(*_text++);
}
void SSD1306_TextSize(uint8_t t_size)
{
	  if(t_size < 1)
	    t_size = 1;
	  text_size = t_size;
}
void SSD1306_GotoXY(uint8_t x, uint8_t y)
{
	  if((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
	    return;
	  x_pos = x;
	  y_pos = y;
}
void SSD1306_Print(uint8_t c)
{
	  int _color;
	  uint8_t i, j, line;

	  if (c == ' ' && x_pos == 0 && wrap)
	    return;
	  if(c == '\a') {
	    x_pos = y_pos = 0;
	    return;
	  }
	  if( (c == '\b') && (x_pos >= text_size * 6) ) {
	    x_pos -= text_size * 6;
	    return;
	  }
	  if(c == '\r') {
	    x_pos = 0;
	    return;
	  }
	  if(c == '\n') {
	    y_pos += text_size * 8;
	    if((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
	      y_pos = 0;
	    return;
	  }

	  if((c < ' ') || (c > '~'))
	    c = '?';

	  for(i = 0; i < 5; i++ ) {
	    if(c < 'S')
	      line = Font[(c - ' ') * 5 + i];
	    else
	      line = Font2[(c - 'S') * 5 + i];

	    for(j = 0; j < 7; j++, line >>= 1) {
	      if(line & 0x01)
	        _color = true;
	      else
	        _color = false;
	      if(text_size == 1) SSD1306_DrawPixel(x_pos + i, y_pos + j, _color);
	      else               SSD1306_FillRect(x_pos + (i * text_size), y_pos + (j * text_size), text_size, text_size, _color);
	    }
	  }

	  SSD1306_FillRect(x_pos + (5 * text_size), y_pos, text_size, 7 * text_size, false);

	  x_pos += text_size * 6;

	  if( x_pos > (SSD1306_LCDWIDTH + text_size * 6) )
	    x_pos = SSD1306_LCDWIDTH;

	  if (wrap && (x_pos + (text_size * 5)) > SSD1306_LCDWIDTH)
	  {
	    x_pos = 0;
	    y_pos += text_size * 8;
	    if((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
	      y_pos = 0;
	  }
}
void SSD1306_PutCustomC(uint8_t *c)
{
	 int _color;
	  uint8_t i, j, line;

	  for(i = 0; i < 5; i++ ) {
	    line = c[i];

	    for(j = 0; j < 7; j++, line >>= 1) {
	      if(line & 0x01)
	        _color = true;
	      else
	        _color = false;
	      if(text_size == 1) SSD1306_DrawPixel(x_pos + i, y_pos + j, _color);
	      else               SSD1306_FillRect(x_pos + (i * text_size), y_pos + (j * text_size), text_size, text_size, _color);
	    }
	  }

	  SSD1306_FillRect(x_pos + (5 * text_size), y_pos, text_size, 7 * text_size, false);

	  x_pos += (text_size * 6);

	  if( x_pos > (SSD1306_LCDWIDTH + text_size * 6) )
	    x_pos = SSD1306_LCDWIDTH;

	  if (wrap && (x_pos + (text_size * 5)) > SSD1306_LCDWIDTH)
	  {
	    x_pos = 0;
	    y_pos += text_size * 8;
	    if((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
	      y_pos = 0;
	  }
}
void SSD1306_SetTextWrap(bool w)
{
	  wrap = w;
}
void SSD1306_InvertDisplay(bool i)
{
	  if (i)
	    ssd1306_command(SSD1306_INVERTDISPLAY_);
	  else
	    ssd1306_command(SSD1306_NORMALDISPLAY);
}
void SSD1306_DrawBMP(uint8_t x, uint8_t y, uint8_t *bitmap, uint8_t w,
		uint8_t h)
{
}
