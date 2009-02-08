/*-------------------------------------------------------------------------
  Copyright (C) 2008, Charles Wang
  Author: Charles Wang <charlesw123456@gmail.com>
  License: LGPLv2 (see LICENSE-LGPL)
-------------------------------------------------------------------------*/
#ifndef  COCO_XMLSCANOPER_H
#define  COCO_XMLSCANOPER_H

#ifndef  COCO_CDEFS_H
#include "c/CDefs.h"
#endif

/* This is an expat version, we can implement other version for different
 * SAX parser. */
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

/* SZ_TEXTBUF is the initialize size, it may be extended
 * automatically if required. */
#define  SZ_TEXTBUF    256
#define  SZ_STACK      256

typedef struct {
    const CcxSpec_t * spec;
    const CcxTag_t * tag;
}  CcxScanStack_t;

typedef struct {
    CcsErrorPool_t * errpool;
    char * fname;
    FILE * fp;

    XML_Parser parser;

    CcsBool_t EOFGenerated;

    CcsToken_t * dummy;
    CcsToken_t * tokens;
    CcsToken_t * peek;

    char * textStart;
    char * textUsed;
    size_t textSpace;

    int kindText;
    int kindWhitespace;
    int kindComment;
    CcxScanStack_t stack[SZ_STACK];
    CcxScanStack_t * effect;
    CcxScanStack_t * cur;

    /* Set by CcxScanner */
    int kindUnknownNS;
    const CcxSpec_t * firstXmlSpec;
    size_t numXmlSpecs;
}  CcxScanOper_t;

CcxScanOper_t *
CcxScanOper(CcxScanOper_t * self, CcsErrorPool_t * errpool, FILE * infp);
CcxScanOper_t *
CcxScanOper_ByName(CcxScanOper_t * self, CcsErrorPool_t * errpool,
		   const char * infn);
void CcxScanOper_Destruct(CcxScanOper_t * self);

CcsToken_t * CcxScanOper_GetDummy(CcxScanOper_t * self);
CcsToken_t * CcxScanOper_Scan(CcxScanOper_t * self);
CcsToken_t * CcxScanOper_Peek(CcxScanOper_t * self);
void CcxScanOper_ResetPeek(CcxScanOper_t * self);
void CcxScanOper_IncRef(CcxScanOper_t * self, CcsToken_t * token);
void CcxScanOper_DecRef(CcxScanOper_t * self, CcsToken_t * token);

EXTC_END

#endif
