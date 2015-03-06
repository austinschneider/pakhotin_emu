/*
 * TestResultsManager.h
 *
 *  Created on: Jul 26, 2012
 *      Author: Austin Schneider
 */

#ifndef TESTRESULTSMANAGER_H_
#define TESTRESULTSMANAGER_H_


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
#include "emu/pc/XMLManager.h"

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

namespace emu { namespace pc {

class TestResultsManager
{
public:
  struct TestError {
    std::string errorID;
    std::string errorDescription;
    std::string signalID;
  };

private:
  // the last directory path to be set
  std::string currentPath_;

  std::set<std::string> boardLabels_;

  std::set<std::string> testLabels_;

  XMLManager xm_;

  void refreshBoardLabels();
  void refreshTestLabels();
  void evaluateToString(std::vector<std::string> &, std::string const &, std::string const &);

public:

  // Default Constructor
  TestResultsManager()
  {
    xm_.setNameFormat("(.*)(Board_)(.+)", 3);
  };

  // Load logs on construction
  TestResultsManager(const std::string dir_name)
  {
    TestResultsManager();
    processDirectory(dir_name);
  };

  /// Process a directory of log files for test results
  void processDirectory(std::string const & dir_name);

  std::set<std::string> getBoardLabels();

  std::set<std::string> getTestLabels();

  std::vector<std::string> getTestTimes(std::string const &, std::string const &);

  std::string getLatestTestTime(std::string const &, std::string const &);

  std::vector<std::string> getBoardLogTimes(std::string const &);

  std::string getLatestBoardLogTime(std::string const &);

  std::vector<std::string> getLogTestTimes(std::string const &, std::string const &);

  std::string getTestName(std::string const &, std::string const &);

  std::string getTestResult(std::string const &, std::string const &);

  std::vector<TestError> getTestErrors(std::string const &, std::string const &);
};

} } //namespaces



#endif /* TESTRESULTSMANAGER_H_ */
