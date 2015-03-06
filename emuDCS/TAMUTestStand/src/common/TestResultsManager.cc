/*
 * $Id: $
 */

// class header
#include "emu/pc/TestResultsManager.h"

namespace emu { namespace pc {

using std::endl;
using std::cout;
using std::string;
using std::map;
using std::pair;
using std::multimap;
using std::vector;

void TestResultsManager::evaluateToString(std::vector<std::string> & out, std::string const & board, std::string const & xpath)
{
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr)
  XALAN_USING_XALAN(XalanDOMChar)
  XALAN_USING_XALAN(XPathEvaluator)
  XPathEvaluator eval;
  XObjectPtr xptr = xm_.evaluate(board, eval, xpath);
  if(xptr.null()) return;
  const NodeRefListBase & nodes = xptr->nodeset();
  for(size_t i=0, len = nodes.getLength(); i<len; ++i)
  {
    out.push_back(XMLString::transcode(nodes.item(i)->getNodeValue().c_str()));
  }
}

void TestResultsManager::processDirectory(std::string const & dir_name)
{
  xm_.loadFiles(dir_name);
}

void TestResultsManager::refreshBoardLabels()
{
  xm_.loadFiles(currentPath_);
  boardLabels_ = xm_.getNamesList();
}

void TestResultsManager::refreshTestLabels()
{
  XALAN_USING_XALAN(NodeRefListBase)
  XALAN_USING_XALAN(XObjectPtr)
  XALAN_USING_XALAN(XalanDOMChar)
  XALAN_USING_XALAN(XPathEvaluator)
  XPathEvaluator eval;
  std::set<std::string>::iterator it = boardLabels_.begin();
  for(; it!=boardLabels_.end(); ++it)
  {
    XObjectPtr xptr = xm_.evaluate(*it, eval, "/root/log/test/@label");
    if(xptr.null()) continue;
    const NodeRefListBase & nodes = xptr->nodeset();
    for(size_t i=0, len = nodes.getLength(); i<len; ++i)
    {
      testLabels_.insert(XMLString::transcode(nodes.item(i)->getNodeValue().c_str()));
    }
  }
}

std::set<std::string> TestResultsManager::getBoardLabels()
{
  refreshBoardLabels();
  return boardLabels_;
}
std::set<std::string> TestResultsManager::getTestLabels()
{
  refreshTestLabels();
  return testLabels_;
}

std::vector<std::string> TestResultsManager::getTestTimes(std::string const & board, std::string const & test)
{
  std::vector<std::string> values;
  std::stringstream xpath;
  xpath << "/root/log/test[@label='" << test << "']/@time";
  evaluateToString(values, board, xpath.str());
  return values;
}

std::string TestResultsManager::getLatestTestTime(std::string const & board, std::string const & test)
{
  std::vector<std::string> values;
  std::stringstream xpath;
  xpath << "/root/log/test[@label='" << test << "']/@time";
  evaluateToString(values, board, xpath.str());
  long int max_time = 0;
  for(size_t i=0, len = values.size(); i<len; ++i)
  {
    std::stringstream ss;
    ss << values[i];
    long int time;
    ss >> time;
    if(time > max_time || max_time == 0)
    {
      max_time = time;
    }
  }
  std::stringstream ss;
  ss << max_time;
  return ss.str();
}

std::vector<std::string> TestResultsManager::getBoardLogTimes(std::string const & board)
{
  std::vector<std::string> values;
  std::stringstream xpath;
  xpath << "/root/log/@time";
  evaluateToString(values, board, xpath.str());
  return values;
}

std::string TestResultsManager::getLatestBoardLogTime(std::string const & board)
{
  std::vector<std::string> values;
  std::stringstream xpath;
  xpath << "/root/log/@time";
  evaluateToString(values, board, xpath.str());
  long int max_time = 0;
  for(size_t i=0, len = values.size(); i<len; ++i)
  {
    std::stringstream ss;
    ss << values[i];
    long int time;
    ss >> time;
    if(time > max_time || max_time == 0)
    {
      max_time = time;
    }
  }
  std::stringstream ss;
  ss << max_time;
  return ss.str();
}

std::vector<std::string> TestResultsManager::getLogTestTimes(std::string const & board, std::string const & log_time)
{
  std::vector<std::string> values;
  std::stringstream xpath;
  xpath << "/root/log" << "[@time='" << log_time << "']/test/@time";
  evaluateToString(values, board, xpath.str());
  return values;
}

std::string TestResultsManager::getTestName(std::string const & board, std::string const & test_time)
{
  std::vector<std::string> values;
  std::stringstream xpath;
  xpath << "/root/log/test[@time='" << test_time << "']/@label";
  evaluateToString(values, board, xpath.str());
  if(values.size() > 0)
    return values[0];
  else
    return "";
}

std::string TestResultsManager::getTestResult(std::string const & board, std::string const & test_time)
{
  std::vector<std::string> values;
  std::stringstream xpath;
  xpath << "/root/log/test[@time='" << test_time << "']/result/@value";
  evaluateToString(values, board, xpath.str());
  if(values.size() > 0)
    return values[0];
  else
    return "";
}

std::vector<TestResultsManager::TestError> TestResultsManager::getTestErrors(std::string const & board, std::string const & test_time)
{
  std::vector<TestError> errors;
    XALAN_USING_XALAN(NodeRefListBase)
    XALAN_USING_XALAN(XObjectPtr)
    XALAN_USING_XALAN(XalanDOMChar)
    XALAN_USING_XALAN(XPathEvaluator)
    XPathEvaluator eval;
    std::stringstream xpath;
    xpath << "/root/log/test[@time='" << test_time << "']/error";
    XObjectPtr xptr = xm_.evaluate(board, eval, xpath.str());
    if(xptr.null()) return errors;
    const NodeRefListBase & nodes = xptr->nodeset();
    for(size_t i=0, len = nodes.getLength(); i<len; ++i)
    {
      errors.push_back(TestError());
      XalanDOMString attr;
      XalanNode * attribute;

      attr.assign("errorID");
      attribute = nodes.item(i)->getAttributes()->getNamedItem(attr);
      if(attribute != NULL) errors[i].errorID = XMLString::transcode(attribute->getNodeValue().c_str());

      attr.assign("description");
      attribute = nodes.item(i)->getAttributes()->getNamedItem(attr);
      if(attribute != NULL) errors[i].errorDescription = XMLString::transcode(attribute->getNodeValue().c_str());

      attr.assign("signalID");
      attribute = nodes.item(i)->getAttributes()->getNamedItem(attr);
      if(attribute != NULL) errors[i].signalID = XMLString::transcode(attribute->getNodeValue().c_str());
    }
    return errors;
}

}} // namespaces
