
/*
 * $Id: texext.h,v 1.5 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {
#define OPEN_NODE		0

	class _open_t {
		public:
			mcell	node_field;
			int	open_stream_field;
			str	open_name_field;
			str	open_area_field;
			str	open_ext_field;
	};

#define OPEN_NODE_SIZE		sizeof(_open_t)
#define open_stream(P)		((_open_t *) (P))->open_stream_field
#define open_name(P)		((_open_t *) (P))->open_name_field
#define open_area(P)		((_open_t *) (P))->open_area_field
#define open_ext(P)		((_open_t *) (P))->open_ext_field

#define WRITE_NODE		1

	class _write_t {
		public:
			mcell	node_field;
			int	write_stream_field;
			ptr	write_toks_field;
	};

#define WRITE_NODE_SIZE		sizeof(_write_t)
#define write_stream(P)		((_write_t *) (P))->write_stream_field
#define write_toks(P)		((_write_t *) (P))->write_toks_field

#define END_WRITE_TOKEN		sym2tok(FROZEN_END_WRITE)

	extern	file	write_file[];
	extern	bool	write_open[];
	extern	sym	write_cs;
	
	
#define CLOSE_NODE		2
	
	class _close_t {
		public:
			mcell	node_field;
			int	close_stream_field;
			ptr	fill_field;
	};

#define CLOSE_NODE_SIZE		sizeof(_close_t)
#define close_stream(P)		((_close_t *) (P))->close_stream_field


#define SPECIAL_NODE		3

	class _special_t {
		public:
			mcell	node_field;
			ptr	special_toks_field;
			ptr	fill_field;
	};

#define SPECIAL_NODE_SIZE	sizeof(_special_t)
#define special_toks(P)		((_special_t *) (P))->special_toks_field

#define IMMEDIATE_CODE		4

void	do_immediate();

#define SET_LANGUAGE_CODE	5
#define LANGUAGE_NODE		4

	class _language_t {
		public:
			mcell	node_field;
			int	lang_field;
			short	lhm_field;
			short	rhm_field;
	};

#define LANGUAGE_NODE_SIZE	sizeof(_language_t)
#define what_lang(P)		((_language_t *)(P))->lang_field
#define what_lhm(P)		((_language_t *)(P))->lhm_field
#define what_rhm(P)		((_language_t *)(P))->rhm_field

	void	do_extension();
	void	show_whatsit(ptr p);
	ptr	copy_whatsit(ptr p);
	void	free_whatsit(ptr p);
	void	line_break_whatsit(ptr p);
	void	try_hyph_whatsit(ptr p);
	void	out_whatsit(ptr p);
	void	append_whatsit(int s, int w);
	void	do_open();
	void	show_open(ptr p);
	ptr	copy_open(ptr p);
	void	free_open(ptr p);
	void	out_open(ptr p);
	void	do_write();
	void	show_write(ptr p);
	ptr	copy_write(ptr p);
	void	free_write(ptr p);
	void	out_write(ptr p);
	void	do_close();
	void	show_close(ptr p);
	ptr	copy_close(ptr p);
	void	free_close(ptr p);
	void	out_close(ptr p);
	void	do_special();
	void	show_special(ptr p);
	ptr	copy_special(ptr p);
	void	free_special(ptr p);
	void	out_special(ptr p);
	void	do_set_language();
	void	show_language(ptr p);
	ptr	copy_language(ptr p);
	void	free_language(ptr p);
	void	out_language(ptr p);
	void	fix_language();
	void  help_unbal_write();
	void	_texext_init();
	void	_texext_init_once();
};
