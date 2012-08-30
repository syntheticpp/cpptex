
/*
 * $Id: chr.c,v 1.3 2003/09/14 13:30:18 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include "tex.h"

byte	tex::xord[256];
byte	tex::xchr[256];

void tex::_chr_init()
	{
	int	i;
	
	for (i = 0; i <= 0377; incr(i))
	  xchr[i] = i;
	for (i = 0; i <= 0377; incr(i))
	  xord[xchr[i]] = i;
	}

