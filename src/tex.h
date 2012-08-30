
/*
 * $Id: tex.h,v 1.16 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef tex_h_included
#define tex_h_included

#include <stdint.h>
#include	<stdio.h>
#include	<string.h>
#include	<signal.h>
#include	<time.h>
#include	<malloc.h>



// Objects in the TeX namespace

namespace tex {
	extern char *banner;

	class override_t {
		public:
			const char *dimname;
			int32_t     value;
			int32_t     reg;
			bool        override_always;
			bool        addsubtoo;
	};
	class override_internal_t {
		public:
			int32_t   code;
			int32_t   value;
			bool      done_once;
			bool      override_always;
	};
	
	extern override_t override[];
	extern override_internal_t override_internal[];
	extern const unsigned int num_override;
	extern const unsigned int num_override_internal;
	extern bool append_to_vlist_called;
	
	extern bool display_debug;
	extern bool display_warnings;
	extern bool display_progress;
	extern bool display_messages;
	extern bool compatible;
	
	void	initialize();
	void	initialize_once();
	bool	decode_args(int, int, char **);
	void	fix_date_and_time();
	void	handle_int(int);
	void	final_cleanup();
	void	close_files();
};



#define mcopy(x,y)	*(tex::mcell *)x = *(tex::mcell *)y
#define mzero(m)	memset((void *)m,0,sizeof(mcell))




// Include all the other header files.

#include "align.h"
#include "alloc.h"
#include "arith.h"
#include "box.h"
#include "boxlist.h"
#include "chr.h"
#include "cmd.h"
#include "cond.h"
#include "def.h"
#include "dvi.h"
#include "sym.h"
#include "error.h"
#include "eval.h"
#include "expand.h"
#include "fileio.h"
#include "hyph.h"
#include "math.h"
#include "mathlist.h"
#include "mlst-hlst.h"
#include "pack.h"
#include "page.h"
#include "par.h"
#include "print.h"
#include "scan.h"
#include "str.h"
#include "tfm.h"
#include "tok.h"
#include "toklist.h"
#include "texext.h"

#endif
