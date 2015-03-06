/*
 * $Id: $
 */

// class header
#include "emu/pc/CSCGEMTestApplication.h"

// Emu includes
#include "emu/utils/Cgi.h"
#include "emu/utils/System.h"
#include "emu/pc/Crate.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/TMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/MPC.h"
#include "emu/pc/ALCTController.h"
#include "emu/pc/RAT.h"
#include "emu/pc/VMECC.h"
#include "emu/pc/DDU.h"

// XDAQ includes
#include "cgicc/Cgicc.h"
#include "cgicc/HTMLClasses.h"
#include "toolbox/string.h"

// system includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <string>
#include <unistd.h> // for sleep()

// radtest includes
#include "eth_lib.h"
#include "utils.h"
#include "commands.h"

namespace emu {  namespace pc {

using std::endl;
using std::cout;
using std::string;
using emu::utils::bindMemberMethod;


CSCGEMTestApplication::CSCGEMTestApplication(xdaq::ApplicationStub * s)
: xdaq::WebApplication(s)
{
  //------------------------------------------------------
  // bind methods
  //------------------------------------------------------
  xgi::bind(this, &CSCGEMTestApplication::Default, "Default");
  xgi::bind(this, &CSCGEMTestApplication::GenerateDCFEBData, "GenerateDCFEBData");
  xgi::bind(this, &CSCGEMTestApplication::UploadDCFEBData, "UploadDCFEBData");
  xgi::bind(this, &CSCGEMTestApplication::CheckDCFEBData, "CheckDCFEBData");
  xgi::bind(this, &CSCGEMTestApplication::TransmitDCFEBData, "TransmitDCFEBData");
  xgi::bind(this, &CSCGEMTestApplication::ReadOutTriggerResults, "ReadOutTriggerResults");
}


void CSCGEMTestApplication::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  using cgicc::br;
  using namespace std;

  cgicc::Cgicc cgi(in);

  string urn = getApplicationDescriptor()->getURN();

  emu::utils::headerXdaq(out, this, "GEM-CSC Test Stand");

  //*out << "Hello World!";
  
  char DirGen[200];
  char DirUp[200];
  char DCFEBN[10];
  char HalfStripN[10];
  string s1,s2,s3,s4,s5,s6,s7;
  string c1,c2,c3,c4,c5,c6,c7;

  sprintf(DirGen,"/home/cscdev/TriDAS/emu/emuDCS/CSCGEMTestStand/tmp");
  sprintf(DirUp,"/home/cscdev/TriDAS/emu/emuDCS/CSCGEMTestStand/tmp");
  sprintf(DCFEBN,"1");
  sprintf(HalfStripN,"1");

  ifstream fs;
  string line;
  fs.open("s1.txt"); getline(fs,s1); fs.close();
  fs.open("s2.txt"); getline(fs,s2); fs.close();
  fs.open("s3.txt"); getline(fs,s3); fs.close();
  fs.open("s4.txt"); getline(fs,s4); fs.close();
  fs.open("s5.txt"); getline(fs,s5); fs.close();
  fs.open("s6.txt"); getline(fs,s6); fs.close();
  fs.open("s7.txt"); getline(fs,s7); fs.close();
  fs.open("c1.txt"); getline(fs,c1); fs.close();
  fs.open("c2.txt"); getline(fs,c2); fs.close();
  fs.open("c3.txt"); getline(fs,c3); fs.close();
  fs.open("c4.txt"); getline(fs,c4); fs.close();
  fs.open("c5.txt"); getline(fs,c5); fs.close();
  fs.open("c6.txt"); getline(fs,c6); fs.close();
  fs.open("c7.txt"); getline(fs,c7); fs.close();

  ofstream ofs; 
  ofs.open("s1.txt"); ofs << "a"; ofs.close();
  ofs.open("s2.txt"); ofs << "a"; ofs.close();
  ofs.open("s3.txt"); ofs << "a"; ofs.close();
  ofs.open("s4.txt"); ofs << "a"; ofs.close();
  ofs.open("s5.txt"); ofs << "a"; ofs.close();
  ofs.open("s6.txt"); ofs << "a"; ofs.close();
  ofs.open("s7.txt"); ofs << "a"; ofs.close();
  ofs.open("c1.txt"); ofs << "a"; ofs.close();
  ofs.open("c2.txt"); ofs << "a"; ofs.close();
  ofs.open("c3.txt"); ofs << "a"; ofs.close();
  ofs.open("c4.txt"); ofs << "a"; ofs.close();
  ofs.open("c5.txt"); ofs << "a"; ofs.close();
  ofs.open("c6.txt"); ofs << "a"; ofs.close();
  ofs.open("c7.txt"); ofs << "a"; ofs.close();

  // unit 1

  *out << cgicc::fieldset().set("style", "font-size: 11pt; background-color:#FFFFBB") << endl;
  *out << cgicc::legend("Generate DCFEB data").set("style", "color:blue") ;
  *out << cgicc::form().set("method", "GET").set("action", "/" + urn + "/GenerateDCFEBData" ) << endl;
  *out << "Directory:" << cgicc::input().set("type", "text").set("name", "DirGen").set("size", "60").set("value", DirGen) << endl;
  *out << cgicc::br() << endl;
  *out << cgicc::br() << endl;
  *out << "DCFEB #:" << cgicc::input().set("type", "text").set("name", "DCFEBN").set("size", "2").set("value", DCFEBN) << endl;
  *out << "Half-strip #:" << cgicc::input().set("type", "text").set("name", "HalfStripN").set("size", "2").set("value", HalfStripN) << endl;
  *out << "&nbsp" << endl;
  *out << cgicc::input().set("type", "submit").set("value", "Generate data").set("name", "Generate data") << endl;
  *out << cgicc::form() << endl;
  *out << cgicc::fieldset() << endl;
  *out << cgicc::br() << endl;

  // unit 2

  *out << cgicc::fieldset().set("style", "font-size: 11pt; background-color:#FFFFBB") << endl;
  *out << cgicc::legend("Upload DCFEB data").set("style", "color:blue") ;
  *out << cgicc::form().set("method", "GET").set("action", "/" + urn + "/UploadDCFEBData" ) << endl;
  *out << "Directory:" << cgicc::input().set("type", "text").set("name", "DirUp").set("size", "60").set("value", DirUp) << endl;
  *out << cgicc::br() << endl;
  *out << cgicc::br() << endl;
  *out << cgicc::input().set("type", "submit").set("value", "Upload data").set("name", "Upload data") << endl;
  *out << "&nbsp&nbsp" << endl;
  *out << "Status of DCFEB#: " << endl;
  if (s1 == "0") *out << cgicc::input().set("type", "button").set("value", "1").set("style", "color:green") << endl;
  else if (s1 == "1") *out << cgicc::input().set("type", "button").set("value", "1").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "1") << endl;
  if (s2 == "0") *out << cgicc::input().set("type", "button").set("value", "2").set("style", "color:green") << endl;
  else if (s2 == "1") *out << cgicc::input().set("type", "button").set("value", "2").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "2") << endl;
  if (s3 == "0") *out << cgicc::input().set("type", "button").set("value", "3").set("style", "color:green") << endl;
  else if (s3 == "1") *out << cgicc::input().set("type", "button").set("value", "3").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "3") << endl;
  if (s4 == "0") *out << cgicc::input().set("type", "button").set("value", "4").set("style", "color:green") << endl;
  else if (s4 == "1") *out << cgicc::input().set("type", "button").set("value", "4").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "4") << endl;
  if (s5 == "0") *out << cgicc::input().set("type", "button").set("value", "5").set("style", "color:green") << endl;
  else if (s5 == "1") *out << cgicc::input().set("type", "button").set("value", "5").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "5") << endl;
  if (s6 == "0") *out << cgicc::input().set("type", "button").set("value", "6").set("style", "color:green") << endl;
  else if (s6 == "1") *out << cgicc::input().set("type", "button").set("value", "6").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "6") << endl;
  if (s7 == "0") *out << cgicc::input().set("type", "button").set("value", "7").set("style", "color:green") << endl;
  else if (s7 == "1") *out << cgicc::input().set("type", "button").set("value", "7").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "7") << endl;
  *out << cgicc::br() << endl;
  *out << cgicc::form() << endl;
  *out << cgicc::form().set("method", "GET").set("action", "/" + urn + "/CheckDCFEBData" ) << endl;
  *out << cgicc::input().set("type", "submit").set("value", "Check data").set("name", "Check data") << endl;
  *out << "&nbsp&nbsp&nbsp" << endl;
  *out << "Status of DCFEB#: " << endl;
  if (c1 == "0") *out << cgicc::input().set("type", "button").set("value", "1").set("style", "color:green") << endl;
  else if (c1 == "1") *out << cgicc::input().set("type", "button").set("value", "1").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "1") << endl;
  if (c2 == "0") *out << cgicc::input().set("type", "button").set("value", "2").set("style", "color:green") << endl;
  else if (c2 == "1") *out << cgicc::input().set("type", "button").set("value", "2").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "2") << endl;
  if (c3 == "0") *out << cgicc::input().set("type", "button").set("value", "3").set("style", "color:green") << endl;
  else if (c3 == "1") *out << cgicc::input().set("type", "button").set("value", "3").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "3") << endl;
  if (c4 == "0") *out << cgicc::input().set("type", "button").set("value", "4").set("style", "color:green") << endl;
  else if (c4 == "1") *out << cgicc::input().set("type", "button").set("value", "4").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "4") << endl;
  if (c5 == "0") *out << cgicc::input().set("type", "button").set("value", "5").set("style", "color:green") << endl;
  else if (c5 == "1") *out << cgicc::input().set("type", "button").set("value", "5").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "5") << endl;
  if (c6 == "0") *out << cgicc::input().set("type", "button").set("value", "6").set("style", "color:green") << endl;
  else if (c6 == "1") *out << cgicc::input().set("type", "button").set("value", "6").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "6") << endl;
  if (c7 == "0") *out << cgicc::input().set("type", "button").set("value", "7").set("style", "color:green") << endl;
  else if (c7 == "1") *out << cgicc::input().set("type", "button").set("value", "7").set("style", "color:red") << endl;
  else *out << cgicc::input().set("type", "button").set("value", "7") << endl;
  *out << cgicc::form() << endl;
  *out << cgicc::fieldset() << endl;
  *out << cgicc::br() << endl;

  // unit 3

  *out << cgicc::fieldset().set("style", "font-size: 11pt; background-color:#FFFFBB") << endl;
  *out << cgicc::legend("Transmit DCFEB data").set("style", "color:blue") ;
  *out << cgicc::form().set("method", "GET").set("action", "/" + urn + "/TransmitDCFEBData" ) << endl;
  *out << cgicc::input().set("type", "submit").set("value", "Transmit data").set("name", "Transmit data") << endl;
  *out << cgicc::form() << endl;
  *out << cgicc::fieldset() << endl;
  *out << cgicc::br() << endl;

  // unit 4

  *out << cgicc::fieldset().set("style", "font-size: 11pt; background-color:#FFFFBB") << endl;
  *out << cgicc::legend("Readout results from OTMB").set("style", "color:blue") ;
  *out << cgicc::form().set("method", "GET").set("action", "/" + urn + "/ReadOutTriggerResults" ) << endl;
  *out << cgicc::input().set("type", "submit").set("value", "Readout results").set("name", "Readout results") << endl;
  *out << cgicc::form() << endl;
  *out << cgicc::fieldset() << endl;


  emu::utils::footer(out);
}

void CSCGEMTestApplication::GenerateDCFEBData(xgi::Input * in, xgi::Output * out )
{
    cgicc::Cgicc cgi(in);

    char DirGen[200];
    char DCFEBN[10];
    char HalfStripN[10];

    //bits = (char*)calloc(size,sizeof(char));
    char bits[4096];

    if (xgi::Utils::hasFormElement(cgi, "DirGen")) sprintf(DirGen,cgi["DirGen"]->getValue().c_str());
    if(xgi::Utils::hasFormElement(cgi, "DCFEBN")) sprintf(DCFEBN,cgi["DCFEBN"]->getValue().c_str());
    if(xgi::Utils::hasFormElement(cgi, "HalfStripN")) sprintf(HalfStripN,cgi["HalfStripN"]->getValue().c_str());

    int HalfStrip = atoi(HalfStripN);
    int diStrip = (HalfStrip-1)/4+1;
    int strip = (HalfStrip-(diStrip-1)*4-1)/2;
    int halfStrip = HalfStrip-(diStrip-1)*4-strip*2-1;

    for (int j = 1; j < 8; j++) {
        char filename[200];
        char strj[10];
        sprintf(strj, "%d", j);
        strcpy(filename,DirGen);
        strcat(filename,"/DCFEB_");
        strcat(filename,strj);
        strcat(filename,".pat");
    
        FILE *outfile = fopen(filename,"w");

        if (j == atoi(DCFEBN)) {
            for (int i = 0; i < 6; i++) {
                bits[i] = 0xff & (int)pow(2,8-diStrip);
            }
            for (int i = 6; i < 12; i++) {
                bits[i] = 0xff & (int)pow(2,8-diStrip)*strip;
            }
            for (int i = 13; i < 18; i++) {
                bits[i] = 0xff & (int)pow(2,8-diStrip)*halfStrip;
            }
        }
        else {
            for (int i = 0; i < 18; i++) {
                bits[i] = 0x00;
            }
        }
        for (int i = 18; i < 4096; i++) {
            bits[i] = 0x00;
        }
        fwrite(bits, sizeof(char), sizeof(bits), outfile);
        fclose(outfile);
    }

    this->Default(in, out);
}

void CSCGEMTestApplication::UploadDCFEBData(xgi::Input * in, xgi::Output * out )
{
    using namespace std;

    cgicc::Cgicc cgi(in);
    char DirUp[200];
    int pbase = 2816;

    if(xgi::Utils::hasFormElement(cgi, "DirUp")) sprintf(DirUp,cgi["DirUp"]->getValue().c_str());

    ofstream fs;
    fs.open("s1.txt"); fs << "a"; fs.close();
    fs.open("s2.txt"); fs << "a"; fs.close();
    fs.open("s3.txt"); fs << "a"; fs.close();
    fs.open("s4.txt"); fs << "a"; fs.close();
    fs.open("s5.txt"); fs << "a"; fs.close();
    fs.open("s6.txt"); fs << "a"; fs.close();
    fs.open("s7.txt"); fs << "a"; fs.close();

    for (int j = 1; j < 8; j++) {
        char filename[200];
        char strj[10];
        sprintf(strj, "%d", j);
        strcpy(filename,DirUp);
        strcat(filename,"/DCFEB_");
        strcat(filename,strj);
        strcat(filename,".pat");

        char block[RAMPAGE_SIZE];
        fread(block, sizeof(char), RAMPAGE_SIZE, fopen(filename,"r"));
        memcpy(wdat,block,RAMPAGE_SIZE);

        eth_open("/dev/schar2");
        eth_reset();

        int e1 = write_command(7,pbase+j, block);
        char* pkt;
        int e2 = read_command(&pkt);

        char out[10];
        strcpy(out,"s");
        strcat(out,strj);
        strcat(out,".txt");
        fs.open(out);
        if (e1 == 0 && e2 == 7) fs << "0";
        else fs << "1";
        fs.close();

        eth_close();
    }


    
    this->Default(in, out);
}

void CSCGEMTestApplication::CheckDCFEBData(xgi::Input * in, xgi::Output * out )
{
    using namespace std;

    int pbase = 2816;

    ofstream fs;
    fs.open("c1.txt"); fs << "a"; fs.close();
    fs.open("c2.txt"); fs << "a"; fs.close();
    fs.open("c3.txt"); fs << "a"; fs.close();
    fs.open("c4.txt"); fs << "a"; fs.close();
    fs.open("c5.txt"); fs << "a"; fs.close();
    fs.open("c6.txt"); fs << "a"; fs.close();
    fs.open("c7.txt"); fs << "a"; fs.close();

    for (int j = 1; j < 8; j++) {
        char strj[10];
        sprintf(strj, "%d", j);

        eth_open("/dev/schar2");
        eth_reset();
        int e1 = write_command(3,pbase+j);
        char* pkt;
        int e2 = read_command(&pkt);

        char out[10];
        strcpy(out,"c");
        strcat(out,strj);
        strcat(out,".txt");
        fs.open(out);
        if (e1 == 0 && e2 == -3) fs << "0";
        else fs << "1";
        fs.close();

        eth_close(); 
    }

    this->Default(in, out);
}

void CSCGEMTestApplication::TransmitDCFEBData(xgi::Input * in, xgi::Output * out )
{
    this->Default(in, out);
}

void CSCGEMTestApplication::ReadOutTriggerResults(xgi::Input * in, xgi::Output * out )
{
    this->Default(in, out);
}

}}  // namespaces

// factory instantion of XDAQ application
XDAQ_INSTANTIATOR_IMPL(emu::pc::CSCGEMTestApplication)
