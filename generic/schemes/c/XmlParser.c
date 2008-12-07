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
#include  "c/XmlParser.h"
#include  "c/Token.h"

/*---- cIncludes ----*/
#include  "XmlSpec.h"
#include  "Globals.h"
#include  "syntax/Nodes.h"
/*---- enable ----*/

static void CcsXmlParser_SynErr(CcsXmlParser_t * self, int n);
static const char * set[];

static void
CcsXmlParser_Get(CcsXmlParser_t * self)
{
    for (;;) {
	self->t = self->la;
	self->la = CcsXmlScanner_Scan(&self->scanner);
	if (self->la->kind <= self->maxT) { /*++self->errDist;*/ break; }
	/*---- Pragmas ----*/
	/*---- enable ----*/
	self->la = self->t;
    }
}

static CcsBool_t
CcsXmlParser_StartOf(CcsXmlParser_t * self, int s)
{
    return set[s][self->la->kind] == '*';
}

static void
CcsXmlParser_Expect(CcsXmlParser_t * self, int n)
{
    if (self->la->kind == n) CcsXmlParser_Get(self);
    else CcsXmlParser_SynErr(self, n);
}

static void
CcsXmlParser_ExpectWeak(CcsXmlParser_t * self, int n, int follow)
{
    if (self->la->kind == n) CcsXmlParser_Get(self);
    else {
	CcsXmlParser_SynErr(self, n);
	while (!CcsXmlParser_StartOf(self, follow)) CcsXmlParser_Get(self);
    }
}

static CcsBool_t
CcsXmlParser_WeakSeparator(CcsXmlParser_t * self, int n, int syFol, int repFol)
{
    if (self->la->kind == n) { CcsXmlParser_Get(self); return TRUE; }
    else if (CcsXmlParser_StartOf(self, repFol)) { return FALSE; }
    CcsXmlParser_SynErr(self, n);
    while (!(CcsXmlParser_StartOf(self, syFol) ||
	     CcsXmlParser_StartOf(self, repFol) ||
	     CcsXmlParser_StartOf(self, 0)))
	CcsXmlParser_Get(self);
    return CcsXmlParser_StartOf(self, syFol);
}

/*---- ProductionsHeader ----*/
static void CcsXmlParser_CocoXml(CcsXmlParser_t * self);
static void CcsXmlParser_SchemeDecl(CcsXmlParser_t * self);
static void CcsXmlParser_SectionDecl(CcsXmlParser_t * self);
static void CcsXmlParser_UpdateDecl(CcsXmlParser_t * self);
static void CcsXmlParser_XmlSpecDecl(CcsXmlParser_t * self, CcXmlSpec_t ** xsdef);
static void CcsXmlParser_XmlNamespaceDecl(CcsXmlParser_t * self);
static void CcsXmlParser_AttrDecl(CcsXmlParser_t * self, CcSymbolNT_t * sym);
static void CcsXmlParser_SemText(CcsXmlParser_t * self, CcsPosition_t ** pos);
static void CcsXmlParser_Expression(CcsXmlParser_t * self, CcGraph_t ** g);
static void CcsXmlParser_OptionDecl(CcsXmlParser_t * self, CcXmlSpec_t * xsdef);
static void CcsXmlParser_XmlTagDecl(CcsXmlParser_t * self, CcXmlSpec_t * xsdef);
static void CcsXmlParser_XmlAttrDecl(CcsXmlParser_t * self, CcXmlSpec_t * xsdef);
static void CcsXmlParser_XmlPIDecl(CcsXmlParser_t * self, CcXmlSpec_t * xsdef);
static void CcsXmlParser_Term(CcsXmlParser_t * self, CcGraph_t ** g);
static void CcsXmlParser_Resolver(CcsXmlParser_t * self, CcsPosition_t ** pos);
static void CcsXmlParser_Factor(CcsXmlParser_t * self, CcGraph_t ** g);
static void CcsXmlParser_Sym(CcsXmlParser_t * self, char ** name);
static void CcsXmlParser_Attribs(CcsXmlParser_t * self, CcNode_t * p);
static void CcsXmlParser_Condition(CcsXmlParser_t * self);
/*---- enable ----*/

void
CcsXmlParser_Parse(CcsXmlParser_t * self)
{
    self->t = NULL;
    self->la = CcsXmlScanner_GetDummy(&self->scanner);
    CcsXmlParser_Get(self);
    /*---- ParseRoot ----*/
    CcsXmlParser_CocoXml(self);
    /*---- enable ----*/
    CcsXmlParser_Expect(self, 0);
}

CcsBool_t
CcsXmlParser_Finish(CcsXmlParser_t * self)
{
    return CcGlobals_Finish(&self->globals);
}

void
CcsXmlParser_SemErr(CcsXmlParser_t * self, const CcsToken_t * token,
		    const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    CcsErrorPool_VError(&self->errpool, token->line, token->col,
			format, ap);
    va_end(ap);
}

void
CcsXmlParser_SemErrT(CcsXmlParser_t * self, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    CcsErrorPool_VError(&self->errpool, self->t->line, self->t->col,
			format, ap);
    va_end(ap);
}

CcsXmlParser_t *
CcsXmlParser(CcsXmlParser_t * self, const char * fname, FILE * errfp)
{
    if (!CcsErrorPool(&self->errpool, errfp)) goto errquit0;
    if (!CcsXmlScanner(&self->scanner, &self->errpool, fname)) goto errquit1;
    self->t = self->la = NULL;
    /*---- constructor ----*/
    self->maxT = 40;
    if (!CcGlobalsXml(&self->globals, &self->errpool)) goto ERRQUIT;
    self->schemeName = NULL;
    self->prefix = NULL;
    self->members = NULL;
    self->constructor = NULL;
    self->destructor = NULL;
    self->symtab = &self->globals.symtab;
    self->xmlspecmap = self->globals.xmlspecmap;
    self->syntax = &self->globals.syntax; 
    /*---- enable ----*/
    return self;
 ERRQUIT:
 errquit1:
    CcsErrorPool_Destruct(&self->errpool);
 errquit0:
    return NULL;
}

void
CcsXmlParser_Destruct(CcsXmlParser_t * self)
{
    /*---- destructor ----*/
    if (self->destructor) CcsPosition_Destruct(self->destructor);
    if (self->constructor) CcsPosition_Destruct(self->constructor);
    if (self->members) CcsPosition_Destruct(self->members);
    if (self->prefix) CcFree(self->prefix);
    if (self->schemeName) CcFree(self->schemeName);
    CcGlobals_Destruct(&self->globals);
    /*---- enable ----*/
    CcsXmlScanner_Destruct(&self->scanner);
    CcsErrorPool_Destruct(&self->errpool);
}

/*---- ProductionsBody ----*/
static void
CcsXmlParser_CocoXml(CcsXmlParser_t * self)
{
    CcSymbol_t  * sym;
    CcGraph_t   * g;
    char        * gramName = NULL;
    CcXmlSpec_t * xsdef;
    CcsToken_t  * beg; 
    while (self->la->kind == 14 || self->la->kind == 15 || self->la->kind == 16) {
	if (self->la->kind == 14) {
	    CcsXmlParser_SchemeDecl(self);
	} else if (self->la->kind == 15) {
	    CcsXmlParser_SectionDecl(self);
	} else {
	    CcsXmlParser_UpdateDecl(self);
	}
    }
    CcsXmlParser_Expect(self, 6);
    CcsXmlParser_Expect(self, 1);
    gramName = CcStrdup(self->t->val); 
    if (self->la->kind == 7) {
	CcsXmlParser_Get(self);
	CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
	while (CcsXmlParser_StartOf(self, 1)) {
	    CcsXmlParser_Get(self);
	}
	self->members = CcsXmlScanner_GetPosition(&self->scanner, beg, self->la);
	CcsXmlScanner_DecRef(&self->scanner, beg); 
    }
    if (self->la->kind == 8) {
	CcsXmlParser_Get(self);
	CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
	while (CcsXmlParser_StartOf(self, 2)) {
	    CcsXmlParser_Get(self);
	}
	self->constructor = CcsXmlScanner_GetPosition(&self->scanner, beg, self->la);
	CcsXmlScanner_DecRef(&self->scanner, beg); 
    }
    if (self->la->kind == 9) {
	CcsXmlParser_Get(self);
	CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
	while (CcsXmlParser_StartOf(self, 3)) {
	    CcsXmlParser_Get(self);
	}
	self->destructor = CcsXmlScanner_GetPosition(&self->scanner, beg, self->la);
	CcsXmlScanner_DecRef(&self->scanner, beg); 
    }
    CcsXmlParser_XmlSpecDecl(self, &xsdef);
    CcXmlSpecMap_Add(self->xmlspecmap, "", xsdef); 
    while (self->la->kind == 17) {
	CcsXmlParser_XmlNamespaceDecl(self);
    }
    while (!(self->la->kind == 0 || self->la->kind == 10)) {
	CcsXmlParser_SynErr(self, 41); CcsXmlParser_Get(self);
    }
    CcsXmlParser_Expect(self, 10);
    CcXmlSpecMap_MakeTerminals(self->xmlspecmap, &self->globals);
    CcEBNF_Clear(&self->syntax->base); 
    while (self->la->kind == 1) {
	CcsXmlParser_Get(self);
	sym = CcSymbolTable_FindSym(self->symtab, self->t->val);
	CcsBool_t undef = (sym == NULL);
	if (undef) {
	    sym = CcSymbolTable_NewNonTerminal(self->symtab,
					       self->t->val, self->t->line);
	} else {
	    if (sym->base.type == symbol_nt) {
		if (((CcSymbolNT_t *)sym)->graph != NULL)
		    CcsXmlParser_SemErrT(self, "name declared twice");
	    } else {
		CcsXmlParser_SemErrT(self, "this symbol kind not allowed on left side of production");
	    }
	    sym->line = self->t->line;
	}
	CcsAssert(sym->base.type == symbol_nt);
	CcsBool_t noAttrs = (((CcSymbolNT_t *)sym)->attrPos == NULL);
	if (!noAttrs) {
	    CcsPosition_Destruct(((CcSymbolNT_t *)sym)->attrPos);
	    ((CcSymbolNT_t *)sym)->attrPos = NULL; 
	} 
	if (self->la->kind == 23 || self->la->kind == 25) {
	    CcsXmlParser_AttrDecl(self, (CcSymbolNT_t *)sym);
	}
	if (!undef && noAttrs != (((CcSymbolNT_t *)sym)->attrPos == NULL))
	    CcsXmlParser_SemErrT(self, "attribute mismatch between declaration and use of this symbol"); 
	if (self->la->kind == 38) {
	    CcsXmlParser_SemText(self, &((CcSymbolNT_t *)sym)->semPos);
	}
	CcsXmlParser_ExpectWeak(self, 11, 4);
	CcsXmlParser_Expression(self, &g);
	((CcSymbolNT_t *)sym)->graph = g->head;
	CcGraph_Finish(g);
	CcGraph_Destruct(g); 
	CcsXmlParser_ExpectWeak(self, 12, 5);
    }
    CcsXmlParser_Expect(self, 13);
    CcsXmlParser_Expect(self, 1);
    if (strcmp(gramName, self->t->val))
	CcsXmlParser_SemErrT(self, "name does not match grammar name");
    self->syntax->gramSy = CcSymbolTable_FindSym(self->symtab, gramName);
    CcFree(gramName);
    if (self->syntax->gramSy == NULL) {
	CcsXmlParser_SemErrT(self, "missing production for grammar name");
    } else {
	sym = self->syntax->gramSy;
	if (((CcSymbolNT_t *)sym)->attrPos != NULL)
	    CcsXmlParser_SemErrT(self, "grammar symbol must not have attributes");
    }
    /* noSym gets highest number */
    self->syntax->noSy = CcSymbolTable_NewTerminal(self->symtab, "???", 0);
    CcSyntax_SetupAnys(self->syntax); 
    CcsXmlParser_Expect(self, 12);
}

static void
CcsXmlParser_SchemeDecl(CcsXmlParser_t * self)
{
    CcsXmlParser_Expect(self, 14);
    CcsXmlParser_Expect(self, 1);
    if (self->schemeName) CcFree(self->schemeName);
    self->schemeName = CcStrdup(self->t->val); 
    CcsXmlParser_Expect(self, 1);
    if (self->prefix) CcFree(self->prefix);
    self->prefix = CcStrdup(self->t->val); 
}

static void
CcsXmlParser_SectionDecl(CcsXmlParser_t * self)
{
    char * secname; CcsToken_t * beg; 
    CcsXmlParser_Expect(self, 15);
    CcsXmlParser_Expect(self, 1);
    secname = CcStrdup(self->t->val);
    CcsXmlScanner_IncRef(&self->scanner, beg = self->t); 
    while (CcsXmlParser_StartOf(self, 6)) {
	CcsXmlParser_Get(self);
    }
    CcGlobals_NewSection(&self->globals, secname,
			 CcsXmlScanner_GetPositionBetween(&self->scanner, beg, self->la));
    CcsXmlScanner_DecRef(&self->scanner, beg);
    CcFree(secname); 
    CcsXmlParser_Expect(self, 13);
    CcsXmlParser_Expect(self, 12);
}

static void
CcsXmlParser_UpdateDecl(CcsXmlParser_t * self)
{
    CcsXmlParser_Expect(self, 16);
    while (self->la->kind == 3) {
	CcsXmlParser_Get(self);
	CcGlobals_AddUpdate(&self->globals, self->t->val); 
    }
    CcsXmlParser_Expect(self, 13);
    CcsXmlParser_Expect(self, 12);
}

static void
CcsXmlParser_XmlSpecDecl(CcsXmlParser_t * self, CcXmlSpec_t ** xsdef)
{
    *xsdef = CcXmlSpec(&self->globals); 
    if (self->la->kind == 18) {
	CcsXmlParser_Get(self);
	CcXmlSpec_SetCaseSensitive(*xsdef, FALSE); 
    }
    if (self->la->kind == 19) {
	CcsXmlParser_Get(self);
	while (self->la->kind == 1) {
	    CcsXmlParser_OptionDecl(self, *xsdef);
	}
    }
    if (self->la->kind == 20) {
	CcsXmlParser_Get(self);
	while (self->la->kind == 1) {
	    CcsXmlParser_XmlTagDecl(self, *xsdef);
	}
    }
    if (self->la->kind == 21) {
	CcsXmlParser_Get(self);
	while (self->la->kind == 1) {
	    CcsXmlParser_XmlAttrDecl(self, *xsdef);
	}
    }
    if (self->la->kind == 22) {
	CcsXmlParser_Get(self);
	while (self->la->kind == 1) {
	    CcsXmlParser_XmlPIDecl(self, *xsdef);
	}
    }
}

static void
CcsXmlParser_XmlNamespaceDecl(CcsXmlParser_t * self)
{
    char * nsURI; CcXmlSpec_t * xsdef; 
    CcsXmlParser_Expect(self, 17);
    CcsXmlParser_Expect(self, 3);
    nsURI = CcStrdup(self->t->val); 
    CcsXmlParser_XmlSpecDecl(self, &xsdef);
    if (!CcXmlSpecMap_Add(self->xmlspecmap, nsURI, xsdef))
	CcsXmlParser_SemErrT(self, "The namespace '%s' is defined more than once.", nsURI); 
    CcsXmlParser_Expect(self, 13);
    CcFree(nsURI); 
}

static void
CcsXmlParser_AttrDecl(CcsXmlParser_t * self, CcSymbolNT_t * sym)
{
    if (self->la->kind == 23) {
	CcsXmlParser_Get(self);
	CcsToken_t * beg;
	CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
	while (CcsXmlParser_StartOf(self, 7)) {
	    if (CcsXmlParser_StartOf(self, 8)) {
		CcsXmlParser_Get(self);
	    } else {
		CcsXmlParser_Get(self);
		CcsXmlParser_SemErrT(self, "bad string in attributes"); 
	    }
	}
	CcsXmlParser_Expect(self, 24);
	sym->attrPos = CcsXmlScanner_GetPosition(&self->scanner, beg, self->t);
	CcsXmlScanner_DecRef(&self->scanner, beg); 
    } else if (self->la->kind == 25) {
	CcsXmlParser_Get(self);
	CcsToken_t * beg;
	CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
	while (CcsXmlParser_StartOf(self, 9)) {
	    if (CcsXmlParser_StartOf(self, 10)) {
		CcsXmlParser_Get(self);
	    } else {
		CcsXmlParser_Get(self);
		CcsXmlParser_SemErrT(self, "bad string in attributes"); 
	    }
	}
	CcsXmlParser_Expect(self, 26);
	sym->attrPos = CcsXmlScanner_GetPosition(&self->scanner, beg, self->t);
	CcsXmlScanner_DecRef(&self->scanner, beg); 
    } else CcsXmlParser_SynErr(self, 42);
}

static void
CcsXmlParser_SemText(CcsXmlParser_t * self, CcsPosition_t ** pos)
{
    CcsXmlParser_Expect(self, 38);
    CcsToken_t * beg;
    CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
    while (CcsXmlParser_StartOf(self, 11)) {
	if (CcsXmlParser_StartOf(self, 12)) {
	    CcsXmlParser_Get(self);
	} else if (self->la->kind == 4) {
	    CcsXmlParser_Get(self);
	    CcsXmlParser_SemErrT(self, "bad string in semantic action"); 
	} else {
	    CcsXmlParser_Get(self);
	    CcsXmlParser_SemErrT(self, "missing end of previous semantic action"); 
	}
    }
    CcsXmlParser_Expect(self, 39);
    *pos = CcsXmlScanner_GetPosition(&self->scanner, beg, self->t);
    CcsXmlScanner_DecRef(&self->scanner, beg); 
}

static void
CcsXmlParser_Expression(CcsXmlParser_t * self, CcGraph_t ** g)
{
    CcGraph_t * g2; 
    CcsXmlParser_Term(self, g);
    CcsBool_t first = TRUE; 
    while (CcsXmlParser_WeakSeparator(self, 27, 14, 13)) {
	CcsXmlParser_Term(self, &g2);
	if (first) { CcEBNF_MakeFirstAlt(&self->syntax->base, *g); first = FALSE; }
	CcEBNF_MakeAlternative(&self->syntax->base, *g, g2);
	CcGraph_Destruct(g2); 
    }
}

static void
CcsXmlParser_OptionDecl(CcsXmlParser_t * self, CcXmlSpec_t * xsdef)
{
    CcsXmlParser_Expect(self, 1);
    CcXmlSpec_SetOption(xsdef, self->t); 
}

static void
CcsXmlParser_XmlTagDecl(CcsXmlParser_t * self, CcXmlSpec_t * xsdef)
{
    char * tagname; 
    CcsXmlParser_Expect(self, 1);
    tagname = CcStrdup(self->t->val); 
    CcsXmlParser_Expect(self, 11);
    CcsXmlParser_Expect(self, 3);
    CcXmlSpec_AddTag(xsdef, tagname, self->t);
    CcFree(tagname); 
}

static void
CcsXmlParser_XmlAttrDecl(CcsXmlParser_t * self, CcXmlSpec_t * xsdef)
{
    char * attrname; 
    CcsXmlParser_Expect(self, 1);
    attrname = CcStrdup(self->t->val); 
    CcsXmlParser_Expect(self, 11);
    CcsXmlParser_Expect(self, 3);
    CcXmlSpec_AddAttr(xsdef, attrname, self->t);
    CcFree(attrname); 
}

static void
CcsXmlParser_XmlPIDecl(CcsXmlParser_t * self, CcXmlSpec_t * xsdef)
{
    char * target; 
    CcsXmlParser_Expect(self, 1);
    target = CcStrdup(self->t->val); 
    CcsXmlParser_Expect(self, 11);
    CcsXmlParser_Expect(self, 3);
    CcXmlSpec_AddProcessInstruction(xsdef, target, self->t);
    CcFree(target); 
}

static void
CcsXmlParser_Term(CcsXmlParser_t * self, CcGraph_t ** g)
{
    CcGraph_t * g2; CcsPosition_t * pos; CcNode_t * rslv = NULL;
    *g = NULL; 
    if (CcsXmlParser_StartOf(self, 15)) {
	if (self->la->kind == 37) {
	    CcsXmlParser_Resolver(self, &pos);
	    rslv = CcEBNF_NewNode(&self->syntax->base,
				  CcNodeRslvP(self->la->line, pos));
	    *g = CcGraphP(rslv); 
	}
	CcsXmlParser_Factor(self, &g2);
	if (rslv == NULL) *g = g2;
	else {
	    CcEBNF_MakeSequence(&self->syntax->base, *g, g2);
	    CcGraph_Destruct(g2);
	} 
	while (CcsXmlParser_StartOf(self, 16)) {
	    CcsXmlParser_Factor(self, &g2);
	    CcEBNF_MakeSequence(&self->syntax->base, *g, g2);
	    CcGraph_Destruct(g2); 
	}
    } else if (CcsXmlParser_StartOf(self, 17)) {
	*g = CcGraphP(CcEBNF_NewNode(&self->syntax->base, CcNodeEps(0))); 
    } else CcsXmlParser_SynErr(self, 43);
    if (*g == NULL) /* invalid start of Term */
	*g = CcGraphP(CcEBNF_NewNode(&self->syntax->base, CcNodeEps(0))); 
}

static void
CcsXmlParser_Resolver(CcsXmlParser_t * self, CcsPosition_t ** pos)
{
    CcsXmlParser_Expect(self, 37);
    CcsXmlParser_Expect(self, 29);
    CcsToken_t * beg;
    CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
    CcsXmlParser_Condition(self);
    *pos = CcsXmlScanner_GetPosition(&self->scanner, beg, self->t);
    CcsXmlScanner_DecRef(&self->scanner, beg); 
}

static void
CcsXmlParser_Factor(CcsXmlParser_t * self, CcGraph_t ** g)
{
    char * name = NULL; CcsPosition_t * pos; CcsBool_t weak = FALSE; 
    *g = NULL; 
    switch (self->la->kind) {
    case 1: case 28: {
	if (self->la->kind == 28) {
	    CcsXmlParser_Get(self);
	    weak = TRUE; 
	}
	CcsXmlParser_Sym(self, &name);
	CcSymbol_t * sym = CcSymbolTable_FindSym(self->symtab, name);
	CcsBool_t undef = (sym == NULL);
	if (undef) { /* forward nt */
	     sym = CcSymbolTable_NewNonTerminal(self->symtab, name, 0);
	}
	CcFree(name);
	if (sym->base.type != symbol_t && sym->base.type != symbol_nt)
	    CcsXmlParser_SemErrT(self, "this symbol kind is not allowed in a production");
	if (weak) {
	    if (sym->base.type != symbol_t)
		CcsXmlParser_SemErrT(self, "only terminals may be weak");
	}
	CcNode_t * p = CcSyntax_NodeFromSymbol(self->syntax, sym, self->t->line, weak);
	*g = CcGraphP(p); 
	if (self->la->kind == 23 || self->la->kind == 25) {
	    CcsXmlParser_Attribs(self, p);
	}
	if (undef) {
	    if (sym->base.type == symbol_nt)
		((CcSymbolNT_t *)sym)->attrPos = CcsPosition_Clone(((CcNodeNT_t *)p)->pos);
	} else if (sym->base.type == symbol_nt &&
		   (((CcNodeNT_t *)p)->pos == NULL) !=
		   (((CcSymbolNT_t *)sym)->attrPos == NULL))
	    CcsXmlParser_SemErrT(self, "attribute mismatch between declaration and use of this symbol"); 
	break;
    }
    case 29: {
	CcsXmlParser_Get(self);
	CcsXmlParser_Expression(self, g);
	CcsXmlParser_Expect(self, 30);
	break;
    }
    case 31: {
	CcsXmlParser_Get(self);
	CcsXmlParser_Expression(self, g);
	CcsXmlParser_Expect(self, 32);
	CcEBNF_MakeOption(&self->syntax->base, *g); 
	break;
    }
    case 33: {
	CcsXmlParser_Get(self);
	CcsXmlParser_Expression(self, g);
	CcsXmlParser_Expect(self, 34);
	CcEBNF_MakeIteration(&self->syntax->base, *g); 
	break;
    }
    case 38: {
	CcsXmlParser_SemText(self, &pos);
	CcNode_t * p = CcEBNF_NewNode(&self->syntax->base, CcNodeSem(0));
	((CcNodeSEM_t *)p)->pos = pos;
	*g = CcGraphP(p); 
	break;
    }
    case 35: {
	CcsXmlParser_Get(self);
	CcNode_t * p = CcEBNF_NewNode(&self->syntax->base, CcNodeAny(0));
	*g = CcGraphP(p); 
	break;
    }
    case 36: {
	CcsXmlParser_Get(self);
	CcNode_t * p = CcEBNF_NewNode(&self->syntax->base, CcNodeSync(0));
	*g = CcGraphP(p);
	break;
    }
    default: CcsXmlParser_SynErr(self, 44); break;
    }
    if (*g == NULL) /* invalid start of Factor */
	*g = CcGraphP(CcEBNF_NewNode(&self->syntax->base, CcNodeEps(0))); 
}

static void
CcsXmlParser_Sym(CcsXmlParser_t * self, char ** name)
{
    CcsXmlParser_Expect(self, 1);
    *name = CcStrdup(self->t->val); 
}

static void
CcsXmlParser_Attribs(CcsXmlParser_t * self, CcNode_t * p)
{
    if (self->la->kind == 23) {
	CcsXmlParser_Get(self);
	CcsToken_t * beg;
	CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
	while (CcsXmlParser_StartOf(self, 7)) {
	    if (CcsXmlParser_StartOf(self, 8)) {
		CcsXmlParser_Get(self);
	    } else {
		CcsXmlParser_Get(self);
		CcsXmlParser_SemErrT(self, "bad string in attributes"); 
	    }
	}
	CcsXmlParser_Expect(self, 24);
	CcNode_SetPosition(p, CcsXmlScanner_GetPosition(&self->scanner, beg, self->t));
	CcsXmlScanner_DecRef(&self->scanner, beg); 
    } else if (self->la->kind == 25) {
	CcsXmlParser_Get(self);
	CcsToken_t * beg;
	CcsXmlScanner_IncRef(&self->scanner, beg = self->la); 
	while (CcsXmlParser_StartOf(self, 9)) {
	    if (CcsXmlParser_StartOf(self, 10)) {
		CcsXmlParser_Get(self);
	    } else {
		CcsXmlParser_Get(self);
		CcsXmlParser_SemErrT(self, "bad string in attributes"); 
	    }
	}
	CcsXmlParser_Expect(self, 26);
	CcNode_SetPosition(p, CcsXmlScanner_GetPosition(&self->scanner,
						     beg, self->t));
	CcsXmlScanner_DecRef(&self->scanner, beg); 
    } else CcsXmlParser_SynErr(self, 45);
}

static void
CcsXmlParser_Condition(CcsXmlParser_t * self)
{
    while (CcsXmlParser_StartOf(self, 18)) {
	if (self->la->kind == 29) {
	    CcsXmlParser_Get(self);
	    CcsXmlParser_Condition(self);
	} else {
	    CcsXmlParser_Get(self);
	}
    }
    CcsXmlParser_Expect(self, 30);
}

/*---- enable ----*/

static void
CcsXmlParser_SynErr(CcsXmlParser_t * self, int n)
{
    const char * s; char format[20];
    switch (n) {
    /*---- SynErrors ----*/
    case 0: s = "\"" "EOF" "\" expected"; break;
    case 1: s = "\"" "ident" "\" expected"; break;
    case 2: s = "\"" "number" "\" expected"; break;
    case 3: s = "\"" "string" "\" expected"; break;
    case 4: s = "\"" "badString" "\" expected"; break;
    case 5: s = "\"" "char" "\" expected"; break;
    case 6: s = "\"" "COMPILER" "\" expected"; break;
    case 7: s = "\"" "MEMBERS" "\" expected"; break;
    case 8: s = "\"" "CONSTRUCTOR" "\" expected"; break;
    case 9: s = "\"" "DESTRUCTOR" "\" expected"; break;
    case 10: s = "\"" "PRODUCTIONS" "\" expected"; break;
    case 11: s = "\"" "=" "\" expected"; break;
    case 12: s = "\"" "." "\" expected"; break;
    case 13: s = "\"" "END" "\" expected"; break;
    case 14: s = "\"" "SCHEME" "\" expected"; break;
    case 15: s = "\"" "SECTION" "\" expected"; break;
    case 16: s = "\"" "UPDATES" "\" expected"; break;
    case 17: s = "\"" "NAMESPACE" "\" expected"; break;
    case 18: s = "\"" "IGNORECASE" "\" expected"; break;
    case 19: s = "\"" "OPTIONS" "\" expected"; break;
    case 20: s = "\"" "TAGS" "\" expected"; break;
    case 21: s = "\"" "ATTRS" "\" expected"; break;
    case 22: s = "\"" "PROCESSING_INSTRUCTIONS" "\" expected"; break;
    case 23: s = "\"" "<" "\" expected"; break;
    case 24: s = "\"" ">" "\" expected"; break;
    case 25: s = "\"" "<." "\" expected"; break;
    case 26: s = "\"" ".>" "\" expected"; break;
    case 27: s = "\"" "|" "\" expected"; break;
    case 28: s = "\"" "WEAK" "\" expected"; break;
    case 29: s = "\"" "(" "\" expected"; break;
    case 30: s = "\"" ")" "\" expected"; break;
    case 31: s = "\"" "[" "\" expected"; break;
    case 32: s = "\"" "]" "\" expected"; break;
    case 33: s = "\"" "{" "\" expected"; break;
    case 34: s = "\"" "}" "\" expected"; break;
    case 35: s = "\"" "ANY" "\" expected"; break;
    case 36: s = "\"" "SYNC" "\" expected"; break;
    case 37: s = "\"" "IF" "\" expected"; break;
    case 38: s = "\"" "(." "\" expected"; break;
    case 39: s = "\"" ".)" "\" expected"; break;
    case 40: s = "\"" "???" "\" expected"; break;
    case 41: s = "invalid \"" "CocoXml" "\""; break;
    case 42: s = "this symbol not expected in \"" "AttrDecl" "\""; break;
    case 43: s = "this symbol not expected in \"" "Term" "\""; break;
    case 44: s = "this symbol not expected in \"" "Factor" "\""; break;
    case 45: s = "this symbol not expected in \"" "Attribs" "\""; break;
    /*---- enable ----*/
    default:
	snprintf(format, sizeof(format), "error %d", n);
	s = format;
	break;
    }
    CcsXmlParser_SemErr(self, self->la, "%s", s);
}

static const char * set[] = {
    /*---- InitSet ----*/
    /*    5    0    5    0    5    0    5    0 */
    "*.........*...............................", /* 0 */
    ".*******...******......******************.", /* 1 */
    ".********..******......******************.", /* 2 */
    ".*********.******......******************.", /* 3 */
    "**........*.*..............***.*.*.****...", /* 4 */
    "**........*..*............................", /* 5 */
    ".************.***************************.", /* 6 */
    ".***********************.****************.", /* 7 */
    ".***.*******************.****************.", /* 8 */
    ".*************************.**************.", /* 9 */
    ".***.*********************.**************.", /* 10 */
    ".**************************************.*.", /* 11 */
    ".***.*********************************..*.", /* 12 */
    "............*.................*.*.*.......", /* 13 */
    ".*..........*..............************...", /* 14 */
    ".*..........................**.*.*.****...", /* 15 */
    ".*..........................**.*.*.**.*...", /* 16 */
    "............*..............*..*.*.*.......", /* 17 */
    ".*****************************.**********."  /* 18 */
    /*---- enable ----*/
};
