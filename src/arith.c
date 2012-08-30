
/*
 * Common TeX:arith.c:3.2:EPA
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 *
 * 
 */

#include "arith.h"
#include "print.h"
#include <sstream>

bool	tex::arith_error;
scal	tex::remainder;

int tex::half(int x)
	{
	return (odd(x) ? (x + 1) / 2 : x / 2);
	}

std::string tex::scal2string(scal s)
	{
	std::stringstream str;
	scal	delta;
	
	if (s < 0) {
		str << "-";
		s = -s;
		}
	str << (int)(s/UNITY) << ".";
	s = 10 * (s % UNITY) + 5;
	delta = 10;
	do {
		if (delta > UNITY)
			s += 0100000 - 50000;
		str << (char)('0' + s / UNITY);
		s = 10 * (s % UNITY);
		delta *= 10;
		} while (s > delta);

	return str.str();
	}

void tex::print_scaled(scal s)
	{
	scal	delta;
	
	if (s < 0) {
		tex::print("-");
		s = -s;
		}
	tex::print_int(s / UNITY);
	tex::print(".");
	s = 10 * (s % UNITY) + 5;
	delta = 10;
	do {
		if (delta > UNITY)
			s += 0100000 - 50000;
		tex::print_char('0' + s / UNITY);
		s = 10 * (s % UNITY);
		delta *= 10;
		} while (s > delta);
	}

scal tex::mult_and_add(int n, scal x, scal y, scal max_answer)
	{
	if (n < 0) {
		x = -x;
		n = -n;
		}
	if (n == 0)
		return y;
	if (x <= (max_answer - y) / n && -x <= (max_answer + y) / n)
		return (n * x + y);
	tex::arith_error = true;
	return 0;
	}

scal tex::x_over_n(scal x, int n)
	{
	bool	negative;
	scal	quotient;

	negative = false;
	if (n == 0) {
		tex::arith_error = true;;
		tex::remainder = x;
		return 0;
	}
	if (n < 0) {
		x = -x;
		n = -n;
		negative = true;;
	}
	if (x >= 0) {
		quotient = x / n;
		tex::remainder = x % n;
	} else {
		quotient = -(-x / n);
		tex::remainder = -(-x % n);
	}
	if (negative)
		negate(tex::remainder);
	return quotient;
	}

scal tex::xn_over_d(scal x, int n, int d)
	{
	int	t;
	int	u;
	int	v;
	bool	positive;

	if (x >= 0) {
		positive = true;;
	} else {
		x = -x;
		positive = false;
	}
	t = x % 0100000 * n;
	u = x / 0100000 * n + t / 0100000;
	v = u % d * 0100000 + t % 0100000;
	if (u / d >= 0100000) {
		tex::arith_error = true;
	} else {
		u = 0100000 * (u / d) + v / d;
	}
	if (positive) {
		tex::remainder = v % d;
		return u;
	} else {
		tex::remainder = - (v % d);
		return -u;
	}
	}

int tex::badness(scal t, scal s)
	{
	int	r;

	if (t == 0) {
		return 0;
	} else if (s <= 0) {
		return INF_BAD;
	} else {
		if (t <= 7230584)
			r = t * 297 / s;
		else if (s >= 1663497)
			r = t / (s / 297);
		else
			r = t;
		if (r > 1290)
			return INF_BAD;
		else
			return ((r * r * r + 0400000) / 01000000);
	}
	}

scal tex::nx_plus_y(int N, scal X, scal Y)
	{
	return tex::mult_and_add(N,X,Y,07777777777);
	}

scal tex::mult_integers(int N, scal X) 
	{
	return tex::mult_and_add(N,X,0,017777777777);
	}
