/*
 * XMLManager.cc
 *
 *  Created on: Jul 3, 2013
 *      Author: cscdev
 */

#include "emu/pc/XMLManager.h"

namespace emu {
namespace pc {


XMLManager::XMLManager()
{
}

XMLManager::XMLManager(std::string r, unsigned int r_n):
    nameFormat(r),
    nameSubmatch(r_n)
{
}

XMLManager::~XMLManager()
{
  for(unsigned int i=0; i<fileList_.size(); ++i)
    delete fileList_[i];
}

void XMLManager::setNameFormat(std::string r, unsigned int r_n)
{
  nameFormat.assign(r);
  nameSubmatch = r_n;
}

std::set<std::string> XMLManager::getNamesList()
{
  return namesList_;
}

void XMLManager::loadFiles(std::string path_s)
{
  std::cout << __func__ << " " << path_s << std::endl;
  namespace fs = boost::filesystem;
  try {
    fs::path path(path_s);
    fs::directory_iterator end_iter;

    //Verify path
    if(fs::exists(path) && fs::is_directory(path))
    {
      //Iterate over files in directory
      for(fs::directory_iterator dir_iter(path) ; dir_iter != end_iter ; ++dir_iter)
      {
        fs::path file(*dir_iter);
        if(!fs::is_directory(file))
            loadFile(file.string());
      }
    }
  }
  catch(...) {
    std::cout << "Error bad file name!" << std::endl;
  }
}

void XMLManager::loadFile(std::string path_s)
{
  try {
    boost::filesystem::path file(path_s);
    boost::smatch matches;

    std::string name;

    if(boost::regex_match(file.string(), matches, nameFormat))
    {
      name = matches[nameSubmatch];
      if(namesList_.find(name) == namesList_.end())
      {
        XMLWrapper * xml_file = new XMLWrapper(path_s);
        namesList_.insert(name);
        fileList_.push_back(xml_file);
        files_[name] = xml_file;
        paths_[name] = file;
        lastModified_[name] = boost::filesystem::last_write_time(file);
      }
      else
      {
        if(lastModified_[name] < boost::filesystem::last_write_time(file))
          files_[name]->refresh();
      }
    }
  }
  catch(...) {
    std::cout << "Error bad file name!" << std::endl;
  }
}

bool XMLManager::refresh(std::string name)
{
  if(lastModified_[name] < boost::filesystem::last_write_time(paths_[name]))
  {
    files_[name]->refresh();
    return true;
  }
  else
  {
    return false;
  }
}

bool XMLManager::refresh()
{
  bool b = false;
  std::set<std::string>::iterator it = namesList_.begin();
  for(; it!=namesList_.begin(); ++it)
  {
    b |= refresh(*it);
  }
  return b;
}

XObjectPtr XMLManager::evaluate(std::string name, XPathEvaluator & eval, XMLCh * chp)
{
  return files_[name]->evaluate(eval, chp);
}

XObjectPtr XMLManager::evaluate(std::string name, XPathEvaluator & eval, std::string s)
{
  return files_[name]->evaluate(eval, s);
}


}
} // namespaces


