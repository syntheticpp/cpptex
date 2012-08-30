
/*
 * $Id: nodes.h,v 1.1 2002/04/19 11:21:10 t16 Exp $
 *
 * Copyright 2002           Kasper Peeters
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 * 
 */

// This file is currently unused. It contains preliminary class
// definitions for node types, necessary to clean up TeX's memory
// handling.

// We stick to TeX's concept of variable-length nodes, but instead
// of using a 'type' field, we give the classes different names.

class tex_node {
	public:
		tex_node *link;
};

class char_node : public tex_node {
	public:
		int8_t character;
		int8_t font;
};

class hlist_node : public tex_node {
	public:
		scal    width;
		scal    depth;
		scal    height;
		scal    shift_amount;
		void   *list_ptr;
		int8_t  glue_order;
		int8_t  glue_sign;
		int32_t glue_set;
};

class vlist_node : public tex_node {

};
