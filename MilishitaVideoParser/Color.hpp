#pragma once
typedef struct RgbColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RgbColor;
typedef struct HsvColor {
	unsigned char h;
	unsigned char s;
	unsigned char v;
	unsigned char l;
} HsvColor;
HsvColor RgbToHsv(RgbColor const&rgb);
