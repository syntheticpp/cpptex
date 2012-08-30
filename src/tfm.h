
/*
 * $Id: tfm.h,v 1.6 2003/09/23 08:08:36 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef tfm_h_included
#define tfm_h_included

#include "textypes.h"
#include "cells.h"

class font_set_t {
	public:
		class font_t {
			public:
				int	font_length_field;
				tex::qcell	font_check_field;
				scal	font_size_field;
				scal	font_dsize_field;
				int	font_params_field;
				str	font_name_field;
				str	font_area_field;
				int	font_bc_field;
				int	font_ec_field;
				ptr	font_glue_field;
				bool	font_used_field;
				int	hyphen_char_field;
				int	skew_char_field;
				int	font_bchar_field;
				int	font_false_bchar_field;
				tex::qcell	*char_base_field;
				scal	*width_base_field;
				scal	*height_base_field;
				scal	*depth_base_field;
				scal	*italic_base_field;
				tex::qcell	*lig_kern_base_field;
				tex::qcell	*bchar_label_field;
				scal	*kern_base_field;
				tex::qcell	*exten_base_field;
				scal	*param_base_field;
			private:
				font_set_t& font_set_;
		};

		font_t *fmem(int F) { return (font_info[(F) - null_font]); }
		int     font_length(int F) { return fmem(F)->font_length_field; }
		tex::qcell   font_check(int F) { return fmem(F)->font_check_field; }
		scal    font_size(int F) { return fmem(F)->font_size_field; }
		scal    font_dsize(int F) { return fmem(F)->font_dsize_field; }
		int     font_params(int F) { return fmem(F)->font_params_field; }
		str     font_name(int F) { return fmem(F)->font_name_field; }
		str     font_area(int F) { return fmem(F)->font_area_field; }
		int     font_bc(int F) { return fmem(F)->font_bc_field; }
		int     font_ec(int F) { return fmem(F)->font_ec_field; }
		ptr     font_glue(int F) { return fmem(F)->font_glue_field; }
		bool    font_used(int F) { return fmem(F)->font_used_field; }
		int     hyphen_char(int F) { return fmem(F)->hyphen_char_field; }
		int     skew_char(int F) { return fmem(F)->skew_char_field; }
		int     font_bchar(int F) { return fmem(F)->font_bchar_field; }
		int     font_false_bchar(int F) { return fmem(F)->font_false_bchar_field; }
		tex::qcell *bchar_label(int F) { return fmem(F)->bchar_label_field; }
		tex::qcell *char_base(int F) { return fmem(F)->char_base_field; }
		scal       *width_base(int F) { return fmem(F)->width_base_field; }
		scal       *height_base(int F) { return fmem(F)->height_base_field; }
		scal       *depth_base(int F) { return fmem(F)->depth_base_field; }
		scal       *italic_base(int F) { return fmem(F)->italic_base_field; }
		tex::qcell *lig_kern_base(int F) { return fmem(F)->lig_kern_base_field; }
		scal       *kern_base(int F) { return fmem(F)->kern_base_field; }
		scal       *param_base(int F) { return fmem(F)->param_base_field; }
		tex::qcell *exten_base(int F) { return fmem(F)->exten_base_field; }
	
		enum codes_t {
			SLANT_CODE=1, SPACE_CODE=2, SPACE_STRETCH_CODE=3,
			SPACE_SHRINK_CODE=4, X_HEIGHT_CODE=5, QUAD_CODE=6,
			EXTRA_SPACE_CODE=7 };

		int	nfonts;
		font_t	**font_info;
		font_t	**font_end;
		font_t	**font_ptr;

		const static int FONT_BASE=32;
		const static int FONT_MAX =1250;
		const static int null_font=FONT_BASE;
	private:
		file	     tfm_file; // current tfm file
		tex::qcell null_character;


#define NON_CHAR		256
#define NON_ADDRESS		0

#define char_info(F, C)		char_base(F)[C]
#define char_exists(CI)		((CI).b0 > 0)
#define char_width(F, CI)	width_base(F)[(CI).b0]
#define height_depth(CI)	(CI).b1
#define char_height(F, CB)	height_base(F)[(CB) / 16]
#define char_depth(F, CB)	depth_base(F)[(CB) % 16]
#define char_italic(F, CI)	italic_base(F)[(CI).b2 / 4]
#define char_tag(CI)		((CI).b2 % 4)

#define NO_TAG			0
#define LIG_TAG			1
#define LIST_TAG		2
#define EXT_TAG			3

#define skip_byte(LK)		(LK).b0
#define next_char(LK)		(LK).b1
#define op_byte(LK)		(LK).b2
#define rem_byte(LK)		(LK).b3
#define lig_offset(LK)		((op_byte(LK) * 256) + rem_byte(LK))
#define kern_offset(LK)		(((op_byte(LK) - 128) * 256) + rem_byte(LK))
#define char_kern(F, LK)	kern_base(F)[kern_offset(LK)]
#define lig_kern_start(F, LK)	(lig_kern_base(F) + rem_byte(LK))
#define lig_kern_restart(F, LK)	(lig_kern_base(F) + lig_offset(LK))

#define STOP_FLAG		128
#define KERN_FLAG		128

#define ext_top(M)		M.b0
#define ext_mid(M)		M.b1
#define ext_bot(M)		M.b2
#define ext_rep(M)		M.b3

#define param(F, CODE)		param_base(F)[CODE]
#define slant(F)		param(F, SLANT_CODE)
#define space(F)		param(F, SPACE_CODE)
#define space_stretch(F)	param(F, SPACE_STRETCH_CODE)
#define space_shrink(F)		param(F, SPACE_SHRINK_CODE)
#define x_height(F)		param(F, X_HEIGHT_CODE)
#define quad(F)			param(F, QUAD_CODE)
#define extra_space(F)		param(F, EXTRA_SPACE_CODE)

//		fnt	read_font_info(sym u, str nom, str aire, scal s);
		fnt	read_font_info(str nom, str aire, scal s);
		void	scan_font_ident();
		scal	*find_font_dimen(bool writing);
		void  adjust_bases(fnt f, int x);
		ptr	find_font_glue(fnt f);
		void	char_warning(fnt f, int c);
		ptr	new_character(fnt f, int c);
		void	_tfm_init();
		void	_tfm_init_once();

		void read_sixteen(int&);
		void store_four_quarters(tex::qcell& Q);
		void store_scaled(scal& S, scal z);
		void check_byte_range(int);
		tex::qcell check_existence(int);

		// Variables used while reading a font:

		int alpha, beta; // box dimensions while reading file.
		int bc, ec;      // beginning/ending character
		fnt f;           // pointer to current font
};

#endif
