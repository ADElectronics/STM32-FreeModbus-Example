#include "ST7735.h"
#include "font7x15.h"
#include "main.h"

static SPI_HandleTypeDef *hspi;

void ST7735_SendCmd(uint8_t cmd);
void ST7735_SendData(uint8_t data);
void ST7735_at(uint8_t startX, uint8_t startY, uint8_t stopX, uint8_t stopY);

void ST7735_SendData(uint8_t data)
{
	ST7735_DC_HIGH;
	HAL_SPI_Transmit(hspi, &data, 1, 100);
}

void ST7735_SendCmd(uint8_t cmd)
{
	ST7735_DC_LOW;
	HAL_SPI_Transmit(hspi, &cmd, 1, 100);
}

void ST7735_at(uint8_t startX, uint8_t startY, uint8_t stopX, uint8_t stopY)
{
	ST7735_SendCmd(0x2A);
	ST7735_SendData(0x00);
	ST7735_SendData(startX);
	ST7735_SendData(0x00);
	ST7735_SendData(stopX);

	ST7735_SendCmd(0x2B);
	ST7735_SendData(0x00);
	ST7735_SendData(startY);
	ST7735_SendData(0x00);
	ST7735_SendData(stopY);
}

void ST7735_Clear(void)
{
	ST7735_FillRect(0, 0, ST7735_HEIGHT-1, ST7735_WIDTH-1, 0);
}

void ST7735_Init(SPI_HandleTypeDef *hspi_ex)
{
	hspi = hspi_ex;
	ST7735_CS_LOW;

#if LCD_SOFT_RESET > 0
	ST7735_SendCmd(0x01);
	HAL_Delay(ST7735_SOFT_RST_DELAY);
#else
	ST7735_RST_LOW;
	HAL_Delay(0);
	ST7735_RST_HIGH;
	HAL_Delay(ST7735_HRDW_RST_DELAY);
#endif

	ST7735_SendCmd(0x11);
	HAL_Delay(ST7735_HRDW_RST_DELAY);

	ST7735_SendCmd(0xb1);
	ST7735_SendData(0x05);
	ST7735_SendData(0x3c);
	ST7735_SendData(0x3c);

	ST7735_SendCmd (0x3A); // Режим цвета:
	//ST7735_SendData(0x03); // RGB 4-4-4
	ST7735_SendData(0x05); // RGB 5-6-5
	//ST7735_SendData(0x06); // RGB 6-6-6

	ST7735_SendCmd(0x20); // Инверсия выкл
	//ST7735_SendCmd(0x21); // вкл

	//ST7735_SendCmd(0x13);

	ST7735_SendCmd(0x36);
#if ST7735_RGB_MODE > 0
	ST7735_SendData(0x08); 
#else
	ST7735_SendData(0x00); 
#endif
	ST7735_SendCmd(0x26); // Гамма
	ST7735_SendData(0x01); // (G2.2) или (G1.0)
	//ST7735_SendData(0x02); // (G1.8) или (G2.5)
	//ST7735_SendData(0x04); // (G2.5) или (G2.2)
	//ST7735_SendData(0x08); // (G1.0) или (G1.8)
	ST7735_CS_HIGH;

	// Делаем красиво, а не как у ардуинщиков
	ST7735_Clear();

	ST7735_CS_LOW;
	ST7735_SendCmd (0x29);
	ST7735_CS_HIGH;
}

// процедура заполнения прямоугольной области экрана заданным цветом
void ST7735_FillRect(uint8_t startX, uint8_t startY, uint8_t stopX, uint8_t stopY, uint16_t color)
{
	uint8_t x, y, data;

	ST7735_CS_LOW;

	ST7735_at(startX, startY, stopX, stopY);
	ST7735_SendCmd(0x2C);

	ST7735_DC_HIGH;

	for (y=startY;y<stopY+1;y++)
	{		
		for (x=startX;x<stopX+1;x++) 
		{
			data = color >> 8;
			HAL_SPI_Transmit(hspi, &data, 1, 100);
			data = color & 0xFF;
			HAL_SPI_Transmit(hspi, &data, 1, 100);
		}
	}
	
	ST7735_CS_HIGH;
}

void ST7735_PutPix(uint8_t x, uint8_t y, uint16_t Color) 
{
	ST7735_CS_LOW;
	
	ST7735_at(x, y, x, y);
	ST7735_SendCmd(0x2C);
	ST7735_SendData((uint8_t)((Color & 0xFF00)>>8));
	ST7735_SendData((uint8_t) (Color & 0x00FF));
	
  ST7735_CS_HIGH;
}

void ST7735_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color) 
{
	int8_t dx, dy, sx, sy;
	uint8_t x, y, mdx, mdy, l;

	if (x1==x2) 
	{ // быстрая отрисовка вертикальной линии
		ST7735_FillRect(x1,y1, x1,y2, color);
		return;
	}

	if (y1==y2) 
	{ // быстрая отрисовка горизонтальной линии
		ST7735_FillRect(x1,y1, x2,y1, color);
		return;
	}

	dx=x2-x1; dy=y2-y1;

	if (dx>=0) 
	{ 
		mdx=dx; 
		sx=1; 
	} 
	else 
	{ 
		mdx=x1-x2; 
		sx=-1; 
	}
	
	if (dy>=0) 
	{ 
		mdy=dy; 
		sy=1; 
	} 
	else 
	{ 
		mdy=y1-y2; 
		sy=-1; 
	}

	x=x1; y=y1;

	if (mdx>=mdy) 
	{
		l=mdx;
		while (l>0) 
		{
			if (dy>0) 
			{ 
				y=y1+mdy*(x-x1)/mdx; 
			}
			else 
			{ 
				y=y1-mdy*(x-x1)/mdx; 
			}
			
			ST7735_PutPix(x,y,color);
			x = x + sx;
			l--;
		}
	} 
	else 
	{
		l = mdy;
		while (l>0) 
		{
			if (dy>0) 
			{
				x=x1+((mdx*(y-y1))/mdy); 
			}
			else
			{ 
				x=x1+((mdx*(y1-y))/mdy);
			}
			
			ST7735_PutPix(x,y,color);
			y = y + sy;
			l--;
		}
	}
	ST7735_PutPix(x2, y2, color);
}

// рисование прямоугольника (не заполненного)
void ST7735_rect(int8_t x1,int8_t y1,int8_t x2,int8_t y2, uint16_t color)
{
	ST7735_FillRect(x1,y1, x2,y1, color);
	ST7735_FillRect(x1,y2, x2,y2, color);
	ST7735_FillRect(x1,y1, x1,y2, color);
	ST7735_FillRect(x2,y1, x2,y2, color);
}

// печать десятичного числа
void ST7735_PutDec(int16_t numb, uint8_t dcount, uint8_t x, uint8_t y,uint16_t fntColor, uint16_t bkgColor)
{
	uint16_t divid = 10000;
	uint8_t i, res;
	
	if(numb < 0)
	{
		numb = -numb;
		ST7735_PutChar(x, y, '-', fntColor, bkgColor);
		y = y + 6;
	}
	else
	{
		ST7735_PutChar(x, y, '+', fntColor, bkgColor);
		y = y + 6;
	}
	
	for (i=5; i!=0; i--)
	{
		res = numb / divid;

		if (i<=dcount)
		{
			ST7735_PutChar(x, y, res+'0', fntColor, bkgColor);
			y=y+6;
		}

		numb %= divid;
		divid /= 10;
	}
}

void ST7735_PutChar(uint8_t x, uint8_t y, uint8_t chr, uint16_t charColor, uint16_t bkgColor)
{
	uint8_t i, j, k, data, chl, h;
	uint16_t color;
	
	ST7735_CS_LOW;

	ST7735_at(x, y, x+12, y+8);
	ST7735_SendCmd(0x2C);

	ST7735_DC_HIGH;
	
	for (i=0;i<7;i++)
	{
		for (k=2;k>0;k--)
		{
			chl = font_7x15_ru[ ( (chr-0x20)*14 + i+ 7*(k-1)) ];
			chl = chl<<2*(k-1); // нижнюю половину символа сдвигаем на 1 позицию влево (убираем одну линию снизу)
			
			if (k==2) 
				h=6; 
			else 
				h=7; // у нижней половины выведем только 6 точек вместо 7
			for (j=0;j<h;j++)
			{
				if (chl & 0x80) 
					color = charColor; 
				else 
					color = bkgColor;
				chl = chl<<1;

				data = color >> 8;
				HAL_SPI_Transmit(hspi, &data, 1, 100);
				data = color & 0xFF;
				HAL_SPI_Transmit(hspi, &data, 1, 100);
			}
		}
	}
	// рисуем справо от символа пустую вертикальную линию для бокового интервала
	for (j=0;j<13;j++)
	{
		data = bkgColor >> 8;
		HAL_SPI_Transmit(hspi, &data, 1, 100);
		data = bkgColor & 0xFF;
		HAL_SPI_Transmit(hspi, &data, 1, 100);
	}

	ST7735_CS_HIGH;
}

void ST7735_PutStr(uint8_t x, uint8_t y, const uint8_t str[], uint16_t int8_tColor, uint16_t bkgColor)
{
	while (*str!=0)
	{
		ST7735_PutChar(x, y, *str, int8_tColor, bkgColor);
		y=y+8;
		str++;
	}
}

void ST7735_PutImg(uint8_t x, uint8_t y, uint16_t w, uint16_t h, const uint16_t *img)
{
	while(x < h)
	{
		while(y < w)
		{
			ST7735_PutPix(x, y, *img);
			img++;
			y++;
		}
		y = 0;
		x++;
	}
}
