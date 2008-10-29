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
#ifndef  COCO_DEFS_H
#define  COCO_DEFS_H

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#ifdef  __cplusplus
#define EXTC_BEGIN extern "C" {
#define EXTC_END   }
#else
#define EXTC_BEGIN
#define EXTC_END
#endif

EXTC_BEGIN

typedef int    Bool_t;
#define FALSE  0
#define TRUE   1

/* Basic DataStructures */
typedef struct ArrayList_s ArrayList_t;
typedef struct HashTable_s HashTable_t;

/* EBNF types */
typedef struct CsNodeType_s CsNodeType_t;
typedef struct CsNode_s  CsNode_t;
typedef struct CsGraph_s CsGraph_t;

extern const CsNodeType_t * node_alt;
extern const CsNodeType_t * node_iter;
extern const CsNodeType_t * node_opt;

#define CocoMalloc(size) _CocoMalloc_(size, __FILE__, __LINE__)
void * _CocoMalloc_(size_t size, const char * fname, int line);

#define CocoRealloc(ptr, size) _CocoRealloc_(ptr, size, __FILE__, __LINE__)
void * _CocoRealloc_(void * ptr, size_t size, const char * fname, int line);

#define CocoFree(ptr) _CocoFree_(ptr, __FILE__, __LINE__)
void _CocoFree_(void * ptr, const char * fname, int line);

#define CocoStrdup(str) _CocoStrdup_(str, __FILE__, __LINE__)
char * _CocoStrdup_(const char * str, const char * fname, int line);

#define AllocObject(self, szobj) _AllocObject_(self, szobj, __FILE__, __LINE__)
void * _AllocObject_(void * self, size_t szobj, const char * fname, int line);

EXTC_END

#endif /* COCO_DEFS_H */
