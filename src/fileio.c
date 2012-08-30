
/*
 * $Id: fileio.c,v 1.18 2003/09/23 08:08:36 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include <stdexcept>
#include <iostream>
#include <sys/types.h>
#ifdef quad
#undef quad
#endif
#include <sys/stat.h>
#include <stdlib.h>

#include "tex.h"

const char *fileio::default_font_path=":/usr/local/lib/tex/fonts/tfm";
const char *fileio::default_input_path=":/usr/local/lib/tex/inputs";

int	tex::name_length;
str	tex::name_of_file;
str	name_str;
str	area_str;
str	ext_str;
str	tex::cur_name;
str	tex::cur_area;
str	tex::cur_ext;
str	tex::job_area;
str	tex::job_name;
bool  tex::name_in_progress;
bool	tex::log_opened;
int	tex::area_delimiter;
int	tex::ext_delimiter;
int	tex::path_delimiter;
file	tex::read_file[16];
int	tex::read_open[17]; 
str	tex::str_dvi;
str	tex::str_log;
str	tex::str_tex;
str	tex::str_tfm;
str	tex::str_texput;

void tex::begin_name ()
	{
	name_str = name_of_file;
	area_str = null_str;
	ext_str = null_str;
	}
		
#define append_to_name(C) \
	{if (name_str - name_of_file == STR_SIZE) \
		overflow("file name", STR_SIZE); \
	*name_str++ = C;}

bool tex::more_name(int c)
	{
	if (c == ' ') {
		return FALSE;
		} 
	else {
		append_to_name(c);
		if (c == area_delimiter) {
			area_str = name_str;
			ext_str = null_str;
			} 
		else if (c == ext_delimiter && ext_str == null_str) {
			ext_str = name_str - 1;
			}
		return TRUE;
		}
	}

void tex::end_name ()
	{
	int	n;
	
	if (area_str == null_str) {
		cur_area = null_str;
		area_str = name_of_file;
		} 
	else {
		n = area_str - name_of_file;
		cur_area = new_str(n);
		strncpy(cur_area, name_of_file, n);
		}
	if (ext_str == null_str) {
		cur_ext = null_str;
		ext_str = name_str;
		} 
	else {
		n = name_str - ext_str;
		cur_ext = new_str(n);
		strncpy(cur_ext, ext_str, n);
		}
	n = ext_str - area_str;
	if (n == 0) {
		cur_name = null_str;
		}
	else {
		cur_name = new_str(n);
		strncpy(cur_name, area_str, n);
		}	
	}

void tex::scan_file_name ()
	{
	name_in_progress = TRUE;
	get_nbx_token();
	begin_name();
	loop {
		if (cur_cmd > OTHER_CHAR || cur_chr > 255) {
			back_input();
			break;
			}
		if (!more_name(cur_chr))
			break;
		get_x_token();
		}
	end_name();
	name_in_progress = FALSE;
	}

// CONVERT: concatenates three strings ( a-n-e ).
void tex::pack_file_name(str n, str a, str e)
	{
	str	s;
	
	name_str = name_of_file;
	for (s = a; *s; incr(s))
		append_to_name(*s);
	for (s = n; *s; incr(s))
		append_to_name(*s);
	for (s = e; *s; incr(s))
		append_to_name(*s);
	append_to_name(NUL);
	name_length = name_str - name_of_file;
	}

void tex::pack_job_name(str s)
	{
	cur_area = job_area;
	cur_name = job_name;
	cur_ext = s;
	pack_file_name(cur_name, cur_area, cur_ext);
	}

void tex::print_file_name(str n, str a, str e)
	{
	print(a);
	print(n);
	print(e);
	}


// Copy 'name_of_file' into a newly allocated string.

str tex::make_name_str()
	{
	str	s;
	
	s = new_str(name_length);
	strcpy(s, name_of_file);
	return (s);
	}


// Scan a file name from the input, open the file, switch input to this
// file and input the first line. 

void tex::start_input(const std::string& fname)
	{
	start_input_prepare(fname);
	start_input_do();
	}

void tex::start_input_prepare(const std::string& fname)
	{
	if(fname!="stdin") {
		if(fname.size()==0) {
			scan_file_name();
			if (cur_ext == null_str)
				cur_ext = str_tex;
			pack_file_name(cur_name, cur_area, cur_ext);
			}
		else {
			begin_name();
			strcpy(name_of_file, fname.c_str());
			name_str=name_of_file+strlen(fname.c_str());
			ext_str=null_str;
			area_str=null_str;
			end_name();
			pack_file_name(cur_name, cur_area, cur_ext);
			}
		loop {
			begin_file_reading();
			if((cur_file = a_open_in()))
				break;
			end_file_reading();
			if (cur_ext == str_tex) { // remove .tex and try again
				cur_ext = null_str;
				pack_file_name(cur_name, cur_area, cur_ext);
				begin_file_reading();
				if((cur_file = a_open_in()))
					break;
				end_file_reading();
				}
			std::cerr << "cannot open " << name_of_file << std::endl;
			throw std::logic_error("file not found");
			}
		}
	else {
		begin_name();
		strcpy(name_of_file, "stdin");
		name_str=name_of_file+strlen(fname.c_str());
		ext_str=null_str;
		area_str=null_str;
		end_name();
		pack_file_name(cur_name, cur_area, cur_ext);
		begin_file_reading();
		cur_file=stdin;
		}
	name = make_name_str();
	if (job_name == null_str) {
		job_area = cur_area;
		job_name = cur_name;
		open_log_file();
		}
	if (term_offset + str_length(name) > MAX_PRINT_LINE - 2) {
		print_ln();
		} 
	else if (term_offset > 0 || file_offset > 0) {
		print(" ");
		}
	incr(open_parens);
	print("(");
	print(name);
	cur_input.state_field = NEW_LINE;
	line = 1;
	index = 18;
	}

void tex::start_input_do()
	{
	input_ln(cur_file);
	if (end_line_char_active)
		*++limit = end_line_char;
	next = buffer;
	}


// Open the file 'name_of_file' in read access mode.

FILE *tex::a_open_in()
	{
	if(test_access(READ_ACCESS, INPUT_FILE_PATH))
		return (fopen(name_of_file, "r"));
	return null_file;
	}


// Open the file 'name_of_file' in write mode.

FILE *tex::a_open_out()
	{
	if(test_access(WRITE_ACCESS, NO_FILE_PATH))
		return (fopen(name_of_file, "w"));
	return null_file;
	}

FILE *tex::b_open_in()
	{
	if(test_access(READ_ACCESS, FONT_FILE_PATH))
		return (fopen(name_of_file, "rb"));
	return null_file;
	}

FILE *tex::b_open_out()
	{
	if(test_access(WRITE_ACCESS, NO_FILE_PATH))
		return (fopen(name_of_file, "wb"));
	return null_file;
	}

bool tex::input_ln(file f)
	{
	int	c;

	next = buffer;
	limit = next - 1;
	c = getc(f);
	if (feof(f))
		return FALSE;
	loop {
		if (c == EOLN || c == EOF)
			break;
		if ((*next = xord[c]) != ' ')
			limit = next;
		if (++next >= buffer + BUF_SIZE)
			overflow("buffer size", BUF_SIZE);
		c = getc(f);
	}
	next = buffer;
	return TRUE;
	}

void tex::term_input()
	{
	int	c;
	str	s;
	
	flush_str();
	loop {
		c = getc(term_in);
		if (c == EOLN)
			break;
		if (c == EOF)
			fatal_error("! End of file on the terminal");
		append_char(xord[c]);
		if (cur_str_ptr >= cur_str_end)
			overflow("str size", STR_SIZE);
		}
	term_offset = 0;
	decr(selector);
	if (cur_str_ptr != cur_str)
		for (s = cur_str; s < cur_str_ptr; incr(s))
			print_char(*s);
	print_ln();
	incr(selector);
	}

void tex::open_log_file()
	{
	byte	*s;
	byte	*t;
	int	old_setting;
	
	old_setting = selector;
	if (job_name == null_str) {
		job_area = null_str;
		job_name = str_texput;
		}
	pack_job_name(str_log);
	log_file = stderr;
	log_name = make_name_str();
	selector = LOG_ONLY;
	log_opened = TRUE;
	*input_ptr = cur_input;
	print_nl("**");
	s = input_stack[0].obj_field.f.buf_field;
	t = input_stack[0].obj_field.f.limit_field;
	if (*t == end_line_char)
		decr(t);
	while (s <= t)
		print_char(*s++);
	print_ln();
	selector = old_setting + 2; 
	}

str	tex::cur_path;
str	tex::input_path = default_input_path;
str 	tex::font_path = default_font_path;

void tex::set_paths ()
	{
	str	env_path;
	
	if((env_path = getenv("TEXPP_STY")))
		input_path = env_path;
	if((env_path = getenv("TEXPP_TFM")))
		font_path = env_path;
	}

/*
**	test_access(amode, file_path)
**
**  Test whether or not the file whose name is in the global name_of_file
**  can be opened for reading according to access mode.
**
**  If the filename given in name_of_file does not begin with '/', we try 
**  prepending all the ':'-separated areanames in the appropriate path to the
**  filename until access can be made.
*/

bool tex::test_access(int amode, int file_path)
	{
	int	nl;
	bool	ok;
	char	original_name[STR_SIZE];
	
	ok=FALSE;

	strcpy(original_name, name_of_file);
	nl = name_length;
	switch(file_path) {
		case NO_FILE_PATH:
			cur_path = null_str;
			break;
		case INPUT_FILE_PATH: 
			cur_path = input_path;
			break;
		case FONT_FILE_PATH: 
			cur_path = font_path;
			break;
		}
	if(name_of_file[0] == '/' || ( name_of_file[0] == '.' && name_of_file[1] == '/' )) {
		cur_path = null_str;
		}
	do {
		strcpy(name_of_file, original_name);
		name_length = nl;
		if (get_real_name()) {
			switch (amode) {
				case READ_ACCESS: {
					FILE *fp = fopen(name_of_file, "r");
					ok = fp != (FILE *) 0;
					if (ok) {
						struct stat st;
						fstat(fileno(fp), &st);
						fclose(fp);
						ok = (st.st_mode & S_IFMT) == S_IFREG;
						}
					break;
					}
				
				case WRITE_ACCESS: {
					FILE *fp = fopen(name_of_file, "w");
					ok = fp != (FILE *) 0;
					if (ok)
						fclose(fp);
					break;
					}
				}
			} 
		else {
			ok = FALSE;
			}
		} while (!ok && cur_path != null_str);
	return ok;
	}

#define append_to_real_name(C) \
	{if (k == STR_SIZE) \
		overflow("real_name", STR_SIZE); \
	real_name[k++] = C;}

int tex::get_real_name()
	{
	int	k;
	str	s;
	char	real_name[STR_SIZE];
	
	real_name[k = 0] = NUL;
	s = cur_path;
	while(*s && *s != path_delimiter) {
		append_to_real_name(*s);
		incr(s);
		}
	if (*s == NUL)
		cur_path = null_str;
	else 
		cur_path = ++s;
	if(k && real_name[k-1] != area_delimiter)
		append_to_real_name(area_delimiter);

	if (*(s = name_of_file)) {
		while (*s) {
			append_to_real_name(*s);
			incr(s);
			}
		}
	name_length = k;
	append_to_real_name(NUL);
	strcpy(name_of_file, real_name);
	return k;
	}
	
void tex::_fileio_init()
	{
	int	i;

	job_name = null_str;
	name_in_progress = FALSE;
	log_opened = FALSE;
	for (i = 0; i <= 16; incr(i)) 
	  read_open[i] = CLOSED;
	}

void tex::_fileio_init_once()
	{
	name_of_file = new_str(STR_SIZE);
	area_delimiter='/';
	ext_delimiter='.';
	path_delimiter=':';
	str_texput = "texput";
	}
