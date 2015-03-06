/*
 * TestLogger.cc
 *
 *  Created on: Mar 7, 2013
 *      Author: Austin Schneider
 */

// class header
#include "emu/pc/TestLogger.h"
#include <time.h>

namespace emu { namespace pc {

TestLogger::TestLogger():
    testing(false),
    logging(false),
    //tester(),
    currentBoard(),
    currentTest(),
    workingDirectory(DEFAULT_LOGGING_DIRECTORY),
    log()
{
}
TestLogger::TestLogger(std::string t):
    testing(false),
    logging(false),
    //tester(t),
    currentBoard(),
    currentTest(),
    workingDirectory(DEFAULT_LOGGING_DIRECTORY),
    log()
{
}
void TestLogger::openFile(std::string boardName)
{
  testing  = true;
  time_t t = time(NULL);
  std::stringstream ss;
  ss << workingDirectory << "/";
  ss << "Board_" << boardName;
  log.open(ss.str().c_str(), std::ios_base::out | std::ios_base::app);
  std::cout << "Opening File: " << ss.str() << std::endl;
  log << "<log board=\"" << boardName << "\" time=\"" << t << "\">" << std::endl;
}
void TestLogger::closeFile()
{
  testing  = false;
  log << "</log>\n";
  log.close();
}
void TestLogger::startTest(std::string testName)
{
  currentTest = testName;
  if(logging)
  {
    log << "<test label=\"" << currentTest << "\" time=\"" << time(NULL) <<"\">" << std::endl;
  }
  else
    log << "NOT LOGGING" << std::endl;
}
void TestLogger::endTest(int result)
{
  if(logging)
  {
    log << "<result value=\"" << result << "\"></result>" << std::endl;
    log << "</test>" << std::endl;
  }
}
bool TestLogger::isTesting()
{
  return testing;
}
void TestLogger::reportError(TestError & error)
{
  if(logging)
  {
    log << "<error errorID=\"" << error.errorID.str()
        << "\" description=\"" << error.errorDescription.str()
        << "\" signalID=\"" << error.signalID.str()
        << "\"></error>"
        << std::endl;
    error.errorID.str(std::string());
    error.errorDescription.str(std::string());
    error.signalID.str(std::string());
  }
}
void TestLogger::resumeLogging()
{
  logging = true;
}
void TestLogger::pauseLogging()
{
  logging = false;
}
bool TestLogger::isLogging()
{
  return logging;
}
void TestLogger::setBoard(std::string board)
{
  if(board == "") return;
  logging = true;
  currentBoard = board;
  if(log.is_open()) closeFile();
  openFile(board);
}
void TestLogger::setDNA(std::string dna)
{
  if(logging)
  {
    if(log.is_open())
    {
      log << "<DNA value=\"" << dna << "\"></DNA>" << std::endl;
    }
  }
}
std::string TestLogger::getBoard()
{
  return currentBoard;
}
void TestLogger::setWorkingDirectory(std::string dir)
{
  workingDirectory = dir;
}



} } //Namespaces
