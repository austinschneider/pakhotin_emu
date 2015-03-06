#include "emu/otmbdev/Buttons.h"
#include "emu/otmbdev/utils.h"
#include "emu/otmbdev/Manager.h"

#include "emu/pc/Crate.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/TMB.h"
#include "emu/pc/ALCTController.h"
#include "emu/pc/TMB_constants.h"

#include "emu/utils/String.h"
#include "cgicc/HTMLClasses.h"

#include <iomanip>
#include <ctime>



using namespace std;
using namespace emu::pc;

/******************************************************************************
 * Some classes are declared in the header file because they are short and
 * sweet. Check there first!
 *
 * Also note, at the end of this file is a template for new Action
 * sub-classes.
 * 
 *****************************************************************************/

namespace emu {
  namespace otmbdev {
    
    void HardReset::respond(xgi::Input * in, ostringstream & out) { cout<<"==>HardReset"<<endl; 
      if(ccb_->GetCCBmode() != CCB::VMEFPGA) ccb_->setCCBMode(CCB::VMEFPGA); // we want the CCB in this mode for out test stand
      ccb_->HardReset_crate(); // send a simple hard reset without any sleeps
      usleep(150000); // need at least 150 ms after hard reset 
    }
    
    void SoftReset::respond(xgi::Input * in, ostringstream & out) { cout<<"==>SoftReset"<<endl; 
      ccb_->SoftReset_crate(); // send a simple soft reset without any sleeps

      usleep(150000); // need at least 150 ms after soft reset 
    }
    
    void L1Reset::respond(xgi::Input * in, ostringstream & out) { cout<<"==>L1Reset"<<endl; ccb_->l1aReset(); }

    void BC0::respond(xgi::Input * in, ostringstream & out) { cout<<"==>BC0"<<endl; ccb_->bc0(); }


    /**************************************************************************
     * SetFineDelayForADCFEB
     *
     *************************************************************************/

    SetFineDelayForADCFEB::SetFineDelayForADCFEB(Crate * crate)
      : Action(crate),
        Action2Values<int, int>(1, 0) {}

    void SetFineDelayForADCFEB::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Set Fine Delay: delay(0-15), DCFEB(0-4):",
                                "FineDelay",
                                numberToString(value1()),
                                "DcfebNumber",
                                numberToString(value2()));
    }
    
    void SetFineDelayForADCFEB::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetFineDelayForADCFEB"<<endl; 
      int delay = getFormValueInt("FineDelay", in);
      int cfeb_number = getFormValueInt("DcfebNumber", in);
      value1(delay); // save the value
      value2(cfeb_number); // save the value
      
      for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	vector<CFEB> cfebs = (*dmb)->cfebs();
	(*dmb)->dcfeb_fine_delay(cfebs.at(cfeb_number), delay); // careful, I this may depend on the order in the xml
	usleep(100);
	(*dmb)->Pipeline_Restart(cfebs[cfeb_number]); // careful, I this may depend on the order in the xml
      }
    }

    /**************************************************************************
     * TmbDavDelayScan
     *
     *************************************************************************/

    TmbDavDelayScan::TmbDavDelayScan(Crate * crate, emu::otmbdev::Manager* manager)
      : Action( crate, manager ),
        Action2Values<int, int>(0, 31) {}

    void TmbDavDelayScan::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Scan TMB DAV delay with pulses",
                                "From",
                                numberToString(value1()),
                                "To",
                                numberToString(value2()));
    }

    void TmbDavDelayScan::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TmbDavDelayScan"<<endl; 
      int fromdelay = getFormValueInt("From", in);
      value1( fromdelay ); // save the value

      int todelay = getFormValueInt("To", in);
      value2( todelay ); // save the value

      const int strip_to_pulse = 8; // TODO: make configurable

      // set register 0 appropriately for communication over the VME
      // backplane, this is necessary for the CCB to issue hard resets
      // and to respond to L1 requests from the TMB.
      ccb_->setCCBMode(CCB::VMEFPGA);

      //
      // SetUpPrecisionCapacitors
      //
      ccb_->hardReset();


      // tmb_->SetClctPatternTrigEnable(1); // enable CLCT pretriggers
      // tmb_->WriteRegister(emu::pc::seq_trig_en_adr);

      // enable L1A and clct_pretrig from any of dmb_cfeb_calib
      // signals and disable all other trigger sources
      ccb_->EnableL1aFromDmbCfebCalibX();
      usleep(100);


      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
	  (*dmb)->set_ext_chanx(strip_to_pulse);
	  (*dmb)->buck_shift();
	  usleep(100);
	}


      //
      // Loop over the requested range of dav delays
      //
      for ( int idelay = fromdelay; idelay <= todelay; ++idelay ){
	
	ccb_->l1aReset();
	usleep(100);
	
	for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	  (*dmb)->varytmbdavdelay(idelay);
	}
	
	ccb_->l1aReset();
	usleep(100);
	ccb_->bc0();
	
	manager_->startDAQ( string("TmbDavDelay")+emu::utils::stringFrom<int>( idelay ) );
	
	ccb_->pulse(1,0);
	  //ccb_->GenerateDmbCfebCalib0();
	
	manager_->stopDAQ();
      }
    }


    /**************************************************************************
     * L1aDelayScan
     *
     *************************************************************************/

    L1aDelayScan::L1aDelayScan(Crate * crate, emu::otmbdev::Manager* manager)
    : Action( crate, manager ),
	    Action2Values<int, int>(100, 140) {}

    void L1aDelayScan::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Scan CCB L1A delay with pulses",
                                "From",
                                numberToString(value1()),
                                "To",
                                numberToString(value2()));
    }

    void L1aDelayScan::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>L1aDelayScan"<<endl; 
      int fromdelay = getFormValueInt("From", in);
      value1( fromdelay ); // save the value

      int todelay = getFormValueInt("To", in);
      value2( todelay ); // save the value

      const int strip_to_pulse = 8; // TODO: make configurable

      // set register 0 appropriately for communication over the VME
      // backplane, this is necessary for the CCB to issue hard resets
      // and to respond to L1 requests from the TMB.
      ccb_->setCCBMode(CCB::VMEFPGA);

      //
      // SetUpPrecisionCapacitors
      //
      ccb_->hardReset();

      // enable L1A and clct_pretrig from any of dmb_cfeb_calib
      // signals and disable all other trigger sources
      ccb_->EnableL1aFromDmbCfebCalibX();
      usleep(100);
      //ccb_->SetExtTrigDelay(19);
      //usleep(100);

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	(*dmb)->set_ext_chanx(strip_to_pulse);
	(*dmb)->buck_shift();
	usleep(100);
      }


      //
      // Loop over the requested range of l1a delays
      //
      for ( int idelay = fromdelay; idelay <= todelay; ++idelay ){

	ccb_->l1aReset();
	usleep(100);
	
	ccb_->SetL1aDelay(idelay);

	ccb_->l1aReset();
	usleep(100);
	ccb_->bc0();
	
	manager_->startDAQ( string("L1aDelay")+emu::utils::stringFrom<int>( idelay ) );
    
	ccb_->pulse(1,0);
	//ccb_->GenerateDmbCfebCalib0();
	usleep(10);

	manager_->stopDAQ();
      }
    }

    /**************************************************************************
     * TMBHardResetTest
     *
     *************************************************************************/

    TMBHardResetTest::TMBHardResetTest(Crate * crate)
      : Action(crate),
        ActionValue<int>(100) {}

    void TMBHardResetTest::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "TMB Hard Reset Test, number of resets:",
                           "NumberOfHardResets",
                           numberToString(value()));
    }

    void TMBHardResetTest::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TMBHardResetTest"<<endl; 
      int NumberOfHardResets = getFormValueInt("NumberOfHardResets", in);
      value(NumberOfHardResets); // save the value

      out << "=== TMB Hard Reset Test ===\n";

      int expected_day = tmb_->GetExpectedTmbFirmwareDay();
      int expected_month = tmb_->GetExpectedTmbFirmwareMonth();
      int expected_year = tmb_->GetExpectedTmbFirmwareYear();
      int expected_type = tmb_->GetExpectedTmbFirmwareType();
      int expected_version = tmb_->GetExpectedTmbFirmwareVersion();
      int hiccup_number = 0;

      time_t now = time(0);
      struct tm* tm = localtime(&now);
      out << "Beginning time: " << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec << endl;

      int i; // we'll want to get the value of this after the loop is complete
      // in order to print how many succesful hard resets we ran
      bool firmware_lost = false;

      // the CCB writes to stdout every time it issues a hard rest, but we
      // don't care we turn this back on after the loop
      ostringstream waste;
      ccb_->RedirectOutput(&waste);

      for (i = 0;
           i < NumberOfHardResets && !firmware_lost;
           ++i)
	{
	  if (i % 500 == 0) {
	    out << "Hard Reset Number " << i << endl;
	  }

	  //ccb_->hardReset(); // slow
	  ccb_->HardReset_crate(); // no sleeps
	  usleep(800000); // need at least 150 ms for hard resets

	  const int maximum_firmware_readback_attempts = 2;
	  int firmware_readback_attempts = 0;
	  do {
	    firmware_lost = false;
	    tmb_->FirmwareDate(); // reads the month and day off of the tmb
	    int actual_day = tmb_->GetReadTmbFirmwareDay();
	    int actual_month = tmb_->GetReadTmbFirmwareMonth();
	    tmb_->FirmwareYear(); // reads the year off of the tmb
	    int actual_year = tmb_->GetReadTmbFirmwareYear();
	    tmb_->FirmwareVersion(); // reads the version off of the tmb
	    int actual_type = tmb_->GetReadTmbFirmwareType();
	    int actual_version = tmb_->GetReadTmbFirmwareVersion();

	    if ((actual_day != expected_day) ||
		(actual_month != expected_month) ||
		(actual_year != expected_year) ||
		(actual_type != expected_type) ||
		(actual_version != expected_version))
	      {
		firmware_lost = true;
		hiccup_number++;
		usleep(1000); // sometimes the readback fails, so wait and try again
	      }
	    // if we haven't gone over our maximum number of readback attempts and
	    // the firmware was "lost" (i.e. the readback didn't match the expected
	    // values), then try again.

	  } while (++firmware_readback_attempts < maximum_firmware_readback_attempts &&
		   firmware_lost);
	}

      ccb_->RedirectOutput(&cout);

      now = time(0);
      tm =  localtime(&now);
      out << "End time: " << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec << endl;
      out << "Number of hiccups: " << hiccup_number << endl;

      if(firmware_lost) {
        out << "The frimware was lost after " << i << " hard resets." << endl;
      } else {
        out << "The firmware was *never* lost after " << i << " hard resets." << endl;
      }
    }

    TMBReadRegisters::TMBReadRegisters(Crate * crate, emu::otmbdev::Manager* manager)
      : Action( crate, manager ),
        ActionValue<string>("0x14a") {}

    void TMBReadRegisters::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                                "Read TMB Registers (hex)",
                                "Register",
                                value());
    }

    void TMBReadRegisters::respond(xgi::Input * in, ostringstream & out)
    {
      int RegisterValue = getFormValueIntHex("Register", in);

      std::stringstream hexstr_RegisterValue;
      hexstr_RegisterValue << std::hex << RegisterValue;
      value( hexstr_RegisterValue.str() ); // save value in hex
      
      out<<"Read TMB Register: "<<std::hex<<RegisterValue<<" to "<<std::hex<<tmb_->ReadRegister(RegisterValue)<<endl;
    }

    TMBRegisters::TMBRegisters(Crate * crate)
      : Action(crate) { }

    void TMBRegisters::display(xgi::Output * out)
    {
      addButton(out, "Read TMB GTX Registers");
    }

    void TMBRegisters::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TMBRegisters"<<endl;      
      //Print out current registers
      out<< "GTX Registers for fibers 0-6 "<<endl;
      out<<" 0x14a \t"<<std::hex<<tmb_->ReadRegister(0x14a)<<endl; 
      out<<" 0x14c \t"<<std::hex<<tmb_->ReadRegister(0x14c)<<endl;
      out<<" 0x14e \t"<<std::hex<<tmb_->ReadRegister(0x14e)<<endl;
      out<<" 0x150 \t"<<std::hex<<tmb_->ReadRegister(0x150)<<endl;
      out<<" 0x152 \t"<<std::hex<<tmb_->ReadRegister(0x152)<<endl;
      out<<" 0x154 \t"<<std::hex<<tmb_->ReadRegister(0x154)<<endl;
      out<<" 0x156 \t"<<std::hex<<tmb_->ReadRegister(0x156)<<endl;
      out<<" 0x158 \t"<<std::hex<<tmb_->ReadRegister(0x158)<<endl;
      out<<"Hot Channel Masks"<<endl;
      out<<" 0x42 \t"<<std::hex<<tmb_->ReadRegister(0x42)<<endl;
      out<<" 0x4a \t"<<std::hex<<tmb_->ReadRegister(0x4a)<<endl;
      out<<" 0x4c \t"<<std::hex<<tmb_->ReadRegister(0x4c)<<endl;
      out<<" 0x4e \t"<<std::hex<<tmb_->ReadRegister(0x4e)<<endl;
      out<<" 0x50 \t"<<std::hex<<tmb_->ReadRegister(0x50)<<endl;
      out<<" 0x52 \t"<<std::hex<<tmb_->ReadRegister(0x52)<<endl;
      out<<" 0x54 \t"<<std::hex<<tmb_->ReadRegister(0x54)<<endl;
      out<<" 0x56 \t"<<std::hex<<tmb_->ReadRegister(0x56)<<endl;
      out<<" 0x58 \t"<<std::hex<<tmb_->ReadRegister(0x58)<<endl;
      out<<" 0x5a \t"<<std::hex<<tmb_->ReadRegister(0x5a)<<endl;
      out<<" 0x5c \t"<<std::hex<<tmb_->ReadRegister(0x5c)<<endl;
      out<<" 0x5e \t"<<std::hex<<tmb_->ReadRegister(0x5e)<<endl;
      out<<" 0x60 \t"<<std::hex<<tmb_->ReadRegister(0x60)<<endl;
      out<<" 0x62 \t"<<std::hex<<tmb_->ReadRegister(0x62)<<endl;
      out<<" 0x64 \t"<<std::hex<<tmb_->ReadRegister(0x64)<<endl;
      out<<" 0x66 \t"<<std::hex<<tmb_->ReadRegister(0x66)<<endl;
      out<<" 0x16e \t"<<std::hex<<tmb_->ReadRegister(0x16e)<<endl;
      out<<" 0x170 \t"<<std::hex<<tmb_->ReadRegister(0x170)<<endl;
      out<<" 0x172 \t"<<std::hex<<tmb_->ReadRegister(0x172)<<endl;
      out<<" 0x174 \t"<<std::hex<<tmb_->ReadRegister(0x174)<<endl;
      out<<" 0x176 \t"<<std::hex<<tmb_->ReadRegister(0x176)<<endl;
      out<<" 0x178 \t"<<std::hex<<tmb_->ReadRegister(0x178)<<endl;
      out<<"bad bit registers"<<endl;
      out<<" 0x122 \t"<<std::hex<<tmb_->ReadRegister(0x122)<<endl;
      out<<" 0x15c \t"<<std::hex<<tmb_->ReadRegister(0x15c)<<endl; 
      
    }
    
    TMBSetRegisters::TMBSetRegisters(Crate * crate, emu::otmbdev::Manager* manager)
      : Action( crate, manager ),
        Action2Values<string, string>("0x14a", "0x0001") {}

    void TMBSetRegisters::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Set TMB Registers (hex)",
                                "Register",
                                value1(),
                                "Value",
                                value2());
    }

    void TMBSetRegisters::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TMBSetRegisters"<<endl; 
      int RegisterValue = getFormValueIntHex("Register", in);
      int setting = getFormValueIntHex("Value", in);

      std::stringstream hexstr_RegisterValue;
      hexstr_RegisterValue << std::hex << RegisterValue;
      value1( hexstr_RegisterValue.str() ); // save value in hex
      
      std::stringstream hexstr_setting;
      hexstr_setting << std::hex << setting;
      value2( hexstr_setting.str() ); // save value in hex
      
      tmb_->WriteRegister(RegisterValue,setting);										      
      usleep(100000);
      
      out<<"Set TMB Register: "<<std::hex<<RegisterValue<<" to "<<std::hex<<tmb_->ReadRegister(RegisterValue)<<endl;
    }


    TMBEnableCLCTInput::TMBEnableCLCTInput(Crate * crate)
      : Action(crate),
        ActionValue<int>(1) {}

    void TMBEnableCLCTInput::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Enable DCFEB:",
                           "(1-7)",
                           numberToString(value()));
    }

    void TMBEnableCLCTInput::respond(xgi::Input * in, ostringstream & out)
    {
      int DCFEBtoEnable = getFormValueInt("(1-7)", in);
      value(DCFEBtoEnable); // save the value
      if(DCFEBtoEnable ==1){
        tmb_->WriteRegister(0x4a,0xffff);
        tmb_->WriteRegister(0x4c,0xffff);
        tmb_->WriteRegister(0x4e,0xffff);
        if(tmb_->ReadRegister(0x4a)==0xffff && tmb_->ReadRegister(0x4c)==0xffff &&tmb_->ReadRegister(0x4e)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB1"<<endl;
      }
      if(DCFEBtoEnable ==2){
        tmb_->WriteRegister(0x50,0xffff);
        tmb_->WriteRegister(0x52,0xffff);
        tmb_->WriteRegister(0x54,0xffff);
        if(tmb_->ReadRegister(0x50)==0xffff && tmb_->ReadRegister(0x52)==0xffff &&tmb_->ReadRegister(0x54)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB2"<<endl;
      }
      if(DCFEBtoEnable ==3){
        tmb_->WriteRegister(0x56,0xffff);
        tmb_->WriteRegister(0x58,0xffff);
        tmb_->WriteRegister(0x5a,0xffff);
        if(tmb_->ReadRegister(0x56)==0xffff && tmb_->ReadRegister(0x58)==0xffff &&tmb_->ReadRegister(0x5a)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB3"<<endl;
      }
      if(DCFEBtoEnable ==4){
        tmb_->WriteRegister(0x5c,0xffff);
        tmb_->WriteRegister(0x5e,0xffff);
        tmb_->WriteRegister(0x60,0xffff);
        if(tmb_->ReadRegister(0x5c)==0xffff && tmb_->ReadRegister(0x5e)==0xffff &&tmb_->ReadRegister(0x60)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB4"<<endl;
      }
      if(DCFEBtoEnable ==5){
        tmb_->WriteRegister(0x62,0xffff);
        tmb_->WriteRegister(0x64,0xffff);
        tmb_->WriteRegister(0x66,0xffff);
        if(tmb_->ReadRegister(0x62)==0xffff && tmb_->ReadRegister(0x64)==0xffff &&tmb_->ReadRegister(0x66)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB5"<<endl;
      }
      if(DCFEBtoEnable ==6){
        tmb_->WriteRegister(0x16e,0xffff);
        tmb_->WriteRegister(0x170,0xffff);
        tmb_->WriteRegister(0x172,0xffff);
        if(tmb_->ReadRegister(0x16e)==0xffff && tmb_->ReadRegister(0x170)==0xffff &&tmb_->ReadRegister(0x172)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB6"<<endl;
      }
      if(DCFEBtoEnable ==7){
        tmb_->WriteRegister(0x174,0xffff);
        tmb_->WriteRegister(0x176,0xffff);
        tmb_->WriteRegister(0x178,0xffff);
        if(tmb_->ReadRegister(0x174)==0xffff && tmb_->ReadRegister(0x176)==0xffff &&tmb_->ReadRegister(0x178)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB7"<<endl;
      }
    }

    TMBDisableCLCTInput::TMBDisableCLCTInput(Crate * crate)
      : Action(crate),
        ActionValue<int>(1) {}

    void TMBDisableCLCTInput::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Disable DCFEB:",
                           "(1-7)", 
                           numberToString(value()));
    }

    void TMBDisableCLCTInput::respond(xgi::Input * in, ostringstream & out)
    {
      int DCFEBtoDisable = getFormValueInt("(1-7)", in);
      value(DCFEBtoDisable); // save the value
      if(DCFEBtoDisable ==1){ 
        tmb_->WriteRegister(0x4a,0);
        tmb_->WriteRegister(0x4c,0);
        tmb_->WriteRegister(0x4e,0);
        if(tmb_->ReadRegister(0x4a)==0 && tmb_->ReadRegister(0x4c)==0 &&tmb_->ReadRegister(0x4e)==0) out<<"FELICIDADES! Has apagado el DCFEB1"<<endl;
      }
      if(DCFEBtoDisable ==2){  
        tmb_->WriteRegister(0x50,0);
        tmb_->WriteRegister(0x52,0);
        tmb_->WriteRegister(0x54,0);
        if(tmb_->ReadRegister(0x50)==0 && tmb_->ReadRegister(0x52)==0 &&tmb_->ReadRegister(0x54)==0) out<<"FELICIDADES! Has apagado el DCFEB2"<<endl;
      }
      if(DCFEBtoDisable ==3){
        tmb_->WriteRegister(0x56,0);
        tmb_->WriteRegister(0x58,0);
        tmb_->WriteRegister(0x5a,0);
        if(tmb_->ReadRegister(0x56)==0 && tmb_->ReadRegister(0x58)==0 &&tmb_->ReadRegister(0x5a)==0) out<<"FELICIDADES! Has apagado el DCFEB3"<<endl;
      }
      if(DCFEBtoDisable ==4){
        tmb_->WriteRegister(0x5c,0);
        tmb_->WriteRegister(0x5e,0);
        tmb_->WriteRegister(0x60,0);
        if(tmb_->ReadRegister(0x5c)==0 && tmb_->ReadRegister(0x5e)==0 &&tmb_->ReadRegister(0x60)==0) out<<"FELICIDADES! Has apagado el DCFEB4"<<endl;
      }
      if(DCFEBtoDisable ==5){
        tmb_->WriteRegister(0x62,0);
        tmb_->WriteRegister(0x64,0);
        tmb_->WriteRegister(0x66,0);
        if(tmb_->ReadRegister(0x62)==0 && tmb_->ReadRegister(0x64)==0 &&tmb_->ReadRegister(0x66)==0) out<<"FELICIDADES! Has apagado el DCFEB5"<<endl;
      }
      if(DCFEBtoDisable ==6){
        tmb_->WriteRegister(0x16e,0);
        tmb_->WriteRegister(0x170,0);
        tmb_->WriteRegister(0x172,0);
        if(tmb_->ReadRegister(0x16e)==0 && tmb_->ReadRegister(0x170)==0 &&tmb_->ReadRegister(0x172)==0) out<<"FELICIDADES! Has apagado el DCFEB6"<<endl;
      }
      if(DCFEBtoDisable ==7){
        tmb_->WriteRegister(0x174,0);
        tmb_->WriteRegister(0x176,0);
        tmb_->WriteRegister(0x178,0);
        if(tmb_->ReadRegister(0x174)==0 && tmb_->ReadRegister(0x176)==0 &&tmb_->ReadRegister(0x178)==0) out<<"FELICIDADES! Has apagado el DCFEB7)"<<endl;
      }
    }

    
    TMBDisableCopper::TMBDisableCopper(Crate * crate)
      : Action(crate) { }

    void TMBDisableCopper::display(xgi::Output * out)
    {
      addButton(out, "TMB Disable Copper");
    }

    void TMBDisableCopper::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TMBDisableCopper"<<endl; 
      
      out<<" Initial TMB Register 42 = "<<std::hex<<tmb_->ReadRegister(42)<<endl;
      
      tmb_->SetDistripHotChannelMask(0,0x00000000ff);                       
      tmb_->SetDistripHotChannelMask(1,0x00000000ff);                        
      tmb_->SetDistripHotChannelMask(2,0x00000000ff);                        
      tmb_->SetDistripHotChannelMask(3,0x00000000ff);                        
      tmb_->SetDistripHotChannelMask(4,0x00000000ff);  
      tmb_->SetDistripHotChannelMask(5,0x00000000ff);                      
      tmb_->WriteDistripHotChannelMasks(); 
      
      
      
      out<<" TMB Register 42 = "<<std::hex<<tmb_->ReadRegister(42)<<endl;
      
    }

    /*****************************************************
    Set ALCT TOF Delay
    *****************************************************/   
 
        SetAlctTofDelay::SetAlctTofDelay(Crate * crate)
       : Action( crate),
         ActionValue<int>(1) {}



    void SetAlctTofDelay::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                            "Set ALCT TOF delay ([0-15])",
                            "tofDelay",
                           numberToString(value()) );
    }



    void SetAlctTofDelay::respond(xgi::Input * in, ostringstream & out)
    {
      int tofDelay = getFormValueInt("tofDelay", in);

      tmb_->SetAlctTOFDelay(tofDelay);
      tmb_->configure();
      tmb_->alctController()->configure();
      ccb_->hardReset();
    }
    
    /*****************************************************
    YP Test
    *****************************************************/   
 
        YPTest::YPTest(Crate * crate)
       : Action( crate),
         ActionValue<int>(1) {}



    void YPTest::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                            "Inject muon pattern",
                            "YPTestVal",
                           numberToString(value()) );
    }



    void YPTest::respond(xgi::Input * in, ostringstream & out)
    {
      int YPTestVal = getFormValueInt("YPTestVal", in);

      tmb_->SetAlctTOFDelay(YPTestVal);
      tmb_->configure();
      tmb_->alctController()->configure();
      ccb_->hardReset();
    }
    

    /*****************************************************
    Set ALCT TX/RX Delays
    *****************************************************/   
 
    SetAlctTxRx::SetAlctTxRx(Crate * crate)
    : Action( crate),
    Action2Values<int,int>(0,0) {}

    void SetAlctTxRx::display(xgi::Output * out)
    {
       addButtonWithTwoTextBoxes(out,
                           "Set ALCT tx/rx delays (alct_tx_clock_delay[0-24ns], alct_rx_clock_delay[0-24ns])",
                           "txDelay",
                           numberToString(value1()),
                           "rxDelay", 
                           numberToString(value2()));
    }

    void SetAlctTxRx::respond(xgi::Input * in, ostringstream & out)
    {
      int txDelay = getFormValueInt("txDelay", in);
      int rxDelay = getFormValueInt("rxDelay", in);

      tmb_->SetAlctTxClockDelay(txDelay);
      tmb_->SetAlctRxClockDelay(rxDelay);

      tmb_->configure();
      tmb_->alctController()->configure();
      ccb_->hardReset();
    }


    /*****************************************************
    Set ALCT tx/rx posneg
    *****************************************************/   
 
    SetAlctTxRxposneg::SetAlctTxRxposneg(Crate * crate)
    : Action( crate),
    Action2Values<int,int>(0,0) {}

    void SetAlctTxRxposneg::display(xgi::Output * out)
    {
       addButtonWithTwoTextBoxes(out,
                           "Set ALCT tx/rx posneg (alct_tx_posneg[0,1], alct_posneg[0,1])",
                           "txposneg",
                           numberToString(value1()),
                           "rxposneg", 
                           numberToString(value2()));
    }

    void SetAlctTxRxposneg::respond(xgi::Input * in, ostringstream & out)
    {
      int txposneg = getFormValueInt("txposneg", in);
      int rxposneg = getFormValueInt("rxposneg", in);

      tmb_->SetAlctTxPosNeg(txposneg);
      tmb_->SetAlctRxPosNeg(rxposneg);

      tmb_->configure();
      tmb_->alctController()->configure();
      ccb_->hardReset();
    }
    
    /******************************************************
    Configure Crate
    *******************************************************/

    CrateConfigure::CrateConfigure(Crate * crate)
    : Action(crate) { }

    void CrateConfigure::display(xgi::Output * out)
    {
      addButton(out, "Crate configure");
    }

    void CrateConfigure::respond(xgi::Input * in, ostringstream & out)
    {
        crate_->configure(0);
    }

    /******************************************************
    ALCT Configure
    *******************************************************/
    ConfigureAlct::ConfigureAlct(Crate * crate)
    : Action(crate) { }

    void ConfigureAlct::display(xgi::Output * out)
    {
      addButton(out, "Configure ALCT");
    }

    void ConfigureAlct::respond(xgi::Input * in, ostringstream & out)
    {
        alct_->configure();
    }

    /************************************************************
    *  Read back ALCT_TOF delay
    ************************************************************/
    ReadAlctTofDelay::ReadAlctTofDelay(Crate * crate)
    : Action(crate) { }

    void ReadAlctTofDelay::display(xgi::Output * out)
    {
      addButton(out, "Read ALCT TOF Delay");
    }

    void ReadAlctTofDelay::respond(xgi::Input * in, ostringstream & out)
    {
      out<<"ALCT TOF Delay = "<<tmb_->GetAlctTOFDelay()<<endl;
    } 




    /**************************************************************************
     * DDU_KillFiber
     *
     *************************************************************************/

    DDU_KillFiber::DDU_KillFiber(Crate * crate)
      : Action(crate),
	ActionValue<string>("read") {}

    void DDU_KillFiber::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
			   "Read(read)/Write(15bit hex#) DDU Kill Fiber",
			   "KillFiber",
			   value());
    }

    void DDU_KillFiber::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>DDU_KillFiber"<<endl; 
      int KillFiber = getFormValueIntHex("KillFiber", in);
      string KillFiberString = getFormValueString("KillFiber", in);
      //value( KillFiberString ); // save value in hex
      value("read"); // always default to "read"

      if( KillFiberString == "read" ){ // READ
	out << "DDU Read Kill Fiber:" << endl;
	for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu){
	  out << "  DDU in slot " << (*ddu)->slot() << ": " << endl;
	  out << "  DDU with ctrl fpga user code: " << (*ddu)->CtrlFpgaUserCode()
	      << hex << setfill('0') // set up for next two hex values
	      << " and vme prom user code: "
	      << setw(8) << (*ddu)->VmePromUserCode()
	      << " has Kill Fiber is set to: "
	      << setw(4) << (*ddu)->readFlashKillFiber() << endl;
	}
      }else{  // WRITE
	out << "DDU Write Kill Fiber:" << endl;
	for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu){
	  out << "  DDU in slot " << (*ddu)->slot() << "..." << endl;
	  (*ddu)->writeFlashKillFiber(KillFiber);
	}
      }
    }

    /**************************************************************************
     * DDU_EthPrescale
     *
     *************************************************************************/

    DDU_EthPrescale::DDU_EthPrescale(Crate * crate)
      : Action(crate),
	ActionValue<string>("read") {}
    
    void DDU_EthPrescale::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
			   "Read(read)/Write(hex#) DDU Gb Eth Prescale",
			   "prescale",
			   value());
    }
    
    void DDU_EthPrescale::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>DDU_EthPrescale"<<endl; 
      int prescale = getFormValueIntHex("prescale", in);
      string prescaleString = getFormValueString("prescale", in);
      //value( prescaleString );
      value("read"); // always default to "read"      

      if(prescaleString == "read" ){ // READ
	out << "DDU Read Gb Eth Prescale: " << endl;
	for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu) {
	  out << "  DDU in slot " << (*ddu)->slot() << hex << setfill('0') << ": " << setw(4) << (*ddu)->readGbEPrescale() << dec << endl;
	}
	
      }else{ // WRITE
	out << "DDU Write Gb Eth Prescale: " << endl;
	for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu) {
	  out << "  DDU in slot " << (*ddu)->slot() << "..." << endl;
	  (*ddu)->writeGbEPrescale(prescale);
	}
      }
    }

    /**************************************************************************
     * DDU_FakeL1
     *
     *************************************************************************/

    DDU_FakeL1::DDU_FakeL1(Crate * crate)
      : Action(crate),
	ActionValue<string>("read") {}
    
    void DDU_FakeL1::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
			   "Read(read)/Write(hex#) DDU Fake L1 (passthrough)",
			   "mode",
			   value());
    }
    
    void DDU_FakeL1::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>DDU_FakeL1"<<endl; 
      int mode = getFormValueIntHex("mode", in);
      string modeString = getFormValueString("mode", in);
      //value( modeString );
      value("read"); // always default to "read"
      
      if(modeString == "read" ){ // READ
	out << "DDU Read Fake L1 (passthrough): " << endl;
	for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu) {
	  out << "  DDU in slot " << (*ddu)->slot() << hex << setfill('0') << ": " << setw(4) << (*ddu)->readFakeL1() << dec << endl;
	}
	
      }else{ // WRITE
	out << "DDU Write Fake L1 (passthrough): " << endl;
	for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu) {
	  out << "  DDU in slot " << (*ddu)->slot() << "..." << endl;
	  (*ddu)->writeFakeL1(mode);
	}
      }
    }

    /**************************************************************************
     * ExecuteVMEDSL
     *
     * A domain-specific-lanaguage for issuing vme commands. 
     *************************************************************************/

    ExecuteVMEDSL::ExecuteVMEDSL(Crate * crate)
      : Action(crate),
	ActionValue<string>("/home/cscme11/vme.commands") {}
    
    void ExecuteVMEDSL::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
			   "Execute VME DSL commands in file:",
			   "VMEProgramFile",
			   value(),
			   "min-width: 25em; width: 25%; ",
			   "min-width: 40em; width: 70%; ");
    }

    void ExecuteVMEDSL::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>ExecuteVMEDSL"<<endl; 
      int slot = 15; // hard code a default VME slot number
      string programfile = getFormValueString("VMEProgramFile", in);
      value(programfile); // save the value

      // the arguments for vme_controller //
      char rcv[2];
      unsigned int addr;
      unsigned short int data;
      int irdwr;
      //// From VMEController.cc:
      // irdwr:   
      // 0 bufread
      // 1 bufwrite 
      // 2 bufread snd  
      // 3 bufwrite snd 
      // 4 flush to VME (disabled)
      // 5 loop back (disabled)
      // 6 delay
      
      // Read in commands from specified file
      stringstream alltext;
      ifstream file( programfile.c_str() );
      if ( file ){
	alltext << file.rdbuf();
	file.close();
      }

      //stringstream alltext(this->textBoxContents); // get all the text
      string line;

      while (getline(alltext,line,'\n')) // read in one line at a time
	{
	  if(!line.size()) continue; // Next line, please.

	  istringstream iss(line);

	  int num=0;
	  if(iss >> num && iss.good()) // the first token is a number, which we interpret as:
	    {
	      //
	      // 0 : end of run, stop execution
	      //
	      // 1 : The rest of the line must contain exactly 64
	      // characters, which are interpreted as a 32 bit address and
	      // 32 bits of data (to match simulation files).  Spaces are
	      // ignored and characters that are not 0 are interpreted as
	      // 1.  Only the lowest 19 bits of address and 16 bits of
	      // data are used with the exception that the 26th bit (from
	      // right) of address determines read(1) or write(0).
	      //
	      // >1 : Produces a sleep for that number of microseconds
	      //
	      // <0 : Send an error message to the output and abort further execution.

	      if(num==0){
		out<<"Found EOR, exiting."<<endl;
		return; // EOR instruction
	      }
	      else if(num==1) // The line begins with 1, so now expect 32 bits for address and 32 bits for data
		{
		  string addr_str;
		  string data_str;
		  string tmp_str;

		  while(addr_str.size()<32 && iss.good()) // read in 32 bits for address
		    {
		      iss >> tmp_str;
		      addr_str += tmp_str;
		      //out<<"addr_str:"<<addr_str<<endl;
		    }
		  while(data_str.size()<32 && iss.good()) // read in 32 bits for data
		    {
		      iss >> tmp_str;
		      data_str += tmp_str;
		      //out<<"data_str:"<<data_str<<endl;
		    }

		  if(addr_str.size()!=32 || data_str.size()!=32)
		    {
		      out<<"ERROR: address("<<addr_str<<") or data("<<data_str<<") is not 32 bits on line: "<<line<<endl;
		      return;
		    }

		  irdwr = (addr_str.at(addr_str.size()-26)=='1')? 2 : 3; // 26th and 25th "bits" from right tell read (10) or write (01)
		  addr = binaryStringToUInt(addr_str);
		  data = binaryStringToUInt(data_str);
		}
	      else if(num > 1)
		{
		  out<<"sleep for "<<num<<" microseconds"<<endl;
		  out<<flush;
		  usleep(num);
		  continue; // Next line, please.
		}
	      else
		{ // This shouldn't happen
		  out<<"ERROR: line begins with unexpected number = "<<num<<".  Aborting further execution."<<endl;
		  return;
		}
      
	    }
	  else
	    {
	      // Parse the line as follows:
	      //
	      // readfile <int> :
	      // Readfile <int> :
	      // READFILE <int> :
	      //  Read in VME commands from this file.
	      //	  //
	      // slot <int> :
	      // Slot <int> :
	      // SLOT <int> :
	      //  Change the slot number to <int>.
	      //
	      // w <hex address> <hex data> :
	      // W <hex address> <hex data> :
	      //  A write command using the lowest 19/16 bits of give address/data.
	      //
	      // r <hex address> <hex data> :
	      // R <hex address> <hex data> :
	      //  A read command using the lowest 19/16 bits of give address/data.
	      //
	      // Anything else is treated as a comment.

	      iss.clear(); // reset iss
	      iss.str(line); // not needed, but just to make it clear
	      string key;
	      iss >> key;

	      if( key == "readfile" ||
		  key == "Readfile" ||
		  key == "READFILE" )
		{ //
		  string tmp_filename;
		  if( iss >> tmp_filename )
		    {
		      programfile = tmp_filename;
		      out << "Read commands from file: " << programfile << endl;
		      continue; // Next line, please.
		    }
		  else
		    {
		      out<<"ERROR: did not find an integer number after \""<<key<<"\" on the line: "<<line<<endl;
		      out<<"  Aborting further execution."<<endl;
		      return;
		    }
		}
	      else if( key == "slot" ||
		       key == "Slot" ||
		       key == "SLOT" ) // Try to change the slot number
		{
		  int tmp_slot;
		  if( iss >> tmp_slot )
		    {
		      slot = tmp_slot;
		      out << "Slot number set to " << slot << endl;
		      continue; // Next line, please.
		    }
		  else
		    {
		      out<<"ERROR: did not find an integer number after \""<<key<<"\" on the line: "<<line<<endl;
		      out<<"  Aborting further execution."<<endl;
		      return;
		    }
		}
	      else if( key == "w" ||
		       key == "W" ||
		       key == "r" ||
		       key == "R" )
		{
		  if( key == "w" || key == "W" ) irdwr = 2;
		  else irdwr = 3;

		  if( !(iss >> hex >> addr) || !(iss >> hex >> data) )
		    { // put hex string directly into addr/data
		      out<<"ERROR: problem reading hex values for address or data on the line: "<<line<<endl;
		      out<<"  Aborting further execution."<<endl;
		      return;
		    }

		}
	      else
		{
		  out<<"COMMENT: "<<line<<endl;
		  continue; // Next line, please.
		}
	    }

	  //// If we make it here, we have a VME command to run! ////

	  // set the top bits of address to the slot number
	  addr = (addr&0x07ffff) | slot<<19;

	  printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x})  ",irdwr,addr&0xffffff,data&0xffff,rcv[0]&0xff,rcv[1]&0xff);
	  crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); // Send the VME command!
	  VMEController::print_decoded_vme_address(addr,&data);
	  usleep(1);

	  // if it was a read, then show the result
	  if(irdwr==2) printf("  ==> rcv[1,0] = %02x %02x",rcv[1]&0xff,rcv[0]&0xff);
	  printf("\n");
	  fflush(stdout);

	} // while parsing lines
    }


     /**************************************************************************
     * CommonUtilities_setupDDU_passThrough
     * -- S.Z. Shalhout June 26, 2013 (sshalhou@cern.ch)
     *************************************************************************/

    CommonUtilities_setupDDU_passThrough::CommonUtilities_setupDDU_passThrough(Crate * crate)
      : Action(crate) {}
 
    void CommonUtilities_setupDDU_passThrough::display(xgi::Output * out)
    {
      addButton(out, "SetUp DDU PassThrough","width: 100%; ");
    } 

    void CommonUtilities_setupDDU_passThrough::respond(xgi::Input * in, ostringstream & out)
    {

      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu)
	{
	
	  (*ddu)->writeFlashKillFiber(0x7fff); 
	  usleep(20);
	  ccb_->HardReset_crate();
	  usleep(250000);
	  (*ddu)->writeGbEPrescale( 0x7878 ); // 0x7878: test-stand without TCC
	  usleep(10);
	  (*ddu)->writeFakeL1( 0x8787 ); // 0x8787: passthrough // 0x0000: normal
	  usleep(10);
	  ccb_->l1aReset();
	  usleep(50000);
	  usleep(50000);
	  ccb_->bc0();


	} 

    }

    /**************************************************************************
     * CommonUtilities_setupDDU
     * -- S.Z. Shalhout April 26, 2013 (sshalhou@cern.ch)
     *************************************************************************/

    CommonUtilities_setupDDU::CommonUtilities_setupDDU(Crate * crate)
      : Action(crate) {}
 
    void CommonUtilities_setupDDU::display(xgi::Output * out)
    {
      addButton(out, "SetUp DDU ","width: 100%; ");
    } 

    void CommonUtilities_setupDDU::respond(xgi::Input * in, ostringstream & out)
    {

      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu)
	{
	
	  (*ddu)->writeFlashKillFiber(0x7fff); 
	  usleep(20);
//szsX	  ccb_->HardReset_crate();
	  usleep(250000);
	  (*ddu)->writeGbEPrescale( 0x7878 ); // 0x7878: test-stand without TCC
	  usleep(10);
	  (*ddu)->writeFakeL1( 0x0000 ); // 0x8787: passthrough // 0x0000: normal
	  usleep(10);
	  ccb_->l1aReset();
	  usleep(50000);
	  usleep(50000);
	  ccb_->bc0();


	} 

    }


  
    /**************************************************************************
     * CommonUtilities_restoreCFEBIdle
     * -- S.Z. Shalhout April 26, 2013 (sshalhou@cern.ch)
     *************************************************************************/

    CommonUtilities_restoreCFEBIdle::CommonUtilities_restoreCFEBIdle(Crate * crate)
      : Action(crate) {}
 
    void CommonUtilities_restoreCFEBIdle::display(xgi::Output * out)
    {
      addButton(out, "Restore CFEBS to IDLE","width: 100%; ");
    } 

    void CommonUtilities_restoreCFEBIdle::respond(xgi::Input * in, ostringstream & out)
    {

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){ (*dmb)->restoreCFEBIdle(); }

    }


  } // namespace otmbdev
} // namespace emu
