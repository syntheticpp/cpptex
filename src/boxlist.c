
/*
 * $Id: boxlist.c,v 1.9 2003/09/14 13:30:18 kp229 Exp $
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

ptr tex::cur_box;

void tex::handle_right_brace ()
	{
	scal	d;
	int	f;
	ptr	p;
	ptr	q;
	
	switch (cur_group)
	{
	case SIMPLE_GROUP:
		unsave();
		break;
	
	case BOTTOM_LEVEL:
		print_err("Too many }'s");
		help_close_group();
		error();
		break;
	
	case SEMI_SIMPLE_GROUP:
	case MATH_SHIFT_GROUP:
	case MATH_LEFT_GROUP:
		extra_right_brace();
		break;

	case HBOX_GROUP:
		package(0);
		break;
	
	case ADJUSTED_HBOX_GROUP:
		tex::adjust_tail = tex::adjust_head;
		package(0);
		break;
	
	case VBOX_GROUP:
		end_graf();
		package(0);
		break;
	
	case VTOP_GROUP:
		end_graf();
		package(VTOP_CODE);
		break;

	case INSERT_GROUP:
		end_graf();
		q = split_top_skip;
		add_glue_ref(q);
		d = split_max_depth;
		f = floating_penalty;
		unsave();
		decr(save_ptr);
		p = vpack(link(head), 0, ADDITIONAL);
		pop_nest();
		if (saved(0) < 255) {
			tail_append(new_node(INS_NODE_SIZE));
			type(tail) = INS_NODE;
			subtype(tail) = saved(0);
			ins_height(tail) = box_height(p) + box_depth(p);
			ins_ptr(tail) = list_ptr(p);
			split_top_ptr(tail) = q;
			ins_depth(tail) = d;
			float_cost(tail) = f;
		} else {
			tail_append(new_node(SMALL_NODE_SIZE));
			type(tail) = ADJUST_NODE;
			subtype(tail) = 0;
			adjust_ptr(tail) = list_ptr(p);
			delete_glue_ref(q);
		}
		free_node(p, BOX_NODE_SIZE);
		if (nest_ptr == nest) 
			build_page();
		break;
	
	case OUTPUT_GROUP:
		if (loc != null
		|| (token_type != OUTPUT_TEXT && token_type != BACKED_UP)) {
			print_err("Unbalanced output routine");
			help_output_balance();
			error();
			do get_token();
			while (loc != null);
		}
		end_token_list();
		end_graf();
		unsave();
		output_active = FALSE;
		insert_penalties = 0;
		if (box(255) != null) {
			print_err("Output routine didn't use all of ");
			print_esc("box255");
			help_output();
			box_error(255);
		}
		if (tail != head) {
			link(page_tail) = link(head);
			page_tail = tail;
		}
		if (link(page_head) != null) {
			if (link(contrib_head) == null)
				contrib_tail = page_tail;
			link(page_tail) = link(contrib_head);
			link(contrib_head) = link(page_head);
			link(page_head) = null;
			page_tail = page_head;
		}
		pop_nest();
		build_page();
		break;
	
	case DISC_GROUP:
		build_discretionary();
		break;
	
	case ALIGN_GROUP:
		back_input();
		cur_tok = sym2tok(FROZEN_CR);
		print_err("Missing ");
		print_esc("cr");
		print(" inserted");
		help_align_cr();
		ins_error(); 
		break;

	case NO_ALIGN_GROUP:
		end_graf();
		unsave();
		align_peek();
		break;
	
	case VCENTER_GROUP:
		end_graf();
		unsave();
		save_ptr -= 2;
		p = vpackage(link(head), saved(1), saved(0), MAX_DIMEN);
		pop_nest();
		tail_append(new_noad());
		type(tail) = VCENTER_NOAD;
		math_type(nucleus(tail)) = SUB_BOX;
		info(nucleus(tail)) = p;
		break;
	
	case MATH_CHOICE_GROUP:
		build_choices();
		break;

	case MATH_GROUP:
		unsave();
		decr(save_ptr);
		math_type(saved(0)) = SUB_MLIST;
		p = fin_mlist(null);
		math_link(saved(0)) = p;
		if (p != null) {
			if (link(p) == null) {
				if (type(p) == ORD_NOAD) {
					if (math_type(subscr(p)) == EMPTY
					&& math_type(supscr(p)) == EMPTY) {
						mcopy(saved(0), nucleus(p));
						free_node(p, NOAD_SIZE);
					}
				} else if (type(p) == ACCENT_NOAD
					&& saved(0) == nucleus(tail)
					&& type(tail) == ORD_NOAD) {
					q = head;
					while (link(q) != tail)
						q = link(q);
					link(q) = p;
					free_node(tail, NOAD_SIZE);
					tail = p;
				}
			}
		}
		break;
	
	default:
		confusion("rightbrace");
		break;
	}
}

void tex::extra_right_brace ()
	{
	print_err("Extra }, or forgotten ");
	switch (cur_group) {
		case SEMI_SIMPLE_GROUP: print_esc("endgroup"); break;
		case MATH_SHIFT_GROUP: print("$"); break;
		case MATH_LEFT_GROUP: print_esc("right"); break;
		}
	help_group_close();
	error();
	incr(align_state);
	}

void tex::append_glue () 
	{
	int	s;
	
	s = cur_chr;
	switch (s) {
		case FIL_CODE: cur_val = fil_glue; break;
		case FILL_CODE: cur_val = fill_glue; break;
		case SS_CODE: cur_val = ss_glue; break;
		case FIL_NEG_CODE: cur_val = fil_neg_glue; break;
		case SKIP_CODE: scan_glue(GLUE_VAL); break;
		case MSKIP_CODE: scan_glue(MU_VAL); break;
		default: break;
		}
	tail_append(new_glue(cur_val));
	if (s >= SKIP_CODE) {
		decr(glue_ref_count(cur_val));
		if (s > SKIP_CODE)
			subtype(tail) = MU_GLUE;
		}
	}

void tex::append_kern ()
	{
	int	s;
	
	s = cur_chr;
	scan_dimen(s == MU_GLUE, FALSE, FALSE);
	tail_append(new_kern(cur_val));
	subtype(tail) = s;
	}

void tex::scan_box(int box_context)
	{
	get_nbrx_token();
	if (cur_cmd == MAKE_BOX) {
		begin_box(box_context);
	} else if (box_context >= LEADER_FLAG
		&& (cur_cmd == HRULE || cur_cmd == VRULE)) {
		cur_box = scan_rule_spec();
		box_end(box_context);
	} else {
		print_err("A <box> was supposed to be here");
		help_box();
		back_error();
	}
}

void tex::begin_box(int box_context)
	{
	int	k, m, n;
	ptr	p=0, q;

	switch (cur_chr)
	{
	case BOX_CODE:
		scan_eight_bit_int();
		cur_box = box(cur_val);
		box(cur_val) = null;
		break;
	
	case COPY_CODE:
		scan_eight_bit_int();
		cur_box = copy_node_list(box(cur_val));
		break;
	
	case LAST_BOX_CODE:
		cur_box = null;
		if (abs(mode) == MMODE) {
			you_cant();
			help_lastbox_m();
			error();
		} else if (mode == VMODE && head == tail) {
			you_cant();
			help_lastbox_v();
			error();
		} else if (type(tail) == HLIST_NODE
			|| type(tail) == VLIST_NODE) {
			for (q = head; q != tail; q = link(p)) {
				p = q;
				if (type(q) == DISC_NODE) {
					m = 1;
					while (m <= replace_count(q)) {
						p = link(p);
						incr(m);
					}
					if (p == tail) {
						break;
					}
				}
			}
			cur_box = tail;
			shift_amount(cur_box) = 0;
			tail = p;
			link(p) = null;
		}
		break;

	case VSPLIT_CODE:
		scan_eight_bit_int();
		n = cur_val;
		if (!scan_keyword("to")) {
			print_err("Missing `to' inserted");
			help_vsplit();
			error();
		}
		scan_normal_dimen();
		cur_box = vsplit(n, cur_val);
		break;
	
	default:
		k = cur_chr - VTOP_CODE;
		saved(0) = box_context;
		if (k == HMODE) {
			if (box_context < BOX_FLAG && abs(mode) == VMODE) {
				scan_spec(ADJUSTED_HBOX_GROUP, TRUE);
			} else {
				scan_spec(HBOX_GROUP, TRUE);
			}
		} else {
			if (k == VMODE) {
				scan_spec(VBOX_GROUP, TRUE);
			} else {
				scan_spec(VTOP_GROUP, TRUE);
				k = VMODE;
			}
			normal_paragraph();
		}
		push_nest();
		mode = -k;
		if (k == VMODE) {
			prev_depth = IGNORE_DEPTH;
			if (every_vbox != null)
				begin_token_list(every_vbox, EVERY_VBOX_TEXT);
		} else {
			space_factor = 1000;
			if (every_hbox != null) 
				begin_token_list(every_hbox, EVERY_HBOX_TEXT);
		}
		return;
	}
	box_end(box_context);
}

void tex::box_end(int box_context)
	{
	ptr	p;

#define LEADERS "Leaders not followed by proper glue"

	if (box_context < BOX_FLAG) {
		if (cur_box != null) {
			shift_amount(cur_box) = box_context;
			if (abs(mode) == VMODE) {
				append_to_vlist(cur_box);
				if (tex::adjust_tail != null) {
					if (tex::adjust_head != tex::adjust_tail) {
						link(tail) = link(tex::adjust_head);
						tail = tex::adjust_tail;
					}
					tex::adjust_tail = null;
				}
				if (mode > 0) 
					build_page();
			} else {
				if (abs(mode) == HMODE) {
					space_factor = 1000;
				} else {
					p = new_noad();
					math_type(nucleus(p)) = SUB_BOX;
					info(nucleus(p)) = cur_box;
					cur_box = p;
				}
				tail_append(cur_box);
			}
		}
	} else if (box_context < SHIP_OUT_FLAG) {
		if (box_context < BOX_FLAG + 256) {
			reg_define(box_reg[box_context - BOX_FLAG],
				BOX_REG, cur_box);
		} else {
			reg_gdefine(box_reg[box_context - BOX_FLAG - 256],
				BOX_REG, cur_box);
		}
	} else if (cur_box != null) {
		if (box_context > SHIP_OUT_FLAG) {
			get_nbrx_token();
			if (cur_cmd == HSKIP && abs(mode) != VMODE
			|| cur_cmd == VSKIP && abs(mode) == VMODE
			|| cur_cmd == MSKIP && abs(mode) == MMODE) {
				append_glue();
				leader_ptr(tail) = cur_box;
				subtype(tail) = box_context -
					(LEADER_FLAG - A_LEADERS);
			} else {
				print_err(LEADERS);
				help_leaders();
				back_error();
				flush_node_list(cur_box);
			}
		} else {
			ship_out(cur_box);
		}
	}
}

void tex::package(int c)
	{
	scal	d;
	scal	h;
	ptr	p;

	d = box_max_depth;
	unsave();
	save_ptr -= 3;
	if (mode == -HMODE) {
		cur_box = hpack(link(head), saved(2), saved(1));
	} else {
		cur_box = vpackage(link(head), saved(2), saved(1), d);
		if (c == VTOP_CODE) {
			h = 0;
			p = list_ptr(cur_box);
			if (p != null && type(p) <= RULE_NODE)
				h = box_height(p);
			box_depth(cur_box) += box_height(cur_box) - h;
			box_height(cur_box) = h;
		}
	}
	pop_nest();
	box_end(saved(0));
}

void tex::normal_paragraph ()
	{
	if (looseness != 0)
		reg_define(int_reg[LOOSENESS_CODE], INT_REG, 0);
	if (hang_indent != 0)
		reg_define(dimen_reg[HANG_INDENT_CODE], DIMEN_REG, 0);
	if (hang_after != 1)
		reg_define(int_reg[HANG_AFTER_CODE], INT_REG, 1);
	if (par_shape_ptr != null)
		eq_define(par_shape_cs, SET_SHAPE, null);
	}

int tex::norm_min(int h)
	{
	return (h <= 0 ? 1 : h >= 63 ? 63 : h);
	}

void tex::new_graf(bool indented)
	{
	prev_graf = 0;
	if (mode == VMODE || head != tail)
		tail_append(new_param_glue(PAR_SKIP_CODE));
	lhmin = norm_min(left_hyphen_min);
	rhmin = norm_min(right_hyphen_min);
	push_nest();
	mode = HMODE;
	space_factor = 1000;
	clang = 0;	
	if (indented) {
		link(head) = tail = new_null_box();
		box_width(tail) = par_indent;
	}
	if (every_par != null)
		begin_token_list(every_par, EVERY_PAR_TEXT);
	if (nest_ptr == nest + 1) 
		build_page();
}

void tex::indent_in_hmode ()
	{
	ptr	p, q;
	
	if (cur_chr > 0) {
		p = new_null_box();
		box_width(p) = par_indent;
		if (abs(mode) == HMODE) {
			space_factor = 1000;
			} 
		else {
			q = new_noad();
			math_type(nucleus(q)) = SUB_BOX;
			math_link(nucleus(q)) = p;
			p = q;
			}
		tail_append(p);
		}
	}

void tex::end_graf ()
	{
	if (mode == HMODE) {
		if (head == tail)
			pop_nest();
		else line_break(widow_penalty);
		normal_paragraph();
		error_cnt = 0;
		}
	}

void tex::head_for_vmode ()
	{
	if (mode < 0) {
		if (cur_cmd != HRULE) {
			off_save();
			} 
		else {
			print_err("You can't use `");
			print_esc("hrule");
			print("' here except with leaders");
			help_head_for_vmode();
			error();
			}
		} 
	else {
		back_input();
		cur_tok = par_tok;
		back_input();
		token_type = INSERTED;
		}
	}

void tex::append_to_vlist(ptr b)
	{
	scal	d;
	ptr	p;

	append_to_vlist_called=true;
	if (prev_depth > IGNORE_DEPTH) {
		d = glue_width(baseline_skip) - prev_depth - box_height(b);
		if (d < line_skip_limit) {
			p = new_param_glue(LINE_SKIP_CODE);
			} 
		else {
			p = new_skip_param(BASELINE_SKIP_CODE);
			glue_width(glue_ptr(p)) = d;
			}
		tail_append(p);
		}
	tail_append(b);
	prev_depth = box_depth(b);
	}

void tex::begin_insert_or_adjust ()
	{
	if (cur_cmd == VADJUST) {
		cur_val = 255;
		} 
	else {
		scan_eight_bit_int();
		if (cur_val == 255) {
			print_err("You can't ");
			print_esc("insert");
			print_int(255);
			help_insert_255();
			error();
			cur_val = 0;
			}
		}
	saved(0) = cur_val;
	incr(save_ptr);
	new_save_level(INSERT_GROUP);
	scan_left_brace();
	normal_paragraph();
	push_nest();
	mode = -VMODE;
	prev_depth = IGNORE_DEPTH;
	}

void tex::make_mark ()
	{
	ptr	p;
	
	scan_toks(FALSE, TRUE);
	p = new_node(SMALL_NODE_SIZE);
	type(p) = MARK_NODE;
	subtype(p) = 0;
	mark_ptr(p) = def_ref;
	tail_append(p);
	}

void tex::append_penalty ()
	{
	scan_int();
	tail_append(new_penalty(cur_val));
	if (mode == VMODE) 
		build_page();
	}

void tex::delete_last ()
	{
	int	m;
	ptr	p=0, q;
	
	if (mode == VMODE && tail == head) {
		if (cur_chr != GLUE_NODE
			 || last_glue != null) {
			you_cant();
			if (cur_chr == KERN_NODE) {
				help_delete_last_kern();
			} else if (cur_chr == GLUE_NODE) {
				help_delete_last_skip();
			} else {
				help_delete_last_pen(); 
			}
			error();
		}
	} else if (!is_char_node(tail) && type(tail) == cur_chr) {
		for (q = head; q != tail; q = link(p)) {
			p = q;
			if (!is_char_node(q) && type(q) == DISC_NODE) {
				for (m = 1; m <= replace_count(q); incr(m))
					p = link(p);
				if (p == tail)
					return;
			}
		}
		link(p) = null;
		flush_node_list(tail);
		tail = p;
		}
	}

void tex::unpackage ()
	{	
	int	c;
	ptr	p;
	
	c = cur_chr;
	scan_eight_bit_int();
	p = box(cur_val);
	if (p == null)
		return;
	if (abs(mode) == MMODE
	|| abs(mode) == VMODE && type(p) != VLIST_NODE
	|| abs(mode) == HMODE && type(p) != HLIST_NODE) {
		print_err("Incompatible list can't be unboxed");
		help_pandora();
		error();
		return;
	}
	if (c == COPY_CODE) {
		link(tail) = copy_node_list(list_ptr(p));
	} else {
		link(tail) = list_ptr(p);
		box(cur_val) = null;
		free_node(p, BOX_NODE_SIZE);
	}
	while (link(tail) != null)
		tail = link(tail);
	}

void tex::append_italic_correction ()
	{
	ptr	p;
	fnt	f;

	if (tail != head) {
		if (is_char_node(tail))
			p = tail;
		else if (type(tail) == LIGATURE_NODE)
			p = lig_char(tail);
		else return;
		f = font(p);
		tail = link(tail) =
			new_kern(char_italic(f, char_info(f, character(p))));
		subtype(tail) = EXPLICIT;
		}
	}

void tex::append_discretionary ()
	{
	int	c;

	tail_append(new_disc());
	if (cur_chr == 1) {
		c = hyphen_char(cur_font);
		if (c >= 0 && c < 256)
			pre_break(tail) = new_character(cur_font, c);
		}
	else {
		incr(save_ptr);
		saved(-1) = 0;
		scan_left_brace();
		new_save_level(DISC_GROUP);
		push_nest();
		mode = -HMODE;
		space_factor = 1000;
		}
	}

void tex::build_discretionary ()
	{
	int	n;
	ptr	p, q;

	unsave();
	q = head;
	p = link(q);
	n = 0;
	while (p != null) {
		if (!is_char_node(p)
		&& type(p) > RULE_NODE
		&& type(p) != KERN_NODE
		&& type(p) != LIGATURE_NODE) {
			print_err("Improper discretionary list");
			help_discretionary();
			error();
			flush_discretionary(p);
			link(q) = null;
			break;
		}
		q = p;
		p = link(q);
		incr(n);
	}
	p = link(head);
	pop_nest();
	switch (saved(-1))
	{
	case 0:
		pre_break(tail) = p;
		break;

	case 1:
		post_break(tail) = p;
		break;

	case 2:
		if (n > 0 && abs(mode) == MMODE) {
			print_err("Illegal math ");
			print_esc("discretionary");
			help_math_disc();
			flush_node_list(p);
			n = 0;
			error();
		} else {
			link(tail) = p;
		}
		if (n <= MAX_QUARTERWORD) {
			replace_count(tail) = n;
		} else {
			print_err("Discretionary list is too long");
			help_disc();
			error();
		}
		if (n > 0)
			tail = q;
		decr(save_ptr);
		return;
	}
	incr(saved(-1));
	scan_left_brace();
	new_save_level(DISC_GROUP);
	push_nest();
	mode = -HMODE;
	space_factor = 1000;
}

void tex::flush_discretionary(ptr p)
	{
	begin_diagnostic();
	print_nl("The following discretionary sublist has been deleted:");
	show_box(p);
	end_diagnostic(TRUE);
	flush_node_list(p);
	}

void tex::make_accent ()
	{
	fnt	f;
	qcell	i;
	ptr	p, q, r;
	scal	a, h, w, x;
	float	s, t;
	scal	delta;

	scan_char_num();
	f = cur_font;
	p = new_character(f, cur_val);
	if (p != null) {
		x = x_height(f);
		s = (float) slant(f) / 65536.0;
		a = char_width(f, char_info(f, character(p)));
		do_assignments();
		q = null;
		f = cur_font;
		if (cur_cmd == LETTER
		|| cur_cmd == OTHER_CHAR
		|| cur_cmd ==  CHAR_GIVEN) {
			q = new_character(f, cur_chr);
		} else if (cur_cmd ==  CHAR_NUM) {
			scan_char_num();
			q = new_character(f, cur_val);
		} else {
			back_input();
		}
		if (q != null) {
			t = (float) slant(f) / 65536.0;
			i = char_info(f, character(q));
			w = char_width(f, i); 
			h = char_height(f, height_depth(i));
			if (h != x) {
				p = hpack(p, 0, ADDITIONAL);
				shift_amount(p) = x - h;
			}
			delta = round((float) (w - a) / 2.0 + h * t - x * s);
			r = new_kern(delta);
			subtype(r) = ACC_KERN;
			link(tail) = r;
			link(r) = p;
			tail = new_kern(-a - delta);
			subtype(tail) = ACC_KERN;
			link(p) = tail;
			p = q;
		}
		tail_append(p);
		space_factor = 1000;
		}
	}

void tex::align_error ()
	{
	if (abs(align_state) > 2) {
		print_err("Misplaced ");
		print_cmd_chr(cur_cmd, cur_chr);
		if (cur_tok == TAB_TOKEN + '&') 
			help_tab();
		else help_align_error();
		error();
	} else {
		back_input();
		if (align_state < 0) {
			print_err("Missing { inserted");
			incr(align_state);
			cur_tok = LEFT_BRACE_TOKEN + '{';
		} else {
			print_err("Missing } inserted");
			decr(align_state);
			cur_tok = RIGHT_BRACE_TOKEN + '}';
		}
		help_fix_alignment();
		ins_error();
		}
	}

void tex::no_align_error ()
	{
	print_err("Misplaced ");
	print_esc("noalign");
	help_noalign();
	error();
	}

void tex::omit_error ()
	{
	print_err("Misplaced ");
	print_esc("omit");
	help_omit();
	error();
	}

void tex::do_endv ()
	{
	if (cur_group == ALIGN_GROUP) {
		end_graf();
		if (fin_col())
			fin_row();
		} else {
			off_save();
			}
	}

void tex::cs_error ()
	{
	print_err("Extra ");
	print_esc("endcsname");
	help_csname();
	error();
	}

void tex::_boxlist_init ()
	{
	}

void tex::_boxlist_init_once ()
	{
	}

