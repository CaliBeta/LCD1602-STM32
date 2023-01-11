/*
 * lcd.c
 *
 *  Created on: 11/09/2022
 *      Author: CaliBeta
 *      Modified: 04/01/2023
 *      Version: 1.1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void lcd_setRowOffsets(LCD_t *lcd, int row0, int row1, int row2,
    int row3);
static void lcd_send(LCD_t *lcd, char data, uint8_t rs);
static void lcd_sendData(LCD_t *lcd, char data);
static void lcd_sendCmd(LCD_t *lcd, char cmd);
static void delay_us(uint16_t time);

/* Exported functions --------------------------------------------------------*/
//init lcd display
void lcd_begin(LCD_t *lcd, uint8_t cols, uint8_t rows, uint8_t dotsize) {
	// 4 bit initialization
	lcd->displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	if (rows > 1) lcd->displayfunction |= LCD_2LINE;
	lcd->rows = rows;
	lcd->cols = cols;
	lcd_setRowOffsets(lcd, 0x00, 0x40, 0x00 + cols, 0x40 + cols);

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != LCD_5x8DOTS) && (rows == 1)) {
		lcd->displayfunction |= LCD_5x10DOTS;
	}

	HAL_Delay(50);	// wait for >40ms
	lcd_sendCmd(lcd, 0x03);
	HAL_Delay(5);		// wait for >4.1ms
	lcd_sendCmd(lcd, 0x03);
	delay_us(150);	// wait for >100us
	lcd_sendCmd(lcd, 0x03);
	lcd_sendCmd(lcd, 0x02);	//set 4 bit mode
	HAL_Delay(1);

	// finally, set interface type, # lines and font size
	lcd_sendCmd(lcd, LCD_FUNCTIONSET | lcd->displayfunction);
	//set display control D=1,C=0, B=0, display on, cursor off and cursor blink off
	lcd->displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_display(lcd);
	lcd_clear(lcd);  // clear display
	HAL_Delay(2);
	// set the entry mode --> I/D = 1 (increment cursor) & S = 0 (no shift)
	lcd->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	lcd_sendCmd(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}
// ----------------------------------------------------------------

//clear lcd display
void lcd_clear(LCD_t *lcd) {
	lcd_sendCmd(lcd, LCD_CLEARDISPLAY);
}
// ----------------------------------------------------------------

// set cursor position to zero
void lcd_home(LCD_t *lcd) {
	lcd_sendCmd(lcd, LCD_RETURNHOME);
	HAL_Delay(2);	//delay >= 1.52ms
}
// ----------------------------------------------------------------

// Turn the display off
void lcd_noDisplay(LCD_t *lcd) {
	lcd->displaycontrol &= ~LCD_DISPLAYON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
// ----------------------------------------------------------------

// Turn the display on
void lcd_display(LCD_t *lcd) {
	lcd->displaycontrol |= LCD_DISPLAYON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
// ----------------------------------------------------------------

// Turn off the blinking cursor
void lcd_noBlink(LCD_t *lcd) {
	lcd->displaycontrol &= ~LCD_BLINKON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
// ----------------------------------------------------------------

// Turn on the blinking cursor
void lcd_blink(LCD_t *lcd) {
	lcd->displaycontrol |= LCD_BLINKON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
// ----------------------------------------------------------------

// Turns the underline cursor off
void lcd_noCursor(LCD_t *lcd) {
	lcd->displaycontrol &= ~LCD_CURSORON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
// ----------------------------------------------------------------

// Turns the underline cursor on
void lcd_cursor(LCD_t *lcd) {
	lcd->displaycontrol |= LCD_CURSORON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
// ----------------------------------------------------------------

//scroll the display to left without changing the RAM
void lcd_scrollDisplayLeft(LCD_t *lcd) {
	lcd_sendCmd(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
// ----------------------------------------------------------------

//scroll the display to right without changing the RAM
void lcd_scrollDisplayRight(LCD_t *lcd) {
	lcd_sendCmd(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}
// ----------------------------------------------------------------

//set cursor position
void lcd_setCursor(LCD_t *lcd, uint8_t col, uint8_t row) {
	const size_t max_rows = sizeof(lcd->row_offsets) / sizeof(*lcd->row_offsets);
	if (row >= max_rows) {
		row = max_rows - 1;    // we count rows starting w/0
	}
	if (row >= lcd->rows) {
		row = lcd->rows - 1;    // we count rows starting w/0
	}

	lcd_sendCmd(lcd, LCD_SETDDRAMADDR | (col + lcd->row_offsets[row]));
}
// ----------------------------------------------------------------

//send strings to lcd
void lcd_print(LCD_t *lcd, char *str) {
	while (*str)
		lcd_sendData(lcd, *str++);
}
// ----------------------------------------------------------------

//send raw data to lcd
void lcd_write(LCD_t *lcd, uint8_t data) {
	lcd_sendData(lcd, data);
}
// ----------------------------------------------------------------

//create custom char to CGRAM
void lcd_createChar(LCD_t *lcd, char *data, uint8_t pos) {
	if (pos > 8) pos = 8;

	uint8_t cmd = LCD_SETCGRAMADDR + (pos * 8);
	lcd_sendCmd(lcd, cmd);
	for (int i = 0; i < 8; i++)
		lcd_sendData(lcd, data[i]);
}
// ----------------------------------------------------------------

/* Privated functions --------------------------------------------------------*/
//set row offset for diferent display
static void lcd_setRowOffsets(LCD_t *lcd, int row0, int row1, int row2,
    int row3) {
	lcd->row_offsets[0] = row0;
	lcd->row_offsets[1] = row1;
	lcd->row_offsets[2] = row2;
	lcd->row_offsets[3] = row3;
}
// ----------------------------------------------------------------

//send info to lcd in 4 byte mode
static void lcd_send(LCD_t *lcd, char data, uint8_t rs) {
	// rs = 1 for data, rs=0 for command
	HAL_GPIO_WritePin(lcd->RS_port, lcd->RS_pin, rs);

	// write the data to the respective pin
	HAL_GPIO_WritePin(lcd->D7_port, lcd->D7_pin, ((data >> 3) & 0x01));
	HAL_GPIO_WritePin(lcd->D6_port, lcd->D6_pin, ((data >> 2) & 0x01));
	HAL_GPIO_WritePin(lcd->D5_port, lcd->D5_pin, ((data >> 1) & 0x01));
	HAL_GPIO_WritePin(lcd->D4_port, lcd->D4_pin, ((data >> 0) & 0x01));

	HAL_GPIO_WritePin(lcd->EN_port, lcd->EN_pin, GPIO_PIN_SET);
	delay_us(1);	// enable pulse must be > 450ns
	HAL_GPIO_WritePin(lcd->EN_port, lcd->EN_pin, GPIO_PIN_RESET);
	delay_us(50);	// commands need > 37us to settle
}
// ----------------------------------------------------------------

//send data to lcd
static void lcd_sendData(LCD_t *lcd, char data) {
	char _data;

	// send higher nibble
	_data = ((data >> 4) & 0x0f);
	lcd_send(lcd, _data, 1);  // rs =1 for sending data

	// send Lower nibble
	_data = ((data) & 0x0f);
	lcd_send(lcd, _data, 1);
}
// ----------------------------------------------------------------

//send command to lcd
static void lcd_sendCmd(LCD_t *lcd, char cmd) {
	char _cmd;

	// send higher nibble
	_cmd = ((cmd >> 4) & 0x0f);
	lcd_send(lcd, _cmd, 0);  // rs = 0 for sending command

	// send Lower nibble
	_cmd = ((cmd) & 0x0f);
	lcd_send(lcd, _cmd, 0);
}
// ----------------------------------------------------------------

//set delay in microseconds
static void delay_us(uint16_t time) {
	uint16_t delay = time * (HAL_RCC_GetHCLKFreq() / 1000000);
	for (int i = 0; i < delay; i += 14) {
		__ASM("NOP");
	}
}
// ----------------------------------------------------------------
