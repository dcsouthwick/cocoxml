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
#define KcScanner_MAX_KEYWORD_LEN 0
#define KcScanner_CASE_SENSITIVE
#define KcScanner_INDENTATION
#define KcScanner_INDENT_START 32
#define KcScanner_INDENT_IN 0
#define KcScanner_INDENT_OUT 0
#define KcScanner_INDENT_ERR 0
/*---- enable ----*/

typedef struct KcScanner_s KcScanner_t;
struct KcScanner_s {
    CcsErrorPool_t * errpool;

    int            eofSym;
    int            noSym;
    int            maxT;

    CcsToken_t   * dummyToken;

    CcsToken_t   * busyTokenList;
    CcsToken_t  ** curToken;
    CcsToken_t  ** peekToken;

    int            ch;
    int            chBytes;
    int            pos;
    int            line;
    int            col;
    int            oldEols;
    int            oldEolsEOL;

    CcsBuffer_t    buffer;
#ifdef KcScanner_INDENTATION
    CcsBool_t      lineStart;
    int          * indent;
    int          * indentUsed;
    int          * indentLast;
#endif
};

KcScanner_t *
KcScanner(KcScanner_t * self, CcsErrorPool_t * errpool, FILE * fp);
void KcScanner_Destruct(KcScanner_t * self);
CcsToken_t * KcScanner_GetDummy(KcScanner_t * self);
CcsToken_t * KcScanner_Scan(KcScanner_t * self);
CcsToken_t * KcScanner_Peek(KcScanner_t * self);
void KcScanner_ResetPeek(KcScanner_t * self);
void KcScanner_IncRef(KcScanner_t * self, CcsToken_t * token);
void KcScanner_DecRef(KcScanner_t * self, CcsToken_t * token);

CcsPosition_t *
KcScanner_GetPosition(KcScanner_t * self, const CcsToken_t * begin,
		       const CcsToken_t * end);
CcsPosition_t *
KcScanner_GetPositionBetween(KcScanner_t * self, const CcsToken_t * begin,
			      const CcsToken_t * end);

EXTC_END

#endif  /* COCO_KcScanner_H */
