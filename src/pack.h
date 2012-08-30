
/*
 * $Id: pack.h,v 1.6 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002,2003      Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {
	enum pack_type { EXACTLY=0, ADDITIONAL=1 };

	extern ptr	adjust_head;
	extern ptr	adjust_tail;
	extern scal	total_stretch[];
	extern scal	total_shrink[];
	extern int	last_badness;
	extern int	pack_begin_line;

	int   get_stretch_order();
	int   get_shrink_order();
	void  clr_dimens();
	ptr	hpack(ptr p, scal w, int m);
	ptr   vpack(ptr p, scal h, int m);
	ptr	vpackage(ptr p, scal h, int m, scal l);
	void	_pack_init();
	void	_pack_init_once();
}

