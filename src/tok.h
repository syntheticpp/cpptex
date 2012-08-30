
/*
 * $Id: tok.h,v 1.8 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef tok_h_included
#define tok_h_included

#include "textypes.h"
#include "sym.h"
#include "cmd.h"
#include "fileio.h"

class tokeniser {
	public:
		tokeniser(symbols&, fileio&);

	   ptr& token_link(ptr T) { return tex::link(T); };
	   ptr& token(ptr T)      { return tex::info(T); };

		const static int LEFT_BRACE_TOKEN   =0400;
		const static int LEFT_BRACE_LIMIT	=01000;
		const static int RIGHT_BRACE_TOKEN	=01000;
		const static int RIGHT_BRACE_LIMIT	=01400;
		const static int MATH_SHIFT_TOKEN	=01400;
		const static int TAB_TOKEN		      =02000;
		const static int OUT_PARAM_TOKEN		=02400;
		const static int SPACE_TOKEN		   =05040;
		const static int LETTER_TOKEN		   =05400;
		const static int OTHER_TOKEN		   =06000;
		const static int MATCH_TOKEN		   =06400;
		const static int END_MATCH_TOKEN		=07000;
		const static int CS_TOKEN_FLAG		=010000;

		bool         is_sym(int TOK);
		tok          sym2tok(symbols::sym SYM);
		symbols::sym tok2sym(tok TOK);
		bool         end_line_char_active(); 

	   const int END_TEMPLATE_TOKEN;
		const static int NO_EXPAND_FLAG=257;

		tok	cur_tok;
		ptr	cur_align;
		tok	par_tok;
		int	cur_cmd;
		int	cur_chr;
		bool	force_eof;
		int	open_parens;
		int	align_state;
		
		enum scan_status_t { 
			NORMAL=0, SKIPPING=1, DEFINING=2, MATCHING=3,
			ALIGNING=4, ABSORBING=5 };

		scan_status_t	scanner_status;

		ptr	null_list;
		ptr	def_ref;
		ptr	match_toks;
		ptr	align_toks;
		ptr	omit_template;
	
		class input {
			public:
				short	state_field;
				short	type_field;
				union {
						struct {
								symbols::sym  in_cs_field;
								ptr	        start_field;
								ptr	        loc_field;
								ptr	       *param_field;
						} t;
						struct {
								ptr	in_open_field;
								byte	*buf_field;
								byte	*next_field;
								byte	*limit_field;
						} f;
				} obj_field;
		};
		
		input	cur_input;
		input	*input_stack;
		input	*input_end;
		input	*input_ptr;
		input	*base_ptr;
		input	*max_in_stack;
		
		const static int MID_LINE=1;
		const static int NEW_LINE=(3 + 2*commands::MAX_CHAR_CODE);
		const static int SKIP_BLANKS=(2+commands::MAX_CHAR_CODE);

#define file_state	(cur_input.state_field != TOKEN_LIST)

#define index		cur_input.type_field
#define in_file	cur_input.obj_field.f.in_open_field
#define buffer		cur_input.obj_field.f.buf_field
#define next		cur_input.obj_field.f.next_field
#define limit		cur_input.obj_field.f.limit_field

		const static int BUF_SIZE=4096;

#define terminal_input	(index == 0)

		class infile {
			public:
				file	file_field;
				str	name_field;
				int	line_field;
		};
		
#define cur_file	((infile *) in_file)->file_field
#define file_name	((infile *) in_file)->name_field
#define file_line	((infile *) in_file)->line_field
		
		str	name;
		int	line;
		infile	*file_stack;
		infile	*file_end;
		infile	*file_ptr;
		infile	*max_file_stack;
	
#define TOKEN_LIST	0

#define token_type	cur_input.type_field
#define in_cs		   ((sym_t *)cur_input.obj_field.t.in_cs_field)
#define start		   cur_input.obj_field.t.start_field
#define loc		      cur_input.obj_field.t.loc_field
#define param_start	cur_input.obj_field.t.param_field

#define cs_name		text(in_cs)

		enum token_types {
			PARAMETER=0, U_TEMPLATE=1, V_TEMPLATE=2,
			BACKED_UP=3, INSERTED=4, MACRO=5, OUTPUT_TEXT=6,
			EVERY_PAR_TEXT=7, EVERY_MATH_TEXT=8, EVERY_DISPLAY_TEXT=9,
			EVERY_HBOX_TEXT=10, EVERY_VBOX_TEXT=11, EVERY_JOB_TEXT=12,
			EVERY_CR_TEXT=13, MARK_TEXT=14, WRITE_TEXT=15 };

		int	nparams;
		ptr	*param_stack;
		ptr	*param_end;
		ptr	*param_ptr;
		ptr	*max_param_stack;
		

#define new_token	new_avail
#define free_token	free_avail
	
		void back_list(ptr L) { begin_token_list(L, BACKED_UP); };
		void ins_list(ptr L)  { begin_token_list(L, INSERTED); };
		void get_token();
		void get_next();
		void get_cs();
		void check_outer_validity();
		void push_input();
		void pop_input();
		void begin_token_list(ptr p, int t);
		void end_token_list();
		void begin_file_reading();
		void end_file_reading();
		void back_input();
		void back_error();
		void ins_error();
		void runaway();
		void show_context();
		void clear_for_error_prompt();

	private:
		symbols& sym_;
		fileio&  fil_;
		int	   ninputs;
};

#endif
