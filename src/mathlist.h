
/*
 * $Id: mathlist.h,v 1.3 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#define ABOVE_CODE	0
#define OVER_CODE	1
#define ATOP_CODE	2
#define DELIMITED_CODE	3
#define VAR_CODE	070000

namespace tex {
	void	push_math(int c);
	void	init_math();
	void	start_eq_no();
	void	scan_math(ptr p);
	void	set_math_char(int c);
	void	math_limit_switch();
	void	scan_delimiter(ptr p, bool r);
	void	math_radical();
	void	math_ac();
	void	append_choices();
	void	build_choices();
	void	sub_sup();
	void	math_fraction();
	ptr	fin_mlist(ptr p);
	void	math_left_right();
	void	after_math();
	void	check_dollar();
	bool	check_font_params();
	void	resume_after_display();
	void  help_math_accent();
	void  help_math_sy();
	void  help_math_ex();
	void  help_limits();
	void  help_delimiter();
	void  help_fraction();
	void  help_xtra_right();
	void  help_doldol();
	void  help_double_sub();
	void  help_double_sup();
	void	_mathlist_init();
	void	_mathlist_init_once();
};
