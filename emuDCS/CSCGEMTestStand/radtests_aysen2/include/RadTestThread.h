#ifndef RADTESTTHREAD_H
#define RADTESTTHREAD_H

#include <QObject>
#include <QThread>
#include <iostream>
#include "commands.h"

using namespace std;

class RadTestThread : public QObject
//class RadTestThread : public QThread
{  
  Q_OBJECT

 public:
  //void run();
  void testmeth();

 signals:
  void finished();

};


#endif
