
/*
 * $Id: error.c,v 1.12 2003/09/14 13:30:19 kp229 Exp $
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <stdlib.h>
#include "tex.h"

tex::error::error()
	: line_start(0), line_end(0)
	{
	}

tex::error::error(const char *msg_) 
	: line_start(0), line_end(0), msg(msg_)
	{
	}

std::ostream& tex::operator<<(std::ostream& str, const error& err)
	{
	if(err.line_start) {
		if(err.line_end)
			str << std::setw(5) << err.line_start << "-" << std::setw(5) << err.line_end;
		else 
			str << "      " << std::setw(5) << err.line_start;
		str << ": " << err.msg.str() << std::endl;
		}
	return str;
	}

std::ostream& tex::operator<<(std::ostream& str, const error& err)
	{
	if(err.line_start) {
		if(err.line_end)
			str << std::setw(5) << err.line_start << "-" << std::setw(5) << err.line_end;
		else 
			str << "      " << std::setw(5) << err.line_start;
		str << ": " << err.msg.str() << std::endl;
		}
	str << "capacity exceeded" << std::endl;
	return str;
	}





