
/*
 * $Id: hyph.c,v 1.5 2003/09/14 13:30:19 kp229 Exp $
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

ptr	tex::ha;
ptr	tex::hb;
int	tex::hc[66];
fnt	tex::hf;
int	tex::hn;
int	tex::hu[64];
int	tex::hyf[65];
int	tex::hyf_char;

int	tex::hyphen_passed;

int	tex::l_hyf;
int	tex::r_hyf;
int	tex::cur_lang;

ptr	tex::init_list;
bool	tex::init_lig;
bool	tex::init_lft;

#define HYPH_SIZE	307

int	 tex::hyph_count;
int	*hyph_len;
str	*tex::hyph_word;
ptr	*tex::hyph_list;

#define TRIE_SIZE	20000

bool	tex::trie_not_ready;

tex::trie_t	*tex::trie;
int	    tex::trie_max;
int	    tex::trie_ptr;
int	   *tex::trie_hash;
bool	   *tex::trie_taken;
int	   *tex::trie_min;
int	   *tex::trie_c;
int	   *tex::trie_o;
int	   *tex::trie_l;
int	   *tex::trie_r;

#define TRIE_OP_SIZE		600
#define MAX_TRIE_OPS_PER_LANG	300

int	 tex::trie_op_ptr;
int	*tex::trie_op_hash;	
int	*tex::trie_op_lang;
int	*tex::trie_op_used;
int	*tex::trie_op_val;
int	*tex::op_start;
int	*tex::hyf_distance;
int	*tex::hyf_next;
int	*tex::hyf_num;

void tex::init_hyph ()
	{
	if (trie_not_ready)
		init_trie();
	l_hyf = lhmin;
	r_hyf = rhmin;
	cur_lang = 0;
	}

void tex::try_hyph ()
	{
	int	c, j;
	ptr	prev_p, p, q;
	
	prev_p = cur_p;
	p = link(cur_p);
	if (p != null) {
		loop {
			if (is_char_node(p)) {
				c = character(p);
				hf = font(p);
			} else if (type(p) == LIGATURE_NODE) {
				if (lig_ptr(p) == null)
					goto contin;
				q = lig_ptr(p);
				c = character(q);
				hf = font(q);
			} else if (type(p) == KERN_NODE
				&& subtype(p) == NORMAL) {
				goto contin;
			} else if (type(p) == WHATSIT_NODE) {
				try_hyph_whatsit(p);
				goto contin;
			} else {
				goto done1;
			}
			if (lc_code(c) != 0) {
				if (lc_code(c) == c || uc_hyph > 0) {
					goto done2;
				} else {
					goto done1;
				}
			}
		contin:
			prev_p = p;
			p = link(p);
		}
	done2:
		hyf_char = hyphen_char(hf);
		if (hyf_char < 0 || hyf_char > 255)
			goto done1;
		ha = prev_p;
		if (l_hyf + r_hyf > 63)
			goto done1;
		hn = 0;
		loop {
			if (is_char_node(p)) {
				if (font(p) != hf)
					goto done3;
				c = character(p);
				if (lc_code(c) == 0 || hn == 63)
					goto done3;
				hb = p;
				incr(hn);
				hu[hn] = c;
				hc[hn] = lc_code(c);
			} else if (type(p) == LIGATURE_NODE) {
				if (font(lig_char(p)) != hf)
					goto done3;
				j = hn;
				q = lig_ptr(p);
				while (q != null) {
					c = character(q);
					if (lc_code(c) == 0 || j == 63)
						goto done3;
					incr(j);
					hu[j] = c;
					hc[j] = lc_code(c);
					q = link(q);
				}
				hb = p;
				hn = j;
			} else if (type(p) != KERN_NODE
				|| subtype(p) != NORMAL) {
				goto done3;
			}
			p = link(p);
		}
	done3:
		if (hn < l_hyf + r_hyf)
			goto done1;
		loop {
			if (!is_char_node(p)) {
				switch (type(p))
				{
				case LIGATURE_NODE:
					break;

				case KERN_NODE:
					if (subtype(p) != NORMAL)
						goto done4;
					break;

				case WHATSIT_NODE:
				case GLUE_NODE:
				case PENALTY_NODE:
				case INS_NODE:
				case ADJUST_NODE:
				case MARK_NODE:
					goto done4;
				
				default: 
					goto done1;
				}
			}
			p = link(p);
		}

	done4:
		hyphenate();
	}
done1:	return;
}

void tex::hyphenate ()
	{
	str	t, u;
	ptr	q, r, s;
	ptr	hyf_node;
	int	bchar;
	int	c_loc;
	int	r_count;
	ptr	major_tail;
	ptr	minor_tail;
	int	c, h, i, j, l, v, z;
	c=0; // FIXME: avoiding compiler warning here, but is it correct?

	for (j = 0; j <= hn; incr(j))
		hyf[j] = 0;
	h = hc[1];
	incr(hn);
	hc[hn] = cur_lang;
	for (j = 2; j <= hn; incr(j))
		h = (h + h + hc[j]) % HYPH_SIZE;
	loop {
		t = hyph_word[h];
		if (t == null_str)
			goto not_found;
		l = hyph_len[h];
		if (l < hn)
			goto not_found;
		if (l == hn) {
			j = 1;
			u = t;
			while (j <= hn) {
				if (*u < hc[j])
					goto not_found;
				if (*u > hc[j])
					goto done;
				incr(u);
				incr(j);
			}
			for (s = hyph_list[h]; s != null; s = link(s))
				hyf[info(s)] = 1;
			decr(hn);
			goto found;
		}

	done:
		if (h > 0) {
			decr(h);
		} else {
			h = HYPH_SIZE;
		}
	}

not_found:
	decr(hn);
	if (trie_char(cur_lang + 1) != cur_lang)
		return;
	hc[0] = 0;
	hc[hn + 1] = 0;
	hc[hn + 2] = 256;
	for (j = 0; j <= hn - r_hyf + 1; incr(j)) {
		z = trie_link(cur_lang + 1) + hc[j];
		l = j;
		while (hc[l] == trie_char(z)) {
			if ((v = trie_op(z))) {
				while (v) {
					v += op_start[cur_lang];
					i = l - hyf_distance[v];
					if (hyf_num[v] > hyf[i])
						hyf[i] = hyf_num[v];
					v = hyf_next[v];
				}
			}
			incr(l);
			z = trie_link(z) + hc[l];
		}
	}

found:
	for (j = 0; j < l_hyf; incr(j)) {
		hyf[j] = 0;
	}
	for (j = 0; j < r_hyf; incr(j)) {
		hyf[hn - j] = 0;
	}
	for (j = l_hyf; j <= hn - r_hyf; incr(j))
		if (odd(hyf[j]))
			goto found1;
	return;

found1:
	q = link(hb);
	link(hb) = null;
	r = link(ha);
	link(ha) = null;
	bchar = NON_CHAR;
	if (type(hb) == LIGATURE_NODE && odd(subtype(hb))) {
		bchar = font_bchar(hf);
	}
	if (is_char_node(ha)) {
		if (font(ha) != hf) {
			goto found2;
		} else {
			init_list = ha;
			init_lig = FALSE;
			hu[0] = character(ha);
		}
	} else if (type(ha) == LIGATURE_NODE) {
		if (font(lig_char(ha)) != hf) {
			goto found2;
		} else {
			init_list = lig_ptr(ha);
			init_lig = TRUE;
			init_lft = subtype(ha) > 1;
			hu[0] = character(lig_char(ha));
			if (init_list == null && init_lft) {
				hu[0] = 256;
				init_lig = FALSE;
			}
			free_node(ha, SMALL_NODE_SIZE);
		}
	} else {
		if (type(r) == LIGATURE_NODE && subtype(r) > 1) {
			goto found2;
		}
		j = 1;
		s = ha;
		init_list = null;
		goto common_end;
	}
	s = cur_p;
	while (link(s) != ha)
		s = link(s);
	j = 0;
	goto common_end;

found2:
	s = ha;
	j = 0;
	hu[0] = 256;
	init_lig = FALSE;
	init_list = null;

common_end:
	flush_node_list(r);

#define advance_major_tail()						\
{	major_tail = link(major_tail);					\
	incr(r_count);							\
}

#define put_pre_break()							\
{	minor_tail = null;						\
	pre_break(r) = null;						\
	hyf_node = new_character(hf, hyf_char);				\
	if (hyf_node != null) {						\
		incr(i);						\
		c = hu[i];						\
		hu[i] = hyf_char;					\
		free_avail(hyf_node);					\
	}								\
	while (l <= i) {						\
		l = reconstitute(l, i, font_bchar(hf), NON_CHAR) + 1;	\
		if (link(hold_head) == null) {				\
			continue;					\
		}							\
		if (minor_tail == null) {				\
			pre_break(r) = link(hold_head);			\
		} else {						\
			link(minor_tail) = link(hold_head);		\
		}							\
		minor_tail = link(hold_head);				\
		while (link(minor_tail) != null)			\
			minor_tail = link(minor_tail);			\
	}								\
	if (hyf_node != null) {						\
		hu[i] = c;						\
		l = i;							\
		decr(i);						\
	}								\
}

#define put_post_break()						\
{	minor_tail = null;						\
	post_break(r) = null;						\
	c_loc = 0;							\
	if (bchar_label(hf) != NON_ADDRESS) {				\
		decr(l);						\
		c = hu[l];						\
		c_loc = l;						\
		hu[l] = 256;						\
	}								\
	while (l < j) {							\
		do {							\
			l = reconstitute(l, hn, bchar, NON_CHAR) + 1;	\
			if (c_loc > 0) {				\
				hu[c_loc] = c;				\
				c_loc = 0;				\
			}						\
			if (link(hold_head) == null) {			\
				continue;				\
			}						\
			if (minor_tail == null) {			\
				post_break(r) = link(hold_head);	\
			} else {					\
				link(minor_tail) = link(hold_head);	\
			}						\
			minor_tail = link(hold_head);			\
			while (link(minor_tail) != null)		\
				minor_tail = link(minor_tail);		\
		} while (l < j);					\
		while (l > j) {						\
			j = reconstitute(j, hn, bchar, NON_CHAR) + 1;	\
			link(major_tail) = link(hold_head);		\
			while (link(major_tail) != null) {		\
				advance_major_tail();			\
			}						\
		}							\
	}								\
}

	do {
		l = j;
		j = reconstitute(j, hn, bchar, hyf_char) + 1;
		if (hyphen_passed == 0) {
			link(s) = link(hold_head);
			while (link(s) != null)
				s = link(s);
			if (odd(hyf[j - 1])) {
				l = j;
				hyphen_passed = j - 1;
				link(hold_head) = null;
			}
		}
		if (hyphen_passed > 0) {
			do {
				r = new_node(SMALL_NODE_SIZE);
				link(r) = link(hold_head);
				type(r) = DISC_NODE;
				major_tail = r;
				r_count = 0;
				while (link(major_tail) != null) {
					advance_major_tail();
				}
				i = hyphen_passed;
				hyf[i] = 0;
				put_pre_break();
				put_post_break();
				if (r_count > 127) {
					link(s) = link(r);
					link(r) = null;
					flush_node_list(r);
				} else {
					link(s) = r;
					replace_count(r) = r_count;
				}
				s = major_tail;
				hyphen_passed = j - 1;
				link(hold_head) = null;
			} while (odd(hyf[j - 1]));
		}
	} while (j <= hn);
	link(s) = q;
	flush_list(init_list);
}

#define append_charnode_to_t(C)					\
{	t = link(t) = new_avail(); 				\
	font(t) = hf; character(t) = (C); 			\
}

#define set_cur_r()						\
{	cur_r = (j < n) ? hu[j + 1] : bchar;			\
	cur_rh = (odd(hyf[j])) ? hchar : NON_CHAR;		\
}

#define wrap_lig(RT_HIT)					\
{	if (ligature_present) {					\
		p = new_ligature(hf, cur_l, link(cur_q));	\
		if (lft_hit) {					\
			subtype(p) = 2;				\
			lft_hit = FALSE;			\
		}						\
		if (RT_HIT) {					\
			if (lig_stack == null) {		\
				incr(subtype(p));		\
				rt_hit = FALSE;			\
			}					\
		}						\
		t = link(cur_q) = p;				\
		ligature_present = FALSE;			\
	}							\
}

#define pop_lig_stack()						\
{	if (lig_ptr(lig_stack) != null) {			\
		t = link(t) = lig_ptr(lig_stack);		\
		incr(j);					\
	}							\
	p = lig_stack;						\
	lig_stack = link(p);					\
	free_node(p, SMALL_NODE_SIZE);				\
	if (lig_stack == null) {				\
		set_cur_r();					\
	} else {						\
		cur_r = character(lig_stack);			\
	}							\
}

#define lig_replace() 						\
{	if (cur_l == NON_CHAR) 					\
		lft_hit = TRUE;					\
	if (j == n && lig_stack == null)			\
		rt_hit = TRUE;					\
	 if(interrupted) throw std::logic_error("interrupted");\
	switch (op_byte(q)) {					\
	case 1: case 5:						\
		cur_l = rem_byte(q);				\
		ligature_present = TRUE;			\
		break;						\
	case 2: case 6:						\
		cur_r = rem_byte(q);				\
		if (lig_stack != null) {			\
			character(lig_stack) = cur_r;		\
		} else {					\
			lig_stack = new_lig_item(cur_r);	\
			if (j == n) {				\
				bchar = NON_CHAR;		\
			} else {				\
				p = new_avail();		\
				lig_ptr(lig_stack) = p;		\
				character(p) = hu[j + 1];	\
				font(p) = hf;			\
			}					\
		}						\
		break;						\
	case 3:							\
		cur_r = rem_byte(q);				\
		p = lig_stack;					\
		lig_stack = new_lig_item(cur_r);		\
		link(lig_stack) = p;				\
		break;						\
	case 7: case 11:					\
		wrap_lig(FALSE);				\
		cur_q = t;					\
		cur_l = rem_byte(q);				\
		ligature_present = TRUE;			\
		break;						\
	default:						\
		cur_l = rem_byte(q);				\
		ligature_present = TRUE;			\
		if (lig_stack != null) {			\
			pop_lig_stack();			\
		} else if (j == n) {				\
			goto done;				\
		} else {					\
			append_charnode_to_t(cur_r);		\
			incr(j);				\
			set_cur_r();				\
		}						\
		break;						\
	}							\
	if (op_byte(q) > 4 && op_byte(q) != 7) {		\
		goto done;					\
	}							\
	goto contin;						\
}


int tex::reconstitute(int j, int n, int bchar, int hchar)
	{
	scal	w;
	qcell	*k, q;
	ptr	p, t;
	int	cur_rh;
	int	test_char;
	
	hyphen_passed = 0;
	t = hold_head;
	link(hold_head) = null;
	w = 0;
	cur_l = hu[j];
	cur_q = t;
	if (j == 0) {
		ligature_present = init_lig;
		p = init_list;
		if (ligature_present)
			lft_hit = init_lft;
		while (p != null) {
			append_charnode_to_t(character(p));
			p = link(p);
		}
	} else if (cur_l < NON_CHAR) {
		append_charnode_to_t(cur_l);
	}
	lig_stack = null;
	set_cur_r();

contin:
	if (cur_l == NON_CHAR) {
		k = bchar_label(hf);
		if (k == NON_ADDRESS) {
			goto done;
		} else {
			q = *k;
		}
	} else {
		q = char_info(hf, cur_l);
		if (char_tag(q) != LIG_TAG)
			goto done;
		k = lig_kern_start(hf, q);
		q = *k;
		if (skip_byte(q) > STOP_FLAG) {
			k = lig_kern_restart(hf, q);
			q = *k;
		}
	}
	test_char = (cur_rh < NON_CHAR) ? cur_rh : cur_r;
	loop {
		if (next_char(q) == test_char
		&& skip_byte(q) <= STOP_FLAG) {
			if (cur_rh < NON_CHAR) {
				hyphen_passed = j;
				hchar = NON_CHAR;
				cur_rh = NON_CHAR;
				goto contin;
			} else {
				if (hchar < NON_CHAR && odd(hyf[j])) {
					hyphen_passed = j;
					hchar = NON_CHAR;
				}
				if (op_byte(q) < KERN_FLAG) {
					lig_replace();
				}
				w = char_kern(hf, q);
				goto done;
			}
		}
		if (skip_byte(q) >= STOP_FLAG) {
			if (cur_rh == NON_CHAR) {
				goto done;
			} else {
				cur_rh = NON_CHAR;
				goto contin;
			}
		}
		k += skip_byte(q) + 1;
		q = *k;
	}
done:
	wrap_lig(rt_hit);
	if (w != 0) {
		t = link(t) = new_kern(w);
		w = 0;
	}
	if (lig_stack != null) {
		cur_q = t;
		cur_l = character(lig_stack);
		ligature_present = TRUE;
		pop_lig_stack();
		goto contin;
	}
	return j;
}

#define set_cur_lang() \
	{cur_lang = (language <= 0 || language > 255) ? 0 : language;}

void tex::new_hyph_exceptions ()
{
	ptr	p, q;

	scan_left_brace();
	set_cur_lang();
	hn = 0;
	p = null;
	loop {
		get_x_token();

	reswitch:
		switch (cur_cmd)
		{
		case LETTER:
		case OTHER_CHAR:
		case CHAR_GIVEN:
			if (cur_chr == '-') {
				if (hn < 63) {
					q = new_avail();
					link(q) = p;
					info(q) = hn;
					p = q;
				}
			} else {
				if (lc_code(cur_chr) == 0) {
					print_err("Not a letter");
					help_hyph_lccode();
					error();
				} else if (hn < 63) {
					incr(hn);
					hc[hn] = lc_code(cur_chr);
				}
			}
			break;
		
		case CHAR_NUM:
			scan_char_num();
			cur_chr = cur_val;
			cur_cmd = CHAR_GIVEN;
			goto reswitch;

		case SPACER:
		case RIGHT_BRACE:
			if (hn > 1)
				enter_hyph_exception(p);
			if (cur_cmd == RIGHT_BRACE)
				return;
			hn = 0;
			p = null;
			break;

		default:
			print_err("Improper ");
			print_esc("hyphenation");
			print(" will be flushed");
			help_hyph();
			error();
			break;
		}
	}
}

void tex::enter_hyph_exception(ptr p)
	{
	ptr	q;
	int	h, j, l, m;
	str 	s, t, u, v, w;

	incr(hn);
	hc[hn] = cur_lang;
	str_room(hn);
	h = 0;
	for (j = 1; j <= hn; incr(j)) {
		h = (h + h + hc[j]) % HYPH_SIZE;
		append_char(hc[j]);
	}
	l = cur_length();
	s = make_str();
	if (hyph_count == HYPH_SIZE)
		overflow("exception dictionary", HYPH_SIZE);
	incr(hyph_count);
	while (hyph_word[h] != null_str) {
		w = hyph_word[h];
		m = hyph_len[h];
		if (m < l)
			goto found;
		if (m > l)
			goto not_found;
		u = w;
		v = s;
		while (m--) {
			if (*u < *v)
				goto found;
			if (*u > *v)
				goto not_found;
			incr(u);
			incr(v);
		}

	found:
		q = hyph_list[h];
		hyph_list[h] = p;
		p = q;
		t = hyph_word[h];
		hyph_word[h] = s;
		s = t;
		m = hyph_len[h];
		hyph_len[h] = l;
		l = m;
	
	not_found:
		if (h > 0) {
			decr(h);
		} else {
			h = HYPH_SIZE;
		}
	}
	hyph_word[h] = s;
	hyph_len[h] = l;
	hyph_list[h] = p;
}

void tex::new_patterns ()
	{
//	trie_t	z;
	int	c, k, l, p, q, v; //r, s, v;
	bool	first_child;
	bool	digit_sensed;

	if (trie_not_ready == FALSE) {
		print_err("Too late for ");
		print_esc("patterns");
		help_patterns();
		error();
		scan_toks(FALSE, FALSE);
		flush_list(def_ref);
		return;
	}
	set_cur_lang();
	scan_left_brace();
	k = hyf[0] = 0;
	digit_sensed = FALSE;
	loop {
		get_x_token();
		switch (cur_cmd)
		{
		case LETTER:
		case OTHER_CHAR:
			if (digit_sensed || cur_chr < '0' || cur_chr > '9') {
				if (cur_chr == '.') {
					cur_chr = 0;
				} else {
					cur_chr = lc_code(cur_chr);
					if (cur_chr == 0) {
						print_err("Nonletter");
						help1("(See Appendix H.)");
						error();
					}
				}
				if (k < 63) {
					incr(k);
					hc[k] = cur_chr;
					hyf[k] = 0;
					digit_sensed = FALSE;
				}
			} else if (k < 63){
				hyf[k] = cur_chr - '0';
				digit_sensed = TRUE;
			}
			break;

		case SPACER:
		case RIGHT_BRACE:
			if (k == 0) {
				if (cur_cmd == RIGHT_BRACE)
					goto done;
				break;
			}
			if (hc[1] == 0)
				hyf[0] = 0;
			if (hc[k] == 0)
				hyf[k] = 0;
			v = 0;
			l = k;
			loop {
				if (hyf[l])
					v = new_trie_op(k - l, hyf[l], v);
				if (l > 0) {
					decr(l);
				} else {
					break;
				}
			}
			q = 0; 
			hc[0] = cur_lang;
			while (l <= k) {
				c = hc[l];
				incr(l);
				p = trie_l[q];
				first_child = TRUE;
				while (p > 0 && c > trie_c[p]) {
					q = p;
					p = trie_r[q];
					first_child = FALSE;
				}
				if (p == 0 || c < trie_c[p]) {
					check_trie_ptr();
					incr(trie_ptr);
					trie_r[trie_ptr] = p;
					p = trie_ptr;
					trie_l[p] = 0;
					if (first_child) {
						trie_l[q] = p;
					} else {
						trie_r[q] = p;
					}
					trie_c[p] = c;
					trie_o[p] = 0;
				}
				q = p;
			}
			if (trie_o[q]) {
				print_err("Duplicate pattern");
				help1("(See Appendix H.)");
				error();
			}
			trie_o[q] = v;
			if (cur_cmd == RIGHT_BRACE)
				goto done;
			k = hyf[0] = 0;
			digit_sensed = FALSE;
			if (cur_cmd == RIGHT_BRACE)
				goto done;
			break;

		default:
			print_err("Bad ");
			print_esc("patterns");
			help1("(See Appendix H.)");
			error();
			break;
		}
	}
done:
	return;
}

void tex::check_trie_ptr()
	{
	if (trie_ptr == TRIE_SIZE) {
		overflow("pattern memory", TRIE_SIZE);
		}
	}

void tex::init_trie ()
	{
	int	j, k, p, r, s, t;
	trie_t	z;

	op_start[0] = 0;
	for (j = 1; j <= 255; incr(j))
		op_start[j] = op_start[j - 1] + trie_op_used[j - 1];
	for (j = 1; j <= trie_op_ptr; incr(j))
		trie_op_hash[j] = op_start[trie_op_lang[j]] + trie_op_val[j];
	for (j = 1; j <= trie_op_ptr; incr(j)) {
		while (trie_op_hash[j] > j) {
			k = trie_op_hash[j];
			t = hyf_distance[k];
			hyf_distance[k] = hyf_distance[j];
			hyf_distance[j] = t;
			t = hyf_num[k];
			hyf_num[k] = hyf_num[j];
			hyf_num[j] = t;
			t = hyf_next[k];
			hyf_next[k] = hyf_next[j];
			hyf_next[j] = t;
			trie_op_hash[j] = trie_op_hash[k];
			trie_op_hash[k] = k;
		}
	}
	for (p = 0; p <= TRIE_SIZE; incr(p))
		trie_hash[p] = 0;
	trie_root = trie_compress(trie_root);
	for (p = 0; p <= trie_ptr; incr(p))
		trie_ref[p] = 0;
	for (p = 0; p <= 255; incr(p))
		trie_min[p] = p + 1;
	trie_link(0) = 1;
	trie_max = 0;
	if (trie_root != 0) {
		first_fit(trie_root);
		trie_pack(trie_root);
	}
	z.s = 0;
	z.u_s.bb.b0 = z.u_s.bb.b1 = 0;
	if (trie_root == 0) {
		for (r = 0; r <= 256; incr(r))
			trie[r] = z;
		trie_max = 256;
	} else {
		trie_fix(trie_root);
		for (r = 0; r <= trie_max; r = s) {
			s = trie_link(r);
			trie[r] = z;
		}
	}
	trie_char(0) = '?';
	trie_not_ready = FALSE;
	free_pattern_memory();
}

int tex::new_trie_op(int d, int n, int v)
	{
	int	h;
	int	u;
	int	l;

	h = abs(n + 313 * d + 361 * v + 1009 * cur_lang);
	h = h % (TRIE_OP_SIZE + TRIE_OP_SIZE) - TRIE_OP_SIZE;
	loop {
		l = trie_op_hash[h];
		if (l == 0) {
			if (trie_op_ptr == TRIE_OP_SIZE) {
				overflow("pattern memory ops", TRIE_OP_SIZE);
			}
			u = trie_op_used[cur_lang];
			if (u == MAX_TRIE_OPS_PER_LANG) {
				overflow("pattern memory ops per language",
					MAX_TRIE_OPS_PER_LANG);
			}
			incr(u);
			trie_op_used[cur_lang] = u;
			incr(trie_op_ptr);
			hyf_distance[trie_op_ptr] = d;
			hyf_num[trie_op_ptr] = n;
			hyf_next[trie_op_ptr] = v;
			trie_op_lang[trie_op_ptr] = cur_lang;
			trie_op_val[trie_op_ptr] = u;
			trie_op_hash[h] = trie_op_ptr;
			return u;
		}
		if (hyf_distance[l] == d
		&& hyf_num[l] == n
		&& hyf_next[l] == v
		&& trie_op_lang[l] == cur_lang) {
			return (trie_op_val[l]);
		}
		if (h > -TRIE_OP_SIZE) {
			decr(h);
		} else {
			h = TRIE_OP_SIZE;
		}
	}
}

int tex::trie_compress(int p)
	{
	if (p == 0)
		return 0;
	trie_l[p] = trie_compress(trie_l[p]);
	trie_r[p] = trie_compress(trie_r[p]);
	return (trie_node(p));
	}
		
int tex::trie_node(int p)
	{
	int	h;
	int	q;

	h = trie_c[p] + 1009 * trie_o[p] +
		2718 * trie_l[p] + 3142 * trie_r[p];
	h = abs(h) % TRIE_SIZE;
	loop {
		q = trie_hash[h];
		if (q == 0) {
			trie_hash[h] = p; 
			return p;
		}
		if (trie_c[q] == trie_c[p]
		&& trie_o[q] == trie_o[p]
		&& trie_l[q] == trie_l[p]
		&& trie_r[q] == trie_r[p])
			return q;
		if (h > 0) {
			decr(h);
		} else {
			h = TRIE_SIZE;
		}
	}
}

void tex::trie_pack(int p)
	{
	int	q;
	
	do {
		q = trie_l[p];
		if (q > 0 && trie_ref[q] == 0) {
			first_fit(q);
			trie_pack(q);
			}
		p = trie_r[p];
		} while (p);
	}

void tex::first_fit(int p)
	{
	int	c, h, l, r, ll, q, z;

	c = trie_c[p];
	z = trie_min[c];
	loop {
		h = z - c;
		if (trie_max < h + 256) {
			if (TRIE_SIZE <= h + 256)
				overflow("pattern memory", TRIE_SIZE);
			while (trie_max != h + 256) {
				incr(trie_max); 
				trie_taken[trie_max] = FALSE;
				trie_link(trie_max) = trie_max + 1;
				trie_back(trie_max) = trie_max - 1;
			}
		}
		if (trie_taken[h])
			goto not_found;
		for (q = trie_r[p]; q > 0; q = trie_r[q])
			if (trie_link(h + trie_c[q]) == 0)
				goto not_found;
		goto found;

	not_found:
		z = trie_link(z);
	}

found:
	trie_taken[h] = TRUE;
	trie_ref[p] = h;
	for (q = p; q > 0; q = trie_r[q]) {
		z = h + trie_c[q];
		l = trie_back(z);
		r = trie_link(z);
		trie_back(r) = l;
		trie_link(l) = r;
		trie_link(z) = 0;
		if (l < 256) {
			ll = (z < 256) ? z : 256;
			while (l != ll) {
				trie_min[l] = r;
				incr(l);
			}
		}
	}
}

void tex::trie_fix(int p)
	{
	int	c;
	int	q;
	int	z;

	z = trie_ref[p];
	while (p) {
		q = trie_l[p];
		c = trie_c[p];
		trie_link(z + c) = trie_ref[q];
		trie_char(z + c) = c;
		trie_op(z + c) = trie_o[p];
		if (q > 0)
			trie_fix(q);
		p = trie_r[p];
	}
}

void tex::init_pattern_memory ()
	{
	int	l;
	int	p;

	l = TRIE_OP_SIZE + 1;
	trie_op_hash = (int *) malloc ((l+l)*sizeof(int));
	trie_op_hash += l;
	trie_op_val = (int *) malloc (l*sizeof(int));
	trie_op_lang = (int *) malloc (l*sizeof(int));
	trie_op_used = (int *) malloc (256*sizeof(int));
	for (p = -TRIE_OP_SIZE; p <= TRIE_OP_SIZE; incr(p))
		trie_op_hash[p] = 0;
	for (p = 0; p <= 255; incr(p))
		trie_op_used[p] = 0;
	trie_op_ptr = 0;

	l = TRIE_SIZE + 1;
	trie_hash = (int *) malloc (l*sizeof(int));
	trie_taken = (bool *) malloc (l*sizeof(bool));
	trie_c = (int *) malloc (l*sizeof(int));
	trie_o = (int *) malloc (l*sizeof(int));
	trie_l = (int *) malloc (l*sizeof(int));
	trie_r = (int *) malloc (l*sizeof(int));
	trie_min = (int *) malloc(256*sizeof(int));
	trie_root = 0;
	trie_c[0] = 0;
	trie_ptr = 0;
	}

void tex::free_pattern_memory ()
	{
	free(trie_op_hash - TRIE_OP_SIZE - 1);
	free(trie_op_used);
	free(trie_op_val);
	free(trie_op_lang);
	free(trie_hash);
	free(trie_taken);
	free(trie_min);
	free(trie_c);
	free(trie_o);
	free(trie_l);
	free(trie_r);
	}

void tex::_hyph_init ()
	{
	}

void tex::_hyph_init_once ()
	{
	int	k;

	init_pattern_memory();
	trie = (trie_t *)malloc((TRIE_SIZE+1)*sizeof(trie_t));
	op_start = (int *)malloc(256*sizeof(int));
	hyf_distance = (int *)malloc((TRIE_OP_SIZE+1)*sizeof(int));
	hyf_num = (int *)malloc((TRIE_OP_SIZE+1)*sizeof(int));
	hyf_next = (int *)malloc((TRIE_OP_SIZE+1)*sizeof(int));
	hyph_word = (str *)malloc((HYPH_SIZE+1)*sizeof(str));
	hyph_len = (int *)malloc((HYPH_SIZE+1)*sizeof(int));
	hyph_list = (ptr *)malloc((HYPH_SIZE+1)*sizeof(ptr));
	for (k = 0; k <= HYPH_SIZE; incr(k)) {
		hyph_word[k] = null_str;
		hyph_list[k] = null;
	}
	hyph_count = 0;
	trie_not_ready = TRUE;
	}

/*
**	Help text
*/

void tex::help_patterns()
	{
	help1("All patterns must be given before typesetting begins.");
	}

void tex::help_hyph_lccode()
	{
	help2("Letters in \\hyphenation words must have \\lccode > 0",
	"Proceed; I'll ignore the character I just read.");
	}

void tex::help_hyph()
	{
	help2("Hyphenation exceptions must contain only letters",
			"and hyphens. But continue; I'll forgive and forget.");
	}

