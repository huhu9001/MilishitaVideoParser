#ifndef NOTEPARSER_COLOR_HPP
#define NOTEPARSER_COLOR_HPP

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
	HsvColor(RgbColor rgb) {
		unsigned char rgbMin, rgbMax;

		rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
		rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

		v = rgbMax;
		l = (rgbMax + rgbMin) / 2;
		if (v == 0) {
			h = 0;
			s = 0;
			return;
		}

		s = 255 * long(rgbMax - rgbMin) / v;
		if (s == 0) {
			h = 0;
			return;
		}

		if (rgbMax == rgb.r)
			h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
		else if (rgbMax == rgb.g)
			h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
		else
			h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);
	}
} HsvColor;

#endif