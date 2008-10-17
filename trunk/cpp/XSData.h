#if !defined(COCOXML_XSDATA_H__)
#define COCOXML_XSDATA_H__

#include <stddef.h>
#include <vector>
#include <map>
#include "Symbol.h"
#include "Node.h"
#include "Tab.h"

using namespace std;

namespace CocoXml {
struct options_s{
    const char* name;
    const int   value;
};

typedef enum {
    UNKNOWN_NAMESPACE, END_UNKNOWN_NAMESPACE,
    UNKNOWN_TAG, END_UNKNOWN_TAG,
    UNKNOWN_ATTR_NAMESPACE, UNKNOWN_ATTR,
    UNKNOWN_PROCESSING_INSTRUCTION,
    // For the nodes in common status.
    TEXT, CDATA, COMMENT, WHITESPACE,
    // For the nodes in Unknown namespaces.
    UNS_TEXT, UNS_CDATA, UNS_COMMENT, UNS_WHITESPACE,
    // For the nodes in Unknown tags.
    UT_TEXT, UT_CDATA, UT_COMMENT, UT_WHITESPACE
}Options;

extern options_s enum_options[];
class TagInfo{
public:
    int startToken;
    int endToken;
};

class XmlLangDefinition{
public:
    Tab                             *tab;
    Errors                          *errors;
    vector<bool>                    useVector;
    map<const wchar_t*, int>        optionsMap;
    map<const wchar_t*, TagInfo*>    Tags;
    map<const wchar_t*, int>        Attrs;
    map<const wchar_t*, int>        PInstructions;

    XmlLangDefinition(Tab *tab, Errors *errors);
    
    int addUniqueToken(const wchar_t* tokenName, const wchar_t* typeName, int line);
    void AddOption(const wchar_t* optname, int liane);
    void AddTag(const wchar_t* tagname, wchar_t* tokenname, int line);
    void AddAttr(const wchar_t* attrname, const wchar_t* tokenname, int line);
    void AddProcessingInstruction(const wchar_t* pinstruction, const wchar_t* tokenname, int line);
    void Write(FILE* gen);
};

class XmlScannerData{
public:
    Tab         *tab;
    Errors      *errors;
    FILE        *fram;
    FILE        *gen;

    map<const wchar_t*, XmlLangDefinition*>  XmlLangMap;

    XmlScannerData(Parser *parser);
    void Add(const wchar_t* NamespaceURI, XmlLangDefinition *xldef);
    void OpenGen(const wchar_t *genName, bool backUp);
    void CopyFramePart(const wchar_t * stop);
    void WriteOptions();
    void WriteDeclarations();
    void WriteInitialization();
    void WriteXmlScanner();
};

} //namespace

#endif
