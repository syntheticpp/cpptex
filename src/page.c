
/*
 * $Id: page.c,v 1.5 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include "tex.h"

ptr	tex::contrib_head;
ptr	tex::page_ins_head;
ptr	tex::page_head;
ptr	tex::page_tail;
int	tex::page_contents;
scal	tex::page_so_far[8];
scal	tex::page_max_depth;
ptr	tex::last_glue;
scal	tex::last_kern;
int	tex::last_penalty;
bool	tex::output_active;
int	tex::insert_penalties;
int	tex::least_page_cost;
ptr	tex::best_page_break;
scal	tex::best_size;
scal	tex::best_height_plus_depth;

#define set_page_so_far_zero(P)	(page_so_far[P] = 0)
#define set_height_zero(H)	(active_height[H] = 0)

void tex::build_page ()
	{
	int	pi=0, b, c;
	ptr	p, q, r;
	
#define INF_SHRINK_PAGE "Infinite glue shrinkage found on current page"
	
	if (link(contrib_head) == null || output_active)
		return;
	do {
		p = link(contrib_head);
		if (last_glue != null)
			delete_glue_ref(last_glue);
		last_penalty = 0;
		last_kern = 0;
		if (type(p) == GLUE_NODE) {
			last_glue = glue_ptr(p);
			add_glue_ref(last_glue);
		} else {
			last_glue = null;
			if (type(p) == PENALTY_NODE) {
				last_penalty = penalty(p);
			} else if (type(p) == KERN_NODE) {
				last_kern = kern_width(p);
			}
		}
		switch (type(p))
		{
		case HLIST_NODE:
		case VLIST_NODE:
		case RULE_NODE:
			if (page_contents < BOX_THERE) {
				if (page_contents == EMPTY) {
					freeze_page_specs(BOX_THERE);
				} else {
					page_contents = BOX_THERE;
				}
				q = new_skip_param(TOP_SKIP_CODE);
				link(q) = p;
				link(contrib_head) = q;
				r = glue_ptr(q);
				if (glue_width(r) > box_height(p)) {
					glue_width(r) -= box_height(p);
				} else {
					glue_width(r) = 0;
				}
				continue;
			} else {
				page_total += page_depth + box_height(p);
				page_depth = box_depth(p);
				goto contribute;
			}
		
		case GLUE_NODE:
			if (page_contents < BOX_THERE) {
				goto done;
			} else if (precedes_break(page_tail)) {
				pi = 0;
			} else {
				goto update_heights;
			}
			break;
		
		case KERN_NODE:
			if (page_contents < BOX_THERE) {
				goto done;
			} else if (link(p) == null) {
				return;
			} else if (type(link(p)) == GLUE_NODE) {
				pi = 0;
			} else {
				goto update_heights;
			}
			break;
		
		case PENALTY_NODE:
			if (page_contents < BOX_THERE) {
				goto done;
			} else {
				pi = penalty(p);
			}
			break;

		case WHATSIT_NODE:
			goto contribute;
		
		case MARK_NODE:
			goto contribute;

		case INS_NODE:
			insert_page(p);
			goto contribute;
		
		default:
			confusion("page");
			break;
		}
		if (pi < INF_PENALTY) {
			b = page_badness();
			if (b < AWFUL_BAD) {
				if (pi <= EJECT_PENALTY) {
					c = pi;
				} else if (b < INF_BAD) {
					c = b + pi + insert_penalties;
				} else {
					c = DEPLORABLE;
				}
			} else {
				c = b;
			}
			if (insert_penalties >= 10000)
				c = AWFUL_BAD;
			if (tracing_pages > 0)
				show_page_stats(b, pi, c);
			if (c <= least_page_cost) {
				best_page_break = p;
				best_size = page_goal;
				least_page_cost = c;
				r = link(page_ins_head);
				while (r != page_ins_head) {
					best_ins_ptr(r) = last_ins_ptr(r);
					r = link(r);
				}
			}
			if (c == AWFUL_BAD || pi <= EJECT_PENALTY) {
				fire_up(p);
				if (output_active)
					return;
				continue;
			}
		}
		if (type(p) < GLUE_NODE || type(p) > KERN_NODE) {
			goto contribute;
		}
		
	update_heights:
		if (type(p) == KERN_NODE) {
			page_total += page_depth + kern_width(p);
		} else {
			q = glue_ptr(p);
			page_so_far[2 + stretch_order(q)] += stretch(q);
			page_shrink += shrink(q);
			if (shrink_order(q) != NORMAL && shrink(q) != 0) {
				print_err(INF_SHRINK_PAGE);
				help_inf_shrink_page();
				error();
				r = new_spec(q);
				shrink_order(r) = NORMAL;
				delete_glue_ref(q);
				q = glue_ptr(p) = r;
			}
			page_total += page_depth + glue_width(q);
		}
		page_depth = 0;

	contribute:
		if (page_depth > page_max_depth) {
			page_total = page_total + page_depth - page_max_depth;
			page_depth = page_max_depth;
		}
		page_tail = link(page_tail) = p;
		link(contrib_head) = link(p);
		link(p) = null;
		continue;

	done:
		link(contrib_head) = link(p);
		link(p) = null;
		flush_node_list(p);
	} while (link(contrib_head) != null);
	if (nest_ptr == nest) {
		tail = contrib_head;
	} else {
		contrib_tail = contrib_head;
	}
}

void tex::insert_page(ptr p)
	{
	int	n;
	ptr	q, r;
	scal	h, w;
	scal	delta;

	if (page_contents == EMPTY)
		freeze_page_specs(INSERTS_ONLY);
	n = subtype(p);
	r = page_ins_head;
	while (n >= subtype(link(r)))
		r = link(r);
	if (subtype(r) != n) {
		q = new_node(PAGE_INS_NODE_SIZE);
		link(q) = link(r);
		r = link(r) = q;
		subtype(r) = n;
		type(r) = INSERTING;
		ensure_vbox(n);
		page_ins_height(r) = (box(n) == null) ? 0 :
			box_height(box(n)) + box_depth(box(n));
		best_ins_ptr(r) = null;
		q = skip(n);
		if (count(n) == 1000)
			h = page_ins_height(r);
		else h = x_over_n(page_ins_height(r), 1000) * count(n);
		page_goal -= h + glue_width(q);
		page_so_far[2 + stretch_order(q)] += stretch(q);
		page_shrink += shrink(q);
		if (shrink_order(q) != NORMAL && shrink(q) != 0) {
			print_err("Infinite glue shrinkage inserted from ");
			print_esc("skip");
			print_int(n);
			help_inf_shrink_ins();
			error();
		}
	}
	if (type(r) == SPLIT_UP) {
		insert_penalties += float_cost(p);
		return;
	}
	last_ins_ptr(r) = p;
	delta = page_goal - page_total - page_depth + page_shrink;
	if (count(n) == 1000) {
		h = ins_height(p);
	} else {
		h = x_over_n(ins_height(p), 1000) * count(n);
	}
	if ((h <= 0 || h <= delta)
	&& ins_height(p) + page_ins_height(r) <= dimen(n)) {
		page_goal -= h;
		page_ins_height(r) += ins_height(p);
	} else {
		if (count(n) <= 0) {
			w = MAX_DIMEN;
		} else {
			w = page_goal - page_total - page_depth;
			if (count(n) != 1000)  {
				w = x_over_n(w, count(n)) * 1000;
			}
		}
		if (w > dimen(n) - page_ins_height(r)) {
			w = dimen(n) - page_ins_height(r);
		}
		q = vert_break(ins_ptr(p), w, ins_depth(p));
		page_ins_height(r) += best_height_plus_depth;
		if (tracing_pages > 0)
			show_split(n, w, q);
		if (count(n) != 1000) {
			best_height_plus_depth =
				x_over_n(best_height_plus_depth, 1000) *
					count(n);
		}
		page_goal -= best_height_plus_depth;
		type(r) = SPLIT_UP;
		broken_ptr(r) = q;
		broken_ins(r) = p;
		if (q == null) {
			insert_penalties += EJECT_PENALTY;
		} else if (type(q) == PENALTY_NODE) {
			insert_penalties += penalty(q);
		}
	}
}

ptr tex::vert_break(ptr p, scal h, scal d)
	{
	int	b;
	ptr	q;
	ptr	r;
	int	t;
	int	pi=0;
	ptr	prev_p;
	scal	prev_dp;
	ptr	best_place=0;
	int	least_cost;

#define	INF_SHRINK_BOX "Infinite glue shrinkage found in box being split"

	prev_p = p;
	least_cost = AWFUL_BAD;
	do_all_six(set_height_zero);
	prev_dp = 0;
	loop {
		if (p == null) {
			pi = EJECT_PENALTY;
		} else {
			switch (type(p))
			{
			case HLIST_NODE:
			case VLIST_NODE:
			case RULE_NODE:
				cur_height += prev_dp + box_height(p);
				prev_dp = box_depth(p);
				goto not_found;
			
			case WHATSIT_NODE:
				goto not_found;
			
			case GLUE_NODE:
				if (precedes_break(prev_p)) {
					pi = 0;
				} else {
					goto update_heights;
				}
				break;
			
			case KERN_NODE:
				if (link(p) == null) {
					t = PENALTY_NODE;
				} else {
					t = type(link(p));
				}
				if (t == GLUE_NODE) {
					pi = 0;
				} else {
					goto update_heights;
				}
				break;
			
			case PENALTY_NODE:
				pi = penalty(p);
				break;
			
			case MARK_NODE:
			case INS_NODE:
				goto not_found;
			
			default:
				confusion("vertbreak");
			}
		}
		if (pi < INF_PENALTY) {
			b = vert_badness(h);
			if (b < AWFUL_BAD) {
				if (pi <= EJECT_PENALTY) {
					b = pi;
				} else if (b < INF_BAD) {
					b += pi;
				} else {
					b = DEPLORABLE;
				}
			}
			if (b <= least_cost) {
				best_place = p;
				least_cost = b;
				best_height_plus_depth = cur_height + prev_dp;
			}
			if (b == AWFUL_BAD || pi <= EJECT_PENALTY) {
				return best_place;
			}
		}
		if (type(p) < GLUE_NODE || type(p) > KERN_NODE) {
			goto not_found;
		}

	update_heights:
		if (type(p) == KERN_NODE) {
			cur_height += prev_dp + kern_width(p);
		} else {
			q = glue_ptr(p);
			active_height[2 + stretch_order(q)] += stretch(q);
			active_height[6] += shrink(q);
			if (shrink_order(q) != NORMAL && shrink(q) != 0) {
				print_err(INF_SHRINK_BOX);
				help_inf_shrink_box();
				error();
				r = new_spec(q);
				delete_glue_ref(q);
				shrink_order(r) = NORMAL;
				glue_ptr(p) = r;
			}
			cur_height += prev_dp + glue_width(q);
		}
		prev_dp = 0;

	not_found:
		if (prev_dp > d) {
			cur_height = cur_height + prev_dp - d;
			prev_dp = d;
		}
		prev_p = p;
		p = link(prev_p);
	}
}

ptr tex::vsplit(int n, scal h)
	{
	ptr	p;
	ptr	q;
	ptr	v;

	v = box(n);
	if (split_first_mark != null) {
		delete_token_ref(split_first_mark);
		split_first_mark = null;
		delete_token_ref(split_bot_mark);
		split_bot_mark = null;
	}
	if (v == null)
		return null;
	if (type(v) != VLIST_NODE) {
		print_err(null_str);
		print_esc("vsplit");
		print(" needs a ");
		print_esc("vbox");
		help_vsplit_vbox();
		error();
		return null;
	}
	q = vert_break(list_ptr(v), h, split_max_depth);
	p = list_ptr(v);
	if (p == q) {
		list_ptr(v) = null;
	} else {
		loop {
			if (type(p) == MARK_NODE) {
				if (split_first_mark == null) {
					split_first_mark = mark_ptr(p);
					split_bot_mark = split_first_mark;
					token_ref_count(split_first_mark) += 2;
				} else {
					delete_token_ref(split_bot_mark);
					split_bot_mark = mark_ptr(p);
					add_token_ref(split_bot_mark);
				}
			}
			if (link(p) == q) {
				link(p) = null;
				break;
			}
			p = link(p);
		}
	}
	q = prune_page_top(q);
	p = list_ptr(v);
	free_node(v, BOX_NODE_SIZE);
	if (q == null) {
		box(n) = null;
	} else {
		box(n) = vpack(q, 0, ADDITIONAL);
	}
	return (vpackage(p, h, EXACTLY, split_max_depth));
}

ptr tex::prune_page_top(ptr p)
	{
	mcell	m;
	ptr	prev_p;
	ptr	q, s, t;

	t = (ptr)&m;
	prev_p = t;
	link(t) = p;
	while (p != null) {
		switch (type(p))
		{
		case HLIST_NODE:
		case VLIST_NODE:
		case RULE_NODE:
			q = new_skip_param(SPLIT_TOP_SKIP_CODE);
			link(prev_p) = q;
			link(q) = p;
			s = glue_ptr(q);
			if (glue_width(s) > box_height(p)) {
				glue_width(s) -= box_height(p);
			} else {
				glue_width(s) = 0;
			}
			p = null;
			break;

		case WHATSIT_NODE:
		case MARK_NODE:
		case INS_NODE:
			prev_p = p;
			p = link(p);
			break;
		
		case GLUE_NODE:
		case KERN_NODE:
		case PENALTY_NODE:
			q = p;
			p = link(p);
			link(q) = null;
			link(prev_p) = p;
			flush_node_list(q);
			break;

		default:
			confusion("pruning");
			break;
		}
	}
	return (link(t));
}

int tex::page_badness ()
	{
	int	b;

	if (page_total < page_goal) {
		if (page_so_far[3] != 0
		|| page_so_far[4] != 0
		|| page_so_far[5] != 0) {
			b = 0;
		} else {
			b = badness(page_goal - page_total, page_so_far[2]);
		}
	} else if (page_total - page_goal > page_shrink) {
		b = AWFUL_BAD;
	} else {
		b = badness(page_total - page_goal, page_shrink);
	}
	return b;
	}

int tex::vert_badness(int height)
	{
	int	b;

	if (cur_height < height) {
		if (active_height[3] != 0
		|| active_height[4] != 0
		|| active_height[5] != 0) {
			b = 0;
		} else {
			b = badness(height - cur_height, active_height[2]);
		}
	} else if (cur_height - height > active_height[6]) {
		b = AWFUL_BAD;
	} else  {
		b = badness(cur_height - height, active_height[6]);
	}
	return b;
	}

void tex::freeze_page_specs(int s)
	{
	page_contents = s;
	page_goal = vsize;
	page_max_depth = max_depth;
	page_depth = 0;
	do_all_six(set_page_so_far_zero);
	least_page_cost = AWFUL_BAD;
	if (tracing_pages > 0) {
		begin_diagnostic();
		print_nl("%% goal height=");
		print_scaled(page_goal);
		print(", max depth=");
		print_scaled(page_max_depth);
		end_diagnostic(FALSE);
		}
	}

void tex::box_error(int n)
	{
	error();
	begin_diagnostic();
	print_nl("The following box has been deleted:");
	show_box(box(n));
	end_diagnostic(TRUE);
	flush_node_list(box(n));
	box(n) = null;
	}

void tex::ensure_vbox(int n)
	{
	ptr	p;

	p = box(n);
	if (p != null && type(p) == HLIST_NODE) {
		print_err("Insertions can only be added to a vbox");
		help_tut();
		box_error(n);
		}
	}

void tex::print_plus(int s, str o)
	{
	if (page_so_far[s] != 0) {
		print(" plus ");
		print_scaled(page_so_far[s]);
		print(o);
		}
	}

void tex::print_totals ()
	{
	print_scaled(page_total);
	print_plus(2, null_str);
	print_plus(3, "fil");
	print_plus(4, "fill");
	print_plus(5, "filll");
	if (page_shrink != 0) {
		print(" minus ");
		print_scaled(page_shrink);
		}
	}

void tex::show_split(int n, scal w, ptr q)
	{
	begin_diagnostic();
	print_nl("% split");
	print_int(n);
	print(" to ");
	print_scaled(w);
	print_char(',');
	print_scaled(best_height_plus_depth);
	print(" p=");
	if (q == null) {
		print_int(EJECT_PENALTY);
	} else if (type(q) == PENALTY_NODE) {
		print_int(penalty(q));
	} else {
		print("0");
	}
	end_diagnostic(FALSE);
	}

void tex::show_page_stats(int b, int pi, int c)
	{
	begin_diagnostic();
	print_nl("%");
	print(" t=");
	print_totals();
	print(" g=");
	print_scaled(page_goal);
	print(" b=");
	if (b == AWFUL_BAD) {
		print_char('*');
		} 
	else {
		print_int(b);
		}
	print(" p=");
	print_int(pi);
	print(" c=");
	if (c == AWFUL_BAD) {
		print("*");
	} else {
		print_int(c);
	}
	if (c <= least_page_cost) {
		print("#");
	}
	end_diagnostic(FALSE);
}

#define set_output_penalty(P) \
	reg_gdefine(int_reg[OUTPUT_PENALTY_CODE], INT_REG, P)

	
void tex::fire_up(ptr c)
	{
	int	n;
	bool	wait;
	ptr	prev_p;
	scal	save_vfuzz;
	int	save_vbadness;
	ptr	save_split_top_skip;
	ptr	p, q, r;

	if (type(best_page_break) == PENALTY_NODE) {
		set_output_penalty(penalty(best_page_break));
		penalty(best_page_break) = INF_PENALTY;
	} else {
		set_output_penalty(INF_PENALTY);
	}
	if (bot_mark != null) {
		if (top_mark != null)
			delete_token_ref(top_mark);
		top_mark = bot_mark;
		add_token_ref(top_mark);
		delete_token_ref(first_mark);
		first_mark = null;
	}
	if (c == best_page_break) {
		best_page_break = null;
	}
	if (box(255) != null) {
		print_err(null_str);
		print_esc("box");
		print("255 is not void");
		help_box_255();
		box_error(255);
	}
	insert_penalties = 0;
	save_split_top_skip = split_top_skip;
	if (holding_inserts <= 0) {
		r = link(page_ins_head);
		while (r != page_ins_head) {
			if (best_ins_ptr(r) != null) {
				n = subtype(r);
				ensure_vbox(n);
				if (box(n) == null)
					box(n) = new_null_box();
				p = node_list(box(n));
				while (link(p) != null) {
					p = link(p);
				}
				last_ins_ptr(r) = p;
			}
			r = link(r);
		}
	}
	q = hold_head;
	link(q) = null;
	prev_p = page_head;
	p = link(prev_p);
	while (p != best_page_break) {
		if (type(p) == INS_NODE) {
			if (holding_inserts <= 0) {
				wait = insert_box(p);
				link(prev_p) = link(p);
				link(p) = null;
				if (wait) {
					q = link(q) = p;
					incr(insert_penalties);
				} else {
					delete_glue_ref(split_top_ptr(p));
					free_node(p, INS_NODE_SIZE);
				}
				p = prev_p;
			}
		} else if (type(p) == MARK_NODE) {
			if (first_mark == null) {
				first_mark = mark_ptr(p);
				add_token_ref(first_mark);
			}
			if (bot_mark != null)
				delete_token_ref(bot_mark);
			bot_mark = mark_ptr(p);
			add_token_ref(bot_mark);
		}
		prev_p = p;
		p = link(prev_p);
	}
	split_top_skip = save_split_top_skip;
	if (p != null) {
		if (link(contrib_head) == null) {
			if (nest_ptr == nest) {
				tail = page_tail;
			} else {
				contrib_tail = page_tail;
			}
		}
		link(page_tail) = link(contrib_head);
		link(contrib_head) = p;
		link(prev_p) = null;
	}
	save_vbadness = vbadness;
	save_vfuzz = vfuzz;
	vbadness = INF_BAD;
	vfuzz = MAX_DIMEN;
	box(255) = vpackage(link(page_head),
		best_size, EXACTLY, page_max_depth);
	vbadness = save_vbadness;
	vfuzz = save_vfuzz;
	if (last_glue != null)
		delete_glue_ref(last_glue);
	start_new_page();
	if (q != hold_head) {
		link(page_head) = link(hold_head);
		page_tail = q;
	}
	r = link(page_ins_head);
	while (r != page_ins_head) {
		q = link(r);
		free_node(r, PAGE_INS_NODE_SIZE);
		r = q;
	}
	link(page_ins_head) = page_ins_head;
	if (top_mark != null && first_mark == null) {
		first_mark = top_mark;
		add_token_ref(top_mark);
	}
	if (output_routine != null) {
		if (dead_cycles >= max_dead_cycles) {
			print_err("Output loop---");
			print_int(dead_cycles);
			print(" consecutive dead cycles");
			help_dead_cycles();
			error();
		} else {
			output_active = TRUE;
			incr(dead_cycles);
			push_nest();
			mode = -VMODE;
			prev_depth = IGNORE_DEPTH;
			mode_line = -line;
			begin_token_list(output_routine, OUTPUT_TEXT);
			new_save_level(OUTPUT_GROUP);
			normal_paragraph();
			scan_left_brace();
			return;
		}
	}
	if (link(page_head) != null) {
		if (link(contrib_head) == null) {
			if (nest_ptr == nest) {
				tail = page_tail;
			} else {
				contrib_tail = page_tail;
			}
		} else {
			link(page_tail) = link(contrib_head);
		}
		link(contrib_head) = link(page_head);
		link(page_head) = null;
		page_tail = page_head;
	}
	ship_out(box(255));
	box(255) = null;
}

bool tex::insert_box(ptr p)
	{
	bool	wait;
	ptr	q, r, t;
	int	n;

	q = link(page_ins_head);
	while (subtype(q) != subtype(p))
		q = link(q);
	if (best_ins_ptr(q) == null)
		return TRUE;
	wait = FALSE;
	r = last_ins_ptr(q);
	link(r) = ins_ptr(p);
	if (best_ins_ptr(q) == p) {
		if (type(q) == SPLIT_UP
		&& broken_ins(q) == p
		&& broken_ptr(q) != null) {
			while (link(r) != broken_ptr(q))
				r = link(r);
			link(r) = null;
			split_top_skip = split_top_ptr(p);
			ins_ptr(p) = prune_page_top(broken_ptr(q));
			if (ins_ptr(p) != null) {
				t = vpack(ins_ptr(p), 0, ADDITIONAL);
				ins_height(p) = box_height(t) + box_depth(t);
				free_node(t, BOX_NODE_SIZE);
				wait = TRUE;
			}
		}
		best_ins_ptr(q) = null;
		n = subtype(q);
		t = list_ptr(box(n));
		free_node(box(n), BOX_NODE_SIZE);
		box(n) = vpack(t, 0, ADDITIONAL);
	} else {
		while (link(r) != null)
			r = link(r);
		last_ins_ptr(q) = r;
	}
	return wait;
	}

void tex::show_cur_page()
	{
	ptr	p, q;
	int	t;

	if (page_head == page_tail)
		return;
	print_nl("### current page:");
	if (output_active)
		print(" (held over for next output)");
	show_box(link(page_head));
	if (page_contents > EMPTY) {
		print_nl("total height ");
		print_totals();
		print_nl(" goal height ");
		print_scaled(page_goal);
		p = link(page_ins_head);
		while (p != page_ins_head) {
			print_ln();
			print_esc("insert");
			t = subtype(p);
			print_int(t);
			print(" adds ");
			t = x_over_n(page_ins_height(p), 1000) * count(t);
			print_scaled(t);
			if (type(p) == SPLIT_UP) {
				q = page_head;
				t = 0;
				do {
					q = link(q);
					if (type(q) == INS_NODE
					&& subtype(q) == subtype(p))
						incr(t);
				} while (q != broken_ins(p));
				print(", #");
				print_int(t);
				print(" might split");
			}
			p = link(p);
		}
	}
}

void tex::_page_init ()
	{
	start_new_page();
	prev_depth = IGNORE_DEPTH;
	output_active = FALSE;
	insert_penalties = 0;
	}

void tex::_page_init_once ()
	{
	contrib_head = new_avail();
	page_tail = page_head = new_glue(zero_glue);
	page_ins_head = new_node(PAGE_INS_NODE_SIZE);
	link(page_ins_head) = page_ins_head;
	type(page_ins_head) = SPLIT_UP;
	subtype(page_ins_head) = 255;
	}

/*
**	Help text
*/

void tex::help_tut()
	{
	help3("Tut tut: You're trying to \\insert into a",
	"\\box register that now contains an \\hbox.",
	"Proceed, and I'll discard its present contents.");
	}

void tex::help_vsplit_vbox()
	{
	help2("The box you are trying to split is an \\hbox.",
	"I can't split such a box, so I'll leave it alone.");
	}

void tex::help_inf_shrink_ins()
	{
	help3("The correction glue for page breaking with insertions",
	"must have finite shrinkability. But you may proceed,",
	"since the offensive shrinkability has been made finite.");
	}

void tex::help_inf_shrink_box()
	{
	help4("The box you are \\vsplitting contains some infinitely",
	"shrinkable glue, e.g., `\\vss' or `\\vskip 0pt minus 1fil'.",
	"Such glue doesn't belong there; but you can safely proceed,",
	"since the offensive shrinkability has been made finite.");
	}

void tex::help_inf_shrink_page()
	{
	help4("The page about to be output contains some infinitely",
	"shrinkable glue, e.g., `\\vss' or `\\vskip 0pt minus 1fil'.",
	"Such glue doesn't belong there; but you can safely proceed,",
	"since the offensive shrinkability has been made finite.");
	}

void tex::help_box_255()
	{
	help2("You shouldn't use \\box255 except in \\output routines.",
	"Proceed, and I'll discard its present contents.");
	}

void tex::help_dead_cycles()
	{
	help3("I've concluded that your \\output is awry; it never does a",
	"\\shipout, so I'm shipping \\box255 out myself. Next time",
	"increase \\maxdeadcycles if you want me to be more patient!");
	}
