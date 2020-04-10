#ifndef FONT_H
# define FONT_H
uint8 fontWidth = 5;
uint8 fontHeight = 11;

uint8 font[0xff][11] = {
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0000000,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0010100,0b0010100,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0010100,0b0010100,0b0111110,0b0010100,0b0010100,0b0111110,0b0010100,0b0010100,0b0000000,0b0000000,0b0000000},
	{0b0011110,0b0101000,0b0101000,0b0011000,0b0001100,0b0001010,0b0001010,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0011010,0b0100100,0b0100100,0b0011000,0b0001100,0b0010010,0b0010010,0b0101100,0b0000000,0b0000000,0b0000000},
	{0b0011000,0b0100100,0b0100100,0b0011000,0b0101000,0b0100110,0b0100100,0b0011010,0b0000000,0b0000000,0b0000000},
	{0b0001000,0b0001000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000100,0b0001000,0b0010000,0b0010000,0b0010000,0b0010000,0b0001000,0b0000100,0b0000000,0b0000000,0b0000000},
	{0b0010000,0b0001000,0b0000100,0b0000100,0b0000100,0b0000100,0b0001000,0b0010000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0101010,0b0011100,0b0001000,0b0011100,0b0101010,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0001000,0b0001000,0b0111110,0b0001000,0b0001000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0001000,0b0010000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0111110,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0010000,0b0000000,0b0000000,0b0000000},
	{0b0000010,0b0000100,0b0000100,0b0001000,0b0001000,0b0010000,0b0010000,0b0100000,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100110,0b0101010,0b0101010,0b0101010,0b0101010,0b0110010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0001000,0b0011000,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100010,0b0000100,0b0001000,0b0010000,0b0100000,0b0100000,0b0111110,0b0000000,0b0000000,0b0000000},
	{0b0111100,0b0000010,0b0000010,0b0111100,0b0000010,0b0000010,0b0000010,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0000110,0b0001010,0b0010010,0b0100010,0b0111110,0b0000010,0b0000010,0b0000010,0b0000000,0b0000000,0b0000000},
	{0b0111110,0b0100000,0b0100000,0b0111100,0b0000010,0b0000010,0b0000010,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100010,0b0100000,0b0111100,0b0100010,0b0100010,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0111110,0b0000010,0b0000100,0b0000100,0b0001000,0b0001000,0b0010000,0b0010000,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100010,0b0100010,0b0011100,0b0100010,0b0100010,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100010,0b0100010,0b0011110,0b0000010,0b0000010,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0001000,0b0000000,0b0000000,0b0000000,0b0001000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0001000,0b0000000,0b0000000,0b0000000,0b0001000,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0000100,0b0001000,0b0010000,0b0100000,0b0100000,0b0010000,0b0001000,0b0000100,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0111110,0b0000000,0b0111110,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0100000,0b0010000,0b0001000,0b0000100,0b0000100,0b0001000,0b0010000,0b0100000,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0000100,0b0000100,0b0001100,0b0001000,0b0001000,0b0000000,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100010,0b0100010,0b0101010,0b0101010,0b0101110,0b0100000,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0001000,0b0010100,0b0100010,0b0100010,0b0111110,0b0100010,0b0100010,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0111100,0b0100010,0b0100010,0b0111100,0b0100010,0b0100010,0b0100010,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100010,0b0100000,0b0100000,0b0100000,0b0100000,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0111100,0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0111110,0b0100000,0b0100000,0b0111110,0b0100000,0b0100000,0b0100000,0b0111110,0b0000000,0b0000000,0b0000000},
	{0b0111110,0b0100000,0b0100000,0b0111100,0b0100000,0b0100000,0b0100000,0b0100000,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100000,0b0100000,0b0101110,0b0100010,0b0100010,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0100010,0b0100010,0b0111110,0b0100010,0b0100010,0b0100010,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0000100,0b0000100,0b0000100,0b0000100,0b0000100,0b0000100,0b0100100,0b0011000,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0100100,0b0101000,0b0110000,0b0101000,0b0100100,0b0100010,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0100000,0b0100000,0b0100000,0b0100000,0b0100000,0b0100000,0b0100000,0b0111110,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0110110,0b0101010,0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0110010,0b0110010,0b0101010,0b0101010,0b0100110,0b0100110,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0111100,0b0100010,0b0100010,0b0111100,0b0100000,0b0100000,0b0100000,0b0100000,0b0000000,0b0000000,0b0000000},
	{0b0011100,0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0100100,0b0011010,0b0000000,0b0000000,0b0000000},
	{0b0111100,0b0100010,0b0100010,0b0111100,0b0100010,0b0100010,0b0100010,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0011110,0b0100000,0b0100000,0b0011000,0b0001100,0b0000010,0b0000010,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0111110,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0010100,0b0010100,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0100010,0b0100010,0b0100010,0b0100010,0b0101010,0b0110110,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0010100,0b0010100,0b0001000,0b0001000,0b0010100,0b0010100,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0100010,0b0010100,0b0010100,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0111110,0b0000010,0b0000100,0b0001000,0b0001000,0b0010000,0b0100000,0b0111110,0b0000000,0b0000000,0b0000000},
	{0b0111100,0b0100000,0b0100000,0b0100000,0b0100000,0b0100000,0b0100000,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0100000,0b0010000,0b0010000,0b0001000,0b0001000,0b0000100,0b0000100,0b0000010,0b0000000,0b0000000,0b0000000},
	{0b0111100,0b0000100,0b0000100,0b0000100,0b0000100,0b0000100,0b0000100,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0001000,0b0010100,0b0100010,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0111110,0b0000000,0b0000000,0b0000000},
	{0b0100000,0b0010000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0011100,0b0000010,0b0011110,0b0100010,0b0011110,0b0000000,0b0000000,0b0000000},
	{0b0100000,0b0100000,0b0100000,0b0111100,0b0100010,0b0100010,0b0100010,0b0111100,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0011100,0b0100010,0b0100000,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0000010,0b0000010,0b0000010,0b0011110,0b0100010,0b0100010,0b0100010,0b0011110,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0011100,0b0100010,0b0111100,0b0100000,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0001100,0b0010000,0b0010000,0b0111100,0b0010000,0b0010000,0b0010000,0b0010000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0011100,0b0100010,0b0100010,0b0100010,0b0011110,0b0000010,0b0100010,0b0011100},
	{0b0100000,0b0100000,0b0100000,0b0100000,0b0111100,0b0100010,0b0100010,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0001000,0b0000000,0b0001000,0b0001000,0b0001000,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000100,0b0000000,0b0000100,0b0000100,0b0000100,0b0000100,0b0000100,0b0100100,0b0011000},
	{0b0100000,0b0100000,0b0100000,0b0100110,0b0101000,0b0110000,0b0101100,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0010000,0b0010000,0b0010000,0b0010000,0b0010000,0b0010000,0b0010000,0b0011000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0010100,0b0101010,0b0100010,0b0100010,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0111100,0b0100010,0b0100010,0b0100010,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0011100,0b0100010,0b0100010,0b0100010,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0011100,0b0100010,0b0100010,0b0100010,0b0111100,0b0100000,0b0100000,0b0100000},
	{0b0000000,0b0000000,0b0000000,0b0011100,0b0100010,0b0100010,0b0100010,0b0011110,0b0000010,0b0000010,0b0000010},
	{0b0000000,0b0000000,0b0000000,0b0001100,0b0010010,0b0010000,0b0010000,0b0010000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0001100,0b0010000,0b0001000,0b0000100,0b0011000,0b0000000,0b0000000,0b0000000},
	{0b0001000,0b0001000,0b0001000,0b0011100,0b0001000,0b0001000,0b0001010,0b0000100,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0100010,0b0100010,0b0100010,0b0100010,0b0011110,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0100010,0b0100010,0b0100010,0b0010100,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0100010,0b0100010,0b0100010,0b0101010,0b0010100,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0100010,0b0010100,0b0001000,0b0010100,0b0100010,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0100010,0b0100010,0b0100010,0b0100010,0b0011110,0b0000010,0b0000010,0b0011100},
	{0b0000000,0b0000000,0b0000000,0b0011100,0b0000100,0b0001000,0b0010000,0b0011100,0b0000000,0b0000000,0b0000000},
	{0b0000110,0b0001000,0b0011000,0b0100000,0b0100000,0b0011000,0b0001000,0b0000110,0b0000000,0b0000000,0b0000000},
	{0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0001000,0b0000000,0b0000000,0b0000000},
	{0b0110000,0b0001000,0b0001100,0b0000010,0b0000010,0b0001100,0b0001000,0b0110000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0010000,0b0101010,0b0000100,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
	{0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000},
};

#endif