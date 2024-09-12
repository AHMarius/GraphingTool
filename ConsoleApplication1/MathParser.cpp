#include "raylib.h"
#include <string>

std::string GetUserInputAndSetExpression() {
	const int maxInputLength = 100;
	char inputBuffer[maxInputLength + 1] = "\0";  // Buffer to hold input
	bool isInputting = true;
	bool shiftPressed = false;

	// Main loop
	while (isInputting) {
		// Update shift state
		shiftPressed = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

		// Capture text input
		int key = GetKeyPressed();
		if (key > 0) { // If a key was pressed
			char charToAdd = '\0'; // Initialize character to be added

			if (key >= 32 && key <= 126) { // Allow printable ASCII characters
				// Handle shift functionality for special characters
				if (shiftPressed) {
					switch (key) {
					case KEY_ONE: charToAdd = '!'; break;
					case KEY_TWO: charToAdd = '@'; break;
					case KEY_THREE: charToAdd = '#'; break;
					case KEY_FOUR: charToAdd = '$'; break;
					case KEY_FIVE: charToAdd = '%'; break;
					case KEY_SIX: charToAdd = '^'; break;
					case KEY_SEVEN: charToAdd = '&'; break;
					case KEY_EIGHT: charToAdd = '*'; break;
					case KEY_NINE: charToAdd = '('; break;
					case KEY_ZERO: charToAdd = ')'; break;
					case KEY_MINUS: charToAdd = '_'; break;
					case KEY_EQUAL: charToAdd = '+'; break;
					case KEY_LEFT_BRACKET: charToAdd = '{'; break;
					case KEY_RIGHT_BRACKET: charToAdd = '}'; break;
					case KEY_BACKSLASH: charToAdd = '|'; break;
					case KEY_SEMICOLON: charToAdd = ':'; break;
					case KEY_APOSTROPHE: charToAdd = '"'; break;
					case KEY_COMMA: charToAdd = '<'; break;
					case KEY_PERIOD: charToAdd = '>'; break;
					case KEY_SLASH: charToAdd = '?'; break;
					case KEY_A: charToAdd = 'A'; break;
					case KEY_B: charToAdd = 'B'; break;
						// Add more cases for other letters if needed
					default: charToAdd = '\0'; break;
					}
				}
				else {
					switch (key) {
					case KEY_MINUS: charToAdd = '-'; break;
					case KEY_EQUAL: charToAdd = '='; break;
						// Add cases for other non-shifted characters if needed
					default: charToAdd = (char)key; break;
					}
				}

				// Handle input length
				if (charToAdd != '\0' && strlen(inputBuffer) < maxInputLength) {
					int length = strlen(inputBuffer);
					inputBuffer[length] = charToAdd;
					inputBuffer[length + 1] = '\0'; // Null-terminate the string
				}
			}
			else if (key == KEY_BACKSPACE) {
				int length = strlen(inputBuffer);
				if (length > 0) {
					inputBuffer[length - 1] = '\0'; // Remove last character
				}
			}
			else if (key == KEY_ENTER) {
				isInputting = false;  // End inputting
			}
		}

		// Render
		BeginDrawing();
		ClearBackground(BLACK);
		DrawText("Math here:", 20, 20, 20, DARKGRAY);
		DrawText(inputBuffer, 20, 60, 20, MAROON);
		EndDrawing();
	}

	CloseWindow();  // Close the window and cleanu
	return std::string(inputBuffer);  // Return the inputted expression as a string
}
