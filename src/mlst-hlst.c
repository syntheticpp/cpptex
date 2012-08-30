
/*
 * $Id: mlst-hlst.c,v 1.5 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include <stdexcept>
#include "tex.h"

fnt	tex::cur_f;
int	tex::cur_c;
tex::qcell	tex::cur_i;
int	cur_style;
int	tex::cur_size;
scal	tex::cur_mu;

#define change_size_and_mu() \
{	if (cur_style < SCRIPT_STYLE) { \
		cur_size = TEXT_SIZE; \
	} else { \
		cur_size = 16 * ((cur_style - TEXT_STYLE) / 2); \
	} \
	cur_mu = x_over_n(math_quad(cur_size), 18); \
}

int	magic_offset = - 9 * ORD_NOAD;

char	math_spacing[] = 
	"0234000122*4000133**3**344*0400400*000000234000111*1111112341011";

#define new_hlist(N)		link(nucleus(N))
#define choose_mlist(L)	{p = L(q); L(q) = null;}

ptr tex::mlist_to_hlist(ptr mlist, int style, bool penalties)
	{
	ptr	h;
	ptr	p=0;
	ptr	q;
	ptr	r;
	int	s;
	int	t;
	ptr	x;
	ptr	y;
	ptr	z;
	int	pen;
	scal	delta;
	scal	max_d;
	scal	max_h;
	int	r_type;
	int	save_style;

	q = mlist;
	r = null;
	r_type = OP_NOAD;
	max_h = 0;
	max_d = 0;
	cur_style = style;
	change_size_and_mu();

	while (q != null)
	{
reswitch:
		delta = 0;
		switch (type(q))
		{
		case BIN_NOAD:	
			switch (r_type)
			{
			case BIN_NOAD:
			case OP_NOAD:
			case REL_NOAD:
			case OPEN_NOAD:
			case PUNCT_NOAD:
			case LEFT_NOAD:
				type(q) = ORD_NOAD;
				goto reswitch;

			default:
				break;
			}
			break;

		case REL_NOAD:
		case CLOSE_NOAD:
		case PUNCT_NOAD:
		case RIGHT_NOAD:
			if (r_type == BIN_NOAD)
				type(r) = ORD_NOAD;
			if (type(q) == RIGHT_NOAD)
				goto done_with_noad;
			break;

		case LEFT_NOAD:
			goto done_with_noad;

		case FRACTION_NOAD:
			make_fraction(q);
			goto check_dimensions;
		
		case OP_NOAD:
			delta = make_op(q);
			if (subtype(q) == LIMITS)
				goto check_dimensions;
			break;

		case ORD_NOAD:
			make_ord(q);
			break;

		case OPEN_NOAD:
		case INNER_NOAD:
			break;
		
		case RADICAL_NOAD:
			make_radical(q);
			break;
		
		case OVER_NOAD:
			make_over(q);
			break;

		case UNDER_NOAD:
			make_under(q);
			break;
	
		case ACCENT_NOAD:
			make_math_accent(q);
			break;
		
		case VCENTER_NOAD:
			make_vcenter(q);
			break;
		
		case STYLE_NODE:
			cur_style = subtype(q);
			change_size_and_mu();
			goto done_with_node;
		
		case CHOICE_NODE:
			switch (cur_style / 2) {
			case 0: choose_mlist(display_mlist); break;
			case 1: choose_mlist(text_mlist); break;
			case 2: choose_mlist(script_mlist); break;
			case 3: choose_mlist(script_script_mlist); break;
			}
			flush_node_list(display_mlist(q));
			flush_node_list(text_mlist(q));
			flush_node_list(script_mlist(q));
			flush_node_list(script_script_mlist(q));
			memset((void *)q, 0, STYLE_NODE_SIZE);
			type(q) = STYLE_NODE;
			subtype(q) = cur_style;
			if (p != null) {
				z = link(q);
				link(q) = p;
				while (link(p) != null)
					p = link(p);
				link(p) = z;
			}
			goto done_with_node;

		case INS_NODE:
		case MARK_NODE:
		case ADJUST_NODE:
		case WHATSIT_NODE:
		case PENALTY_NODE:
		case DISC_NODE:
			goto done_with_node;
		
		case RULE_NODE:
			if (rule_height(q) > max_h)
				max_h = rule_height(q);
			if (rule_depth(q) > max_d)
				max_d = rule_depth(q);
			goto done_with_node;

		case GLUE_NODE:
			if (subtype(q) == MU_GLUE) {
				x = glue_ptr(q);
				y = math_glue(x, cur_mu);
				delete_glue_ref(x);
				glue_ptr(q) = y;
				subtype(q) = NORMAL;
			} else if (cur_size != TEXT_SIZE &&
				subtype(q) == COND_MATH_GLUE) {
				p = link(q);
				if (p != null &&
					(type(p) == GLUE_NODE ||
					type(p) == KERN_NODE)) {
					link(q) = link(p);
					link(p) = null;
					flush_node_list(p);
				}
			}
			goto done_with_node;
		
		case KERN_NODE:
			math_kern(q, cur_mu);
			goto done_with_node;
		
		default:
			confusion("mlist1");
		}

		switch (math_type(nucleus(q)))
		{
		case MATH_CHAR:
		case MATH_TEXT_CHAR:
			fetch(nucleus(q));
			if (char_exists(cur_i)) {
				delta = char_italic(cur_f, cur_i);
				p = new_character(cur_f, cur_c);
				if (math_type(nucleus(q)) == MATH_TEXT_CHAR &&
					space(cur_f) != 0)
					delta = 0;
				if (math_type(subscr(q)) == EMPTY &&
					delta != 0) {
					link(p) = new_kern(delta);
					delta = 0;
				}
			} else {
				p = null;
			}
			break;
		
		case EMPTY:
			p = null;
			break;

		case SUB_BOX:
			p = math_link(nucleus(q));
			break;

		case SUB_MLIST:
			save_style = cur_style;
			p = mlist_to_hlist(math_link(nucleus(q)),
				cur_style, FALSE);
			cur_style = save_style;
			change_size_and_mu();
			p = hpack(p, 0, ADDITIONAL); 
			break;

		default:
			confusion("mlist2");
		}
		new_hlist(q) = p;
		if (math_type(subscr(q)) == EMPTY && 
			math_type(supscr(q)) == EMPTY)
			goto check_dimensions;
		make_scripts(q, delta);
		
check_dimensions:
		z = hpack(new_hlist(q), 0, ADDITIONAL);
		if (box_height(z) > max_h)
			max_h = box_height(z);
		if (box_depth(z) > max_d)
			max_d = box_depth(z);
		free_node(z, BOX_NODE_SIZE);

done_with_noad:
		r = q;
		r_type = type(r);

done_with_node:
		q = link(q);
	} 

	if (r_type == BIN_NOAD)
		type(r) = ORD_NOAD;
	h = p = new_avail();
	link(p) = null;
	q = mlist;
	r_type = 0;
	cur_style = style;
	change_size_and_mu();

	while (q != null) {
		t = ORD_NOAD;
		s = NOAD_SIZE;
		pen = INF_PENALTY;
		switch (type(q))
		{
		case OP_NOAD:
		case OPEN_NOAD:
		case CLOSE_NOAD:
		case PUNCT_NOAD:
		case INNER_NOAD:
			t = type(q);
			break;
		
		case BIN_NOAD:
			t = BIN_NOAD;
			pen = bin_op_penalty;
			break;
		
		case REL_NOAD:
			t = REL_NOAD;
			pen = rel_penalty;
			break; 
		
		case ORD_NOAD:
		case VCENTER_NOAD:
		case OVER_NOAD:
		case UNDER_NOAD:
			break;

		case RADICAL_NOAD:
			s = RADICAL_NOAD_SIZE;
			break;

		case ACCENT_NOAD:
			s = ACCENT_NOAD_SIZE;
			break;

		case FRACTION_NOAD:
			t = INNER_NOAD;
			s = FRACTION_NOAD_SIZE;
			break;

		case LEFT_NOAD:
		case RIGHT_NOAD:
			t = make_left_right(q, style, max_d, max_h);
			break;
		
		case STYLE_NODE:
			s = STYLE_NODE_SIZE;
			cur_style = subtype(q);
			change_size_and_mu();
			goto delete_q;

		case WHATSIT_NODE:
		case PENALTY_NODE:
		case RULE_NODE:
		case DISC_NODE:
		case ADJUST_NODE:
		case INS_NODE:
		case MARK_NODE:
		case GLUE_NODE:
		case KERN_NODE:
			p = link(p) = q;
			q = link(q);
			link(p) = null;
			continue;
		
		default:
			confusion("mlist3");
		}

		if (r_type > 0) {
			int	x=0;

			switch (math_spacing[r_type * 8 + t + magic_offset]) 
			{
			case '0':
				x = -1;
				break;

			case '1':
				if (cur_style < SCRIPT_STYLE)
					x = THIN_MU_SKIP_CODE;
				else x = -1;
				break;

			case '2':
				x = THIN_MU_SKIP_CODE;
				break;

			case '3':
				if (cur_style < SCRIPT_STYLE)
					x = MED_MU_SKIP_CODE;
				else x = -1;
				break;

			case '4':
				if (cur_style < SCRIPT_STYLE)
					x = THICK_MU_SKIP_CODE;
				else x = -1;
				break;

			default:
				confusion("mlist4");
				break;
			}
			if (x >= 0) {
				y = math_glue(mu_glue_par(x), cur_mu);
				z = new_glue(y);
				glue_ref_count(y) = null;
				p = link(p) = z;
				subtype(z) = GLUE_PARS + x + 1;
			}
		}
		if (new_hlist(q) != null) {
			link(p) = new_hlist(q);
			do p = link(p);
			while (link(p) != null);
		}
		if (penalties && link(q) != null && pen < INF_PENALTY) {
			r_type = type(link(q));
			if (r_type != PENALTY_NODE && r_type != REL_NOAD) {
				z = new_penalty(pen);
				p = link(p) = z;
			}
		}
		r_type = t;

delete_q:
		r = q;
		q = link(q);
		free_node(r, s);
	}
	return (link(h));
}

void tex::make_over(ptr q)
	{
	math_link(nucleus(q)) =
		overbar(clean_box(nucleus(q), cramped_style(cur_style)),
			3 * default_rule_thickness, default_rule_thickness);
	math_type(nucleus(q)) = SUB_BOX;
	}

void tex::make_under(ptr q)
	{
	ptr	p;
	ptr	x;
	ptr	y;
	scal	delta;

	x = clean_box(nucleus(q), cur_style);
	link(x) = p = new_kern(3 * default_rule_thickness);
	link(p) = fraction_rule(default_rule_thickness);
	y = vpack(x, 0, ADDITIONAL);
	delta = box_height(y) + box_depth(y) + default_rule_thickness;
	box_height(y) = box_height(x);
	box_depth(y) = delta - box_height(y);
	math_link(nucleus(q)) = y;
	math_type(nucleus(q)) = SUB_BOX;
}

void tex::make_vcenter(ptr q)
	{
	ptr	v;
	scal	delta;

	v = math_link(nucleus(q));
	if (type(v) != VLIST_NODE)
		confusion("vcenter");
	delta = box_height(v) + box_depth(v);
	box_height(v) = axis_height(cur_size) + half(delta);
	box_depth(v) = delta - box_height(v);
	}

void tex::make_radical(ptr q)
	{
	ptr	x;
	ptr	y;
	scal	clr;
	scal	delta;

	x = clean_box(nucleus(q), cramped_style(cur_style));
	clr = default_rule_thickness;
	if (cur_style < TEXT_STYLE) {
		clr += abs(math_x_height(cur_size)) / 4;
	} else {
		clr += (abs(clr) / 4);
	}
	y = var_delimiter(left_delimiter(q), cur_size,
		box_height(x) + box_depth(x) + clr + default_rule_thickness);
	delta = box_depth(y) - (box_height(x) + box_depth(x) + clr);
	if (delta > 0)
		clr += half(delta);
	shift_amount(y) = -(box_height(x) + clr);
	link(y) = overbar(x, clr, box_height(y));
	math_link(nucleus(q)) = hpack(y, 0, ADDITIONAL);
	math_type(nucleus(q)) = SUB_BOX;
	}

void tex::make_math_accent(ptr q)
	{
	qcell	*a;
	int	c;
	fnt	f;
	scal	h;
	qcell	i;
	ptr	p;
	scal	s;
	scal	w;
	ptr	x;
	ptr	y;
	scal	delta;

	fetch(accent_chr(q)); 
	if (!char_exists(cur_i))
		return;
	i = cur_i;
	c = cur_c;
	f = cur_f;
	s = 0;
	if (math_type(nucleus(q)) == MATH_CHAR) {
		fetch(nucleus(q));
		if (char_tag(cur_i) == LIG_TAG) {
			a = lig_kern_start(cur_f, cur_i);
			cur_i = *a;
			if (skip_byte(cur_i) > STOP_FLAG) {
				a = lig_kern_restart(cur_f, cur_i);
				cur_i = *a;
			}
			loop {
				if (next_char(cur_i) == skew_char(cur_f)) {
					if (op_byte(cur_i) >= KERN_FLAG
					&& skip_byte(cur_i) <= STOP_FLAG) {
						s = char_kern(cur_f, cur_i);
					}
					break;
				}
				if (skip_byte(cur_i) >= STOP_FLAG)
					break;
				a += skip_byte(cur_i) + 1;
				cur_i = *a;
			}
		}
	}
	x = clean_box(nucleus(q), cramped_style(cur_style));
	w = box_width(x);
	h = box_height(x);
	loop {
		if (char_tag(i) != LIST_TAG)
			break;
		y = rem_byte(i);
		i = char_info(f, y);
		if (char_width(f, i) > w)
			break;
		c = y;
	}
	delta = (h < x_height(f) ? h : x_height(f));
	if ((math_type(supscr(q)) != EMPTY
		|| math_type(subscr(q)) != EMPTY)
	&& math_type(nucleus(q)) == MATH_CHAR) {
			flush_node_list(x);
			x = new_noad(); 
			mcopy(nucleus(x), nucleus(q));
			mcopy(supscr(x), supscr(q));
			mcopy(subscr(x), subscr(q));
			mzero(supscr(q));
			mzero(subscr(q));
			math_type(nucleus(q)) = SUB_MLIST;
			math_link(nucleus(q)) = x;
			x = clean_box(nucleus(q), cur_style);
			delta += box_height(x) - h;
			h = box_height(x);
	}
	y = char_box(f, c);
	shift_amount(y) = s + half(w - box_width(y));
	box_width(y) = 0;
	link(y) = p = new_kern(-delta);
	link(p) = x;
	y = vpack(y, 0, ADDITIONAL);
	box_width(y) = box_width(x);
	if (box_height(y) < h) {
		p = new_kern(h - box_height(y));
		link(p) = list_ptr(y);
		list_ptr(y) = p;
		box_height(y) = h;
	}
	math_link(nucleus(q)) = y;
	math_type(nucleus(q)) = SUB_BOX;
	}

void tex::make_fraction(ptr q)
	{
	ptr	p;
	ptr	v;
	ptr	x;
	ptr	y;
	ptr	z;
	scal	clr;
	scal	delta;
	scal	delta1;
	scal	delta2;
	scal	shift_up;
	scal	shift_down;

	if (thickness(q) == DEFAULT_CODE)
		thickness(q) = default_rule_thickness;
	x = clean_box(numerator(q), num_style(cur_style));
	z = clean_box(denominator(q), denom_style(cur_style));
	if (box_width(x) < box_width(z))
		x = rebox(x, box_width(z));
	else z = rebox(z, box_width(x));
	if (cur_style < TEXT_STYLE) {
		shift_up = num1(cur_size);
		shift_down = denom1(cur_size);
	} else {
		shift_down = denom2(cur_size);
		if (thickness(q) != 0)
			shift_up = num2(cur_size);
		else shift_up = num3(cur_size);
	}
	if (thickness(q) == 0) {
		if (cur_style < TEXT_STYLE)
			clr = 7 * default_rule_thickness;
		else clr = 3 * default_rule_thickness;
		delta = half(clr - 
			((shift_up - box_depth(x)) -
			(box_height(z) - shift_down)));
		if(delta > 0) {
			shift_up += delta;
			shift_down += delta;
		}
	} else {
		if (cur_style < TEXT_STYLE)
			clr = 3 * thickness(q);
		else clr = thickness(q);
		delta1 = delta2 = clr;
		delta = half(thickness(q));
		delta1 -= (shift_up - box_depth(x)) -
			(axis_height(cur_size)+delta);
		delta2 -= (axis_height(cur_size) - delta) -
			(box_height(z) - shift_down);
		if (delta1 > 0)
			shift_up += delta1;
		if (delta2 > 0)
			shift_down += delta2;
	}
	v = new_null_box();
	type(v) = VLIST_NODE;
	box_height(v) = shift_up + box_height(x);
	box_depth(v) = box_depth(z) + shift_down;
	box_width(v) = box_width(x);
	if (thickness(q) == 0) {
		p = new_kern((shift_up - box_depth(x)) -
			(box_height(z) - shift_down));
		link(p) = z;
	} else {
		y = fraction_rule(thickness(q));
		p = new_kern((axis_height(cur_size) - delta) -
			(box_height(z) - shift_down));
		link(y) = p;
		link(p) = z;
		p = new_kern((shift_up - box_depth(x)) -
			(axis_height(cur_size)+delta));
		link(p) = y;
	}
	link(x) = p;
	list_ptr(v) = x;
	if (cur_style < TEXT_STYLE)
		delta = delim1(cur_size);
	else delta = delim2(cur_size);
	x = var_delimiter(left_delimiter(q), cur_size, delta);
	link(x) = v;
	z = var_delimiter(right_delimiter(q), cur_size, delta);
	link(v) = z;
	new_hlist(q) = hpack(x, 0, ADDITIONAL);
	}

scal tex::make_op(ptr q)
	{
	ptr	p;
	ptr	v;
	ptr	x;
	ptr	y;
	ptr	z;
	scal	delta;
	scal	shift_up;
	scal	shift_down;

	if (subtype(q) == NORMAL && cur_style < TEXT_STYLE)
		subtype(q) = LIMITS;
	if (math_type(nucleus(q)) == MATH_CHAR) {
		fetch(nucleus(q));
		if (cur_style < TEXT_STYLE && char_tag(cur_i) == LIST_TAG) {
			cur_c = rem_byte(cur_i);
			character(nucleus(q)) = cur_c;
			cur_i = char_info(cur_f, cur_c);
		}
		delta = char_italic(cur_f, cur_i);
		x = clean_box(nucleus(q), cur_style);
		if (math_type(subscr(q)) != EMPTY && subtype(q) != LIMITS)
			box_width(x) -= delta;
		shift_amount(x) = half(box_height(x) - box_depth(x)) -
			axis_height(cur_size);
		math_type(nucleus(q)) = SUB_BOX;
		math_link(nucleus(q)) = x;
	} else {
		delta = 0;
	}
	if (subtype(q) == LIMITS) {
		x = clean_box(supscr(q), sup_style(cur_style));
		y = clean_box(nucleus(q), cur_style);
		z = clean_box(subscr(q), sub_style(cur_style));
		v = new_null_box();
		type(v) = VLIST_NODE;
		box_width(v) = box_width(y);
		if (box_width(x) > box_width(v))
			box_width(v) = box_width(x);
		if (box_width(z) > box_width(v))
			box_width(v) = box_width(z);
		x = rebox(x, box_width(v));
		y = rebox(y, box_width(v));
		z = rebox(z, box_width(v));
		shift_amount(x) = half(delta);
		shift_amount(z) = -shift_amount(x);
		box_height(v) = box_height(y);
		box_depth(v) = box_depth(y);
		if (math_type(supscr(q)) == EMPTY) {
			free_node(x, BOX_NODE_SIZE);
			list_ptr(v) = y;
		} else {
			shift_up = big_op_spacing3 - box_depth(x);
			if (shift_up < big_op_spacing1)
				shift_up = big_op_spacing1;
			p = new_kern(shift_up);
			link(x) = p; link(p) = y; 
			p = new_kern(big_op_spacing5);
			list_ptr(v) = p;
			link(p) = x;
			box_height(v) += big_op_spacing5 +
				box_height(x) + box_depth(x) + shift_up;
		}
		if (math_type(subscr(q)) == EMPTY) {
			free_node(z, BOX_NODE_SIZE);
		} else {
			shift_down = big_op_spacing4 - box_height(z);
			if (shift_down < big_op_spacing2)
				shift_down = big_op_spacing2;
			p = new_kern(shift_down);
			link(y) = p;
			link(p) = z;
			p = new_kern(big_op_spacing5);
			link(z) = p;
			box_depth(v) += big_op_spacing5 +
				box_height(z) + box_depth(z) + shift_down;
		}
		new_hlist(q) = v;
	}
	return delta;
	}

#define ord_lig()							\
{	switch (op_byte(cur_i))						\
	{								\
	case 1: case 5:							\
		character(nucleus(q)) = rem_byte(cur_i);		\
		break;							\
	case 2: case 6:							\
		character(nucleus(p)) = rem_byte(cur_i);		\
		break;							\
	case 3: case 7: case 11:					\
		r = new_noad();						\
		character(nucleus(r)) = rem_byte(cur_i);		\
		fam(nucleus(r)) = fam(nucleus(q));			\
		link(q) = r;						\
		link(r) = p;						\
		if (op_byte(cur_i) < 11) {				\
			math_type(nucleus(r)) = MATH_CHAR;		\
		} else {						\
			math_type(nucleus(r)) = MATH_TEXT_CHAR;		\
		}							\
		break;							\
	default:							\
		link(q) = link(p);					\
		character(nucleus(q)) = rem_byte(cur_i);		\
		mcopy(subscr(q), subscr(p));				\
		mcopy(supscr(q), supscr(p));				\
		free_node(p, NOAD_SIZE);				\
		break;							\
	}								\
	if (op_byte(cur_i) > 3)						\
		return;							\
	math_type(nucleus(q)) = MATH_CHAR;				\
	goto restart;							\
}

void tex::make_ord(ptr q)
	{
	ptr	p, r;
	qcell	*a;

restart:
	if (math_type(subscr(q)) != EMPTY
	|| math_type(supscr(q)) != EMPTY
	|| math_type(nucleus(q)) != MATH_CHAR) {
		return;
	}
	p = link(q);
	if (p == null
	|| type(p) < ORD_NOAD
	|| type(p) > PUNCT_NOAD
	|| math_type(nucleus(p)) != MATH_CHAR
	|| fam(nucleus(p)) != fam(nucleus(q))) {
		return;
	}
	math_type(nucleus(q)) = MATH_TEXT_CHAR;
	fetch(nucleus(q));
	if (char_tag(cur_i) == LIG_TAG) {
		a = lig_kern_start(cur_f, cur_i);
		cur_c = character(nucleus(p));
		cur_i = *a;
		if (skip_byte(cur_i) > STOP_FLAG) {
			a = lig_kern_restart(cur_f, cur_i);
			cur_i = *a;
		}
		loop {
			if (next_char(cur_i) == cur_c
			&& skip_byte(cur_i) <= STOP_FLAG) {
				if (op_byte(cur_i) >= KERN_FLAG) {
					p = new_kern(char_kern(cur_f, cur_i));
					link(p) = link(q);
					link(q) = p;
					return;
				} else {
					if(interrupted) throw std::logic_error("interrupted");
					ord_lig();
				}
			}
			if (skip_byte(cur_i) >= STOP_FLAG)
				return;
			a += skip_byte(cur_i) + 1;
			cur_i = *a;
		}
	}
}

void tex::make_scripts(ptr q, scal delta)
	{
	ptr	p;
	int	t;
	ptr	x;
	ptr	y;
	ptr	z;
	scal	clr;
	scal	shift_up;
	scal	shift_down;

	p = new_hlist(q);
	if (is_char_node(p)) {
		shift_up = 0;
		shift_down = 0;
	} else {
		z = hpack(p, 0, ADDITIONAL);
		if (cur_style < SCRIPT_STYLE)	
			t = SCRIPT_SIZE;
		else t = SCRIPT_SCRIPT_SIZE;
		shift_up = box_height(z) - sup_drop(t);
		shift_down = box_depth(z) + sub_drop(t);
		free_node(z, BOX_NODE_SIZE);
	}
	if (math_type(supscr(q)) == EMPTY) {
		x = clean_box(subscr(q), sub_style(cur_style));
		box_width(x) += script_space;
		if (shift_down < sub1(cur_size))
			shift_down = sub1(cur_size);
		clr = box_height(x) - (abs(math_x_height(cur_size) * 4) / 5);
		if (shift_down < clr)
			shift_down = clr;
		shift_amount(x) = shift_down;
	} else {
		x = clean_box(supscr(q), sup_style(cur_style));
		box_width(x) += script_space;
		if (odd(cur_style))
			clr = sup3(cur_size);
		else if (cur_style < TEXT_STYLE)
			clr = sup1(cur_size);
		else clr = sup2(cur_size);
		if (shift_up < clr)
			shift_up = clr;
		clr = box_depth(x) + (abs(math_x_height(cur_size)) / 4);
		if (shift_up < clr)
			shift_up = clr;
		if (math_type(subscr(q)) == EMPTY) {
			shift_amount(x) = -shift_up;
		} else {
			y = clean_box(subscr(q), sub_style(cur_style));
			box_width(y) += script_space;
			if (shift_down < sub2(cur_size))
				shift_down = sub2(cur_size);
			clr = 4 * default_rule_thickness -
				((shift_up - box_depth(x)) - 
					(box_height(y) - shift_down));
			if (clr > 0) {
				shift_down += clr;
				clr = (abs(math_x_height(cur_size) * 4) / 5) -
					(shift_up - box_depth(x));
				if (clr > 0) {
					shift_up += clr;
					shift_down -= clr;
				}
			}
			shift_amount(x) = delta;
			p = new_kern((shift_up - box_depth(x)) -
				(box_height(y) - shift_down));
			link(x) = p; link(p) = y;
			x = vpack(x, 0, ADDITIONAL);
			shift_amount(x) = shift_down;
		}
	}
	if (new_hlist(q) == null) {
		new_hlist(q) = x;
	} else {
		p = new_hlist(q);
		while (link(p) != null)
			p = link(p);
		link(p) = x;
	}
}

int tex::make_left_right(ptr q, int style, scal max_d, scal max_h)
	{
	scal	delta;
	scal	delta1;
	scal	delta2;

	if (style < SCRIPT_STYLE)
		cur_size = TEXT_SIZE;
	else cur_size = 16 * ((style - TEXT_STYLE) / 2);
	delta2 = max_d + axis_height(cur_size);
	delta1 = max_h + max_d - delta2;
	if (delta2 > delta1)
		delta1 = delta2;
	delta = (delta1 / 500) * delimiter_factor;
	delta2 = delta1 + delta1 - delimiter_shortfall;
	if (delta < delta2)
		delta = delta2;
	new_hlist(q) = var_delimiter(delimiter(q), cur_size, delta);
	return (type(q) - (LEFT_NOAD - OPEN_NOAD));
	}

void tex::fetch(ptr a)
	{
	cur_c = character(a);
	cur_f = fam_fnt(fam(a) + cur_size);
	if (cur_f == null_font) {
		print_err(null_str);
		print_size(cur_size);
		print(" ");
		print_int(fam(a));
		print(" is undefined (character ");
		print_ASCII(cur_c);
		print(")");
		help_undefd_mathchar();
		error();
		cur_i = null_character;
		math_type(a) = EMPTY;
	} else {
		if (cur_c >= font_bc(cur_f)
		&& cur_c <= font_ec(cur_f)) {
			cur_i = char_info(cur_f, cur_c);
		} else {
			cur_i = null_character;
		}
		if (!char_exists(cur_i)) {
			char_warning(cur_f, cur_c);
			math_type(a) = EMPTY;
		}
	}
	}

ptr tex::clean_box(ptr p, int style)
	{
	ptr	m;
	ptr	q;
	ptr	r;
	ptr	x;
	int	save_style;

	switch (math_type(p))
	{
	case MATH_CHAR:
		m = new_noad();
		mcopy(nucleus(m), p);
		break;

	case SUB_BOX:
		q = math_link(p);
		goto found; 
	
	case SUB_MLIST:
		m = math_link(p);
		break;
	
	default:
		q = new_null_box();
		goto found;
	}

	save_style = cur_style;
	q = mlist_to_hlist(m, style, FALSE);
	cur_style = save_style;
	change_size_and_mu();

found:
	if (is_char_node(q) || q == null) {
		x = hpack(q, 0, ADDITIONAL);	
	} else if (link(q) == null
		&& type(q) <= VLIST_NODE
		&& shift_amount(q) == 0) {
		x = q;
	} else {
		x = hpack(q, 0, ADDITIONAL);
	}
	q = list_ptr(x);
	if (is_char_node(q)) {
		r = link(q);
		if (r != null
		&& link(r) == null
		&& type(r) == KERN_NODE) {
			free_node(r, SMALL_NODE_SIZE);
			link(q) = null;
		}
	}
	return x;
}

void tex::_mlst_hlst_init ()
	{
	}

void tex::_mlst_hlst_init_once ()
	{
	}

/*
**	Help text
*/

void tex::help_undefd_mathchar()
	{
	help4("Somewhere in the math formula just ended, you used the",
	"stated character from an undefined font family. For example,",
	"plain TeX doesn't allow \\it or \\sl in subscripts. Proceed,",
	"and I'll try to forget that I needed that character.");
	}
