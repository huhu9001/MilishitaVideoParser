#ifndef NOTEPARSER_COLOR_HPP
#define NOTEPARSER_COLOR_HPP

struct RgbColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};
struct HsvColor {
	unsigned char h;
	unsigned char s;
	unsigned char v;
	unsigned char l;
	HsvColor(RgbColor rgb) {
		unsigned char rgbMax, rgbMin;
		if (rgb.r > rgb.g) {
			rgbMax = rgb.r > rgb.b ? rgb.r : rgb.b;
			rgbMin = rgb.g < rgb.b ? rgb.g : rgb.b;
		}
		else {
			rgbMax = rgb.g > rgb.b ? rgb.g : rgb.b;
			rgbMin = rgb.r < rgb.b ? rgb.r : rgb.b;
		}

		if (rgbMax != rgbMin) {
			v = rgbMax;
			l = (rgbMax + rgbMin) / 2;
			s = 255 * long(rgbMax - rgbMin) / v;

			if (rgbMax == rgb.r)
				h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
			else if (rgbMax == rgb.g)
				h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
			else
				h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);
		}
		else h = s = 0, v = l = rgbMax;
	}
};

#endif