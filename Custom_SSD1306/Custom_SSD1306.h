/**
	Custom_SSD1306.h
	
	This library does not use the display's scrolling feature.
	
	Author: Brett Warren

*/

#include <Arduino.h>
#include <Wire.h>

enum DISPLAY_COLOR {
	BLACK,
	WHITE,
	INVERT,
	NOOP
};

class Custom_SSD1306 {
	private:
		uint8_t _address;
		uint8_t _frameBuffer[1024];
		
		const uint8_t _frameHeight = 64;
		const uint8_t _frameWidth = 128;
	
	public:
		Custom_SSD1306(uint8_t addr = 0x3C);
		bool begin();
		void drawPixel(unsigned int x, unsigned int y, DISPLAY_COLOR color);
		
		void drawRectangle(unsigned int x, unsigned int y, int width, int height, DISPLAY_COLOR color);
		
		void drawCharacter(int x, int y, uint8_t* data, int fontSize, DISPLAY_COLOR color);
		void drawString(int x, int y, char* text, uint8_t* fontData, int fontSize, DISPLAY_COLOR color);
		void drawInteger(int x, int y, long number, uint8_t* fontData, int fontSize, DISPLAY_COLOR color);
		
		void clearDisplay();
		void updateDisplay();
		
		bool getPixel(unsigned int x, unsigned int y);
		
		// TODO: implement all commented-out functions.
};