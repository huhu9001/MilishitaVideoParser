#include"..\\MilishitaVideoParser\\noteParser.hpp"

unsigned const noteParser::n_channels = 6U;

unsigned noteParser::GetPixelIndex(unsigned n_channel, int x_offset, int y_offset) const {
	return (x_focus[n_channel] + x_offset) * 3 + (y_focus - y_offset) * linesize;
}

unsigned noteParser::GetCenterPixelIndex(int x_offset, int y_offset) const {
	return (x_center + x_offset) * 3 + (y_focus - y_offset) * linesize;
}