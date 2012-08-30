
/*
 * $Id: boxlist.h,v 1.5 2003/09/14 13:30:18 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef boxlist_h_included
#define boxlist_h_included

#define BOX_FLAG	010000000000
#define SHIP_OUT_FLAG	(BOX_FLAG + 512)
#define LEADER_FLAG	(BOX_FLAG + 513)

namespace tex {
	extern	ptr	cur_box;

	enum glue_codes {
		FIL_CODE=0, FILL_CODE=1, SS_CODE=2,
		FIL_NEG_CODE=3, SKIP_CODE=4, MSKIP_CODE=5 };

	enum char_codes {
		BOX_CODE=0, COPY_CODE=1, LAST_BOX_CODE=2, 
		VSPLIT_CODE=3, VTOP_CODE=4 };

	void	handle_right_brace();
	void	extra_right_brace();
	void	append_glue();
	void	append_kern();
	void	scan_box(int box_context);
	void	begin_box(int box_context);
	void	box_end(int box_context);
	void	package(int c);
	void	normal_paragraph();
	void	indent_in_hmode();
	int	norm_min(int h);
	void	new_graf(bool indented);
	void	end_graf();
	void	head_for_vmode();
	void	append_to_vlist(ptr b);
	void	begin_insert_or_adjust();
	void	make_mark();
	void	append_penalty();
	void	delete_last();
	void	unpackage();
	void	append_italic_correction();
	void	append_discretionary();
	void	build_discretionary();
	void	flush_discretionary(ptr p);
	void	make_accent();
	void	align_error();
	void	no_align_error();
	void	omit_error();
	void	do_endv();
	void	cs_error();
	
	void	_boxlist_init();
	void	_boxlist_init_once();
};

#endif
