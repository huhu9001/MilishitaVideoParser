#include"..\\MilishitaVideoParser\\noteParser.hpp"

noteParser::Color noteParser::GetPixel(uint8_t const*frame0, unsigned linesize, unsigned x, unsigned y) {
	if (x < 2) return RecogColor(RgbToHsv(RgbColor{
			frame0[x_2mfocus[x] * 3 + (y_bottom - y) * linesize],
			frame0[x_2mfocus[x] * 3 + (y_bottom - y) * linesize + 1],
			frame0[x_2mfocus[x] * 3 + (y_bottom - y) * linesize + 2],
		}));
	else return black;
}