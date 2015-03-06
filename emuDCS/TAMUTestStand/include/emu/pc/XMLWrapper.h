/*
 * XMLWrapper.h
 *
 *  Created on: Jun 30, 2013
 *      Author: Austin Schneider
 */

#ifndef LOGREADER_H_
#define LOGREADER_H_

#include "emu/utils/String.h"
#include "emu/utils/DOM.h"
#include "emu/utils/Xalan.h"
#include "emu/pc/BasicTable.h"
#include "emu/pc/TestUtils.h"

#include <string>
#include <cerrno>
#include <fstream>

#include "xercesc/dom/DOMXPathResult.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/StdOutFormatTarget.hpp"
#include "xercesc/framework/MemBufFormatTarget.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/SAXParseException.hpp"

#include "xalanc/PlatformSupport/XSLException.hpp"
#include "xalanc/XPath/XObject.hpp"
#include "xalanc/XPath/XPathEvaluator.hpp"
#include "xalanc/XPath/NodeRefList.hpp"
#include "xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp"
#include "xalanc/XercesParserLiaison/XercesParserLiaison.hpp"
#include "xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp"
#include "xalanc/XSLT/XSLTInputSource.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeInit.hpp"

namespace emu {
namespace pc {

XALAN_USING_XALAN(XalanDocument)
XALAN_USING_XALAN(XalanNode)
XALAN_USING_XALAN(XalanDOMString)
XALAN_USING_XERCES(XMLPlatformUtils)
XALAN_USING_XALAN(XPathEvaluator)
XALAN_USING_XALAN(XercesDOMSupport)
XALAN_USING_XALAN(XercesParserLiaison)
XALAN_USING_XALAN(NodeRefList)
XALAN_USING_XALAN(NodeRefListBase)
XALAN_USING_XALAN(XalanDocumentPrefixResolver);
XALAN_USING_XALAN(XObjectPtr);

/*
 * Class to handle initialization of XMLPlatformUtils and XPathEvaluator
 */
class XPathInitializer {
public:
  XPathInitializer()
  {
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();
  }
  ~XPathInitializer()
  {
    XPathEvaluator::terminate();
    XMLPlatformUtils::Terminate();
  }
private:
  XPathInitializer(const XPathInitializer &);
  XPathInitializer& operator=(const XPathInitializer &);
};


/*
 * Provides a simple interface for xpath evaluation
 * Note: If no XPathEvaluator is specified the internal XPathEvaluator is used.
 * When using the internal XPathEvaluator only the most recently returned XObjectPtr is valid.
 */
class XMLWrapper {
private:
  XPathInitializer init;
  std::string plainText;
  std::string filePath;
  XercesDOMSupport support;
  XercesParserLiaison liaison;
  XalanDocument * XMLDoc;
  XalanDocumentPrefixResolver * resolver;

  XalanDocument * parseXMLDoc();

public:
  XMLWrapper(std::string);
  ~XMLWrapper();
  void refresh();
  XObjectPtr evaluate(XPathEvaluator &, XMLCh *);
  XObjectPtr evaluate(XPathEvaluator &, std::string);

  static void dumpFileToString(const char *filename, std::string & out)
  {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
      unsigned int init_size = (unsigned int)out.size();
      in.seekg(0, std::ios::end);
      out.resize((unsigned int)in.tellg() + init_size);
      in.seekg(0, std::ios::beg);
      in.read(&out[init_size], out.size());
      in.close();
    }
    else
      throw(errno);
  }

private:
  XMLWrapper();
  XMLWrapper(const XMLWrapper &);
  XMLWrapper& operator=(const XMLWrapper &);
};


}
}


#endif /* LOGREADER_H_ */
