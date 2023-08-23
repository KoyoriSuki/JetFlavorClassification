#include "Pythia8/Pythia.h"
#include "fastjet/ClusterSequence.hh"

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TGraph2D.h"
#include "TCanvas.h"
#include "TPDF.h"

#include <iostream>
#include <fstream>

using namespace Pythia8;
using namespace fastjet;
using namespace std;

// A function to match 4-momentum with particle
// because Fastjet loses this information
Particle findParticleByFourMomentum(PseudoJet constituent, vector <Particle> finalParticlesInfo)
{
	for (unsigned i = 0; i < finalParticlesInfo.size(); i++)
	{
		double nearlyEqual = 1e-10;
		double flag1 = fabs(constituent.px() - finalParticlesInfo[i].px());
		double flag2 = fabs(constituent.py() - finalParticlesInfo[i].py());
		double flag3 = fabs(constituent.pz() - finalParticlesInfo[i].pz());
		double flag4 = fabs(constituent.e()  - finalParticlesInfo[i].e() );
		if (flag1 + flag2 + flag3 + flag4 < nearlyEqual)return finalParticlesInfo[i];
	}
}

// Identify if a particle is a neutrino
int isNeutrino(Particle p)
{
	int thisID = abs(p.id());
	if (thisID == 12 || thisID == 14 || thisID == 16)return 1;
	else return 0;
}

int main() {
	// Create an instance of Pythia class
	Pythia pythia;

	// Pick new random number seed for each run, based on /dev/urandom
	srand(time(0));
	int randomN = rand() % 100000;
	string seed = to_string(randomN);
	cout << "string seed " << seed << endl;
	pythia.readString("Random:setSeed = on");
	pythia.readString("Random:seed = " + seed);

	// Set the beam type and energies in GeV
	pythia.readString("Beams:eCM = 200.");
	pythia.readString("Beams:idA = 2212");
	pythia.readString("Beams:idB = 2212");

	// Set the process selection
	pythia.readString("HardQCD:all = on");

	// Set event parameters from input file
	int nEvent = 0;
	int printFlag = 1;
	double jetPtCutoff = 0; //GeV
	int thread = 0;
	FILE* inputFile;
	if ((inputFile = fopen("input.dat", "r+")) == nullptr)
	{
		cout << "Cannot open input file!" << endl;
		return -1;
	}
	fscanf(inputFile, "EventNum:%d\nPrintOption:%d\nJetPtCutoff:%lf\nThread:%d", &nEvent, &printFlag, &jetPtCutoff, &thread);
	fclose(inputFile);

	// Pythia initialization
	pythia.init();

	// Fastjet initialization
	double R = 0.7;
	JetDefinition jet_def(antikt_algorithm, R);

	// Histograms
	TCanvas* canvas = new TCanvas("canvas", "title", 800, 600);
	TH2D* jetsDistribution = new TH2D("jetsDistribution", "jetsDistribution", 100, -12, 12, 100, -3.1416, 3.1416);
	vector <int> particleIDs;

	// Test variables
	int nHeavyFlavorJet = 0;

	// Output data file
	string fileName = "trainingData_thread" + to_string(thread) + ".txt";
	FILE* trainingData = fopen(fileName.c_str(), "w+");

	// Begin event loop. Generate event. Skip if error. List first one.
	for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

		// Generate a new event
		if (!pythia.next()) continue;

		// Event statistics
		int nCharged = 0;
		std::vector <fastjet::PseudoJet> finalParticlesFourMomentum;
		std::vector <Particle> finalParticlesInfo;
		finalParticlesFourMomentum.resize(0);
		finalParticlesInfo.resize(0);
		static int firstEvent = 1;
		jetsDistribution->Reset();

		// Begin track loop
		for (int i = 0; i < pythia.event.size(); i++)
		{
			Particle& p = pythia.event[i];

			// Charged multiplicity
			if (p.isFinal() && p.isCharged())++nCharged;

			// Fastjet input
			if (p.isFinal() && !isNeutrino(p))
			{
				finalParticlesFourMomentum.push_back(fastjet::PseudoJet(p.px(),p.py(), p.pz(), p.e()));
				finalParticlesInfo.push_back(p);
			}

			// final particles info
			if (p.isFinal())
			{
				if (printFlag) // Print the particle's info
				{
					cout << "Particle " << i << ": id = " << p.id() << ", status = " << p.status()
						<< ", pT = " << p.pT() << ", eta = " << p.eta() << ", phi = " << p.phi() << endl;
					jetsDistribution->Fill(p.eta(), p.phi(), p.pT());			// Fill TGraph2D with axis y, phi, pt
				}
				if (find(particleIDs.begin(), particleIDs.end(), p.id()) == particleIDs.end()) // record all possible final particles
				{
					particleIDs.push_back(p.id());
				}
			}
			

		}

		// Fastjet find jet
		ClusterSequence fastjetClusterSequence(finalParticlesFourMomentum, jet_def);
		vector<PseudoJet> jets = sorted_by_pt(fastjetClusterSequence.inclusive_jets());
		
		// For the first event, print the FastJet details
		if (firstEvent) {
			cout << "Ran " << jet_def.description() << endl;
			cout << "Strategy adopted by FastJet was "
				<< fastjetClusterSequence.strategy_string() << endl << endl;
			jetsDistribution->Draw("LEGO1");
			TFile* f = new TFile("output.root", "RECREATE");
			jetsDistribution->Write();
			canvas->Write();
			f->Close();
			firstEvent = 0;
		}

		// print the jets info to shell
		if (printFlag)
		{
			cout << "        pt y phi" << endl;
			for (unsigned i = 0; i < jets.size(); i++) {
				cout << "jet " << i << ": " << jets[i].pt() << " " << jets[i].rap() << " " << jets[i].phi() << endl;
				vector<PseudoJet> constituents = jets[i].constituents();
				for (unsigned j = 0; j < constituents.size(); j++) {
					cout << "    constituent " << j << "'s pt: " << constituents[j].pt() << " " << "chain: ";
					Particle thisParticle = findParticleByFourMomentum(constituents[j], finalParticlesInfo);
					cout << pythia.particleData.name(thisParticle.id());
					int motherIndex = thisParticle.mother1();
					while (motherIndex != 0)
					{
						cout << "<-" << pythia.particleData.name(pythia.event[motherIndex].id()) << "(index:" << pythia.event[motherIndex].index() << ")";
						motherIndex = pythia.event[motherIndex].mother1();
					}
					cout << endl;
				}
			}
		}

		//write the jet nearest to the quark info for ML training
		double deltaR = 1e10;
		Particle flavorQuark = pythia.event[5];
		int heavyFlavorFlag = 0;
		if (abs(flavorQuark.id()) == 4 || abs(flavorQuark.id()) == 5 || abs(flavorQuark.id()) == 6)heavyFlavorFlag = 1;
		int nearestJetIndex = 0;
		for (unsigned i = 0; i < jets.size(); i++) // find the nearest jet
		{
			double tempDeltaR = jets[i].delta_R(PseudoJet(flavorQuark.px(), flavorQuark.py(), flavorQuark.pz(), flavorQuark.e()));
			if (tempDeltaR < deltaR)
			{
				deltaR = tempDeltaR;
				nearestJetIndex = i;
			}
		}
		if (jets[nearestJetIndex].pt() > jetPtCutoff) //pt cutoff
		{
			vector<PseudoJet> constituents = jets[nearestJetIndex].constituents();
			fprintf(trainingData, "%d %d  %lf %lf %lf %lf\n", constituents.size(), flavorQuark.id(), jets[nearestJetIndex].px(), jets[nearestJetIndex].py(), jets[nearestJetIndex].pz(), jets[nearestJetIndex].e());
			for (unsigned j = 0; j < constituents.size(); j++) //constituents loop
			{
				Particle thisParticle = findParticleByFourMomentum(constituents[j], finalParticlesInfo);
				fprintf(trainingData, "%d %d %le %le %le %le %le %le %le %le\n", j + 1, thisParticle.id(), thisParticle.px(), thisParticle.py(), thisParticle.pz(), thisParticle.e(), thisParticle.xProd(), thisParticle.yProd(), thisParticle.zProd(), thisParticle.tProd());
			}
			//fprintf(trainingData, "%d %d \n\n", pythia.event[5].id(), pythia.event[5].index());
		}
		
		// End of event loop
		if (printFlag)cout << endl;


	}

	if (printFlag)
	{
		cout << "Heavy Flavor: " << nHeavyFlavorJet << endl;
	}
	for (auto it = particleIDs.begin(); it != particleIDs.end(); it++)
	{
		cout << pythia.particleData.name(*it) << "(" << *it << ")" << " ";
	}

	// Finalize the event generation
	pythia.stat();
	return 0;
}