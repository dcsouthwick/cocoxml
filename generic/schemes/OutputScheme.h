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
#ifndef  COCO_OUTPUTSCHEME_H
#define  COCO_OUTPUTSCHEME_H

#ifndef  COCO_OBJECT_H
#include "Object.h"
#endif

#ifndef  COCO_GLOBALS_H
#include "Globals.h"
#endif

EXTC_BEGIN

typedef struct {
    FILE * outfp;
    int indent;
}  CcOutput_t;

void CcPrintf(CcOutput_t * self, const char * format, ...);
void CcPrintfI(CcOutput_t * self, const char * format, ...);
void CcSource(CcOutput_t * self, const CcsPosition_t * pos);

struct CcOutputSchemeType_s {
    CcObjectType_t base;

    CcsBool_t (* write)(CcOutputScheme_t * self, CcOutput_t * output,
			const char * func, const char * params);
};

struct CcOutputScheme_s {
    CcObject_t base;
    CcGlobals_t * globals;
    CcArguments_t * arguments;
};

CcOutputScheme_t *
CcOutputScheme(const CcOutputSchemeType_t * type, CcGlobals_t * globals,
	       CcArguments_t * arguments);
void CcOutputScheme_Destruct(CcObject_t * self);

CcsBool_t CcOutputScheme_GenerateOutputs(CcOutputScheme_t * self);

EXTC_END

#endif  /* COCO_OUTPUTSCHEME_H */
