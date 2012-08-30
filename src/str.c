
/*
 * $Id: str.c,v 1.4 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include "tex.h"

str	tex::null_str;
str	tex::ASCII[256];
str	tex::cur_str;
str	tex::cur_str_end;
str	tex::cur_str_ptr;


// Make a new string and fill it with the contents of the global
// scratch buffer 'cur_str', from 'cur_str' to 'cur_str_ptr'.
// Reset cur_str_ptr to the beginning (ie. empty the global buffer).

str tex::make_str()
	{
	int	n;
	str	s;

	n = cur_length();
	s = new_str(n);
	memcpy(s, cur_str, n);
	cur_str_ptr = cur_str;

	return s;
	}


// Check whether there is still space for 'i' characters more in
// the global string buffer.

void tex::str_room(int i)
	{
	if (cur_str_ptr + i >= cur_str_end)
		overflow("str pool", STR_SIZE);
	}


// Allocate a new string of length 'i'.

str tex::new_str(int i)
	{
	str	s;
	
	s = (str) malloc(i+1);
	if (s == (str) 0)
		overflow("new str", i);
	s[i] = NUL;
	
	return	s;
	}

void tex::free_str(str s)
	{
	free(s);
	}


// Fill the ASCII array with pointers to TeX-input representations of
// characters in the range 0-255. Examples: 64->'A' and 157->'^^9d'.
// FIXME: this array is never freed again.

void tex::_str_init_once ()
	{
	int 	k;
	
#define app_lc_hex(L) append_char(((L)<10) ? (L)+'0' : (L)-10+'a')

	cur_str = new_str(STR_SIZE);
	cur_str_end = cur_str + STR_SIZE;
	cur_str_ptr = cur_str;
	for (k = 0; k <= 255; incr(k)) {
		if (k < ' ' || k > '~') {
			append_char('^');
			append_char('^');
			if (k < 0100) {
				append_char(k + 0100);
				} 
			else if (k < 0200) {
				append_char(k - 0100);
				} 
			else {
				app_lc_hex(k / 16);
				app_lc_hex(k % 16);
				}
			} 
		else {
			append_char(k);
			}
		ASCII[k] = make_str();
		}
	null_str = "";
	}
