/*-------------------------------------------------------------------------
  Copyright (C) 2008, Charles Wang
  Author: Charles Wang <charlesw123456@gmail.com>
  License: GPLv2 (see LICENSE-GPL)
-------------------------------------------------------------------------*/
#include  "CBaseOutputScheme.h"
#include  "syntax/Nodes.h"

/* When the number of possible terminals is greater than maxTerm,
   symSet is used. */
#define  maxTerm  3

CcCBaseOutputScheme_t *
CcCBaseOutputScheme(const CcOutputSchemeType_t * type, CcGlobals_t * globals,
		    CcArguments_t * arguments)
{
    CcCBaseOutputScheme_t * self = (CcCBaseOutputScheme_t *)
	CcOutputScheme(type, globals, arguments);
    self->prefix = globals->syntax.grammarPrefix;
    if (self->prefix == NULL) self->prefix = "";
    CcSyntaxSymSet(&self->symSet);
    CcSyntaxSymSet_New(&self->symSet, self->base.globals->syntax.allSyncSets);
    self->useStartOf = FALSE;
    self->useGetSS = FALSE;
    self->useExpectSS = FALSE;
    self->useExpectWeak = FALSE;
    self->useWeakSeparator = FALSE;
    return self;
}

static CcsBool_t
COS_SynDefines(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    if (self->useStartOf)
	CcPrintfIL(output, "#define %sParser_USE_StartOf", self->prefix);
    if (self->useGetSS)
	CcPrintfIL(output, "#define %sParser_USE_GetSS", self->prefix);
    if (self->useExpectSS)
	CcPrintfIL(output, "#define %sParser_USE_ExpectSS", self->prefix);
    if (self->useExpectWeak)
	CcPrintfIL(output, "#define %sParser_USE_ExpectWeak", self->prefix);
    if (self->useWeakSeparator)
	CcPrintfIL(output, "#define %sParser_USE_WeakSeparator", self->prefix);
    return TRUE;
}

static CcsBool_t
COS_Members(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    if (self->base.globals->syntax.members)
	CcSource(output, self->base.globals->syntax.members);
    return TRUE;
}

static CcsBool_t
COS_Constructor(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    CcPrintfIL(output, "self->maxT = %d;",
	      self->base.globals->symtab.terminals.Count - 1);
    if (self->base.globals->syntax.constructor)
	CcSource(output, self->base.globals->syntax.constructor);
    return TRUE;
}

static CcsBool_t
COS_Destructor(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    if (self->base.globals->syntax.destructor)
	CcSource(output, self->base.globals->syntax.destructor);
    return TRUE;
}

static CcsBool_t
COS_Pragmas(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    CcArrayListIter_t iter;
    const CcSymbolPR_t * sym, * sym1;
    const CcArrayList_t * pragmas = &self->base.globals->symtab.pragmas;

    for (sym = sym1 = (const CcSymbolPR_t *)CcArrayList_FirstC(pragmas, &iter);
	 sym; sym = (const CcSymbolPR_t *)CcArrayList_NextC(pragmas, &iter)) {
	CcPrintfIL(output, "%sif (self->la->kind == %d) {",
		   (sym == sym1) ? "" : "} else ", sym->base.kind);
	if (sym->semPos) {
	    output->indent += 4;
	    CcSource(output, sym->semPos);
	    output->indent -= 4;
	}
    }
    if (sym1) CcPrintfIL(output, "}");
    return TRUE;
}

static CcsBool_t
COS_ProductionsHeader(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    CcArrayListIter_t iter;
    const CcSymbolNT_t * sym;
    const CcArrayList_t * nonterminals =
	&self->base.globals->symtab.nonterminals;

    for (sym = (const CcSymbolNT_t *)CcArrayList_FirstC(nonterminals, &iter);
	 sym;
	 sym = (const CcSymbolNT_t *)CcArrayList_NextC(nonterminals, &iter))
	if (sym->attrPos)
	    CcPrintfIL(output,
		       "static void %sParser_%s(%sParser_t * self, %s);",
		       self->prefix, sym->base.name,
		       self->prefix, sym->attrPos->text);
	else
	    CcPrintfIL(output, "static void %sParser_%s(%sParser_t * self);",
		       self->prefix, sym->base.name, self->prefix);
    return TRUE;
}

static CcsBool_t
COS_ParseRoot(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    CcPrintfIL(output, "%sParser_%s(self);", self->prefix,
	       self->base.globals->syntax.gramSy->name);
    return TRUE;
}

static void
SCOS_GenCond(CcCBaseOutputScheme_t * self, CcOutput_t * output,
	     const char * prefix, const char * suffix,
	     const CcBitArray_t * s, const CcNode_t * p)
{
    const CcNodeRSLV_t * prslv; int n;
    CcArrayListIter_t iter; const CcSymbol_t * sym;
    const CcArrayList_t * terminals;

    if (p->base.type == node_rslv) {
	prslv = (CcNodeRSLV_t *)p;
	CcPrintfIL(output, "%s%s%s", prefix, prslv->pos->text, suffix);
    } else if ((n = CcBitArray_Elements(s)) == 0) {
	CcPrintfIL(output, "%s%s%s", prefix, "FALSE", suffix);
    } else if (n <= maxTerm) {
	CcPrintfI(output, "%s", prefix);
	terminals = &self->base.globals->symtab.terminals;
	for (sym = (const CcSymbol_t *)CcArrayList_FirstC(terminals, &iter);
	     sym; sym = (const CcSymbol_t *)CcArrayList_NextC(terminals, &iter))
	    if (CcBitArray_Get(s, sym->kind)) {
		CcPrintf(output, "self->la->kind == %d", sym->kind);
		if (--n > 0) CcPrintf(output, " || ");
	    }
	CcPrintfL(output, "%s", suffix);
    } else {
	self->useStartOf = TRUE;
	CcPrintfIL(output, "%s%sParser_StartOf(self, %d)%s",
		   prefix, self->prefix,
		   CcSyntaxSymSet_New(&self->symSet, s), suffix);
    }
}

static CcsBool_t
SCOS_UseSwitch(CcCBaseOutputScheme_t * self, CcNode_t * p)
{
    CcBitArray_t s1, s2; int nAlts;
    CcSyntax_t * syntax = &self->base.globals->syntax;
    CcArrayList_t * terminals = &self->base.globals->symtab.terminals;

    if (p->base.type != node_alt) return FALSE;
    nAlts = 0;
    CcBitArray(&s1, terminals->Count);
    while (p != NULL) {
	CcSyntax_Expected0(syntax, &s2, p->sub, self->curSy);
	if (CcBitArray_Intersect(&s1, &s2)) goto falsequit2;
	CcBitArray_Or(&s1, &s2);
	CcBitArray_Destruct(&s2);
	++nAlts;
	if (p->sub->base.type == node_rslv) goto falsequit1;
	p = p->down;
    }
    CcBitArray_Destruct(&s1);
    return nAlts > 5;
 falsequit2:
    CcBitArray_Destruct(&s2);
 falsequit1:
    CcBitArray_Destruct(&s1);
    return FALSE;
}

static void
SCOS_GenCode(CcCBaseOutputScheme_t * self, CcOutput_t * output,
	     CcNode_t * p, const CcBitArray_t * IsChecked)
{
    int err; CcsBool_t equal, useSwitch; int index;
    CcNode_t * p2; CcBitArray_t s1, s2, isChecked;
    CcNodeNT_t * pnt; CcNodeT_t * pt; CcNodeWT_t * pwt;
    CcNodeSEM_t * psem; CcNodeSYNC_t * psync;
    CcSyntax_t * syntax = &self->base.globals->syntax;
    CcArrayList_t * terminals = &self->base.globals->symtab.terminals;

    CcBitArray_Clone(&isChecked, IsChecked);
    while (p != NULL) {
	if (p->base.type == node_nt) {
	    pnt = (CcNodeNT_t *)p;
	    if (pnt->pos) {
		CcPrintfIL(output, "%sParser_%s(self, %s);",
			   self->prefix, pnt->sym->name, pnt->pos->text);
	    } else {
		CcPrintfIL(output, "%sParser_%s(self);",
			   self->prefix, pnt->sym->name);
	    }
	} else if (p->base.type == node_t) {
	    pt = (CcNodeT_t *)p;
	    if (CcBitArray_Get(&isChecked, pt->sym->kind)) {
		if (pt->pos) {
		    self->useGetSS = TRUE;
		    CcPrintfIL(output, "%sParser_GetSS(self, %sSubScanner_%s);",
			       self->prefix, self->prefix, pt->pos->text);
		} else {
		    CcPrintfIL(output, "%sParser_Get(self);", self->prefix);
		}
	    } else {
		if (pt->pos) {
		    self->useGetSS = self->useExpectSS = TRUE;
		    CcPrintfIL(output, "%sParser_ExpectSS(self, %d, %sSubScanner_%s);",
			       self->prefix, pt->sym->kind, self->prefix, pt->pos->text);
		} else {
		    CcPrintfIL(output, "%sParser_Expect(self, %d);",
			       self->prefix, pt->sym->kind);
		}
	    }
	} else if (p->base.type == node_wt) {
	    pwt = (CcNodeWT_t *)p;
	    CcSyntax_Expected(syntax, &s1, p->next, self->curSy);
	    CcBitArray_Or(&s1, syntax->allSyncSets);
	    self->useExpectWeak = TRUE;
	    CcPrintfIL(output, "%sParser_ExpectWeak(self, %d, %d);",
		       self->prefix, pwt->sym->kind,
		       CcSyntaxSymSet_New(&self->symSet, &s1));
	    CcBitArray_Destruct(&s1);
	} else if (p->base.type == node_any) {
	    CcPrintfIL(output, "%sParser_Get(self);", self->prefix);
	} else if (p->base.type == node_eps) {
	} else if (p->base.type == node_rslv) {
	} else if (p->base.type == node_sem) {
	    psem = (CcNodeSEM_t *)p;
	    CcSource(output, psem->pos);
	} else if (p->base.type == node_sync) {
	    psync = (CcNodeSYNC_t *)p;
	    err = CcSyntax_SyncError(syntax, self->curSy);
	    CcBitArray_Clone(&s1, psync->set);
	    SCOS_GenCond(self, output, "while (!(", ")) {", &s1, p);
	    output->indent += 4;
	    CcPrintfIL(output, "%sParser_SynErr(self, %d); %sParser_Get(self);",
		       self->prefix, err, self->prefix);
	    output->indent -= 4;
	    CcPrintfIL(output, "}");
	    CcBitArray_Destruct(&s1);
	} else if (p->base.type == node_alt) {
	    CcSyntax_First(syntax, &s1, p);
	    equal = CcBitArray_Equal(&s1, &isChecked);
	    CcBitArray_Destruct(&s1);
	    useSwitch = SCOS_UseSwitch(self, p);
	    if (useSwitch)
		CcPrintfIL(output, "switch (self->la->kind) {");
	    p2 = p;
	    while (p2 != NULL) {
		CcSyntax_Expected(syntax, &s1, p2->sub, self->curSy);
		if (useSwitch) {
		    CcPrintfI(output, "");
		    for (index = 0; index < terminals->Count; ++index)
			if (CcBitArray_Get(&s1, index))
			    CcPrintf(output, "case %d: ", index);
		    CcPrintfL(output,"{");
		} else if (p2 == p) {
		    SCOS_GenCond(self, output, "if (", ") {", &s1, p2->sub);
		} else if (p2->down == NULL && equal) {
		    CcPrintfIL(output, "} else {");
		} else {
		    SCOS_GenCond(self, output,
				 "} else if (", ") {", &s1, p2->sub);
		}
		CcBitArray_Or(&s1, &isChecked);
		output->indent += 4;
		SCOS_GenCode(self, output, p2->sub, &s1);
		if (useSwitch) CcPrintfIL(output, "break;");
		output->indent -= 4;
		if (useSwitch) CcPrintfIL(output, "}");
		p2 = p2->down;
		CcBitArray_Destruct(&s1);
	    }
	    if (equal) {
		CcPrintfIL(output, "}");
	    } else {
		err = CcSyntax_AltError(syntax, self->curSy);
		if (useSwitch) {
		    CcPrintfIL(output,
			       "default: %sParser_SynErr(self, %d); break;",
			       self->prefix, err);
		    CcPrintfIL(output, "}");
		} else {
		    CcPrintfIL(output, "} else %sParser_SynErr(self, %d);",
			       self->prefix, err);
		}
	    }
	} else if (p->base.type == node_iter) {
	    p2 = p->sub;
	    if (p2->base.type == node_wt) {
		CcSyntax_Expected(syntax, &s1, p2->next, self->curSy);
		CcSyntax_Expected(syntax, &s2, p->next, self->curSy);
		self->useWeakSeparator = TRUE;
		CcPrintfIL(output,
			   "while (%sParser_WeakSeparator(self, %d, %d, %d)) {",
			   self->prefix, ((CcNodeWT_t *)p2)->sym->kind,
			   CcSyntaxSymSet_New(&self->symSet, &s1),
			   CcSyntaxSymSet_New(&self->symSet, &s2));
		CcBitArray_Destruct(&s1); CcBitArray_Destruct(&s2);
		CcBitArray(&s1, terminals->Count);
		if (p2->up || p2->next == NULL) p2 = NULL; else p2 = p2->next;
	    } else {
		CcSyntax_First(syntax, &s1, p2);
		SCOS_GenCond(self, output, "while (", ") {", &s1, p2);
	    }
	    output->indent += 4;
	    SCOS_GenCode(self, output, p2, &s1);
	    output->indent -= 4;
	    CcPrintfIL(output, "}");
	    CcBitArray_Destruct(&s1);
	} else if (p->base.type == node_opt) {
	    CcSyntax_First(syntax, &s1, p->sub);
	    SCOS_GenCond(self, output, "if (", ") {", &s1, p->sub);
	    output->indent += 4;
	    SCOS_GenCode(self, output, p->sub, &s1);
	    output->indent -= 4;
	    CcPrintfIL(output, "}");
	    CcBitArray_Destruct(&s1);
	}
	if (p->base.type != node_eps && p->base.type != node_sem &&
	    p->base.type != node_sync)
	    CcBitArray_SetAll(&isChecked, FALSE);
	if (p->up) break;
	p = p->next;
    }
    CcBitArray_Destruct(&isChecked);
}

static CcsBool_t
COS_ProductionsBody(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    CcBitArray_t isChecked;
    CcArrayListIter_t iter;
    const CcSymbolNT_t * sym;
    const CcArrayList_t * nonterminals =
	&self->base.globals->symtab.nonterminals;

    CcBitArray(&isChecked, self->base.globals->symtab.terminals.Count);
    for (sym = (const CcSymbolNT_t *)CcArrayList_FirstC(nonterminals, &iter);
	 sym;
	 sym = (const CcSymbolNT_t *)CcArrayList_NextC(nonterminals, &iter)) {
	self->curSy = (const CcSymbol_t *)sym;
	if (sym->attrPos == NULL) {
	    CcPrintfIL(output, "static void");
	    CcPrintfIL(output, "%sParser_%s(%sParser_t * self)",
		       self->prefix, sym->base.name,
		       self->prefix);
	} else {
	    CcPrintfIL(output, "static void");
	    CcPrintfIL(output, "%sParser_%s(%sParser_t * self, %s)",
		       self->prefix, sym->base.name,
		       self->prefix, sym->attrPos->text);
	}
	CcPrintfIL(output, "{");
	output->indent += 4;
	if (sym->semPos) CcSource(output, sym->semPos);
	SCOS_GenCode(self, output, sym->graph, &isChecked);
	output->indent -= 4;
	CcPrintfIL(output, "}");
	CcPrintfL(output, "");
    }
    CcBitArray_Destruct(&isChecked);
    return TRUE;
}

static CcsBool_t
COS_SynErrors(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    CcArrayListIter_t iter; char * str;
    const CcSyntaxError_t * synerr;
    const CcArrayList_t * errors = &self->base.globals->syntax.errors;
    for (synerr = (const CcSyntaxError_t *)CcArrayList_FirstC(errors, &iter);
	 synerr;
	 synerr = (const CcSyntaxError_t *)CcArrayList_NextC(errors, &iter)) {
	CcPrintfI(output, "case %d: s = \"", synerr->base.index);
	str = CcEscape(synerr->sym->name);
	switch (synerr->type) {
	case cet_t:
	    CcPrintf(output, "\\\"\" %s \"\\\" expected", str);
	    break;
	case cet_alt:
	    CcPrintf(output,
		     "this symbol not expected in \\\"\" %s \"\\\"", str);
	    break;
	case cet_sync:
	    CcPrintf(output, "invalid \\\"\" %s \"\\\"", str);
	    break;
	}
	CcFree(str);
	CcPrintfL(output, "\"; break;");
    }
    return TRUE;
}

static CcsBool_t
COS_InitSet(CcCBaseOutputScheme_t * self, CcOutput_t * output)
{
    char * setstr; int setlen, index;
    const CcBitArray_t * cur;

    setlen = self->base.globals->symtab.terminals.Count;
    setstr = CcMalloc(setlen + 1); setstr[setlen] = 0;
    if (setlen > 4) {
	for (index = 0; index < setlen; ++index)
	    if (index == 0) setstr[index] = '*';
	    else setstr[index] = index % 5 == 0 ? '0' + index % 10 : ' ';
	CcPrintfIL(output, "/%s */", setstr);
    }
    for (cur = self->symSet.start; cur < self->symSet.used; ++cur) {
	CcsAssert(setlen == CcBitArray_getCount(cur));
	for (index = 0; index < setlen; ++index)
	    setstr[index] = CcBitArray_Get(cur, index) ? '*' : '.';
	CcPrintfIL(output, "\"%s.\"%c /* %d */", setstr,
		   cur < self->symSet.used - 1 ? ',' : ' ',
		   cur - self->symSet.start);
    }
    CcFree(setstr);
    return TRUE;
}

CcsBool_t
CcCBaseOutputScheme_write(CcOutputScheme_t * self, CcOutput_t * output,
			  const char * func, const char * params)
{
    CcCBaseOutputScheme_t * ccself = (CcCBaseOutputScheme_t *)self;
    if (!strcmp(func, "SynDefines")) {
	return COS_SynDefines(ccself, output);
    } else if (!strcmp(func, "members")) {
	return COS_Members(ccself, output);
    } else if (!strcmp(func, "constructor")) {
	return COS_Constructor(ccself, output);
    } else if (!strcmp(func, "destructor")) {
	return COS_Destructor(ccself, output);
    } else if (!strcmp(func, "Pragmas")) {
	return COS_Pragmas(ccself, output);
    } else if (!strcmp(func, "ProductionsHeader")) {
	return COS_ProductionsHeader(ccself, output);
    } else if (!strcmp(func, "ParseRoot")) {
	return COS_ParseRoot(ccself, output);
    } else if (!strcmp(func, "ProductionsBody")) {
	return COS_ProductionsBody(ccself, output);
    } else if (!strcmp(func, "SynErrors")) {
	return COS_SynErrors(ccself, output);
    } else if (!strcmp(func, "InitSet")) {
	return COS_InitSet(ccself, output);
    } else if (!strcmp(func, "hIncludes")) {
	/* hInclude section is not presented, omit it. */
	return TRUE;
    } else if (!strcmp(func, "cIncludes")) {
	/* cInclude section is not presented, omit it. */
	return TRUE;
    }
    fprintf(stderr, "Unknown section '%s' encountered.\n", func);
    return TRUE;
}

void
CcCBaseOutputScheme_Destruct(CcObject_t * self)
{
    CcCBaseOutputScheme_t * ccself = (CcCBaseOutputScheme_t *)self;
    CcSyntaxSymSet_Destruct(&ccself->symSet);
    CcOutputScheme_Destruct(self);
}
