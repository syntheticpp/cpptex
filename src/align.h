
/*
 * $Id: align.h,v 1.6 2003/09/14 13:30:18 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef align_h_included
#define align_h_included

#include "textypes.h"
#include "alloc.h"
#include "tok.h"
#include "eval.h"
#include "cmd.h"

class align {
	public:
		align(alloc&, tokeniser&, symbols&, evaluator&, commands&);

		class align_stack_t {
			public:
				struct align_stack_t *align_stack_field;
				ptr	preamble_field;
				ptr	cur_align_field;
				ptr	cur_span_field;
				ptr	cur_loop_field;
				ptr	cur_tail_field;
				ptr	cur_head_field;
				int	align_state_field;
		};
		class span_t {
			public:
				int	span_count_field;
				ptr	span_link_field;
				scal	span_width_field;
				ptr	fill_field;
		};
		
		align_stack_t *align_stack;

		ptr	align_head;
		ptr	cur_span;
		ptr	cur_loop;
		ptr	cur_head;
		ptr	cur_tail;
		ptr	end_span;		
		
#define span_count(P)	((span_t *) (P))->span_count_field
#define span_link(P)		((span_t *) (P))->span_link_field
#define span_width(P)	((span_t *) (P))->span_width_field

		const static int SPAN_CODE=256;
		const static int CR_CODE=257;
		const static int CR_CR_CODE=CR_CODE+1;
		
		void	push_alignment();
		void	pop_alignment();
		void	init_align();
		void	get_preamble_token();
		void	align_peek();
		void	init_row();
		void	init_span(ptr p);
		void	init_col();
		bool	fin_col();
		void	fin_row();
		void	fin_align();

	private:
		alloc&     alloc_;
		tokeniser& tok_;
		symbols&   syms_;
		evaluator& eval_;
		commands&  cmd_;
};

#endif
