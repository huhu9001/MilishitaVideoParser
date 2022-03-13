#include"..\\MilishitaVideoParser\\noteParser.hpp"

noteParser::Color noteParser::GetPixel(uint8_t const*frame0, unsigned linesize, unsigned x, unsigned y) {
	if (x < 2) return RecogColor(RgbToHsv(RgbColor{
			frame0[(y_2pbottom - y) * 3 + x_2pfocus[x] * linesize],
			frame0[(y_2pbottom - y) * 3 + x_2pfocus[x] * linesize + 1],
			frame0[(y_2pbottom - y) * 3 + x_2pfocus[x] * linesize + 2],
		}));
	else return black;
}