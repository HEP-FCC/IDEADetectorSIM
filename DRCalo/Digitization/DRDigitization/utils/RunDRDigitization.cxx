#include "DigitSteer.h"
#include "FiberDigitizer.h"
#include "SiPMSensor.h"
#include "SiPMProperties.h"
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

int main(int argc, char **argv)
{
  std::cout << "*********************************\n";
  std::cout << "     " << argv[0] << "       \n"; 
  std::cout << "*********************************\n";
  std::cout << "\n\n" << std::endl;
  
  if (argc != 3) {
    std::cout << "USAGE: " << argv[0] << " [INPUTFILE] [OUTPUTFILE]\n";
    std::cout << "where [INPUTFILE] is the podio file containing simulation hits\n";
    return -1;
  }

  std::cout << "Setting output  file name to " << argv[2] << std::endl;

  podio::EventStore l_store;                                                                                 
  podio::ROOTWriter l_writer(argv[2],&l_store);

  DigitSteer l_steer;
  l_steer.SetDebug(false);

  l_steer.SetOutputEventStore(&l_store);
  l_steer.SetOutputROOTWriter(&l_writer);
  
  std::cout << "Loading input file " << std::endl;
  if (!l_steer.LoadFile(argv[1])) return -1;


  // Now configuring the SiPM sensor
  sipm::SiPMProperties l_properties;

  // creating a default sensor. Many properties of the sensor are available
  
  sipm::SiPMSensor l_sensor (l_properties);

  // Now creating the FiberDigitizer, which acts asa hook to the sensor. A possible property of the FiberDigitizer is teh integration parameters (look at the header file)
  
  FiberDigitizer l_digitizer;
  l_digitizer.SetSiPMSensor(&l_sensor);
  
  std::cout << "Starting process" << std::endl;
  l_steer.SetFiberDigitizer(&l_digitizer);
  l_steer.DoCalibration(false); // The calibration it technically doable, but it needs to be done 
  l_steer.Process();
  std::cout << "Closing file and exiting" << std::endl;
  l_steer.CloseFile();
  return 0;

}

