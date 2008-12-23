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
#ifndef  COCO_COUTPUTSCHEME_H
#define  COCO_COUTPUTSCHEME_H

#ifndef  COCO_OUTPUTSCHEME_H
#include "OutputScheme.h"
#endif

#ifndef  COCO_SYNTAX_H
#incldue "Syntax.h"
#endif

#ifndef  COCO_PARSER_H
#include "c/Parser.h"
#endif

EXTC_BEGIN

typedef struct {
    CcOutputScheme_t base;

    CcsParser_t * parser;
    CcsXmlParser_t * xmlparser;

    const char * prefix;
    CcSyntaxSymSet_t symSet;
    const CcSymbol_t * curSy;
} CcCOutputScheme_t;

CcCOutputScheme_t *
CcCOutputScheme(CcsParser_t * parser, CcsXmlParser_t * xmlparser,
		CcArguments_t * arguments);

EXTC_END

#endif /* COCO_COUTPUTSCHEME_H */
