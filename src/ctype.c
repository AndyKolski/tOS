#include <ctype.h>
#include <system.h>

char toLower(char c) {
	if (isUpper(c)) {
		return c + 32;
	}
	return c;
}
char toUpper(char c) {
	if (isLower(c)) {
		return c - 32;
	}
	return c;
}

bool isCharInRange (char a, char b, char c) {
	return ((c >= a) && (c <= b));
}

bool isxDigit(char c) {
	return (isCharInRange('A', 'F', c) || isCharInRange('a', 'f', c) || isDigit(c));
}
bool isDigit(char c) {
	return isCharInRange('0', '9', c);
}
bool isLower(char c) {
	return isCharInRange('a', 'z', c);
}
bool isUpper(char c) {
	return isCharInRange('A', 'Z', c);
}
bool isAlpha(char c) {
	return (isUpper(c) || isLower(c));
}
bool isAlNum(char c) {
	return (isAlpha(c) || isDigit(c));
}
bool isPunct(char c) {
	return isCharInRange('!', '/', c) || isCharInRange(':', '@', c) || isCharInRange('[', '`', c) || isCharInRange('{', '~', c);
}
bool isGraph(char c) {
	return (isAlNum(c) || isPunct(c));
}
bool isBlank(char c) {
	return (c == ' ' || c == '\t');
}
bool isSpace(char c) {
	return (isBlank(c) || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}
bool isPrint(char c) {
	return (isGraph(c) || c == ' ');
}
bool isCntrl(char c) {
	return !isPrint(c);
}