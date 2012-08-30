
/*
 * $Id: sym.h,v 1.7 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef sym_h_included
#define sym_h_included

#include "cells.h"

class symbols {
	public:
		class reg_t {
			public:
				short	type_field;
				short	level_field;
				int	equiv_field;
		};
		typedef reg_t * reg;

		class sym_t {
			public:
				reg_t	reg_field;
				int	length_field;
				str	text_field;
				struct sym_t *hash_field;
		};
		typedef sym_t * sym;
		
		int&    length(sym);
		sym_t*& hash(sym);
		
		sym	cur_cs;
		sym	warning_cs;
		sym	par_cs;
		sym	par_shape_cs;
		sym	undefined_cs;
		sym	null_cs;
		sym	*active_base;
		sym	*single_base;
		sym	*hash_base;
		sym	*frozen_cs_base;
		sym	*font_id_base;
		reg	*skip_reg;
		reg	*mu_skip_reg;
		reg	*box_reg;
		reg	*int_reg;
		reg	*dimen_reg;
		reg	*toks_reg;
		reg	*fnt_reg;
		reg	*cat_code_reg;
		reg	*lc_code_reg;
		reg	*uc_code_reg;
		reg	*sf_code_reg;
		reg	*math_code_reg;
		reg	*del_code_reg;
		bool	no_new_control_sequence;
		
		class shape_t {
			public:
				scal	indent_field;
				scal	width_field;
		};
		
		class par_shape_t {
			public:
				tex::mcell 	node_field;
				shape_t	   par_shape_field[1];
		};
	
		int   count(int R);
		sym	id_lookup(str, int);
		sym	new_sym(str, int);
		sym	primitive(str s, int code, int order);
		sym	freeze(sym);
		void	print_cs(str);
		void	sprint_cs(sym);
		bool	issingle(sym s);
		bool	isactive(sym s);
		bool	isfrozen(sym s);
		void	new_save_level(int);
		bool	realloc_save_stack();
		void	reg_destroy(reg);
		void	reg_save(reg r, int l);
		void	eq_define(sym s, int t, int e);
		void	reg_define(reg r, int t, int e);
		void	eq_gdefine(sym s, int t, int e);
		void	reg_gdefine(reg r, int t, int e);
		void	save_for_after(tok t);
		void	unsave();
		void	off_save();
		void	restore_trace(reg r, str s);
		void	show_reg(reg);
		sym  *sym_alloc(int i);
		reg  *reg_alloc(int i);
		void  help_offsave_xtra();
		void  help_offsave_missing();
		void	_sym_init();
		void	_sym_init_once();
		
		static const int FROZEN_CONTROLS=11; 
		sym FROZEN_PROTECTION()   { return frozen_cs_base[0];  }
		sym FROZEN_CR()			  { return frozen_cs_base[1];  }
		sym FROZEN_END_GROUP()	  { return frozen_cs_base[2];  }
		sym FROZEN_RIGHT()		  { return frozen_cs_base[3];  }
		sym FROZEN_FI()			  { return frozen_cs_base[4];  }
		sym FROZEN_END_TEMPLATE() { return frozen_cs_base[5];  }
		sym FROZEN_ENDV()			  { return frozen_cs_base[6];  }
		sym FROZEN_RELAX()		  { return frozen_cs_base[7];  }
		sym FROZEN_END_WRITE()	  { return frozen_cs_base[8];  }
		sym FROZEN_DONT_EXPAND()  { return frozen_cs_base[9];  }
		sym FROZEN_NULL_FONT()	  { return frozen_cs_base[10]; }

		enum mu_glue_codes {
			THIN_MU_SKIP_CODE=0, MED_MU_SKIP_CODE=1, THICK_MU_SKIP_CODE=2 };

		enum glue_codes {
			LINE_SKIP_CODE=0, BASELINE_SKIP_CODE=1, PAR_SKIP_CODE=2,
			ABOVE_DISPLAY_SKIP_CODE=3, BELOW_DISPLAY_SKIP_CODE=4,
			ABOVE_DISPLAY_SHORT_SKIP_CODE=5, BELOW_DISPLAY_SHORT_SKIP_CODE=6,
			LEFT_SKIP_CODE=7, RIGHT_SKIP_CODE=8, TOP_SKIP_CODE=9,
			SPLIT_TOP_SKIP_CODE=10, TAB_SKIP_CODE=11, SPACE_SKIP_CODE=12,
			XSPACE_SKIP_CODE=13, PAR_FILL_SKIP_CODE=14 };

		enum dimen_codes {
			PAR_INDENT_CODE=0, MATH_SURROUND_CODE=1, LINE_SKIP_LIMIT_CODE=2,
			HSIZE_CODE=3, VSIZE_CODE=4, MAX_DEPTH_CODE=5, 
			SPLIT_MAX_DEPTH_CODE=6, BOX_MAX_DEPTH_CODE=7, HFUZZ_CODE=8,
			VFUZZ_CODE=9, DELIMITER_SHORTFALL_CODE=10, NULL_DELIMITER_SPACE_CODE=11,
			SCRIPT_SPACE_CODE=12, PRE_DISPLAY_SIZE_CODE=13, DISPLAY_WIDTH_CODE=14,
			DISPLAY_INDENT_CODE=15, OVERFULL_RULE_CODE=16, HANG_INDENT_CODE=17,
			H_OFFSET_CODE=18, V_OFFSET_CODE=19, EMERGENCY_STRETCH_CODE=20 };
		
		enum int_codes {
			PRETOLERANCE_CODE=0, TOLERANCE_CODE=1, LINE_PENALTY_CODE=2,
			HYPHEN_PENALTY_CODE=3, CLUB_PENALTY_CODE=4, EX_HYPHEN_PENALTY_CODE=5,
			WIDOW_PENALTY_CODE=6, DISPLAY_WIDOW_PENALTY_CODE=7, BROKEN_PENALTY_CODE=8,
			BIN_OP_PENALTY_CODE=9, REL_PENALTY_CODE=10, PRE_DISPLAY_PENALTY_CODE=11,
			POST_DISPLAY_PENALTY_CODE=12, INTER_LINE_PENALTY_CODE=13,
			DOUBLE_HYPHEN_DEMERITS_CODE=14, FINAL_HYPHEN_DEMERITS_CODE=15,
			ADJ_DEMERITS_CODE=16, MAG_CODE=17, DELIMITER_FACTOR_CODE=18,
			LOOSENESS_CODE=19, TIME_CODE=20, DAY_CODE=21, MONTH_CODE=22,
			YEAR_CODE=23, SHOW_BOX_BREADTH_CODE=24, SHOW_BOX_DEPTH_CODE=25,
			HBADNESS_CODE=26, VBADNESS_CODE=27, PAUSING_CODE=28,
			TRACING_ONLINE_CODE=29, TRACING_MACROS_CODE=30, TRACING_STATS_CODE=31,
			TRACING_PARAGRAPHS_CODE=32, TRACING_PAGES_CODE=33, TRACING_OUTPUT_CODE=34,	
			TRACING_LOST_CHARS_CODE=35, TRACING_COMMANDS_CODE=36,
			TRACING_RESTORES_CODE=37, UC_HYPH_CODE=38, OUTPUT_PENALTY_CODE=39,
			MAX_DEAD_CYCLES_CODE=40, HANG_AFTER_CODE=41, FLOATING_PENALTY_CODE=42,
			GLOBAL_DEFS_CODE=43, CUR_FAM_CODE=44, ESCAPE_CHAR_CODE=45,
			DEFAULT_HYPHEN_CHAR_CODE=46, DEFAULT_SKEW_CHAR_CODE=47,
			END_LINE_CHAR_CODE=48, NEW_LINE_CHAR_CODE=49, LANGUAGE_CODE=50,
			LEFT_HYPHEN_MIN_CODE=51, RIGHT_HYPHEN_MIN_CODE=52, 
			HOLDING_INSERTS_CODE=53, ERROR_CONTEXT_LINES_CODE=54 };

		enum tok_codes {
			OUTPUT_ROUTINE_CODE=0, EVERY_PAR_CODE=1, EVERY_MATH_CODE=2,
			EVERY_DISPLAY_CODE=3, EVERY_HBOX_CODE=4, EVERY_VBOX_CODE=5,
			EVERY_JOB_CODE=6, EVERY_CR_CODE=7, ERR_HELP_CODE=8 };

		int& mu_glue_par(mu_glue_codes D) { return mu_skip_reg[D]->equiv_field; };
		int& glue_par(glue_codes D)       { return skip_reg[D]->equiv_field; };
		int& dimen_par(dimen_codes D)     { return dimen_reg[D]->equiv_field; };
		int& int_par(int_codes D)         { return int_reg[D]->equiv_field; };
      int& tok_par(tok_codes D)			 { return toks_reg[D]->equiv_field; };

		int& cat_code(int R) { return cat_code_reg[R]->equiv_field; };
		int& lc_code(int R)  { return lc_code_reg[R]->equiv_field; };
		int& uc_code(int R)  { return uc_code_reg[R]->equiv_field; };
		int& sf_code(int R)  { return sf_code_reg[R]->equiv_field; };
		int& math_code(int R){ return math_code_reg[R]->equiv_field; };
		int& del_code(int R) { return del_code_reg[R]->equiv_field; };

		reg    sym2reg(sym S)   { return ((reg) & ((sym)(S))->reg_field); };
		short& reg_level(reg R) { return ((reg) (R))->level_field; };
		short& reg_type(reg R)  { return ((reg) (R))->type_field; };
		int&   reg_equiv(reg R) { return	((R))->equiv_field; };
		short& eq_level(sym S)  { return reg_level(sym2reg(S)); };
		short& eq_type(sym S)	{ return reg_type(sym2reg(S)); };
		sym    reg2sym(reg R)   { return	((sym) R); };
		int&   equiv(sym S)     { return reg_equiv(sym2reg(S)); };
		str    text(sym S)      { return ((sym) (S))->text_field; };

		// Convenience functions to extract the equivalence fields
		int& pretolerance() { return int_par(PRETOLERANCE_CODE); }
		int& tolerance() { return int_par(TOLERANCE_CODE); }
		int& line_penalty() { return int_par(LINE_PENALTY_CODE); }
		int& hyphen_penalty() { return int_par(HYPHEN_PENALTY_CODE); }
		int& ex_hyphen_penalty() { return int_par(EX_HYPHEN_PENALTY_CODE); }
		int& club_penalty() { return int_par(CLUB_PENALTY_CODE); }
		int& widow_penalty() { return int_par(WIDOW_PENALTY_CODE); }
		int& display_widow_penalty() { return int_par(DISPLAY_WIDOW_PENALTY_CODE); }
		int& broken_penalty() { return int_par(BROKEN_PENALTY_CODE); }
		int& bin_op_penalty() { return int_par(BIN_OP_PENALTY_CODE); }
		int& rel_penalty() { return int_par(REL_PENALTY_CODE); }
		int& pre_display_penalty() { return int_par(PRE_DISPLAY_PENALTY_CODE); }
		int& post_display_penalty() { return int_par(POST_DISPLAY_PENALTY_CODE); }
		int& inter_line_penalty() { return int_par(INTER_LINE_PENALTY_CODE); }
		int& double_hyphen_demerits() { return int_par(DOUBLE_HYPHEN_DEMERITS_CODE); }
		int& final_hyphen_demerits() { return int_par(FINAL_HYPHEN_DEMERITS_CODE); }
		int& adj_demerits() { return int_par(ADJ_DEMERITS_CODE); }
		int& mag() { return int_par(MAG_CODE); }
		int& delimiter_factor() { return int_par(DELIMITER_FACTOR_CODE); }
		int& looseness() { return int_par(LOOSENESS_CODE); }
		int& time() { return int_par(TIME_CODE); }
		int& day() { return int_par(DAY_CODE); }
		int& month() { return int_par(MONTH_CODE); }
		int& year() { return int_par(YEAR_CODE); }
		int& show_box_breadth() { return int_par(SHOW_BOX_BREADTH_CODE); }
		int& show_box_depth() { return int_par(SHOW_BOX_DEPTH_CODE); }
		int& hbadness() { return int_par(HBADNESS_CODE); }
		int& vbadness() { return int_par(VBADNESS_CODE); }
		int& pausing() { return int_par(PAUSING_CODE); }
		int& tracing_online() { return int_par(TRACING_ONLINE_CODE); }
		int& tracing_macros() { return int_par(TRACING_MACROS_CODE); }
		int& tracing_stats() { return int_par(TRACING_STATS_CODE); }
		int& tracing_paragraphs() { return int_par(TRACING_PARAGRAPHS_CODE); }
		int& tracing_pages() { return int_par(TRACING_PAGES_CODE); }
		int& tracing_output() { return int_par(TRACING_OUTPUT_CODE); }
		int& tracing_lost_chars() { return int_par(TRACING_LOST_CHARS_CODE); }
		int& tracing_commands() { return int_par(TRACING_COMMANDS_CODE); }
		int& tracing_restores() { return int_par(TRACING_RESTORES_CODE); }
		int& uc_hyph() { return int_par(UC_HYPH_CODE); }
		int& max_dead_cycles() { return int_par(MAX_DEAD_CYCLES_CODE); }
		int& output_penalty() { return int_par(OUTPUT_PENALTY_CODE); }
		int& hang_after() { return int_par(HANG_AFTER_CODE); }
		int& floating_penalty() { return int_par(FLOATING_PENALTY_CODE); }
		int& global_defs() { return int_par(GLOBAL_DEFS_CODE); }
		int& cur_fam() { return int_par(CUR_FAM_CODE); }
		int& escape_char() { return int_par(ESCAPE_CHAR_CODE); }
		int& default_hyphen_char() { return int_par(DEFAULT_HYPHEN_CHAR_CODE); }
		int& default_skew_char() { return int_par(DEFAULT_SKEW_CHAR_CODE); }
		int& end_line_char() { return int_par(END_LINE_CHAR_CODE); }
		int& new_line_char() { return int_par(NEW_LINE_CHAR_CODE); }
		int& language() { return int_par(LANGUAGE_CODE); }
		int& left_hyphen_min() { return int_par(LEFT_HYPHEN_MIN_CODE); }
		int& right_hyphen_min() { return int_par(RIGHT_HYPHEN_MIN_CODE); }
		int& holding_inserts() { return int_par(HOLDING_INSERTS_CODE); }
		int& error_context_lines() { return int_par(ERROR_CONTEXT_LINES_CODE); }
		
		int& thin_mu_skip() { return mu_glue_par(THIN_MU_SKIP_CODE); }
		int& med_mu_skip() { return mu_glue_par(MED_MU_SKIP_CODE); }
		int& thick_mu_skip() { return mu_glue_par(THICK_MU_SKIP_CODE); }
		
		int& line_skip() { return glue_par(LINE_SKIP_CODE); }
		int& baseline_skip() { return glue_par(BASELINE_SKIP_CODE); }
		int& par_skip() { return glue_par(PAR_SKIP_CODE); }
		int& above_display_skip() { return glue_par(ABOVE_DISPLAY_SKIP_CODE); }
		int& below_display_skip() { return glue_par(BELOW_DISPLAY_SKIP_CODE); }
		int& above_display_short_skip() { return glue_par(ABOVE_DISPLAY_SHORT_SKIP_CODE); }
		int& below_display_short_skip() { return glue_par(BELOW_DISPLAY_SHORT_SKIP_CODE); }
		int& left_skip() { return glue_par(LEFT_SKIP_CODE); }
		int& right_skip() { return glue_par(RIGHT_SKIP_CODE); }
		int& top_skip() { return glue_par(TOP_SKIP_CODE); }
		int& split_top_skip() { return glue_par(SPLIT_TOP_SKIP_CODE); }
		int& tab_skip() { return glue_par(TAB_SKIP_CODE); }
		int& space_skip() { return glue_par(SPACE_SKIP_CODE); }
		int& xspace_skip() { return glue_par(XSPACE_SKIP_CODE); }
		int& par_fill_skip() { return glue_par(PAR_FILL_SKIP_CODE); }
		
		int& par_indent() { return dimen_par(PAR_INDENT_CODE); }
		int& math_surround() { return dimen_par(MATH_SURROUND_CODE); }
		int& line_skip_limit() { return dimen_par(LINE_SKIP_LIMIT_CODE); }
		int& hsize() { return dimen_par(HSIZE_CODE); }
		int& vsize() { return dimen_par(VSIZE_CODE); }
		int& max_depth() { return dimen_par(MAX_DEPTH_CODE); }
		int& split_max_depth() { return dimen_par(SPLIT_MAX_DEPTH_CODE); }
		int& box_max_depth() { return dimen_par(BOX_MAX_DEPTH_CODE); }
		int& hfuzz() { return dimen_par(HFUZZ_CODE); }
		int& vfuzz() { return dimen_par(VFUZZ_CODE); }
		int& delimiter_shortfall() { return dimen_par(DELIMITER_SHORTFALL_CODE); }
		int& null_delimiter_space() { return dimen_par(NULL_DELIMITER_SPACE_CODE); }
		int& script_space() { return dimen_par(SCRIPT_SPACE_CODE); }
		int& pre_display_size() { return dimen_par(PRE_DISPLAY_SIZE_CODE); }
		int& display_width() { return dimen_par(DISPLAY_WIDTH_CODE); }
		int& display_indent() { return dimen_par(DISPLAY_INDENT_CODE); }
		int& overfull_rule() { return dimen_par(OVERFULL_RULE_CODE); }
		int& hang_indent() { return dimen_par(HANG_INDENT_CODE); }
		int& h_offset() { return dimen_par(H_OFFSET_CODE); }
		int& v_offset() { return dimen_par(V_OFFSET_CODE); }
		int& emergency_stretch() { return dimen_par(EMERGENCY_STRETCH_CODE); }
		
		int& output_routine() { return tok_par(OUTPUT_ROUTINE_CODE); }
		int& every_par() { return tok_par(EVERY_PAR_CODE); }
		int& every_math() { return tok_par(EVERY_MATH_CODE); }
		int& every_display() { return tok_par(EVERY_DISPLAY_CODE); }
		int& every_hbox() { return tok_par(EVERY_HBOX_CODE); }
		int& every_vbox() { return tok_par(EVERY_VBOX_CODE); }
		int& every_job() { return tok_par(EVERY_JOB_CODE); }
		int& every_cr() { return tok_par(EVERY_CR_CODE); }
		int& err_help() { return tok_par(ERR_HELP_CODE); }

	private:
		ptr	hold_head;
		ptr	zero_glue;
		ptr	fil_glue;
		ptr 	fill_glue;
		ptr	ss_glue;
		ptr	fil_neg_glue;
		int	cs_count;
		int	cur_level;
		int	cur_group;
		int	nsaves;
		reg	save_stack;
		reg	save_end;
		reg	save_ptr;
		reg	max_save_stack;
};

//#define hash(S)			((sym) (S))->hash_field

#define font_id(F)		font_id_base[F]
#define font_id_text(F)		text(font_id_base[F])
#define font_id_length(F)	font_id_base[F]->length_field

#define MU_GLUE_PARS			3

#define GLUE_PARS			15


#define DIMEN_PARS			21
//#define dimen_par(D)			((scal) reg_equiv(dimen_reg[D]))




#define INT_PARS			55
//#define int_par(I)			((int) reg_equiv(int_reg[I]))


#define TOK_PARS			9



#define cur_font_ptr		fnt_reg[0]
#define cur_font		   reg_equiv(cur_font_ptr)
#define fam_fnt(R)		reg_equiv(fnt_reg[(R)+1])

#define mu_skip(R)		reg_equiv(tex::mu_skip_reg[MU_GLUE_PARS + (R)])
#define skip(R)			reg_equiv(tex::skip_reg[GLUE_PARS + (R)])
#define dimen(R)		   reg_equiv(tex::dimen_reg[DIMEN_PARS + (R)])
//#define count(R)		   reg_equiv(int_reg[INT_PARS + (R)])

#define toks(R)			reg_equiv(toks_reg[TOK_PARS + (R)])
#define box(R)			   reg_equiv(box_reg[R])

#define output_routine_reg toks_reg[OUTPUT_ROUTINE_CODE]

#define CAT_CODE		   0
#define LC_CODE			1
#define UC_CODE			2
#define SF_CODE			3
#define MATH_CODE		   4
#define DEL_CODE	 	   5


#define par_shape_ptr	equiv(par_shape_cs)


#define par_shape	((par_shape_t *) par_shape_ptr)->par_shape_field

#define par_shape_indent(I)	par_shape[I - 1].indent_field
#define par_shape_width(I)	par_shape[I - 1].width_field

// #define save_type	reg_type
// #define save_level	reg_level
// #define save_index	reg_equiv

#define saved(I)	reg_equiv(save_ptr+I)

#define RESTORE_OLD_VALUE		0
#define INSERT_TOKEN			1
#define LEVEL_BOUNDARY			2

#define LEVEL_ZERO			MIN_QUARTERWORD
#define LEVEL_ONE			(LEVEL_ZERO + 1)
#define BOTTOM_LEVEL			0
#define SIMPLE_GROUP			1
#define HBOX_GROUP			2
#define ADJUSTED_HBOX_GROUP		3
#define VBOX_GROUP			4
#define VTOP_GROUP			5
#define ALIGN_GROUP			6
#define NO_ALIGN_GROUP			7
#define OUTPUT_GROUP			8
#define MATH_GROUP			9
#define DISC_GROUP			10
#define INSERT_GROUP			11
#define VCENTER_GROUP			12
#define MATH_CHOICE_GROUP		13
#define SEMI_SIMPLE_GROUP		14
#define MATH_SHIFT_GROUP		15
#define MATH_LEFT_GROUP			16
#define MAX_GROUP_CODE			16


#endif
