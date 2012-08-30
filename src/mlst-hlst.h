
/*
 * $Id: mlst-hlst.h,v 1.4 2003/09/10 15:46:55 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {
	extern	int	cur_c;
	extern	fnt	cur_f;
	extern	qcell	cur_i;
	extern	scal	cur_mu;
	extern	int	cur_size;

	ptr	mlist_to_hlist(ptr mlist, int style, bool penalties);
	void	make_over(ptr q);
	void	make_under(ptr q);
	void	make_vcenter(ptr q);
	void	make_radical(ptr q);
	void	make_math_accent(ptr q);
	void	make_fraction(ptr q);
	scal	make_op(ptr q);
	void	make_ord(ptr q);
	void	make_scripts(ptr q, scal delta);
	int	make_left_right(ptr q, int style, scal max_d, scal max_h);
	void	fetch(ptr a);
	ptr	clean_box(ptr p, int style);
	void  help_undefd_mathchar();
	void	_mlst_hlst_init();
	void	_mlst_hlst_init_once();
};
