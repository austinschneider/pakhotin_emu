/*
 * XMLManager.h
 *
 *  Created on: Jul 3, 2013
 *      Author: cscdev
 */

#ifndef XMLMANAGER_H_
#define XMLMANAGER_H_

//System Includes
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <time.h>
#include <stdio.h>

//Emu Includes
#include "emu/utils/String.h"
#include "emu/utils/DOM.h"
#include "emu/utils/Xalan.h"
#include "emu/pc/BasicTable.h"
#include "emu/pc/TestUtils.h"
#include "emu/pc/XMLWrapper.h"

//Library Includes
#include <boost/regex.hpp>
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"

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


class XMLManager {
private:
  boost::regex nameFormat;
  unsigned int nameSubmatch;

  std::vector<XMLWrapper *> fileList_;
  std::set<std::string> namesList_;
  std::map<std::string, boost::filesystem::path> paths_;
  std::map<std::string, XMLWrapper *> files_;
  std::map<std::string, std::time_t> lastModified_;

public:
  XMLManager();
  XMLManager(std::string, unsigned int);
  ~XMLManager();

  void setNameFormat(std::string, unsigned int);

  std::set<std::string> getNamesList();

  void loadFiles(std::string);
  void loadFile(std::string);
  bool refresh(std::string);
  bool refresh();

  XObjectPtr evaluate(std::string, XPathEvaluator &, XMLCh *);
  XObjectPtr evaluate(std::string, XPathEvaluator &, std::string);

};

}
} // namespaces

#endif /* XMLMANAGER_H_ */
