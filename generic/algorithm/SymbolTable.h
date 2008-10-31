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
#ifndef  COCO_SYMBOLTABLE_H
#define  COCO_SYMBOLTABLE_H

#ifndef  COCO_OBJECT_H
#include "Object.h"
#endif

#ifndef  COCO_ARRAYLIST_H
#include "ArrayList.h"
#endif

#ifndef  COCO_BITARRAY_H
#include "BitArray.h"
#endif

#ifndef  COCO_POSITION_H
#include "c/Position.h"
#endif

struct CcSymbolType_s {
    CcObjectType_t base;
};

struct CcSymbol_s {
    CcObject_t base;
    char * name;
    int line;
};

typedef struct {
    CcSymbol_t base;
    CcSymbolT_TokenKind_t tokenKind;
} CcSymbolT_t;

typedef struct {
    CcSymbol_t      base;
    CcNode_t      * graph;
    CcsBool_t       deletable;
    CcsBool_t       firstReady;
    CcBitArray_t    first;
    CcBitArray_t    follow;
    CcBitArray_t    nts;
    CcsPosition_t * attrPos;
} CcSymbolNT_t;

typedef struct {
    CcSymbol_t      base;
    CcsPosition_t * semPos;
} CcSymbolPR_t;

struct CcSymbolTable_s {
    CcArrayList_t terminals;
    CcArrayList_t pragmas;
    CcArrayList_t nonterminals;
};

CcSymbolTable_t * CcSymbolTable(CcSymbolTable_t * self);
void CcSymbolTable_Destruct(CcSymbolTable_t * self);

CcSymbol_t *
CcSymbolTable_NewTerminal(CcSymbolTable_t * self, const char * name, int line);

CcSymbol_t *
CcSymbolTable_NewNonTerminal(CcSymbolTable_t * self,
			     const char * name, int line);

CcSymbol_t *
CcSymbolTable_Pragma(CcSymbolTable_t * self, const char * name, int line);

#endif  /* COCO_SYMBOLTABLE_H */
