#ifndef KAPPA_PARTICLE_H
#define KAPPA_PARTICLE_H

#include "KBasic.h"

/// particleinfo = [charge:1][status:3][id:...]
const unsigned int KParticleStatusPosition = 28;
const unsigned int KParticleSignPosition   = 31;
const unsigned int KParticleSignMask   =  (unsigned int)1 << KParticleSignPosition;
const unsigned int KParticleStatusMask =  (unsigned int)3 << KParticleStatusPosition;
const unsigned int KParticlePdgIdMask  = ((unsigned int)1 << KParticleStatusPosition) - (unsigned int)1;

/// Particle base class for generator particles or candidates
struct KParticle : public KLV
{
	unsigned int particleinfo;

	int status() const
	{
		return (particleinfo & KParticleStatusMask) >> KParticleStatusPosition;
	}
	int pdgId() const
	{
		return (particleinfo & KParticleSignMask ? -1 : 1) * (particleinfo & KParticlePdgIdMask);
	}
	int sign() const
	{
		return (particleinfo & KParticleSignMask ? -1 : 1);
	}

	/// particle charge multiplied by 3 (for integer comparisons)
	// e.g. 2 for up-quark, -3 for electron
	// valid for quarks, leptons, bosons, diquarks, mesons and baryons
	// no special, technicolor, SUSY, Kaluza-Klein particles, R-hadrons and pentaquarks
	int chargeTimesThree() const
	{
		int pdg = std::abs(pdgId());
		int had = pdg % 10000 / 10;  // quark content of hadrons (digits 2-4)

		if (pdg == 0)
			return 0;
		// quark
		if (pdg < 9)
			return (-3 * (pdg % 2) + 2) * sign();
		// lepton
		if (pdg > 10 && pdg < 19)
			return -3 * (pdg % 2) * sign();
		// boson
		if (pdg > 20 && pdg < 40)
			return (pdg == 24 || pdg == 34 || pdg == 37) ? 3 * sign() : 0;
		// meson
		if (had < 100)
		{
			int q1 = -3 * (had / 10 % 2) + 2;
			int q2 = -3 * (had % 2) + 2;
			return (q1 - q2) * sign();
		}
		// baryon
		else
		{
			int q1 = had / 100;
			int q2 = had / 10 % 10;
			int q3 = had % 10;
			q1 = (q1 == 0) ? 0 : -3 * (q1 % 2) + 2;
			q2 = (q2 == 0) ? 0 : -3 * (q2 % 2) + 2;
			q3 = (q3 == 0) ? 0 : -3 * (q3 % 2) + 2;
			return (q1 + q2 + q3) * sign();
		}
	}

	/// particle charge
	double charge() const
	{
		return chargeTimesThree() / 3.;
	}
};
typedef std::vector<KParticle> KParticles;


/// Generator particle
struct KGenParticle : public KParticle
{
	std::vector<unsigned int> daughterIndices;

	/// return the number of daughters
	int nDaughters() const
	{
		return daughterIndices.size();
	}

	/// return index of daughter i
	/** checks if i<numberOfDaughters
	    returns -1 in case i is out of bounds
	 */
	unsigned int daughterIndex(unsigned int i) const
	{
		if (i<daughterIndices.size())
			return daughterIndices.at(i);
		else
			return -1;
	}
};
typedef std::vector<KGenParticle> KGenParticles;

struct KGenPhoton : public KLV
{
	KLV mother;
	char type;
	bool isPhoton() const { return (type == 1); }
	bool isPi0() const { return (type == 2); }
};
typedef std::vector<KGenPhoton> KGenPhotons;

struct KGenTau : public KGenParticle
{
	KLV visible;		// momenta of visible particles
	RMPoint vertex;

	/// decay mode
	/** bit definition:
	0 - undefined
	1 - electron
	2 - muon
	>2 - hadronic
	     3 - 1prong
	     4 - 3prong
	     5 - >3prong
	most significant bit (1<<7):
		0 = tau
		1 = descendant of a tau
	*/
	unsigned char decayMode;

	static const int DescendantMask = 1 << 7;

	bool isElectronicDecay() const { return (decayMode & ~DescendantMask) == 1; }
	bool isMuonicDecay() const { return (decayMode & ~DescendantMask) == 2; }
	bool isHadronicDecay() const { return (decayMode & ~DescendantMask) > 2; }
	bool isDescendant() const { return (decayMode & DescendantMask) != 0; }
};
typedef std::vector<KGenTau> KGenTaus;


/// Particle-Flow Candidate
struct KPFCandidate : public KParticle
{
	double deltaP;
	double ecalEnergy, hcalEnergy;
};
typedef std::vector<KPFCandidate> KPFCandidates;

#endif
