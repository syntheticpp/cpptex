
/*
 * $Id: toklist.h,v 1.4 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {
#define NUMBER_CODE		0
#define ROMAN_NUMERAL_CODE	1
#define STRING_CODE		2
#define MEANING_CODE		3
#define FONT_NAME_CODE		4
#define JOB_NAME_CODE		5

#define store_new_token(T) \
	{q = new_token(); token_link(p) = q; token(q) = T; p = q;}

#define token_ref_count(T)	token(T)
#define add_token_ref(T) incr(token_ref_count(T))
#define delete_token_ref(T) \
	{if (token_ref_count(T) == 0) \
		flush_list(T); \
	else decr(token_ref_count(T));}

	ptr	str_toks();
	ptr	the_toks();
	void	conv_toks();
	void	scan_toks(bool, bool);
	void	read_toks(int, sym);
	void	print_meaning();
	void	token_show(ptr p);
	void	show_token_list(ptr p, ptr q, int l);
	void	flush_list(ptr p);
	void help_param_num();
	void help_param_count();
	void help_left_brace();
	void help_param_use();
	void help_read();
	void	_toklist_init();
	void	_toklist_init_once();
};
