
#include "cells.h"

ptr& tex::rinfo(ptr P)
	{
	return ((mcell *)(P))->h1.i;
	}

ptr& tex::link(ptr P)
	{
	return ((mcell *)(P))->h1.p;
	}

ptr& tex::info(ptr P)
	{
	return ((mcell *)(P))->h2.i;
	}

ptr& tex::llink(ptr P)
	{
	return ((mcell *)(P))->h2.p;
	}

short& tex::type(ptr P)
	{
	return ((mcell *)(P))->h2.qq.b0;
	}

short& tex::subtype(ptr P)
	{
	return ((mcell *)(P))->h2.qq.b1;
	}

short& tex::q0(ptr P)
	{
	return ((mcell *)(P))->h1.qq.b0;
	}

short& tex::q1(ptr P)
	{
	return ((mcell *)(P))->h1.qq.b1;
	}

short& tex::q2(ptr P)
	{
	return ((mcell *)(P))->h2.qq.b0;
	}

short& tex::q3(ptr P)
	{
	return ((mcell *)(P))->h2.qq.b1;
	}

