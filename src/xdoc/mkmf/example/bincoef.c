/*
Biniomial coefficients
*/

#include "binom.h"

int bincoef (int n, int k)
{
	return factorial(n) / (factorial(k) * factorial(n - k));
}
