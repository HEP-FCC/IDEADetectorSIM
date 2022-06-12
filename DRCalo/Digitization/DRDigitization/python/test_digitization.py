from EventStore import EventStore
import ROOT

def getCosTheta(x,y,z):
    return ROOT.TMath.Cos(getTheta(x,y,z))

def getPhi(x,y,z):
    v = ROOT.TVector3()
    v.SetXYZ(x,y,z)
    return v.Phi()

def getTheta(x,y,z):
    v = ROOT.TVector3()
    v.SetXYZ(x,y,z)
    return v.Theta()

def main():
    import argparse

    #parse arguments from command line

    parser = argparse.ArgumentParser(description='This script does some validation plotting. One can simply pass teh input digit file and get some histograms. If one also passes the hits file, one gets direct comparisons of the two estimates of quantities')

    parser.add_argument('--input', dest='ifile',help='Input file name. Leave empty to run with no input file (for example to debug sensor properties)')
    parser.add_argument('--outputROOT',dest='ofile',help='Output file name',default='output_digit.podio.root')
    parser.add_argument('--inputHits',dest='hitfile',help='Optiona input hits file')
    parser.add_argument('--debug',action='store_true',dest='debug',default=False, help='Print more debugging information')
    parser.add_argument('--outputPdf',dest='outPDF',default='validation_output.pdf',help='Name of teh output pdf file')
    
    par  = parser.parse_args()

    filename = par.ifile
    hitfilename = par.hitfile
    ofilename = par.ofile

    store = EventStore(filename)
    storehit = None

    ofile = ROOT.TFile(ofilename,"recreate")
    ofile.mkdir("Digitization")
    
    ofile.cd("Digitization")

    h_list = []
    
    h_c_fiber_energy = ROOT.TH1F("h_c_fiber_energy","",100,0,-1)
    h_c_fiber_energy.SetXTitle("Individual C fibre energy (arbitrary units)")
    h_list.append(h_c_fiber_energy)
    h_s_fiber_energy = ROOT.TH1F("h_s_fiber_energy","",100,0,-1)
    h_s_fiber_energy.SetXTitle("Individual S fibre energy (arbitrary units)")
    h_list.append(h_s_fiber_energy)
    h_c_fiber_time = ROOT.TH1F("h_c_fiber_time","",100,0,-1)
    h_c_fiber_time.SetXTitle("Individual C fibre time [ns]")
    h_list.append(h_c_fiber_time)    
    h_s_fiber_time = ROOT.TH1F("h_s_fiber_time","",100,0,-1)
    h_s_fiber_time.SetXTitle("Individual S fibre time [ns]")
    h_list.append(h_s_fiber_time)
        
    h_c_energy = ROOT.TH1F("h_c_energy","",100,0,-1)
    h_c_energy.SetXTitle("Total C Energy (a.u.)")
    h_list.append(h_c_energy)    
    h_s_energy = ROOT.TH1F("h_s_energy","",100,0,-1)
    h_s_energy.SetXTitle("Total S Energy (a.u.)")
    h_list.append(h_s_energy)    
    h_c_time = ROOT.TH1F("h_c_time","",100,0,-1)
    h_c_time.SetXTitle("Average C time [ns]")
    h_list.append(h_c_time)    
    h_s_time = ROOT.TH1F("h_s_time","",100,0,-1)
    h_s_time.SetXTitle("Average C time [ns]")
    h_list.append(h_s_time)    
    
    h_n_s_fiber = ROOT.TH1F("h_n_s_fiber","",100,0,-1)
    h_n_s_fiber.SetXTitle("S fibre multiplicity")
    h_list.append(h_n_s_fiber)    
    h_n_c_fiber = ROOT.TH1F("h_n_c_fiber","",100,0,-1)
    h_n_c_fiber.SetXTitle("C fibre multiplicity")
    h_list.append(h_n_c_fiber)

    h_pos_s = ROOT.TH2F("h_pos_s","",20,-1,1,64,-3.2,3.2)
    h_pos_s.SetXTitle("cos #theta")
    h_pos_s.SetYTitle("#phi")
    h_list.append(h_pos_s)

    h_pos_c = ROOT.TH2F("h_pos_c","",20,-1,1,64,-3.2,3.2)
    h_pos_c.SetXTitle("cos #theta")
    h_pos_c.SetYTitle("#phi")
    h_list.append(h_pos_c)
    
        
    c = ROOT.TCanvas()

    if hitfilename != None:
        storehit = EventStore(hitfilename)
        ofile.mkdir("DigiVsHits")
        ofile.cd("DigiVsHits")
        h_c_ratio_simdigi = ROOT.TH1F("h_c_ratio_simdigi","",100,0,-1)
        h_c_ratio_simdigi.SetXTitle("Hit energy/Digi energy")
        h_list.append(h_c_ratio_simdigi)    
        h_s_ratio_simdigi = ROOT.TH1F("h_s_ratio_simdigi","",100,0,-1)
        h_s_ratio_simdigi.SetXTitle("Hit energy/Digi energy")
        h_list.append(h_s_ratio_simdigi)    
    
    for i, event in enumerate(store):
        if i % 1000 == 0:
            print('reading event', i)
        c_hits = event.get("C_CalorimeterHits")
        s_hits = event.get("S_CalorimeterHits")
        tot_s_energy = 0
        tot_c_energy = 0
        tot_s_time = 0
        tot_c_time = 0
        n_s_fiber = 0
        n_c_fiber = 0
        for hit in c_hits:
            h_c_fiber_energy.Fill(hit.getEnergy())
            h_c_fiber_time.Fill(hit.getTime())
            tot_c_energy = tot_c_energy + hit.getEnergy()
            tot_c_time = tot_c_time + hit.getTime()
            n_c_fiber = n_c_fiber + 1
            h_pos_c.Fill(getCosTheta(hit.getPosition().x,
                                     hit.getPosition().y,
                                     hit.getPosition().z),
                         getPhi(hit.getPosition().x,
                                hit.getPosition().y,
                                hit.getPosition().z))   
        for hit in s_hits:
            h_s_fiber_energy.Fill(hit.getEnergy())
            h_s_fiber_time.Fill(hit.getTime())
            tot_s_energy = tot_s_energy + hit.getEnergy()
            tot_s_time = tot_s_time + hit.getTime()
            n_s_fiber = n_s_fiber + 1
            h_pos_s.Fill(getCosTheta(hit.getPosition().x,
                                     hit.getPosition().y,
                                     hit.getPosition().z),
                         getPhi(hit.getPosition().x,
                                hit.getPosition().y,
                                hit.getPosition().z))   
        
        if (n_s_fiber != 0):
            tot_s_time = tot_s_time/n_s_fiber
        if (n_c_fiber != 0):
            tot_c_time = tot_c_time/n_c_fiber

        h_c_energy.Fill(tot_c_energy)
        h_s_energy.Fill(tot_s_energy)
        h_c_time.Fill(tot_c_time)
        h_s_time.Fill(tot_s_time)

        h_n_s_fiber.Fill(n_s_fiber + 0.5)
        h_n_c_fiber.Fill(n_c_fiber + 0.5)
    
        # ok done, now let's try to access the hits as well

        if hitfilename != None:

            eventhits = storehit[i]
            c_simhits = eventhits.get("C_caloHits")
            s_simhits = eventhits.get("S_caloHits")

            tot_s_hitenergy = 0
            tot_c_hitenergy = 0

            for hit in c_simhits:
                tot_c_hitenergy = tot_c_hitenergy + hit.getEnergy()
            for hit in s_simhits:
                tot_s_hitenergy = tot_s_hitenergy + hit.getEnergy()

            if (tot_c_energy != 0):
                h_c_ratio_simdigi.Fill(tot_c_hitenergy/tot_c_energy)
            if (tot_s_energy != 0):
                h_s_ratio_simdigi.Fill(tot_s_hitenergy/tot_s_energy)

    c.Print(par.outPDF+'[')
    for h in h_list:
        c.Clear()
        h.Draw()
        c.Print(par.outPDF)
    c.Print(par.outPDF+']')
    

    ofile.Write()
    ofile.Close()


if __name__ == "__main__":
    main()



