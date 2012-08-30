
/*
 * $Id: def.h,v 1.4 2003/09/10 15:46:55 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002,2003      Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {

	extern	tok	after_token;
	extern	int	mag_set;
	extern	bool	long_help_seen;

	enum def_codes {
		CHAR_DEF_CODE=0, MATH_CHAR_DEF_CODE=1, COUNT_DEF_CODE=2,
		DIMEN_DEF_CODE=3, SKIP_DEF_CODE=4, MU_SKIP_DEF_CODE=5,
		TOKS_DEF_CODE=6 };


#define WD_CODE			0
#define HT_CODE			1
#define DP_CODE			2

#define SHOW_CODE		0
#define SHOW_BOX_CODE		1
#define SHOW_THE_CODE		2
#define SHOW_LISTS		3

	void	get_r_token();
	void	prefixed_command();
	void	trap_zero_glue();
	void	do_register_command(int a);
	void	alter_aux();
	void	alter_prev_graf();
	void	alter_page_so_far();
	void	alter_integer();
	void	alter_box_dimen();
	void	new_font(int);
	void	prepare_mag();
	void	new_interaction();
	void	do_assignments();
	void	clopen_stream();
	void	issue_message();
	void	give_err_help();
	void	shift_case();
	void	show_whatever();
	void	_def_init();
	void	_def_init_once();
	
	void help_missing_cs();
	void help_prefix();
	void help_pref();
	void help_read_to();
	void help_code();
	void help_register();
	void help_space_factor();
	void help_prevgraf();
	void help_overflow();
	void help_font_at();
	void help_font_magnification();
	void help_mag();
	void help_ill_mag();
	void help_err_msg();
	void help_poirot();
	void help_show_online();
	void help_show();
};

