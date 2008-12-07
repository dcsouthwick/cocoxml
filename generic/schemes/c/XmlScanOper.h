/*-------------------------------------------------------------------------
 Copyright (c) 2008, Charles Wang <charlesw123456@gmail.com>

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
-------------------------------------------------------------------------*/
#ifndef  COCO_XMLSCANOPER_H
#define  COCO_XMLSCANOPER_H

#ifndef  COCO_CDEFS_H
#include "c/CDefs.h"
#endif

#include  <expat.h>

EXTC_BEGIN

typedef struct {
    const char * name;
    int kind;
    int kindEnd;
} CcxTag_t;

typedef struct {
    const char * name;
    int kind;
} CcxAttr_t;

typedef struct {
    const char * name;
    int kind;
} CcxPInstruction_t;

typedef struct {
    const char * nsURI;
    CcsBool_t caseSensitive;
    int kinds[XSO_SIZE];
    const CcxTag_t * firstTag;  /* The sorted tag list. */
    size_t numTags;
    const CcxAttr_t * firstAttr; /* The sorted attr list. */
    size_t numAttrs;
    const CcxPInstruction_t * firstPInstruction; /* The sorted PI list. */
    size_t numPInstructions;
} CcxSpec_t;

#define  SZ_SPECSTACK  256

typedef struct {
    CcsErrorPool_t * errpool;

    XML_Parser parser;
    FILE * fp;

    CcsToken_t * dummy;
    CcsToken_t * tokens;
    CcsToken_t * peek;

    const CcxSpec_t * specStack[SZ_SPECSTACK];
    const CcxSpec_t ** curSpecStack;

    /* Set by CcxScanner */
    int kindUnknownNS;
    const CcxSpec_t * firstXmlSpec;
    size_t numXmlSpecs;
}  CcxScanOper_t;

CcxScanOper_t * CcxScanOper(CcxScanOper_t * self, CcsErrorPool_t * errpool,
			    const char * filename);

void CcxScanOper_Destruct(CcxScanOper_t * self);
CcsToken_t * CcxScanOper_GetDummy(CcxScanOper_t * self);
CcsToken_t * CcxScanOper_Scan(CcxScanOper_t * self);
CcsToken_t * CcxScanOper_Peek(CcxScanOper_t * self);
void CcxScanOper_ResetPeek(CcxScanOper_t * self);
void CcxScanOper_IncRef(CcxScanOper_t * self, CcsToken_t * token);
void CcxScanOper_DecRef(CcxScanOper_t * self, CcsToken_t * token);

EXTC_END

#endif
