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
#include  "Globals.h"
#include  "BasicObjects.h"
#include  "c/ErrorPool.h"

CcGlobals_t *
CcGlobals(CcGlobals_t * self, CcsErrorPool_t * errpool)
{
    self->errpool = errpool;
    self->templatePrefix = "Ccs";
    if (!CcSymbolTable(&self->symtab)) goto errquit1;
    if (!(self->lexical = CcLexical(&self->u.lexicalSpace, self)))
	goto errquit2;
    self->xmlspecmap = NULL;
    if (!CcSyntax(&self->syntax, self)) goto errquit3;
    if (!CcArrayList(&self->sections)) goto errquit4;
    if (!CcArrayList(&self->updates)) goto errquit5;
    return self;
 errquit5:
    CcArrayList_Destruct(&self->sections);
 errquit4:
    CcSyntax_Destruct(&self->syntax);
 errquit3:
    CcLexical_Destruct(self->lexical);
 errquit2:
    CcSymbolTable_Destruct(&self->symtab);
 errquit1:
    return NULL;
}

CcGlobals_t *
CcGlobalsXml(CcGlobals_t * self, CcsErrorPool_t * errpool)
{
    self->errpool = errpool;
    self->templatePrefix = "Ccx";
    if (!CcSymbolTable(&self->symtab)) goto errquit1;
    self->lexical = NULL;
    if (!(self->xmlspecmap = CcXmlSpecMap(&self->u.xmlspecmapSpace, self)))
	goto errquit2;
    if (!CcSyntax(&self->syntax, self)) goto errquit3;
    if (!CcArrayList(&self->sections)) goto errquit4;
    if (!CcArrayList(&self->updates)) goto errquit5;
    return self;
 errquit5:
    CcArrayList_Destruct(&self->sections);
 errquit4:
    CcSyntax_Destruct(&self->syntax);
 errquit3:
    CcXmlSpecMap_Destruct(self->xmlspecmap);
 errquit2:
    CcSymbolTable_Destruct(&self->symtab);
 errquit1:
    return NULL;
}

void
CcGlobals_Destruct(CcGlobals_t * self)
{
    CcArrayList_Destruct(&self->updates);
    CcArrayList_Destruct(&self->sections);
    CcSyntax_Destruct(&self->syntax);
    if (self->lexical) CcLexical_Destruct(self->lexical);
    if (self->xmlspecmap) CcXmlSpecMap_Destruct(self->xmlspecmap);
    CcSymbolTable_Destruct(&self->symtab);
}

CcsBool_t
CcGlobals_Finish(CcGlobals_t * self)
{
    if (self->errpool->errorCount > 0) return FALSE;
    if (!CcSymbolTable_Finish(&self->symtab)) return FALSE;
    if (self->lexical && !CcLexical_Finish(self->lexical)) return FALSE;
    if (self->xmlspecmap && !CcXmlSpecMap_Finish(self->xmlspecmap))
	return FALSE;
    if (!CcSyntax_Finish(&self->syntax)) return FALSE;
    return TRUE;
}

typedef struct {
    CcObject_t base;
    char * name;
    CcsPosition_t * pos;
}  CcSection_t;

static void
CcSection_Destruct(CcObject_t * self)
{
    CcSection_t * ccself = (CcSection_t *)self;

    CcsPosition_Destruct(ccself->pos);
    CcFree(ccself->name);
    CcObject_Destruct(self);
}

static const CcObjectType_t SectionType = {
    sizeof(CcSection_t), "Section", CcSection_Destruct
};

void
CcGlobals_NewSection(CcGlobals_t * self, const char * secname,
		     CcsPosition_t * pos)
{
    CcSection_t * section = (CcSection_t *)
	CcArrayList_New(&self->sections, CcObject(&SectionType));
    section->name = CcStrdup(secname);
    section->pos = pos;
}

const CcsPosition_t *
CcGlobals_GetSection(const CcGlobals_t * self, const char * secname)
{
    CcArrayListIter_t iter; const CcSection_t * section;

    for (section = (const CcSection_t *)CcArrayList_FirstC(&self->sections, &iter);
	 section;
	 section = (const CcSection_t *)CcArrayList_NextC(&self->sections, &iter))
	if (!strcmp(section->name, secname)) return section->pos;
    return NULL;
}

void
CcGlobals_AddUpdate(CcGlobals_t * self, const char * update)
{
    char * update0 = CcUnescape(update);
    CcArrayList_New(&self->updates, (CcObject_t *)CcString(update0));
    CcFree(update0);
}
