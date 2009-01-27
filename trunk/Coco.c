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
#include  "Arguments.h"
#include  "OutputScheme.h"
#include  "c/Parser.h"
#include  "cxml/Parser.h"
#include  "c/COutputScheme.h"
#include  "cxml/CXmlOutputScheme.h"
#include  "csharp/CSharpOutputScheme.h"
#include  "csharpxml/CSharpXmlOutputScheme.h"
#include  "dump/DumpOutputScheme.h"

static const char * UsageHeadFormat =
    "Usage: %s {{Options}} Grammar.atg/Grammar.xatg\n"
    "Options:\n";
static const char * UsageTail =
    "If '--' is encountered, all the following arguments are not treated as any options.\n";

static const CcArgDesc_t ArgDescList[] = {
    { 's', "scheme", "SCHEME", NULL,
      "The possible value of SCHEME are 'dump', SCHEME is fetched from atg/xatg if this argument not present." },
    { 'o', "output", "OUTPUT-METHOD", NULL,
      "The possible value of OUTPUT-METHOD are 'auto', 'generate', 'update', 'auto' is default." },
    { 'g', "output", NULL, "generate", "This is a shortcut of '-o generate'." },
    { 'd', "dir", "OUTPUT-DIR", NULL,
      "The value of OUTPUT-DIR is the directory where the outputs are located." },
    { 't', "tempdir", "TEMPLATE-DIR", NULL,
      "The value of TEMPLATE-DIR is the directory where the templates for various schemes are located." }
};
static const CcArgDesc_t * ArgDescLast =
    ArgDescList + sizeof(ArgDescList) / sizeof(ArgDescList[0]);

static CcsBool_t
CmpExtension(const char * fname, const char * ext)
{
    int lf = strlen(fname), le = strlen(ext);
    if (lf < le) return FALSE;
    return !strcmp(fname + (lf - le), ext);
}

int
main(int argc, char * argv[])
{
    CcArguments_t arguments;
    CcArgumentsIter_t iter;
    union {
	CcsParser_t parser;
	CcsXmlParser_t xmlparser;
    } u;
    CcsParser_t * parser;
    CcsXmlParser_t * xmlparser;
    const char * atgName, * schemeName;
    CcOutputScheme_t * scheme;

    printf("Coco/R & CocoXml (2008 ~ 2009)\n");
    CcArguments(&arguments, ArgDescList, ArgDescLast, argc, argv);
    atgName = CcArguments_First(&arguments, "", &iter);
    if (atgName == NULL) {
	printf(UsageHeadFormat, argv[0]);
	CcArguments_ShowHelp(stdout, ArgDescList, ArgDescLast);
	printf(UsageTail);
	return 0;
    }

    parser = NULL; xmlparser = NULL;
    if (CmpExtension(atgName, ".atg")) {
	parser = !strcmp(atgName, "-") ?
	    CcsParser(&u.parser, stdin, stderr) :
	    CcsParser_ByName(&u.parser, atgName, stderr);
	if (!parser) goto errquit0;
	CcsParser_Parse(parser);
	if (!CcGlobals_Finish(&parser->globals)) goto errquit1;
    } else if (CmpExtension(atgName, ".xatg")) {
	xmlparser = !strcmp(atgName, "-") ?
	    CcsXmlParser(&u.xmlparser, stdin, stderr) :
	    CcsXmlParser_ByName(&u.xmlparser, atgName, stderr);
	if (!xmlparser) goto errquit0;
	CcsXmlParser_Parse(xmlparser);
	if (!CcGlobals_Finish(&xmlparser->globals)) goto errquit1;
    } else {
	fprintf(stderr, "The supported extension are: *.atg, *.xatg.\n");
	goto errquit1;
    }

    schemeName = CcArguments_First(&arguments, "scheme", &iter);
    if (schemeName == NULL) {
	if (parser) schemeName = parser->syntax->schemeName;
	else if (xmlparser) schemeName = xmlparser->syntax->schemeName;
	if (schemeName == NULL) schemeName = "dump";
    }
    scheme = NULL;
    if (!strcmp(schemeName, "dump")) {
	if (!(scheme = (CcOutputScheme_t *)
	      CcDumpOutputScheme(parser, xmlparser, &arguments)))
	    goto errquit1;
    } else if (parser) {
	if (!strcmp(schemeName, "c")) {
	    if (!(scheme = (CcOutputScheme_t *)
		  CcCOutputScheme(parser, &arguments)))
		goto errquit1;
	} else if (!strcmp(schemeName, "csharp")) {
	    if (!(scheme = (CcOutputScheme_t *)
		  CcCSharpOutputScheme(parser, &arguments)))
		goto errquit1;
	}
    } else if (xmlparser) {
	if (!strcmp(schemeName, "cxml")) {
	    if (!(scheme = (CcOutputScheme_t *)
		  CcCXmlOutputScheme(xmlparser, &arguments)))
		goto errquit1;
	} else if (!strcmp(schemeName, "csharpxml")) {
	    if (!(scheme = (CcOutputScheme_t *)
		  CcCSharpXmlOutputScheme(xmlparser, &arguments)))
		goto errquit1;
	}
    }
    if (!scheme) {
	fprintf(stderr, "Unsupported scheme '%s' for %s.\n",
		schemeName, atgName);
	goto errquit2;
    }
    printf("scheme '%s' is used.\n", schemeName);
    if (!CcOutputScheme_GenerateOutputs(scheme, schemeName, atgName))
	goto errquit2;
    CcObject_VDestruct((CcObject_t *)scheme);
    if (parser) CcsParser_Destruct(parser);
    else if (xmlparser) CcsXmlParser_Destruct(xmlparser);
    CcArguments_Destruct(&arguments);
    return 0;
 errquit2:
    CcObject_VDestruct((CcObject_t *)scheme);
 errquit1:
    if (parser) CcsParser_Destruct(parser);
    else if (xmlparser) CcsXmlParser_Destruct(xmlparser);
 errquit0:
    CcArguments_Destruct(&arguments);
    return -1;
}

void
CcArguments_ShowHelp(FILE * outfp, const CcArgDesc_t * desc,
		     const CcArgDesc_t * descLast)
{
    const CcArgDesc_t * cur;
    for (cur = desc; cur < descLast; ++cur) {
	if (cur->promptValue)
	    fprintf(outfp, " -%c <%s>\n", cur->ch, cur->promptValue);
	else
	    fprintf(outfp, " -%c\n", cur->ch);
	fprintf(outfp, "    %s\n", cur->help);
    }
}
