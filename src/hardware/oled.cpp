#include "oled.h"
#include "cstring"
#include "math.h"

// Screen data is stored in an array. Each set of 8 pixels is written one by one
// Each set of 8 is stored as an integer. The panel is written to horizontally, then vertically
// Array stores the values in width - height format with bits stored along vertical rows
uint8_t OLEDBuffer[128][8];
char outBuffer[OB_SIZE];

// The index of the current top level menu item
int topLevelMenuIndex = 0;
int lastTopLevelMenuIndex = 0;

// The current value of the cursor
int currentCursorIndex = 0;

// The current menu depth. 0 would be the motor data, 1 the main menu, and 2 any submenus
int menuDepthIndex = 0;
int lastMenuDepthIndex = 0;

// Displays the bootscreen
void showBootscreen() {
    writeOLEDString(0, 0,  F("Intellistep"), false);
    writeOLEDString(0, 32, F("Version: "), false);
    writeOLEDString(0, 48, F(VERSION), true);
}


// Function for displaying relevant data on the OLED panel, such as motor data or menu data
void updateDisplay() {

    // Decide on the current depth of the menu
    switch(menuDepthIndex){
        case 0:
            // Not actually in the menu, just displaying motor data for now
            displayMotorData();
            break;

        case 1:
            // In the top level of the menu. Make sure that the set top level menu index is within the range of the menu length
            // Values must first be determined using the mod function in order to prevent overflow errors
            // Then the strings are converted into character arrays by giving the address of the first character in the string
            clearOLED();
            writeOLEDString(0, 0, "->", false);
            writeOLEDString(25, 0,  &topLevelMenuItems[(topLevelMenuIndex)     % topLevelMenuLength][0], false);
            writeOLEDString(25, 16, &topLevelMenuItems[(topLevelMenuIndex + 1) % topLevelMenuLength][0], false);
            writeOLEDString(25, 32, &topLevelMenuItems[(topLevelMenuIndex + 2) % topLevelMenuLength][0], false);
            writeOLEDString(25, 48, &topLevelMenuItems[(topLevelMenuIndex + 3) % topLevelMenuLength][0], true);
            break;

        case 2:
            // We should be in a sub menu, this is where we have to figure out which submenu that should be
            switch(topLevelMenuIndex) {
                case 0:
                    // In the first menu, the calibration one. No need to do anything here, besides maybe displaying an progress bar or PID values (later?)
                    clearOLED();
                    writeOLEDString(0, 0, "Are you sure?", false);
                    writeOLEDString(0, 16, "Press select", false);
                    writeOLEDString(0, 32, "to confirm", true);
                    break;

                case 1:
                    // In the second menu, the motor mAs. This is dynamically generated and has increments every 100 mA from 0 mA (testing only) to 3500 mA
                    clearOLED();

                    // Constrain the current setting within 0 and the maximum current
                    if (currentCursorIndex > MAX_CURRENT / 100) {

                        // Loop back to the start of the list
                        currentCursorIndex = 0;
                    }

                    // Write the pointer
                    writeOLEDString(0, 0, "->", false);

                    // Write each of the strings
                    for (int stringIndex = 0; stringIndex <= 3; stringIndex++) {

                        // Check to make sure that the current isn't out of range of the max current
                        if (!((currentCursorIndex + stringIndex) * 100 > MAX_CURRENT)) {

                            // Value is in range, display the current on that line
                            snprintf(outBuffer, OB_SIZE, "%dmA", (int)((currentCursorIndex + stringIndex) * 100));
                            writeOLEDString(25, stringIndex * 16, outBuffer, false);
                        }
                        // else {
                            // Value is out of range, display a blank line for this line
                        // }
                    }

                    // Write out the info to the OLED display
                    writeOLEDBuffer();
                    break;

                case 2:
                    // In the microstep menu, this is also dynamically generated. Get the current stepping of the motor, then display all of the values around it
                    clearOLED();
                    writeOLEDString(0, 0, "->", false);

                    // Loop the currentCursor index back if it's out of range
                    if (currentCursorIndex > log2(MAX_MICROSTEP_DIVISOR)) {
                        currentCursorIndex = 2;
                    }
                    else if (currentCursorIndex < 2) {

                        // Make sure that the cursor index is in valid range
                        currentCursorIndex = 2;
                    }

                    // Write each of the strings
                    for (int stringIndex = 0; stringIndex <= 3; stringIndex++) {

                        // Check to make sure that the current isn't out of range of the max current
                        if (!(pow(2, currentCursorIndex + stringIndex) > MAX_MICROSTEP_DIVISOR)) {

                            // Value is in range, display the current on that line
                            snprintf(outBuffer, OB_SIZE, "1/%dth", (int)pow(2, currentCursorIndex + stringIndex));
                            writeOLEDString(25, stringIndex * 16, outBuffer, false);
                        }
                        // else {
                            // Value is out of range, display a blank line for this line
                        // }
                    }

                    // Write out the data to the OLED
                    writeOLEDBuffer();
                    break;

                case 3:
                    // In the enable logic menu, a very simple menu. Just need to invert the displayed state
                    // Clear the OLED
                    clearOLED();

                    // Title
                    writeOLEDString(0, 0, "Enable logic:", false);

                    // Write the string to the screen
                    if (currentCursorIndex % 2 == 0) {

                        // The index is even, the logic is inverted
                        writeOLEDString(0, 24, "Inverted", true);
                    }
                    else {
                        // Index is odd, the logic is normal
                        writeOLEDString(0, 24, "Normal", true);
                    }
                    break;

                case 4:
                    // Another easy menu, just the direction pin. Once again, just need to invert the state
                    clearOLED();

                    // Title
                    writeOLEDString(0, 0, "Dir logic:", false);

                    // Write the string to the screen
                    if (currentCursorIndex % 2 == 0) {

                        // The index is even, the logic is inverted
                        writeOLEDString(0, 24, "Inverted", true);
                    }
                    else {
                        // Index is odd, the logic is normal
                        writeOLEDString(0, 24, "Normal", true);
                    }
                    break;
            } // Top level menu switch
            break;
    } // Main switch

    // Save the last set value (for display functions that don't need to clear the OLED all of the time)
    lastMenuDepthIndex = menuDepthIndex;
    lastTopLevelMenuIndex = topLevelMenuIndex;

} // Display menu function


// Gets the latest parameters from the motor and puts them on screen
void displayMotorData() {

    // Clear the old menu off of the display
    if (lastMenuDepthIndex != 0) {
        clearOLED();
    }

    // RPM of the motor (RPM is capped at 2 decimal places)
    #ifdef ENCODER_SPEED_ESTIMATION

    // Check if the motor RPM can be updated. The update rate of the speed must be limited while using encoder speed estimation
    if (millis() - lastAngleSampleTime > SPD_EST_MIN_INTERVAL) {
        writeOLEDString(0, 0, (String("RPM: ") + padNumber(motor.getMotorRPM(), 2, 3)), false);
    }

    #else // ! ENCODER_SPEED_ESTIMATION

    // No need to check, just sample it
    snprintf(outBuffer, OB_SIZE, "RPM: %06.3f", motor.getMotorRPM());
    writeOLEDString(0, 0, outBuffer, false);

    #endif // ! ENCODER_SPEED_ESTIMATION

    // Angle error
    snprintf(outBuffer, OB_SIZE, "Err: %06.2f", motor.getAngleError());
    writeOLEDString(0, 16, outBuffer, false);

    // Current angle of the motor
    snprintf(outBuffer, OB_SIZE, "Deg: %06.2f", getEncoderAngle());
    writeOLEDString(0, 32, outBuffer, false);

    // Maybe a 4th line later?
    snprintf(outBuffer, OB_SIZE, "Temp: %.0f C", getEncoderTemp());
    writeOLEDString(0, 48, outBuffer, true);
}


// Function for moving the cursor up
void selectMenuItem() {

    // Go down in the menu index if we're not at the bottom already
    if (menuDepthIndex < 2) {
        menuDepthIndex++;
    }

    // If we're in certain menus, the cursor should start at their current value
    else if (menuDepthIndex == 2) {
        // Cursor settings are only needed in the submenus

        // Check the submenus available
        switch(topLevelMenuIndex % topLevelMenuLength) {

            case 0:
                // Nothing to see here, just the calibration.
                motor.calibrate();

                // Exit the menu
                menuDepthIndex--;

                break;

            case 1:
                // Motor mAs. Need to get the current motor mAs, then convert that to a cursor value
                if (motor.getCurrent() % 100 == 0) {
                    // Motor current is one of the menu items, we can check it to get the cursor index
                    currentCursorIndex = motor.getCurrent() / 100;
                }
                else {
                    // Non-standard value (probably set with serial or CAN)
                    currentCursorIndex = 0;
                }

                // Exit the menu
                menuDepthIndex--;

                break;

            case 2:
                // Motor microstepping. Need to get the current microstepping setting, then convert it to a cursor value. Needs to be -2 because the lowest index, 1/4 microstepping, would be at index 0
                currentCursorIndex = log2(motor.getMicrostepping()) - 2;

                // Exit the menu
                menuDepthIndex--;

                break;

            case 3:
                // Get if the enable pin is inverted
                if (currentCursorIndex % 2 == 0) {

                    // The index is even, the logic is inverted
                    motor.setEnableInversion(true);
                }
                else {
                    // Index is odd, the logic is normal
                    motor.setEnableInversion(false);
                }

                // Exit the menu
                menuDepthIndex--;

                break;

            case 4:
                // Get if the direction pin is inverted
                if (currentCursorIndex % 2 == 0) {

                    // The index is even, the direction is inverted
                    motor.setReversed(true);
                }
                else {
                    // Index is odd, the direction is normal
                    motor.setReversed(false);
                }

                // Exit the menu
                menuDepthIndex--;

                break;
        }

    }
}


// Function for moving the cursor down
void moveCursor() {

    // If we're on the motor display menu, do nothing for now
    if (menuDepthIndex == 0) {
        // Do nothing (maybe add a feature later?)
    }
    else if (menuDepthIndex == 1) {
        // We're in the top level menu, change the topLevelMenuIndex (as long as we haven't exceeded the length of the list)
        if (topLevelMenuIndex + 3 > topLevelMenuLength) {
            topLevelMenuIndex = 0;
        }
        else {
            topLevelMenuIndex++;
        }
    }
    else {
        // We have to be in the submenu, increment the cursor index (submenus handle the display themselves)
        currentCursorIndex++;
    }
}


// Function for exiting the current menu
void exitCurrentMenu() {

    // Go up in the menu index if we're not already at the motor data screen
    if (menuDepthIndex > 0) {
        menuDepthIndex--;
    }
}

// Returns the depth of the menu (helpful for watching the select button)
int getMenuDepth() {
    return menuDepthIndex;
}


// A list of all of the top level menu items
const char* topLevelMenuItems[] = {
    "Calibrate",
    "Motor mA",
    "Microstep",
    "En Logic",
    "Dir. Logic",
    ""
};

// Length of the list of top menu items (found by dividing the length of the list by how much space a single element takes up)
const int topLevelMenuLength = sizeof(topLevelMenuItems) / sizeof(topLevelMenuItems[0]);

// Sets up the OLED panel
void initOLED() {

    // Set all of the menu values back to their defaults (for if the screen needs to be reinitialized)
    topLevelMenuIndex = 0;

	RCC->APB2ENR |= 1<<3;
	RCC->APB2ENR |= 1<<2;

    GPIOA->CRH &= 0XFFFffFF0;
    GPIOA->CRH |= 0X00000003;
    GPIOA->ODR |= 1<<8;

  	GPIOB->CRH &= 0X0000FFFF;
  	GPIOB->CRH |= 0X33330000;
	GPIOB->ODR |= 0xF<<12;

	OLED_RST_PIN = 0;
	delay(100);
	OLED_RST_PIN = 1;
	writeOLEDByte(0xAE, COMMAND);//
	writeOLEDByte(0xD5, COMMAND);//
	writeOLEDByte(80,   COMMAND);  //[3:0],;[7:4],
	writeOLEDByte(0xA8, COMMAND);//
	writeOLEDByte(0X3F, COMMAND);//(1/64)
	writeOLEDByte(0xD3, COMMAND);//
	writeOLEDByte(0X00, COMMAND);//

	writeOLEDByte(0x40, COMMAND);// [5:0],.

	writeOLEDByte(0x8D, COMMAND);//
	writeOLEDByte(0x14, COMMAND);///
	writeOLEDByte(0x20, COMMAND);//
	writeOLEDByte(0x02, COMMAND);//[1:0],;
	writeOLEDByte(0xA1, COMMAND);//,bit0:0,0->0;1,0->127;
	writeOLEDByte(0xC0, COMMAND);//;bit3:0,;1, COM[N-1]->COM0;N:
	writeOLEDByte(0xDA, COMMAND);//
	writeOLEDByte(0x12, COMMAND);//[5:4]

	writeOLEDByte(0x81, COMMAND);//
	writeOLEDByte(0xEF, COMMAND);//1~255;
	writeOLEDByte(0xD9, COMMAND);//
	writeOLEDByte(0xf1, COMMAND);//[3:0],PHASE 1;[7:4],PHASE 2;
	writeOLEDByte(0xDB, COMMAND);//
	writeOLEDByte(0x30, COMMAND);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	writeOLEDByte(0xA4, COMMAND);//;bit0:1,;0,;
	writeOLEDByte(0xA6, COMMAND);//;bit0:1,;0,
	writeOLEDByte(0xAF, COMMAND);//
	delay(100);
	clearOLED();
}


// Write a single byte to the LCD panel
void writeOLEDByte(uint8_t data, OLED_MODE mode) {

    // Write the current mode and enable the screen's communcation
	OLED_RS_PIN = (uint8_t)mode;
	OLED_CS_PIN = 0;

    // Write each bit of the byte
	for(uint8_t i = 0; i < 8; i++) {

        // Prevent the screen from reading the data in
		OLED_SCLK_PIN = 0;

        // If the bit is 1 (true)
		if(data & 0x80) {

            // Write true
            OLED_SDIN_PIN = 1;
        }
		else {
            // Write false
            OLED_SDIN_PIN = 0;
        }

        // Signal that the data needs to be sampled again
		OLED_SCLK_PIN = 1;

        // Shift all of the bits so the next will be read
		data <<= 1;
	}

    // Disable the screen's communication
	OLED_CS_PIN = 1;
	OLED_RS_PIN = 1;
}


// Turn the display on
void writeOLEDOn() {
	writeOLEDByte(0X8D, COMMAND);  //SET
	writeOLEDByte(0X14, COMMAND);  //DCDC ON
	writeOLEDByte(0XAF, COMMAND);  //DISPLAY ON
}


// Turn the display off
void writeOLEDOff() {
	writeOLEDByte(0X8D, COMMAND);  //SET
	writeOLEDByte(0X10, COMMAND);  //DCDC OFF
	writeOLEDByte(0XAE, COMMAND);  //DISPLAY OFF
}


// Write the entire OLED buffer to the screen
void writeOLEDBuffer() {

    // Loop through each of the vertical lines
	for(uint8_t vertIndex = 0; vertIndex < 8; vertIndex++) {

		// Specify the line that is being written to
        writeOLEDByte(0xb0 + vertIndex, COMMAND);
		writeOLEDByte(0x00, COMMAND);
		writeOLEDByte(0x10, COMMAND); // DCDC OFF

        // Loop through the horizontal lines
		for(uint8_t horzIndex = 0; horzIndex < 128; horzIndex++) {
		    writeOLEDByte(OLEDBuffer[horzIndex][vertIndex], DATA);
        }
	}
}


// Wipes the output buffer, then writes the zeroed array to the screen
void clearOLED() {

    // Set all of the values in the buffer to 0
	memset(OLEDBuffer, 0X00, sizeof(OLEDBuffer));

    // Push the values to the display
	writeOLEDBuffer();
}


// Writes a point on the buffer
void setOLEDPixel(uint8_t x, uint8_t y, OLED_COLOR color) {

    // Create variables for later use
	uint8_t yPos;

    // Exit if the function would be writing to an index that is off of the screen
	if (x > 127 || y > 63) {
        return;
    }

    // Calculate the y position
	yPos = 7 - (y / 8);

    // Write white pixels if the point isn't inverted
	if(color) {
        OLEDBuffer[x][yPos] |= (1<<(7-(y%8)));
    }

    // Otherwise, write black pixels to the point
	else {
        OLEDBuffer[x][yPos] &= ~(1<<(7-(y%8)));
    }
}


// Fill an area of pixels, starting at x1 or y1 to x2 or y2
void fillOLED(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, OLED_COLOR color, bool updateScreen) {

    // Loop through all of the X points
	for(uint8_t x = x1; x <= x2; x++) {

        // Loop through all of the Y points
		for(uint8_t y = y1; y <= y2; y++) {
            setOLEDPixel(x, y, color);
        }
	}

    // Write the buffer to the screen if specified
    if (updateScreen) {
	    writeOLEDBuffer();
    }
}


// Writes a characer to the display
void writeOLEDChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t fontSize, OLED_COLOR color, bool updateScreen) {
	
    // Create the variables to be used
    uint8_t pixelData;
	uint8_t y0 = y;

    // Correct the character index
	chr = chr - ' ';

    // Loop through the data of each of the pixels of the character
    for (uint8_t pixelIndex = 0; pixelIndex < fontSize; pixelIndex++) {

        // Use the appropriate font data
		if (fontSize == 12) {
            pixelData = OLED_1206_Font[chr][pixelIndex];
        }
		else {
            pixelData = OLED_1608_Font[chr][pixelIndex];
        }

        // Loop through the bits, setting them on the buffer array
        for(uint8_t bitIndex = 0; bitIndex < 8; bitIndex++) {

            // Check if the pixel should be inverted (as compared to what was set)
			if (pixelData & 0x80) {
                setOLEDPixel(x, y, color);
            }
			else {
                setOLEDPixel(x, y, OLED_COLOR(!color));
            }

            // Shift all of the bits to the left, bringing the next bit in for the next cycle
			pixelData <<= 1;

            // Move down in the y direction so two pixels don't overwrite eachother
			y++;

            // When the character vertical's vertical column is filled, move to the next one
			if((y-y0) == fontSize) {
				y = y0;
				x++;
				break;
			}
		}
    }

    // Update the screen if desired
    if (updateScreen) {
        writeOLEDBuffer();
    }
	
}


// Writes a number to the OLED display
void writeOLEDNum(uint8_t x, uint8_t y, uint32_t number, uint8_t len, uint8_t fontSize, bool updateScreen) {

    // Create variables
	uint8_t digit;
	uint8_t enshow = 0;

    // Loop through each of the digits
	for (uint8_t digitIndex = 0; digitIndex < len; digitIndex++) {

        // Choose the digit out of the number
		digit = (number / (uint8_t)pow(10, len - (digitIndex + 1))) % 10;

        // Display the digit if everything is good
		if(enshow == 0 && digitIndex < (len-1)) {
			if(digit == 0) {
				writeOLEDChar(x+(fontSize/2)*digitIndex, y, ' ', fontSize, WHITE, false);
				continue;
			}
            else {
                enshow = 1;
            }
		}

        // Write out the character
	 	writeOLEDChar(x+(fontSize/2)*digitIndex, y, digit + '0', fontSize, WHITE, false);
	}

    // Update the screen if specified
    if (updateScreen) {
        writeOLEDBuffer();
    }
}


// Function to write a string to the screen
void writeOLEDString(uint8_t x, uint8_t y, const char *p, uint8_t fontSize, bool updateScreen) {

    // Check if we haven't reached the end of the string
    while(*p != '\0') {

        // Make sure that the x and y don't exceed the maximum indexes
        if(x > MAX_CHAR_POSX){
            x = 0;
            y += 16;
        }
        if(y > MAX_CHAR_POSY){
            y = x = 0;
            clearOLED();
        }

        // Display the character on the screen
        writeOLEDChar(x, y, *p, fontSize, WHITE, false);

        // Move over by half the font size
        x += (fontSize / 2);

        // Move to the next character
        p++;
    }

    // Update the screen if specified
    if (updateScreen) {
        writeOLEDBuffer();
    }
}


// Convience function for writing a specific string to the OLED panel
void writeOLEDString(uint8_t x, uint8_t y, String string, bool updateScreen) {
    writeOLEDString(x, y, string.c_str(), 16, updateScreen);
}