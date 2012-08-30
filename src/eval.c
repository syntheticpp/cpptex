
/*
 * $Id: eval.c,v 1.7 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002,2003      Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 *
 * 
 */

#include <iostream>
#include <stdexcept>
#include "tex.h"

#define NEST_SIZE	40
#define NEST_INC	20

int	       nlists;
fnt	main_f;
tex::qcell	main_i;
tex::qcell	main_j;
tex::qcell	*main_k;
ptr	main_p;
int	main_s;
ptr	main_t;

int	bchar;
int	false_bchar;
bool	cancel_boundary;
bool	ins_disc;

void tex::push_nest ()
	{
	if(nest_ptr > max_nest_stack) {
		max_nest_stack = nest_ptr;
		if (nest_ptr == nest_end && !realloc_nest())
			throw tex::overflow_error("semantic nest size", nlists);
		}
	*nest_ptr++ = cur_list;
	tail = head = new_avail();
	prev_graf = 0;
	mode_line = line;
	}

void tex::pop_nest ()
	{
	free_avail(head);
	cur_list = *--nest_ptr;
	}

bool tex::realloc_nest ()
	{
	list *tmp;
	
	nlists += NEST_INC;
	tmp = (list *)realloc(nest, nlists * sizeof(list));
	if (tmp == (list *) 0) {
		overflow("nest", nlists);
		}
	nest = tmp;
	nest_end = nest + nlists - 1;
	nest_ptr = nest + nlists - NEST_INC;
	return TRUE;
	}

void tex::print_mode(int m)
	{
	if (m > 0) {
		switch (m / (MAX_COMMAND + 1)) {
			case 0: print("vertical"); break;
			case 1: print("horizontal"); break;
			case 2: print("display math"); break;
			}
		} 
	else if (m == 0) {
		print("no");
		} 
	else {	
		switch (-m / (MAX_COMMAND + 1)) {
			case 0: print("internal vertical"); break;
			case 1: print("restricted horizontal"); break;
			case 2: print("math"); break;
			}
		}
	print(" mode");
	}

#define vmode(CMD) \
	case VMODE + CMD

#define hmode(CMD) \
	case HMODE + CMD

#define non_math(CMD) \
	case VMODE + CMD: \
	case HMODE + CMD

#define mmode(CMD) \
	case MMODE + CMD

#define any_mode(CMD) \
	case VMODE + CMD: \
	case HMODE + CMD: \
	case MMODE + CMD

void tex::main_control()
	{
	if (every_job != null)
		begin_token_list(every_job, EVERY_JOB_TEXT);
	
	big_switch:
	get_x_token();

   reswitch:
	if (tracing_commands > 0)
		show_cur_cmd_chr();
	
	switch(abs(mode) + cur_cmd) {
		hmode(LETTER):
		hmode(OTHER_CHAR):
		hmode(CHAR_GIVEN):
			goto main_loop;
		
		hmode(CHAR_NUM):
			scan_char_num();
		   cur_chr = cur_val;
		   goto main_loop;
		
		hmode(NO_BOUNDARY):
			get_x_token();
		   if (cur_cmd == LETTER || cur_cmd == OTHER_CHAR
				 || cur_cmd == CHAR_GIVEN || cur_cmd == CHAR_NUM) {
				cancel_boundary = TRUE;
				}
			goto reswitch;

		hmode(SPACER):
			if (space_factor == 1000) {
				goto append_normal_space;
				} 
			else {
				app_space();
				}
		   break;
	
	hmode(EX_SPACE):
	mmode(EX_SPACE):
		goto append_normal_space;
	
	any_mode(RELAX):
	vmode(SPACER):
	mmode(SPACER):
	mmode(NO_BOUNDARY):
		break;
	
	any_mode(IGNORE_SPACES):
		get_nbx_token();
		goto reswitch;

	vmode(STOP):
		if (its_all_over())
			return;
		break;

	any_mode(LAST_ITEM):
	any_mode(MAC_PARAM):
	non_math(EQ_NO):
	vmode(VMOVE):
	hmode(HMOVE):
	mmode(HMOVE):
	vmode(VADJUST):
	vmode(ITAL_CORR):
		report_illegal_case();
		break;

	non_math(SUP_MARK):
	non_math(SUB_MARK):
	non_math(MATH_CHAR_NUM):
	non_math(MATH_GIVEN):
	non_math(MATH_COMP):
	non_math(DELIM_NUM):
	non_math(LEFT_RIGHT):
	non_math(ABOVE):
	non_math(RADICAL):
	non_math(MATH_STYLE):
	non_math(MATH_CHOICE):
	non_math(VCENTER):
	non_math(NON_SCRIPT):
	non_math(MKERN):
	non_math(LIMIT_SWITCH):
	non_math(MSKIP):
	non_math(MATH_ACCENT):
	mmode(ENDV):
	mmode(PAR_END):
	mmode(STOP):
	mmode(VSKIP):
	mmode(UN_VBOX):
	mmode(VALIGN):
	mmode(HRULE):
		insert_dollar_sign();
		break;
	
	vmode(HRULE):
	hmode(VRULE):
	mmode(VRULE):
		tail_append(scan_rule_spec());
		if (abs(mode) == VMODE)
			prev_depth = IGNORE_DEPTH;
		else if (abs(mode) == HMODE)
			space_factor = 1000;
		break;
	
	vmode(VSKIP):
	hmode(HSKIP):
	mmode(HSKIP):
	mmode(MSKIP):
		append_glue();
		break;
	
	any_mode(KERN):
	mmode(MKERN):
		append_kern();
		break;
	
	non_math(LEFT_BRACE):
		new_save_level(SIMPLE_GROUP);
		break;

	any_mode(BEGIN_GROUP):
		new_save_level(SEMI_SIMPLE_GROUP);
		break;

	any_mode(END_GROUP):
		if (cur_group == SEMI_SIMPLE_GROUP) {
			unsave();
		} else {
			off_save();
		}
		break;
	
	any_mode(RIGHT_BRACE):
		handle_right_brace();
		break;

	vmode(HMOVE):
	hmode(VMOVE):
	mmode(VMOVE): {
		int	t;

		t = cur_chr;
		scan_normal_dimen();
		if (t == 0) {
			scan_box(cur_val);
		} else {
			scan_box(-cur_val);
		}
		break;
	}
	any_mode(LEADER_SHIP):
		scan_box(LEADER_FLAG + cur_chr - A_LEADERS);
		break;
	
	any_mode(MAKE_BOX):
		begin_box(0);
		break;

	vmode(START_PAR):
		new_graf(cur_chr > 0);
		break;
	
	vmode(LETTER):
	vmode(OTHER_CHAR):
	vmode(CHAR_NUM):
	vmode(CHAR_GIVEN):
	vmode(MATH_SHIFT):
	vmode(UN_HBOX):
	vmode(VRULE):
	vmode(ACCENT):
	vmode(DISCRETIONARY):
	vmode(HSKIP):
	vmode(VALIGN):
	vmode(EX_SPACE):
	vmode(NO_BOUNDARY):
		back_input();
		new_graf(TRUE);
		break;
	
	hmode(START_PAR):
	mmode(START_PAR):
		indent_in_hmode();
		break;
	
	vmode(PAR_END):
		normal_paragraph();
		if (mode > 0)
			build_page();
		break;

	hmode(PAR_END):
		if (align_state < 0)
			off_save();
		end_graf();
		if (mode == VMODE)	
			build_page();
		break;
	
	hmode(STOP):
	hmode(VSKIP):
	hmode(HRULE):
	hmode(UN_VBOX):
	hmode(HALIGN):
		head_for_vmode();
		break;
	
	any_mode(INSERT):
	hmode(VADJUST):
	mmode(VADJUST):
		begin_insert_or_adjust();
		break;
	
	any_mode(MARK):
		make_mark();
		break;
	
	any_mode(BREAK_PENALTY):
		append_penalty();
		break;
	
	any_mode(REMOVE_ITEM):
		delete_last();
		break;
	
	vmode(UN_VBOX):
	hmode(UN_HBOX):
	mmode(UN_HBOX):
		unpackage();
		break;
	
	hmode(ITAL_CORR):
		append_italic_correction();
		break;
	
	mmode(ITAL_CORR):
		tail_append(new_kern(0));
		break;

	hmode(DISCRETIONARY):
	mmode(DISCRETIONARY):
		append_discretionary();
		break;
	
	hmode(ACCENT):
		make_accent();
		break;
	
	any_mode(CAR_RET):
	any_mode(TAB_MARK):
		align_error();
		break;

	any_mode(NO_ALIGN):
		no_align_error();
		break;
	
	any_mode(OMIT):
		omit_error();
		break;

	vmode(HALIGN):
	hmode(VALIGN):
		init_align();
		break;
	
	mmode(HALIGN):
		if (privileged()) {
			if (cur_group == MATH_SHIFT_GROUP) {
				init_align();
			} else {
				off_save();
			}
		}
		break;

	vmode(ENDV):
	hmode(ENDV):
		do_endv();
		break;
	
	any_mode(END_CS_NAME):
		cs_error();
		break;

	hmode(MATH_SHIFT):
		init_math();
		break;
	
	mmode(EQ_NO):
		if (privileged()) {
			if (cur_group == MATH_SHIFT_GROUP) {
				start_eq_no();
			} else {
				off_save();
			}
		}
		break;
	
	mmode(LEFT_BRACE):
		tail_append(new_noad());
		back_input();
		scan_math(nucleus(tail));
		break;

	mmode(LETTER):
	mmode(OTHER_CHAR):
	mmode(CHAR_GIVEN):
		if (cur_chr < 256)
			set_math_char(math_code(cur_chr));
		else set_math_char(cur_chr);
		break;
	
	mmode(CHAR_NUM):
		scan_char_num();
		cur_chr = cur_val;
		if (cur_chr < 256)
			set_math_char(math_code(cur_chr));
		else set_math_char(cur_chr);
		break;
	
	mmode(MATH_CHAR_NUM):
		scan_fifteen_bit_int();
		set_math_char(cur_val);
		break;
	
	mmode(MATH_GIVEN):
		set_math_char(cur_chr);
		break;

	mmode(DELIM_NUM):
		scan_twenty_seven_bit_int();
		set_math_char(cur_val / 010000);
		break;
	
	mmode(MATH_COMP):
		tail_append(new_noad());
		type(tail) = cur_chr;
		scan_math(nucleus(tail));
		break;
	
	mmode(LIMIT_SWITCH):
		math_limit_switch();
		break;

	mmode(RADICAL):
		math_radical();
		break;

	mmode(ACCENT):
	mmode(MATH_ACCENT):
		math_ac();
		break;

	mmode(VCENTER):
		scan_spec(VCENTER_GROUP, FALSE);
		normal_paragraph();
		push_nest();
		mode = -VMODE;
		prev_depth = IGNORE_DEPTH;
		if (every_vbox != null)
			begin_token_list(every_vbox, EVERY_VBOX_TEXT);
		break;
	
	mmode(MATH_STYLE):
		tail_append(new_style(cur_chr));
		break;
	
	mmode(NON_SCRIPT):
		tail_append(new_glue(zero_glue));
		subtype(tail) = COND_MATH_GLUE;
		break;
	
	mmode(MATH_CHOICE):
		append_choices();
		break;

	mmode(SUB_MARK):
	mmode(SUP_MARK):
		sub_sup();
		break;
	
	mmode(ABOVE):
		math_fraction();
		break;
	
	mmode(LEFT_RIGHT):
		math_left_right();
		break;

	mmode(MATH_SHIFT):
		if (cur_group == MATH_SHIFT_GROUP) {
			after_math();
		} else {
			off_save();
		}
		break;

	any_mode(AFTER_ASSIGNMENT):
		get_token();
		after_token = cur_tok;
		break;
	
	any_mode(AFTER_GROUP):
		get_token();
		save_for_after(cur_tok);
		break;

	any_mode(IN_STREAM):
		clopen_stream();
		break;

	any_mode(MESSAGE):
		issue_message();
		break;
	
	any_mode(CASE_SHIFT):
		shift_case();
		break;
	
	any_mode(XRAY):
		show_whatever();
		break;
	
	any_mode(EXTENSION):
		do_extension();
		break;
	
	default:
		prefixed_command();
		break;
	}
	goto big_switch;

#define adjust_space_factor() \
{ \
	main_s = sf_code(cur_chr); \
	if (main_s == 1000) { \
		space_factor = 1000; \
	} else if (main_s < 1000) { \
		if (main_s > 0) { \
			space_factor = main_s; \
		} \
	} else if (space_factor < 1000) { \
		space_factor = 1000; \
	} else { \
		space_factor = main_s; \
	} \
}
		
main_loop:
	adjust_space_factor();
	main_f = cur_font;
	bchar = font_bchar(main_f);
	false_bchar = font_false_bchar(main_f);
	if (mode > 0 && language != clang)
		fix_language();
	fast_new_avail(lig_stack);
	font(lig_stack) = main_f;
	character(lig_stack) = cur_l = cur_chr;
	cur_q = tail;
	if (cancel_boundary) {
		cancel_boundary = FALSE;
		main_k = NON_ADDRESS;
	} else {
		main_k = bchar_label(main_f);
	}
	if (main_k == NON_ADDRESS)
		goto main_loop_move_2;
	cur_r = cur_l;
	cur_l = NON_CHAR;
	goto main_lig_loop_1;

#define pack_lig(RT_HIT) \
{ \
	main_p = new_ligature(main_f, cur_l, link(cur_q)); \
	if (lft_hit) { \
		subtype(main_p) = 2; \
		lft_hit = FALSE; \
	} \
	if (RT_HIT && lig_stack == null) { \
		incr(subtype(main_p)); \
		rt_hit = FALSE; \
	} \
	tail = link(cur_q) = main_p; \
	ligature_present = FALSE; \
}

#define wrapup(RT_HIT) \
{ \
	if (cur_l < NON_CHAR) { \
		if (character(tail) == hyphen_char(main_f) \
		&& link(cur_q) != null) \
			ins_disc = TRUE; \
		if (ligature_present) \
			pack_lig(RT_HIT); \
		if (ins_disc) { \
			ins_disc = FALSE; \
			if (mode > 0) \
				tail_append(new_disc()); \
		} \
	} \
}

main_loop_wrapup:
	wrapup(rt_hit);

main_loop_move:
	if (lig_stack == null)
		goto reswitch;
	cur_q = tail;
	cur_l = cur_r;

main_loop_move_1:
	if (!is_char_node(lig_stack))
		goto main_loop_move_lig;

main_loop_move_2:
	if (cur_chr < font_bc(main_f) || cur_chr > font_ec(main_f)) {
		char_warning(main_f, cur_chr);
		free_avail(lig_stack);
		goto big_switch;
	}
	main_i = char_info(main_f, cur_l);
	if (!char_exists(main_i)) {
		char_warning(main_f, cur_chr);
		free_avail(lig_stack);
		goto big_switch;
	}
	tail_append(lig_stack);

main_loop_lookahead:
	get_next();
	if (cur_cmd == LETTER
	|| cur_cmd == OTHER_CHAR
	|| cur_cmd == CHAR_GIVEN) {
		goto main_loop_lookahead_1;
	}
	x_token();
	if (cur_cmd == LETTER
	|| cur_cmd == OTHER_CHAR
	|| cur_cmd == CHAR_GIVEN) {
		goto main_loop_lookahead_1;
	}
	if (cur_cmd == CHAR_NUM) {
		scan_char_num();
		cur_chr = cur_val;
		goto main_loop_lookahead_1;
	}
	if (cur_cmd == NO_BOUNDARY) {
		bchar = NON_CHAR;
	}
	cur_r = bchar;
	lig_stack = null;
	goto main_lig_loop;

main_loop_lookahead_1:
	adjust_space_factor();
	fast_new_avail(lig_stack);
	font(lig_stack) = main_f;
	character(lig_stack) = cur_r = cur_chr;
	if (cur_r == false_bchar) {
		cur_r = NON_CHAR;
	}

main_lig_loop:
	if (char_tag(main_i) != LIG_TAG) {
		goto main_loop_wrapup;
	}
	main_k = lig_kern_start(main_f, main_i);
	main_j = *main_k;
	if (skip_byte(main_j) <= STOP_FLAG) {
		goto main_lig_loop_2;
	}
	main_k = lig_kern_restart(main_f, main_j);

main_lig_loop_1:
	main_j = *main_k;

main_lig_loop_2:
	if (next_char(main_j) == cur_r
	&& skip_byte(main_j) <= STOP_FLAG) {
		if (op_byte(main_j) >= KERN_FLAG) {
			wrapup(rt_hit);
			tail_append(new_kern(char_kern(main_f, main_j)));
			goto main_loop_move;
		}
		if (cur_l == NON_CHAR) {
			lft_hit = TRUE;
		} else if (lig_stack == null) {
			rt_hit = TRUE;
		}
		if(interrupted) throw std::logic_error("interrupted");
		switch (op_byte(main_j))
		{
		case 1: case 5:
			cur_l = rem_byte(main_j);
			main_i = char_info(main_f, cur_l);
			ligature_present = TRUE;
			break;
			
		case 2: case 6:
			cur_r = rem_byte(main_j);
			if (lig_stack == null) {
				lig_stack = new_lig_item(cur_r);
				bchar = NON_CHAR;
			} else if (is_char_node(lig_stack)) {
				main_p = lig_stack;
				lig_stack = new_lig_item(cur_r);
				lig_ptr(lig_stack) = main_p;
			} else {
				character(lig_stack) = cur_r;
			}
			break;

		case 3:
			cur_r = rem_byte(main_j);
			main_p = lig_stack;
			lig_stack = new_lig_item(cur_r);
			link(lig_stack) = main_p;
			break;

		case 7: case 11:
			wrapup(FALSE);
			cur_q = tail;
			cur_l = rem_byte(main_j);
			main_i = char_info(main_f, cur_l);
			ligature_present = TRUE;
			break;

		default:
			cur_l = rem_byte(main_j);
			ligature_present = TRUE;
			if (lig_stack == null) {
				goto main_loop_wrapup;
			} else {
				goto main_loop_move_1;
			}
			break;
		}
		if (op_byte(main_j) > 4 && op_byte(main_j) != 7)
			goto main_loop_wrapup;
		if (cur_l < NON_CHAR)
			goto main_lig_loop;
		main_k = bchar_label(main_f);
		goto main_lig_loop_1;
	}
	if (skip_byte(main_j) == 0) {
		incr(main_k);
	} else {
		if (skip_byte(main_j) >= STOP_FLAG)
			goto main_loop_wrapup;
		main_k += skip_byte(main_j) + 1;
	}
	goto main_lig_loop_1;

main_loop_move_lig:
	main_p = lig_ptr(lig_stack);
	if (main_p != null) {
		tail_append(main_p);
	}
	main_t = lig_stack;
	lig_stack = link(main_t);
	free_node(main_t, SMALL_NODE_SIZE);
	main_i = char_info(main_f, cur_l);
	ligature_present = TRUE;
	if (lig_stack == null) {
		if (main_p != null) {
			goto main_loop_lookahead;
		} else {
			cur_r = bchar;
		}
	} else {
		cur_r = character(lig_stack);
	}
	goto main_lig_loop;

append_normal_space:
	if (space_skip == zero_glue) {
		main_p = font_glue(cur_font);
		if (main_p == null)
			main_p = find_font_glue(cur_font);
		main_t = new_glue(main_p);
	} else {
		main_t = new_param_glue(SPACE_SKIP_CODE);
	}
	tail_append(main_t);
	goto big_switch;
}

void tex::app_space ()
	{
	ptr	p, q;
	
	if (space_factor >= 2000 && xspace_skip != zero_glue) {
		q = new_param_glue(XSPACE_SKIP_CODE);
		} 
	else {
		if (space_skip != zero_glue) {
			p = space_skip;
			} 
		else {
			p = find_font_glue(cur_font);
			}
		p = new_spec(p);
		if (space_factor >= 2000)
			glue_width(p) += extra_space(cur_font);
		stretch(p) = xn_over_d(stretch(p), space_factor, 1000);
		shrink(p) = xn_over_d(shrink(p), 1000L, space_factor);
		q = new_glue(p);
		glue_ref_count(p) = 0;
		}
	tail_append(q);
	}

void tex::insert_dollar_sign ()
	{
	back_input();
	cur_tok = MATH_SHIFT_TOKEN + '$';
	print_err("Missing $ inserted");
	help_dollar();
	ins_error();
	}

void tex::you_cant ()
	{
	print_err("You can't use `");
	print_cmd_chr(cur_cmd, cur_chr);
	print("' in ");
	print_mode(mode);
	}

void tex::report_illegal_case ()
	{
	you_cant();
	help_illegal_case();
	error();
	}

bool tex::privileged ()
	{
	if (mode > 0)
		return TRUE;
	report_illegal_case();
	return FALSE;
	}

bool tex::its_all_over ()
	{
	if (privileged()) {
		if (page_head == page_tail
			 && head == tail
			 && dead_cycles == 0) {
			return TRUE;
			}
		back_input();
		tail_append(new_null_box());
		box_width(tail) = hsize;
		tail_append(new_glue(fill_glue));
		tail_append(new_penalty(-010000000000));
		build_page();
		}
	return FALSE;
	}

void tex::show_activities ()
	{
	int	a;
	int	m;
	list	*p;

	*nest_ptr = cur_list;
	print_nl(null_str);
	print_ln();
	for (p = nest_ptr; p >= nest; decr(p)) {
		m = p->mode_field;
		a = p->aux_field;
		print_nl("### ");
		print_mode(m);
		print(" entered at line ");
		print_int(abs(p->ml_field));
		if (m == HMODE) {
			if (p->lhm_field != 2 || p->rhm_field != 3) {
				print(" (hyphenmin ");
				print_int(p->lhm_field);
				print(",");
				print_int(p->rhm_field);
				print(")");
			}
		}
		if (p->ml_field < 0) {
			print(" (\\output routine)");
		}
		if (p == nest) {
			show_cur_page();
			if (link(contrib_head) != null)
				print_nl("### recent contributions:");
		}
		show_box(link(p->head_field));
		switch (abs(m) / (MAX_COMMAND + 1))
		{
		case 0:
			print_nl("prevdepth ");
			if (a <= IGNORE_DEPTH) {
				print("ignored");
			} else {
				print_scaled(a);
			}
			if (p->pg_field != 0) {
				print(", prevgraf ");
				print_int(p->pg_field);
				print(" line");
				if (p->pg_field != 1)
					print("s");
			}
			break;

		case 1:
			print_nl("spacefactor ");
			print_int(a);
			if (m > 0 && p->clang_field > 0) {
				print(", current language ");
				print_int(p->clang_field);
			}
			break;

		case 2:
			if (a != null) {
				print_nl("this will be denominator of:");
				show_box(a);
			}
			break;
		}
	}
}

void tex::_eval_init ()
	{
	head = tail = contrib_head;
	mode = VMODE;
	prev_depth = IGNORE_DEPTH;
	mode_line = 0;
	prev_graf = 0;
	shown_mode = 0;
	lhmin = 0;
	rhmin = 0;
	ligature_present = FALSE;
	cancel_boundary = FALSE;
	ins_disc = FALSE;
	lft_hit = FALSE;
	rt_hit = FALSE;
	max_nest_stack = nest_ptr = nest;
	}

void tex::_eval_init_once ()
	{
	nlists = NEST_SIZE;
	nest = (list *)malloc(nlists * sizeof(list));
	if (nest == (list *) 0)
		overflow("nest", nlists);
	nest_end = nest + nlists - 1;
	}

/*
** Help text
*/

void tex::help_dollar()
	{
	help2("I've inserted a begin-math/end-math symbol since I think",
	"you left one out. Proceed, with fingers crossed.");
	}

void tex::help_illegal_case()
	{
	help4("Sorry, but I'm not programmed to handle this case;",
	"I'll just pretend that you didn't ask for it.",
	"If you're in the wrong mode, you might be able to",
	"return to the right one by typing `I}' or `I$' or `I\\par'.");
	}
