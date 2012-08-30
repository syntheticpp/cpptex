
/*
 * $Id: tex.c,v 1.27 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include <exception>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include "tex.h"

char *tex::banner="TeX++ 0.1 $Date: 2003/09/14 13:30:19 $";

bool tex::compatible=true;  // fallback mode to disable fiddling with default parameters

bool tex::display_debug=false;      // debugging output, only for things you never want to see
bool tex::display_progress=false;   // display progress information (page counters and so on)
bool tex::display_warnings=false;   // display all warnings (overfull boxes mainly)
bool tex::display_messages=false;   // display all other messages (\message, \show*)

enum preload_t { pr_plain, pr_latex, pr_none } preload=pr_plain;

int main(int argc, str argv[])
	{
	static struct option long_options[] = {
		{"hsize",         1, 0,  1},
		{"vsize",         1, 0,  2},
		{"hoffset",       1, 0,  3},
		{"voffset",       1, 0,  4},
		{"offset",        1, 0,  5},
		{"margin",        1, 0,  6},
		{"warnings",      0, 0, 13},
		{"progress",      0, 0, 15},
		{"debug",         0, 0,  8},
		{"compatible",    0, 0,  9},
		{"magnification", 1, 0, 10},
		{"plain",         0, 0, 11},
		{"latex",         0, 0, 12},
		{"initex",        0, 0, 14},
		{"help",          0, 0, 'h'},
		{"version",       0, 0, 'v'},
		{0, 0, 0, 0}
	};
	
	tex::mag_set = 1000;
	signal(SIGINT, tex::handle_int); 
	tex::set_paths();

	if(strcmp(argv[0],"tex++")==0)          preload=pr_plain;
	else if(strcmp(argv[0],"latex++")==0)   preload=pr_latex;

	while(1) {
		int option_index = 0;
		int c = getopt_long(argc, argv, "hv", long_options, &option_index);
		if (c == EOF)
			break;

		switch (c) {
			case 1:
				tex::override_internal[0].value=atol(optarg)*65536;
				tex::compatible=false;
				break;
			case 2:
				tex::override_internal[1].value=atol(optarg)*65536;
				tex::compatible=false;
				break;
			case 3:
				tex::override_internal[2].value=atol(optarg)*65536;
				tex::compatible=false;
				break;
			case 4:
				tex::override_internal[3].value=atol(optarg)*65536;
				tex::compatible=false;
				break;
			case 5:
				tex::override_internal[2].value=atol(optarg)*65536;
				tex::override_internal[3].value=atol(optarg)*65536;
				tex::compatible=false;
				break;
			case 6:
				// hoffset/voffset start at -1 inch.
				tex::override_internal[2].value=(int)((atol(optarg)-72.27)*65536);  // Magnification problems.
				tex::override_internal[3].value=(int)((atol(optarg)-72.27)*65536);
				// the others are more sane.
				tex::override[7].value=atol(optarg);
				tex::override[8].value=atol(optarg);
				tex::override[9].value=(atol(optarg)-55)*65536;
				tex::compatible=false;
				break;
			case 8:
				tex::display_debug=true;
				break;
			case 9:
				tex::compatible=true;
				break;
			case 10:
				tex::mag_set=atol(optarg);
				tex::compatible=false;
				break;
			case 11:
				preload=pr_plain;
				break;
			case 12:
				preload=pr_latex;
				break;
			case 13:
				tex::display_warnings=true;
				break;
			case 15: 
				tex::display_progress=true;
				break;
			case 14:
				preload=pr_none;
				break;
			case 'v': {
				std::cerr << tex::banner << std::endl
							 << "Copyright (C) 1987-1991 Pat Monardo" << std::endl
							 << "Copyright (C) 2002-2003 Kasper Peeters" << std::endl;
				exit(1);
				}
			case 'h': {
			   std::cerr << "Usage: tex++ [options]" << std::endl << std::endl;
				unsigned int i=0;
				while(long_options[i].name!=0) {
					std::cerr << "  --" <<  long_options[i].name;
					if(long_options[i].has_arg!=0) {
						std::cerr << "=[dimension in pt]";
						}
					std::cerr << std::endl;
					++i;
					}
				exit(1);
				}
			default:
				std::cerr << "tex++: option handling internal error" << std::endl;
				exit(2);
			}
		}

	tex::initialize_once();
	tex::initialize();
	if(!tex::compatible && tex::mag_set>0)
		tex::reg_gdefine(tex::int_reg[MAG_CODE], tex::INT_REG, tex::mag_set);

	try {
		bool from_file=tex::decode_args(argc,optind,argv);
		if (end_line_char_active)
			*++tex::limit = end_line_char;
		if (tex::interaction == tex::BATCH_MODE)
			tex::selector = NO_PRINT;
		else 
			tex::selector = TERM_ONLY;
		tex::fix_date_and_time();
		if(from_file) tex::start_input_prepare("");
		else          tex::start_input_prepare("stdin");
		if(tex::display_progress)
			std::cerr << "main input stream open..." << std::endl;
		if(preload==pr_plain) {
			if(tex::display_progress) 
				std::cerr << "reading plain.tex... " << std::endl;
			tex::start_input("plain.tex");
			}
		else if(preload==pr_latex) {
			if(tex::display_progress)
				std::cerr << "reading latex.ltx... " << std::endl;
			tex::start_input("latex.ltx");
			}
		tex::start_input_do();
		tex::history = tex::SPOTLESS;
		tex::main_control();
		}
	catch(std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		}
	tex::final_cleanup();
	if(tex::history==tex::SPOTLESS) return 0;
	else return tex::history;
	}

bool tex::decode_args(int argc, int offset, str argv[])
	{
	int	i, j;
	
	if(argc-offset==0) {
		return FALSE;
		}
	// There should be at most one argument left, which is
	// interpreted as a filename.
	for(i=offset; i < argc; i++) {
		j = str_length(argv[i]);
		argv[i][j] = ' ';
		if (limit + j + 1 >= buffer + BUF_SIZE) {
			fprintf(stderr, "out of buffer space\n");
			return FALSE;
			}
		strncpy((char *)(limit + 1), argv[i], j + 1);
		limit += j + 1;
		argv[i][j] = '\0';
		}
	return TRUE;
	}

void tex::fix_date_and_time()
	{
	long	clock, begintime();
	struct tm	*tmptr;
	
	clock = begintime();
	tmptr = localtime(&clock);
//	time = 60 * tmptr->tm_hour + tmptr->tm_min;
	day = tmptr->tm_mday;
	month = tmptr->tm_mon + 1;
	year = tmptr->tm_year + 1900;
	}

#undef time
long begintime()
	{
	return (time(0));
	}

void tex::handle_int(int num)
	{	
	signal(SIGINT, handle_int);
	interrupted = 1;
	}

void tex::initialize ()
	{
	tex::_chr_init();
	tex::_error_init();
	tex::_sym_init();
	tex::_def_init();
	tex::_scan_init();
	tex::_expand_init();
	tex::_fileio_init();
	tex::_cond_init();
	tex::_eval_init();
	tex::_box_init();
	tex::_boxlist_init();
	tex::_math_init();
	tex::_mathlist_init();
	tex::_hyph_init();
	tex::_pack_init();
	tex::_page_init();
	tex::_par_init();
	tex::_mlst_hlst_init();
	tex::_align_init();
	tex::_dvi_init();
	tex::_cmd_init();
	tex::_tfm_init();
	tex::_print_init();
	tex::_tok_init();
	}

void tex::initialize_once()
	{
	tex::_print_init_once();
	tex::_error_init_once();
	tex::_alloc_init_once();
	tex::_str_init_once();
	tex::_sym_init_once();
	tex::_tok_init_once();
	tex::_fileio_init_once();
	tex::_eval_init_once();
	tex::_box_init_once();
	tex::_boxlist_init_once();
	tex::_math_init_once();
	tex::_mathlist_init_once();
	tex::_hyph_init_once();
	tex::_pack_init_once();
	tex::_page_init_once();
	tex::_par_init_once();
	tex::_mlst_hlst_init_once();
	tex::_align_init_once();
	tex::_dvi_init_once();
	tex::_cmd_init_once();
	tex::_tfm_init_once();
	}

void tex::final_cleanup()
	{
	while(open_parens > 0) {
		print(" )");
		decr(open_parens);
		}
	if(cur_level > LEVEL_ONE) {
		std::cerr << "(\\end occurred inside a group at level " 
					 << cur_level - LEVEL_ONE << ")" << std::endl;
		}
	while(cond_ptr != null) {
		std::cerr << "(\\end occurred when ";
		print_cmd_chr(IF_TEST, cur_if);
		if (if_line != 0) {
			print(" on line ");
			print_int(if_line);
			}
		print(" was incomplete)");
		if_line = if_line_field(cond_ptr); 
		cur_if = subtype(cond_ptr);
		cond_ptr = link(cond_ptr);
		}
	close_files();
	}

void tex::close_files()
	{
	int	k;
	
	for (k = 0; k < 16; incr(k))
		if (write_open[k])
			a_close(write_file[k]);
	fin_dvi();

	std::cout << std::flush;
	std::cerr << std::flush;
	}

// bool tex::one(str s, str t)
// 	{	
// 	int	i;
// 	int	j;
// 	int	one;
// 
// 	one = 0;
// 	j = strlen(t) - 1;
// 	for (i = 0; i < j; incr(i))
// 	  if (strncmp(s, &t[i], 2) == 0)
// 		 if (++one > 1)
// 			return FALSE;
// 	if (one == 0)
// 	  return FALSE;
// 	return TRUE;
// 	}

