
/*
 * $Id: alloc.h,v 1.5 2003/09/14 13:30:18 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef alloc_h_included
#define alloc_h_included

#include "textypes.h"

class alloc {
	public:
#define null 0
	
		ptr	avail;
		ptr	rover;
		
		void  fast_new_avail(ptr M);
		void  free_avail(ptr M);
		ptr	new_avail();
		ptr	new_node(int s);
		void	free_node(ptr p, int s);
		void	_alloc_init_once();
		
		class blk_t {
			public:
				int	size_field;
				ptr	free_field;
				ptr	rblk_field;
				ptr	lblk_field;
		};
		
	private:
		ptr  avail_ptr;
		ptr  avail_end;
		ptr  nil;
};

#endif
