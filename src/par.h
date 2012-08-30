
/*
 * $Id: par.h,v 1.5 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {

	class passive_t {
		public:
			mcell	node_field;
			ptr	cur_break_field;
			ptr	prev_break_field;
	};

#define PASSIVE_NODE_SIZE	sizeof(passive_t)
#define serial			info
#define cur_break(P)		((passive_t *) (P))->cur_break_field
#define prev_break(P)		((passive_t *) (P))->prev_break_field
#define next_break		prev_break

	extern	ptr	passive;

	class active_t {
		public:
			mcell	node_field;
			ptr	break_node_field;
			int	line_number_field;
			int	total_demerits_field;
	};

#define ACTIVE_NODE_SIZE	sizeof(active_t)
#define UNHYPHENATED		0
#define HYPHENATED		1
#define fitness			subtype
#define VERY_LOOSE_FIT		0
#define LOOSE_FIT		1
#define DECENT_FIT		2
#define TIGHT_FIT		3
#define break_node(P)		((active_t *) (P))->break_node_field
#define line_number(P)		((active_t *) (P))->line_number_field
#define total_demerits(P)	((active_t *) (P))->total_demerits_field

	extern	ptr	active;
#define last_active		active
	
#define DELTA_NODE		2

	class delta_t {
		public:
			mcell	node_field;
			scal	delta_field[7];
	};

#define DELTA_NODE_SIZE		sizeof(delta_t)
#define deltas(P)		((delta_t *) (P))->delta_field

#define do_all_six(F) \
	{F(1); F(2); F(3); F(4); F(5); F(6);}

	extern	ptr	just_box;
	
	extern	ptr	printed_node;
	extern	int	pass_number;
	
	extern	scal	active_width[];
	extern	scal	cur_active_width[];
	extern	scal	background[];
	extern	scal	break_width[];
	extern	bool	no_shrink_error_yet;
	
	extern	ptr	cur_p;
	extern	bool	second_pass;
	extern	bool	final_pass;
	extern	int	threshold;
	
#define AWFUL_BAD		07777777777
	
	extern	int	fit_class;
	extern	int	minimal_demerits[];
	extern	int	minimum_demerits;
	extern	ptr	best_place[];
	extern	int	best_pl_line[];
	
	extern	int	easy_line;
	extern	int	last_special_line;
	extern	scal	first_width;
	extern	scal	second_width;
	extern	scal	first_indent;
	extern	scal	second_indent;
	extern	scal	line_width;
	extern	scal	disc_width;
	extern	ptr	best_bet;
	extern	int	fewest_demerits;
	extern	int	best_line;
	extern	int	line_diff;
	extern	int	actual_looseness;
	
	void	line_break(int final_widow_penalty);
	ptr	finite_shrink(ptr p);
	void	try_break(int pi, int break_type);
	void	post_line_break(ptr p, int final_widow_penalty);
	void	set_disc_width();
	void	set_act_width();
	void	set_break_width(int break_type);
	int	get_best_bet();
	int	get_badness();
	ptr	get_break_node(int fit_class, int break_type);
	void	show_break_node(ptr p, int fit_class, int break_type);
	void	show_break_status(ptr r, bool a, int b, int p, int d);
	void	update_printed_node();
	void  help_shrink();
	void	_par_init();
	void	_par_init_once();
};
