
/*
 * $Id: cond.h,v 1.4 2003/09/10 15:46:55 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002,2003      Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {

	extern	ptr	cond_ptr;
	extern	int	cur_if;
	extern	int	if_limit;
	extern	int	if_line;
	extern	int	skip_line;
	
	class if_t {
		public:
			mcell	node;
			int	line_field;
			ptr	fill_field;
	};
	
	enum if_codes {
		IF_CODE=1, FI_CODE=2, ELSE_CODE=3, OR_CODE=4 };

	enum ifwhat_codes {
		IF_CHAR_CODE=	0, IF_CAT_CODE=	1,  IF_INT_CODE=	2,
		IF_DIM_CODE=	3, IF_ODD_CODE=	4,  IF_VMODE_CODE=	5,
		IF_HMODE_CODE=	6, IF_MMODE_CODE=	7,  IF_INNER_CODE=	8,
		IF_VOID_CODE=	9, IF_HBOX_CODE=	10, IF_VBOX_CODE=	11,
		IFX_CODE=	  12, IF_EOF_CODE=	13, IF_TRUE_CODE=	14,
		IF_FALSE_CODE=15, IF_CASE_CODE  =16 };
		
	void	conditional();
	void	push_cond();
	void	pop_cond();
	void	pass_text();
	void	change_if_limit(int l, ptr p);
	void  help_or();
	void  help_relation();
	void	_cond_init();
	void	_cond_init_once();

	int&  if_line_field(ptr);
};
