
/*
 * Common TeX:arith.h:3.2:EPA
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 *
 * 
 */

#include <string>
#include <iostream>

#include "textypes.h"

// Functions for TeX-style numerics.

namespace tex {

	int	half(int);
	scal	round_decimals(int);
	void	print_scaled(scal s);
	scal	mult_and_add(int n, scal x, scal y, scal max_answer);
	scal	x_over_n(scal x, int n);
	scal	xn_over_d(scal x, int n, int d);
	int	badness(scal t, scal s);
	scal  nx_plus_y(int N, scal X, scal Y);
	scal  mult_integers(int N, scal X);
	
	std::string scal2string(scal s);
	
	extern bool	arith_error;
	extern scal	remainder;
		
	const static int32_t UNITY  =0200000;
	const static int32_t TWO    =0400000;
	const static int32_t INF_BAD=10000;

};
