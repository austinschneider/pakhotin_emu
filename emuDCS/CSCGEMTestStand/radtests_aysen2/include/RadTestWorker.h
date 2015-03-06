#ifndef RADTESTWORKER_H
#define RADTESTWORKER_H

#include <QObject>
#include <QtConcurrentRun>
#include <QTimer>
#include <QEventLoop>
#include <iostream>
#include <time.h>
#include "debug.h"
#include "commands.h"
#include "utils.h"
//#include "RadTestThread.h"

// number of tests+1 (+1 because the 0th element is skipped)
#define NTESTS 8

using namespace std;

class RadTestWorker : public QObject
{
  Q_OBJECT

 private:
  QTimer _timer;
  QTimer _pulse;
  int currenttest;
  bool dotest[NTESTS]; // flag of which tests to do
  bool reinit[NTESTS]; // flag that counters need to be (re)initialized for each test
  void clear();
  int check_RAMpage(int pageid); // code for checking each RAM page in RAM Page Test
  int write_RAMpage(int pageid); // code for writing each RAM page
  void wait_msec(int msec);
  
  // variables used in tests
  bool SEUcounted;
  int ntry;
  int f4try;
  int errnum;
  long int change;
  int seqn;
  int pageid;
  static const int firstb = 2816; // first BRAM address
  static const int firstc = 3072; // first CRAM address
  static const int maxtry = 3; // number of tries/test before requesting hard reset
  static const int trywait = 10; // milliseconds to wait between tries
  static const int testwait = 500; // milliseconds to wait between different tests
  bool newstart;
  bool resendBRAMs;
  bool netcheckdone;
  int resetlevel;
  bool bump;

  // buffers for tests
  unsigned int snap12_counts[8];
  unsigned int snap12_counts_last[8];
  unsigned int voting_counts[12];
  unsigned int voting_counts_last[12];
  unsigned short trans_counts[24];
  unsigned short trans_counts_last[24];
  char f3hdr[4];
  char block[RAMPAGE_SIZE];
  char block2[RAMPAGE_SIZE];
  char fblock[RAMPAGE_SIZE];
  int prev_id[1];
  int curr_id[1];
  int pre2_id[1];
  unsigned int eth_counts[8];
  unsigned int eth_counts_last[8];
  
  
 public:
  RadTestWorker(QObject *parent=0);
  ostream* radlog;
  bool notests();
  int counter[NTESTS][2];
  void start(){ _timer.start(0); }; // start timer -> calls to runTest() every this many ms
  void pause(){ _timer.stop(); }; // stop timer -> will stop sending calls to runTest()
  void stop(){ _timer.stop(); newstart=1; clear(); }; // stop timer, next start will be "new", clear counters
  void step(){ this->runTest(); }; // single step

  inline void setlog(ostream* s){radlog = s;}

 private slots:
  void runTest();

 public slots:
  // signals in GUI connected to these slots
  void turnOnF0(bool e){ dotest[0]=e;     if(DEBUG>2) cout<<"F0 toggled:"<<e<<endl;};
  void turnOnF1(bool e){ dotest[1]=e;     if(DEBUG>2) cout<<"F1 toggled:"<<e<<endl;};
  void turnOnF2(bool e){ dotest[2]=e;     if(DEBUG>2) cout<<"F2 toggled:"<<e<<endl;};
  void turnOnF3(bool e){ dotest[3]=e; f7_needed(e);    if(DEBUG>2) cout<<"F3 toggled:"<<e<<endl;};
  void turnOnF4a(bool e){ dotest[4]=e;    if(DEBUG>2) cout<<"F4a toggled:"<<e<<endl;};
  void turnOnF4b(bool e){ dotest[4]=e;    if(DEBUG>2) cout<<"F4b toggled:"<<e<<endl;}; // 4b is tied to 4a
  void turnOnF5(bool e){ dotest[5]=e;     if(DEBUG>2) cout<<"F5 toggled:"<<e<<endl;};
  void turnOnF7(bool e){ resendBRAMs=e;   if(DEBUG>2) cout<<"F7 toggled:"<<e<<endl;};

 signals:
  void counterchanged(); // signal to GUI that counter has changed
  void needReset(int level=1); // signal to GUI that a hard reset is needed
  void f7_needed(bool e); // signal to GUI that F7 was done
};


#endif
