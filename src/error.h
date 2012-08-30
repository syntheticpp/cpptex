
/*
 * $Id: error.h,v 1.12 2003/09/23 08:08:36 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#ifndef error_h_included
#define error_h_included

#include <string>
#include <iostream>

// Warnings which do not interrupt the program should be streamed into
// the errstr class; we'll deal with verbosity flags later.

// Fatal errors should be handled by throwing an exception of
// type 'tex::error'; the program cannot be resumed after that.

namespace tex {
	class error {
		public:
			error();
			error(const char *);
	
			int line_start;
			int line_end;

			std::string msg;
	};
	class overflow_error : public error {
		public:
			overflow_error(const char *, int);

			std::string memory_area;
			int         current_size;
	};
	// for errors during reading of binary files (e.g. fonts)
	class file_error : public error {
		public:
			file_error(const char *);
	};

	std::ostream& operator<<(std::ostream& str, const error& err);
};

#endif
