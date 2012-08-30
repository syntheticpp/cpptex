
/*
 * $Id: box.c,v 1.8 2003/09/23 08:08:36 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include <cstdlib>
#include <cmath>

#include "box.h"

int	depth_threshold;
int	breadth_max;
fnt	font_in_short_display;

tex::box_t::box_t()
	{
	type(this) = HLIST_NODE;
	subtype(this) = MIN_QUARTERWORD;
	box_width(this) = 0;
	box_depth(this) = 0;
	box_height(this) = 0;
	shift_amount(this) = 0;
	list_ptr(this) = 0;
	glue_sign(this) = NORMAL;
	glue_order(this) = NORMAL;
	glue_set(this) = 0.0;
	}

tex::rule_t::rule_t()
	{
	tex::type(this) = RULE_NODE;
	tex::subtype(this) = 0;
	rule_width(this) = NULL_FLAG;
	rule_depth(this) = NULL_FLAG;
	rule_height(this) = NULL_FLAG;
	}

tex::lig_t::lig_t(int f, int c, ptr q)
	{
	tex::type(this) = LIGATURE_NODE;
	tex::subtype(this) = 0;
	font(lig_char(this)) = f;
	character(lig_char(this)) = c;
	lig_ptr(this) = q;
	}

tex::lig_t::lig_t(int c) // lig_item
	{
	tex::type(this) = LIGATURE_NODE;
	tex::subtype(this) = c;
	lig_ptr(this) = 0;
	}

// replace with copy constructor
//ptr tex::make_char_from_lig(ptr p) 
//	{
//	static tex::mcell m;
//	mcopy(&m, lig_char(p));
//	tex::link((ptr)&m) = tex::link(p);
//	return ((ptr)&m);
//	}

tex::disc_t::disc_t()
	{
	tex::type(this) = DISC_NODE;
	replace_count(this) = 0;
	pre_break(this) = 0;
	post_break(this) = 0;
	}

tex::math_t::math_t(scal w, int s)
	{
	tex::type(this) = MATH_NODE;
	tex::subtype(this) = s;
	math_width(this) = w;
	}

tex::spec_t::spec_t(box_base_t *p)
	{
	stretch_order(this) = stretch_order(p);
	shrink_order(this) = shrink_order(p);
	glue_ref_count(this) = 0;
	glue_width(this) = glue_width(p);
	stretch(this) = stretch(p);
	shrink(this) = shrink(p);
	}


// These two depend on glue_par, which depends on 
// glue registers. Better to require that the register
// is passed instead of the register index, to avoid
// that box depends on sym.

//tex::glue_t::glue_t(int n)
//	{
//	tex::type(this) = GLUE_NODE;
//	tex::subtype(this) = n + 1;
//	leader_ptr(this) = 0;
//	box_base_t *q = glue_par(n);
//	glue_ptr(this) = q;
//	incr(glue_ref_count(q));
//	}
//

tex::glue_t::glue_t(glue_t *q)
	{
	tex::type(this) = GLUE_NODE;
	tex::subtype(this) = NORMAL;
	leader_ptr(this) = 0;
	glue_ptr(this) = q;
	incr(glue_ref_count(q));
	}

// tex::skip_t::skip_t(int n)
// 	{
// 	spec_t *q = new spec_t(glue_par(n));
// 	tex::type(this) = GLUE_NODE;
// 	tex::subtype(this) = NORMAL;
// 	leader_ptr(this) = 0;
// 	glue_ptr(this) = q;
// 	incr(glue_ref_count(q));
// 	p = new_glue(q);
// 	glue_ref_count(q) = 0;  // FIXME: is this correct?
// 	tex::subtype(this) = n + 1;
// 	}

tex::kern_t::kern_t(scal w)
	{
	tex::type(this) = KERN_NODE;
	tex::subtype(this) = NORMAL;
	kern_width(this) = w;
	}

tex::pen_t::pen_t(int i)
	{
	tex::type(this) = PENALTY_NODE;
	tex::subtype(this) = 0;
	penalty(this) = i;
	}

void tex::print_font_and_char(ptr p)
	{
	if (font(p) < FONT_BASE || font(p) > FONT_MAX) {
		print("* ");
		} 
	else {
		print_cs((char *)font_id(font(p)));
		print_ASCII(character(p));
		}
	}

void tex::print_mark(ptr p)
	{
	print("{");
	show_token_list(token_link(p), null, MAX_PRINT_LINE - 10);
	print("}");
	}

void tex::print_rule_dimen(scal d)
	{
	if (is_running(d))
		print("*");
	else print_scaled(d);
	}

void tex::print_glue(scal d, int o, str s)
	{
	print_scaled(d);
	if (o < NORMAL || o > FILLL) {
		print("foul");
		} 
	else if (o > NORMAL) {
		print("fil");
		while  (o >  FIL) {
			print("l");
			decr(o);
			}
		} 
	else if (s) {
		print(s);
		}
	}

void tex::print_spec(ptr p, str s)
	{
	print_scaled(glue_width(p));
	print(s);
	if (stretch(p) != 0) {
		print(" plus ");
		print_glue(stretch(p), stretch_order(p), s);
		}
	if (shrink(p) != 0) {
		print(" minus ");
		print_glue(shrink(p), shrink_order(p), s);
		}
	}

void tex::short_display(ptr p)
	{
	int	n;

	for (; p != null; p = link(p)) {
		if (is_char_node(p)) {
			if (font(p) != font_in_short_display) {
				if (font(p) < FONT_BASE || font(p) > FONT_MAX) {
					print("*");
				} else {
					print_esc(null_str);
					print(font_id_text(font(p)));
				}
				print(" ");
				font_in_short_display = font(p);
			}
			print_ASCII(character(p));
		} else {
			switch (tex::type(p))
			{
			case HLIST_NODE:
			case VLIST_NODE: 
			case INS_NODE:
			case WHATSIT_NODE:
			case MARK_NODE:
			case ADJUST_NODE:
			case UNSET_NODE:
				print("[]");
				break;

			case RULE_NODE:
				print("|");
				break;

			case GLUE_NODE:
				if (glue_ptr(p) != zero_glue)
					print(" ");
				break;

			case MATH_NODE:
				print("$");
				break;

			case LIGATURE_NODE:
				short_display(lig_ptr(p));
				break;

			case DISC_NODE:
				short_display(pre_break(p));
				short_display(post_break(p));
				n = replace_count(p);
				while (n > 0) {
					if (link(p) != null)
						p = link(p);
					decr(n);
				}
				break;

			default:
				break;
			}
		}
	}
}

void tex::show_box(ptr p)
	{
	depth_threshold = show_box_depth;
	breadth_max = show_box_breadth;
	if (breadth_max <= 0)
		breadth_max = 5;
	flush_str();
	if (cur_str_ptr + depth_threshold >= cur_str_end)
		depth_threshold = cur_str_end - cur_str_ptr - 1;
	show_node_list(p);
	}

void tex::show_glue_set(ptr p)
	{
	if (glue_set(p) != 0 && glue_sign(p) != NORMAL) {
		print(", glue set ");
		if (glue_sign(p) == SHRINKING)
			print("- ");
		if (fabs(glue_set(p)) > 20000.0) {
			if (glue_set(p) > 0)
				print(">");
			else print("< -");
			print_glue(20000 * UNITY, glue_order(p), null_str);
			} 
		else {
			print_glue((int)round(glue_set(p) * UNITY),
						  glue_order(p), null_str);
			}
		}
	}

void tex::show_box1(ptr p)
	{
	if (tex::type(p) == HLIST_NODE)
		print_esc("h");
	else if (tex::type(p) == VLIST_NODE)
		print_esc("v");
	else print_esc("unset");
	print("box(");
	print_scaled(box_height(p));
	print("+")	;
	print_scaled(box_depth(p));
	print(")x")	;
	print_scaled(box_width(p));
	if (tex::type(p) == UNSET_NODE) {
		if (unset_span_count(p) != MIN_QUARTERWORD) {
			print(" (");
			print_int(unset_span_count(p)+1);
			print(" columns)");
		}
		if (unset_stretch(p) != 0) {
			print(", stretch ");
			print_glue(unset_stretch(p), glue_order(p), null_str);
		}
		if (unset_shrink(p) != 0) {
			print(", shrink ");
			print_glue(unset_shrink(p), glue_sign(p), null_str);
		}
	} else {
		show_glue_set(p);
		if (shift_amount(p) != 0) {
			print(", shifted ");
			print_scaled(shift_amount(p));
		}
	}
	node_list_display(list_ptr(p));
}

void tex::show_rule(ptr p)
	{
	print_esc("rule(");
	print_rule_dimen(rule_height(p));
	print("+");
	print_rule_dimen(rule_depth(p));
	print(")x");
	print_rule_dimen(rule_width(p));
	}

void tex::show_insertion(ptr p)
	{
	print_esc("insert");
	print_int(tex::subtype(p));
	print(", natural size ");
	print_scaled(ins_height(p));
	print("; split(");
	print_spec(split_top_ptr(p), null_str);
	print(",");
	print_scaled(ins_depth(p));
	print("); float cost ");
	print_int(float_cost(p));
	node_list_display(ins_ptr(p));
	}

void tex::show_leaders(ptr p)
	{
	print_esc(null_str);
	if (tex::subtype(p) == C_LEADERS)
		print("c");
	else if (tex::subtype(p) == X_LEADERS)
		print("x");
	print("leaders ");
	print_spec(glue_ptr(p), null_str);
	node_list_display(leader_ptr(p));
}

void tex::show_glue_type(int n)
	{
	print("(");
	if (n < COND_MATH_GLUE) {
		if (n <= GLUE_PARS) {
			print_skip_param(n - 1);
			} 
		else {
			print_mu_skip_param(n - 1 - GLUE_PARS);
			}
		} 
	else if (n == COND_MATH_GLUE) {
		print_esc("nonscript");
		} 
	else {
		print_esc("mskip");
		}
	print(")");
	}

void tex::show_glue(ptr p)
	{
	if (tex::subtype(p) >= A_LEADERS) {
		show_leaders(p);
	} else {
		print_esc("glue");
		if (tex::subtype(p) != NORMAL)
			show_glue_type(tex::subtype(p));
		if (tex::subtype(p) != COND_MATH_GLUE) {
			print(" ");
			if (tex::subtype(p) < COND_MATH_GLUE)
				print_spec(glue_ptr(p), null_str);
			else print_spec(glue_ptr(p), "mu");
		}
	}
}

void tex::show_kern(ptr p)
	{
	if (subtype(p) != MU_GLUE) {
		print_esc("kern");
		if (subtype(p) != NORMAL)
			print(" ");
		print_scaled(kern_width(p));
		if (subtype(p) == ACC_KERN)
			print(" (for accent)");
	} else {
		print_esc("mkern");
		print_scaled(kern_width(p));
		print("mu");
	}
	}

void tex::show_math(ptr p)
	{
	print_esc("math");
	if (subtype(p) == BEFORE) {
		print("on");
	} else {
		print("off");
	}
	if (math_width(p) != 0) {
		print(", surrounded ");
		print_scaled(math_width(p));
	}
	}
	
void tex::show_ligature(ptr p)
	{
	print_font_and_char(lig_char(p));
	print(" (ligature ");
	if (subtype(p) > 1)
		print("|");
	font_in_short_display = font(lig_char(p));
	short_display(lig_ptr(p));
	if (odd(subtype(p)))
		print("|");
	print(")");
	}

void tex::show_discretionary(ptr p)
	{
	print_esc("discretionary");
	if (replace_count(p) > 0) {
		print(" replacing ");
		print_int(replace_count(p));
	}
	node_list_display(pre_break(p));
	append_char('|');
	show_node_list(post_break(p));
	flush_char();
	}

void tex::show_penalty(ptr p)
	{
	print_esc("penalty ");
	print_int(penalty(p));
	}

void tex::show_mark(ptr p)
	{
	print_esc("mark");
	print_mark(mark_ptr(p));
	}

void tex::show_adjust(ptr p)
	{
	print_esc("vadjust");
	node_list_display(adjust_ptr(p));
	}

void tex::show_node_list(ptr p)
	{
	int	n;

	if (cur_length() > depth_threshold) {
		if (p > null)
			print(" []");
		return;
	}
	n = 0;
	while (p > null) {
		print_ln();
		print_str();
		incr(n);
		if (n > breadth_max) {
			print("etc.");
			return;
		}
		if (is_char_node(p)) {
			print_font_and_char(p);
		} else {
			switch (type(p))
			{
			case HLIST_NODE:
			case VLIST_NODE:
			case UNSET_NODE:
				show_box1(p);
				break;

			case RULE_NODE:
				show_rule(p);
				break;
			
			case INS_NODE:
				show_insertion(p);
				break;
			
			case WHATSIT_NODE:
				show_whatsit(p);
				break;
			
			case GLUE_NODE:
				show_glue(p);
				break;
			
			case KERN_NODE:
				show_kern(p);
				break;
			
			case MATH_NODE:
				show_math(p);
				break;
			
			case LIGATURE_NODE:
				show_ligature(p);
				break;

			case PENALTY_NODE:
				show_penalty(p);
				break;
		
			case DISC_NODE:
				show_discretionary(p);
				break;
			
			case MARK_NODE:
				show_mark(p);
				break;
			
			case ADJUST_NODE:
				show_adjust(p);
				break;
			
			case STYLE_NODE:
				print_style(subtype(p));
				break;
			
			case CHOICE_NODE:
				show_choice_node(p);
				break;

			case INNER_NOAD:
			case ORD_NOAD:
			case OP_NOAD:
			case BIN_NOAD:
			case REL_NOAD:
			case OPEN_NOAD:
			case CLOSE_NOAD:
			case PUNCT_NOAD:
			case RADICAL_NOAD:
			case OVER_NOAD:
			case UNDER_NOAD:
			case VCENTER_NOAD:
			case ACCENT_NOAD:
			case LEFT_NOAD:
			case RIGHT_NOAD:
				show_normal_noad(p);
				break;
			
			case FRACTION_NOAD:
				show_fraction_noad(p);
				break;

			default:
				print("Unknown node type!");
				break;
			}
		}
		p = link(p);
	}
}

void tex::delete_glue_ref(ptr p)
	{
	if (glue_ref_count(p) == 0) {
		free_node(p, GLUE_SPEC_SIZE);
		} 
	else {
		decr(glue_ref_count(p));
		}
	}

void tex::flush_node_list(ptr p)
	{
	ptr	q;

	while (p != null) {
		q = link(p);
		if (is_char_node(p)) {
			free_avail(p);
		} else {
			switch (type(p))
			{
			case HLIST_NODE:
			case VLIST_NODE:
			case UNSET_NODE:
				flush_node_list(list_ptr(p));
				free_node(p, BOX_NODE_SIZE);
				goto done;
					
			case RULE_NODE:
				free_node(p, RULE_NODE_SIZE);
				goto done;

			case INS_NODE:
				flush_node_list(ins_ptr(p));
				delete_glue_ref(split_top_ptr(p));
				free_node(p, INS_NODE_SIZE);
				goto done;

			case WHATSIT_NODE:
				free_whatsit(p);
				goto done;
			
			case GLUE_NODE:
				fast_delete_glue_ref(glue_ptr(p));
				if (leader_ptr(p) != null)
					flush_node_list(leader_ptr(p));
				break;

			case KERN_NODE:
			case MATH_NODE:
			case PENALTY_NODE:
				break;

			case LIGATURE_NODE:
				flush_node_list(lig_ptr(p));
				break;

			case MARK_NODE:
				delete_token_ref(mark_ptr(p));
				break;
			
			case DISC_NODE:
				flush_node_list(pre_break(p));
				flush_node_list(post_break(p));
				break;

			case ADJUST_NODE:
				flush_node_list(adjust_ptr(p));
				break;

			case STYLE_NODE:
				free_node(p, STYLE_NODE_SIZE);
				goto done;

			case CHOICE_NODE:
				flush_node_list(display_mlist(p));
				flush_node_list(text_mlist(p));
				flush_node_list(script_mlist(p));
				flush_node_list(script_script_mlist(p));
				free_node(p, STYLE_NODE_SIZE);
				goto done;

			case ORD_NOAD:
			case OP_NOAD:
			case BIN_NOAD:
			case REL_NOAD:
			case OPEN_NOAD:
			case CLOSE_NOAD:
			case PUNCT_NOAD:
			case INNER_NOAD:
			case RADICAL_NOAD:
			case OVER_NOAD:
			case UNDER_NOAD:
			case VCENTER_NOAD:
			case ACCENT_NOAD:
				if (math_type(nucleus(p)) >= SUB_BOX)
					flush_node_list(math_link(nucleus(p)));
				if (math_type(supscr(p)) >= SUB_BOX)
					flush_node_list(math_link(supscr(p)));
				if (math_type(subscr(p)) >= SUB_BOX)
					flush_node_list(math_link(subscr(p)));
				if (type(p) == RADICAL_NOAD)
					free_node(p, RADICAL_NOAD_SIZE);
				else if (type(p) == ACCENT_NOAD)
					free_node(p, ACCENT_NOAD_SIZE);
				else free_node(p, NOAD_SIZE);
				goto done;
			
			case LEFT_NOAD:
			case RIGHT_NOAD:
				free_node(p, NOAD_SIZE);
				goto done;
			
			case FRACTION_NOAD:
				flush_node_list(math_link(numerator(p)));
				flush_node_list(math_link(denominator(p)));
				free_node(p, FRACTION_NOAD_SIZE);
				goto done;

			default:
				confusion("flushing");
				break;
			}
			free_node(p, SMALL_NODE_SIZE);
			done:;	
		}
		p = q;
	}
}

ptr tex::copy_node_list(ptr p)
	{
	ptr	h;
	ptr	q;
	ptr	r=0;

	h = q = new_avail();
	while (p != null) {
		if (is_char_node(p)) {
			r = new_avail();
			font(r) = font(p);
			character(r) = character(p);
		} else {
			switch (type(p))
			{
			case HLIST_NODE:
			case VLIST_NODE:
			case UNSET_NODE:
				r = tex::new_node(BOX_NODE_SIZE);
				memcpy((void *)r, (void *)p, BOX_NODE_SIZE);
				list_ptr(r) = copy_node_list(list_ptr(p));
				break;
			
			case RULE_NODE:
				r = tex::new_node(RULE_NODE_SIZE);
				memcpy((void *)r, (void *)p, RULE_NODE_SIZE);
				break;

			case INS_NODE:
				r = tex::new_node(INS_NODE_SIZE);
				memcpy((void *)r, (void *)p, INS_NODE_SIZE);
				add_glue_ref(split_top_ptr(p));
				ins_ptr(r) = copy_node_list(ins_ptr(p));
				break;

			case WHATSIT_NODE:
				r = copy_whatsit(p);
				break;
			
			case GLUE_NODE:
				r = tex::new_node(SMALL_NODE_SIZE);
				memcpy((void *)r, (void *)p, SMALL_NODE_SIZE);
				add_glue_ref(glue_ptr(p));
				glue_ptr(r) = glue_ptr(p);
				leader_ptr(r) = copy_node_list(leader_ptr(p));
				break;
			
			case KERN_NODE:
			case MATH_NODE:
			case PENALTY_NODE:
				r = tex::new_node(SMALL_NODE_SIZE);
				memcpy((void *)r, (void *)p, SMALL_NODE_SIZE);
				break;
			
			case LIGATURE_NODE:
				r = tex::new_node(SMALL_NODE_SIZE);
				memcpy((void *)r, (void *)p, SMALL_NODE_SIZE);
				lig_ptr(r) = copy_node_list(lig_ptr(p));
				break;
				
			case DISC_NODE:
				r = tex::new_node(SMALL_NODE_SIZE);
				memcpy((void *)r, (void *)p, SMALL_NODE_SIZE);
				pre_break(r) = copy_node_list(pre_break(p));
				post_break(r) = copy_node_list(post_break(p));
				break;

			case MARK_NODE:
				r = tex::new_node(SMALL_NODE_SIZE);
				memcpy((void *)r, (void *)p, SMALL_NODE_SIZE);
				add_token_ref(mark_ptr(p));
				break;

			case ADJUST_NODE:
				r = tex::new_node(SMALL_NODE_SIZE);
				memcpy((void *)r, (void *)p, SMALL_NODE_SIZE);
				adjust_ptr(r) = copy_node_list(adjust_ptr(p));
				break;
			
			default:
				confusion("copying");
				break;
			}
		}
		q = link(q) = r;
		p = link(p);
	}
	link(q) = null;
	q = link(h);
	free_avail(h);
	return q;
}

void tex::_box_init ()
	{
	}

void tex::_box_init_once ()
	{
	}
