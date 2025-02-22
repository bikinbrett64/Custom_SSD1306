#include <Arduino.h>
#include <Wire.h>
#include "Custom_SSD1306.h"

Custom_SSD1306::Custom_SSD1306(uint8_t addr, uint8_t* font) {
	memset(_frameBuffer, 0, 1024);
	_address = addr;
	_font = font;
}

bool Custom_SSD1306::begin() {
	//delay(100);
	Wire.beginTransmission(_address);  // Initializing default values according to the datasheet.
	
	
	Wire.write(0x80);  // Set MUX Ratio
	Wire.write(0xA8);
	Wire.write(0x3F);
	
	
	Wire.write(0x80);  // Set Display Offset
	Wire.write(0xD3);
	Wire.write(0x7F);  // I don't understand why I have to set the offset to 63 to avoid an actual offset. Two's complement, mayhaps?
	
	Wire.write(0x80);  // Set Display Start Line
	Wire.write(0x40);
	
	Wire.write(0x80);  // Set Segment Re-map
	Wire.write(0xA1);
	
	Wire.write(0x80);  // Set COM output scan direction
	Wire.write(0xC8);
	
	Wire.write(0x80);  // Set COM pins hardware configuration (the datasheet appears to have a typo regarding this on page 64; I found this out the hard way)
	Wire.write(0xDA);
	Wire.write(0x10);
	
	/*
	Wire.write(0x80);  // Set Contrast Control
	Wire.write(0x81);
	Wire.write(0x70);
	*/
	
	// Splitting the batch of I2C transmissions to accommodate a 32-byte buffer. (causes problems?)
	/*
	Wire.endTransmission(false);
	Wire.beginTransmission(_address);
	*/
	
	Wire.write(0x80);  // Disable entire display ON
	Wire.write(0xA4);
	
	
	Wire.write(0x80);  // Set Normal Display
	Wire.write(0xA6);
	
	/*
	Wire.write(0x80);  // Set Osc Frequency
	Wire.write(0xD5);
	Wire.write(0x80);
	*/
	
	Wire.write(0x80);  // Enable charge pump regulator (Very important!)
	Wire.write(0x8D);
	Wire.write(0x14);
	
	
	Wire.write(0x80);  // Set Memory Addressing Mode
	Wire.write(0x20);
	Wire.write(0x00);
	
	/*
	Wire.write(0x80);  // Set Column Address
	Wire.write(0x21);
	Wire.write(0x00);
	Wire.write(0x7F);
	
	Wire.write(0x80);  // Set Page Address
	Wire.write(0x22);
	Wire.write(0x00);
	Wire.write(0x07);
	*/
	
	Wire.write(0x80);  // Display On
	Wire.write(0xAF);
	if (Wire.endTransmission() != 0) {
		return false;
	}
	
	return true;
}

void Custom_SSD1306::drawPixel(unsigned int x, unsigned int y, DISPLAY_COLOR color) {
	if (x >= _frameWidth || y >= _frameHeight) return;
	
	// The display paints upward in 8-bit strokes and proceeds from left to right.
	// Then it goes on to the next "page." That is, if the memory addressing mode happens to be the Horizontal one.
	// For this reason, I have to do some wonky stuff here:
	int pageNumber = y / 8;  // The page the pixel lands in.
	int pagePosition = y % 8;  // The vertical position of the pixel within the page.
	int index = pageNumber * 128 + x;  // The frame buffer address to access.
	
	int value = 0x01 << pagePosition;
	
	switch (color) {
		case BLACK:
			_frameBuffer[index] &= ~value;
			break;
		case WHITE:
			_frameBuffer[index] |= value;
			break;
		case INVERT:
			_frameBuffer[index] ^= value;
			break;
		default:
			break;  // Do nothing in this case since we must be in NOOP territory (just thought I'd make that explicit).
	}
}

void Custom_SSD1306::drawRectangle(unsigned int x, unsigned int y, int width, int height, DISPLAY_COLOR color) {
	for (int i = y; i < y + height; i++) {
		for (int j = x; j < x + width; j++) {
			drawPixel(j, i, color);
		}
	}
}

void Custom_SSD1306::drawCharacter(int x, int y, uint8_t* data, int fontSize, DISPLAY_COLOR color) {
	int horizontalOffset = 2;
	int verticalOffset = 1;
	for (int i = verticalOffset; i < 8; i++) {
		for (int j = horizontalOffset; j < 8; j++) {
			bool bit = (data[i] >> (7 - j)) & 0x01;  // Going through the character one bit at a time.
			int tempX = x + (fontSize * (j - horizontalOffset));  // Applying offset to the position of the character on the screen.
			int tempY = y + (fontSize * (i - verticalOffset));
			DISPLAY_COLOR tempColor;
			switch (color) {
				case BLACK:
					tempColor = bit ? BLACK : NOOP;
					break;
				case WHITE:
					tempColor = bit ? WHITE : NOOP;
					break;
				case INVERT:
					tempColor = INVERT;
					break;
				default:
					break;  // Do nothing in this case since we must be in NOOP territory (just thought I'd make that explicit).
			}
			drawRectangle(tempX, tempY, fontSize, fontSize, tempColor);
		}
	}
}

void Custom_SSD1306::drawString(int x, int y, char* text, uint8_t* fontData, int fontSize, DISPLAY_COLOR color) {
	int index = 0;
	int currentOffset = 0;
	while (text[index] != '\0') {
		uint8_t letterIndex = uint8_t(text[index]);
		drawCharacter(x + currentOffset, y, fontData + (8 * letterIndex), fontSize, color);
		currentOffset += 6 * fontSize;
		index++;
	}
}

void Custom_SSD1306::drawInteger(int x, int y, long number, uint8_t* fontData, int fontSize, DISPLAY_COLOR color) {
	long temp = number;
	int index = 0;
	char result[11];  // 1 for the sign, 9 for the digits, and 1 for the null terminator.
	memset(result, 0, 11);  // This line is to guarantee that the null terminator is present.
	if (temp < 0) {
		result[index++] = '-';
		temp = -temp;
	}
	else if (temp == 0) {
		result[index] = '0';
	}
	while (temp > 0) {  // In this loop, we write the number as a string, starting with the ones' place.
		int currentDigit = temp % 10;
		int i = index;
		while (result[i] != '\0') {  // This loop is for finding the null terminator.
			i++;
		}
		for (int j = i; j > index; j--) {  // Now, we need to shift all digits backward one space.
			result[j] = result[j - 1];
		}
		result[index] = currentDigit + '0';  // Draw the current digit into the array.
		temp /= 10;  // Go on to the next digit up.
	}
	drawString(x, y, result, fontData, fontSize, color);  // Finally, we can print the number to the display buffer.
}

void Custom_SSD1306::setCursor(int x, int y) {
	_cursorX = x;
	_cursorY = y;
}

void Custom_SSD1306::setFontSize(int size) {
	_fontSize = size;
}

void Custom_SSD1306::setColor(DISPLAY_COLOR color) {
	_printColor = color;
}

void Custom_SSD1306::print(char* text) {
	if (_font != 0) {
		drawString(_cursorX, _cursorY, text, _font, _fontSize, _printColor);
	}
}

void Custom_SSD1306::print(char c) {
	if (_font != 0) {
		drawCharacter(_cursorX, _cursorY, &_font[(uint8_t)c], _fontSize, _printColor);
	}
}

void Custom_SSD1306::print(long number) {
	if (_font != 0) {
		drawInteger(_cursorX, _cursorY, number, _font, _fontSize, _printColor);
	}
}

void Custom_SSD1306::clearDisplay() {
	memset(_frameBuffer, 0, 1024);
}

void Custom_SSD1306::updateDisplay() {
	int temp = 0;
	
	// We need to ensure that the display begins updating in the correct location.
	// (I had to comment out this block of code because it made the display unusable)
	/*
	Wire.beginTransmission(_address);
	Wire.write(0x80);  // Set Column Address
	Wire.write(0x21);
	Wire.write(0x00);
	Wire.write(0x7F);
	
	Wire.write(0x80);  // Set Page Address
	Wire.write(0x22);
	Wire.write(0x00);
	Wire.write(0x07);
	Wire.endTransmission();
	*/
	
	for (int i = 0; i < 64; i++) {  // Iterating through the pages/columns.
		Wire.beginTransmission(_address);
		Wire.write(0x40);  // Declare the following to be data.
		for (int k = 0; k < 16; k++) {  // Splitting the batch of write operations to avoid exceeding the Wire library's 32-byte buffer.
			Wire.write(_frameBuffer[temp++]);
		}
		Wire.endTransmission();
	}
	
	Wire.beginTransmission(_address);
	Wire.write(0x80);
	Wire.write(0xA4);
	Wire.endTransmission();
	
}

bool Custom_SSD1306::getPixel(unsigned int x, unsigned int y) {
	if (x >= _frameWidth || y >= _frameHeight) return false;
	
	// The display paints upward in 8-bit strokes and proceeds from left to right.
	// Then it goes on to the next "page." That is, if the memory addressing mode happens to be the Horizontal one.
	// For this reason, I have to do some wonky stuff here:
	int pageNumber = y / 8;  // The page the pixel lands in.
	int pagePosition = y % 8;  // The vertical position of the pixel within the page.
	int index = pageNumber * 128 + x;  // The frame buffer address to access.
	
	uint8_t value = 0x01 << pagePosition;
	
	return (value & _frameBuffer[index]) > 0;
}