
/*
 * $Id: pack.c,v 1.8 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include <cstdlib>
#include <iostream>
#include "tex.h"

ptr	tex::adjust_head;
ptr	tex::adjust_tail;
scal	tex::total_shrink[4];
scal	tex::total_stretch[4];
int	tex::last_badness;
int	tex::pack_begin_line;

int tex::get_stretch_order() 
	{
	return (tex::total_stretch[FILLL] != 0) ? FILLL : 
		(tex::total_stretch[FILL] != 0) ? FILL : 
		(tex::total_stretch[FIL] != 0) ? FIL : 
		NORMAL;
	}

int tex::get_shrink_order()
	{
	return (tex::total_shrink[FILLL] != 0) ? FILLL :
		(tex::total_shrink[FILL] != 0) ? FILL : 
		(tex::total_shrink[FIL] != 0) ? FIL : 
		NORMAL;
	}

void tex::clr_dimens()
	{
	tex::total_stretch[FILLL] = 0; 
	tex::total_stretch[FILL] = 0; 
	tex::total_stretch[FIL] = 0; 
	tex::total_stretch[NORMAL] = 0; 
	tex::total_shrink[FILLL] = 0; 
	tex::total_shrink[FILL] = 0; 
	tex::total_shrink[FIL] = 0; 
	tex::total_shrink[NORMAL] = 0;
	}

ptr tex::hpack(ptr p, scal w, int m)
	{
	ptr	q, g, r;
	scal	s, h, d, x;
	int	hd, o;
	fnt	f;
	qcell	i;
	
	tex::last_badness = 0;
	r = new_node(BOX_NODE_SIZE);
	type(r) = HLIST_NODE;
	subtype(r) = MIN_QUARTERWORD;
	shift_amount(r) = 0;
	q = node_list(r);
	link(q) = p;
	x = h = d = 0;
	clr_dimens();

	tex_error hpack_err;
	
	while (p != null) {
	reswitch:
		while (is_char_node(p)) {
			f = font(p);
			i = char_info(f, character(p));
			hd = height_depth(i);
			x += char_width(f, i);
			s = char_height(f, hd);
			if (s > h)
				h = s;
			s = char_depth(f, hd);
			if (s > d)
				d = s;
			p = link(p);
			}
		if (p != null) {
			switch (type(p)) {
				case HLIST_NODE:
				case VLIST_NODE:
				case RULE_NODE:
				case UNSET_NODE:
					x += box_width(p);
					if (type(p) >= RULE_NODE)
						s = 0;
					else 
						s = shift_amount(p);
					if (box_height(p) - s > h)
						h = box_height(p) - s;
					if (box_depth(p) + s > d)
						d = box_depth(p) + s;
					break;
				case INS_NODE:
				case MARK_NODE:
				case ADJUST_NODE:
					if (tex::adjust_tail == null) {
						break;
						}
					while (link(q) != p)
						q = link(q);
					if (type(p) == ADJUST_NODE) {
						link(tex::adjust_tail) = adjust_ptr(p);
						while (link(tex::adjust_tail) != null)
							tex::adjust_tail =
								link(tex::adjust_tail);
						p = link(p);
						free_node(link(q), SMALL_NODE_SIZE);
						} 
					else {
						tex::adjust_tail = link(tex::adjust_tail) = p;
						p = link(p);
						}
					link(q) = p;
					p = q;
					break;
				case WHATSIT_NODE:
					break;
				case GLUE_NODE:
					g = glue_ptr(p);
					x += glue_width(g);
					o = stretch_order(g);
					tex::total_stretch[o] += stretch(g);
					o = shrink_order(g);
					tex::total_shrink[o] += shrink(g);
					if (subtype(p) >= A_LEADERS) {
						g = leader_ptr(p);
						if (box_height(g) > h)
							h = box_height(g);
						if (box_depth(g) > d)
							d = box_depth(g);
						}
					break;
				case KERN_NODE:
				case MATH_NODE:
					x += kern_width(p);
					break;
				case LIGATURE_NODE:
					p = make_char_from_lig(p);
					goto reswitch;
				default:
					break;
				}
			p = link(p);
			}
		}
	if (tex::adjust_tail != null)
		link(tex::adjust_tail) = null;
	box_height(r) = h;
	box_depth(r) = d;
	if (m == ADDITIONAL)
		w += x;
	box_width(r) = w;
	x = w - x;
	if (x == 0) {
		glue_sign(r) = NORMAL;
		glue_order(r) = NORMAL;
		glue_set(r) = 0.0;
		return r;
		} 
	else if (x > 0) {
		o = get_stretch_order(); 
		glue_order(r) = o;
		glue_sign(r) = STRETCHING;
		if (tex::total_stretch[o] != 0) {
			glue_set(r) = (float) x / (float) tex::total_stretch[o];
			} 
		else {
			glue_sign(r) = NORMAL;
			glue_set(r) = 0.0;
			}
		if (o == NORMAL && list_ptr(r) != null) {
			tex::last_badness = badness(x, tex::total_stretch[NORMAL]);
			if (tex::last_badness > hbadness) {
				if (tex::last_badness > 100)
					hpack_err.msg << "underfull";
				else
					hpack_err.msg << "loose";
				hpack_err.msg << " \\hbox (badness " << tex::last_badness << ")";
				goto common_end;
				}
			}
		return r;
		} 
	else {
		o = get_shrink_order();
		glue_order(r) = o;
		glue_sign(r) = SHRINKING;
		if (tex::total_shrink[o] != 0) {
			glue_set(r) = (float) -x / tex::total_shrink[o];
			} 
		else {
			glue_sign(r) = NORMAL;
			glue_set(r) = 0.0;
			}
		if (tex::total_shrink[o] < -x && o == NORMAL && list_ptr(r) != null) {
			tex::last_badness = 1000000;
			glue_set(r) = 1.0;
			if (-x - tex::total_shrink[NORMAL] > hfuzz || hbadness < 100) {
				if (overfull_rule > 0 && -x - tex::total_shrink[NORMAL] > hfuzz) {
					while (link(q) != null)
						q = link(q);
					link(q) = new_rule();
					rule_width(link(q)) = overfull_rule;
					}
				hpack_err.msg << "overfull \\hbox (" 
								  << scal2string(-x-tex::total_shrink[NORMAL])
								  << "pt too wide)";
				goto common_end;
				}
			} 
		else if (o == NORMAL && list_ptr(r) != null) {
			tex::last_badness = badness(-x, tex::total_shrink[NORMAL]);
			if (tex::last_badness > hbadness) {
				hpack_err.msg << "tight \\hbox (badness " << tex::last_badness << ")";
				goto common_end;
				}
			}
		return r;
		}
	
	common_end:
	if (output_active) {
		hpack_err.msg << "has occurred while \\output is active";
		} 
	else {
		if (tex::pack_begin_line != 0) {
			if (tex::pack_begin_line > 0)
				hpack_err.msg << " in paragraph";
			else
				hpack_err.msg << " in alignment";
			hpack_err.line_start=abs(tex::pack_begin_line);
			hpack_err.line_end=line;
			} 
		else {
			hpack_err.msg << " detected";
			hpack_err.line_start=line;
			}
		}
	std::cerr << hpack_err;
	font_in_short_display = null_font;
	short_display(list_ptr(r));
	begin_diagnostic();
	show_box(r);
	end_diagnostic(TRUE);
	return r;
	}

ptr tex::vpack(ptr p, scal h, int m)
	{
	return vpackage(p, h, m, MAX_DIMEN);
	}

ptr tex::vpackage(ptr p, scal h, int m, scal l)
	{
	scal	s, w, d, x;
	ptr	g, r;
	int	o;

	tex_error vpack_err;

	tex::last_badness = 0;
	r = new_node(BOX_NODE_SIZE);
	type(r) = VLIST_NODE;
	subtype(r) = MIN_QUARTERWORD;
	shift_amount(r) = 0;
	list_ptr(r) = p;
	d = x = w = 0;
	clr_dimens();
	while (p != null) {
		if (is_char_node(p))
			confusion("vpack");
		switch (type(p)) {
			case HLIST_NODE:
			case VLIST_NODE:
			case RULE_NODE:
			case UNSET_NODE:
				x += d + box_height(p);
				d = box_depth(p);
				if (type(p) >= RULE_NODE)
					s = 0;
				else s = shift_amount(p);
				if (box_width(p) + s > w)
					w = box_width(p) + s;
				break;
			case WHATSIT_NODE:
				break;
			case GLUE_NODE:
				x += d;
				d = 0;
				g = glue_ptr(p);
				x += glue_width(g);
				o = stretch_order(g);
				tex::total_stretch[o] += stretch(g);
				o = shrink_order(g);
				tex::total_shrink[o] += shrink(g);
				if (subtype(p) >= A_LEADERS) {
					g = leader_ptr(p);
					if (box_width(g) > w)
						w = box_width(g);
					}
				break;
			case KERN_NODE:
				x += d + kern_width(p);
				d = 0;
				break;
			default:
				break;
			}
		p = link(p);
		}
	box_width(r) = w;
	if (d > l) {
		x += d - l;
		box_depth(r) = l;
		} 
	else {
		box_depth(r) = d;
		}
	if (m == ADDITIONAL)
		h += x;
	box_height(r) = h;
	x = h - x;
	if (x == 0) {
		glue_sign(r) = NORMAL;
		glue_order(r) = NORMAL;
		glue_set(r) = 0.0;
		return r;
		} 
	else if (x > 0) {
		o = get_stretch_order();
		glue_order(r) = o;
		glue_sign(r) = STRETCHING;
		if (tex::total_stretch[o] != 0) {
			glue_set(r) = (float) x / tex::total_stretch[o];
			} 
		else {
			glue_sign(r) = NORMAL;
			glue_set(r) = 0.0;
			}
		if (o == NORMAL && list_ptr(r) != 0) {
			tex::last_badness = badness(x, tex::total_stretch[NORMAL]);
			if (tex::last_badness > vbadness) {
				if (tex::last_badness > 100)
					vpack_err.msg << "underfull";
				else
					vpack_err.msg << "loose";
				vpack_err.msg << " \\vbox (badness " << tex::last_badness << ")";
				goto common_end;
				}
			}
		return r;
		} 
	else {
		o = get_shrink_order();
		glue_order(r) = o;
		glue_sign(r) = SHRINKING;
		if (tex::total_shrink[o] != 0) {
			glue_set(r) = (float) -x / tex::total_shrink[o];
			} 
		else {
			glue_sign(r) = NORMAL;
			glue_set(r) = 0.0;
			}
		if (tex::total_shrink[o] < -x && o == NORMAL && list_ptr(r) != 0) {
			tex::last_badness = 1000000;
			glue_set(r) = 1.0;
			if (-x - tex::total_shrink[NORMAL] > vfuzz || vbadness < 100) {
				vpack_err.msg << "overfull \\vbox (" 
								  << scal2string(-x - tex::total_shrink[NORMAL])
								  << "pt too high";
				goto common_end;
				}
			} 
		else if (o == NORMAL && list_ptr(r) != null) {
			tex::last_badness = badness(-x, tex::total_shrink[NORMAL]);
			if (tex::last_badness > vbadness) {
				vpack_err.msg << "tight \\vbox (badness "
								  << tex::last_badness << ")"; 
				goto common_end;
				}
			}
		return r;
		}
	
	common_end:
	if (output_active) {
		vpack_err.msg << " has occurred while \\output is active";
		} 
	else {
		if (tex::pack_begin_line != 0) {
			vpack_err.msg << " in alignment";
			vpack_err.line_start=abs(tex::pack_begin_line);
			vpack_err.line_end=line;
			} 
		else {
			vpack_err.msg << " detected";
			vpack_err.line_start=line;
			}
		}
	std::cerr << vpack_err;
	begin_diagnostic();
	show_box(r);
	end_diagnostic(TRUE);
	return r;
	}

void tex::_pack_init ()
	{
	tex::pack_begin_line = 0;
	tex::last_badness = 0;
	tex::adjust_tail = null;
	}

void tex::_pack_init_once ()
	{
	tex::adjust_head = new_avail();
	}
