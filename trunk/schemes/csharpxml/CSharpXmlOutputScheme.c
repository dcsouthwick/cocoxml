/*-------------------------------------------------------------------------
  Copyright (C) 2008, Charles Wang
  Author: Charles Wang <charlesw123456@gmail.com>
  License: GPLv2 (see LICENSE-GPL)
-------------------------------------------------------------------------*/
#include  "csharpxml/CSharpXmlOutputScheme.h"
#include  "XmlSpec.h"

static CcsBool_t
CSOS_KindUnknownNS(CcCSharpXmlOutputScheme_t * self, CcOutput_t * output)
{
    CcsAssert(self->base.base.globals->xmlspecmap);
    CcPrintfIL(output, "self->base.kindUnknownNS = %d;",
	       self->base.base.globals->xmlspecmap->kindUnknownNS);
    return TRUE;
}

static int
cmpSpecKey(const void * cs0, const void * cs1)
{
    return strcmp(*(const char **)cs0, *(const char **)cs1);
}

static CcsBool_t
CSOS_XmlSpecSubLists(CcCSharpXmlOutputScheme_t * self, CcOutput_t * output)
{
    int count; CcHTIterator_t iter;
    const char ** keylist, ** curkey;
    const CcXmlSpec_t * spec;
    CcXmlSpecData_t * datalist, * datacur; size_t datanum; char * tmp;
    CcXmlSpecMap_t * map = self->base.base.globals->xmlspecmap;

    CcsAssert(map != NULL);
    count = CcHashTable_Num(&map->map);
    keylist = curkey = CcMalloc(sizeof(char *) * count);
    CcHashTable_GetIterator(&map->map, &iter);
    while (CcHTIterator_Forward(&iter)) *curkey++ = CcHTIterator_Key(&iter);
    CcsAssert(curkey - keylist == count);
    qsort(keylist, count, sizeof(const char *), cmpSpecKey);

    CcPrintfIL(output, "static const CcxTag_t XmlTags[] = {");
    for (curkey = keylist; curkey - keylist < count; ++curkey) {
	spec = (const CcXmlSpec_t *)CcHashTable_Get(&map->map, *curkey);
	CcsAssert(spec != NULL);
	datalist = CcXmlSpec_GetSortedTagList(spec, self->base.base.globals,
					      &datanum);
	if (datanum == 0) continue;
	output->indent += 4;
	for (datacur = datalist; datacur - datalist < datanum; ++datacur) {
	    tmp = CcEscape(datacur->name);
	    CcPrintfIL(output, "{ %s, %d, %d },",
		       tmp, datacur->kind0, datacur->kind1);
	    CcFree(tmp);
	}
	output->indent -= 4;
	CcXmlSpecData_Destruct(datalist, datanum);
    }
    CcPrintfL(output, "};");

    CcPrintfIL(output, "static const CcxAttr_t XmlAttrs[] = {");
    for (curkey = keylist; curkey - keylist < count; ++curkey) {
	spec = (const CcXmlSpec_t *)CcHashTable_Get(&map->map, *curkey);
	CcsAssert(spec != NULL);
	datalist = CcXmlSpec_GetSortedAttrList(spec, self->base.base.globals,
					       &datanum);
	if (datanum == 0) continue;
	output->indent += 4;
	for (datacur = datalist; datacur - datalist < datanum; ++datacur) {
	    tmp = CcEscape(datacur->name);
	    CcPrintfIL(output, "{ %s, %d },", tmp, datacur->kind0);
	    CcFree(tmp);
	}
	output->indent -= 4;
	CcXmlSpecData_Destruct(datalist, datanum);
    }
    CcPrintfL(output, "};");

    CcPrintfIL(output, "static const CcxPInstruction_t XmlPIs[] = {");
    for (curkey = keylist; curkey - keylist < count; ++curkey) {
	spec = (const CcXmlSpec_t *)CcHashTable_Get(&map->map, *curkey);
	CcsAssert(spec != NULL);
	datalist = CcXmlSpec_GetSortedPIList(spec, self->base.base.globals,
					     &datanum);
	if (datanum == 0) continue;
	output->indent += 4;
	for (datacur = datalist; datacur - datalist < datanum; ++datacur) {
	    tmp = CcEscape(datacur->name);
	    CcPrintfIL(output, "{ %s, %d },", tmp, datacur->kind0);
	    CcFree(tmp);
	}
	output->indent -= 4;
	CcXmlSpecData_Destruct(datalist, datanum);
    }
    CcPrintfL(output, "};");

    CcFree(keylist);
    return TRUE;
}

static CcsBool_t
CSOS_XmlSpecList(CcCSharpXmlOutputScheme_t * self, CcOutput_t * output)
{
    int kinds[XSO_SIZE];
    int count; CcHTIterator_t iter;
    const char ** keylist, ** curkey;
    int cntTagList, cntAttrList, cntPIList;
    char * tmp; CcsXmlSpecOption_t option;
    const CcXmlSpec_t * spec;
    CcXmlSpecData_t * datalist; size_t datanum;
    CcXmlSpecMap_t * map = self->base.base.globals->xmlspecmap;

    CcsAssert(map != NULL);
    CcXmlSpecMap_GetOptionKinds(map, kinds, self->base.base.globals);
    count = CcHashTable_Num(&map->map);
    keylist = curkey = CcMalloc(sizeof(char *) * count);
    CcHashTable_GetIterator(&map->map, &iter);
    while (CcHTIterator_Forward(&iter)) *curkey++ = CcHTIterator_Key(&iter);
    CcsAssert(curkey - keylist == count);
    qsort(keylist, count, sizeof(const char *), cmpSpecKey);

    cntTagList = cntAttrList = cntPIList = 0;
    for (curkey = keylist; curkey - keylist < count; ++curkey) {
	spec = (const CcXmlSpec_t *)CcHashTable_Get(&map->map, *curkey);
	CcsAssert(spec != NULL);
	tmp = CcEscape(*curkey);
	CcPrintfIL(output, "{ %s, %s,", tmp,
		   spec->caseSensitive ? "TRUE" : "FALSE");
	CcFree(tmp);
	output->indent += 4;
	CcPrintfI(output, "{");
	for (option = XSO_UnknownTag; option < XSO_SIZE; ++option)
	    CcPrintf(output, " %d,",
		     CcBitArray_Get(&spec->options, option) ? kinds[option] : -1);
	CcPrintfL(output, " },");

	datalist = CcXmlSpec_GetSortedTagList(spec, self->base.base.globals,
					      &datanum);
	if (datanum == 0) CcPrintfIL(output, "NULL, 0, /* Tags */");
	else {
	    CcPrintfIL(output, "XmlTags + %d, %d,", cntTagList, datanum);
	    cntTagList += datanum;
	}
	CcXmlSpecData_Destruct(datalist, datanum);

	datalist = CcXmlSpec_GetSortedAttrList(spec, self->base.base.globals,
					       &datanum);
	if (datanum == 0) CcPrintfIL(output, "NULL, 0, /* Attrs */");
	else {
	    CcPrintfIL(output, "XmlAttrs + %d, %d,", cntAttrList, datanum);
	    cntAttrList += datanum;
	}
	CcXmlSpecData_Destruct(datalist, datanum);

	datalist = CcXmlSpec_GetSortedPIList(spec, self->base.base.globals,
					     &datanum);
	if (datanum == 0) {
	    CcPrintfIL(output, "NULL, 0, /* Processing Instructions */");
	} else {
	    CcPrintfIL(output, "XmlPIs + %d, %d,", cntPIList, datanum);
	    cntPIList += datanum;
	}
	CcXmlSpecData_Destruct(datalist, datanum);

	output->indent -= 4;
	CcPrintfIL(output, "},");
    }
    CcFree(keylist);
    return TRUE;
}

static CcsBool_t
CcCSharpXmlOutputScheme_write(CcOutputScheme_t * self, CcOutput_t * output,
			      const char * func, const char * param)
{
    CcCSharpXmlOutputScheme_t * ccself = (CcCSharpXmlOutputScheme_t *)self;
    if (!strcmp(func, "kindUnknownNS")) {
	return CSOS_KindUnknownNS(ccself, output);
    } else if (!strcmp(func, "XmlSpecSubLists")) {
	return CSOS_XmlSpecSubLists(ccself, output);
    } else if (!strcmp(func, "XmlSpecList")) {
	return CSOS_XmlSpecList(ccself, output);
    }
    return CcCSharpBaseOutputScheme_write(self, output, func, param);
}

static const CcOutputSchemeType_t CSharpXmlOutputSchemeType = {
    { sizeof(CcCSharpXmlOutputScheme_t), "CSharpXmlOutputScheme",
      CcCSharpBaseOutputScheme_Destruct },
    /* If the following list are modified, modify install.py too. */
    "Buffer.cs\0ErrorPool.cs\0Token.cs\0Scanner4Xml.cs\0Parser4Xml.cs\0\0",
    CcCSharpXmlOutputScheme_write
};

CcCSharpXmlOutputScheme_t *
CcCSharpXmlOutputScheme(CcsXmlParser_t * parser, CcArguments_t * arguments)
{
    CcCSharpXmlOutputScheme_t * self = (CcCSharpXmlOutputScheme_t *)
	CcCSharpBaseOutputScheme(&CSharpXmlOutputSchemeType,
				 &parser->globals, arguments);
    self->parser = parser;
    return self;
}
