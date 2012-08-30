
/*
 * $Id: str.h,v 1.4 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */


namespace tex {

	extern	str	null_str;
	
	extern	str	ASCII[];
	
	extern	str	cur_str;         // start of buffer
	extern	str	cur_str_end;     // end of buffer
	extern	str	cur_str_ptr;     // current manipulation point of buffer
	
#define STR_SIZE		4096
	
#define append_char(C) 		(*cur_str_ptr++ = C)
#define flush_char()		(decr(cur_str_ptr))
#define cur_length()		(cur_str_ptr - cur_str)
#define flush_str()		(cur_str_ptr = cur_str)
#define print_str()		{*cur_str_ptr = 0; print(cur_str);}

#define str_length(S)		(strlen(S))
#define str_eq(S1,S2)		(strcmp(S1,S2)==0)
#define str_eqn(S1,S2,N)	(memcmp(S1,S2,N)==0)
#define make_str_given(S)	(strcpy(new_str(str_length(S)), S))

	str	make_str();
	void	str_room(int i);
	str	new_str(int i);
	void	free_str(str s);
	void	_str_init();
	void	_str_init_once();
};
