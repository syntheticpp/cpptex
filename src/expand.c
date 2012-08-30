
/*
 * $Id: expand.c,v 1.9 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include "tex.h"

int	tex::long_state;
ptr	tex::cur_mark[5];
ptr	tex::pstack[9];

void tex::get_x_token ()
	{
	restart:
	get_next();
	if (cur_cmd <= MAX_COMMAND)
		goto done;
	if (cur_cmd >= CALL) {
		if (cur_cmd < END_TEMPLATE) {
			macro_call();
			} 
		else {
			cur_cs = FROZEN_ENDV;
			cur_cmd = ENDV;
			goto done;
			}
		} 
	else {
		expand();
		}
	goto restart;
	
	done:
	if (cur_cs == null_sym)
		cur_tok = (tok) (cur_cmd * 0400 + cur_chr);
	else cur_tok = sym2tok(cur_cs);
	}

void tex::expand ()
	{
	ptr	p;
	ptr	q;
	str	s;
	tok	t;
	int	co_backup;
	int	cv_backup;
	int	cvl_backup;
	int	radix_backup;
	int	save_scanner_status;

	radix_backup = radix;
	co_backup = cur_order;
	cv_backup = cur_val;
	cvl_backup = cur_val_level;
	if (cur_cmd < CALL) {
		if (tracing_commands > 1)
			show_cur_cmd_chr();
		switch (cur_cmd) {
			case TOP_BOT_MARK:
				if (cur_mark[cur_chr] != null)
					begin_token_list(cur_mark[cur_chr], MARK_TEXT);
				break;
				
			case EXPAND_AFTER: 
				get_token();
				t = cur_tok;
				get_token();
				if (cur_cmd > MAX_COMMAND)
					expand();
				else back_input();
				cur_tok = t;
				back_input();
				break;
				
			case NO_EXPAND:
				save_scanner_status = scanner_status;
				scanner_status = NORMAL;
				get_token(); 
				scanner_status = save_scanner_status;
				t = cur_tok;
				back_input();
				if (t >= CS_TOKEN_FLAG) {
					p = new_token();
					token(p) = sym2tok(FROZEN_DONT_EXPAND);
					token_link(p) = loc;
					loc = start = p;
					}
				break;
				
			case CS_NAME:
				p = q = new_token();
				do {
					get_x_token();
					if (cur_cs == null_sym) {
						p = token_link(p) = new_token();
						token(p) = cur_tok;
						}
					} while (cur_cs == null_sym);
				if (cur_cmd != END_CS_NAME) {
					print_err("Missing ");
					print_esc("endcsname");
					print(" inserted");
					help_cs();
					back_error();
					}
				s = cur_str;
				p = token_link(q);
				while (p != null) {
					if (s >= cur_str_end) {
						overflow("str size", STR_SIZE);
						}
					*s++ = token(p) % 0400;
					p = token_link(p);
					}
				if (s > cur_str + 1) {
					no_new_control_sequence = FALSE;
					cur_cs = id_lookup(cur_str, s - cur_str);
					no_new_control_sequence = TRUE;
					} 
				else if (s == cur_str) {
					cur_cs = null_cs;
					} 
				else {
					cur_cs = single_base[*cur_str];
					}
				flush_list(q);
				if (eq_type(cur_cs) == UNDEFINED_CS)
					eq_define(cur_cs, RELAX, 256);
				cur_tok = sym2tok(cur_cs);
				back_input();
				break;
				
			case CONVERT:
				conv_toks();
				break;
				
			case THE:
				ins_list(token_link(the_toks()));
				break;
				
			case IF_TEST:
				conditional();
				break;
				
			case FI_OR_ELSE:
				if (cur_chr > if_limit) {
					if (if_limit == IF_CODE) {
						insert_relax();
						} 
					else {
						print_err("Extra ");
						print_cmd_chr(FI_OR_ELSE, cur_chr);
						help_extra_if();
						error();
						}
					} 
				else {
					while (cur_chr != FI_CODE)
						pass_text();
					pop_cond();
					}
				break;
				
			case INPUT:
				if (cur_chr > 0)
					force_eof = TRUE;
				else if (name_in_progress)
					insert_relax();
				else start_input("");
				break;
				
			default:
				print_err("Undefined control sequence");
				help_undefd();
				error();
				break;
			}
		} 
	else if (cur_cmd < END_TEMPLATE) {
		macro_call();
		} 
	else {
		cur_tok = sym2tok(FROZEN_ENDV);
		back_input();
		}
	cur_val = cv_backup;
	cur_val_level = cvl_backup;
	radix = radix_backup;
	cur_order = co_backup;
	}

void tex::insert_relax()
	{
	cur_tok = sym2tok(cur_cs);
	back_input();
	cur_tok = sym2tok(FROZEN_RELAX);
	back_input();
	token_type = INSERTED;
	}

void tex::macro_call()
	{
	int	m=0;
	int	n;
	ptr	p=0;
	ptr	q;
	ptr	r;
	ptr	s;
	ptr	t;
	ptr	u;
	ptr	v;
	int	match_chr=0;
	ptr	ref_count;
	ptr	rbrace_ptr=0;
	sym	save_warning_cs;
	int	save_scanner_status;
	int	unbalance;

	n = 0;
	ref_count = cur_chr;
	r = token_link(ref_count);
	save_scanner_status = scanner_status;
	save_warning_cs = warning_cs;
	warning_cs = cur_cs;
	if (tracing_macros > 0) {
		begin_diagnostic();
		print_ln();
		print_cs((char *)warning_cs);
		token_show(ref_count);
		end_diagnostic(FALSE);
	}
	if (token(r) != END_MATCH_TOKEN) {
		scanner_status = MATCHING;
		unbalance = 0;
		long_state = eq_type(cur_cs);
		if (long_state >= OUTER_CALL)
			long_state -= 2;
		do {
			if (token(r) > MATCH_TOKEN + 255
			|| token(r) < MATCH_TOKEN) {
				s = null;
			} else {
				match_chr = token(r) - MATCH_TOKEN;
				s = r = token_link(r);
				p = match_toks;
				token_link(p) = null;
				m = 0;
			}

		contin:
			get_token();
			if (cur_tok == token(r)) {
				r = token_link(r);
				if (token(r) >= MATCH_TOKEN &&
					token(r) <= END_MATCH_TOKEN) {
					if (cur_tok < LEFT_BRACE_LIMIT)
						decr(align_state);
					goto found;
				} else {
					goto contin;
				}
			}
			if (s != r) {
				if (s == null) {
					print_err("Use of ");
					sprint_cs(warning_cs);
					print(" doesn't match its definition");
					help_match();
					error();
					goto local_exit;
				} else {
					t = s;
					do {
						store_new_token(token(t));
						incr(m);
						u = token_link(t);
						v = s;
						loop {
							if (u == r) {
								if (cur_tok != token(v)) {
									break;
								} else {
									r = token_link(v);
									goto contin;
								}
							}
							if (token(u) != token(v))
								break;
							u = token_link(u);
							v = token_link(v);
						}
						t = token_link(t);
					} while (t != r);
					r = s;
				}
			}
			if (cur_tok == par_tok && long_state != LONG_CALL) {
				runaway_arg(n);
				align_state -= unbalance; 
				goto local_exit;
			}
			if (cur_tok < RIGHT_BRACE_LIMIT) {
				if (cur_tok < LEFT_BRACE_LIMIT) {
					unbalance = 1;
					loop {
						store_new_token(cur_tok);
						get_token();
						if (cur_tok == par_tok && 
							long_state != LONG_CALL) {
							runaway_arg(n);
							align_state -= unbalance; 
							goto local_exit;
						}
						if (cur_tok < RIGHT_BRACE_LIMIT) {
							if (cur_tok < LEFT_BRACE_LIMIT) {
								incr(unbalance);
							} else {
								decr(unbalance);
								if (unbalance == 0)
									break;
							}
						}
					}
					rbrace_ptr = p;
					store_new_token(cur_tok);
				} else {
					back_input();
					print_err("Argument of ");
					sprint_cs(warning_cs);
					print(" has an extra }");
					help_match_xtra();
					incr(align_state);
					long_state = CALL;
					cur_tok = par_tok;
					ins_error();
				}
			} else {
				if (cur_tok == SPACE_TOKEN &&
					token(r) <= END_MATCH_TOKEN &&
					token(r) >= MATCH_TOKEN)
					goto contin;
				store_new_token(cur_tok);
			}
			incr(m);
			if (token(r) > END_MATCH_TOKEN || token(r) < MATCH_TOKEN)
				goto contin;

		found:
			if (s != null) {
				if (m == 1 &&
					token(p) < RIGHT_BRACE_LIMIT &&
					p != match_toks) {
					token_link(rbrace_ptr) = null;
					free_token(p);
					p = token_link(match_toks);
					pstack[n] = token_link(p);
					free_token(p);
				} else {
					pstack[n] = token_link(match_toks);
				}
				incr(n);
				if (tracing_macros > 0) {
					begin_diagnostic();
					print_nl(null_str);
					print_char(match_chr);
					print_int(n);
					print("<-");
					show_token_list(pstack[n - 1], null, 1000);
					end_diagnostic(FALSE);
				}
			}
		} while (token(r) != END_MATCH_TOKEN);
	}
	token_link(match_toks) = null;
	while (cur_input.state_field == TOKEN_LIST && loc == null)
		end_token_list();
	begin_token_list(ref_count, MACRO);
	in_cs = warning_cs;
	name = text(warning_cs);
	loc = token_link(r);
	if (n > 0) {
		if (param_ptr + n > max_param_stack) {
			if (param_ptr + n >= param_end)
				overflow("parameter stack size", nparams);
			max_param_stack = param_ptr + n;
		}
		for (m = 0; m < n; incr(m))
			param_ptr[m] = pstack[m];
		param_ptr += n;
	}

local_exit:
	scanner_status = save_scanner_status; 
	warning_cs = save_warning_cs; 
}

void tex::runaway_arg(int n) 
	{
	int	m;

	if (long_state == CALL) { 
		runaway(); 
		print_err("Paragraph ended before "); 
		sprint_cs(warning_cs); 
		print(" was complete"); 
		help_runaway(); 
		back_error();
	}
	pstack[n] = token_link(match_toks); 
	for (m = 0; m <= n; incr(m))
		flush_list(pstack[m]);
}

void tex::x_token ()
{
	while (cur_cmd > MAX_COMMAND) {
		expand();
		get_next();
	}
	if (cur_cs == 0)
		cur_tok = (tok) (cur_cmd * 0400 + cur_chr);
	else cur_tok = sym2tok(cur_cs);
}

void tex::_expand_init ()
	{
	top_mark = null;
	first_mark = null;
	bot_mark = null;
	split_first_mark = null;
	split_bot_mark = null;
	}

	
/*
**	Help text
*/

void tex::help_runaway()
	{
	help3("I suspect you've forgotten a `}', causing me to apply this",
	"control sequence to too much text. How can we recover?",
	"My plan is to forget the whole thing and hope for the best.");
	}

void tex::help_match()
	{
	help4("If you say, e.g., `\\def\\a1{...}', then you must always",
	"put `1' after `\\a', since control sequence names are",
	"made up of letters only. The macro here has not been",
	"followed by the required stuff, so I'm ignoring it.");
	}

void tex::help_match_xtra()
	{
	help6("I've run across a `}' that doesn't seem to match anything.",
	"For example, `\\def\\a#1{...}' and `\\a}' would produce",
	"this error. If you simply proceed now, the `\\par' that",
	"I've just inserted will cause me to report a runaway",
	"argument that might be the root of the problem. But if",
	"your `}' was spurious, just type `2' and it will go away.");
	}

void tex::help_undefd()
	{
	help5("The control sequence at the end of the top line",
	"of your error message was never \\def'ed. If you have",
	"misspelled it (e.g., `\\hobx'), type `I' and the correct",
	"spelling (e.g., `I\\hbox'). Otherwise just continue,",
	"and I'll forget about whatever was undefined.");
	}

void tex::help_cs()
	{
	help2("The control sequence marked <to be read again> should",
	"not appear between \\csname and \\endcsname.");
	}

void tex::help_extra_if()
	{
	help1("I'm ignoring this; it doesn't match any \\if.");
	}
