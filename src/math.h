
/*
 * $Id: math.h,v 1.4 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */


namespace tex {

	class style_t {
		public:
			mcell	noad_field;
			ptr	display_field;
			ptr	text_field;
			ptr	script_field;
			ptr	script_script_field;
	};
//	typedef struct style_t style_t;
	class noad_t {
		public:
			mcell	noad_field;
			mcell	nucleus_field;
			mcell	supscr_field;
			mcell	subscr_field;
	};
//	typedef struct noad_t noad_t;

	class delimiter_t {
		public:
			mcell	noad_field;
			mcell	nucleus_field;
			mcell	supscr_field;
			mcell	subscr_field;
			mcell	left_field;
			mcell	right_field;
	};
// typedef struct delimiter_t delimiter_t;
	class accent_t {
		public:
			mcell	noad_field;
			mcell	nucleus_field;
			mcell	supscr_field;
			mcell	subscr_field;
			mcell	accent_field;
	};
//	typedef struct accent_t accent_t;

	ptr	new_style(int s);
	ptr	new_choice();
	ptr	new_noad();
	void	print_fam_and_char(ptr p);
	void	print_delimiter(ptr p);
	void	print_subsidiary_data(ptr p, int c);
	void	print_style(int);
	void	print_size(int);
	void	show_normal_noad(ptr p);
	void	show_fraction_noad(ptr p);
	void	show_choice_node(ptr p);
	ptr	fraction_rule(scal t);
	ptr	overbar(ptr b, scal k, scal t);
	ptr	var_delimiter(ptr d, int s, scal v);
	void	stack_into_box(ptr b, fnt f, int c);
	scal	height_plus_depth(fnt f, int c);
	ptr	char_box(fnt f, int c);
	ptr	rebox(ptr b, scal w);
	ptr	math_glue(ptr g, scal m);
	void	math_kern(ptr p, scal m);
	void	flush_math();
	void	_math_init();
	void	_math_init_once();
};

#define STYLE_NODE		(UNSET_NODE + 1)
#define STYLE_NODE_SIZE		sizeof(style_t)
#define DISPLAY_STYLE		0
#define TEXT_STYLE		2
#define SCRIPT_STYLE		4
#define SCRIPT_SCRIPT_STYLE	6
#define CRAMPED			1

#define CHOICE_NODE		(UNSET_NODE + 2)
#define display_mlist(C)	((style_t *) (C))->display_field
#define text_mlist(C)		((style_t *) (C))->text_field
#define script_mlist(C)		((style_t *) (C))->script_field
#define script_script_mlist(C)	((style_t *) (C))->script_script_field

#define NOAD_SIZE		sizeof(noad_t)
#define nucleus(N)		((ptr) &((noad_t *) (N))->nucleus_field)
#define supscr(N)		((ptr) &((noad_t *) (N))->supscr_field)
#define subscr(N)		((ptr) &((noad_t *) (N))->subscr_field)
#define math_type		rinfo
#define math_link		llink

#define MATH_CHAR		1
#define SUB_BOX			2
#define SUB_MLIST		3
#define MATH_TEXT_CHAR		4

#define fam			font

#define ORD_NOAD		(UNSET_NODE + 3)
#define OP_NOAD			(ORD_NOAD + 1)
#define BIN_NOAD		(ORD_NOAD + 2)
#define REL_NOAD		(ORD_NOAD + 3)
#define OPEN_NOAD		(ORD_NOAD + 4)
#define CLOSE_NOAD		(ORD_NOAD + 5)
#define PUNCT_NOAD		(ORD_NOAD + 6)
#define INNER_NOAD		(ORD_NOAD + 7)

#define LIMITS			1
#define NO_LIMITS		2


#define left_delimiter(D)	((ptr) &((delimiter_t *) (D))->left_field)
#define right_delimiter(D)	((ptr) &((delimiter_t *) (D))->right_field)

#define small_fam		q0
#define small_char		q1
#define large_fam		q2
#define large_char		q3

#define RADICAL_NOAD		(INNER_NOAD + 1)
#define RADICAL_NOAD_SIZE	(5 * sizeof(mcell))
#define FRACTION_NOAD		(RADICAL_NOAD + 1)
#define FRACTION_NOAD_SIZE	(6 * sizeof(mcell))
#define DEFAULT_CODE		010000000000
#define thickness		box_width
#define numerator		supscr
#define denominator		subscr

#define UNDER_NOAD		(FRACTION_NOAD + 1)
#define OVER_NOAD		(UNDER_NOAD + 1)

#define ACCENT_NOAD		(OVER_NOAD + 1)

#define ACCENT_NOAD_SIZE	(sizeof(accent_t))
#define accent_chr(A)		((ptr) &((accent_t *) (A))->accent_field)

#define VCENTER_NOAD		(ACCENT_NOAD + 1)
#define LEFT_NOAD		(VCENTER_NOAD + 1)
#define RIGHT_NOAD		(LEFT_NOAD + 1)
#define delimiter		nucleus	

#define scripts_allowed(N)	(type(N) >= ORD_NOAD && type(N) < LEFT_NOAD)

#define TEXT_SIZE		0
#define SCRIPT_SIZE		16
#define SCRIPT_SCRIPT_SIZE	32

#define mathsy(S, P)		param(fam_fnt(2 + S), P)

#define math_x_height(S)	mathsy(S, 5)
#define math_quad(S)		mathsy(S, 6)
#define num1(S)			mathsy(S, 8)
#define num2(S)			mathsy(S, 9)
#define num3(S)			mathsy(S, 10)
#define denom1(S)		mathsy(S, 11)
#define denom2(S)		mathsy(S, 12)
#define sup1(S)			mathsy(S, 13)
#define sup2(S)			mathsy(S, 14)
#define sup3(S)			mathsy(S, 15)
#define sub1(S)			mathsy(S, 16)
#define sub2(S)			mathsy(S, 17)
#define sup_drop(S)		mathsy(S, 18)
#define sub_drop(S)		mathsy(S, 19)
#define delim1(S)		mathsy(S, 20)
#define delim2(S)		mathsy(S, 21)
#define axis_height(S)		mathsy(S, 22)

#define TOTAL_MATHSY_PARAMS	22

#define mathex(P) 		param(fam_fnt(3 + cur_size), P)

#define default_rule_thickness	mathex(8)
#define big_op_spacing1		mathex(9)
#define big_op_spacing2		mathex(10)
#define big_op_spacing3		mathex(11)
#define big_op_spacing4		mathex(12)
#define big_op_spacing5		mathex(13)

#define TOTAL_MATHEX_PARAMS	13

#define cramped_style(S)	(2 * ((S) / 2) + CRAMPED)
#define sub_style(S)		(2 * ((S) / 4) + SCRIPT_STYLE + CRAMPED)
#define sup_style(S)		(2 * ((S) / 4) + SCRIPT_STYLE + ((S) % 2))
#define num_style(S)		((S) + 2 - 2 * ((S) / 6))
#define denom_style(S)		(2 * ((S) / 2) + CRAMPED + 2 - 2 * ((S) / 6))

