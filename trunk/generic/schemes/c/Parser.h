/*---- license ----*/
/*-------------------------------------------------------------------------
 Coco.ATG -- Attributed Grammar
 Compiler Generator Coco/R,
 Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
 extended by M. Loeberbauer & A. Woess, Univ. of Linz
 with improvements by Pat Terry, Rhodes University.
 ported to C by Charles Wang <charlesw123456@gmail.com>

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
/*---- enable ----*/
/*---- prefix(Ccs) ----*/
#ifndef  COCO_PARSER_H
#define  COCO_PARSER_H

#ifndef  COCO_CDEFS_H
#include  "CDefs.h"
#endif

/*---- hIncludes ----*/
#ifndef  COCO_DEFS_H
#include  "Defs.h"
#endif
/*---- enable ----*/

EXTC_BEGIN

struct CcsParser_s {
    CcsGlobals_t    * globals;
    CcsScanner_t    * scanner;
    CcsToken_t      * t;
    CcsToken_t      * la;
    int               maxT;
    /*---- members ----*/
    char            * tokenString;
    CcsBool_t         genScanner;
    char            * schemeName;
    char            * prefix;
    CcsPosition_t   * members;
    CcsPosition_t   * constructor;
    CcsPosition_t   * destructor;
    /* Shortcut pointers */
    CcSymbolTable_t * symtab;
    CcLexical_t     * lexical;
    CcSyntax_t      * syntax;
    /*---- enable ----*/
};

CcsParser_t * CcsParser(CcsParser_t * self, CcsGlobals_t * globals);
void CcsParser_Destruct(CcsParser_t * self);
void CcsParser_Parse(CcsParser_t * self);

EXTC_END

#endif  /* COCO_PARSER_H */
