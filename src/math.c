
/*
 * $Id: math.c,v 1.6 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include "tex.h"

ptr tex::new_style(int s)
	{
	ptr	p;

	p = new_node(STYLE_NODE_SIZE);
	type(p) = STYLE_NODE;
	subtype(p) = s;
	display_mlist(p) = null;
	text_mlist(p) = null;
	script_mlist(p) = null;
	script_script_mlist(p) = null;

	return p;
	}

ptr tex::new_choice ()
	{
	ptr	p;

	p = new_node(STYLE_NODE_SIZE);
	type(p) = CHOICE_NODE;
	subtype(p) = 0;
	display_mlist(p) = null;
	text_mlist(p) = null;
	script_mlist(p) = null;
	script_script_mlist(p) = null;

	return p;
	}

ptr tex::new_noad ()
	{
	ptr	p;

	p = new_node(NOAD_SIZE);
	type(p) = ORD_NOAD;
	subtype(p) = NORMAL;
	mzero((void *)nucleus(p));
	mzero((void *)subscr(p));
	mzero((void *)supscr(p));

	return p;
	}

void tex::print_fam_and_char(ptr p)
	{
	print_esc("fam");
	print_int(fam(p));
	print(" ");
	print_ASCII(character(p));
	}

void tex::print_delimiter(ptr p)
	{
	int	a;

	a = small_fam(p) * 256 + small_char(p);
	a = a * 0x1000 + large_fam(p) * 256 + large_char(p);
	if (a < 0)
		print_int(a);
	else print_hex(a);
	}

void tex::print_subsidiary_data(ptr p, int c)
	{
	if (cur_length() >= depth_threshold) {
		if (math_type(p) != EMPTY)
			print(" []");
		return;
	}
	append_char(c);
	switch (math_type(p)) {
		case MATH_CHAR:
			print_ln();
			print_str();
			print_fam_and_char(p);
			break;
			
		case SUB_BOX:
			show_node_list(math_link(p));
			break;
			
		case SUB_MLIST:
			if (math_link(p) == null) {
				print_ln();
				print_str();
				print("{}");
				} 
			else {
				show_node_list(math_link(p));
				}
			break;
		}
	flush_char();
	}

void tex::print_style(int c)
	{
	switch (c / 2) {
		case 0: print_esc("displaystyle"); break;
		case 1: print_esc("textstyle"); break;
		case 2: print_esc("scriptstyle"); break;
		case 3: print_esc("scriptscriptstyle"); break;
		default: print("Unknown style!"); break;
		}
	}

void tex::print_size(int s)
	{
	if (s == TEXT_SIZE)
	  print_esc("textfont");
	else if (s == SCRIPT_SIZE)
	  print_esc("scriptfont");
	else print_esc("scriptscriptfont");
	}

void tex::show_normal_noad(ptr p)
	{
	switch (type(p)) 
	{
	case ORD_NOAD:
		print_esc("mathord");
		break;

	case OP_NOAD:
		print_esc("mathop");
		break;

	case BIN_NOAD:
		print_esc("mathbin");
		break;

	case REL_NOAD:
		print_esc("mathrel");
		break;

	case OPEN_NOAD:
		print_esc("mathopen");
		break;

	case CLOSE_NOAD:	
		print_esc("mathclose");
		break;

	case PUNCT_NOAD:
		print_esc("mathpunct");
		break;

	case INNER_NOAD:
		print_esc("mathinner");
		break;

	case OVER_NOAD:
		print_esc("overline");
		break;

	case UNDER_NOAD:
		print_esc("underline");
		break;

	case VCENTER_NOAD:
		print_esc("vcenter");
		break;

	case RADICAL_NOAD:
		print_esc("radical");
		print_delimiter(left_delimiter(p));
		break;

	case ACCENT_NOAD:
		print_esc("accent");
		print_fam_and_char(accent_chr(p));
		break;

	case LEFT_NOAD:
		print_esc("left");
		print_delimiter(nucleus(p));
		break;

	case RIGHT_NOAD:
		print_esc("right");
		print_delimiter(nucleus(p));
		break;

	}
	if (subtype(p) != NORMAL)
		if (subtype(p) == LIMITS)
			print_esc("limits");
		else print_esc("nolimits");
	if (type(p) < LEFT_NOAD)
		print_subsidiary_data(nucleus(p), '.');
	print_subsidiary_data(supscr(p), '^');
	print_subsidiary_data(subscr(p), '_');
}

void tex::show_fraction_noad(ptr p)
	{
	print_esc("fraction, thickness ");
	if (thickness(p) == DEFAULT_CODE)
		print("= default");
	else print_scaled(thickness(p));
	if (small_fam(left_delimiter(p)) != 0 ||
		small_char(left_delimiter(p)) != MIN_QUARTERWORD ||
		large_fam(left_delimiter(p)) != 0 ||
		large_char(left_delimiter(p)) != MIN_QUARTERWORD) {
		print(", left-delimiter ");
		print_delimiter(left_delimiter(p));
	}
	if (small_fam(right_delimiter(p)) != 0 ||
		small_char(right_delimiter(p)) != MIN_QUARTERWORD ||
		large_fam(right_delimiter(p)) != 0 ||
		large_char(right_delimiter(p)) != MIN_QUARTERWORD) {
		print(", right-delimiter ");
		print_delimiter(right_delimiter(p));
	}
	print_subsidiary_data(numerator(p), '\\');
	print_subsidiary_data(denominator(p), '/');
}

void tex::show_choice_node(ptr p)
	{
	print_esc("mathchoice");
	append_char('D');
	show_node_list(display_mlist(p));
	flush_char();
	append_char('T');
	show_node_list(text_mlist(p));
	flush_char();
	append_char('S');
	show_node_list(script_mlist(p));
	flush_char();
	append_char('s');
	show_node_list(script_script_mlist(p));
	flush_char();
	}

ptr tex::fraction_rule(scal t)
	{
	ptr	p;

	p = new_rule();
	rule_height(p) = t;
	rule_depth(p) = 0;

	return p;
	}

ptr tex::overbar(ptr b, scal k, scal t)
	{
	ptr	p;
	ptr	q;

	p = new_kern(k);
	link(p) = b;
	q = fraction_rule(t);
	link(q) = p;
	p = new_kern(t);
	link(p) = q;
	return (vpack(p, 0, ADDITIONAL));
	}

#define check_variants()						\
	y = x; 								\
	if (y >= font_bc(g) && y <= font_ec(g)) {			\
contin:		q = char_info(g, y); 					\
		if (char_exists(q)) { 					\
			if (char_tag(q) == EXT_TAG)  { 			\
				f = g; 					\
				c = y; 					\
				goto found; 				\
			}						\
			hd = height_depth(q);				\
			u = char_height(g, hd) + char_depth(g, hd);	\
			if (u > w) {					\
				f = g;					\
				c = y;					\
				w = u;					\
				if (u >= v)				\
					goto found;			\
			}						\
			if (char_tag(q) == LIST_TAG) {			\
				y = rem_byte(q);			\
				goto contin;				\
			}						\
		}							\
	}								\


ptr tex::var_delimiter(ptr d, int s, scal v)
	{
	ptr	b;
	fnt	f, g;
	int	m, n;
	qcell	q, r;
	scal	u, w;
	int	c=0, x, y, z;
	int	hd;
	bool	large_attempt;

	f = null_font;
	w = 0;
	large_attempt = FALSE;
	z = small_fam(d);
	x = small_char(d);
	loop {
		if (z != 0 || x != 0) {
			z += s + 16;
			do {
				z -= 16;
				g = fam_fnt(z);
				if (g != null_font) {
					check_variants();
				}
			} while (z >= 16);
		}
		if (large_attempt)
			goto found;
		large_attempt = TRUE;
		z = large_fam(d);
		x = large_char(d);
	}

found:
	if (f != null_font) {
		if (char_tag(q) == EXT_TAG) {
			b = new_null_box();
			type(b) = VLIST_NODE;
			r = exten_base(f)[rem_byte(q)];
			c = ext_rep(r);
			u = height_plus_depth(f, c);
			w = 0;
			q = char_info(f, c);
			box_width(b) = char_width(f, q) + char_italic(f, q);
			c = ext_bot(r); 
			if (c != MIN_QUARTERWORD)
				w += height_plus_depth(f, c);
			c = ext_mid(r);
			if (c != MIN_QUARTERWORD)
				w += height_plus_depth(f, c);
			c = ext_top(r);
			if (c != MIN_QUARTERWORD)
				w += height_plus_depth(f, c);
			n = 0;
			if (u > 0) {
				while (w < v) {
					w += u;
					incr(n);
					if (ext_mid(r) != MIN_QUARTERWORD)
						w += u;
				}
			}
			c = ext_bot(r);
			if (c != MIN_QUARTERWORD)
				stack_into_box(b, f, c);
			c = ext_rep(r);
			for (m = 1; m <= n; incr(m)) 
				stack_into_box(b, f, c);
			c = ext_mid(r);
			if (c != MIN_QUARTERWORD) {
				stack_into_box(b, f, c);
				c = ext_rep(r);
				for (m = 1; m <= n; incr(m))
					stack_into_box(b, f, c);
			}
			c = ext_top(r);
			if (c != MIN_QUARTERWORD)
				stack_into_box(b, f, c);
			box_depth(b) = w - box_height(b);
		} else {
			b = char_box(f, c);
		}
	} else {
		b = new_null_box();
		box_width(b) = null_delimiter_space;
	}
	shift_amount(b) = half(box_height(b) - box_depth(b)) - axis_height(s);
	return b;
}

ptr tex::char_box(fnt f, int c)
	{
	ptr	b;
	ptr	p;
	qcell	q;
	int	hd;

	q = char_info(f, c);
	hd = height_depth(q);
	b = new_null_box();
	box_width(b) = char_width(f, q) + char_italic(f, q);
	box_height(b) = char_height(f, hd);
	box_depth(b) = char_depth(f, hd);
	p = new_avail();
	character(p) = c;
	font(p) = f;
	list_ptr(b) = p;
	
	return b;
	}

void tex::stack_into_box(ptr b, fnt f, int c)
	{
	ptr	p;

	p = char_box(f, c);
	link(p) = list_ptr(b);
	list_ptr(b) = p;
	box_height(b) = box_height(p);
	}

scal tex::height_plus_depth(fnt f, int c)
	{
	qcell	q;
	int	hd;

	q = char_info(f, c);
	hd = height_depth(q);
	return (char_height(f, hd) + char_depth(f, hd));
}

ptr tex::rebox(ptr b, scal w)
	{
	fnt	f;
	ptr	p;
	scal	v;

	if (box_width(b) != w && list_ptr(b) != null) {
		if (type(b) == VLIST_NODE)
			b = hpack(b, 0, ADDITIONAL);
		p = list_ptr(b);
		if (is_char_node(p) && link(p) == null) {
			f = font(p);
			v = char_width(f, char_info(f, character(p)));
			if (v != box_width(b)) 
				link(p) = new_kern(box_width(b) - v);
		}
		free_node(b, BOX_NODE_SIZE);
		b = new_glue(ss_glue);
		link(b) = p;
		while (link(p) != null)
			p = link(p);
		link(p) = new_glue(ss_glue); 
		return (hpack(b, w, EXACTLY));
	} else {
		box_width(b) = w;
		return b;
	}
}

#define mu_mult(x) \
	nx_plus_y(n, x, xn_over_d(x, f, 0200000))

ptr tex::math_glue(ptr g, scal m)
	{
	scal	f;
	int	n;
	ptr	p;

	n = x_over_n(m, 0200000);
	f = tex::remainder;
	p = new_node(GLUE_SPEC_SIZE);
	glue_width(p) = mu_mult(glue_width(g));
	stretch_order(p) = stretch_order(g);
	if (stretch_order(p) == NORMAL)
		stretch(p) = mu_mult(stretch(g));
	else stretch(p) = stretch(g);
	shrink_order(p) = shrink_order(g);
	if (shrink_order(p) == NORMAL)
		shrink(p) = mu_mult(shrink(g));
	else shrink(p) = shrink(g);

	return p;
	}

void tex::math_kern(ptr p, scal m)
	{
	scal	f;
	int	n;

	if (subtype(p) == MU_GLUE) {
		n = x_over_n(m, 0200000);
		f = tex::remainder;
		kern_width(p) = mu_mult(kern_width(p));
		subtype(p) = NORMAL;
		}
	}

void tex::flush_math ()
	{
	flush_node_list(link(head));
	flush_node_list(incompleat_noad);
	link(head) = null;
	tail = head;
	incompleat_noad = null;
	}

void tex::_math_init ()
	{
	}

void tex::_math_init_once ()
	{
	}
