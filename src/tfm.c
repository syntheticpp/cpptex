
/*
 * $Id: tfm.c,v 1.4 2003/09/23 08:08:36 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include "tfm.h"
#include "error.h"

// #define start_font_error_message() 
// {	print_err("Font "); sprint_cs(u); 
// 	print("="); print_file_name(nom, aire, null_str); 
// 	if (s >= 0) {print(" at "); print_scaled(s); print("pt");} 
// 	else if (s != -1000) {print(" scaled "); print_int(-s);} 
// }

void font_set_t::read_sixteen(int& X)
	{	
	X = getc(tfm_file);
	if (X > 255) 
		throw tex::file_error("Illegal word in .tfm file.");
	X = (X) * 0400 + getc(tfm_file);
	}

void font_set_t::store_four_quarters(tex::qcell& Q)
	{
	Q.b0 = getc(tfm_file);
	Q.b1 = getc(tfm_file);
	Q.b2 = getc(tfm_file);
	Q.b3 = getc(tfm_file);
	}

void font_set_t::store_scaled(scal& S, scal z)
	{
	int a,b,c,d;
	scal sw;

	a = getc(tfm_file); b = getc(tfm_file);
	c = getc(tfm_file); d = getc(tfm_file);
	sw = ((((d * z) / 0400) + (c * z)) / 0400 + (b * z)) / beta; 
	if (a == 0) S = sw; 
	else if (a == 255) S = sw - alpha; 
	else throw tex::file_error("Illegal byte while reading scal.");
	}

void font_set_t::check_byte_range(int C) 
	{	
	if (C < bc || C > ec)
		throw tex::file_error("Byte out of range.");
	}

tex::qcell font_set_t::check_existence(int C)
	{	
	check_byte_range(C); 
	tex::qcell qw = char_info(f, C); 
	if (!char_exists(qw)) 
		throw tex::file_error("Char does not exist.");
	return qw;
	}

fnt font_set_t::read_font_info(str nom, str aire, scal s)
	{
	int	a, b, c, d, k;
	fnt	g;
	int	lf, lh;
	int	nw, nh, nd, ni, nk, nl, ne, np;
	tex::qcell	qw;
	scal	sw, z;
	tex::qcell	*fp;
	tex::qcell	*pq;
	scal	*ps;
	int	bchar;
	int	bch_label;
	bool	file_opened;
	tex::qcell	dev_null;

	// Open tfm_file for input.
	g = null_font;
	file_opened = FALSE;
	pack_file_name(nom, aire, str_tfm);
	if ((tfm_file = b_open_in()) == null_file)
		abort();
	file_opened = TRUE;
	
	// Read the .tfm size fields.
	read_sixteen(lf);
	read_sixteen(lh);
	read_sixteen(bc);
	read_sixteen(ec);
	if (bc > ec + 1 || ec > 255)
		abort();
	if (bc > 255) {
		bc = 1;
		ec = 0;
		}
	read_sixteen(nw);
	read_sixteen(nh);
	read_sixteen(nd);
	read_sixteen(ni);
	read_sixteen(nl);
	read_sixteen(nk);
	read_sixteen(ne);
	read_sixteen(np);
	if (lf != 6 + lh + (ec - bc + 1) +
		nw + nh + nd + ni + nl + nk + ne + np) 
		abort();

	// Allocate font information memory.
	if (font_ptr == font_end) {
		start_font_error_message();
		print(" not loaded: No more room.");
		help_font();
		error();
		goto done;
		}
	k = (ec - bc + 1 + nl + ne) * sizeof(tex::qcell) + 
		(nw + nh + nd + ni + nk + np + 1) * sizeof(scal);
	if (np < 7)
		k += (7 - np) * sizeof(scal);	
	f = null_font + font_ptr - font_info;
	*font_ptr = (font_t *)malloc(sizeof(font_t) + k);
	if (*font_ptr == (font_t *) 0) {
		start_font_error_message();
		print(" not loaded: No more room.");
		help_font();
		error();
		goto done;
		}
	font_length(f) = sizeof(font_t) + k;
	fp = (tex::qcell *) (fmem(f) + 1);
	char_base(f) = fp - bc;
	width_base(f) = (scal *) (char_base(f) + ec + 1);
	height_base(f) = width_base(f) + nw;
	depth_base(f) = height_base(f) + nh;
	italic_base(f) = depth_base(f) + nd;
	lig_kern_base(f) = (tex::qcell *) (italic_base(f) + ni);
	kern_base(f) = (scal *) (lig_kern_base(f) + nl);
	exten_base(f) = (tex::qcell *) (kern_base(f) + nk);
	param_base(f) = (scal *) (exten_base(f) + ne);

	// Read the .tfm header.
	if (lh < 2)
		abort();
	store_four_quarters(font_check(f));
	read_sixteen(z);
	z = z * 0400 + getc(tfm_file);
	z = z * 020 + getc(tfm_file) / 020;
	if (z < UNITY)
		abort();
	while (lh > 2) {
		store_four_quarters(dev_null);
		decr(lh);
	}
	font_dsize(f) = z;
	if (s != -1000) {
		if (s >= 0)
			z = s;
		else z = xn_over_d(z, -s, 1000L);
	}
	font_size(f) = z;


	// Read the character data.
	for (pq = fp; pq < (tex::qcell *)width_base(f); incr(pq)) {
		store_four_quarters(*pq);
		if (a >= nw || b / 020 >= nh || b % 020 >= nd || c / 4 > ni) 
			abort();
		switch (c % 4)
		{
		case LIG_TAG:
			if (d >= nl)
				abort();
			break;
		
		case EXT_TAG:
			if (d >= ne)
				abort();
			break;
		
		case LIST_TAG:
#define ccbwo ((int) (pq + bc - fp))
			check_byte_range(d);
			while (d < ccbwo) {
				qw = char_info(f, d);
				if (char_tag(qw) != LIST_TAG)
					goto not_found;
				d = rem_byte(qw);
			}
			if (d == ccbwo)
				abort();
			break;

		not_found:
			break;
		}
	}

	// Read box dimensions.
	alpha = 16;
	while (z >= 040000000) {
		z >>= 1;
		alpha <<= 1;
	}
	beta = 256 / alpha;
	alpha *= z;
	for (ps = width_base(f); ps < (scal *)lig_kern_base(f); incr(ps))
		store_scaled(*ps, z);
	if (width_base(f)[0] != 0)
		abort();
	if (height_base(f)[0] != 0)
		abort();
	if (depth_base(f)[0] != 0)
		abort();
	if (italic_base(f)[0] != 0)
		abort();

	// Read ligature/kern programs.
	bch_label = 077777;
	bchar = 256;
	if (nl > 0) {
		pq = lig_kern_base(f);
		while (pq < (tex::qcell *) kern_base(f)) {
			store_four_quarters(*pq);
			if (a > 128) {
				if (256 * c + d >= nl)
					abort();
				if (a == 255
					 && pq == lig_kern_base(f)) {
					bchar = b;
					}
				} 
			else {
				if (b != bchar) {
					check_existence(b);
					}
				if (c < 128) {
					check_existence(d);
					} 
				else if (256 * (c - 128) + d >= nk) {
					abort();
					}
				if (a < 128
					 && pq - lig_kern_base(f) + a + 1 >= nl) {
					abort();
					}
				}
			if (a == 255) {
				bch_label = 256 * c + d;
				}
			incr(pq);
			}
		}

	// Read kern dimensions.
	for (ps = kern_base(f); ps < (scal *)exten_base(f); incr(ps))
		store_scaled(*ps, z);

	// Read extensible character recipes.
	for (pq = exten_base(f); pq < (tex::qcell *)param_base(f); incr(pq)) {
		store_four_quarters(*pq);
		if (a != 0)
			check_existence(a);
		if (b != 0)
			check_existence(b);
		if (c != 0)
			check_existence(c);
		check_existence(d);
		}

	// Read font parameters.
	for (k = 1; k <= np; incr(k)) {
		if (k == 1) {
			sw = getc(tfm_file);
			if (sw > 127)
				sw -= 256;
			sw = sw * 0400 + getc(tfm_file);
			sw = sw * 0400 + getc(tfm_file);
			param(f, 0) = sw * 020 + getc(tfm_file) / 020;
			} 
		else {
			store_scaled(param(f, k - 1), z);
			}
		}
	if (feof(tfm_file))
		abort();
	
	for (k = np + 1; k <= 7; incr(k))
		param(f, k - 1) = 0;

	// Final adjustments.
	decr(param_base(f));
	font_params(f) = (np >= 7) ? np : 7;
	hyphen_char(f) = default_hyphen_char;
	skew_char(f) = default_skew_char;
	if (bch_label < nl) {
		bchar_label(f) = lig_kern_base(f) + bch_label;
		} 
	else {
		bchar_label(f) = NON_ADDRESS;
		}
	font_bchar(f) = bchar;
	font_false_bchar(f) = bchar;
	if (bchar <= ec && bchar >= bc) {
		qw = char_info(f, bchar);
		if (char_exists(qw))
			font_false_bchar(f) = NON_CHAR;
		}
	font_name(f) = nom;
	font_area(f) = aire;
	font_bc(f) = bc;
	font_ec(f) = ec;
	font_glue(f) = null;
	font_used(f) = FALSE;
	incr(font_ptr);
	g = f;

	if (file_opened)
		b_close(tfm_file);
	return g;
	}

void tex::scan_font_ident ()
	{	
	int	m;
	fnt	f;

	get_nbx_token();
	if (cur_cmd == DEF_FONT) {
		f = cur_font;
		} 
	else if (cur_cmd == SET_FONT) {
		f = cur_chr;
		} 
	else if (cur_cmd == DEF_FAMILY) {
		m = cur_chr;
		scan_four_bit_int();
		f = reg_equiv(fnt_reg[1 + m + cur_val]);
		} 
	else {
		print_err("Missing font identifier");
		help_font_cs();
		back_error();
		f = null_font;
		}
	cur_val = f;
	}

scal *tex::find_font_dimen(bool writing)
	{
	fnt	f;
	int	i, l, m, n, x;
	char	*fp, *nfp;
	
	scan_int();
	n = cur_val;
	scan_font_ident();
	f = cur_val;
	if (n <= 0) {
		throw tex::file_error("negative value read.");
		} 
	else {
		if (writing && n <= SPACE_SHRINK_CODE && 
			 n >= SPACE_CODE && font_glue(f) != null) {
			delete_glue_ref(font_glue(f));
			font_glue(f) = null;
			}
		if (n > font_params(f)) {
			if (f < null_font + font_ptr - font_info - 1) {
				throw tex::file_error("illegal pointer offset.");
				}
			i = font_params(f);
			l = font_length(f);
			m = l + (n - font_params(f)) * sizeof(scal);
			fp = (char *)fmem(f);
			nfp = (char *)realloc((void *)fp, m);
			x = nfp - fp;
			if (fp == (char *) 0)
				confusion("font mem");
			if (nfp == (char *) 0)
				overflow("font mem", m);
			fmem(f) = (font_t *) nfp;
			adjust_bases(f, x);
			font_length(f) = m;
			font_params(f) = n;
			while (i++ < n)
				param(f, i) = 0;
			return (param_base(f) + font_params(f));
			} 
		else {
			return (param_base(f) + n);
			}
		}
	return ((scal *) 0);
	}
		
void tex::adjust_bases(fnt f, int x)
	{
	char_base(f) = (tex::qcell *) ((char *)char_base(f) + x);
	width_base(f) = (scal *) ((char *)width_base(f) + x);
	height_base(f) = (scal *) ((char *)height_base(f) + x);
	depth_base(f) = (scal *) ((char *)depth_base(f) + x);
	italic_base(f) = (scal *) ((char *)italic_base(f) + x);
	lig_kern_base(f) = (tex::qcell *) ((char *)lig_kern_base(f) + x);
	bchar_label(f) = (tex::qcell *) ((char *)bchar_label(f) + x);
	kern_base(f) = (scal *) ((char *)kern_base(f) + x);
	exten_base(f) = (tex::qcell *) ((char *)exten_base(f) + x);
	param_base(f) = (scal *) ((char *)param_base(f) + x);
	}

ptr tex::find_font_glue(fnt f)
	{
	ptr	p;
	scal	*q;
	
	p = font_glue(f);
	if (p == null) {
		p = new_spec(zero_glue);
		q = param_base(cur_font) + SPACE_CODE;
		glue_width(p) = *q++;
		stretch(p) = *q++;
		shrink(p) = *q++; 
		font_glue(f) = p;
		}
	return p;
	}

void tex::char_warning(fnt f, int c)
	{
	if (tracing_lost_chars > 0) {
		begin_diagnostic();
		print_nl("Missing character: There is no ");
		print_ASCII(c);
		print(" in font ");
		print(font_name(f));
		print("!");
		end_diagnostic(FALSE);
		}
	}

ptr tex::new_character(fnt f, int c)
	{
	ptr	p;

	if (font_bc(f) <= c && font_ec(f) >= c && char_exists(char_info(f, c))) {
		p = new_avail();
		font(p) = f;
		character(p) = c;
		return p;
		}
	char_warning(f, c);
	return null;
	}

font_set_t::font_set_t()
	{
	int	l;
	
	nfonts = FONT_MAX-FONT_BASE+1;
	font_info = (font_t **)malloc(nfonts * sizeof(font_t *));
	if (font_info == (font_t **) 0)
		overflow("font info", nfonts);
	font_end = font_info + nfonts;
	font_ptr = font_info + 1;
	l = sizeof(font_t) + 7 * sizeof(scal);
	font_info[0] = (font_t *)malloc(l);
	if (font_info[0] == (font_t *) 0)
		overflow("nullfont", l);
	memset(fmem(null_font), 0, l);
	font_length(null_font) = l;
	param_base(null_font) = (scal *) (fmem(null_font) + 1);
	font_params(null_font) = 7;
	font_area(null_font) = null_str;
	font_name(null_font) = "nullfont";
	font_bc(null_font) = 1;
	font_ec(null_font) = 0;
	font_glue(null_font) = zero_glue;
	hyphen_char(null_font) = '-';
	skew_char(null_font) = -1;
	bchar_label(null_font) = NON_ADDRESS;
	font_bchar(null_font) = NON_CHAR;
	font_false_bchar(null_font) = NON_CHAR;
	}

