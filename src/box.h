
/*
 * $Id: box.h,v 1.6 2003/09/23 08:08:36 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef box_h_included
#define box_h_included

namespace tex{
	class box_base_t;
	typedef box_base_t *ptr;
};

#include "textypes.h"
#include "cells.h"

#define is_char_node(P)		((P) != null && tex::type(P) >= FONT_BASE)
#define safe_is_char_node(P)	(tex::type(P) >= FONT_BASE)

#define font			tex::type
#define character		tex::subtype

#define HLIST_NODE		0
#define VLIST_NODE		1


namespace tex {
	class box_base_t {
		public:
			mcell node_field;
	};

	class box_t : public box_base_t {
		public:
			box_t();
			scal	width_field;
			scal	depth_field;
			scal	height_field;
			scal	shift_field;
			mcell	node_list_field;
			ratio	glue_set_field;
			void  show();
	};
	
	class rule_t : public box_base_t {
		public:
			rule_t();
			scal	width_field;
			scal	depth_field;
			scal	height_field;
			void  show();
	};
	
	class ins_t : public box_base_t {
		public:
			int	cost_field;
			scal	height_field;
			scal	depth_field;
			ptr	ins_field;
			ptr	split_top_field;
			void  show();
	};
	
	class mark_t : public box_base_t {
		public:
			ptr	mark_field;
			int	fill_field;
			void  show();
	};
	
	class adjust_t : public box_base_t {
		public:
			ptr	adjust_field;
			int	fill_field;
			void  show();
	};
	
	class lig_t : public box_base_t {
		public:
			lig_t(int c);
			lig_t(int f, int c, ptr q);
			mcell	lig_field;
			void  show();
	};
	
	class disc_t : public box_base_t {
		public:
			disc_t();
			ptr	pre_field;
			ptr	post_field;
			void  show();
	};
	
	class math_t : public box_base_t {
		public:
			math_t(scal w, int s);
			scal	width_field;
			int	fill_field;
			void  show();
	};
	
	class glue_t : public box_base_t {
		public:
			glue_t(int n);
			glue_t(glue_t *q);
			ptr	glue_field;
			ptr	leader_field;
			void  show();
	};
	
	class spec_t {
		public:
			spec_t(box_base_t *p);
			int	ref_count_field;
			short	stretch_order_field;
			short	shrink_order_field;
			scal	width_field;
			scal	stretch_field;
			scal	shrink_field;
	};
	
	class kern_t : public box_base_t {
		public:
			kern_t(scal w);
			scal	kern_field;
			int	fill_field;
			void  show();
	};
	
	class pen_t : public box_base_t {
		public:
			pen_t(int i);
			int	penalty_field;
			int	fill_field;
			void  show();
	};
	
	class unset_t : public box_base_t {
		public:
			scal	width_field;
			ptr	u_ptr_field;
			ptr	v_ptr_field;
			scal	stretch_field;
			mcell	info_field;
			scal	shrink_field;
	};
		
	enum node_types {
		RULE_NODE=2, INS_NODE=3, MARK_NODE=4, ADJUST_NODE=5,
		LIGATURE_NODE=6, DISC_NODE=7, WHATSIT_NODE=8, 
		MATH_NODE=9, GLUE_NODE=10, KERN_NODE=11,
		PENALTY_NODE=12, UNSET_NODE=13 };
	
	ptr	new_null_box();
	ptr	new_rule();
	ptr	new_ligature(int f, int c, ptr q);
	ptr   new_lig_item(int c);
	ptr	make_char_from_lig(ptr p);
	ptr	new_disc();
	ptr	new_math(scal w, int s);
	ptr	new_spec(ptr p);
	ptr	new_param_glue(int n);
	ptr	new_glue(ptr q);
	ptr	new_skip_param(int n);
	ptr	new_kern(scal w);
	ptr	new_penalty(int i);
	void	print_short_display();
	void	print_font_and_char(ptr p);
	void	print_mark(ptr p);
	void	print_rule_dimen(scal d);
	void	print_glue(scal d, int o, str s);
	void	print_spec(ptr p, str s); 
	void	short_display(ptr p);
	
//  	void	show_box(ptr p);
	void  show_glue_set(ptr p);
	void	show_box1(ptr p);
//		void  show_rule(ptr p);
//		void  show_insertion(ptr p);
	void  show_leaders(ptr p);
	void  show_glue_type(int n);
//		void  show_glue(ptr p);
//		void  show_kern(ptr p);
//		void  show_math(ptr p);
//		void  show_ligature(ptr p);
//		void  show_discretionary(ptr p);
//		void  show_penalty(ptr p);
//		void  show_mark(ptr p);
//		void  show_adjust(ptr p);

	void	show_node_list(ptr p);
	ptr	copy_node_list(ptr p);
	void	flush_node_list(ptr p);
	void	delete_glue_ref(ptr p);
	void  flush_node_list(ptr p);
	void	_box_init();
	void	_box_init_once();
};

#define BOX_NODE_SIZE		(sizeof(tex::box_t))
#define box_width(B)		((tex::box_t *) (B))->width_field
#define box_depth(B)		((tex::box_t *) (B))->depth_field
#define box_height(B)		((tex::box_t *) (B))->height_field
#define shift_amount(B)		((tex::box_t *) (B))->shift_field
#define node_list(B)		((ptr) & ((tex::box_t *) (B))->node_list_field)
#define list_ptr(B)		tex::link(node_list(B))
#define glue_order(B)		tex::type(node_list(B))
#define glue_sign(B)		tex::subtype(node_list(B))
#define glue_set(B)		((tex::box_t *) (B))->glue_set_field
#define NORMAL			0
#define STRETCHING		1
#define SHRINKING		2

#define RULE_NODE_SIZE		(sizeof(tex::rule_t))
#define rule_width(R)		((tex::rule_t *) (R))->width_field
#define rule_depth(R)		((tex::rule_t *) (R))->depth_field
#define rule_height(R)		((tex::rule_t *) (R))->height_field
#define NULL_FLAG		-010000000000
#define is_running(R)		((R) == NULL_FLAG)

#define INS_NODE_SIZE		(sizeof(tex::ins_t))
#define float_cost(I)		((tex::ins_t *) (I))->cost_field
#define ins_height(I)		((tex::ins_t *) (I))->height_field
#define ins_depth(I)		((tex::ins_t *) (I))->depth_field
#define ins_ptr(I)		((tex::ins_t *) (I))->ins_field
#define split_top_ptr(I)	((tex::ins_t *) (I))->split_top_field

#define SMALL_NODE_SIZE		(2 * sizeof(tex::mcell))

#define mark_ptr(M)		((tex::mark_t *) (M))->mark_field
	
#define adjust_ptr(A)		((tex::adjust_t *) (A))->adjust_field
#define lig_char(L)		((ptr) &((tex::lig_t *) (L))->lig_field)
#define lig_ptr(L)		tex::link(lig_char(L))

#define replace_count		tex::subtype
#define pre_break(D)		((tex::disc_t *) (D))->pre_field
#define post_break(D)		((tex::disc_t *) (D))->post_field

#define math_width(M)		((tex::math_t *) (M))->width_field
#define BEFORE			0
#define AFTER			1

#define precedes_break(M)	(tex::type(M) < MATH_NODE)
#define non_discardable(M)	(tex::type(M) < MATH_NODE)

#define glue_ptr(G)		((tex::glue_t *) (G))->glue_field
#define leader_ptr(G)		((tex::glue_t *) (G))->leader_field
#define COND_MATH_GLUE		98
#define MU_GLUE			99
#define A_LEADERS		100
#define C_LEADERS		101
#define X_LEADERS		102

#define GLUE_SPEC_SIZE		sizeof(tex::spec_t)
#define stretch_order(S)	((tex::spec_t *) (S))->stretch_order_field
#define shrink_order(S)		((tex::spec_t *) (S))->shrink_order_field
#define glue_ref_count(S)	((tex::spec_t *) (S))->ref_count_field
#define glue_width(S)		((tex::spec_t *) (S))->width_field
#define stretch(S)		((tex::spec_t *) (S))->stretch_field
#define shrink(S)		((tex::spec_t *) (S))->shrink_field
#define FIL			1
#define FILL			2
#define FILLL			3

#define kern_width(K)		((tex::kern_t *) (K))->kern_field
#define EXPLICIT		1
#define ACC_KERN		2

#define penalty(P)		((tex::pen_t *) (P))->penalty_field
#define INF_PENALTY		10000
#define EJECT_PENALTY		-INF_PENALTY

#define unset_span_count	tex::subtype
#define unset_span_list		tex::llink
#define	end_link		tex::llink
#define unset_width(U)		((tex::unset_t *) (U))->width_field
#define u_part(U)		((tex::unset_t *) (U))->u_ptr_field
#define v_part(U)		((tex::unset_t *) (U))->v_ptr_field
#define unset_info(U)		info((ptr)&((tex::unset_t *) (U))->info_field)
#define unset_stretch(U)	((tex::unset_t *) (U))->stretch_field
#define unset_shrink(U)		((tex::unset_t *) (U))->shrink_field


#define node_list_display(N) \
	{append_char('.'); show_node_list(N); flush_char();}

extern	fnt	font_in_short_display;
extern	int	depth_threshold;
extern	int	breadth_max;

#define fast_delete_glue_ref(G)	\
{	if (glue_ref_count(G) == 0) { \
		free_node(G, GLUE_SPEC_SIZE); \
	} else { \
		decr(glue_ref_count(G)); \
	} \
}

#define add_glue_ref(G) \
	(incr(glue_ref_count(G)))


#endif
