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

#include  <glib/gtypes.h>

#ifdef  __cplusplus
#define  EXTC_BEGIN  extern "C" {
#else
#define  EXTC_BEGIN
#endif

#ifdef  __cplusplus
#define  EXTC_END    }
#else
#define  EXTC_END
#endif

EXTC_BEGIN

typedef struct BitArray_s  BitArray_t;

typedef struct State_s     State_t;
typedef struct Action_s    Action_t;
typedef struct Target_s    Target_t;
typedef struct Melted_s    Melted_t;
typedef struct Comment_s   Comment_t;
typedef struct CharSet_s   CharSet_t;
typedef struct DFA_s       DFA_t;

typedef struct Position_s  Position_t;
typedef struct Symbol_s    Symbol_t;
typedef struct Node_s      Node_t;
typedef struct Graph_t     Graph_t;
typedef struct CharClass_s CharClass_t;
typedef struct Tab_s       Tab_t;

typedef struct Scanner_s   Scanner_t;
typedef struct Parser_s    Parser_t;
typedef struct Errors_s    Errors_t;

typedef enum {
    symbol_fixedToken = 0, symbol_classToken = 1,
    symbol_litToken = 2, symbol_classLitToken = 3
}  SymbolToken_t;

typedef enum {
    node_t = 1, node_pr = 2, node_nt = 3, node_clas = 4, node_chr = 5,
    node_wt = 6, node_any = 7, node_eps = 8, node_sync = 9, node_sem = 10,
    node_alt = 11, node_iter = 12, node_opt = 13, node_rslv = 14
}  NodeKind_t;

typedef enum {
    node_normalTrans = 0, node_contextTrans = 1
}  NodeTrans_t;

typedef struct {
    char * start;
    char * cur;
    char * last;
}  DumpBuffer_t;

void DumpBuffer(DumpBuffer_t * self, char * buf, size_t szbuf);
void DumpBuffer_Print(DumpBuffer_t * self, const char * format, ...);
gboolean DumpBuffer_Full(DumpBuffer_t * self);

void Unescape(DumpBuffer_t * buf, const char * s);
void EscapeCh(DumpBuffer_t * buf, int ch);
void Escape(DumpBuffer_t * buf, const char * s);

EXTC_END

#endif  /* COCO_DEFS_H */
