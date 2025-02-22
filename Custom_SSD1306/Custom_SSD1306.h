/**
	Custom_SSD1306.h
	
	This library does not use the display's scrolling feature.
	
	Author: Brett Warren

*/

#include <Arduino.h>
#include <Wire.h>

/**
	An enumeration used to determine what to do to a given pixel.
*/
enum DISPLAY_COLOR {
	/// For turning a pixel dark.
	BLACK,
	/// For setting a pixel to light up.
	WHITE,
	/// For setting a pixel to the opposite of how it started.
	INVERT,
	/// For ignoring a certain pixel.
	NOOP
};

/**
	A class for interfacing with a 128x64 OLED display with the SSD1306 MCU as its driver.
*/
class Custom_SSD1306 {
	private:
		uint8_t _address;
		uint8_t _frameBuffer[1024];
		
		const uint8_t _frameHeight = 64;
		const uint8_t _frameWidth = 128;
		
		int _fontSize = 1;
		int _cursorX = 0;
		int _cursorY = 0;
		DISPLAY_COLOR _printColor = WHITE;
		
		uint8_t* _font;
	
	public:
		/**
			Creates the display's object, specifying the I2C address and allocating a 1kB frame buffer as well as specifying the font data.
			
			@param addr The address to use when communicating with the display.
		*/
		Custom_SSD1306(uint8_t addr = 0x3C, uint8_t* font = 0);
		
		/**
			Prepares the display for use by setting the appropriate registers.
			
			@return True if the communication was successful.
		*/
		bool begin();
		
		/**
			Modifies the frame buffer by setting/clearing a given bit.
			
			@param x The horizontal position of the pixel to modify.
			@param y The vertical position of the pixel to modify.
			@param color The color to write to the pixel.
			
			@note If `color` is `NOOP`, the pixel is ignored.
		*/
		void drawPixel(unsigned int x, unsigned int y, DISPLAY_COLOR color);
		
		/**
			Draws a rectangle of the specified color.
			
			@param x The horizontal position of the upper left-hand corner of the rectangle.
			@param y The vertical position of the upper left-hand corner of the rectangle.
			@param width The width of the rectangle measured in pixels.
			@param height The height of the rectangle measured in pixels.
			@param color The color with which to draw the rectangle.
		*/
		void drawRectangle(unsigned int x, unsigned int y, int width, int height, DISPLAY_COLOR color);
		
		/**
			Draws a character at the given position with the given font size.
			
			@param x The horizontal position of the upper left-hand corner of the character.
			@param y The vertical position of the upper left-hand corner of the character.
			@param data The data defining the shape of the character.
			@param fontSize A number serving as the multiplier for the size of the character.
			@param color The color with which to draw the character.
		*/
		void drawCharacter(int x, int y, uint8_t* data, int fontSize, DISPLAY_COLOR color);
		
		/**
			Draws a string of the given font size at the given position using the given font data.
			
			@param x The horizontal position of the upper left-hand corner of the string.
			@param y The vertical position of the upper left-hand corner of the string.
			@param text The text to draw onto the screen (must be explicitly null-terminated).
			@param fontData The two-dimensional array to use for the font.
			@param fontSize A number serving as the multiplier for the size of the characters in the string.
			@param color The color with which to draw the string.
			
			@note If the input string is not explicitly null-terminated, unexpected results will likely occur.
		*/
		void drawString(int x, int y, char* text, uint8_t* fontData, int fontSize, DISPLAY_COLOR color);
		
		/**
			Draws a decimal number at the specified position with the specified font size.
			
			@param x The horizontal position of the upper left-hand corner of the number to draw.
			@param y The vertical position of the upper left-hand corner of the number to draw.
			@param number The number to print (will be represented in decimal).
			@param fontData The two-dimensional array to use for the font.
			@param fontSize A number serving as the multiplier for the size of the characters in the number.
			@param color The color with which to draw the number.
		*/
		void drawInteger(int x, int y, long number, uint8_t* fontData, int fontSize, DISPLAY_COLOR color);
		
		/**
			Sets the cursor to the specified location on the display.
			
			@param x The horizontal position at which to place the cursor.
			@param y The vertical position at which to place the cursor.
		*/
		void setCursor(int x, int y);
		
		/// Specifies the font size to use for print statements.
		void setFontSize(int size);
		
		/// Specifies the color to use for print statements.
		void setColor(DISPLAY_COLOR color);
		
		/// Prints the given string at the cursor's position and with the set color and font size. Does nothing if no font is set.
		void print(char* text);
		
		/// Prints the given character at the cursor's position and with the set color and font size. Does nothing if no font is set.
		void print(char c);
		
		/// Prints the given number at the cursor's position and with the set color and font size. Does nothing if no font is set. Prints in decimal.
		void print(long number);
		
		/// Clears all bits in the frame buffer.
		void clearDisplay();
		
		/// Transfers all data in the frame buffer to the display's graphical RAM.
		void updateDisplay();
		
		/**
			Checks the frame buffer at the given position and returns whether that bit is set.
			
			@param x The horizontal position of the pixel to check.
			@param y The vertical position of the pixel to check.
			
			@return True if the specified bit is set; false otherwise.
		*/
		bool getPixel(unsigned int x, unsigned int y);
};