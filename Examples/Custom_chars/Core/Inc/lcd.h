/*
 * lcd.h
 *
 *  Created on: 11/09/2022
 *      Author: CaliBeta
 *      Modified: 04/01/2023
 *      Version: 1.1.0
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_LCD_H_
#define INC_LCD_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
	//Hardware settings:
	GPIO_TypeDef *RS_port;
	uint16_t RS_pin;
	GPIO_TypeDef *RW_port;
	uint16_t RW_pin;
	GPIO_TypeDef *EN_port;
	uint16_t EN_pin;
	GPIO_TypeDef *D4_port;
	uint16_t D4_pin;
	GPIO_TypeDef *D5_port;
	uint16_t D5_pin;
	GPIO_TypeDef *D6_port;
	uint16_t D6_pin;
	GPIO_TypeDef *D7_port;
	uint16_t D7_pin;

	//Graphics settings
	uint8_t displaycontrol; 	//for display control command
	uint8_t displayfunction;	//for display funtion command
	uint8_t displaymode;			//for display mode command
	uint8_t row_offsets[4];		//offset value for rows in RAM
	uint8_t cols;			//num of colums
	uint8_t rows;			//num of rows
} LCD_t;

/* Exported constants --------------------------------------------------------*/
// commands
#define LCD_CLEARDISPLAY 		0x01
#define LCD_RETURNHOME 			0x02
#define LCD_ENTRYMODESET 		0x04
#define LCD_DISPLAYCONTROL	0x08
#define LCD_CURSORSHIFT 		0x10
#define LCD_FUNCTIONSET 		0x20
#define LCD_SETCGRAMADDR 		0x40
#define LCD_SETDDRAMADDR 		0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 					0x00
#define LCD_ENTRYLEFT 					0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 		0x04
#define LCD_DISPLAYOFF 		0x00
#define LCD_CURSORON 			0x02
#define LCD_CURSOROFF			0x00
#define LCD_BLINKON 			0x01
#define LCD_BLINKOFF 			0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 	0x08
#define LCD_CURSORMOVE 		0x00
#define LCD_MOVERIGHT 		0x04
#define LCD_MOVELEFT 			0x00

// flags for function set
#define LCD_8BITMODE 			0x10
#define LCD_4BITMODE 			0x00
#define LCD_2LINE 				0x08
#define LCD_1LINE 				0x00
#define LCD_5x10DOTS 			0x04
#define LCD_5x8DOTS 			0x00

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables------------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void lcd_begin(LCD_t *lcd, uint8_t cols, uint8_t rows, uint8_t dotsize);
void lcd_clear(LCD_t *lcd);
void lcd_home(LCD_t *lcd);
void lcd_noDisplay(LCD_t *lcd);
void lcd_display(LCD_t *lcd);
void lcd_noBlink(LCD_t *lcd);
void lcd_blink(LCD_t *lcd);
void lcd_noCursor(LCD_t *lcd);
void lcd_cursor(LCD_t *lcd);
void lcd_scrollDisplayLeft(LCD_t *lcd);
void lcd_scrollDisplayRight(LCD_t *lcd);
void lcd_setCursor(LCD_t *lcd, uint8_t col, uint8_t row);
void lcd_print(LCD_t *lcd, char *str);
void lcd_write(LCD_t *lcd, uint8_t data);
void lcd_createChar(LCD_t *lcd, char *data, uint8_t pos);

#endif /* INC_LCD_H_ */
