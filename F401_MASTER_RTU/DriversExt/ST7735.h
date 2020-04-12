#ifndef _LCD_ST7735_
#define _LCD_ST7735_

#include "main.h"

#define ST7735_CS_PORT	GPIOA	
#define ST7735_CS_PIN	GPIO_PIN_4

#define ST7735_DC_PORT	GPIOA
#define ST7735_DC_PIN	GPIO_PIN_3

#define ST7735_RST_PORT	GPIOA
#define ST7735_RST_PIN	GPIO_PIN_2

// Использовать ли софтовый сброс заместо хардварного ?
#define LCD_SOFT_RESET			0
// Задержки в мс
#define ST7735_SOFT_RST_DELAY	150
#define ST7735_HRDW_RST_DELAY 	150
// 0 - порядок кодирования 5B-6G-5R, 1 - 5R-6G-5B
#define ST7735_RGB_MODE			1

#define ST7735_WIDTH			160
#define ST7735_HEIGHT			120

void ST7735_Init(SPI_HandleTypeDef *hspi_ex);
void ST7735_Clear(void);
void ST7735_FillRect(uint8_t startX, uint8_t startY, uint8_t stopX, uint8_t stopY, uint16_t color);
void ST7735_PutPix(uint8_t x, uint8_t y, uint16_t Color);
void ST7735_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
void ST7735_Rect(int8_t x1,int8_t y1,int8_t x2,int8_t y2, uint16_t color);
void ST7735_PutChar(uint8_t x, uint8_t y, uint8_t chr, uint16_t int8_tColor, uint16_t bkgColor);
void ST7735_PutStr(uint8_t x, uint8_t y, const uint8_t str[], uint16_t int8_tColor, uint16_t bkgColor);
void ST7735_PutDec(int16_t num, uint8_t dcount, uint8_t x, uint8_t y, uint16_t fntColor, uint16_t bkgColor);
void ST7735_PutImg(uint8_t x, uint8_t y, uint16_t w, uint16_t h, const uint16_t *img);

#define ST7735_CS_LOW 		HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
#define ST7735_CS_HIGH 		HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);

#define ST7735_DC_LOW 		HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_RESET);
#define ST7735_DC_HIGH 		HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);

#define ST7735_RST_LOW 		HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_RESET);
#define ST7735_RST_HIGH 	HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_SET);

#endif // _LCD_ST7735_

