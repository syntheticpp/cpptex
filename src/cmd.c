
/*
 * Common TeX:cmd.c:3.2:EPA
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 *
 * 
 */

#include "cmd.h"

void tex::show_cur_cmd_chr()
	{
	begin_diagnostic();
	print_nl("{");
	if (mode != shown_mode) {
		print_mode(mode);
		print(": ");
		shown_mode = mode;
	}
	print_cmd_chr(cur_cmd, cur_chr);
	print("}");
	end_diagnostic(FALSE);
	}

void tex::print_cmd_chr(int cmd, int chr)
	{

#define chr_cmd(S) {print(S); print_ASCII(chr);}

	switch (cmd) {
	case RELAX:
		print_esc("relax");
		break;

	case LEFT_BRACE:
		chr_cmd("begin-group character ");
		break;

	case RIGHT_BRACE:
		chr_cmd("end-group character ");
		break;

	case MATH_SHIFT:
		chr_cmd("math shift character ");
		break;

	case TAB_MARK:
		if (chr == SPAN_CODE)
			print_esc("span");
		else chr_cmd("alignment tab character ");
		break;

	case CAR_RET:
		if (chr == CR_CODE)
			print_esc("cr");
		else print_esc("crcr");
		break;

	case MAC_PARAM:
		chr_cmd("macro parameter character ");
		break;

	case SUP_MARK:
		chr_cmd("superscript character ");
		break;

	case SUB_MARK:
		chr_cmd("subscript character ");
		break;

	case ENDV:
		print("end of alignment template");
		break;

	case SPACER:
		chr_cmd("blank space ");
		break;

	case LETTER:
		chr_cmd("the letter ");
		break;

	case OTHER_CHAR:
		chr_cmd("the character ");
		break;

	case ASSIGN_GLUE:
		if (chr < GLUE_PARS) {
			print_skip_param(chr);
		} else {
			print_esc("skip");
			print_int(chr - GLUE_PARS);
		}
		break;

	case ASSIGN_MU_GLUE:
		if (chr < MU_GLUE_PARS) {
			print_mu_skip_param(chr);
		} else {
			print_esc("muskip");
			print_int(chr - MU_GLUE_PARS);
		}
		break;

	case ASSIGN_TOKS:
		if (chr < TOK_PARS) {
			print_toks_param(chr);
		} else {
			print_esc("toks");
			print_int(chr - TOK_PARS);
		}
		break;

	case ASSIGN_INT:
		if (chr < INT_PARS) {
			print_param(chr);
		} else {
			print_esc("count");
			print_int(chr - INT_PARS);
		} 
		break;

	case ASSIGN_DIMEN:
		if (chr < DIMEN_PARS) {
			print_length_param(chr);
		} else {
			print_esc("dimen");
			print_int(chr - DIMEN_PARS);
		} 
		break;

	case ACCENT:
		print_esc("accent");
		break;

	case ADVANCE:
		print_esc("advance");
		break;

	case AFTER_ASSIGNMENT:
		print_esc("afterassignment");
		break;

	case AFTER_GROUP:
		print_esc("aftergroup");
		break;

	case ASSIGN_FONT_DIMEN:
		print_esc("fontdimen");
		break;

	case BEGIN_GROUP:
		print_esc("begingroup");
		break;

	case BREAK_PENALTY:
		print_esc("penalty");
		break;

	case CHAR_NUM:
		print_esc("char");
		break;

	case CS_NAME:
		print_esc("csname");
		break;

	case DEF_FONT:
		print_esc("font");
		break;

	case DELIM_NUM:
		print_esc("delimiter");
		break;

	case DIVIDE:
		print_esc("divide");
		break;

	case END_CS_NAME:
		print_esc("endcsname");
		break;

	case END_GROUP:
		print_esc("endgroup");
		break;

	case EX_SPACE:
		print_esc(" ");
		break;

	case EXPAND_AFTER:
		print_esc("expandafter");
		break;

	case INPUT:
		if (chr == 0)
			print_esc("input");
		else print_esc("endinput");
		break;

	case HALIGN:
		print_esc("halign");
		break;

	case HRULE:
		print_esc("hrule");
		break;

	case IGNORE_SPACES:
		print_esc("ignorespaces");
		break;

	case INSERT:
		print_esc("insert");
		break;

	case ITAL_CORR:
		print_esc("/");
		break;

	case MARK:
		print_esc("mark");
		break;

	case TOP_BOT_MARK:
		switch (chr) {
		case TOP_MARK_CODE: print_esc("topmark");break; 
		case FIRST_MARK_CODE: print_esc("firstmark");break; 
		case BOT_MARK_CODE: print_esc("botmark");break; 
		case SPLIT_FIRST_MARK_CODE: print_esc("splitfirstmark");break; 
		case SPLIT_BOT_MARK_CODE: print_esc("splitbotmark");break;
		}
		break;

	case MATH_ACCENT:
		print_esc("mathaccent");
		break;

	case MATH_CHAR_NUM:
		print_esc("mathchar");
		break;

	case MATH_CHOICE:
		print_esc("mathchoice");
		break;

	case MULTIPLY:
		print_esc("multiply");
		break;

	case NO_ALIGN:
		print_esc("noalign");
		break;

	case NO_BOUNDARY:
		print_esc("noboundary");
		break;

	case NO_EXPAND:
		print_esc("noexpand");
		break;

	case NON_SCRIPT:
		print_esc("nonscript");
		break;

	case OMIT:
		print_esc("omit");
		break;

	case RADICAL:
		print_esc("radical");
		break;

	case READ_TO_CS:
		print_esc("read");
		break;

	case SET_BOX:
		print_esc("setbox");
		break;

	case SET_PREV_GRAF:
		print_esc("prevgraf");
		break;

	case SET_SHAPE:
		print_esc("parshape");
		break;

	case THE:
		print_esc("the");
		break;

	case TOKS_REGISTER:
		print_esc("toks");
		break;

	case VADJUST:
		print_esc("vadjust");
		break;

	case VALIGN:
		print_esc("valign");
		break;

	case VCENTER:
		print_esc("vcenter");
		break;

	case VRULE:
		print_esc("vrule");
		break;

	case PAR_END:
		print_esc("par");
		break;

	case SET_AUX:
		if (chr == VMODE)
			print_esc("prevdepth");
		else print_esc("spacefactor");
		break;

	case SET_PAGE_INT:
		if (chr == 0)
			print_esc("deadcycles");
		else print_esc("insertpenalties");
		break;

	case SET_BOX_DIMEN:
		if (chr == WD_CODE)
			print_esc("wd");
		else if (chr == HT_CODE)
			print_esc("ht");
		else print_esc("dp");
		break;

	case SET_PAGE_DIMEN:
		switch (chr) {
		case 0: print_esc("pagegoal");break; 
		case 1: print_esc("pagetotal");break; 
		case 2: print_esc("pagestretch");break; 
		case 3: print_esc("pagefilstretch");break; 
		case 4: print_esc("pagefillstretch");break; 
		case 5: print_esc("pagefilllstretch");break; 
		case 6: print_esc("pageshrink");break;
		}
		break;

	case LAST_ITEM:
		switch (chr) {
		case INT_VAL: print_esc("lastpenalty");break;
		case DIMEN_VAL: print_esc("lastkern");break;
		case GLUE_VAL: print_esc("lastskip");break;
		case INPUT_LINE_NO_CODE: print_esc("inputlineno");break;
		default: print_esc("badness");break;
		}
		break;

	case REGISTER:
		if (chr == INT_VAL)
			print_esc("count");
		else if (chr == DIMEN_VAL)
			print_esc("dimen");
		else if (chr == GLUE_VAL)
			print_esc("skip");
		else print_esc("muskip");
		break;

	case CONVERT:
		switch (chr) {
		case NUMBER_CODE: print_esc("number");break; 
		case ROMAN_NUMERAL_CODE: print_esc("romannumeral");break; 
		case STRING_CODE: print_esc("string");break; 
		case MEANING_CODE: print_esc("meaning");break; 
		case FONT_NAME_CODE: print_esc("fontname");break; 
		default: print_esc("jobname");break;
		}
		break;

	case IF_TEST:
		switch (chr) {
		case IF_CHAR_CODE: print_esc("if");break; 
		case IF_CAT_CODE: print_esc("ifcat");break; 
		case IF_INT_CODE: print_esc("ifnum");break; 
		case IF_DIM_CODE: print_esc("ifdim");break; 
		case IF_ODD_CODE: print_esc("ifodd");break; 
		case IF_VMODE_CODE: print_esc("ifvmode");break; 
		case IF_HMODE_CODE: print_esc("ifhmode");break; 
		case IF_MMODE_CODE: print_esc("ifmmode");break; 
		case IF_INNER_CODE: print_esc("ifinner");break; 
		case IF_VOID_CODE: print_esc("ifvoid");break; 
		case IF_HBOX_CODE: print_esc("ifhbox");break; 
		case IF_VBOX_CODE: print_esc("ifvbox");break; 
		case IFX_CODE: print_esc("ifx");break; 
		case IF_EOF_CODE: print_esc("ifeof");break; 
		case IF_TRUE_CODE: print_esc("iftrue");break; 
		case IF_FALSE_CODE: print_esc("iffalse");break; 
		case IF_CASE_CODE: print_esc("ifcase");break;
		}
		break;

	case FI_OR_ELSE:
		if (chr == FI_CODE)
			print_esc("fi");
		else if (chr == OR_CODE)
			print_esc("or");
		else print_esc("else");
		break;

	case PREFIX:
		if (chr == 1)
			print_esc("long");
		else if (chr == 2)
			print_esc("outer");
		else print_esc("global");
		break;

	case DEF:
		if (chr == 0)
			print_esc("def");
		else if (chr == 1)
			print_esc("gdef");
		else if (chr == 2)
			print_esc("edef");
		else print_esc("xdef");
		break;

	case LET:
		if (chr != NORMAL)
			print_esc("futurelet");
		else print_esc("let");
		break;

	case SHORTHAND_DEF:
		switch (chr) {
		case CHAR_DEF_CODE: print_esc("chardef");break; 
		case MATH_CHAR_DEF_CODE: print_esc("mathchardef");break; 
		case COUNT_DEF_CODE: print_esc("countdef");break; 
		case DIMEN_DEF_CODE: print_esc("dimendef");break; 
		case SKIP_DEF_CODE: print_esc("skipdef");break; 
		case MU_SKIP_DEF_CODE: print_esc("muskipdef");break; 
		default: print_esc("toksdef");break;
		}
		break;

	case CHAR_GIVEN:
		print_esc("char");
		print_hex(chr);
		break;

	case MATH_GIVEN:
		print_esc("mathchar");
		print_hex(chr);
		break;

	case DEF_CODE:
		if (chr == CAT_CODE)
			print_esc("catcode");
		else if (chr == MATH_CODE)
			print_esc("mathcode");
		else if (chr == LC_CODE)
			print_esc("lccode");
		else if (chr == UC_CODE)
			print_esc("uccode");
		else if (chr == SF_CODE)
			print_esc("sfcode");
		else print_esc("delcode");
		break;

	case DEF_FAMILY:
		print_size(chr);
		break;

	case SET_FONT:
		print("select font ");
		print(font_name(chr));
		if (font_size(chr) != font_dsize(chr)) {
			print(" at ");
			print_scaled(font_size(chr));
			print("pt");
		} 
		break;

	case ASSIGN_FONT_INT:
		if (chr == 1)
			print_esc("skewchar");
		else print_esc("hyphenchar");
		break;

	case HYPH_DATA:
		if (chr == 1)
			print_esc("patterns");
		else print_esc("hyphenation");
		break;

	case SET_INTERACTION:
		switch (chr) {
		case BATCH_MODE: print_esc("batchmode");break; 
		case NONSTOP_MODE: print_esc("nonstop");break; 
		case SCROLL_MODE: print_esc("scrollmode");break; 
		default: print_esc("errorstopmode");break;
		}
		break;

	case IN_STREAM:
		if (chr == 0)
			print_esc("closein");
		else print_esc("openin");
		break;

	case MESSAGE:
		if (chr == 0)
			print_esc("message");
		else print_esc("errmessage");
		break;

	case CASE_SHIFT:
		if (chr == LC_CODE)	
			print_esc("lowercase");
		else print_esc("uppercase");
		break;

	case XRAY:
		switch (chr) {
		case SHOW_BOX_CODE: print_esc("showbox");break; 
		case SHOW_THE_CODE: print_esc("showthe");break; 
		case SHOW_LISTS: print_esc("showlists");break; 
		default: print_esc("show");break;
		}
		break;

	case UNDEFINED_CS:
		print("undefined");
		break;

	case CALL:
		print("macro");
		break;

	case LONG_CALL:
		print_esc("long macro");
		break;

	case OUTER_CALL:
		print_esc("outer macro");
		break;

	case LONG_OUTER_CALL:
		print_esc("long");
		print_esc("outer macro");
		break;

	case END_TEMPLATE:
		print_esc("outer endtemplate");
		break;

	case STOP:
		if (chr == 1)
			print_esc("dump");
		else print_esc("end");
		break;

	case HSKIP:
		switch (chr) {
		case SKIP_CODE: print_esc("hskip");break; 
		case FIL_CODE: print_esc("hfil");break; 
		case FILL_CODE: print_esc("hfill");break; 
		case SS_CODE: print_esc("hss");break; 
		default: print_esc("hfilneg");break;
		}
		break;

	case VSKIP:
		switch (chr) {
		case SKIP_CODE: print_esc("vskip");break; 
		case FIL_CODE: print_esc("vfil");break; 
		case FILL_CODE: print_esc("vfill");break; 
		case SS_CODE: print_esc("vss");break; 
		default: print_esc("vfilneg");break;
		}
		break;

	case MSKIP:
		print_esc("mskip");
		break;

	case KERN:
		print_esc("kern");
		break;

	case MKERN:
		print_esc("mkern");
		break;

	case HMOVE:
		if (chr == 1)
			print_esc("moveleft");
		else print_esc("moveright");
		break;

	case VMOVE:
		if (chr == 1)
			print_esc("raise");
		else print_esc("lower");
		break;

	case MAKE_BOX:
		switch (chr) {
		case BOX_CODE: print_esc("box");break; 
		case COPY_CODE: print_esc("copy");break; 
		case LAST_BOX_CODE: print_esc("lastbox");break; 
		case VSPLIT_CODE: print_esc("vsplit");break; 
		case VTOP_CODE: print_esc("vtop");break; 
		case VTOP_CODE + VMODE: print_esc("vbox");break; 
		default: print_esc("hbox");break;
		} 
		break;

	case LEADER_SHIP:
		if (chr == A_LEADERS)
			print_esc("leaders");
		else if (chr == C_LEADERS)
			print_esc("cleaders");
		else if (chr == X_LEADERS)
			print_esc("xleaders");
		else print_esc("shipout");
		break;

	case START_PAR:
		if (chr == 0)
			print_esc("noindent");
		else print_esc("indent");
		break;

	case REMOVE_ITEM:
		if (chr == GLUE_NODE)
			print_esc("unskip");
		else if (chr == KERN_NODE)
			print_esc("unkern");
		else print_esc("unpenalty");
		break;

	case UN_HBOX:
		if (chr == COPY_CODE)
			print_esc("unhcopy");
		else print_esc("unhbox");
		break;

	case UN_VBOX:
		if (chr == COPY_CODE)
			print_esc("unvcopy");
		else print_esc("unvbox");
		break;

	case DISCRETIONARY:
		if (chr == 1)
			print_esc("-"); 
		else print_esc("discretionary");
		break;

	case EQ_NO:
		if (chr == 1)
			print_esc("leqno");
		else print_esc("eqno");
		break;

	case MATH_COMP:
		switch (chr) {
		case ORD_NOAD: print_esc("mathord");break; 
		case OP_NOAD: print_esc("mathop");break; 
		case BIN_NOAD: print_esc("mathbin");break; 
		case REL_NOAD: print_esc("mathrel");break; 
		case OPEN_NOAD: print_esc("mathopen");break; 
		case CLOSE_NOAD: print_esc("mathclose");break; 
		case PUNCT_NOAD: print_esc("mathpunct");break; 
		case INNER_NOAD: print_esc("mathinner");break; 
		case UNDER_NOAD: print_esc("underline");break; 
		default: print_esc("overline");break;
		}
		break;

	case LIMIT_SWITCH:
		if (chr == LIMITS)
			print_esc("limits");
		else if (chr == NO_LIMITS)
			print_esc("nolimits");
		else print_esc("displaylimits");
		break;

	case MATH_STYLE:
		print_style(chr);
		break;

	case ABOVE:
		switch (chr) {
		case OVER_CODE:
			print_esc("over");
			break;

		case ATOP_CODE:
			print_esc("atop");
			break;

		case DELIMITED_CODE + ABOVE_CODE:
			print_esc("abovewithdelims");
			break;

		case DELIMITED_CODE + OVER_CODE:
			print_esc("overwithdelims");
			break;

		case DELIMITED_CODE + ATOP_CODE:
			print_esc("atopwithdelims");
			break;

		default:
			print_esc("above");
			break;
		}
		break;

	case LEFT_RIGHT:
		if (chr == LEFT_NOAD)
			print_esc("left");
		else print_esc("right");
		break;

	case EXTENSION:
		switch (chr) {
		case OPEN_NODE: print_esc("openout");break; 
		case WRITE_NODE: print_esc("write");break; 
		case CLOSE_NODE: print_esc("closeout");break; 
		case SPECIAL_NODE: print_esc("special");break; 
		case IMMEDIATE_CODE: print_esc("immediate");break;
		case SET_LANGUAGE_CODE: print_esc("setlanguage");break;
		}
		break;
	}
}

void tex::print_mu_skip_param(int n)
	{
	switch(n) {
		case THIN_MU_SKIP_CODE:
			print_esc("thinmuskip");
			break;
			
		case MED_MU_SKIP_CODE:
			print_esc("medmuskip");
			break;
			
		case THICK_MU_SKIP_CODE:
			print_esc("thickmuskip");
			break;
			
		default:
			print("[unknown muglue parameter!]");
			break;
		}
	}

void tex::print_skip_param(int n)
	{
	switch (n) {
		case LINE_SKIP_CODE:
			print_esc("lineskip");
			break;
			
		case BASELINE_SKIP_CODE:
			print_esc("baselineskip");
			break;
			
		case PAR_SKIP_CODE:
			print_esc("parskip");
			break;
		
		case ABOVE_DISPLAY_SKIP_CODE:
			print_esc("abovedisplayskip");
			break;
			
		case BELOW_DISPLAY_SKIP_CODE:
			print_esc("belowdisplayskip");
			break;
			
		case ABOVE_DISPLAY_SHORT_SKIP_CODE:
			print_esc("abovedisplayshortskip");
			break;
			
		case BELOW_DISPLAY_SHORT_SKIP_CODE:
			print_esc("belowdisplayshortskip");
			break;
			
		case LEFT_SKIP_CODE:
			print_esc("leftskip");
			break;
		
		case RIGHT_SKIP_CODE:
			print_esc("rightskip");
			break;
			
		case TOP_SKIP_CODE:
			print_esc("topskip");
			break;
			
		case SPLIT_TOP_SKIP_CODE:
			print_esc("splittopskip");
			break;
			
		case TAB_SKIP_CODE:
			print_esc("tabskip");
			break;
			
		case SPACE_SKIP_CODE:
			print_esc("spaceskip");
			break;
			
		case XSPACE_SKIP_CODE:
			print_esc("xspaceskip");
			break;
			
		case PAR_FILL_SKIP_CODE:
			print_esc("parfillskip");
			break;
			
		default:
			print("[unknown glue parameter!]");
			break;
		}
	}

void tex::print_length_param(int n)
	{
	switch (n) {
	case PAR_INDENT_CODE:
		print_esc("parindent");
		break;

	case MATH_SURROUND_CODE:
		print_esc("mathsurround");
		break;

	case LINE_SKIP_LIMIT_CODE:
		print_esc("lineskiplimit");
		break;

	case HSIZE_CODE:
		print_esc("hsize");
		break;

	case VSIZE_CODE:
		print_esc("vsize");
		break;

	case MAX_DEPTH_CODE:
		print_esc("maxdepth");
		break;

	case SPLIT_MAX_DEPTH_CODE:
		print_esc("splitmaxdepth");
		break;

	case BOX_MAX_DEPTH_CODE:
		print_esc("boxmaxdepth");
		break;

	case HFUZZ_CODE:
		print_esc("hfuzz");
		break;

	case VFUZZ_CODE:
		print_esc("vfuzz");
		break;

	case DELIMITER_SHORTFALL_CODE:
		print_esc("delimitershortfall");
		break;

	case NULL_DELIMITER_SPACE_CODE:
		print_esc("nulldelimiterspace");
		break;

	case SCRIPT_SPACE_CODE:
		print_esc("scriptspace");
		break;

	case PRE_DISPLAY_SIZE_CODE:
		print_esc("predisplaysize");
		break;

	case DISPLAY_INDENT_CODE:
		print_esc("displayindent");
		break;

	case DISPLAY_WIDTH_CODE:
		print_esc("displaywidth");
		break;

	case OVERFULL_RULE_CODE:
		print_esc("overfullrule");
		break;

	case HANG_INDENT_CODE:
		print_esc("hangindent");
		break;

	case H_OFFSET_CODE:
		print_esc("hoffset");
		break;

	case V_OFFSET_CODE:
		print_esc("voffset");
		break;

	case EMERGENCY_STRETCH_CODE:
		print_esc("emergency_stretch");
		break;

	default:
		print("[unknown dimen parameter!]");
		break;
	}
	}

void tex::print_param(int n)
	{
	switch (n)
	{
	case PRETOLERANCE_CODE:
		print_esc("pretolerance");
		break;

	case TOLERANCE_CODE:
		print_esc("tolerance");
		break;

	case LINE_PENALTY_CODE:
		print_esc("linepenalty");
		break;

	case HYPHEN_PENALTY_CODE:
		print_esc("hyphenpenalty");
		break;

	case EX_HYPHEN_PENALTY_CODE:
		print_esc("exhyphenpenalty");
		break;

	case CLUB_PENALTY_CODE:
		print_esc("clubpenalty");
		break;

	case WIDOW_PENALTY_CODE:
		print_esc("widowpenalty");
		break;

	case DISPLAY_WIDOW_PENALTY_CODE:
		print_esc("displaywidowpenalty");
		break;

	case BROKEN_PENALTY_CODE:
		print_esc("brokenpenalty");
		break;

	case BIN_OP_PENALTY_CODE:
		print_esc("binoppenalty");
		break;

	case REL_PENALTY_CODE:
		print_esc("relpenalty");
		break;

	case PRE_DISPLAY_PENALTY_CODE:
		print_esc("predisplaypenalty");
		break;

	case POST_DISPLAY_PENALTY_CODE:
		print_esc("postdisplaypenalty");
		break;

	case INTER_LINE_PENALTY_CODE:
		print_esc("interlinepenalty");
		break;

	case DOUBLE_HYPHEN_DEMERITS_CODE:
		print_esc("doublehyphendemerits");
		break;

	case FINAL_HYPHEN_DEMERITS_CODE:
		print_esc("finalhyphendemerits");
		break;

	case ADJ_DEMERITS_CODE:
		print_esc("adjdemerits");
		break;

	case MAG_CODE:
		print_esc("mag");
		break;

	case DELIMITER_FACTOR_CODE:
		print_esc("delimiterfactor");
		break;

	case LOOSENESS_CODE:
		print_esc("looseness");
		break;

	case TIME_CODE:
		print_esc("time");
		break;

	case DAY_CODE:
		print_esc("day");
		break;

	case MONTH_CODE:
		print_esc("month");
		break;

	case YEAR_CODE:
		print_esc("year");
		break;

	case SHOW_BOX_BREADTH_CODE:
		print_esc("showboxbreadth");
		break;

	case SHOW_BOX_DEPTH_CODE:
		print_esc("showboxdepth");
		break;

	case HBADNESS_CODE:
		print_esc("hbadness");
		break;

	case VBADNESS_CODE:
		print_esc("vbadness");
		break;

	case PAUSING_CODE:
		print_esc("pause");
		break;

	case TRACING_ONLINE_CODE:
		print_esc("tracingonline");
		break;

	case TRACING_MACROS_CODE:
		print_esc("tracingmacros");
		break;

	case TRACING_STATS_CODE:
		print_esc("tracingstats");
		break;

	case TRACING_PARAGRAPHS_CODE:
		print_esc("tracingparagraphs");
		break;

	case TRACING_PAGES_CODE:
		print_esc("tracingpages");
		break;

	case TRACING_OUTPUT_CODE:
		print_esc("tracingoutput");
		break;

	case TRACING_LOST_CHARS_CODE:
		print_esc("tracinglostchars");
		break;

	case TRACING_COMMANDS_CODE:
		print_esc("tracingcommands");
		break;

	case TRACING_RESTORES_CODE:
		print_esc("tracingrestores");
		break;

	case UC_HYPH_CODE:
		print_esc("uchyph");
		break;

	case OUTPUT_PENALTY_CODE:
		print_esc("outputpenalty");
		break;

	case MAX_DEAD_CYCLES_CODE:
		print_esc("maxdeadcycles");
		break;

	case HANG_AFTER_CODE:
		print_esc("hangafter");
		break;

	case FLOATING_PENALTY_CODE:
		print_esc("floatingpenalty");
		break;

	case GLOBAL_DEFS_CODE:
		print_esc("globaldefs");
		break;

	case CUR_FAM_CODE:
		print_esc("fam");
		break;

	case ESCAPE_CHAR_CODE:
		print_esc("escapechar");
		break;

	case DEFAULT_HYPHEN_CHAR_CODE:
		print_esc("defaulthyphenchar");
		break;

	case DEFAULT_SKEW_CHAR_CODE:
		print_esc("defaultskewchar");
		break;

	case END_LINE_CHAR_CODE:
		print_esc("endlinechar");
		break;

	case NEW_LINE_CHAR_CODE:
		print_esc("newlinechar");
		break;
	
	case LANGUAGE_CODE:
		print_esc("language");
		break;

	case LEFT_HYPHEN_MIN_CODE:
		print_esc("lefthyphenmin");
		break;
	
	case RIGHT_HYPHEN_MIN_CODE:
		print_esc("righthyphenmin");
		break;

	case HOLDING_INSERTS_CODE:
		print_esc("holdinginserts");
		break;

	case ERROR_CONTEXT_LINES_CODE:
		print_esc("errorcontextlines");
		break;

	default:
		print("[unknown integer parameter!]");
		break;
	}
}

void tex::print_toks_param(int n)
	{
	switch (n)
	{
	case OUTPUT_ROUTINE_CODE:
		print_esc("output");
		break;

	case EVERY_PAR_CODE:
		print_esc("everypar");
		break;

	case EVERY_MATH_CODE:
		print_esc("everymath");
		break;

	case EVERY_DISPLAY_CODE:
		print_esc("everydisplay");
		break;

	case EVERY_HBOX_CODE:
		print_esc("everyhbox");
		break;

	case EVERY_VBOX_CODE:
		print_esc("everyvbox");
		break;

	case EVERY_JOB_CODE:
		print_esc("everyjob");
		break;

	case EVERY_CR_CODE:
		print_esc("everycr");
		break;

	case ERR_HELP_CODE:
		print_esc("errhelp");
		break;

	default:
		print("[unknown toks parameter!]");
		break;
	}
}

void tex::_cmd_init ()
	{
	no_new_control_sequence = TRUE;
	}

void commands::commands()
	{
	no_new_control_sequence = FALSE;
	primitive("thinmuskip", ASSIGN_MU_GLUE, THIN_MU_SKIP_CODE);
	primitive("medmuskip", ASSIGN_MU_GLUE, MED_MU_SKIP_CODE);
	primitive("thickmuskip", ASSIGN_MU_GLUE, THICK_MU_SKIP_CODE);
	primitive("lineskip", ASSIGN_GLUE, LINE_SKIP_CODE);
	primitive("baselineskip", ASSIGN_GLUE, BASELINE_SKIP_CODE);
	primitive("parskip", ASSIGN_GLUE, PAR_SKIP_CODE);
	primitive("abovedisplayskip", ASSIGN_GLUE, ABOVE_DISPLAY_SKIP_CODE);
	primitive("abovedisplayshortskip", ASSIGN_GLUE, ABOVE_DISPLAY_SHORT_SKIP_CODE);
	primitive("belowdisplayskip", ASSIGN_GLUE, BELOW_DISPLAY_SKIP_CODE);
	primitive("belowdisplayshortskip", ASSIGN_GLUE, BELOW_DISPLAY_SHORT_SKIP_CODE);
	primitive("parfillskip", ASSIGN_GLUE, PAR_FILL_SKIP_CODE);
	primitive("leftskip", ASSIGN_GLUE, LEFT_SKIP_CODE);
	primitive("rightskip", ASSIGN_GLUE, RIGHT_SKIP_CODE);
	primitive("topskip", ASSIGN_GLUE, TOP_SKIP_CODE);
	primitive("splittopskip", ASSIGN_GLUE, SPLIT_TOP_SKIP_CODE);
	primitive("tabskip", ASSIGN_GLUE, TAB_SKIP_CODE);
	primitive("spaceskip", ASSIGN_GLUE, SPACE_SKIP_CODE);
	primitive("xspaceskip", ASSIGN_GLUE, XSPACE_SKIP_CODE);
	primitive("output", ASSIGN_TOKS, OUTPUT_ROUTINE_CODE);
	primitive("everycr", ASSIGN_TOKS, EVERY_CR_CODE);
	primitive("everypar", ASSIGN_TOKS, EVERY_PAR_CODE);
	primitive("everymath", ASSIGN_TOKS, EVERY_MATH_CODE);
	primitive("everydisplay", ASSIGN_TOKS, EVERY_DISPLAY_CODE);
	primitive("everyhbox", ASSIGN_TOKS, EVERY_HBOX_CODE);
	primitive("everyvbox", ASSIGN_TOKS, EVERY_VBOX_CODE);
	primitive("everyjob", ASSIGN_TOKS, EVERY_JOB_CODE);
	primitive("errhelp", ASSIGN_TOKS, ERR_HELP_CODE);
	primitive("pretolerance", ASSIGN_INT, PRETOLERANCE_CODE);
	primitive("tolerance", ASSIGN_INT, TOLERANCE_CODE);
	primitive("linepenalty", ASSIGN_INT, LINE_PENALTY_CODE);
	primitive("hyphenpenalty", ASSIGN_INT, HYPHEN_PENALTY_CODE);
	primitive("exhyphenpenalty", ASSIGN_INT, EX_HYPHEN_PENALTY_CODE);
	primitive("clubpenalty", ASSIGN_INT, CLUB_PENALTY_CODE);
	primitive("widowpenalty", ASSIGN_INT, WIDOW_PENALTY_CODE);
	primitive("displaywidowpenalty", ASSIGN_INT, DISPLAY_WIDOW_PENALTY_CODE);
	primitive("brokenpenalty", ASSIGN_INT, BROKEN_PENALTY_CODE);
	primitive("binoppenalty", ASSIGN_INT, BIN_OP_PENALTY_CODE);
	primitive("relpenalty", ASSIGN_INT, REL_PENALTY_CODE);
	primitive("predisplaypenalty", ASSIGN_INT, PRE_DISPLAY_PENALTY_CODE);
	primitive("postdisplaypenalty", ASSIGN_INT, POST_DISPLAY_PENALTY_CODE);
	primitive("interlinepenalty", ASSIGN_INT, INTER_LINE_PENALTY_CODE);
	primitive("doublehyphendemerits", ASSIGN_INT, DOUBLE_HYPHEN_DEMERITS_CODE);
	primitive("finalhyphendemerits", ASSIGN_INT, FINAL_HYPHEN_DEMERITS_CODE);
	primitive("adjdemerits", ASSIGN_INT, ADJ_DEMERITS_CODE);
	primitive("mag", ASSIGN_INT, MAG_CODE);
	primitive("delimiterfactor", ASSIGN_INT, DELIMITER_FACTOR_CODE);
	primitive("looseness", ASSIGN_INT, LOOSENESS_CODE);
	primitive("time", ASSIGN_INT, TIME_CODE);
	primitive("day", ASSIGN_INT, DAY_CODE);
	primitive("month", ASSIGN_INT, MONTH_CODE);
	primitive("year", ASSIGN_INT, YEAR_CODE);
	primitive("showboxbreadth", ASSIGN_INT, SHOW_BOX_BREADTH_CODE);
	primitive("showboxdepth", ASSIGN_INT, SHOW_BOX_DEPTH_CODE);
	primitive("hbadness", ASSIGN_INT, HBADNESS_CODE);
	primitive("vbadness", ASSIGN_INT, VBADNESS_CODE);
	primitive("pausing", ASSIGN_INT, PAUSING_CODE);
	primitive("tracingonline", ASSIGN_INT, TRACING_ONLINE_CODE);
	primitive("tracingmacros", ASSIGN_INT, TRACING_MACROS_CODE);
	primitive("tracingstats", ASSIGN_INT, TRACING_STATS_CODE);
	primitive("tracingoutput", ASSIGN_INT, TRACING_OUTPUT_CODE);
	primitive("tracingparagraphs", ASSIGN_INT, TRACING_PARAGRAPHS_CODE);
	primitive("tracingpages", ASSIGN_INT, TRACING_PAGES_CODE);
	primitive("tracinglostchars", ASSIGN_INT, TRACING_LOST_CHARS_CODE);
	primitive("tracingcommands", ASSIGN_INT, TRACING_COMMANDS_CODE);
	primitive("tracingrestores", ASSIGN_INT, TRACING_RESTORES_CODE);
	primitive("uchyph", ASSIGN_INT, UC_HYPH_CODE);
	primitive("outputpenalty", ASSIGN_INT, OUTPUT_PENALTY_CODE);
	primitive("maxdeadcycles", ASSIGN_INT, MAX_DEAD_CYCLES_CODE);
	primitive("floatingpenalty", ASSIGN_INT, FLOATING_PENALTY_CODE);
	primitive("globaldefs", ASSIGN_INT, GLOBAL_DEFS_CODE);
	primitive("fam", ASSIGN_INT, CUR_FAM_CODE);
	primitive("escapechar", ASSIGN_INT, ESCAPE_CHAR_CODE);
	primitive("defaulthyphenchar", ASSIGN_INT, DEFAULT_HYPHEN_CHAR_CODE);
	primitive("defaultskewchar", ASSIGN_INT, DEFAULT_SKEW_CHAR_CODE);
	primitive("endlinechar", ASSIGN_INT, END_LINE_CHAR_CODE);
	primitive("newlinechar", ASSIGN_INT, NEW_LINE_CHAR_CODE);
	primitive("language", ASSIGN_INT, LANGUAGE_CODE);
	primitive("lefthyphenmin", ASSIGN_INT, LEFT_HYPHEN_MIN_CODE);
	primitive("righthyphenmin", ASSIGN_INT, RIGHT_HYPHEN_MIN_CODE);
	primitive("holdinginserts", ASSIGN_INT, HOLDING_INSERTS_CODE);
	primitive("errorcontextlines", ASSIGN_INT, ERROR_CONTEXT_LINES_CODE);
	primitive("parindent", ASSIGN_DIMEN, PAR_INDENT_CODE);
	primitive("mathsurround", ASSIGN_DIMEN, MATH_SURROUND_CODE);
	primitive("lineskiplimit", ASSIGN_DIMEN, LINE_SKIP_LIMIT_CODE);
	primitive("hsize", ASSIGN_DIMEN, HSIZE_CODE);
	primitive("vsize", ASSIGN_DIMEN, VSIZE_CODE);
	primitive("maxdepth", ASSIGN_DIMEN, MAX_DEPTH_CODE);
	primitive("splitmaxdepth", ASSIGN_DIMEN, SPLIT_MAX_DEPTH_CODE);
	primitive("boxmaxdepth", ASSIGN_DIMEN, BOX_MAX_DEPTH_CODE);
	primitive("hfuzz", ASSIGN_DIMEN, HFUZZ_CODE);
	primitive("vfuzz", ASSIGN_DIMEN, VFUZZ_CODE);
	primitive("delimitershortfall", ASSIGN_DIMEN, DELIMITER_SHORTFALL_CODE);
	primitive("nulldelimiterspace", ASSIGN_DIMEN, NULL_DELIMITER_SPACE_CODE);
	primitive("scriptspace", ASSIGN_DIMEN, SCRIPT_SPACE_CODE);
	primitive("predisplaysize", ASSIGN_DIMEN, PRE_DISPLAY_SIZE_CODE);
	primitive("displaywidth", ASSIGN_DIMEN, DISPLAY_WIDTH_CODE);
	primitive("displayindent", ASSIGN_DIMEN, DISPLAY_INDENT_CODE);
	primitive("overfullrule", ASSIGN_DIMEN, OVERFULL_RULE_CODE);
	primitive("hangafter", ASSIGN_INT, HANG_AFTER_CODE);
	primitive("hangindent", ASSIGN_DIMEN, HANG_INDENT_CODE);
	primitive("hoffset", ASSIGN_DIMEN, H_OFFSET_CODE);
	primitive("voffset", ASSIGN_DIMEN, V_OFFSET_CODE);
	primitive("emergencystretch", ASSIGN_DIMEN, EMERGENCY_STRETCH_CODE);
	primitive(" ", EX_SPACE, 0);
	primitive("/", ITAL_CORR, 0);
	primitive("accent", ACCENT, 0);
	primitive("advance", ADVANCE, 0);
	primitive("afterassignment", AFTER_ASSIGNMENT, 0);
	primitive("aftergroup", AFTER_GROUP, 0);
	primitive("begingroup", BEGIN_GROUP, 0);
	primitive("char", CHAR_NUM, 0);
	primitive("csname", CS_NAME, 0);
	primitive("font", DEF_FONT, 0);
	primitive("fontdimen", ASSIGN_FONT_DIMEN, 0);
	FROZEN_NULL_FONT=freeze(primitive("nullfont", SET_FONT, null_font));
	font_id_base[null_font] = FROZEN_NULL_FONT;
	primitive("delimiter", DELIM_NUM, 0);
	primitive("divide", DIVIDE, 0);
	primitive("endcsname", END_CS_NAME, 0);
	FROZEN_END_GROUP=freeze(primitive("endgroup", END_GROUP, 0));
	primitive("expandafter", EXPAND_AFTER, 0);
	primitive("halign", HALIGN, 0);
	primitive("hrule", HRULE, 0);
	primitive("ignorespaces", IGNORE_SPACES, 0);
	primitive("insert", INSERT, 0);
	primitive("mark", MARK, 0);
	primitive("topmark", TOP_BOT_MARK, TOP_MARK_CODE);
	primitive("firstmark", TOP_BOT_MARK, FIRST_MARK_CODE);
	primitive("botmark", TOP_BOT_MARK, BOT_MARK_CODE);
	primitive("splitfirstmark", TOP_BOT_MARK, SPLIT_FIRST_MARK_CODE);
	primitive("splitbotmark", TOP_BOT_MARK, SPLIT_BOT_MARK_CODE);
	primitive("mathaccent", MATH_ACCENT, 0);
	primitive("mathchar", MATH_CHAR_NUM, 0);
	primitive("mathchoice", MATH_CHOICE, 0);
	primitive("multiply", MULTIPLY, 0);
	primitive("noalign", NO_ALIGN, 0);
	primitive("noboundary", NO_BOUNDARY, 0);
	FROZEN_DONT_EXPAND=freeze(primitive("noexpand", NO_EXPAND, 0));
	eq_type(FROZEN_DONT_EXPAND) = DONT_EXPAND;
	text(FROZEN_DONT_EXPAND) = "notexpanded:";
	length(FROZEN_DONT_EXPAND) = 12;
	primitive("nonscript", NON_SCRIPT, 0);
	primitive("omit", OMIT, 0);
	par_shape_cs=primitive("parshape", SET_SHAPE, 0);
	primitive("penalty", BREAK_PENALTY, 0);
	primitive("prevgraf", SET_PREV_GRAF, 0);
	primitive("radical", RADICAL, 0);
	primitive("read", READ_TO_CS, 0);
	FROZEN_RELAX=freeze(primitive("relax", RELAX, 256));
	primitive("setbox", SET_BOX, 0);
	primitive("the", THE, 0);
	primitive("toks", TOKS_REGISTER, 0);
	primitive("vadjust", VADJUST, 0);
	primitive("valign", VALIGN, 0);
	primitive("vcenter", VCENTER, 0);
	primitive("vrule", VRULE, 0);
	par_cs=primitive("par", PAR_END, 0);
	par_tok=sym2tok(par_cs);
	primitive("count", REGISTER, INT_VAL);
	primitive("dimen", REGISTER, DIMEN_VAL);
	primitive("skip", REGISTER, GLUE_VAL);
	primitive("muskip", REGISTER, MU_VAL);
	primitive("spacefactor", SET_AUX, HMODE);
	primitive("prevdepth", SET_AUX, VMODE);
	primitive("deadcycles", SET_PAGE_INT, 0);
	primitive("insertpenalties", SET_PAGE_INT, 1);
	primitive("wd", SET_BOX_DIMEN, WD_CODE);
	primitive("ht", SET_BOX_DIMEN, HT_CODE);
	primitive("dp", SET_BOX_DIMEN, DP_CODE);
	primitive("pagegoal", SET_PAGE_DIMEN, 0);
	primitive("pagetotal", SET_PAGE_DIMEN, 1);
	primitive("pagestretch", SET_PAGE_DIMEN, 2);
	primitive("pagefilstretch", SET_PAGE_DIMEN, 3);
	primitive("pagefillstretch", SET_PAGE_DIMEN, 4);
	primitive("pagefilllstretch", SET_PAGE_DIMEN, 5);
	primitive("pageshrink", SET_PAGE_DIMEN, 6);
	primitive("pagedepth", SET_PAGE_DIMEN, 7);
	primitive("lastpenalty", LAST_ITEM, INT_VAL);
	primitive("lastkern", LAST_ITEM, DIMEN_VAL);
	primitive("lastskip", LAST_ITEM, GLUE_VAL);
	primitive("inputlineno", LAST_ITEM, INPUT_LINE_NO_CODE);
	primitive("badness", LAST_ITEM, BADNESS_CODE);
	primitive("input", INPUT, 0);
	primitive("endinput", INPUT, 1);
	primitive("number", CONVERT, NUMBER_CODE);
	primitive("romannumeral", CONVERT, ROMAN_NUMERAL_CODE);
	primitive("string", CONVERT, STRING_CODE);
	primitive("meaning", CONVERT, MEANING_CODE);
	primitive("fontname", CONVERT, FONT_NAME_CODE);
	primitive("jobname", CONVERT, JOB_NAME_CODE);
	primitive("if", IF_TEST, IF_CHAR_CODE);
	primitive("ifcat", IF_TEST, IF_CAT_CODE);
	primitive("ifnum", IF_TEST, IF_INT_CODE);
	primitive("ifdim", IF_TEST, IF_DIM_CODE);
	primitive("ifodd", IF_TEST, IF_ODD_CODE);
	primitive("ifvmode", IF_TEST, IF_VMODE_CODE);
	primitive("ifhmode", IF_TEST, IF_HMODE_CODE);
	primitive("ifmmode", IF_TEST, IF_MMODE_CODE);
	primitive("ifinner", IF_TEST, IF_INNER_CODE);
	primitive("ifvoid", IF_TEST, IF_VOID_CODE);
	primitive("ifhbox", IF_TEST, IF_HBOX_CODE);
	primitive("ifvbox", IF_TEST, IF_VBOX_CODE);
	primitive("ifx", IF_TEST, IFX_CODE);
	primitive("ifeof", IF_TEST, IF_EOF_CODE);
	primitive("iftrue", IF_TEST, IF_TRUE_CODE);
	primitive("iffalse", IF_TEST, IF_FALSE_CODE);
	primitive("ifcase", IF_TEST, IF_CASE_CODE);
	FROZEN_FI=freeze(primitive("fi", FI_OR_ELSE, FI_CODE));
	primitive("or", FI_OR_ELSE, OR_CODE);
	primitive("else", FI_OR_ELSE, ELSE_CODE);
	primitive("hskip", HSKIP, SKIP_CODE);
	primitive("hfil", HSKIP, FIL_CODE);
	primitive("hfill", HSKIP, FILL_CODE);
	primitive("hss", HSKIP, SS_CODE);
	primitive("hfilneg",  HSKIP, FIL_NEG_CODE);
	primitive("vskip", VSKIP, SKIP_CODE);
	primitive("vfil", VSKIP, FIL_CODE);
	primitive("vfill", VSKIP, FILL_CODE);
	primitive("vss", VSKIP, SS_CODE);
	primitive("vfilneg", VSKIP, FIL_NEG_CODE);
	primitive("mskip", MSKIP, MSKIP_CODE);
	primitive("kern", KERN, EXPLICIT);
	primitive("mkern", MKERN, MU_GLUE);
	primitive("moveleft", HMOVE, 1);
	primitive("moveright", HMOVE, 0);
	primitive("raise", VMOVE, 1);
	primitive("lower", VMOVE, 0);
	primitive("box", MAKE_BOX, BOX_CODE);
	primitive("copy", MAKE_BOX, COPY_CODE);
	primitive("lastbox", MAKE_BOX, LAST_BOX_CODE);
	primitive("vsplit", MAKE_BOX, VSPLIT_CODE);
	primitive("vtop", MAKE_BOX, VTOP_CODE);
	primitive("vbox", MAKE_BOX, VTOP_CODE + VMODE);
	primitive("hbox", MAKE_BOX, VTOP_CODE + HMODE);
	primitive("indent", START_PAR, 1);
	primitive("noindent", START_PAR, 0);
	primitive("shipout", LEADER_SHIP, A_LEADERS - 1);
	primitive("leaders", LEADER_SHIP, A_LEADERS);
	primitive("cleaders", LEADER_SHIP, C_LEADERS);
	primitive("xleaders", LEADER_SHIP, X_LEADERS);
	primitive("unpenalty", REMOVE_ITEM, PENALTY_NODE);
	primitive("unkern", REMOVE_ITEM, KERN_NODE);
	primitive("unskip", REMOVE_ITEM, GLUE_NODE);
	primitive("unhbox", UN_HBOX, BOX_CODE);
	primitive("unhcopy", UN_HBOX, COPY_CODE);
	primitive("unvbox", UN_VBOX, BOX_CODE);
	primitive("unvcopy", UN_VBOX, COPY_CODE);
	primitive("discretionary", DISCRETIONARY, 0);
	primitive("-", DISCRETIONARY, 1);
	primitive("eqno", EQ_NO, 0);
	primitive("leqno", EQ_NO, 1);
	primitive("mathord", MATH_COMP, ORD_NOAD);
	primitive("mathop", MATH_COMP, OP_NOAD);
	primitive("mathbin", MATH_COMP, BIN_NOAD);
	primitive("mathrel", MATH_COMP, REL_NOAD);
	primitive("mathopen", MATH_COMP, OPEN_NOAD);
	primitive("mathclose", MATH_COMP, CLOSE_NOAD);
	primitive("mathpunct", MATH_COMP, PUNCT_NOAD);
	primitive("mathinner", MATH_COMP, INNER_NOAD);
	primitive("underline", MATH_COMP, UNDER_NOAD);
	primitive("overline", MATH_COMP, OVER_NOAD);
	primitive("displaylimits", LIMIT_SWITCH, NORMAL);
	primitive("limits", LIMIT_SWITCH, LIMITS);
	primitive("nolimits", LIMIT_SWITCH, NO_LIMITS);
	primitive("displaystyle", MATH_STYLE, DISPLAY_STYLE);
	primitive("textstyle", MATH_STYLE, TEXT_STYLE);
	primitive("scriptstyle", MATH_STYLE, SCRIPT_STYLE);
	primitive("scriptscriptstyle", MATH_STYLE, SCRIPT_SCRIPT_STYLE);
	primitive("above", ABOVE, ABOVE_CODE);
	primitive("over", ABOVE, OVER_CODE);
	primitive("atop", ABOVE, ATOP_CODE);
	primitive("abovewithdelims", ABOVE, DELIMITED_CODE + ABOVE_CODE);
	primitive("overwithdelims", ABOVE, DELIMITED_CODE + OVER_CODE);
	primitive("atopwithdelims", ABOVE, DELIMITED_CODE + ATOP_CODE);
	primitive("left", LEFT_RIGHT, LEFT_NOAD);
	FROZEN_RIGHT=freeze(primitive("right", LEFT_RIGHT, RIGHT_NOAD));
	primitive("span", TAB_MARK, SPAN_CODE);
	FROZEN_CR=freeze(primitive("cr", CAR_RET, CR_CODE));
	primitive("crcr", CAR_RET, CR_CR_CODE);
	FROZEN_ENDV=new_sym("endtemplate", 11);
	eq_type(FROZEN_ENDV) = ENDV;
	eq_level(FROZEN_ENDV) = LEVEL_ONE;
	equiv(FROZEN_ENDV) = null_list;
	FROZEN_END_TEMPLATE=freeze(FROZEN_ENDV);
	eq_type(FROZEN_END_TEMPLATE) = END_TEMPLATE;
	primitive("long", PREFIX, 1);
	primitive("outer", PREFIX, 2);
	primitive("global", PREFIX, 4);
	primitive("def", DEF, 0);
	primitive("gdef", DEF, 1);
	primitive("edef", DEF, 2);
	primitive("xdef", DEF, 3);
	primitive("let", LET, NORMAL);
	primitive("futurelet", LET, NORMAL + 1);
	primitive("chardef", SHORTHAND_DEF, CHAR_DEF_CODE);
	primitive("mathchardef", SHORTHAND_DEF, MATH_CHAR_DEF_CODE);
	primitive("countdef", SHORTHAND_DEF, COUNT_DEF_CODE);
	primitive("dimendef", SHORTHAND_DEF, DIMEN_DEF_CODE);
	primitive("skipdef", SHORTHAND_DEF, SKIP_DEF_CODE);
	primitive("muskipdef", SHORTHAND_DEF, MU_SKIP_DEF_CODE);
	primitive("toksdef", SHORTHAND_DEF, TOKS_DEF_CODE);
	primitive("catcode", DEF_CODE, CAT_CODE);
	primitive("mathcode", DEF_CODE, MATH_CODE);
	primitive("lccode", DEF_CODE, LC_CODE);
	primitive("uccode", DEF_CODE, UC_CODE);
	primitive("sfcode", DEF_CODE, SF_CODE);
	primitive("delcode", DEF_CODE, DEL_CODE);
	primitive("textfont", DEF_FAMILY, TEXT_SIZE);
	primitive("scriptfont", DEF_FAMILY, SCRIPT_SIZE);
	primitive("scriptscriptfont", DEF_FAMILY, SCRIPT_SCRIPT_SIZE);
	primitive("hyphenation", HYPH_DATA, 0);
	primitive("patterns", HYPH_DATA, 1);
	primitive("hyphenchar", ASSIGN_FONT_INT, 0);
	primitive("skewchar", ASSIGN_FONT_INT, 1);
	primitive("batchmode", SET_INTERACTION,  BATCH_MODE);
	primitive("nonstopmode", SET_INTERACTION, NONSTOP_MODE);
	primitive("scrollmode", SET_INTERACTION, SCROLL_MODE);
	primitive("errorstopmode", SET_INTERACTION, ERROR_STOP_MODE);
	primitive("closein", IN_STREAM, 0);
	primitive("openin", IN_STREAM, 1);
	primitive("message", MESSAGE, 0);
	primitive("errmessage", MESSAGE, 1);
	primitive("lowercase", CASE_SHIFT, LC_CODE);
	primitive("uppercase", CASE_SHIFT, UC_CODE);
	primitive("show", XRAY, SHOW_CODE);
	primitive("showbox", XRAY, SHOW_BOX_CODE);
	primitive("showthe", XRAY, SHOW_THE_CODE);
	primitive("showlists", XRAY, SHOW_LISTS);
	primitive("openout", EXTENSION, OPEN_NODE);
	write_cs=primitive("write", EXTENSION, WRITE_NODE);
	FROZEN_END_WRITE=freeze(primitive("endwrite", OUTER_CALL, 0));
	eq_level(FROZEN_END_WRITE) = LEVEL_ONE;
	primitive("closeout", EXTENSION, CLOSE_NODE);
	primitive("special", EXTENSION, SPECIAL_NODE);
	primitive("immediate", EXTENSION, IMMEDIATE_CODE);
	primitive("setlanguage", EXTENSION, SET_LANGUAGE_CODE);
	primitive("end", STOP, 0);
//primitive("dump", STOP, 1);
	primitive("dump", RELAX, 256);
	FROZEN_PROTECTION = new_sym("inaccessible", 12);
	}
