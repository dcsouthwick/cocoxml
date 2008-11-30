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
#ifndef  COCO_XMLSPEC_H
#define  COCO_XMLSPEC_H

#ifndef  COCO_OBJECT_H
#include  "Object.h"
#endif

#ifndef  COCO_ARRAYLIST_H
#include  "ArrayList.h"
#endif

EXTC_BEGIN

struct CcXmlSpec_s {
    CcObject_t base;

    char * nsURI;
    CcsBool_t options[XSO_SIZE];
    CcArrayList_t Tags;
    CcArrayList_t Attrs;
    CcArrayList_t PInstructions;
};

CcXmlSpec_t * CcXmlSpec(const char * nsURI);

void
CcXmlSpec_SetOption(CcXmlSpec_t * self, CcsXmlSpecOption_t option, int line);
void CcXmlSpec_AddTag(CcXmlSpec_t * self, const char * tagname, int line);
void CcXmlSpec_AddAttr(CcXmlSpec_t * self, const char * attrname, int line);
void CcXmlSpec_AddProcessInstruction(CcXmlSpec_t * self,
				     const char * pinstruction, int line);

void CcXmlSpec_MakeTerminals(CcXmlSpec_t * self, CcSymbolTable_t * symtab);

EXTC_END

#endif