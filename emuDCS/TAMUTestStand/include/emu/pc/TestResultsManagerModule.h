#ifndef TESTRESULTSMANAGERMODULE_H_
#define TESTRESULTSMANAGERMODULE_H_

#include "toolbox/lang/Class.h"
#include "emu/pc/TestResultsManager.h"

#include <time.h>

namespace xdaq { class WebApplication; }
namespace xgi { class Input; class Output; }

namespace emu { namespace pc {

class ConfigurablePCrates;

class TestResultsManagerModule : public virtual toolbox::lang::Class
{
public :

  typedef TestResultsManager::TestError TestError;

  enum {BOARD_SUMMARY, LOG_SUMMARY, TEST_SUMMARY, TEST_LIST, LOG_DETAILS, TEST_DETAILS, ERROR_DETAILS};

  /// constructor
  /// the pointers to application and ConfigurablePCrates data are managed externally
  TestResultsManagerModule(xdaq::WebApplication * app);

  /// html interface page for test results (XGI bound method)
  void TestResultsManagerPage(xgi::Input * in, xgi::Output * out);

  /// process logs in a specific directory (XGI bound method)
  void ProcessLogDirectory(xgi::Input * in, xgi::Output * out);

private:

  /// keep link to the application which uses this utility (not owned by this class!)
  xdaq::WebApplication * app_;

  /// manager of test results
  TestResultsManager trm_;

  /// summary of boards
  void BoardSummary(xgi::Input * in, xgi::Output * out);

  /// summary of boards
  void LogSummary(xgi::Input * in, xgi::Output * out);

  /// test summary of individual board
  void TestSummary(xgi::Input * in, xgi::Output * out);

  /// list of all instances of a test type for one board
  void TestList(xgi::Input * in, xgi::Output * out);

  /// list of tests performed during a log
  void LogDetails(xgi::Input * in, xgi::Output * out);

  /// errors reported for a test
  void TestDetails(xgi::Input * in, xgi::Output * out);

  /// details of an error
  void ErrorDetails(xgi::Input * in, xgi::Output * out);

  /// returns a nicely formatted result
  static std::pair<std::string,std::string> niceResult(std::string);

};



} } //Namespaces


#endif /* TESTRESULTSMANAGERMODULE_H_ */
