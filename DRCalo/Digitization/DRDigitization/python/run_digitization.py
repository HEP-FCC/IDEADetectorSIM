import ROOT

ROOT.gSystem.Load("libDRCalo_DRDigitizationLib.so")
#ROOT.gSystem.Load("libsipm.so")

def main():
    import argparse

    #parse arguments from command line

    parser = argparse.ArgumentParser(description='This script runs the digitization code from Insubria on an podio input file (default arguments assume a file like the one prepared by the Pavia simulation')

    parser.add_argument('--input', dest='ifile',help='Input file name. Leave empty to run with no input file (for example to debug sensor properties)')
    parser.add_argument('--output',dest='ofile',help='Output file name',default='output_digit.podio.root')
    parser.add_argument('--debug',action='store_true',dest='debug',default=False, help='Print more debugging information')
    parser.add_argument('--do_calibration',action='store_true',dest='do_cal',default=False, help='Run the calibration step')
    parser.add_argument('--SiPMproperties', action = type('', (argparse.Action, ), dict(__call__ = lambda a, p, n, v, o: getattr(n, a.dest).update(dict([v.split('=')])))), default = {},help='sets one property of the SiPM sensor. The sintaxt is "--SiPMproperties Size=6 --SiPMproperties Pitch=15", etc.') # hack to pass a dictionary
    
    par  = parser.parse_args()

    
    digitsteer = ROOT.DigitSteer()
    digitsteer.DoCalibration(par.do_cal)
    digitsteer.SetDebug(par.debug)

    #create the FiberSigitizer object
    
    fbDigit = ROOT.FiberDigitizer()
    
    #creating SiPM sensor and setting its properties
    
    SiPMprop = ROOT.sipm.SiPMProperties()
    for mykey in par.SiPMproperties.keys():
        SiPMprop.setProperty(mykey,float(par.SiPMproperties[mykey]))

    print('Sensor properties:\n')
    
    SiPMprop.dumpSettings()
    print ('\n')
    SiPMsensor = ROOT.sipm.SiPMSensor(SiPMprop)
    fbDigit.SetSiPMSensor(SiPMsensor)
    digitsteer.SetOutputFileName(par.ofile)
    digitsteer.SetFiberDigitizer(fbDigit)

    if (par.ifile == None):
        return

    print("Ready to digitise file " + par.ifile)
    
    digitsteer.LoadFile(par.ifile)

    #....aaaaand run
    digitsteer.Process()
    
    digitsteer.CloseFile()


if __name__ == "__main__":
    main()

