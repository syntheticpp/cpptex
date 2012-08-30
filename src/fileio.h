
/*
 * $Id: fileio.h,v 1.10 2003/09/23 08:08:36 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef fileio_h_included
#define fileio_h_included

#include <string>

class fileio {
	public:
		int	name_length;
		str	name_of_file;
		str	cur_name;
		str	cur_area;
		str	cur_ext;
		int	area_delimiter;
		int	ext_delimiter;
		int	path_delimiter;
		bool	name_in_progress;
		str	job_name;
		str	job_area;
		bool	log_opened;
	
#define null_file ((FILE *) 0)

#define a_close(FP)		{fflush(FP);fclose(FP);}
#define b_close(FP)		{fflush(FP);fclose(FP);}

#define prompt_input(S)		{print(S); term_input();}

		file	*read_file;
		int	*read_open;

#define OPENED			1
#define CLOSED			2

#define READ_ACCESS		4
#define WRITE_ACCESS 		2

		char	*input_path;
		char	*format_path;
		char	*font_path;
	
		static const char *default_font_path;
		static const char *default_input_path;

		enum path_type {
			NO_FILE_PATH=0, INPUT_FILE_PATH=1, FONT_FILE_PATH=2 };
	
		str	str_texput;
		
		void  begin_name();
		bool  more_name(int c);
		void  end_name();
		void	scan_file_name();
		void	pack_file_name(str n, str a, str e);
		void	pack_job_name(str s);
		void	print_file_name(str n, str a, str e);
		str	make_name_str();
		void	prompt_job_name();
		void	start_input(const std::string&);
		void	start_input_prepare(const std::string&);
		void	start_input_do();
		FILE	*a_open_in();
		FILE	*a_open_out();
		FILE	*b_open_in();
		FILE	*b_open_out();
		bool	input_ln(file f);
		void	term_input();
		void	open_log_file();
		bool	test_access(int amode, int file_path);
		void	set_paths();
		int   get_real_name();
		void	_fileio_init();
		void	_fileio_init_once();

	private:
		str	cur_path;
};

#endif
