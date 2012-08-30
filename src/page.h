
/*
 * $Id: page.h,v 1.5 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {
	extern	ptr	contrib_head;

#define contrib_tail	 nest[0].tail_field

	extern	ptr	page_tail;
	extern	ptr	page_head;
	extern	ptr	page_ins_head;
	extern	int	page_contents;
	
#define INSERTS_ONLY	1
#define BOX_THERE	2

#define active_height	active_width
#define cur_height	active_height[1]


	extern	scal	best_height_plus_depth;

#define DEPLORABLE	100000

	extern	ptr	best_page_break;
	extern	scal	best_size;
	extern	scal	page_max_depth;
	extern	int	least_page_cost;
	
	class page_ins_t {
		public:
			mcell	node_field;
			ptr	broken_ptr_field;
			ptr	broken_ins_field;
			ptr	last_ins_ptr_field;
			ptr	best_ins_ptr_field;
			scal	page_ins_height_field;
	};

#define PAGE_INS_NODE_SIZE	sizeof(page_ins_t)
#define INSERTING		0
#define SPLIT_UP		1
#define broken_ptr(P)		((page_ins_t *) (P))->broken_ptr_field
#define broken_ins(P)		((page_ins_t *) (P))->broken_ins_field
#define last_ins_ptr(P)		((page_ins_t *) (P))->last_ins_ptr_field
#define best_ins_ptr(P)		((page_ins_t *) (P))->best_ins_ptr_field
#define page_ins_height(P)	((page_ins_t *) (P))->page_ins_height_field

	extern	scal	page_so_far[];

#define page_goal	page_so_far[0]
#define page_total	page_so_far[1]
#define page_shrink	page_so_far[6]
#define page_depth	page_so_far[7]

	extern	ptr	last_glue;
	extern	int	last_penalty;
	extern	scal	last_kern;
	extern	int	insert_penalties;
	
#define start_new_page() \
	{page_contents = EMPTY; \
	page_tail = page_head; \
	link(page_head) = null; \
	last_glue = null; \
	last_penalty = 0; \
	last_kern = 0; \
	page_depth = 0; \
	page_max_depth = 0;}

	extern	bool	output_active;
	
	void	build_page();
	void	insert_page(ptr p);
	ptr	vert_break(ptr p, scal h, scal d);
	ptr	vsplit(int n, scal h);
	ptr	prune_page_top(ptr p);
	int	page_badness();
	int	vert_badness(int);
	void	freeze_page_specs(int s);
	void	box_error(int n);
	void	ensure_vbox(int n);
	void  print_plus(int s, str o);
	void	print_totals();
	void	show_page_stats(int b, int pi, int c);
	void	show_split(int n, scal w, ptr q);
	void	fire_up(ptr p);
	bool	insert_box(ptr p);
	void  show_cur_page();
	void  help_tut();
	void  help_vsplit_vbox();
	void  help_inf_shrink_ins();
	void  help_inf_shrink_box();
	void  help_inf_shrink_page();
	void  help_box_255();
	void  help_dead_cycles();
	void	_page_init();
	void	_page_init_once();
};
