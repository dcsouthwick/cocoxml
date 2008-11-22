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
#ifndef  COCO_ARGUMENTS_H
#define  COCO_ARGUMENTS_H

#ifndef  COCO_ARRAYLIST_H
#include  "ArrayList.h"
#endif

#ifndef  COCO_HASHTABLE_H
#include  "HashTable.h"
#endif

EXTC_BEGIN

struct CcArguments_s {
    CcArrayList_t storage;
    CcHashTable_t map;
};

CcArguments_t *
CcArguments(CcArguments_t * self, int argc, char * argv[]);
void CcArguments_Destruct(CcArguments_t * self);

typedef struct {
    const void * next;
}  CcArgumentsIter_t;

const char * CcArguments_First(CcArguments_t * self, const char * key,
			       CcArgumentsIter_t * iter);
const char * CcArguments_Next(CcArguments_t * self, CcArgumentsIter_t * iter);

EXTC_END

#endif  /* COCO_ARGUMENTS_H */
