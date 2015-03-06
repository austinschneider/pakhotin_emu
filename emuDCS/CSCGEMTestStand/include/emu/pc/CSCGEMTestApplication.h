#ifndef _Emu_PC_CSCGEMTestApplication_h_
#define _Emu_PC_CSCGEMTestApplication_h_

#include "xdaq/WebApplication.h"

namespace emu { namespace pc {

/** \class CSCGEMTestApplication
 * main web GUI Application class for the CSC-GEM test stand
 */
class CSCGEMTestApplication: public xdaq::WebApplication
{
public:

  XDAQ_INSTANTIATOR();

  /// constructor
  CSCGEMTestApplication(xdaq::ApplicationStub * s);

  /// home-page of the application
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

private:

  // Generate DCFEB data
  void GenerateDCFEBData(xgi::Input * in, xgi::Output * out );

  // Upload generated DCFEB data on emulator board's RAMs
  void UploadDCFEBData(xgi::Input * in, xgi::Output * out );

  // Check generated DCFEB data on emulator board's RAMs
  void CheckDCFEBData(xgi::Input * in, xgi::Output * out );

  // Initiate data transmission from the emulator board to the OTMB
  void TransmitDCFEBData(xgi::Input * in, xgi::Output * out );

  // Readout trigger results from OTMB
  void ReadOutTriggerResults(xgi::Input * in, xgi::Output * out );

};

}} // namespaces

#endif
