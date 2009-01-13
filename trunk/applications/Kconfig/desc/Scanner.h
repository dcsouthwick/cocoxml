/*---- license ----*/
/*-------------------------------------------------------------------------
Kconfig.atg
Copyright (C) 2008, Charles Wang
Author: Charles Wang  <charlesw123456@gmail.com>
License: LGPLv2
-------------------------------------------------------------------------*/
/*---- enable ----*/
#ifndef COCO_KcScanner_H
#define COCO_KcScanner_H

#ifndef  COCO_TOKEN_H
#include "c/Token.h"
#endif

#ifndef  COCO_BUFFER_H
#include "c/Buffer.h"
#endif

#ifndef  COCO_POSITION_H
#include "c/Position.h"
#endif

EXTC_BEGIN

/*---- defines ----*/
#define KcScanner_MAX_KEYWORD_LEN 14
#define KcScanner_CASE_SENSITIVE
#define KcScanner_KEYWORD_USED
#define KcScanner_INDENTATION
#define KcScanner_INDENT_START 32
#define KcScanner_INDENT_IN 1
#define KcScanner_INDENT_OUT 2
#define KcScanner_INDENT_ERR 3
/*---- enable ----*/

typedef struct KcScanInput_s KcScanInput_t;
typedef struct KcScanner_s KcScanner_t;
struct KcScanner_s {
    CcsErrorPool_t * errpool;
    int              eofSym;
    int              noSym;
    int              maxT;
    CcsToken_t     * dummyToken;
    KcScanInput_t * cur;
};

KcScanner_t *
KcScanner(KcScanner_t * self, CcsErrorPool_t * errpool, FILE * fp);
KcScanner_t *
KcScanner_ByName(KcScanner_t * self, CcsErrorPool_t * errpool,
		  const char * infn);
void KcScanner_Destruct(KcScanner_t * self);
CcsToken_t * KcScanner_GetDummy(KcScanner_t * self);
CcsToken_t * KcScanner_Scan(KcScanner_t * self);
CcsToken_t * KcScanner_Peek(KcScanner_t * self);
void KcScanner_ResetPeek(KcScanner_t * self);
void KcScanner_IncRef(KcScanner_t * self, CcsToken_t * token);
void KcScanner_DecRef(KcScanner_t * self, CcsToken_t * token);
#ifdef KcScanner_INDENTATION
/* If the col >= indentIn->col, not any IndentIn/IndentOut/IndentErr is generated.
 * Useful when we need to collect ANY text by indentation. */
void KcScanner_IndentLimit(KcScanner_t * self, const CcsToken_t * indentIn);
#endif

CcsPosition_t *
KcScanner_GetPosition(KcScanner_t * self, const CcsToken_t * begin,
		       const CcsToken_t * end);
CcsPosition_t *
KcScanner_GetPositionBetween(KcScanner_t * self, const CcsToken_t * begin,
			      const CcsToken_t * end);

CcsBool_t KcScanner_Include(KcScanner_t * self, FILE * fp);
CcsBool_t
KcScanner_IncludeByName(KcScanner_t * self, const CcsIncPathList_t * list,
			 const char * infn);

EXTC_END

#endif  /* COCO_KcScanner_H */