
/*
 * $Id: hyph.h,v 1.4 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

namespace tex {
	extern	ptr	ha;
	extern	ptr	hb;
	extern	fnt	hf;
	extern	int	hn;
	extern	int	hc[];
	extern	int	hu[];
	
	extern	int	l_hyf;
	extern	int	r_hyf;
	extern	int	cur_lang;
	
	extern	ptr	init_list;
	extern	bool	init_lig;
	extern	bool	init_lft;
	
	class trie_t {
		public:
			short	s;
			union {
					short	s;
					struct {
							byte	b0;
							byte	b1;
					} bb;
			} u_s;
	};

#define trie_op(T)		trie[T].u_s.bb.b0
#define trie_char(T)		trie[T].u_s.bb.b1
#define trie_back(T)		trie[T].u_s.s
#define trie_link(T)		trie[T].s

	extern	trie_t	*trie;
	
	extern	int	*op_start;
	extern	int	*hyf_distance;
	extern	int	*hyf_num;
	extern	int	*hyf_next;
	
	extern	int	hyph_count;
	extern	str	*hyph_word;
	extern	ptr	*hyph_list;
	
	
	extern	int	hyf[];
	extern	int	hyf_char;
	
	extern	int	hyphen_passed;
	
	
	extern	int	trie_op_ptr;
	extern	int	*trie_op_hash;
	extern	int	*trie_op_val;
	extern	int	*trie_op_lang;
	extern	int	*trie_op_used;
	
	
#define trie_root	trie_l[0]
	
	extern	int	trie_ptr;
	extern	int	trie_max;
	extern	int	*trie_c;
	extern	int	*trie_o;
	extern	int	*trie_l;
	extern	int	*trie_r;
	extern	int	*trie_hash;
	extern	int	*trie_min;
	
#define trie_ref	trie_hash
	
	extern	bool	*trie_taken;
	extern	bool	trie_not_ready;
	
	void  check_trie_ptr();
	void	new_hyph_exceptions();
	void	enter_hyph_exception(ptr p);
	void	init_hyph();
	void	try_hyph();
	void	hyphenate();
	int	reconstitute(int j, int n, int bchar, int hchar);
	int	new_trie_op(int d, int n, int v);
	int	trie_compress(int p);
	int	trie_node(int p);
	void	first_fit(int p);
	void	trie_fix(int p);
	void	trie_pack(int p);
	void	new_patterns();
	void	init_trie();
	void	init_pattern_memory();
	void	free_pattern_memory();
	void  help_patterns();
	void  help_hyph_lccode();
	void  help_hyph();
	void	_hyph_init();
	void	_hyph_init_once();
};
