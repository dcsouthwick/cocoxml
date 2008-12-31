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
#ifndef  COCO_BITARRAY_H
#define  COCO_BITARRAY_H

#ifndef  COCO_DEFS_H
#include  "Defs.h"
#endif

EXTC_BEGIN

struct CcBitArray_s {
    int numbits;
    unsigned char * data;
};

CcBitArray_t * CcBitArray(CcBitArray_t * self, int numbits);
CcBitArray_t * CcBitArray1(CcBitArray_t * self, int numbits);
CcBitArray_t * CcBitArray_Clone(CcBitArray_t * self, const CcBitArray_t * value);
void CcBitArray_Destruct(CcBitArray_t * self);

/* Return -1 for error. */
int CcBitArray_getCount(const CcBitArray_t * self);
int CcBitArray_Elements(const CcBitArray_t * self);
CcsBool_t CcBitArray_Get(const CcBitArray_t * self, int index);
void CcBitArray_Set(CcBitArray_t * self, int index, CcsBool_t value);
void CcBitArray_SetAll(CcBitArray_t * self, CcsBool_t value);
CcsBool_t CcBitArray_Equal(const CcBitArray_t * self1, const CcBitArray_t * self2);
void CcBitArray_Not(CcBitArray_t * self);
int CcBitArray_And(CcBitArray_t * self, const CcBitArray_t * value);
int CcBitArray_Or(CcBitArray_t * self, const CcBitArray_t * value);
int CcBitArray_Xor(CcBitArray_t * self, const CcBitArray_t * value);
CcsBool_t CcBitArray_Intersect(const CcBitArray_t * self1, const CcBitArray_t * self2);
void CcBitArray_Subtract(CcBitArray_t * self, const CcBitArray_t * b);

EXTC_END

#endif  /* COCO_BITARRAY_H */