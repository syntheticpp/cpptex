
/*
 * Common TeX:toklist.c:3.2:EPA
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 *
 * 
 */

#include "tex.h"

ptr tex::str_toks ()
	{
	str	s;
	tok	t;
	ptr	p, q;

	p = q = new_token();
	token_link(p) = null;
	token_ref_count(p) = 0;
	for (s = cur_str; s < cur_str_ptr; incr(s)) {
		t = *s;
		if (t == ' ')
			t = SPACE_TOKEN;
		else t += OTHER_TOKEN;
		q = token_link(q) = new_token();
		token(q) = t;
	}
	flush_str();
	return p;
	}
		
ptr tex::the_toks ()
	{
	ptr	p, q, r;
	int	old_setting;

	get_x_token();
	scan_something_internal(TOK_VAL, FALSE);
	if (cur_val_level >= IDENT_VAL) {
		p = q = new_token();
		token_link(q) = null;
		token_ref_count(q) = 0;
		if (cur_val_level == IDENT_VAL) {
			q = token_link(q) = new_token();
			token(q) = sym2tok(cur_val);
		} else if ((ptr)cur_val != null) {
			r = token_link(cur_val);
			while (r != null ) {
				q = token_link(q) = new_token();
				token(q) = token(r);
				r = token_link(r);
			}
		}
		return p;
	} else {
		old_setting = selector;
		selector = NEW_STRING;
		switch (cur_val_level)
		{
		case INT_VAL:
			print_int(cur_val);
			break;

		case DIMEN_VAL:
			print_scaled(cur_val);
			print("pt");
			break;

		case GLUE_VAL:
			print_spec(cur_val, "pt");
			delete_glue_ref(cur_val);
			break;

		case MU_VAL:
			print_spec(cur_val,"mu");
			delete_glue_ref(cur_val);
			break;
		}
		selector = old_setting;
		return (str_toks());
		}
	}

void tex::conv_toks ()
	{
	int	c;
	ptr	p;
	int	old_setting;
	int	save_scanner_status;

	c = cur_chr;
	switch (c)
	{
	case NUMBER_CODE: 
	case ROMAN_NUMERAL_CODE:
		scan_int();
		break;

	case STRING_CODE:
	case MEANING_CODE:
		save_scanner_status = scanner_status;
		scanner_status = NORMAL;
		get_token();
		scanner_status = save_scanner_status;
		break;
	
	case FONT_NAME_CODE:
		scan_font_ident();
		break;

	case JOB_NAME_CODE:
		if (job_name == null_str)
			open_log_file();
		break;
	}
	old_setting = selector;
	selector = NEW_STRING;
	switch (c)
	{
	case NUMBER_CODE:
		print_int(cur_val);
		break;
	
	case ROMAN_NUMERAL_CODE:
		print_roman_int(cur_val);
		break;

	case STRING_CODE:
		if (cur_cs != 0)
			sprint_cs(cur_cs);
		else print_char(cur_chr);
		break;
	
	case MEANING_CODE:
		print_meaning();
		break;

	case FONT_NAME_CODE:
		print(font_name(cur_val));
		if (font_size(cur_val) != font_dsize(cur_val)) {
			print(" at ");
			print_scaled(font_size(cur_val));
			print("pt");
		}
		break;

	case JOB_NAME_CODE:
		print(job_name);
		break;
	}
	selector = old_setting; 
	p = str_toks();
	ins_list(token_link(p));
	free_token(p);
	}

void tex::scan_toks(bool macro_def, bool xpand)
	{
	ptr	p, q;
	tok	s, t;
	int	unbalance;
	tok	hash_brace;

#define BAD_PARAM_NO "Illegal parameter number in definition of "
#define ONLY_9_PARAMS "You already have nine parameters"
#define CONSEC_PARAMS "Parameters must be numbered consecutively"

	if (macro_def)
		scanner_status = DEFINING;
	else scanner_status = ABSORBING;
	warning_cs = cur_cs;
	def_ref = new_token();
	token_ref_count(def_ref) = 0;
	p = def_ref;
	hash_brace = 0;
	t = ZERO_TOKEN;
	if (macro_def) {
		loop {
			get_token();
			if (cur_tok < RIGHT_BRACE_LIMIT)
				break;
			if (cur_cmd == MAC_PARAM) {
				s = MATCH_TOKEN + cur_chr;
				get_token();
				if (cur_cmd == LEFT_BRACE) {
					hash_brace = cur_tok; 
					store_new_token(cur_tok);
					store_new_token(END_MATCH_TOKEN);
					goto done;
				}
				if (t == ZERO_TOKEN + 9) {
					print_err(ONLY_9_PARAMS);
					help_param_count();
					error();
				} else {
					incr(t);
					if (cur_tok != t) {
						print_err(CONSEC_PARAMS);
						help_param_num();
						back_error();
					}
					cur_tok = s;
				}
			}
			store_new_token(cur_tok);
		}
		store_new_token(END_MATCH_TOKEN);
		if (cur_cmd == RIGHT_BRACE) {
			print_err("Missing { inserted");
			incr(align_state); 
			help_left_brace();
			error();
			goto found;
		}
	} else {
		scan_left_brace();
	}

done:
	unbalance = 1;
	loop {
		if (xpand) {
			loop {
				get_next();
				if (cur_cmd <= MAX_COMMAND)
					break;
				if (cur_cmd != THE) {
					expand();
				} else {
					q = token_link(the_toks()); 
					if (q != null) {
						p = token_link(p) = q;
						while (token_link(q))
							p = q = token_link(q);
					}
				}
			}
			x_token();
		} else {
			get_token();
		}
		if (cur_tok < RIGHT_BRACE_LIMIT) {
			if (cur_cmd < RIGHT_BRACE) {
				incr(unbalance);
			} else {
				decr(unbalance);
				if (unbalance == 0)
					break;
			}
		} else if (cur_cmd == MAC_PARAM && macro_def) {
			s = cur_tok;
			if (xpand)
				get_x_token();
			else get_token();
			if (cur_cmd != MAC_PARAM) {
				if (cur_tok <= ZERO_TOKEN || cur_tok > t) {
					print_err(BAD_PARAM_NO);
					sprint_cs(warning_cs);
					help_param_use();
					back_error(); 
					cur_tok = s;
				} else {
					cur_tok = OUT_PARAM_TOKEN +
						cur_chr - '0';
				}
			}
		}
		store_new_token(cur_tok);
	}

found:
	scanner_status = NORMAL;
	if (hash_brace != 0)
		store_new_token(hash_brace);
}

void tex::read_toks(int n, sym r)
	{
	ptr	p, q;
	int	m, s;

str NO_READ = "*** (cannot \\read from terminal in nonstop modes)";

	scanner_status  = DEFINING;
	warning_cs = r;
	def_ref = new_token();
	token_ref_count(def_ref) = 0;
	p = def_ref;
	store_new_token(END_MATCH_TOKEN);
	if (n < 0 || n > 15)
		m = 16;
	else m = n;
	s = align_state;
	align_state = 1000000;
	do {
		begin_file_reading();
		index = m + 1;
		if (read_open[m] == CLOSED) {
			if (interaction > NONSTOP_MODE) {
				if (n < 0) {
					prompt_input(null_str);
				} else {
					print_ln();
					sprint_cs(r);
					prompt_input("=");
					n = -1;
				}
				strcpy((char *)buffer, cur_str);
				limit = buffer + cur_length();
			} else {
				fatal_error(NO_READ);
			}
		} else if (read_open[m] == OPENED) {
			if (input_ln(read_file[m])) {
				read_open[m] = NORMAL;
			} else {	
				a_close(read_file[m]);
				read_open[m] = CLOSED;
			}
		} else {
			if (!input_ln(read_file[m])) {
				a_close(read_file[m]);
				read_open[m] = CLOSED;
				if (align_state != 1000000) {
					runaway();
					print_err("File ended within ");
					print_esc("read");
					help_read();
					align_state = 1000000;
					error();
				}
			}
		}
		if (end_line_char_active)
			*++limit = end_line_char;
		cur_input.state_field = NEW_LINE;
		loop {
			get_token();
			if (cur_tok == 0)
				break; 
			store_new_token(cur_tok);
		}
		end_file_reading();
	} while (align_state != 1000000);
	cur_val = def_ref;
	scanner_status = NORMAL; 
	align_state = s;
	}

void tex::show_token_list(ptr p, ptr q, int l)
	{
	int	c;
	int	m;
	int	n;
	int	match_chr;

	match_chr = '#';
	n = '0';
	for (tally = 0; p != null && tally < l; p = token_link(p)) {
		if (p == q)
			set_trick_count();
		if (token(p) >= CS_TOKEN_FLAG) {
			print_cs((char *)tok2sym(token(p)));
		} else {
			m = token(p) / 0400;
			c = token(p) % 0400;
			if (token(p) < 0 || c > 255) {
				print_esc("BAD.");
			} else {
				switch (m)
				{
				case LEFT_BRACE:
				case RIGHT_BRACE:
				case MATH_SHIFT:
				case TAB_MARK:
				case SUP_MARK:
				case SUB_MARK:
				case SPACER:
				case LETTER:
				case OTHER_CHAR:
					print_ASCII(c); 
					break;
				
				case MAC_PARAM:
					print_ASCII(c);
					print_ASCII(c);
					break;
				
				case OUT_PARAM:
					print_ASCII(match_chr);
					if (c <= 9) {
						print_char(c + '0');
					} else {
						print("!");
						return;
					}
					break;
				
				case MATCH:
					match_chr = c;
					print_ASCII(c);
					incr(n);
					print_char(n);
					if (n > '9')
						return;
					break;
				
				case END_MATCH:
					print("->");
					break;
				
				default:
					print_err("BAD.");
					break;
				}
			}
		}
	}
	if (p != null)
		print_esc("ETC.");
}

void tex::token_show(ptr p)
	{ 
	if (p != null) {
		show_token_list(token_link(p), null, 10000000);
		}
	}

void tex::print_meaning ()
	{
	print_cmd_chr(cur_cmd, cur_chr);
	if (cur_cmd >= CALL) {
		print(":");
		print_ln();
		token_show(cur_chr);
	} else if (cur_cmd == TOP_BOT_MARK) {
		print(":");
		print_ln();
		token_show(cur_mark[cur_chr]);
	}
	}

void tex::flush_list(ptr p)
	{
	ptr	q;

	while (p != null) {
		q = token_link(p);
		free_token(p);
		p = q;
		}
	}


/*
**	Help text
*/

void tex::help_param_num()
	{
	help2("I've inserted the digit you should have used after the #.",
	"Type `1' to delete what you did use.");
	}

void tex::help_param_count()
	{
	help1("I'm going to ignore the # sign you just used.");
	}

void tex::help_left_brace()
	{
	help2("Where was the left brace? You said something like `\\def\\a}',",
	"which I'm going to interpret as `\\def\\a{}'.");
	}

void tex::help_param_use()
	{
	help3("You meant to type ## instead of #, right?",
	"Or maybe a } was forgotten somewhere earlier, and things",
	"are all screwed up? I'm going to assume that you meant ##.");
	}

void tex::help_read()
	{
	help1("This \\read has unbalanced braces.");
	}
