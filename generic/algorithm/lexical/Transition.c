/*-------------------------------------------------------------------------
  Author (C) 2008, Charles Wang <charlesw123456@gmail.com>

  This program is free software; you can redistribute it and/or modify it 
  under the terms of the GNU General Public License as published by the 
  Free Software Foundation; either version 2, or (at your option) any 
  later version.

  This program is distributed in the hope that it will be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
  for more details.

  You should have received a copy of the GNU General Public License along 
  with this program; if not, write to the Free Software Foundation, Inc., 
  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

  As an exception, it is allowed to write an extension of Coco/R that is
  used as a plugin in non-free software.

  If not otherwise stated, any source code generated by Coco/R (other than 
  Coco/R itself) does not fall under the GNU General Public License.
-------------------------------------------------------------------------*/
#include  "ArrayList.h"
#include  "lexical/Transition.h"
#include  "lexical/CharSet.h"
#include  "lexical/CharClass.h"

CcTransition_t *
CcTransition(CcTransition_t * self, int chr, CcTransitionCode_t code,
	     CcArrayList_t * classes)
{
    self->classes = classes;
    self->code = code;
    self->single = TRUE;
    self->u.chr = chr;
    return self;
}

CcTransition_t *
CcTransition_FromCharSet(CcTransition_t * self, const CcCharSet_t * s,
			 CcTransitionCode_t code, CcArrayList_t * classes)
{

    self->classes = classes;
    self->code = code;
    CcTransition_SetCharSet(self, s);
    return self;
}

CcTransition_t *
CcTransition_Clone(CcTransition_t * self, const CcTransition_t * t)
{
    self->classes = t->classes;
    self->code = t->code;
    self->single = t->single;
    if (self->single) self->u.chr = t->u.chr;
    else self->u.set = t->u.set;
    return self;
}

int
CcTransition_Size(const CcTransition_t * self)
{
    if (self->single) return 1;
    return CcCharSet_Elements(self->u.set);
}

int
CcTransition_First(const CcTransition_t * self)
{
    if (self->single) return self->u.chr;
    return CcCharSet_First(self->u.set);
}

CcCharSet_t *
CcTransition_GetCharSet(const CcTransition_t * self)
{
    CcCharSet_t * s;
    if (self->single) {
	s = CcCharSet();
	CcCharSet_Set(s, self->u.chr);
    } else {
	s = CcCharSet_Clone(self->u.set);
    }
    return s;
}

void
CcTransition_SetCharSet(CcTransition_t * self, const CcCharSet_t * s)
{
    CcCharClass_t * c; CcArrayListIter_t iter;
    if (CcCharSet_Elements(s) == 1) {
	self->single = TRUE;
	self->u.chr = CcCharSet_First(s);
	return;
    }
    self->single = FALSE;
    for (c = (CcCharClass_t *)CcArrayList_First(self->classes, &iter);
	 c; c = (CcCharClass_t *)CcArrayList_Next(self->classes, &iter))
	if (CcCharSet_Equals(s, c->set)) break;
    if (!c)
	c = (CcCharClass_t *)
	    CcArrayList_New(self->classes, char_class, "#", CcCharSet_Clone(s));
    self->u.set = c->set;
}

void
CcTransition_SetCode(CcTransition_t * self, CcTransitionCode_t code)
{
    self->code = code;
}

CcsBool_t
CcTransition_Check(const CcTransition_t * self, int chr)
{
    if (self->single) return chr == self->u.chr;
    return CcCharSet_Get(self->u.set, chr);
}

CcsBool_t
CcTransition_Overlap(const CcTransition_t * a,const CcTransition_t * b)
{
    if (a->single) {
	if (b->single) return a->u.chr == b->u.chr;
	return CcCharSet_Get(b->u.set, a->u.chr);
    } else {
	if (b->single) return CcCharSet_Get(a->u.set, b->u.chr);
	return CcCharSet_Intersects(a->u.set, b->u.set);
    }
}

void
CcTransition_Destruct(CcTransition_t * self)
{
    /* NOTHING IS OK */
}