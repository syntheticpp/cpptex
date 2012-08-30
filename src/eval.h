
/*
 * $Id: eval.h,v 1.5 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002,2003      Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 *
 * 
 */

#ifndef eval_h_included
#define eval_h_included

#include "cmd.h"

class evaluator {
	public:
		enum eval_modes { VMODE=1, 
								HMODE=VMODE+commands::MAX_COMMAND+1,
								MMODE=HMODE+commands::MAX_COMMAND+1 };

		ptr	lig_stack;
		int	cur_l;
		int	cur_r;
		ptr	cur_q;
		bool	lft_hit;
		bool	rt_hit;
		bool	ligature_present;
		
		void	main_control();
		void	app_space();
		void	push_nest();
		void	pop_nest();
		bool	realloc_nest();
		void	print_mode(int);

#define IGNORE_DEPTH	-65536000

		class list {
			public:
				int	mode;
				ptr	head;
				ptr	tail;
				int	pg;
				int	aux;
				int	ml;
				int	clang;
				short	lhm;
				short	rhm;
		};
		
		list	cur_list;
		list	*nest;
		list	*nest_end;
		list	*nest_ptr;
		list	*max_nest_stack;

		
//#define mode			cur_list.mode_field
//#define head			cur_list.head_field
//#define tail			cur_list.tail_field
//#define prev_graf		cur_list.pg_field
//#define aux			cur_list.aux_field
//#define prev_depth		aux
//#define space_factor		aux
//#define incompleat_noad		aux
//#define mode_line		cur_list.ml_field
//#define clang			cur_list.clang_field
//#define lhmin			cur_list.lhm_field
//#define rhmin			cur_list.rhm_field

		int		shown_mode;
		
		void	show_activities();
		
#define tail_append(N)		{tail = link(tail) = N;}
		
		void	insert_dollar_sign();
		void	you_cant();
		void	report_illegal_case();
		bool	privileged();
		void	missing_font();
		bool	its_all_over();
		void   help_dollar();
		void   help_illegal_case();
		void	_eval_init();
		void	_eval_init_once();
};

#endif
