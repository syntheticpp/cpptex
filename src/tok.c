
/*
 * $Id: tok.c,v 1.11 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002,2003      Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include "tok.h"
#include "error.h"
#include "print.h"

#define STACK_SIZE	200

int	nfiles;

#define INFILE_SIZE	45
#define PARAM_SIZE	100

bool tokeniser::is_sym(int TOK)  
	{ 
	return ((TOK) >= CS_TOKEN_FLAG); 
	};
		
tok tokeniser::sym2tok(symbols::sym SYM) 
	{ 
	return ((tok) (CS_TOKEN_FLAG + (char *) (SYM))); 
	};

symbols::sym tokeniser::tok2sym(tok TOK) 
	{ 
	return ((symbols::sym) ((TOK) - CS_TOKEN_FLAG)); 
	};
		
void tokeniser::get_token ()
	{
	sym_.no_new_control_sequence = false;
	get_next();
	sym_.no_new_control_sequence = true;
	if (sym_.cur_cs == 0) {
		cur_tok = cur_cmd * 0400 + cur_chr;
		} 
	else {
		cur_tok = sym2tok(sym_.cur_cs);
		}
	}

#define any_state(CAT) \
	case MID_LINE + CAT: \
	case SKIP_BLANKS + CAT: \
	case NEW_LINE + CAT

#define delims(STATE) \
	case STATE + commands::MATH_SHIFT: \
	case STATE + commands::TAB_MARK: \
	case STATE + commands::MAC_PARAM: \
	case STATE + commands::SUB_MARK: \
	case STATE + commands::LETTER: \
	case STATE + commands::OTHER_CHAR 

#define mid_line(CAT) \
	case MID_LINE + CAT

#define new_line(CAT) \
	case NEW_LINE + CAT

#define skip_blanks(CAT) \
	case SKIP_BLANKS + CAT

#define is_hex(C) \
	((C) >= '0' && (C) <= '9' || (C) >= 'a' && (C) <= 'f')

#define hex_to_cur_chr(C, CC) \
{	cur_chr = 16 * ((C <= '9') ? C - '0' : C - 'a' + 10); \
	cur_chr += (CC <= '9') ? CC - '0' : CC - 'a' + 10; \
}

void tokeniser::get_next ()
	{
	tok	t;
	int	c, cc;
	
	restart:
	sym_.cur_cs = 0;
	if (file_state) {
	reread:
		if (next <= limit) {
			cur_chr = *next++;
		reswitch:
			cur_cmd = sym_.cat_code(cur_chr);
			switch (cur_input.state_field + cur_cmd) {
				any_state(commands::IGNORE):
				skip_blanks(commands::SPACER):
				new_line(commands::SPACER):
					goto reread;
				
				any_state(commands::ESCAPE):
					get_cs();
				   cur_cmd = sym_.eq_type(sym_.cur_cs);
					cur_chr = sym_.equiv(sym_.cur_cs);
					if (cur_cmd >= commands::OUTER_CALL)
						check_outer_validity();
					break;
				
				any_state(commands::ACTIVE_CHAR):
					sym_.cur_cs = sym_.active_base[cur_chr];
				   cur_cmd = sym_.eq_type(sym_.cur_cs);
					cur_chr = sym_.equiv(sym_.cur_cs);
					cur_input.state_field = MID_LINE;
					if (cur_cmd >= commands::OUTER_CALL)
						check_outer_validity();
					break;
				
				any_state(commands::SUP_MARK):
					if (cur_chr != *next
						 || next >= limit
						 || (c = next[1]) >= 0200) {
						cur_input.state_field = MID_LINE;
						break;
						}
				   next += 2;
					if (is_hex(c) && next <= limit) {
						cc = *next;
						if (is_hex(cc)) {
							next++;
							hex_to_cur_chr(c, cc);
							goto reswitch;
							}
						}
					if (c < 0100) {
						cur_chr = c + 0100;
						} 
					else {
						cur_chr = c - 0100;
						}
					goto reswitch;
					
			   any_state(commands::INVALID_CHAR):
					throw tex::error("Text line contains an invalid character.");
			
				mid_line(commands::SPACER):
					cur_input.state_field = SKIP_BLANKS;
				   cur_chr = ' ';
					break;
			
				mid_line(commands::CAR_RET):
					next = limit + 1;
				   cur_cmd = commands::SPACER;
					cur_chr = ' ';
					break;
					
				skip_blanks(commands::CAR_RET):
				any_state(commands::COMMENT):
					next = limit + 1;
				   goto reread;
				
				new_line(commands::CAR_RET):
					next = limit + 1;
				   sym_.cur_cs = sym_.par_cs;
					cur_cmd = sym_.eq_type(sym_.cur_cs);
					cur_chr = sym_.equiv(sym_.cur_cs);
					if (cur_cmd >= commands::OUTER_CALL)
						check_outer_validity();
					break;
			
				mid_line(commands::LEFT_BRACE):
					incr(align_state);
				   break;

				skip_blanks(commands::LEFT_BRACE):
				new_line(commands::LEFT_BRACE):
					cur_input.state_field = MID_LINE;
				   incr(align_state);
					break;
					
			   mid_line(commands::RIGHT_BRACE):
					decr(align_state);
				   break;
			
			   skip_blanks(commands::RIGHT_BRACE):
			   new_line(commands::RIGHT_BRACE):
				   cur_input.state_field = MID_LINE;
				   decr(align_state);
					break;
			
			   delims(SKIP_BLANKS):
			   delims(NEW_LINE):
					cur_input.state_field = MID_LINE;
				   break;

				default:
					break;
				}
			} 
		else {
			cur_input.state_field = NEW_LINE; 
			if (index > 17) {
				incr(line);
				if (!force_eof) {
					if(!fil_.input_ln(cur_file)) {
						force_eof = true;
						}
					}
				if (force_eof) {
					tex::print(")");
					decr(open_parens);
					force_eof = false;
					end_file_reading();
					check_outer_validity();
					goto restart;
					}
				if (end_line_char_active())
					*++limit = sym_.end_line_char();
				} 
			else {
				if (!terminal_input) {
					cur_cmd = 0;
					cur_chr = 0;
					return;
					}
				if (input_ptr > input_stack) {
					end_file_reading();
					goto restart;
					}
				if (tex::selector < tex::LOG_ONLY)
					fil_.open_log_file();
				throw tex::error("Job aborted, no legal \\end found.");
//				if (interaction > tex::NONSTOP_MODE) {
//					if (limit <= buffer)
//						tex::print_nl(USER_CMD);
//					tex::print_ln();
//					prompt_input("*");
//					memcpy(buffer, cur_str, cur_length());
//					next = buffer;
//					limit = buffer + cur_length() - 1;
//					if (end_line_char_active)
//						*++limit = end_line_char;
//					flush_str();
//					} 
//				else {
//					fatal_error(JOB_ABORT);
//					}
				}
//			if(interrupted) throw std::logic_error("interrupted");
			goto reread;
			}
		} 
	else {
		if (loc != 0) {
			t = token(loc);
			loc = token_link(loc);
			if (t >= CS_TOKEN_FLAG) {
				sym_.cur_cs = tok2sym(t);
				cur_cmd = sym_.eq_type(sym_.cur_cs);
				cur_chr = sym_.equiv(sym_.cur_cs);
				if (cur_cmd >= commands::OUTER_CALL) {
					if (cur_cmd == commands::DONT_EXPAND) {
						sym_.cur_cs = tok2sym(token(loc));
						loc = 0;
						cur_cmd = sym_.eq_type(sym_.cur_cs);
						cur_chr = sym_.equiv(sym_.cur_cs);
						if (cur_cmd > commands::MAX_COMMAND) {
							cur_cmd = commands::RELAX;
							cur_chr = NO_EXPAND_FLAG;
							}
						} 
					else {
						check_outer_validity();
						}
					}
				} 
			else {
				cur_cmd = t / 0400;
				cur_chr = t % 0400;
				switch (cur_cmd) {
					case commands::LEFT_BRACE:
						incr(align_state);
						break;
						
					case commands::RIGHT_BRACE:
						decr(align_state);
						break;
						
					case commands::OUT_PARAM:
						begin_token_list(
							param_start[cur_chr - 1],
							PARAMETER
							);
						goto restart;
						
					default:
						break;
					}
				}
			} 
		else {
			end_token_list();
			goto restart;
			}
		}
	if (cur_cmd <= commands::CAR_RET && cur_cmd >= commands::TAB_MARK && align_state == 0) {
		if (scanner_status == ALIGNING)
			throw tex::error("(interwoven alignment preambles are not allowed)");
		cur_cmd = unset_info(cur_align);
		unset_info(cur_align) = cur_chr;
		if (cur_cmd == commands::OMIT)
			begin_token_list(omit_template, V_TEMPLATE);
		else begin_token_list(v_partg(cur_align), V_TEMPLATE);
		align_state = 1000000;
		goto restart;
		}
	}

void tokeniser::get_cs ()
	{
	byte	*nx;
	int	cat=0;
	int	d, c, cc=0;
	
	if (next > limit) {
		sym_.cur_cs = 0;
		return;
		}
	
#define reduce_expanded_cc(CC) 						\
{	if (*CC == cur_chr && cat == commands::SUP_MARK && CC < limit) { 		\
		if ((c = CC[1]) < 0200) {				\
			d = 2;						\
			if (is_hex(c) && CC + 2 <= limit) {		\
				cc = CC[2];				\
				if (is_hex(cc))				\
					incr(d);			\
			}						\
			if (d > 2) {					\
				hex_to_cur_chr(c, cc);			\
				CC[-1] = cur_chr;			\
			} else if (c < 0100) { 				\
				CC[-1] = c + 0100; 			\
			} else {					\
				CC[-1] = c - 0100; 			\
			}						\
			limit -= d;					\
			while (CC <= limit) { 				\
				CC[0] = CC[d];				\
				incr(CC);				\
			}						\
			goto start_cs;					\
		}							\
	}								\
}

start_cs: 
	nx = next;
	cur_chr = *nx++;
	cat = sym_.cat_code(cur_chr);
	if (cat == commands::LETTER)
		cur_input.state_field = SKIP_BLANKS;
	else if (cat == commands::SPACER)
		cur_input.state_field = SKIP_BLANKS;
	else cur_input.state_field = MID_LINE;
	if (cat == commands::LETTER && nx <= limit) {
		do {
			cur_chr = *nx++;
			cat = sym_.cat_code(cur_chr);
		} while (cat == commands::LETTER && nx <= limit);
		reduce_expanded_cc(nx);
		if (cat != commands::LETTER)
			decr(nx);
		if (nx > next + 1) {
			sym_.cur_cs = sym_.id_lookup((char *)next, nx - next);
			next = nx;
			return;
		}
	} else {
		reduce_expanded_cc(nx);
	}
	sym_.cur_cs = single_base[*next++];
}

void tokeniser::check_outer_validity ()
	{
	ptr	p, q;

	if (scanner_status != NORMAL) {
		deletions_allowed = false;
		if (sym_.cur_cs != 0) {
			if (cur_input.state_field == TOKEN_LIST || index < 1 || index > 17) {
				p = new_token();
				token(p) = sym2tok(sym_.cur_cs);
				back_list(p);
				}
			cur_cmd = commands::SPACER;
			cur_chr = ' ';
			}
		if (scanner_status > SKIPPING) {
			runaway();
			if (sym_.cur_cs == 0) { // FIXME: errors not finished
				throw tex::error("File ended.");
				} 
			else {
				sym_.cur_cs = 0;
				throw tex::error("Forbidden control sequence found");
				}
			tex::print(" while scanning ");
			p = new_token();
			switch (scanner_status) {
				case DEFINING:
					tex::print("definition");
					token(p) = RIGHT_BRACE_TOKEN + '}';
					break;
					
				case MATCHING:
					tex::print("use");
					token(p) = par_tok;
					long_state = commands::OUTER_CALL;
					break;
					
				case ALIGNING:
					tex::print("preamble");
					token(p) = RIGHT_BRACE_TOKEN + '}';
					q = p;
					p = new_token();
					token_link(p) = q;
					token(p) = sym2tok(symbols::FROZEN_CR);
					align_state = -1000000;
					break;
					
				case ABSORBING:
					tex::print("text"); 
					token(p) = RIGHT_BRACE_TOKEN + '}';
					break;

				case NORMAL:
				case SKIPPING:
					break;
				}
			ins_list(p);
			tex::print(" of ");
			sprint_cs(warning_cs);
			help_scanner();
			error();
			} 
		else {
			// FIXME: incomplete error handling
			throw tex::error("Incomplete");
			tex::print_cmd_chr(commands::IF_TEST, cur_if);
			tex::print("; all text was ignored after line ");
			tex::print_int(skip_line);
			if (sym_.cur_cs != 0) {
				sym_.cur_cs = 0;
				help_skip();
				} 
			else {
				help_skif();
				}
			cur_tok = sym2tok(symbols::FROZEN_FI);
			ins_error();
			}
		deletions_allowed = true;
		}
	}


// Push current input info (file handle, point to buffer and so on) onto the stack.

void tokeniser::push_input()
	{
	if(input_ptr > max_in_stack) {
		if (input_ptr == input_end)
			throw tex::overflow_error("input stack size", ninputs);
		max_in_stack = input_ptr;
		}
	if(file_state) {
		if (index == 0 || index > 17) {
			file_line = line;
			file_name = name;
			}
		} 
	else {
		cs_name = name;
		}
	*input_ptr++ = cur_input;
	}

void tokeniser::pop_input()
	{
	cur_input = *--input_ptr;
	if (file_state) {
		if (index == 0 || index > 17) {
			line = file_line;
			name = file_name;
			}
		} 
	else {
		name = cs_name;
		}
	}

void tokeniser::begin_token_list(ptr p, int t)
	{
	push_input();
	cur_input.state_field = TOKEN_LIST;
	start = p;
	token_type = t;
	if (t >= MACRO) {
		add_token_ref(p);
		if (t == MACRO) {
			param_start = param_ptr;
			} 
		else {
			loc = token_link(p);
			if (tracing_macros > 1) {
				begin_diagnostic();
				tex::print_nl(null_str);
				switch (t) {
					case MARK_TEXT:
						tex::print_esc("mark");
						break;
						
					case WRITE_TEXT:
						tex::print_esc("write");
						break;
						
					default:
						tex::print_toks_param(t - OUTPUT_TEXT);
						break;
					}
				tex::print("->");
				token_show(p);
				end_diagnostic(false);
				}
			}
		} 
	else {
		loc = p;
		}
	}

void tokeniser::end_token_list ()
	{
	if (token_type >= BACKED_UP) {
		if (token_type <= INSERTED) {
			flush_list(start);
			} 
		else {
			delete_token_ref(start);
			if (token_type == MACRO)
				while (param_ptr > param_start)
					flush_list(*--param_ptr);
			}
		} 
	else if (token_type == U_TEMPLATE) {
		align_state = 0;
		}
	pop_input();
	if(interrupted) throw std::logic_error("interrupted");
	}

// Push current input info onto the stack (see above) and allocate
// memory for a new file (alloc new buffer and set cur_input state).

void tokeniser::begin_file_reading ()
	{
	push_input();
	if (file_ptr > max_file_stack) {
		if (file_ptr == file_end)
			throw tex::overflow_error("file stack size", nfiles);
		max_file_stack = file_ptr;
		}
	in_file = (ptr)file_ptr++;
	cur_input.obj_field.f.buf_field = (byte *)new_str(BUF_SIZE);
	cur_input.state_field = MID_LINE;
	index = 0;
	}

void tokeniser::end_file_reading ()
	{
	if(index>17) {
		a_close(cur_file);
		}
	free_str((char *)cur_input.obj_field.f.buf_field);
	pop_input();
	decr(file_ptr);
	}

void tokeniser::back_input ()
	{
	ptr	p;
	
	while (cur_input.state_field == TOKEN_LIST && loc == 0)
		end_token_list();
	p = new_token();
	token(p) = cur_tok;
	if (cur_tok < RIGHT_BRACE_LIMIT) {
		if (cur_tok < LEFT_BRACE_LIMIT) {
			decr(align_state);
			} 
		else {
			incr(align_state);
			}
		}
	push_input();
	cur_input.state_field = TOKEN_LIST;
	loc = start = p;
	token_type = BACKED_UP;
	}

void tokeniser::back_error ()
	{
	OK_to_interrupt = false;
	back_input();
	OK_to_interrupt = true;
	error();
	}

void tokeniser::ins_error ()
	{
	OK_to_interrupt = false;
	back_input();
	token_type = INSERTED; 
	OK_to_interrupt = true;
	error();
	}

void tokeniser::clear_for_error_prompt()
	{
	while (cur_input.state_field != TOKEN_LIST
			 && terminal_input
			 && input_ptr > input_stack
			 && next > limit)
		end_file_reading();
	}

void tokeniser::runaway ()
	{
	ptr	p=0;

	if (scanner_status > SKIPPING) {
		tex::print_nl("Runaway ");
		switch (scanner_status) {
			case DEFINING:
				tex::print("definition");
				p = def_ref;
				break;
				
			case MATCHING:
				tex::print("argument");
				p = match_toks;
				break;
				
			case ALIGNING:
				tex::print("preamble");
				p = align_toks;
				break;
				
			case ABSORBING:
				tex::print("text");
				p = def_ref;
				break;
				
			case NORMAL:
			case SKIPPING:
				break;
			}
		tex::print("?");
		tex::print_ln();
		show_token_list(token_link(p), 0, tex::ERROR_LINE - 10);
		}
	}

// Display the location of the error

void tokeniser::show_context()
	{
	int	i, j, l, m, n, nn, p, q;
	int	old_setting;
	bool	bottom_line;

	base_ptr = input_ptr;
	push_input();
	nn = -1;
	bottom_line = false;
	loop {
		cur_input = *base_ptr;
		if (cur_input.state_field != TOKEN_LIST && 
			 (index > 17 || base_ptr == input_stack)) {
			bottom_line = true;
			}
		if (base_ptr == input_ptr - 1 || bottom_line || nn < error_context_lines) {
			if (base_ptr == input_ptr - 1 || cur_input.state_field != TOKEN_LIST || token_type != BACKED_UP || loc != 0) {
				tally = 0;
				old_setting = selector;
				if (file_state) {
					if (index <= 17) {
						if (terminal_input) {
							if (base_ptr == input_stack)
								tex::print_nl("<*>");
							else tex::print_nl("<insert> ");
							} 
						else {
							tex::print_nl("<read ");
							if (index == 17)
								tex::print("*");
							else tex::print_int(index - 1);
							tex::print(">");
							}
						} 
					else {
						tex::print(name_of_file);
						tex::print_nl(" l.");
						tex::print_int(file_line);
						}
					tex::print(" ");
					l = begin_pseudoprint();
					j = limit - buffer;
					if (*limit != end_line_char)
						incr(j);
					if (j > 0) {
						for (i = 0; i < j; incr(i)) {
							if (buffer + i == next)
								set_trick_count();
							tex::print_ASCII(buffer[i]);
							}
						}
					} 
				else {
					switch (token_type) {
						case PARAMETER:
							tex::print_nl("<argument> ");
							break;
							
						case U_TEMPLATE:
						case V_TEMPLATE:
							tex::print_nl("<template> ");
							break;
							
						case BACKED_UP:
							if (loc == 0)
								tex::print_nl("<recently read> "); 
							else  tex::print_nl("<to be read again> ");
							break;
							
						case INSERTED:
							tex::print_nl("<inserted text> ");
							break;
							
						case MACRO:
							tex::print_ln();
							tex::print_cs((char *)in_cs);
							break;
							
						case OUTPUT_TEXT:
							tex::print_nl("<output> ");
							break;
							
						case EVERY_PAR_TEXT:
							tex::print_nl("<everypar> ");
							break;
							
						case EVERY_MATH_TEXT:
							tex::print_nl("<everymath> ");
							break;
							
						case EVERY_DISPLAY_TEXT:
							tex::print_nl("<everydisplay> ");
							break;
							
						case EVERY_HBOX_TEXT:
							tex::print_nl("<everyhbox> ");
							break;
							
						case EVERY_VBOX_TEXT:
							tex::print_nl("<everyvbox> ");
							break;
							
						case EVERY_JOB_TEXT:
							tex::print_nl("<everyjob>");
							break;
							
						case EVERY_CR_TEXT:
							tex::print_nl("<everycr> ");
							break;
							
						case MARK_TEXT:
							tex::print_nl("<mark> ");
							break;
							
						case WRITE_TEXT:
							tex::print_nl("<write> ");
							break;
							
						default:
							tex::print_nl("? ");
							break;
						}
					l = begin_pseudoprint();
					if (token_type < MACRO)
						show_token_list(start, loc, 100000);
					else
						show_token_list(token_link(start),
											 loc, 100000);
					}
				selector = old_setting;
				if (trick_count == 1000000)
					set_trick_count();
				if (tally < trick_count)
					m = tally - first_count;
				else m = trick_count - first_count;
				if (l + first_count <= tex::HALF_ERROR_LINE) {
					p = 0;
					n = l + first_count;
					} 
				else {
					tex::print("...");
					p = l + first_count - tex::HALF_ERROR_LINE + 3;
					n = tex::HALF_ERROR_LINE;
					}
				for (q = p; q < first_count; incr(q))
					tex::print_char(tex::trick_buf[q % tex::ERROR_LINE]);
				tex::print_ln();
				for (q = 1; q <= n; incr(q))
					tex::print(" ");
				if (m + n <= tex::ERROR_LINE)
					p = first_count + m;
				else p = first_count + tex::ERROR_LINE - n - 3;
				for (q = first_count; q < p; q++)
					tex::print_char(tex::trick_buf[q % tex::ERROR_LINE]);
				if (m + n > tex::ERROR_LINE)
					tex::print("...");
				incr(nn);
				}
			} 
		else if (nn == error_context_lines) {
			tex::print_nl("...");
			incr(nn);
			}
		if (bottom_line)
			break;
		decr(base_ptr);
		}
	pop_input();
	}

tokeniser::tokeniser(symbols& s, fileio& f)
	: END_TEMPLATE_TOKEN(sym2tok(sym_.FROZEN_END_TEMPLATE())),
     sym_(s), fil_(f)
	{
	// FIXME: (memory) these are all stacks that can be stack<input>
	ninputs = STACK_SIZE;
	input_stack = (input *)malloc(ninputs*sizeof(input));
	if (input_stack == (input *) 0)
		throw tex::overflow_error("input stack", ninputs);
	input_end = input_stack + ninputs;
	
	nfiles = INFILE_SIZE;
	file_stack = (infile *)malloc(nfiles * sizeof(infile));
	if (file_stack == (infile *) 0)
		throw tex::overflow_error("infile stack", nfiles);
	file_end = file_stack + nfiles;
	
	nparams = PARAM_SIZE;
	param_stack = (ptr *)malloc(nparams * sizeof(ptr));
	if (param_stack == (ptr *) 0)
		throw tex::overflow_error("param stack", nparams);
	param_end = param_stack + nparams;
	match_toks = new_token();
	align_toks = new_token();
	omit_template = new_token();
	null_list = new_token();

	// After loading a dump:
	base_ptr = max_in_stack = input_ptr = input_stack;
	max_file_stack = file_ptr = file_stack;
	max_param_stack = param_ptr = param_stack;
	cur_input.state_field = NEW_LINE;
	index = 0;
	in_file = (ptr)file_ptr++;
	cur_file = stdin;
	name = file_name = "tty";
	line = file_line =  0;
	force_eof = false;
	open_parens = 0;
	scanner_status = NORMAL;
	align_state = 1000000;
	buffer = (byte *)new_str(BUF_SIZE);  // One global buffer for every file.
	next = buffer;
	limit = next - 1;
	token(omit_template) = END_TEMPLATE_TOKEN;
	token(null_list) = 0;
	}

bool tokeniser::end_line_char_active() 
	{ 
	return (end_line_char >= 0 && end_line_char <= 255); 
	};

