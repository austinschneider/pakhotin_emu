/*
 * LogReader.cc
 *
 *  Created on: Jun 30, 2013
 *      Author: Austin Schneider
 */


#include "emu/pc/XMLWrapper.h"

namespace emu {
namespace pc {

XalanDocument * XMLWrapper::parseXMLDoc()
{
  dumpFileToString(filePath.c_str(), plainText);
  plainText = "<root>" + plainText + "</root>";
  liaison.setDoNamespaces(true); // although it seems to be already set...
  liaison.setBuildWrapperNodes(true);
  liaison.setBuildMaps(true);

  const char* const id = "dummy";
  MemBufInputSource theInputSource((const XMLByte*) plainText.c_str(), (unsigned int) plainText.size(), id);
  return liaison.parseXMLStream(theInputSource);
}

XMLWrapper::XMLWrapper(std::string path)
:init(),
 filePath(path),
 support(),
 liaison(support),
 XMLDoc(parseXMLDoc()),
 resolver(new XalanDocumentPrefixResolver(XMLDoc))
{
  plainText.clear();
}

XMLWrapper::~XMLWrapper()
{
  delete resolver;
}

void XMLWrapper::refresh()
{
  XMLDoc = parseXMLDoc();
  plainText.clear();
  if(resolver != NULL) delete resolver;
  resolver = new XalanDocumentPrefixResolver(XMLDoc);
}

XObjectPtr XMLWrapper::evaluate(XPathEvaluator & eval, XMLCh * xpath)
{
  XObjectPtr xop = eval.evaluate(support, XMLDoc, xpath, *resolver);
  return xop;
}

XObjectPtr XMLWrapper::evaluate(XPathEvaluator & eval, std::string xpath_s)
{
  XMLCh * xpath(XMLString::transcode(xpath_s.c_str()));
  XObjectPtr xop = evaluate(eval, xpath);
  XMLString::release(&xpath);
  return xop;
}

}
}

