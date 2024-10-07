#ifndef NOTEPARSER_CIRCLE_HPP
#define NOTEPARSER_CIRCLE_HPP

unsigned constexpr trigono[] = {
	10000,
	9877,
	9510,
	8910,
	8090,
	7071,
	5878,
	4540,
	3090,
	1564,
	0,
};
unsigned constexpr n_trigono = sizeof(trigono) / sizeof(unsigned);

inline int ICos(unsigned theta) {
	return trigono[theta % (2 * n_trigono - 2) / (n_trigono - 1) == 0 ? theta % (n_trigono - 1) : (n_trigono - 1) - theta % (n_trigono - 1)] * ((theta + (n_trigono - 1)) / (2 * n_trigono - 2) % 2 == 0 ? 1 : -1);
}

inline int ISin(unsigned theta) {
	return trigono[theta % (2 * n_trigono - 2) / (n_trigono - 1) == 0 ? (n_trigono - 1) - theta % (n_trigono - 1) : theta % (n_trigono - 1)] * (theta / (2 * n_trigono - 2) % 2 == 0 ? 1 : -1);
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