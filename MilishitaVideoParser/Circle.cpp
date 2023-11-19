#pragma once
#include".\\Circle.hpp"

int ICos(unsigned theta) {
	return trigono[theta % (2 * n_trigono - 2) / (n_trigono - 1) == 0 ? theta % (n_trigono - 1) : (n_trigono - 1) - theta % (n_trigono - 1)] * ((theta + (n_trigono - 1)) / (2 * n_trigono - 2) % 2 == 0 ? 1 : -1);
}

int ISin(unsigned theta) {
	return trigono[theta % (2 * n_trigono - 2) / (n_trigono - 1) == 0 ? (n_trigono - 1) - theta % (n_trigono - 1) : theta % (n_trigono - 1)] * (theta / (2 * n_trigono - 2) % 2 == 0 ? 1 : -1);
}

unsigned ISqrt(unsigned i) {
	unsigned i0 = 1, i1 = i;
	while (i1) {
		i1 >>= 2;
		i0 <<= 1;
	}
	while (i0 >>= 1) if ((i0 | i1) * (i0 | i1) <= i) i1 |= i0;
	return i1;
}