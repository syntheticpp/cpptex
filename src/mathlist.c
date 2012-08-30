
/*
 * $Id: mathlist.c,v 1.6 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include <cstdlib>
#include "tex.h"

void tex::push_math(int c)
	{
	push_nest();
	mode = -MMODE;
	incompleat_noad = null;
	new_save_level(c);
	}

void tex::init_math ()
	{
	fnt	f;
	int	m, n;
	ptr	p, q;
	scal	d, l, s, v, w;

	get_token();
	if (cur_cmd == MATH_SHIFT && mode > 0) {
		if (head == tail) {
			pop_nest();
			w = -MAX_DIMEN;
		} else {
			line_break(display_widow_penalty);
			v = shift_amount(just_box) + 2 * quad(cur_font);
			w = -MAX_DIMEN;
			p = list_ptr(just_box);
			while (p != null) {
		reswitch:
				if (is_char_node(p)) {
					f = font(p);
					d = char_width(f,
						char_info(f, character(p)));
					goto found;
				}
				switch (type(p))
				{
				case HLIST_NODE:
				case VLIST_NODE:
				case RULE_NODE:
					d = box_width(p);
					goto found;
				
				case LIGATURE_NODE:
					p = make_char_from_lig(p);
					goto reswitch;
				
				case KERN_NODE:
					d = kern_width(p);
					break;

				case MATH_NODE:
					d = box_width(p);
					break;

				case GLUE_NODE:
					q = glue_ptr(p);
					d = box_width(q);
					if (glue_sign(just_box) ==
						STRETCHING) {
						if (glue_order(just_box) ==
							stretch_order(q)
						&& stretch(q) != 0)
							v = MAX_DIMEN;
					} else if (glue_sign(just_box) ==
						SHRINKING)  {
						if (glue_order(just_box) ==
							shrink_order(q)
						&& shrink(q) != 0) 
							v = MAX_DIMEN;
					}
					if (subtype(p) >= A_LEADERS)	
						goto found;
					break;

				case WHATSIT_NODE: 
					d = 0;
					break;

				default:
					d = 0;
					break;
				}
				if (v < MAX_DIMEN)
					v += d;
				goto not_found;
			
			found:
				if (v < MAX_DIMEN) {
					v += d;
					w = v;
				} else {
					w = MAX_DIMEN;
					break;
				}

			not_found:
				p = link(p);
			}
		}
		if (par_shape_ptr == null) {
			if (hang_indent != 0
			&& (hang_after >= 0
				&& prev_graf + 2 > hang_after
					|| prev_graf + 1 < -hang_after)) {
				l = hsize - abs(hang_indent);
				s = (hang_indent > 0) ? hang_indent : 0;
			} else {
				l = hsize;
				s = 0;
			}
		} else {
			n = info(par_shape_ptr);
			if (prev_graf + 2 >= n)
				m = n;
			else m = prev_graf + 2;
			l = par_shape_width(m);
			s = par_shape_indent(m);
		}
		push_math(MATH_SHIFT_GROUP);
		mode = MMODE;
		reg_define(int_reg[CUR_FAM_CODE], INT_REG, -1);
		reg_define(dimen_reg[PRE_DISPLAY_SIZE_CODE], DIMEN_REG, w);
		reg_define(dimen_reg[DISPLAY_WIDTH_CODE], DIMEN_REG, l);
		reg_define(dimen_reg[DISPLAY_INDENT_CODE], DIMEN_REG, s);
		if (every_display != null)
			begin_token_list(every_display, EVERY_DISPLAY_TEXT);
		if (nest_ptr == nest + 1)
			build_page();
	} else {
		back_input(); 
		push_math(MATH_SHIFT_GROUP);
		reg_define(int_reg[CUR_FAM_CODE], INT_REG, -1);
		if (every_math != null)
			begin_token_list(every_math, EVERY_MATH_TEXT);
	}
}

void tex::start_eq_no ()
	{
	saved(0) = cur_chr;
	incr(save_ptr);
	push_math(MATH_SHIFT_GROUP);
	reg_define(int_reg[CUR_FAM_CODE], INT_REG, -1);
	if (every_math != null)
		begin_token_list(every_math, EVERY_MATH_TEXT);
	}

#define fam_in_range()	(cur_fam >= 0 && cur_fam < 16)

void tex::scan_math(ptr p)
	{
	int	c;

restart:
	get_nbrx_token();

reswitch:
	switch (cur_cmd)
	{
	case LETTER:
	case OTHER_CHAR:
	case CHAR_GIVEN:
		c = math_code(cur_chr);
		if (c == 0100000) {
			cur_cs = active_base[cur_chr];
			cur_cmd = eq_type(cur_cs);
			cur_chr = equiv(cur_cs);
			x_token();
			back_input();
			goto restart;
		}
		break;
	
	case CHAR_NUM:
		scan_char_num();
		cur_chr = cur_val;
		cur_cmd = CHAR_GIVEN;
		goto reswitch;
	
	case MATH_CHAR_NUM:
		scan_fifteen_bit_int();
		c = cur_val;
		break;

	case MATH_GIVEN:
		c = cur_chr;
		break;
		
	case DELIM_NUM:
		scan_twenty_seven_bit_int();
		c = cur_val / 010000;
		break;

	default:
		back_input();
		scan_left_brace();
		saved(0) = p;
		incr(save_ptr);
		push_math(MATH_GROUP);
		return;
	}
	math_type(p) = MATH_CHAR;
	character(p) = c % 256;
	if (c >= VAR_CODE && fam_in_range()) {
		fam(p) = cur_fam;
	} else {
		fam(p) = c / 256 % 16;
	}
	}

void tex::set_math_char(int c)
	{
	ptr	p;

	if (c >= 0100000) {
		cur_cs = active_base[cur_chr];
		cur_cmd = eq_type(cur_cs);
		cur_chr = equiv(cur_cs);
		x_token();
		back_input();
	} else {
		p = new_noad();
		math_type(nucleus(p)) = MATH_CHAR;
		character(nucleus(p)) = c % 256;
		fam(nucleus(p)) = c / 256 % 16;
		if (c >= VAR_CODE) {
			if (fam_in_range())
				fam(nucleus(p)) = cur_fam;
			type(p) = ORD_NOAD;
		} else {
			type(p) = ORD_NOAD + c / 010000;
		}
		tail_append(p);
	}
}

void tex::math_limit_switch ()
	{
	if (head != tail && type(tail) == OP_NOAD) {
		subtype(tail) = cur_chr;
		return;
		}
	print_err("Limit controls must follow a math operator");
	help_limits();
	error();
	}

void tex::scan_delimiter(ptr p, bool r)
	{
	if (r) {
		scan_twenty_seven_bit_int();
	} else {
		get_nbrx_token();
		switch (cur_cmd) 
		{
		case LETTER:
		case OTHER_CHAR:
			cur_val = del_code(cur_chr);
			break;

		case DELIM_NUM:
			scan_twenty_seven_bit_int();
			break;

		default:
			cur_val = -1;
			break;
		}
	}
	if (cur_val < 0) {
		print_err("Missing delimiter (. inserted)");
		help_delimiter();
		back_error();
		cur_val = 0;
	}
	small_fam(p) = cur_val / 04000000 % 16;
	small_char(p) = cur_val / 010000 % 256;
	large_fam(p) = cur_val / 256 % 16;
	large_char(p) = cur_val % 256;
}

void tex::math_radical ()
	{
	tail_append(new_node(RADICAL_NOAD_SIZE));
	type(tail) = RADICAL_NOAD;
	subtype(tail) = NORMAL;
	mzero(nucleus(tail));
	mzero(supscr(tail));
	mzero(subscr(tail));
	scan_delimiter(left_delimiter(tail), TRUE);
	scan_math(nucleus(tail));
	}

void tex::math_ac ()
	{
	if (cur_cmd == ACCENT) {
		print_err("Please use ");
		print_esc("mathaccent");
		print(" for accents in math mode");
		help_math_accent();
		error();
	}
	tail_append(new_node(ACCENT_NOAD_SIZE));
	type(tail) = ACCENT_NOAD;
	subtype(tail) = NORMAL;
	mzero(nucleus(tail));
	mzero(subscr(tail));
	mzero(supscr(tail));
	math_type(accent_chr(tail)) = MATH_CHAR;
	scan_fifteen_bit_int();
	character(accent_chr(tail)) = cur_val % 256;
	if (cur_val >= VAR_CODE && fam_in_range()) {
		fam(accent_chr(tail)) = cur_fam;
	} else {
		fam(accent_chr(tail)) = cur_val / 256 % 16;
	}
	scan_math(nucleus(tail));
	}

void tex::append_choices ()
	{
	tail_append(new_choice());
	incr(save_ptr);
	saved(-1) = 0;
	scan_left_brace();
	push_math(MATH_CHOICE_GROUP);
	}

void tex::build_choices ()
	{
	ptr	p;
	
	unsave();
	p = fin_mlist(null);
	switch (saved(-1)) {
	case 0: display_mlist(tail) = p; break;
	case 1: text_mlist(tail) = p; break;
	case 2: script_mlist(tail) = p; break;
	case 3: script_script_mlist(tail) = p; decr(save_ptr); return;
	}
	incr(saved(-1));
	scan_left_brace();
	push_math(MATH_CHOICE_GROUP);
	}

void tex::sub_sup ()
	{
	ptr	p;
	int	t;

	p = null;
	t = EMPTY;
	if (tail != head && scripts_allowed(tail)) {
		p = (cur_cmd == SUP_MARK) ? supscr(tail) : subscr(tail);
		t = math_type(p);
	}
	if (p == null || t != EMPTY) {
		tail_append(new_noad());
		p = (cur_cmd == SUP_MARK) ? supscr(tail) : subscr(tail);
		if (t != EMPTY) {
			if (cur_cmd == SUP_MARK) {
				print_err("Double superscript");
				help_double_sup();
			} else {
				print_err("Double subscript");
				help_double_sub();
			}
			error();
		}
	}
	scan_math(p);
}

void tex::math_fraction ()
	{
	int	c;
	mcell	garbage;

	c = cur_chr;
	if (incompleat_noad != null) {
		if (c >= DELIMITED_CODE) {
			scan_delimiter((ptr)&garbage, FALSE);
			scan_delimiter((ptr)&garbage, FALSE);
		}
		if (c % DELIMITED_CODE == ABOVE_CODE)
			scan_normal_dimen();
		print_err("Ambiguous; you need another { and }");
		help_fraction();
		error();
	} else {
		incompleat_noad = new_node(FRACTION_NOAD_SIZE);
		type(incompleat_noad) = FRACTION_NOAD;
		subtype(incompleat_noad) = NORMAL;
		math_type(numerator(incompleat_noad)) = SUB_MLIST;
		math_link(numerator(incompleat_noad)) = link(head);
		mzero(denominator(incompleat_noad));
		mzero(left_delimiter(incompleat_noad));
		mzero(right_delimiter(incompleat_noad));
		link(head) = null;
		tail = head;
		if (c >= DELIMITED_CODE) {
			scan_delimiter(left_delimiter(incompleat_noad), FALSE);
			scan_delimiter(right_delimiter(incompleat_noad), FALSE);
		}
		switch (c % DELIMITED_CODE)
		{
		case ABOVE_CODE:
			scan_normal_dimen();
			thickness(incompleat_noad) = cur_val;
			break;

		case OVER_CODE:
			thickness(incompleat_noad) = DEFAULT_CODE;
			break;

		case ATOP_CODE:
			thickness(incompleat_noad) = 0;
			break;
		}
	}
}


void tex::math_left_right ()
	{
	ptr	p;
	int	t;
	mcell	garbage;

	t = cur_chr;
	if (t == RIGHT_NOAD && cur_group != MATH_LEFT_GROUP) {
		if (cur_group == MATH_SHIFT_GROUP) {
			scan_delimiter((ptr)&garbage, FALSE);
			print_err("Extra ");
			print_esc("right");
			help_xtra_right();
			error();
		} else {
			off_save();
		}
	} else {
		p = new_noad();
		type(p) = t;
		scan_delimiter(delimiter(p), FALSE);
		if (t == LEFT_NOAD) {
			push_math(MATH_LEFT_GROUP);
			tail = link(head) = p;
		} else {
			p = fin_mlist(p);
			unsave();
			tail_append(new_noad());
			type(tail) = INNER_NOAD;
			math_type(nucleus(tail)) = SUB_MLIST;
			math_link(nucleus(tail)) = p;
		}
	}
}

ptr tex::fin_mlist(ptr p)
	{
	ptr	q;

	if (incompleat_noad != null) {
		math_type(denominator(incompleat_noad)) = SUB_MLIST;
		math_link(denominator(incompleat_noad)) = link(head);
		if (p == null) {
			q = incompleat_noad;
		} else {
			q = info(numerator(incompleat_noad));
			if (type(q) != LEFT_NOAD)
				confusion("right");
			math_link(numerator(incompleat_noad)) = link(q);
			link(q) = incompleat_noad;
			link(incompleat_noad) = p;
		}
	} else {
		link(tail) = p;
		q = link(head);
	}
	pop_nest();
	return q;
}

void tex::after_math ()
{
	ptr	a;
	ptr	b;
	scal	d;
	scal	e;
	bool	l;
	int	m;
	ptr	p;
	scal	q;
	ptr	r;
	scal	s;
	scal	t;
	scal	w;
	scal	z;
	int	g1;
	int	g2;
	bool	danger;

	danger = check_font_params();
	m = mode;
	l = FALSE;
	p = fin_mlist(null);
	if (mode == -m) {
		check_dollar();
		a = hpack(mlist_to_hlist(p, TEXT_STYLE, FALSE), 0, ADDITIONAL);
		unsave();
		decr(save_ptr);
		if (saved(0) == 1)
			l = TRUE;
		danger = check_font_params();
		m = mode;
		p = fin_mlist(null);
	} else {
		a = null;
	}
	if (m < 0) {
		tail_append(new_math(math_surround, BEFORE));
		link(tail) = mlist_to_hlist(p, TEXT_STYLE, mode > 0);
		while (link(tail) != null)
			tail = link(tail);
		tail_append(new_math(math_surround, AFTER));
		space_factor = 1000;
		unsave();
	} else {
		if (a == null)
			check_dollar();
		p = mlist_to_hlist(p, DISPLAY_STYLE, FALSE);
		tex::adjust_tail = tex::adjust_head;
		b = hpack(p, 0, ADDITIONAL);
		p = list_ptr(b);
		t = tex::adjust_tail;
		tex::adjust_tail = null;
		w = box_width(b);
		z = display_width;
		s = display_indent;
		if (a == null || danger) {
			e = q = 0;
		} else {
			e = box_width(a);
			q = e + math_quad(TEXT_SIZE);
		}
		if (w + q > z) {
			if (e != 0
			&& (w - tex::total_shrink[NORMAL] + q <= z
			|| tex::total_shrink[FIL] != 0
			|| tex::total_shrink[FILL] != 0
			|| tex::total_shrink[FILLL] != 0)) {
				free_node(b, BOX_NODE_SIZE);
				b = hpack(p, z - q, EXACTLY);
			} else {
				e = 0;
				if (w > z) {
					free_node(b, BOX_NODE_SIZE);
					b = hpack(p, z, EXACTLY);
				}
			}
			w = box_width(b);
		}
		d = half(z - w);
		if (e > 0 && d < 2 * e) {
			d = half(z - w - e);
			if (p != null && type(p) == GLUE_NODE) {
				d = 0;
			}
		}
		tail_append(new_penalty(pre_display_penalty));
		if (d + s <= pre_display_size || l) {
			g1 = ABOVE_DISPLAY_SKIP_CODE;
			g2 = BELOW_DISPLAY_SKIP_CODE;
		} else {
			g1 = ABOVE_DISPLAY_SHORT_SKIP_CODE;
			g2 = BELOW_DISPLAY_SHORT_SKIP_CODE;
		}
		if (l && e == 0) {
			shift_amount(a) = s;
			append_to_vlist(a);
			tail_append(new_penalty(INF_PENALTY));
		} else {
			tail_append(new_param_glue(g1));
		}
		if (e != 0) {
			r = new_kern(z - w - e - d);
			if (l) {
				link(a) = r;
				link(r) = b;
				b = a;
				d = 0;
			} else {
				link(b) = r;
				link(r) = a;
			}
			b = hpack(b, 0, ADDITIONAL);
		}
		shift_amount(b) = s + d;
		append_to_vlist(b);
		if (a != null && e == 0 && !l) {
			tail_append(new_penalty(INF_PENALTY));
			shift_amount(a) = s + z - box_width(a);
			append_to_vlist(a);
			g2 = 0;
		}
		if (t != tex::adjust_head) {
			link(tail) = link(tex::adjust_head);
			tail = t;
		}
		tail_append(new_penalty(post_display_penalty));
		if (g2 > 0) {
			tail_append(new_param_glue(g2));
		}
		resume_after_display();
	}
}

void tex::check_dollar ()
{
	get_x_token();
	if (cur_cmd != MATH_SHIFT) {
		print_err("Display math should end with $$");
		help_doldol();
		back_error();
	}
}

bool tex::check_font_params ()
{
#define CHK_SY(S) font_params(fam_fnt(2 + (S))) < TOTAL_MATHSY_PARAMS
#define CHK_EX(S) font_params(fam_fnt(3 + (S))) < TOTAL_MATHEX_PARAMS
	if (CHK_SY(TEXT_SIZE)
	|| CHK_SY(SCRIPT_SIZE)
	|| CHK_SY(SCRIPT_SCRIPT_SIZE)) {
		print_err("Math formula deleted: Insufficient symbol fonts");
		help_math_sy();
		error();
		flush_math();
		return TRUE;
	} 
	if (CHK_EX(TEXT_SIZE)
	|| CHK_EX(SCRIPT_SIZE)
	|| CHK_EX(SCRIPT_SCRIPT_SIZE)) {
		print_err("Math formula deleted: Insufficient extension fonts");
		help_math_ex();
		error();
		flush_math();
		return TRUE;
	}
	return FALSE;
}

void tex::resume_after_display ()
	{
	if (cur_group != MATH_SHIFT_GROUP)
		confusion("display");
	unsave();
	prev_graf += 3;
	push_nest();
	mode = HMODE;
	space_factor = 1000;
	clang = 0;
	scan_optional_space();
	if (nest_ptr == nest + 1) 
		build_page();
	}

void tex::_mathlist_init ()
	{
	}

void tex::_mathlist_init_once ()
	{
	}

/*
**	Help text
*/

void tex::help_math_accent()
	{
	help2("I'm changing \\accent to \\mathaccent here; wish me luck.",
	"(Accents are not the same in formulas as they are in text.)" );
	}

void tex::help_math_sy()
	{
	help3("Sorry, but I can't typeset math unless \\textfont 2",
	"and \\scriptfont 2 and \\scriptscriptfont 2 have all",
	"the \\fontdimen values needed in math symbol fonts." );
	}

void tex::help_math_ex()
	{
	help3("Sorry, but I can't typeset math unless \\textfont 3",
	"and \\scriptfont 3 and \\scriptscriptfont 3 have all",
	"the \\fontdimen values needed in math extension fonts." );
	}

void tex::help_limits()
	{
	help1("I'm ignoring this misplaced \\limits or \\nolimits command.");
	}

void tex::help_delimiter()
	{
	help6("I was expecting to see something like `(' or `\\{' or",
	"`\\}' here. If you typed, e.g., `{' instead of `\\{', you",
	"should probably delete the `{' by typing `1' now, so that",
	"braces don't get unbalanced. Otherwise just proceed.",
	"Acceptable delimiters are characters whose \\delcode is",
	"nonnegative, or you can use `\\delimiter <delimiter code>'.");
	}

void tex::help_fraction()
	{
	help3("I'm ignoring this fraction specification, since I don't",
	"know whether a construction like `x \\over y \\over z'",
	"means `{x \\over y} \\over z' or `x \\over {y \\over z}'.");
	}

void tex::help_xtra_right()
	{
	help1("I'm ignoring a \\right that had no matching \\left.");
	}

void tex::help_doldol()
	{
	help2("The `$' that I just saw supposedly matches a previous `$$'.",
	"So I shall assume that you typed `$$' both times.");
	}

void tex::help_double_sub()
	{
	help1("I treat `x_1_2' essentially like `x_1{}_2'.");
	}

void tex::help_double_sup()
	{
	help1("I treat `x^1^2' essentially like `x^1{}^2'.");
	}
