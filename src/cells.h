
#ifndef cells_h_included
#define cells_h_included

#include "textypes.h"

namespace tex {
	class qcell {
		public:
			int16_t	b0;
			int16_t	b1;
			int16_t	b2;
			int16_t	b3;
	};
	
	union hcell { 
			int	i;   
			ptr	p;
			struct {
					int16_t	b0;
					int16_t	b1;
			} qq;
	};
	typedef union hcell hcell;
	
	class mcell { 
		public:
			hcell	h1;
			hcell	h2;
	};
	
	ptr& rinfo(ptr P);
	ptr& link(ptr P);
	ptr& info(ptr P);
	ptr& llink(ptr P);
	int16_t& type(ptr P);
	int16_t& subtype(ptr P);
	int16_t& q0(ptr P);
	int16_t& q1(ptr P);
	int16_t& q2(ptr P);
	int16_t& q3(ptr P);
};

#endif
