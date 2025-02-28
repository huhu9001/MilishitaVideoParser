#ifndef NOTEPARSER_CIRCLE_HPP
#define NOTEPARSER_CIRCLE_HPP

constexpr int trigono[] = {
	0, 1564, 3090, 4540, 5878, 7071, 8090, 8910, 9510, 9877,
	10000, 9877, 9510, 8910, 8090, 7071, 5878, 4540, 3090, 1564,
	0, -1564, -3090, -4540, -5878, -7071, -8090, -8910, -9510, -9877,
	-10000, -9877, -9510, -8910, -8090, -7071, -5878, -4540, -3090, -1564,
	0, 1564, 3090, 4540, 5878, 7071, 8090, 8910, 9510, 9877,
};

unsigned constexpr n_trigono_quarter = sizeof(trigono) / sizeof(*trigono) / 5;
unsigned constexpr n_trigono = n_trigono_quarter * 4;

inline int ICos(unsigned theta) {
	return trigono[theta + n_trigono_quarter];
}

inline int ISin(unsigned theta) {
	return trigono[theta];
}

inline unsigned ISqrt(unsigned i) {
	unsigned i0 = 1, i1 = i;
	while (i1) {
		i1 >>= 2;
		i0 <<= 1;
	}
	while (i0 >>= 1) if (unsigned const i2 = i0 | i1; i2 * i2 <= i) i1 = i2;
	return i1;
}

#endif