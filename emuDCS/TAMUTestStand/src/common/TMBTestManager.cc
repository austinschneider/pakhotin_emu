/*
 * $Id: $
 */

// class header
#include "emu/pc/TMBTestManager.h"

// Emu includes
#include "emu/pc/CCBBackplaneTester.h"
#include "emu/pc/FirmwareTester.h"
#include "emu/pc/TMBExternalTester.h"

// system includes
#include <iostream>


namespace emu {  namespace pc {

using std::endl;
using std::cout;
using std::string;


TMBTestManager::TMBTestManager()
: sys_(0)
{}

TMBTestManager::~TMBTestManager()
{
  for(unsigned int i=0; i<logs_.size(); ++i)
  {
    delete logs_[i];
  }
}

void TMBTestManager::Init(ConfigurablePCrates * sys)
{
  sys_ = sys;

  // make sure the "current" crate in the system is set!
  if (sys_->crate() == 0)
  {
    cout<<__PRETTY_FUNCTION__<<" WARNING!!! The current crate is not set! Will not initialize the Test Manager!"<<endl;
    return;
  }

  std::vector< TMB * > tmbs = sys_->crate()->tmbs();

  for (size_t i = 0; i < tmbs.size(); i++)
  {
    logs_.push_back(new TestLogger());
  }

  // ****** register the tests here: ******

  RegisterTestGroup<CCBBackplaneTester>("CCBBackplaneTester");
  RegisterTestGroup<FirmwareTester>("FirmwareTester");
  //RegisterTestGroup<TMBExternalTester>("TMBExternalTester");
}


std::vector<std::string> TMBTestManager::GetTestGroupLabels() const
{
  return testGroupLabels_;
}


boost::shared_ptr< TestWorkerBase > TMBTestManager::GetTester(const std::string& test_group, int tmb)
{
  if (tests_.find(test_group) == tests_.end())
  {
    return boost::shared_ptr< TestWorkerBase >(); // zero pointer
  }

  if (tmb < 0)
  {
    tmb = sys_->tmbN();
  }

  return tests_[test_group][tmb];
}


std::ostringstream & TMBTestManager::GetTestOutput(std::string test_group, int tmb)
{
  if (tmb < 0)
  {
    tmb = sys_->tmbN();
  }

  return testOutputs_[test_group][tmb];
}

void TMBTestManager::SetBoardLabel(std::string board, int tmb)
{
  logs_[tmb]->setBoard(board);
}

void TMBTestManager::SetDNA(std::string dna, int tmb)
{
  logs_[tmb]->setDNA(dna);
}

std::string TMBTestManager::GetBoardLabel(int tmb)
{
  return logs_[tmb]->getBoard();
}

bool TMBTestManager::IsLogging(int tmb)
{
  return logs_[tmb]->isLogging();
}

bool TMBTestManager::IsTesting(int tmb)
{
  return logs_[tmb]->isTesting();
}

void TMBTestManager::BeginLogging(int tmb)
{
  logs_[tmb]->resumeLogging();
}

void TMBTestManager::EndLogging(int tmb)
{
  logs_[tmb]->pauseLogging();
}

void TMBTestManager::FinishTesting(int tmb)
{
  logs_[tmb]->closeFile();
  for(unsigned int i=0; i<testGroupLabels_.size(); ++i)
  {
    tests_[testGroupLabels_[i]][tmb]->FinishTesting();
  }
}


}}  // namespaces
