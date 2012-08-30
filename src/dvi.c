
/*
 * $Id: dvi.c,v 1.11 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include <cmath>
#include <iostream>
#include "tex.h"

file	tex::dvi_file;
str	   tex::dvi_name;

#define HALF_BUF	2048
#define DVI_BUF_SIZE	4096

byte	*tex::dvi_buf;
int	tex::dvi_ptr;
int	tex::dvi_limit;
int	tex::dvi_offset;
int	tex::dvi_gone;

int	tex::cur_s;
scal	tex::cur_h;
scal	tex::cur_v;
fnt	   tex::dvi_f;
scal	tex::dvi_h;
scal	tex::dvi_v;
scal	tex::max_h;
scal	tex::max_v;
scal	tex::rule_dp;
scal	tex::rule_ht;
scal	tex::rule_wd;
int	tex::max_push;
int	tex::total_pages;
int	tex::dead_cycles;
bool	tex::doing_leaders;
int	tex::last_bop;

ptr	tex::down_ptr;
ptr	tex::right_ptr;

void tex::ship_out(ptr p)
	{
	int	page_loc;
	int	j, k;

	if (tracing_output > 0) {
		print_ln();
		print("Completed box being shipped out");
	}
	if (term_offset > MAX_PRINT_LINE - 9)
		print_ln();
	else if (term_offset > 0 || file_offset > 0)
		print(" ");
	print("["); 
	for (j = 9; j > 0 && count(j) == 0; decr(j))
		;
	for (k = 0; k <= j; incr(k)) {
		print_int(count(k));
		if (k < j)
			print(".");
	}
	if (tracing_output > 0) {
		print("]");
		begin_diagnostic();
		show_box(p);
		end_diagnostic(TRUE);
	}
	if (box_height(p) > MAX_DIMEN || box_depth(p) > MAX_DIMEN ||
		box_height(p) + box_depth(p) + v_offset > MAX_DIMEN ||
		box_width(p) + h_offset > MAX_DIMEN) {
		print_err("Huge page cannot be shipped out");
		help_huge_page();
		error();
		if (tracing_output <= 0) {
			begin_diagnostic();
			print_nl("The following box has been deleted:");
			show_box(p);
			end_diagnostic(TRUE);
		}
		goto done;
	}
	if (box_height(p) + box_depth(p) + v_offset > max_v)
		max_v = box_height(p) + box_depth(p) + v_offset;
	if (box_width(p) + h_offset > max_h)
		max_h = box_width(p) + h_offset;
	dvi_h = dvi_v = 0;
	cur_h = h_offset;
	dvi_f = null_font;
	if (dvi_name == null_str) {
		if (job_name == null_str)
			job_name = str_texput;
		pack_job_name(str_dvi);
		dvi_file=stdout;
		dvi_name = make_name_str();
	}
	if (total_pages == 0)
		start_dvi();
	page_loc = dvi_offset + dvi_ptr;
	dvi_out(BOP);
	for (k = 0; k <= 9; incr(k))
		dvi_four(count(k));
	dvi_four(last_bop);
	last_bop = page_loc;
	cur_v = box_height(p) + v_offset;
	if (type(p) == VLIST_NODE) 
		vlist_out(p);
	else hlist_out(p);
	dvi_out(EOP);
	incr(total_pages);
	cur_s = -1;
	
	done:
	if (tracing_output <= 0)
		print("]"); 
	dead_cycles = 0;
#ifdef STAT
	if (tracing_stats > 1) {
		print_nl("Memory usage before: ");
		print_int(var_used);
		print("&");
		print_int(dyn_used);
		print(";");
	}
#endif
	flush_node_list(p);
#ifdef STAT
	if (tracing_stats > 1) {
		print(" after: ");
		print_int(var_used);
		print("&");
		print_int(dyn_used);
		print_ln();
	}
#endif
}

void tex::hlist_out(ptr this_box)
	{
	scal	left_edge, base_line;
	int	save_loc;
	ptr	p;

	p = list_ptr(this_box);
	incr(cur_s);
	if (cur_s > 0)
		dvi_out(PUSH);
	if (cur_s > max_push)
		max_push = cur_s;
	save_loc = dvi_offset + dvi_ptr;
	left_edge = cur_h;
	base_line = cur_v;
	while (p != null) {
	reswitch:
		if (is_char_node(p)) {
			synch_h();
			synch_v();
			do {
				int f = font(p);
				int c = character(p);
				if (f != dvi_f) {
					fnt_out(f);
					dvi_f = f;
				}
				if (c >= 128)
					dvi_out(SET1);
				dvi_out(c);
				cur_h += char_width(f, char_info(f, c));
				p = link(p);
			} while (is_char_node(p));
			dvi_h = cur_h;
		} 
		else {
			switch (type(p)) {
				case HLIST_NODE:
				case VLIST_NODE:
					if (list_ptr(p) == null) {
						cur_h += box_width(p);
						} 
					else {
						scal save_h = dvi_h;
						scal save_v = dvi_v;
						scal edge = cur_h;
						cur_v = base_line + shift_amount(p);
						if (type(p) == VLIST_NODE)
							vlist_out(p);
						else hlist_out(p);
						dvi_h = save_h;
						dvi_v = save_v;
						cur_h = edge + box_width(p);
						cur_v = base_line;
						}
					break;
					
				case RULE_NODE:
					rule_wd = rule_width(p);
					rule_ht = rule_height(p);
					rule_dp = rule_depth(p);
					hrule_out((char *)this_box, base_line);
					break;
					
				case WHATSIT_NODE:
					out_whatsit(p);
					break;
					
				case GLUE_NODE:
					hskip_out(p, this_box, left_edge, base_line);
					break;
					
				case KERN_NODE:
					cur_h += kern_width(p);
					break;
					
				case MATH_NODE:
					cur_h += math_width(p);
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
	prune_movements(save_loc);
	if (cur_s > 0)
		dvi_pop(save_loc);
	decr(cur_s);
	}

void tex::hskip_out(ptr p, ptr this_box, scal left_edge, scal base_line)
	{
	ptr	g;
	int	g_sign, g_order;
	ptr	leader_box;
	scal	leader_wd;
	scal	edge, lx, lq, lr;
	bool	outer_doing_leaders;

	g = glue_ptr(p);
	rule_wd = glue_width(g);
	g_order = glue_order(this_box);
	g_sign = glue_sign(this_box);
	if (g_sign != NORMAL) {
		if (g_sign == STRETCHING) {
			if (stretch_order(g) == g_order)
				rule_wd += round(glue_set(this_box) *
					stretch(g));
		} else {
			if (shrink_order(g) == g_order)
				rule_wd -= round(glue_set(this_box) *
					shrink(g));
		}
	}
	if (subtype(p) >= A_LEADERS) {
		leader_box = leader_ptr(p);
		if (type(leader_box) == RULE_NODE) {
			rule_ht = rule_height(leader_box);
			rule_dp = rule_depth(leader_box);
			hrule_out((char *)this_box, base_line);
			return;
		}
		leader_wd = box_width(leader_box);
		if (leader_wd > 0 && rule_wd > 0) {
			scal save_h, save_v;
			rule_wd += 10;
			edge = cur_h + rule_wd;
			lx = 0;
			if (subtype(p) == A_LEADERS) {
				save_h = cur_h;
				cur_h = left_edge;
				cur_h += leader_wd *
					((save_h - left_edge) / leader_wd);
				if (cur_h < save_h)
					cur_h += leader_wd;
			} else {
				lq = rule_wd / leader_wd;
				lr = rule_wd % leader_wd;
				if (subtype(p) == C_LEADERS) {
					cur_h += lr / 2;
				} else {
					lx = (2 * lr + lq + 1) / (2 * lq + 2);
					cur_h += (lr - (lq - 1) * lx) / 2;
				}
			}
			while (cur_h + leader_wd <= edge) {
				cur_v = base_line + shift_amount(leader_box);
				synch_v();
				save_v = dvi_v;
				synch_h();
				save_h = dvi_h;
				outer_doing_leaders = doing_leaders;
				doing_leaders = TRUE;
				if (type(leader_box) == VLIST_NODE)
					vlist_out(leader_box);
				else hlist_out(leader_box);
				doing_leaders = outer_doing_leaders;
				dvi_v = save_v;
				dvi_h = save_h;
				cur_v = save_v;
				cur_h = save_h + lx + leader_wd;
			}
			cur_h = edge - 10;
			return;
		}
	}
	cur_h += rule_wd;
}

void tex::hrule_out(str this_box, int base_line)
	{
	if (is_running(rule_ht))
		rule_ht = box_height(this_box);
	if (is_running(rule_dp))
		rule_dp = box_depth(this_box);
	rule_ht = rule_ht + rule_dp;
	if (rule_ht > 0 && rule_wd > 0) {
		synch_h();
		cur_v = base_line + rule_dp;
		synch_v();
		dvi_out(SET_RULE);
		dvi_four(rule_ht);
		dvi_four(rule_wd);
		cur_v = base_line;
		dvi_h += rule_wd;
		}
	cur_h += rule_wd;
	}

void tex::vlist_out(ptr this_box)
	{
	scal	left_edge, top_edge;
	int	save_loc;
	ptr	p;

	p = list_ptr(this_box);
	incr(cur_s);
	if (cur_s > 0)
		dvi_out(PUSH);
	if (cur_s > max_push)
		max_push = cur_s;
	save_loc = dvi_offset + dvi_ptr;
	left_edge = cur_h;
	cur_v -= box_height(this_box);
	top_edge = cur_v;
	while (p != null) {
		if (is_char_node(p)) {
			confusion("vlistout");
		} else {
			switch (type(p))
			{
			case HLIST_NODE:
			case VLIST_NODE:
				if (list_ptr(p) == null) {
					cur_v += box_height(p) + box_depth(p);
				} else {
					scal save_h, save_v;
					cur_v += box_height(p);
					synch_v();
					save_h = dvi_h;
					save_v = dvi_v;
					cur_h = left_edge + shift_amount(p);
					if (type(p) == VLIST_NODE)
						vlist_out(p);
					else hlist_out(p);
					dvi_h = save_h;
					dvi_v = save_v;
					cur_v = save_v + box_depth(p);
					cur_h = left_edge;
				}
				break;
			
			case RULE_NODE:
				rule_ht = rule_height(p);
				rule_dp = rule_depth(p);
				rule_wd = rule_width(p);
				vrule_out(this_box);
				break;
			
			case WHATSIT_NODE:
				out_whatsit(p);
				break;
			
			case GLUE_NODE:
				vskip_out(p, this_box, left_edge, top_edge);
				break;

			case KERN_NODE:
				cur_v += kern_width(p);
				break;

			default:
				break;
			}
		}
		p = link(p);
	}
	prune_movements(save_loc);
	if (cur_s > 0)
		dvi_pop(save_loc);
	decr(cur_s);
	}

void tex::vskip_out(ptr p, ptr this_box, scal left_edge, scal top_edge)
	{
	ptr	g;
	int	g_sign, g_order;
	ptr	leader_box;
	scal	leader_ht;
	scal	edge, lq, lr, lx;
	bool	outer_doing_leaders;

	g = glue_ptr(p);
	rule_ht = glue_width(g);
	g_order = glue_order(this_box);
	g_sign = glue_sign(this_box);
	if (g_sign != NORMAL) {
		if (g_sign == STRETCHING) {
			if (stretch_order(g) == g_order)
				rule_ht += round(glue_set(this_box) *
					stretch(g));
		} else if (shrink_order(g) == g_order)
				rule_ht -= round(glue_set(this_box) *
					shrink(g));
	}
	if (subtype(p) >= A_LEADERS) {
		leader_box = leader_ptr(p);
		if (type(leader_box) == RULE_NODE) {
			rule_wd = box_width(leader_box);
			rule_dp = 0;
			vrule_out(this_box);
			return;
		}
		leader_ht = box_height(leader_box) + box_depth(leader_box);
		if (leader_ht > 0 && rule_ht > 0) {
			scal save_h, save_v;
			rule_ht += 10;
			edge = cur_v + rule_ht;
			lx = 0;
			if (subtype(p) == A_LEADERS) {
				save_v = cur_v;
				cur_v = top_edge;
				cur_v += leader_ht *
					((cur_v - top_edge) / leader_ht);
				if (cur_v < save_v)
					cur_v += leader_ht;
			} else {
				lq = rule_ht / leader_ht;
				lr = rule_ht % leader_ht;
				if (subtype(p) == C_LEADERS) {
					cur_v += lr / 2;
				} else {
					lx = (2 * lr + lq + 1) / (2 * lq + 2);
					cur_v += (lr - (lq - 1) * lx) / 2;
				}
			}
			while (cur_v + leader_ht <= edge) {
				cur_h = left_edge + shift_amount(leader_box);
				synch_h();
				save_h = dvi_h;
				cur_v += box_height(leader_box);
				synch_v();
				save_v = dvi_v;
				outer_doing_leaders = doing_leaders;
				doing_leaders = TRUE;
				if (type(leader_box) == VLIST_NODE) 
					vlist_out(leader_box);
				else hlist_out(leader_box);
				doing_leaders = outer_doing_leaders;
				cur_v = dvi_v = save_v;
				cur_h = dvi_h = save_h;
				cur_v += lx + leader_ht -
					box_height(leader_box);
			}
			cur_v = edge - 10;
			return;
		}
	}
	cur_v += rule_ht;
}

void tex::vrule_out(ptr this_box)
	{
	if (is_running(rule_wd))
		rule_wd = box_width(this_box);
	rule_ht += rule_dp;
	cur_v += rule_ht;
	if (rule_ht > 0 && rule_wd > 0) {
		synch_h();
		synch_v();
		dvi_out(PUT_RULE);
		dvi_four(rule_ht);
		dvi_four(rule_wd);
		}
	}

void tex::dvi_swap ()
	{
	if (dvi_limit == DVI_BUF_SIZE) {
		write_dvi(0, HALF_BUF);
		dvi_limit = HALF_BUF;
		dvi_offset += DVI_BUF_SIZE;
		dvi_ptr = 0;
	} else {
		write_dvi(HALF_BUF, DVI_BUF_SIZE);
		dvi_limit = DVI_BUF_SIZE;
	}
	dvi_gone += HALF_BUF;
}

void tex::dvi_four(int x)
	{
	if (x >= 0) {
		dvi_out(x / 0100000000);
	} else {
		x += 010000000000;
		x += 010000000000;
		dvi_out(x / 0100000000 + 128);
	}
	x %= 01000000000;
	dvi_out(x / 0200000);
	x %= 0200000;
	dvi_out(x / 0400);
	dvi_out(x % 0400);
	}

void tex::dvi_pop(int l)
	{
	if (l == dvi_offset + dvi_ptr && dvi_ptr > 0)
		decr(dvi_ptr);
	else dvi_out(POP);
	}

void tex::fnt_out(fnt f)
	{
	if (!font_used(f)) {
		dvi_font_def(f);
		font_used(f) = TRUE;
	}
	if (f <= 64 + null_font) {
		dvi_out(f - null_font - 1 + FNT_NUM_0);
	} else {
		dvi_out(FNT1);
		dvi_out(f - null_font - 1);
	}
	}

void tex::dvi_font_def(fnt f)
	{
	str	s;

	dvi_out(FNT_DEF1);
	dvi_out(f - null_font - 1);
	dvi_out(font_check(f).b0);
	dvi_out(font_check(f).b1);
	dvi_out(font_check(f).b2);
	dvi_out(font_check(f).b3);
	dvi_four(font_size(f));
	dvi_four(font_dsize(f));
	dvi_out(str_length(font_area(f)));
	dvi_out(str_length(font_name(f)));
	for (s = font_area(f); *s != NUL; incr(s))
		dvi_out(*s);
	for (s = font_name(f); *s != NUL; incr(s))
		dvi_out(*s);
}

#define Y_HERE		1
#define Z_HERE		2
#define YZ_OK		3
#define Y_OK		4
#define Z_OK		5
#define D_FIXED		6
#define NONE_SEEN	0
#define Y_SEEN		6
#define Z_SEEN		12

void tex::movement(scal m, int o)
	{
	int	k;
	ptr	p;
	ptr	q;
	int	mstate;

	q = new_node(MOVEMENT_NODE_SIZE);
	move_amount(q) = m;
	location(q) = dvi_offset + dvi_ptr;
	if (o == DOWN1) {
		link(q) = down_ptr;
		down_ptr = q;
		} 
	else {
		link(q) = right_ptr;
		right_ptr = q;
		}
	mstate = NONE_SEEN;
	for (p = link(q); p != null; p = link(p)) {
		if (move_amount(p) == m) {
			switch (mstate + info(p)) {
				case NONE_SEEN + YZ_OK:
				case NONE_SEEN + Y_OK:
				case Z_SEEN + YZ_OK:
				case Z_SEEN + Y_OK:
					if (location(p) < dvi_gone) {
						goto not_found;
						} 
					else {
						k = location(p) - dvi_offset;
						if (k < 0)
							k += DVI_BUF_SIZE;
						dvi_buf[k] += Y1 - DOWN1;
						info(p) = Y_HERE;
						goto found;
						}
					
				case NONE_SEEN + Z_OK:
				case Y_SEEN + YZ_OK:
				case Y_SEEN + Z_OK:
					if (location(p) < dvi_gone) {
						goto not_found;
						} 
					else {
						k = location(p) - dvi_offset;
						if (k < 0)
							k += DVI_BUF_SIZE;
						dvi_buf[k] += Z1 - DOWN1;
						info(p) = Z_HERE;
						goto found;
						}
					
				case NONE_SEEN + Y_HERE:
				case NONE_SEEN + Z_HERE:
				case Y_SEEN + Z_HERE:
				case Z_SEEN + Y_HERE:
					goto found;
				}
			} 
		else {
			switch (mstate + info(p)) {
				case NONE_SEEN + Y_HERE:
					mstate = Y_SEEN;
					break;
					
				case NONE_SEEN + Z_HERE:
					mstate = Z_SEEN;
					break;
					
				case Y_SEEN + Z_HERE:
				case Z_SEEN + Y_HERE:
					goto not_found;
					
				default:
					break;
				}
			}
		}

	  not_found:
	info(q) = YZ_OK;
	if (abs(m) >= 040000000) {
		dvi_out(o + 3);
		dvi_four(m);
		return;
	}
	if (abs(m) >= 0100000) {
		dvi_out(o + 2);
		if (m < 0)
			m += 0100000000;
		dvi_out(m / 0200000);
		m %= 0200000;
		goto two;
	}
	if (abs(m) >= 0200) {
		dvi_out(o + 1);
		if (m < 0)
			m += 0200000;
		goto two;
	}
	dvi_out(o);
	if (m < 0)
		m += 0400;
	goto one;

two: dvi_out(m / 0400);
one: dvi_out(m % 0400);
	return;

found:
	info(q) = info(p);
	if (info(q) == Y_HERE) {
		dvi_out(o + Y0 - DOWN1);
		while (link(q) != p) {
			q = link(q);
			switch (info(q))
			{
			case YZ_OK:
				info(q) = Z_OK;
				break;
			
			case Y_OK:
				info(q) = D_FIXED;
				break;
			}
		}
	} else {
		dvi_out(o + Z0 - DOWN1);
		while (link(q) != p) {
			q = link(q);
			switch (info(q))
			{
			case YZ_OK:
				info(q) = Y_OK;
				break;
			
			case Z_OK:
				info(q) = D_FIXED;
				break;
			default:
				break;
			}
		}
	}
}

void tex::prune_movements(int l)
	{
	ptr	p;

	while (down_ptr != null) {
		if (location(down_ptr) < l)
			break;
		p = down_ptr;
		down_ptr = link(p);
		free_node(p, MOVEMENT_NODE_SIZE);
	}
	while (right_ptr != null) {
		if (location(right_ptr) < l)
			break;
		p = right_ptr;
		right_ptr = link(p);
		free_node(p, MOVEMENT_NODE_SIZE);
	}
	}

void tex::start_dvi ()
	{
	str	s;
	int	old_setting;
	
	dvi_out(PRE);
	dvi_out(ID_BYTE); 
	dvi_four(25400000);
	dvi_four(473628672);
	prepare_mag();
	dvi_four(mag); 
	old_setting = selector;
	selector = NEW_STRING;
	print(" Common TeX output "); 
	print_int(year);
	print(".");
	print_two(month);
	print(".");
	print_two(day);
	print(":");
	/* FIXME: add printing of time (removed to avoid clashes with stdlib */
//	print_two(time / 60);
//	print_two(time % 60);
	selector = old_setting;
	dvi_out(cur_length());
	for (s = cur_str; s < cur_str_ptr; incr(s))
		dvi_out(*s);
	flush_str();
}

void tex::fin_dvi()
	{
	int	f;
	
	while (cur_s > -1) {
		if (cur_s > 0) {
			dvi_out(POP);
			} 
		else {
			dvi_out(EOP);
			incr(total_pages);
			}
		decr(cur_s);
		}
	if (total_pages == 0) {
		if(display_warnings) 
			std::cerr << "No pages of output." << std::endl;
		return;
		}
	dvi_out(POST);
	dvi_four(last_bop);
	last_bop = dvi_offset + dvi_ptr - 5;
	dvi_four(25400000);
	dvi_four(473628672);
	prepare_mag();
	dvi_four(mag);
	dvi_four(max_v);
	dvi_four(max_h);
	dvi_out(max_push / 256);
	dvi_out(max_push % 256);
	dvi_out(total_pages / 256 % 256);
	dvi_out(total_pages % 256);
	for (f = null_font + font_ptr - font_info - 1; f > null_font; decr(f))
		if (font_used(f))
			dvi_font_def(f);
	dvi_out(POST_POST);
	dvi_four(last_bop);
	dvi_out(ID_BYTE);
	for (f = 4 + (DVI_BUF_SIZE - dvi_ptr) % 4; f > 0; decr(f))
		dvi_out(223);
	if (dvi_limit == HALF_BUF)
		write_dvi(HALF_BUF, DVI_BUF_SIZE);
	if (dvi_ptr > 0)
		write_dvi(0, dvi_ptr);
	if(display_progress) {
		std::cerr << "output " << total_pages << " page";
		if (total_pages != 1)
			std::cerr << "s";
		std::cerr << ", " << dvi_offset+dvi_ptr << " bytes." << std::endl;
		}
	}

void tex::_dvi_init ()
	{
	dvi_limit = DVI_BUF_SIZE;
	dvi_name = null_str;
	last_bop = -1;
	cur_s = -1;
	}

void tex::_dvi_init_once ()
	{
	dvi_buf = (byte *)malloc(DVI_BUF_SIZE);
	if (dvi_buf == (byte *) 0) {
		overflow("dvi buffer", DVI_BUF_SIZE);
		}
	}

/*
**	Help text
*/

void tex::help_huge_page()
	{
	help2("The page just created is more than 18 feet tall or",
	"more than 18 feet wide, so I suspect something went wrong.");
	}
