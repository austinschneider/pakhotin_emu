#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QtGui>
#include <QWidget>
#include <QApplication>
#include <QtGui>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "debug.h"
#include "RadTestWorker.h"
#include "QProgressIndicator.h"

using namespace std;

class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QCheckBox;
class QHBoxLayout;
class QVBoxLayout;
class QProgressIndicator;
class QPlainTextEdit;

class MyQTextEdit : public QTextEdit {
  Q_OBJECT

public:
  // make window close when it is clicked
  void mouseReleaseEvent ( QMouseEvent * event ){
    close();
    return;
  }    
};

class FunctionBar : public QObject{
  // simple class with HBoxLayout containing toggle and two displays
  Q_OBJECT

 public:
  QString name;
  QCheckBox *checkBox;
  QLineEdit *display[2];
  QHBoxLayout* layout;
  
  ~FunctionBar(){
    if(checkBox) 	delete checkBox;
    if(layout) 		delete layout;
    if(display[0]) 	delete display[0];
    if(display[1]) 	delete display[1];
    this->~QObject();
  }
  FunctionBar(QString n){
    name = n;
    checkBox = new QCheckBox(name);
    checkBox->setCheckable(1);
    checkBox->setChecked(0); // start disabled
    for(int j=0; j<2; j++){
      display[j] = new QLineEdit("0");
      QPalette p = display[j]->palette();
      p.setColor(QPalette::Disabled,QPalette::Base, Qt::lightGray);
      display[j]->setPalette(p);
      display[j]->setDisabled(!checkBox->isChecked());
      display[j]->setReadOnly(1);
      display[j]->setAlignment(Qt::AlignRight);
      display[j]->setFixedSize(100,28);
      QFont font = display[j]->font();
      font.setPointSize(font.pointSize() + 8);
      display[j]->setFont(font);
    }
    layout = new QHBoxLayout;
    layout->addWidget(checkBox, 1);
    layout->addWidget(display[0], 0);
    layout->addWidget(display[1], 0);    
  }

};

class RadTestGUI : public QWidget
{
  Q_OBJECT
    
 public:
  RadTestGUI(QWidget *parent = 0);
  ~RadTestGUI();
  enum {RUNNING, PAUSED, STOPPED, RESET};
  
 private slots:
    
  void start();
  void pausestep();
  void insertComment();
  void updatenumbers();
  void stop();
  void resetRequested(int level=1);

  void disableAllFunctions(bool e);
  
  // have to define slots separately
  void turnOnBar(int i, bool e);
  inline void turnOnF0(bool e){ turnOnBar(0,e); };
  inline void turnOnF1(bool e){ turnOnBar(1,e); };
  inline void turnOnF2(bool e){ turnOnBar(2,e); };
  inline void turnOnF3(bool e){ turnOnBar(3,e); };
  inline void turnOnF4a(bool e){ turnOnBar(4,e); };
  inline void turnOnF4b(bool e){ turnOnBar(4,e); }; // 4b is tied to 4a
  inline void turnOnF5(bool e){ turnOnBar(6,e); };
  inline void turnOnF7(bool e){ turnOnBar(7,e); };

 private:

  //// Widget Layouts
  QVBoxLayout *mainLayout;
  QHBoxLayout *topLayout;
  FunctionBar *functionbar[NTESTS];
  QGridLayout *controlsLayout;
  QHBoxLayout *commentLayout;
  QHBoxLayout *statusLayout;
  QLineEdit *statusbar;

  //// Buttons
  QPushButton *startButton, *pauseButton, *stopButton;
  QPushButton *timeStampCommentButton, *quitButton;

  //// For popup messages
  MyQTextEdit *popupText;

  //// test in progress indicator
  QProgressIndicator *progressIndicator;

  //// text edit area for putting into the log
  QPlainTextEdit *textEdit;

  //// Running state
  int state;
  
  //// Worker object, Where the tests are done
  RadTestWorker *_worker;

  //// Log file
  ofstream lout;
  ostream* radlog;
  
  //// private methods
  void createFunctionBar(int i);
  QString time_QString();
  string newLogName();
  void open_logfile();
  void close_logfile();
  
};


#endif
