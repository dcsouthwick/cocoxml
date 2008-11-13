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
#ifndef  COCO_LEXICAL_STATE_H
#define  COCO_LEXICAL_STATE_H

#ifndef  COCO_OBJECT_H
#include "Object.h"
#endif

EXTC_BEGIN

struct CcState_s {
    CcObject_t   base;
    CcAction_t * firstAction;
    CcSymbol_t * endOf;
    int          ctx;
};
extern const CcObjectType_t * stateType;

void CcState_AddAction(CcState_t * self, CcAction_t * act);
void CcState_DetachAction(CcState_t * self, CcAction_t * act);
int CcState_MeltWith(CcState_t * self, CcState_t * s);

CcsBool_t CcState_MakeUnique(CcState_t * self);
CcAction_t * CcState_FindAction(CcState_t * self, int ch);

EXTC_END

#endif /* COCO_LEXICAL_STATE_H */
