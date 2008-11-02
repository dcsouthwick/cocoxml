#include <iostream>
#include "XmlParser.hxx"
#include "XmlScanner.hxx"
#include "RSSData.hxx"

using namespace cocoxml;

int main(int argc, char **argv)
{
    if (argc != 2){
        std::cerr << "Usage:" << argv[0] << " <xml file>\n" << std::endl;
        return 1;
    }
    XmlScanner *scanner = new XmlScanner(coco_string_create(argv[1]));
    XmlParser *parser = new XmlParser(scanner);
    
    parser->Parse();
    std::cout << parser->rss << std::endl;
    return 0;
}