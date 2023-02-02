#include <stdbool.h>
#include <ctype.h>

/// @brief Converts a character to lowercase
/// @param c The character to convert
/// @return The lowercase version of the character
char toLower(char c) {
	if (isUpper(c)) {
		return c + 32;
	}
	return c;
}

/// @brief Converts a character to uppercase
/// @param c The character to convert
/// @return The uppercase version of the character
char toUpper(char c) {
	if (isLower(c)) {
		return c - 32;
	}
	return c;
}

/// @brief Checks if a character is in a range
/// @param a The beginning of the range
/// @param b The end of the range
/// @param c The character to test
/// @return Returns true if the character is in the range, false otherwise
bool isCharInRange (char a, char b, char c) {
	return ((c >= a) && (c <= b));
}

/// @brief Checks if a character is a hexadecimal digit
/// @param c The character to test
/// @return Returns true if the character is a hexadecimal digit, false otherwise
bool isxDigit(char c) {
	return (isCharInRange('A', 'F', c) || isCharInRange('a', 'f', c) || isDigit(c));
}

/// @brief Checks if a character is a digit
/// @param c The character to test
/// @return Returns true if the character is a digit, false otherwise
bool isDigit(char c) {
	return isCharInRange('0', '9', c);
}

/// @brief Checks if a character is a lowercase letter
/// @param c The character to test
/// @return Returns true if the character is a lowercase letter, false otherwise
bool isLower(char c) {
	return isCharInRange('a', 'z', c);
}

/// @brief Checks if a character is an uppercase letter
/// @param c The character to test
/// @return Returns true if the character is an uppercase letter, false otherwise
bool isUpper(char c) {
	return isCharInRange('A', 'Z', c);
}

/// @brief Checks if a character is a letter
/// @param c The character to test
/// @return Returns true if the character is a letter, false otherwise
bool isAlpha(char c) {
	return (isUpper(c) || isLower(c));
}

/// @brief Checks if a character is alphanumeric (A letter or a number)
/// @param c The character to test
/// @return Returns true if the character is alphanumeric, false otherwise
bool isAlNum(char c) {
	return (isAlpha(c) || isDigit(c));
}

/// @brief Checks if a character is a punctuation character
/// @param c The character to test
/// @return Returns true if the character is a punctuation character, false otherwise
bool isPunct(char c) {
	return isCharInRange('!', '/', c) || isCharInRange(':', '@', c) || isCharInRange('[', '`', c) || isCharInRange('{', '~', c);
}

/// @brief Checks if a character is a graphical character
/// @param c The character to test
/// @return Returns true if the character is a graphical character, false otherwise
bool isGraph(char c) {
	return (isAlNum(c) || isPunct(c));
}

/// @brief Checks if a character is a blank character
/// @param c The character to test
/// @return Returns true if the character is a blank character, false otherwise
bool isBlank(char c) {
	return (c == ' ' || c == '\t');
}

/// @brief Checks if a character is a space character
/// @param c The character to test
/// @return Returns true if the character is a space character, false otherwise
bool isSpace(char c) {
	return (isBlank(c) || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

/// @brief Checks if a character is printable
/// @param c The character to test
/// @return Returns true if the character is printable, false otherwise
bool isPrint(char c) {
	return (isGraph(c) || c == ' ');
}

/// @brief Checks if a character is a control character
/// @param c The character to test
/// @return Returns true if the character is a control character, false otherwise
bool isCntrl(char c) {
	return !isPrint(c);
}