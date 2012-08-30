
/*
 * $Id: cond.c,v 1.6 2003/09/10 15:46:55 kp229 Exp $
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

ptr	tex::cond_ptr;

int	tex::cur_if;
int	tex::if_limit;
int	tex::if_line;
int	tex::skip_line;

int& tex::if_line_field(ptr I)
	{
	return ((if_t *) (I))->line_field;
	}

void tex::conditional ()
	{
	bool	b=false;
	sym	s;
	int	m, n;
	ptr	p, q, r;
	int	this_if;
	ptr	save_cond_ptr;
	int	save_scanner_status;

	push_cond();
	save_cond_ptr = cond_ptr;
	this_if = cur_chr;
	switch (this_if) {
		case IF_CHAR_CODE:
		case IF_CAT_CODE:
			
#define get_x_token_or_active_char() {get_x_token(); \
	if (cur_cmd == RELAX && cur_chr == NO_EXPAND_FLAG) { \
		cur_cmd = ACTIVE_CHAR; \
		cur_chr = tok2sym(cur_tok) - active_base[0]; }}

		get_x_token_or_active_char();
		if (cur_cmd > ACTIVE_CHAR || cur_chr > 255) {
			m = RELAX;
			n = 256;
			} 
		else {
			m = cur_cmd;
			n = cur_chr;
			}
		get_x_token_or_active_char();
		if (cur_cmd > ACTIVE_CHAR || cur_chr > 255) {
			cur_cmd = RELAX;
			cur_chr = 256;
		}
		if (this_if == IF_CHAR_CODE) {
			b = n == cur_chr;
		} else {
			b = m == cur_cmd;
		}
		break;
	
	case IF_INT_CODE:
	case IF_DIM_CODE:
		if (this_if == IF_INT_CODE) {
			scan_int();
		} else {
			scan_normal_dimen();
		}
		n = cur_val;
		get_nbx_token(); 
		if (cur_tok >= OTHER_TOKEN + '<'
		&& cur_tok <= OTHER_TOKEN + '>') {
			r = cur_tok - OTHER_TOKEN;
		} else {
			print_err("Missing = inserted for ");
			print_cmd_chr(IF_TEST, this_if);
			help_relation();
			back_error();
			r = '=';
		}
		if (this_if == IF_INT_CODE) {
			scan_int();
		} else {
			scan_normal_dimen();
		}
		switch (r) {
		case '<': b = n < cur_val; break;
		case '=': b = n == cur_val; break; 
		case '>': b = n > cur_val; break;
		}
		break;
	
	case IF_ODD_CODE:
		scan_int();
		b = odd(cur_val);
		break;
	
	case IF_VMODE_CODE:
		b = abs(mode) == VMODE;
		break;

	case IF_HMODE_CODE:
		b = abs(mode) == HMODE;
		break;

	case IF_MMODE_CODE:
		b = abs(mode) == MMODE;
		break;
	
	case IF_INNER_CODE:
		b = mode < 0;
		break;
	
	case IF_VOID_CODE:
	case IF_HBOX_CODE:
	case IF_VBOX_CODE:
		scan_eight_bit_int();
		p = box(cur_val);
		if (this_if == IF_VOID_CODE) {
			b = p == null;
		} else if (p == null) {
			b = FALSE;
		} else if (this_if == IF_HBOX_CODE) {
			b = type(p) == HLIST_NODE;
		} else {
			b = type(p) == VLIST_NODE;
		}
		break;

	case IFX_CODE:
		save_scanner_status = scanner_status;
		scanner_status = NORMAL;
		get_next();
		s = cur_cs;
		p = cur_cmd;
		q = cur_chr;
		get_next(); 
		if (cur_cmd != p) {
			b = FALSE;
		} else if (cur_cmd < CALL) {
			b = cur_chr == q;
		} else {
			p = token_link(cur_chr);
			q = token_link(equiv(s));
			if (p == q) {
				b = TRUE;
			} else {
				while (p != null && q != null) {
					if (token(p) != token(q)) {
						p = null;
					} else {
						p = token_link(p);
						q = token_link(q);
					}
				}
				b = p == null && q == null;
			}
		}
		scanner_status = save_scanner_status;
		break;

	case IF_EOF_CODE:
		scan_four_bit_int();
		b = read_open[cur_val] == CLOSED;
		break;
	
	case IF_TRUE_CODE:
		b = TRUE;
		break;

	case IF_FALSE_CODE:
		b = FALSE;
		break;

	case IF_CASE_CODE: 
		scan_int();
		n = cur_val;
		if (tracing_commands > 1) {
			begin_diagnostic();
			print("{case ");
			print_int(n);
			print("}");
			end_diagnostic(FALSE);
		}
		while (n != 0) {
			pass_text();
			if (cond_ptr == save_cond_ptr) {
				if (cur_chr == OR_CODE) {
					decr(n);
				} else {
					goto common_end;
				}
			} else if (cur_chr == FI_CODE) {
				pop_cond();
			}
		}
		change_if_limit(OR_CODE, save_cond_ptr);
		return;
	
	default:
		break;
	}

	if (tracing_commands > 1) {
		begin_diagnostic();
		print(b ? "{true}" : "{false}");
		end_diagnostic(FALSE);
	}

	if (b) {
		change_if_limit(ELSE_CODE, save_cond_ptr);
		return;
	}

	loop {
		pass_text(); 
		if (cond_ptr == save_cond_ptr) {
			if (cur_chr != OR_CODE)
				goto common_end;
			print_err("Extra ");
			print_esc("or");
			help_or();
			error();
		} else if (cur_chr == FI_CODE) {
			pop_cond();
		}
	}

common_end:
	if (cur_chr == FI_CODE) {
		pop_cond();
	} else {
		if_limit = FI_CODE;
	}
}

void tex::push_cond ()
	{
	ptr	p;
	
	p = new_node(sizeof(if_t));
	link(p) = cond_ptr;
	type(p) = if_limit;
	subtype(p) = cur_if;
	if_line_field(p) = if_line;
	cond_ptr = p;
	cur_if = cur_chr;
	if_limit = IF_CODE;
	if_line = line;
	}

void tex::pop_cond ()
	{
	ptr	p;
	
	p = cond_ptr;
	if_line = if_line_field(p);
	cur_if = subtype(p);
	if_limit = type(p);
	cond_ptr = link(p);
	free_node(p, sizeof(if_t));
	}

void tex::pass_text ()
	{
	int	l;
	int	save_scanner_status;
	
	l = 0;
	save_scanner_status = scanner_status;
	scanner_status = SKIPPING;
	skip_line = line;
	loop {
		get_next();
		if (cur_cmd == FI_OR_ELSE) {
			if (l == 0)
				break;
			if (cur_chr == FI_CODE)
				decr(l);
			} 
		else if (cur_cmd == IF_TEST) {
			incr(l);
			}
		}
	scanner_status = save_scanner_status;
	}

void tex::change_if_limit(int l, ptr p)
	{
	ptr	q;

	if (p == cond_ptr) {
		if_limit = l;
		} 
	else {
		q = cond_ptr; 
		loop {
			if (q == null)
				confusion("if");
			if (link(q) == p) {
				type(q) = l;
				return;
				}
			q = link(q);
			}
		}
	}

void tex::_cond_init ()
	{
	cond_ptr = null;
	if_limit = NORMAL;
	if_line = 0;
	cur_if = 0;
	}


/*
**	Help text
*/

void tex::help_or()
	{
	help1("I'm ignoring this; it doesn't match any \\if.");
	}

void tex::help_relation()
	{
	help1("I was expecting to see `<', `=', or `>'. Didn't.");
	}
