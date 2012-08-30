
/*
 * $Id: scan.h,v 1.5 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {
	extern	int	cur_val_level;
	extern	int	cur_val;

	enum val_types {
		INT_VAL=0, DIMEN_VAL=1, GLUE_VAL=2,
		MU_VAL=3, IDENT_VAL=4, TOK_VAL=5 };
	
	
#define INPUT_LINE_NO_CODE	3
#define BADNESS_CODE		4
	
	extern	int	radix;
	
#define PLUS_TOKEN		(OTHER_TOKEN + '+')
#define MINUS_TOKEN		(OTHER_TOKEN + '-')
#define ZERO_TOKEN		(OTHER_TOKEN + '0')
#define A_TOKEN			(LETTER_TOKEN + 'A')
#define OTHER_A_TOKEN		(OTHER_TOKEN + 'A')
#define OCTAL_TOKEN		(OTHER_TOKEN + '\'')
#define HEX_TOKEN		(OTHER_TOKEN + '"')
#define ALPHA_TOKEN		(OTHER_TOKEN + '`')
#define POINT_TOKEN		(OTHER_TOKEN + '.')
#define EURO_POINT_TOKEN	(OTHER_TOKEN + ',')
	
#define MAX_DIMEN	07777777777
	
#define scan_normal_dimen()	scan_dimen(FALSE, FALSE, FALSE)

	extern	int	cur_order;

#define scan_optional_space() \
	{get_x_token(); if (cur_cmd != SPACER) back_input();}

#define get_nbx_token() \
	{do get_x_token(); while (cur_cmd == SPACER);}

#define get_nbrx_token() \
	{do get_x_token(); while (cur_cmd == SPACER || cur_cmd == RELAX);}

	void	scan_left_brace();
	void	scan_optional_equals();
	bool	scan_keyword(str);
	void	scan_something_internal(int, bool);
	void	mu_error();
	void	scan_eight_bit_int();
	void	scan_char_num();
	void	scan_four_bit_int();
	void	scan_fifteen_bit_int();
	void	scan_twenty_seven_bit_int();
	void	scan_int();
	void	scan_dimen(bool mu, bool inf, bool shortcut);
	void	scan_glue(int level);
	void	scan_spec(int c, bool three_codes);
	ptr	scan_rule_spec();
	scal  round_decimals(int k);
	void	_scan_init();
	void	_scan_init_once();
};
