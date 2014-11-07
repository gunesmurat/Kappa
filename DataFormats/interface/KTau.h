/* Copyright (c) 2010 - All Rights Reserved
 *   Armin Burgmeier <burgmeier@ekp.uni-karlsruhe.de>
 *   Fred Stober <stober@cern.ch>
 *   Joram Berger <joram.berger@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#ifndef KAPPA_TAU_H
#define KAPPA_TAU_H

#include "KBasic.h"
#include "KMetadata.h"

struct KDataTau : KLepton
{
	/*
	virtual ~KDataTau() {};
	
	virtual bool isTau() {
		return true;
	};
	*/
	
	unsigned long long binaryDiscriminators;
	std::vector<float> floatDiscriminators;

	bool hasID(const std::string& name, const KTauDiscriminatorMetadata * lumimetadata) const
	{
		for(size_t i = 0; i < lumimetadata->binaryDiscriminatorNames.size(); ++i)
			if(lumimetadata->binaryDiscriminatorNames[i] == name)
				return (binaryDiscriminators & (1ull << i)) != 0;
		return false; // Named discriminator does not exist
	}

	float getDiscriminator(const std::string& name, const KTauDiscriminatorMetadata * lumimetadata) const
	{
		for(size_t i = 0; i < lumimetadata->floatDiscriminatorNames.size(); ++i)
			if(lumimetadata->floatDiscriminatorNames[i] == name)
				return floatDiscriminators[i];
		return -999.0; // Named discriminator does not exist
	}
};
typedef std::vector<KDataTau> KDataTaus;

struct KDataCaloTau : KDataTau {};
typedef std::vector<KDataCaloTau> KDataCaloTaus;

struct KDataPFTau : KDataTau
{
	float emFraction;

	int nSignalChargedHadrCands, nSignalGammaCands, nSignalPiZeroCands, nSignalCands;
	int nSignalTracks;

	KLV leadCand;
	KLV leadChargedHadrCand;
	KLV leadNeutralCand;

	KPFCandidates signalChargedHadrCands;
	KPFCandidates signalGammaCands;
	KLVs signalPiZeroCands;

	KDataTrack track;

	//bool longLived
	int hpsDecayMode; // hadronic decay mode as identified by HPS algorithm
	int tauKey;
};
typedef std::vector<KDataPFTau> KDataPFTaus;

struct KDataGenTau : KGenParticle
{
	KLV visible;		// momenta of visible particles
	unsigned char decayMode;
	//  0 - undefined
	//	1 - electron
	//	2 - muon
	// >2 - hadronic
	//      3 - 1prong
	//      4 - 3prong
	//      5 - >3prong
	// most significant bit (1<<7):
	//		0 = tau
	//		1 = descendant of a tau
	RMPoint vertex;

	static const int DescendantPosition = 7;
	static const int DescendantMask = 1 << DescendantPosition;

	bool isElectronicDecay() const { return (decayMode & ~DescendantMask) == 1; }
	bool isMuonicDecay() const { return (decayMode & ~DescendantMask) == 2; }
	bool isHadronicDecay() const { return (decayMode & ~DescendantMask) > 2; }

	bool isDescendant() const { return (decayMode & DescendantMask) != 0; }
};
typedef std::vector<KDataGenTau> KDataGenTaus;

#endif
