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
#include  "c/XmlParser.h"
#include  "c/COutputScheme.h"
#include  "dump/DumpOutputScheme.h"

static const char * usage_format =
    "Usage: %s Grammar.atg {{Option}}\n"
    "Options:\n"
    "  -scheme  <SCHEME>\n"
    "  -u       <UPDATED-FILE>\n"
    "  -ud      <UPDATED-DIR>\n"
    "\n"
    "The possible value of SCHEME is 'c' or 'dump', 'c' is default.\n"
    "Multiple UPDATE-FILEs are possible, they specifies all of the files which should be updated.\n"
    "Multiple UPDATE-DIRs are permitted, all atg specified files in UPDATE-DIRs are updated.\n";

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

    printf("Coco/R (Oct22, 2008)\n");
    CcArguments(&arguments, argc, argv);
    atgName = CcArguments_First(&arguments, "", &iter);
    if (atgName == NULL) {
	printf(usage_format, argv[0]);
	return 0;
    }

    parser = NULL; xmlparser = NULL;
    if (CmpExtension(atgName, ".atg")) {
	if (!(parser = CcsParser(&u.parser, atgName, stderr))) goto errquit0;
	CcsParser_Parse(parser);
	if (!CcsParser_Finish(parser)) goto errquit1;
    } else if (CmpExtension(atgName, ".xatg")) {
	if (!(xmlparser = CcsXmlParser(&u.xmlparser, atgName, stderr))) goto errquit0;
	CcsXmlParser_Parse(xmlparser);
	if (!CcsXmlParser_Finish(xmlparser)) goto errquit1;
    } else {
	fprintf(stderr, "The supported extension are: *.atg, *.xatg.");
	goto errquit0;
    }

    schemeName = CcArguments_First(&arguments, "scheme", &iter);
    if (schemeName == NULL) {
	if (parser) schemeName = parser->schemeName;
	else if (xmlparser) schemeName = xmlparser->schemeName;
	if (schemeName == NULL) schemeName = "dump";
    }
    if (!strcmp(schemeName, "c")) {
	if (!(scheme = (CcOutputScheme_t *)
	      CcCOutputScheme(parser, xmlparser, &arguments)))
	    goto errquit1;
    } else if (!strcmp(schemeName, "dump")) {
	if (!(scheme = (CcOutputScheme_t *)
	      CcDumpOutputScheme(parser, xmlparser, &arguments)))
	    goto errquit1;
    } else {
	scheme = NULL;
    }
    if (scheme) {
	printf("scheme '%s' is used.\n", schemeName);
	if (!CcOutputScheme_GenerateOutputs(scheme, atgName)) goto errquit2;
	CcObject_VDestruct((CcObject_t *)scheme);
    }
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
