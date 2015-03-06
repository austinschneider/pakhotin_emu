/*
 * TestResultsManagerModule.cc
 *
 *  Created on: Aug 1, 2012
 *      Author: Austin Schneider
 */

#include "emu/pc/TestResultsManagerModule.h"

#include "boost/lexical_cast.hpp"

// Emu includes
#include "emu/pc/TestLogger.h"
#include "emu/utils/Cgi.h"

// XDAQ includes
#include "cgicc/Cgicc.h"
#include "cgicc/HTMLClasses.h"
#include "xgi/Utils.h"
#include "xdaq/WebApplication.h"

// system includes
#include <string>
#include <vector>
#include <set>

namespace emu { namespace pc {

using std::endl;
using std::cout;
using std::string;
using std::map;
using std::vector;
using std::set;

TestResultsManagerModule::TestResultsManagerModule(xdaq::WebApplication *app)
: app_(app)
, trm_()
{
}

void TestResultsManagerModule::TestResultsManagerPage(xgi::Input * in, xgi::Output * out )
{
  using cgicc::tr;
  using cgicc::form;
  using cgicc::input;

  cgicc::Cgicc cgi(in);
  string urn = app_->getApplicationDescriptor()->getURN();

  ///////////////////////////////////////////
  // Get POST data
  cgicc::form_iterator name;
  string s = "";

  int pageMode_ = BOARD_SUMMARY;
  name = cgi.getElement("mode");
  if(name != cgi.getElements().end())
  {
    s = cgi["mode"]->getValue();
    pageMode_ = atoi(s.c_str());
  }

  emu::utils::headerXdaq(out, app_,"Test Results Manager");

  *out << "<table cellpadding=\"2\" cellspacing=\"2\" border=\"0\" style=\"width: 100%; font-family: arial;\">" << endl;
  *out << "<tbody>" << endl;
  *out << "<tr>" << endl;
  *out << "<td width=\"50%\">" << endl;

  ///////////////////////////////////////////
  // Results Manager Controls:

  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << endl;
  *out << cgicc::legend("Results Manager Controls:").set("style", "color:blue") <<endl;

  *out << form().set("method","GET").set("action", "/" + urn + "/TestResultsManagerPage" ) << endl;
  *out << input().set("type","submit").set("value", "Test Results Home").set("style", "color:blue") << endl;
  *out << input().set("type", "hidden").set("value", boost::lexical_cast<std::string>((int)BOARD_SUMMARY)).set("name", "mode") << endl;
  *out << form() << endl;
  *out << cgicc::br() << endl;
  *out << form().set("method","GET").set("action", "/" + urn + "/ProcessLogDirectory" ) << endl;
  *out << input().set("type", "text").set("name", "path").set("size", "100").set("value", DEFAULT_LOGGING_DIRECTORY) << endl;
  *out << input().set("type","submit").set("value", "Process Log Directory").set("style", "color:blue") << endl;
  *out << form() << endl;

  *out << cgicc::fieldset() << endl;


  ///////////////////////////////////////////
  // Results Table:

  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << endl;

  if(pageMode_ == BOARD_SUMMARY)
    BoardSummary(in, out);
  else if(pageMode_ == LOG_SUMMARY)
    LogSummary(in, out);
  else if(pageMode_ == TEST_SUMMARY)
    TestSummary(in, out);
  else if(pageMode_ == TEST_LIST)
    TestList(in, out);
  else if(pageMode_ == LOG_DETAILS)
    LogDetails(in, out);
  else if(pageMode_ == TEST_DETAILS)
    TestDetails(in, out);
  else if(pageMode_ == ERROR_DETAILS)
    ErrorDetails(in, out);

  *out << cgicc::fieldset() << endl;


  *out << "</td>" << endl;
  *out << "</tr>" << endl;
  *out << "</tbody>" << endl;
  *out << "</table>" << endl;

  emu::utils::footer(out);
}

void TestResultsManagerModule::BoardSummary(xgi::Input * in, xgi::Output * out)
{
  using cgicc::a;
  using cgicc::tr;
  using cgicc::td;
  using cgicc::table;
  using cgicc::form;
  using cgicc::input;

  string urn = app_->getApplicationDescriptor()->getURN();

  // section label
  *out << cgicc::legend("Boards:").set("style", "color:blue") <<endl;

  // table
  *out << table().set("border", "1") << endl;
  *out << tr() << endl;

  // column headers
  *out << td() << "Board" << td() << endl;
  *out << td() << "Last Log Time" << td() << endl;
  *out << td() << "Result Status" << td() << endl;
  *out << td() << "Testing Status" << td() << endl;

  *out << tr() << endl;

  set<string> board_labels = trm_.getBoardLabels();
  set<string> test_labels = trm_.getTestLabels();

  // iterate over board labels (each board label corresponds to a single board)
  for(set<string>::iterator i = board_labels.begin(); i != board_labels.end(); ++i)
  {
    bool fail = false;
    bool tested = true;

    std::string log_time_s = trm_.getLatestBoardLogTime(*i);
    std::stringstream ss;
    ss << log_time_s;
    long int log_time;
    ss >> log_time;
    for(set<string>::iterator j = test_labels.begin(); j != test_labels.end(); ++j)
    {
      // check the result of the latest test time for each test
      std::string test_time = trm_.getLatestTestTime(*i,*j);
      std::string result = trm_.getTestResult(*i,test_time);
      if(result == "0")
      {
      }
      else if(result == "")
      {
        tested &= false;
      }
      else
      {
        fail |= true;
      }
    }

    *out << tr() << endl;

    *out << td() << *i << td() << endl; // board
    *out << td() << timeToString(log_time) << td() << endl; // last log time

    // board status (based on last instance of each test type)
    *out << "<td style =\"color:";
    if(fail)
      *out << "red\">Bad";
    else
      *out << "green\">Good";
    *out << "</td>" << endl;

    *out << "<td style =\"color:";
    if(tested)
      *out << "green\">Fully tested";
    else
      *out << "red\">Not fully tested";
    *out << "</td>" << endl;

    // link to log summary of the board
    *out << td() << endl;
    *out << a().set("href", "/" + urn + "/TestResultsManagerPage?mode=" + boost::lexical_cast<std::string>((int)LOG_SUMMARY)
        + "&board_label=" + *i) << endl;
    *out << "Log Summary" << endl;
    *out << a() << endl;
    *out << td() << endl;

    // link to test summary of the board
    *out << td() << endl;
    *out << a().set("href", "/" + urn + "/TestResultsManagerPage?mode=" + boost::lexical_cast<std::string>((int)TEST_SUMMARY)
        + "&board_label=" + *i) << endl;
    *out << "Test Summary" << endl;
    *out << a() << endl;
    *out << td() << endl;

    *out << tr() << endl;
  }

  *out << table() << endl;
}

void TestResultsManagerModule::LogSummary(xgi::Input * in, xgi::Output * out)
{
  using cgicc::a;
  using cgicc::tr;
  using cgicc::td;
  using cgicc::table;
  using cgicc::form;
  using cgicc::input;

  cgicc::Cgicc cgi(in);
  string urn = app_->getApplicationDescriptor()->getURN();

  ///////////////////////////////////////////
  // Get POST data
  cgicc::form_iterator name;

  std::string board;
  name = cgi.getElement("board_label");
  if(name != cgi.getElements().end())
  {
    board = cgi["board_label"]->getValue();
  }

  // section label
  *out << cgicc::legend("Board_" + board + ": Log Summary").set("style", "color:blue") <<endl;

  // table
  *out << table().set("border", "1") << endl;
  *out << tr() << endl;

  // column headers
  *out << td() << "Log Time" << td() << endl;

  *out << tr() << endl;

  std::vector<std::string> log_times = trm_.getBoardLogTimes(board);

  for(int i=log_times.size()-1; i>=0; --i)
  {
    std::stringstream ss;
    long long log_time;
    ss << log_times[i];
    ss >> log_time;

    *out << tr() << endl;

    *out << td() << timeToString(log_time) << td() << endl; // log time

    // button linking to test summary of the board
    *out << td() << endl;
    *out << a().set("href", "/" + urn + "/TestResultsManagerPage?mode=" + boost::lexical_cast<std::string>((int)LOG_DETAILS)
        + "&board_label=" + board + "&log_time=" + log_times[i]) << endl;
    *out << "Log Details" << endl;
    *out << a() << endl;
    *out << td() << endl;

    *out << tr() << endl;
  }

  *out << table() << endl;
}

void TestResultsManagerModule::TestSummary(xgi::Input * in, xgi::Output * out)
{
  using cgicc::a;
  using cgicc::tr;
  using cgicc::td;
  using cgicc::table;
  using cgicc::form;
  using cgicc::input;

  cgicc::Cgicc cgi(in);
  string urn = app_->getApplicationDescriptor()->getURN();

  ///////////////////////////////////////////
  // Get POST data
  cgicc::form_iterator name;

  std::string board;
  name = cgi.getElement("board_label");
  if(name != cgi.getElements().end())
  {
    board = cgi["board_label"]->getValue();
  }

  // section label
  *out << cgicc::legend("Board_" + board + ": Test Summary").set("style", "color:blue") <<endl;

  // results table
  *out << table().set("border", "1") << endl;
  *out << tr() << endl;

  // column headers
  *out << td() << "Test" << td() << endl;
  *out << td() << "Last Tested" << td() << endl;
  *out << td() << "Result" << td() << endl;

  *out << tr() << endl;

  std::set<std::string> test_labels = trm_.getTestLabels();

  // iterate over test labels
  for(set<string>::iterator i = test_labels.begin(); i != test_labels.end(); ++i)
  {
    std::string test_time_s = trm_.getLatestTestTime(board, *i);
    std::string result = trm_.getTestResult(board,test_time_s);
    *out << tr() << endl;

    std::stringstream ss;
    long int test_time;
    ss << test_time_s;
    ss >> test_time;

    std::pair<std::string,std::string> n_r = niceResult(result);

    *out << td() << *i << td() << endl; // test
    if(test_time)
    {
      *out << td() << timeToString(test_time) << td() << endl; // time
      *out << td().set("style", n_r.second) << n_r.first << td() << endl; // result
    }
    else
    {
      *out << td() << "Never" << td() << endl; // time
      *out << td() << td() << endl; // result
    }

    // link to test details
    *out << td() << endl;
    *out << a().set("href", "/" + urn + "/TestResultsManagerPage?mode=" + boost::lexical_cast<std::string>((int)TEST_DETAILS)
        + "&board_label=" + board + "&test_time=" + test_time_s) << endl;
    *out << "Last Test Details" << endl;
    *out << a() << endl;
    *out << td() << endl;

    // link to test list
    *out << td() << endl;
    *out << a().set("href", "/" + urn + "/TestResultsManagerPage?mode=" + boost::lexical_cast<std::string>((int)TEST_LIST)
        + "&board_label=" + board + "&test_label=" + *i) << endl;
    *out << "Test List" << endl;
    *out << a() << endl;
    *out << td() << endl;

    *out << tr() << endl;
  }

  *out << table() << endl;
}

void TestResultsManagerModule::TestList(xgi::Input * in, xgi::Output * out)
{
  using cgicc::a;
  using cgicc::tr;
  using cgicc::td;
  using cgicc::table;
  using cgicc::form;
  using cgicc::input;

  cgicc::Cgicc cgi(in);
  string urn = app_->getApplicationDescriptor()->getURN();

  ///////////////////////////////////////////
  // Get POST data
  cgicc::form_iterator name;

  std::string board;
  name = cgi.getElement("board_label");
  if(name != cgi.getElements().end())
  {
    board = cgi["board_label"]->getValue();
  }

  std::string test;
  name = cgi.getElement("test_label");
  if(name != cgi.getElements().end())
  {
    test = cgi["test_label"]->getValue();
  }

  // section label
  *out << cgicc::legend("Board_" + board + ": Test History: " + test).set("style", "color:blue") <<endl;

  // table
  *out << table().set("border", "1") << endl;
  *out << tr() << endl;

  // column headers
  *out << td() << "Time" << td() << endl;
  *out << td() << "Result" << td() << endl;

  *out << tr() << endl;

  std::vector<std::string> test_times = trm_.getTestTimes(board, test);

  for(int i = test_times.size()-1; i>=0; --i)
  {
    std::string result = trm_.getTestResult(board,test_times[i]);
    *out << tr() << endl;

    std::stringstream ss;
    long int test_time;
    ss << test_times[i];
    ss >> test_time;

    std::pair<std::string,std::string> n_r = niceResult(result);

    *out << td() << timeToString(test_time) << td() << endl; // time
    *out << td().set("style", n_r.second) << n_r.first << td() << endl; // result

    // link to test details
    *out << td() << endl;
    *out << a().set("href", "/" + urn + "/TestResultsManagerPage?mode=" + boost::lexical_cast<std::string>((int)TEST_DETAILS)
        + "&board_label=" + board + "&test_time=" + test_times[i]) << endl;
    *out << "Test Details" << endl;
    *out << a() << endl;
    *out << td() << endl;

    *out << tr() << endl;
  }

  *out << table() << endl;
}

void TestResultsManagerModule::LogDetails(xgi::Input * in, xgi::Output * out)
{
  using cgicc::a;
  using cgicc::tr;
  using cgicc::td;
  using cgicc::table;
  using cgicc::form;
  using cgicc::input;

  cgicc::Cgicc cgi(in);

  ///////////////////////////////////////////
  // Get POST data
  cgicc::form_iterator name;

  std::string board;
  name = cgi.getElement("board_label");
  if(name != cgi.getElements().end())
  {
    board = cgi["board_label"]->getValue();
  }

  std::string log_time;
  name = cgi.getElement("log_time");
  if(name != cgi.getElements().end())
  {
    log_time = cgi["log_time"]->getValue();
  }

  string urn = app_->getApplicationDescriptor()->getURN();

  // section label
  *out << cgicc::legend("Board_" + board + ": Log " + log_time + ": Log Details").set("style", "color:blue") <<endl;

  // results table
  *out << table().set("border", "1") << endl;
  *out << tr() << endl;

  // column headers
  *out << td() << "Test" << td() << endl;
  *out << td() << "Test Time" << td() << endl;
  *out << td() << "Test Result" << td() << endl;

  *out << tr() << endl;

  std::vector<std::string> test_times = trm_.getLogTestTimes(board, log_time);

  // iterate over test times
  for(int i=test_times.size()-1; i>=0; --i)
  {
    std::string test_name  = trm_.getTestName(board, test_times[i]);
    std::string test_result = trm_.getTestResult(board, test_times[i]);
    *out << tr() << endl;

    std::pair<std::string,std::string> n_r = niceResult(test_result);

    *out << td() << test_name << td() << endl; // test
    *out << td() << test_times[i] << td() << endl; // test time
    *out << td().set("style", n_r.second) << n_r.first << td() << endl; // result

    // button linking to test details
    *out << td() << endl;
    *out << a().set("href", "/" + urn + "/TestResultsManagerPage?mode=" + boost::lexical_cast<std::string>((int)TEST_DETAILS)
        + "&board_label=" + board + "&test_time=" + test_times[i]) << endl;
    *out << "Test Details" << endl;
    *out << a() << endl;
    *out << td() << endl;

    *out << tr() << endl;
  }

  *out << table() << endl;
}

void TestResultsManagerModule::TestDetails(xgi::Input * in, xgi::Output * out)
{
  using cgicc::a;
  using cgicc::tr;
  using cgicc::td;
  using cgicc::table;
  using cgicc::form;
  using cgicc::input;
  using cgicc::form_urlencode;

  cgicc::Cgicc cgi(in);
  string urn = app_->getApplicationDescriptor()->getURN();

  ///////////////////////////////////////////
  // Get POST data
  cgicc::form_iterator name;

  std::string board;
  name = cgi.getElement("board_label");
  if(name != cgi.getElements().end())
  {
    board = cgi["board_label"]->getValue();
  }

  std::string test_time_s;
  name = cgi.getElement("test_time");
  if(name != cgi.getElements().end())
  {
    test_time_s = cgi["test_time"]->getValue();
  }

  std::stringstream ss;
  long int test_time;
  ss << test_time_s;
  ss >> test_time;

  std::string test = trm_.getTestName(board, test_time_s);

  // section label
  *out << cgicc::legend("Board_" + board + ": Test Details: " + test + ": " + timeToString(test_time)).set("style", "color:blue") <<endl;

  // table
  *out << table().set("border", "1") << endl;
  *out << tr() << endl;

  // column headers
  *out << td() << "Error ID" << td() << endl;
  *out << td() << "Signal ID" << td() << endl;

  *out << tr() << endl;

  std::vector<TestError> test_errors = trm_.getTestErrors(board, test_time_s);

  for(int i=0; i<test_errors.size(); ++i)
  {
    *out << tr() << endl;

    *out << td() << test_errors[i].errorID << td() << endl; // errorID
    *out << td() << test_errors[i].signalID << td() << endl; // signalID

    // button linking to error details
    *out << td() << endl;
    *out << a().set("href", "/" + urn + "/TestResultsManagerPage?mode=" + boost::lexical_cast<std::string>((int)ERROR_DETAILS)
        + "&board_label=" + board + "&test_time=" + test_time_s
        + "&error_id=" + test_errors[i].errorID + "&error_description=" + form_urlencode(test_errors[i].errorDescription)
        + "&signal_id=" + test_errors[i].signalID) << endl;
    *out << "Error Details" << endl;
    *out << a() << endl;
    *out << td() << endl;

    *out << tr() << endl;
  }

  *out << table() << endl;
}

void TestResultsManagerModule::ErrorDetails(xgi::Input * in, xgi::Output * out)
{
  using cgicc::a;
  using cgicc::tr;
  using cgicc::td;
  using cgicc::table;
  using cgicc::form;
  using cgicc::input;
  using cgicc::form_urldecode;

  cgicc::Cgicc cgi(in);
  string urn = app_->getApplicationDescriptor()->getURN();

  ///////////////////////////////////////////
  // Get POST data
  cgicc::form_iterator name;

  std::string board;
  name = cgi.getElement("board_label");
  if(name != cgi.getElements().end())
  {
    board = cgi["board_label"]->getValue();
  }

  std::string test_time_s;
  name = cgi.getElement("test_time");
  if(name != cgi.getElements().end())
  {
    test_time_s = cgi["test_time"]->getValue();
  }

  std::string error_id;
  name = cgi.getElement("error_id");
  if(name != cgi.getElements().end())
  {
    error_id = cgi["error_id"]->getValue();
  }

  std::string error_description;
  name = cgi.getElement("error_description");
  if(name != cgi.getElements().end())
  {
    error_description = form_urldecode(cgi["error_description"]->getValue());
  }

  std::string signal_id;
  name = cgi.getElement("signal_id");
  if(name != cgi.getElements().end())
  {
    signal_id = cgi["signal_id"]->getValue();
  }

  //std::string signalDetails = getsSignalDetails();
  std::string signal_details = "";

  std::stringstream ss;
  long int test_time;
  ss << test_time_s;
  ss >> test_time;

  std::string test = trm_.getTestName(board, test_time_s);

  // section label
  *out << cgicc::legend("Board_" + board + ": " + test + ": " + timeToString(test_time) + ": Error Details").set("style", "color:blue") <<endl;

  // table
  *out << table().set("border", "1") << endl;

  *out << tr() << endl;
  *out << td() << "Error ID" << td() << endl;
  *out << td() << error_id << td() << endl;
  *out << tr() << endl;

  *out << tr() << endl;
  *out << td() << "Error Description" << td() << endl;
  *out << td() << error_description << td() << endl;
  *out << tr() << endl;

  *out << tr() << endl;
  *out << td() << "Signal ID" << td() << endl;
  *out << td() << signal_id << td() << endl;
  *out << tr() << endl;

  *out << tr() << endl;
  *out << td() << "Signal Details" << td() << endl;
  *out << td() << signal_details << td() << endl;
  *out << tr() << endl;

  *out << table() << endl;
}

void TestResultsManagerModule::ProcessLogDirectory(xgi::Input * in, xgi::Output * out)
{
  cgicc::Cgicc cgi(in);
  cgicc::form_iterator name = cgi.getElement("path");

  boost::filesystem::path p;

  if(name != cgi.getElements().end())
  {
    p = cgi["path"]->getValue();
    cout << __func__ << ":  path " << p.string() << endl;
    trm_.processDirectory(p.string());
  }
  this->TestResultsManagerPage(in,out);
}

std::pair<std::string,std::string> TestResultsManagerModule::niceResult(std::string result_orig)
{
  std::stringstream s1;
  int result;
  s1 << result_orig;
  s1 >> result;

  std::stringstream s2;
  std::string style = "color:";

  if(result == 0)
  {
    s2 << "Pass";
    style += "green";
  }
  else if(result < 1)
  {
    s2 << "Untested";
    style += "blue";
  }
  else
  {
    s2 << "Fail: " << result;
    style += "red";
  }
  return std::pair<std::string,std::string>(s2.str(),style);
}

} } //Namespaces
