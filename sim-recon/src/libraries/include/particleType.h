/*
 * particleType.h
*/

#ifndef particleTypeH_INCLUDED
#define particleTypeH_INCLUDED

#include <math.h>
#include <stdio.h>
#include <string.h>

typedef enum {

  /* An extensive list of the GEANT3 particle
   * codes can be found here:
   * http://wwwasdoc.web.cern.ch/wwwasdoc/geant/node72.html
  */

  Unknown        =  0,
  Gamma          =  1,
  Positron       =  2,
  Electron       =  3,
  Neutrino       =  4,
  MuonPlus       =  5,
  MuonMinus      =  6,
  Pi0            =  7,
  PiPlus         =  8,
  PiMinus        =  9,
  KLong          = 10,
  KPlus          = 11,
  KMinus         = 12,
  Neutron        = 13,
  Proton         = 14,
  AntiProton     = 15,
  KShort         = 16,
  Eta            = 17,
  Lambda         = 18,
  SigmaPlus      = 19,
  Sigma0         = 20,
  SigmaMinus     = 21,
  Xi0            = 22,
  XiMinus        = 23,
  OmegaMinus     = 24,
  AntiNeutron    = 25,
  AntiLambda     = 26,
  AntiSigmaMinus = 27,
  AntiSigma0     = 28,
  AntiSigmaPlus  = 29,
  AntiXi0        = 30,
  AntiXiPlus     = 31,
  AntiOmegaPlus  = 32,
  Deuteron       = 45,
  Helium         = 47,
  Geantino       = 48,
  Triton         = 49,
  
  Pb208          = 111,

  /* the constants defined by GEANT end here */
  
  /*
   * Several particle codes are added below which did
   * not overlap with the original GEANT particle list.
   * However, the link above has several other particle
   * codes added which do actually overlap. Because of 
   * this, each of the values below was increased by
   * 100 so that they don't overlap with any of the new
   * codes and they can be used.
  */
  

  /* These are E852-defined constants */
  Rho0           = 44, // was 157
  RhoPlus        = 42, // was 158
  RhoMinus       = 43, // was 159
  omega          = 33,  // was 160
  phiMeson       = 34,  // was 162
  EtaPrime       = 35,  // was 161
  //Rho0           = 157,
  //RhoPlus        = 158,
  //RhoMinus       = 159,
  //omega          = 160,
  //EtaPrime       = 161,
  //phiMeson       = 162,
  a0_980	 = 163,
  f0_980	 = 164,

  /* These are GlueX-defined constants */

  KStar_892_0 = 165,
  KStar_892_Plus = 166,
  KStar_892_Minus = 167,
  AntiKStar_892_0 = 168,

  K1_1400_Plus = 169,
  K1_1400_Minus = 170,

  b1_1235_Plus = 171,
  Sigma_1385_Minus = 172,
  Sigma_1385_0 = 173,
  Sigma_1385_Plus = 174

} Particle_t;

inline static char* ParticleType(Particle_t p)
{
  switch (p) {
  case Unknown:
    return (char*)"Unknown";
  case Gamma:
    return (char*)"Photon";
  case Positron:
    return (char*)"Positron";
  case Electron:
    return (char*)"Electron";
  case Neutrino:
    return (char*)"Neutrino";
  case MuonPlus:
    return (char*)"Muon+";
  case MuonMinus:
    return (char*)"Muon-";
  case Pi0:
    return (char*)"Pi0";
  case PiPlus:
    return (char*)"Pi+";
  case PiMinus:
    return (char*)"Pi-";
  case KLong:
    return (char*)"KLong";
  case KPlus:
    return (char*)"K+";
  case KMinus:
    return (char*)"K-";
  case Neutron:
    return (char*)"Neutron";
  case Proton:
    return (char*)"Proton";
  case AntiProton:
    return (char*)"AntiProton";
  case KShort:
    return (char*)"KShort";
  case Eta:
    return (char*)"Eta";
  case Lambda:
    return (char*)"Lambda";
  case SigmaPlus:
    return (char*)"Sigma+";
  case Sigma0:
    return (char*)"Sigma0";
  case SigmaMinus:
    return (char*)"Sigma-";
  case Xi0:
    return (char*)"Xi0";
  case XiMinus:
    return (char*)"Xi-";
  case OmegaMinus:
    return (char*)"Omega-";
  case AntiNeutron:
    return (char*)"AntiNeutron";
  case AntiLambda:
    return (char*)"AntiLambda";
  case AntiSigmaMinus:
    return (char*)"AntiSigma-";
  case AntiSigma0:
    return (char*)"AntiSigma0";
  case AntiSigmaPlus:
    return (char*)"AntiSigma+";
  case AntiXi0:
    return (char*)"AntiXi0";
  case AntiXiPlus:
    return (char*)"AntiXi+";
  case AntiOmegaPlus:
    return (char*)"AntiOmega+";
  case Geantino:
    return (char*)"Geantino";
  case Rho0:
    return (char*)"Rho0";
  case RhoPlus:
    return (char*)"Rho+";
  case RhoMinus:
    return (char*)"Rho-";
  case omega:
    return (char*)"omega";
  case EtaPrime:
    return (char*)"EtaPrime";
  case phiMeson:
    return (char*)"phiMeson";
  case a0_980:
    return (char*)"a0(980)";
  case f0_980:
    return (char*)"f0(980)";
  case KStar_892_0:
    return (char*)"K*(892)0";
  case KStar_892_Plus:
    return (char*)"K*(892)+";
  case KStar_892_Minus:
    return (char*)"K*(892)-";
  case AntiKStar_892_0:
    return (char*)"antiK*(892)0";
  case K1_1400_Plus:
    return (char*)"K1(1400)+";
  case K1_1400_Minus:
    return (char*)"K1(1400)-";
  case b1_1235_Plus:
    return (char*)"b1(1235)+";
  case Sigma_1385_Minus:
    return (char*)"Sigma(1385)-";
  case Sigma_1385_0:
    return (char*)"Sigma(1385)0";
  case Sigma_1385_Plus:
    return (char*)"Sigma(1385)+";
  case Deuteron:
    return (char*)"Deuteron";
  case Helium:
    return (char*)"Helium";
  case Triton:
    return (char*)"Triton";
  case Pb208:
    return (char*)"Pb208";
  default:
    return (char*)"Unknown";
  }
}

inline static unsigned short int IsFixedMass(Particle_t p)
{
  switch (p)
  {
  case Gamma:		return 1;
  case Positron:	return 1;
  case Electron:	return 1;
  case Neutrino:	return 1;
  case MuonPlus:	return 1;
  case MuonMinus:	return 1;
  case Pi0:      	return 1;
  case PiPlus:		return 1;
  case PiMinus:		return 1;
  case KShort:		return 1;
  case KLong:		return 1;
  case KPlus:		return 1;
  case KMinus:		return 1;
  case Neutron:		return 1;
  case Proton:		return 1;
  case AntiProton:	return 1;
  case Eta:			return 1;
  case Lambda:		return 1;
  case SigmaPlus:	return 1;
  case Sigma0:		return 1;
  case SigmaMinus:	return 1;
  case Xi0:			return 1;
  case XiMinus:		return 1;
  case OmegaMinus:	return 1;
  case AntiNeutron:	return 1;
  case AntiLambda:	return 1;
  case AntiSigmaMinus:	return 1;
  case AntiSigma0:	return 1;
  case AntiSigmaPlus:	return 1;
  case AntiXi0:		return 1;
  case AntiXiPlus:	return 1;
  case AntiOmegaPlus:	return 1;
  case Geantino:	return 1;
  case EtaPrime:	return 1;
  case Deuteron:	return 1;
  case Helium:		return 1;
  case Triton:	return 1;
  case Pb208:	return 1;
  default: return 0;
  }
}

inline static unsigned short int IsResonance(Particle_t p)
{
	if(IsFixedMass(p) == 1)
		return 0;
	if(p == Unknown)
		return 0;
	if(p == phiMeson)
		return 0;
	if(p == omega)
		return 0;
	return 1;
}

inline static unsigned short int IsDetachedVertex(Particle_t p)
{
  switch (p)
  {
  case MuonPlus:	return 1;
  case MuonMinus:	return 1;
  case PiPlus:		return 1;
  case PiMinus:		return 1;
  case KShort:		return 1;
  case KLong:		return 1;
  case KPlus:		return 1;
  case KMinus:		return 1;
  case Neutron:		return 1;
  case Lambda:		return 1;
  case SigmaPlus:	return 1;
  case SigmaMinus:	return 1;
  case Xi0:			return 1;
  case XiMinus:		return 1;
  case OmegaMinus:	return 1;
  case AntiNeutron:	return 1;
  case AntiLambda:	return 1;
  case AntiSigmaMinus:	return 1;
  case AntiSigmaPlus:	return 1;
  case AntiXi0:		return 1;
  case AntiXiPlus:	return 1;
  case AntiOmegaPlus:	return 1;
  case Deuteron:	return 1;
  case Helium:		return 1;
  case Triton:	return 1;
  default: return 0;
  }
}

inline static char* ParticleName_ROOT(Particle_t p)
{
  switch (p) {
  case Unknown:
    return (char*)"#it{X}";
  case Gamma:
    return (char*)"#it{#gamma}";
  case Positron:
    return (char*)"#it{e}^{+}";
  case Electron:
    return (char*)"#it{e}^{-}";
  case Neutrino:
    return (char*)"#it{#nu}";
  case MuonPlus:
    return (char*)"#it{#mu}^{+}";
  case MuonMinus:
    return (char*)"#it{#mu}^{-}";
  case Pi0:
    return (char*)"#it{#pi}^{0}";
  case PiPlus:
    return (char*)"#it{#pi}^{+}";
  case PiMinus:
    return (char*)"#it{#pi}^{-}";
  case KLong:
    return (char*)"#it{K}^{0}_{L}";
  case KPlus:
    return (char*)"#it{K}^{+}";
  case KMinus:
    return (char*)"#it{K}^{-}";
  case Neutron:
    return (char*)"#it{n}";
  case Proton:
    return (char*)"#it{p}";
  case AntiProton:
    return (char*)"#it{#bar{p}}";
  case KShort:
    return (char*)"#it{K}^{0}_{S}";
  case Eta:
    return (char*)"#it{#eta}";
  case Lambda:
    return (char*)"#it{#Lambda}";
  case SigmaPlus:
    return (char*)"#it{#Sigma}^{+}";
  case Sigma0:
    return (char*)"#it{#Sigma}^{0}";
  case SigmaMinus:
    return (char*)"#it{#Sigma}^{-}";
  case Xi0:
    return (char*)"#it{#Xi}^{0}";
  case XiMinus:
    return (char*)"#it{#Xi}^{-}";
  case OmegaMinus:
    return (char*)"#it{#Omega}^{-}";
  case AntiNeutron:
    return (char*)"#it{#bar^{n}}";
  case AntiLambda:
    return (char*)"#it{#bar^{#Lambda}}";
  case AntiSigmaMinus:
    return (char*)"#it{#bar{#Sigma}}^{-}";
  case AntiSigma0:
    return (char*)"#it{#bar{#Sigma}}^{0}";
  case AntiSigmaPlus:
    return (char*)"#it{#bar{#Sigma}}^{+}";
  case AntiXi0:
    return (char*)"#it{#bar{#Xi}}^{0}";
  case AntiXiPlus:
    return (char*)"#it{#bar{#Xi}}^{+}";
  case AntiOmegaPlus:
    return (char*)"#it{#bar{#Omega}}^{+}";
  case Geantino:
    return (char*)"geantino";
  case Rho0:
    return (char*)"#it{#rho}^{0}";
  case RhoPlus:
    return (char*)"#it{#rho}^{+}";
  case RhoMinus:
    return (char*)"#it{#rho}^{-}";
  case omega:
    return (char*)"#it{#omega}";
  case EtaPrime:
    return (char*)"#it{#eta'}";
  case phiMeson:
    return (char*)"#it{#phi}";
  case a0_980:
    return (char*)"#it{a}_{0}(980)";
  case f0_980:
    return (char*)"#it{f}_{0}(980)";
  case KStar_892_0:
    return (char*)"#it{K}*(892)^{0}";
  case KStar_892_Plus:
    return (char*)"#it{K}*(892)^{+}";
  case KStar_892_Minus:
    return (char*)"#it{K}*(892)^{-}";
  case AntiKStar_892_0:
    return (char*)"#it{#bar{K*}}(892)^{0}";
  case K1_1400_Plus:
    return (char*)"#it{K}_{1}(1400)^{+}";
  case K1_1400_Minus:
    return (char*)"#it{K}_{1}(1400)^{-}";
  case b1_1235_Plus:
    return (char*)"#it{b}_{1}(1235)^{+}";
  case Deuteron:
    return (char*)"#it{D}";
  case Helium:
    return (char*)"#it{He}";
  case Triton:
    return (char*)"#it{T}";
  case Pb208:
    return (char*)"#it{Pb^{208}}";
  case Sigma_1385_Minus:
    return (char*)"#it{#Sigma}(1385)^{-}";
  case Sigma_1385_0:
    return (char*)"#it{#Sigma}(1385)^{0}";
  case Sigma_1385_Plus:
    return (char*)"#it{#Sigma}(1385)^{+}";
  default:
    return (char*)"X";
  }
}

inline static double ParticleMass(Particle_t p)
{
  switch (p) {
  case Unknown:		return HUGE_VAL;
  case Gamma:		return 0;
  case Positron:	return 0.000510998928;
  case Electron:	return 0.000510998928;
  case Neutrino:	return 0;
  case MuonPlus:	return 0.1056583715;
  case MuonMinus:	return 0.1056583715;
  case Pi0:		return 0.1349766;
  case PiPlus:		return 0.13957018;
  case PiMinus:		return 0.13957018;
  case KShort:		return 0.497614;
  case KLong:		return 0.497614;
  case KPlus:		return 0.493677;
  case KMinus:		return 0.493677;
  case Neutron:		return 0.939565379;
  case Proton:		return 0.938272046;
  case AntiProton:	return 0.938272046;
  case Eta:		return 0.547862;
  case Lambda:		return 1.115683;
  case SigmaPlus:	return 1.18937;
  case Sigma0:		return 1.192642;
  case SigmaMinus:	return 1.197449;
  case Xi0:		return 1.31486;
  case XiMinus:		return 1.32171;
  case OmegaMinus:	return 1.67245;
  case AntiNeutron:	return 0.939565379;
  case AntiLambda:	return 1.115683;
  case AntiSigmaMinus:	return 1.18937;
  case AntiSigma0:	return 1.192642;
  case AntiSigmaPlus:	return 1.197449;
  case AntiXi0:		return 1.31486;
  case AntiXiPlus:	return 1.32171;
  case AntiOmegaPlus:	return 1.67245;
  case Geantino:	return 0.0;
  case Rho0:		return 0.7690;  // neutral only, photoproduced and other reactions. e+ e- gives 775.26
  case RhoPlus:		return 0.7665;  // charged only, hadroproduced. tau decays and e+ e- gives 775.11
  case RhoMinus:	return 0.7665;
  case omega:		return 0.78265;
  case EtaPrime:	return 0.95778;
  case phiMeson:	return 1.019455;
  case a0_980:		return 0.980;
  case f0_980:		return 0.990;
  case KStar_892_0: return 0.89581;     // neutral only
  case KStar_892_Plus: return 0.89166;  // charged only, hadroproduced
  case KStar_892_Minus: return 0.89166; // charged only, hadroproduced
  case AntiKStar_892_0: return 0.89581; // neutral only
  case K1_1400_Plus: return 1.403;
  case K1_1400_Minus: return 1.403;
  case b1_1235_Plus: return 1.2295;
  case Deuteron:	return 1.875612859;     // from NIST
  case Helium:		return 3.727379238;     // from NIST 6.64465675 x 10-27 kg
  case Triton:	        return 2.808921004;     // from NIST 5.00735630 x 10^-27 kg
  case Pb208:	        return 193.72899;       // NIST gives 207.976627 AMU
  case Sigma_1385_Minus:	return 1.3872;
  case Sigma_1385_0:		return 1.3837;
  case Sigma_1385_Plus:	return 1.38280;
  default:
    fprintf(stderr,"ParticleMass: Error: Unknown particle type %d,",p);
    fprintf(stderr," returning HUGE_VAL...\n");
    return HUGE_VAL;
  }
}

inline static int ParticleCharge(Particle_t p)
{
  switch (p) {
  case Unknown:		return  0;
  case Gamma:		return  0;
  case Positron:	return +1;
  case Electron:	return -1;
  case Neutrino:	return  0;
  case MuonPlus:	return +1;
  case MuonMinus:	return -1;
  case Pi0:		return  0;
  case PiPlus:		return +1;
  case PiMinus:		return -1;
  case KShort:		return  0;
  case KLong:		return  0;
  case KPlus:		return +1;
  case KMinus:		return -1;
  case Neutron:		return  0;
  case Proton:		return +1;
  case AntiProton:	return -1;
  case Eta:		return  0;
  case Lambda:		return  0;
  case SigmaPlus:	return +1;
  case Sigma0:		return  0;
  case SigmaMinus:	return -1;
  case Xi0:		return  0;
  case XiMinus:		return -1;
  case OmegaMinus:	return -1;
  case AntiNeutron:	return  0;
  case AntiLambda:	return  0;
  case AntiSigmaMinus:	return -1;
  case AntiSigma0:	return  0;
  case AntiSigmaPlus:	return +1;
  case AntiXi0:		return  0;
  case AntiXiPlus:	return +1;
  case AntiOmegaPlus:	return +1;
  case Geantino:	return  0;
  case Rho0:		return  0;
  case RhoPlus:		return +1;
  case RhoMinus:	return -1;
  case omega:		return  0;
  case EtaPrime:	return  0;
  case phiMeson:	return  0;
  case a0_980:		return  0;
  case f0_980:		return  0;
  case KStar_892_0: return  0;
  case KStar_892_Plus: return  1;
  case KStar_892_Minus: return -1;
  case AntiKStar_892_0: return  0;
  case K1_1400_Plus: return  1;
  case K1_1400_Minus: return -1;
  case b1_1235_Plus: return 1;
  case Deuteron:	return 1;
  case Helium:		return 2;
  case Triton:	return 1;
  case Pb208:	return 82;
  case Sigma_1385_Minus:	return -1;
  case Sigma_1385_0:		return 0;
  case Sigma_1385_Plus:	return 1;
  default:
    fprintf(stderr,"ParticleCharge: Error: Unknown particle type %d,",p);
    fprintf(stderr," returning 0...\n");
    return 0;
  }
}

inline static int PDGtype(Particle_t p)
{
  switch (p) {
  case Unknown:		return  0;
  case Gamma:		return  22;
  case Positron:	return -11;
  case Electron:	return  11;
  case Neutrino:	return  121416;
  case MuonPlus:	return -13;
  case MuonMinus:	return  13;
  case Pi0:		return  111;
  case PiPlus:		return  211;
  case PiMinus:		return -211;
  case KShort:		return  310;
  case KLong:		return  130;
  case KPlus:		return  321;
  case KMinus:		return -321;
  case Neutron:		return  2112;
  case Proton:		return  2212;
  case AntiProton:	return -2212;
  case Eta:		return  221;
  case Lambda:		return  3122;
  case SigmaPlus:	return  3222;
  case Sigma0:		return  3212;
  case SigmaMinus:	return  3112;
  case Xi0:		return  3322;
  case XiMinus:		return  3312;
  case OmegaMinus:	return  3332;
  case AntiNeutron:	return -2112;
  case AntiLambda:	return -3122;
  case AntiSigmaMinus:	return -3112;
  case AntiSigma0:	return -3212;
  case AntiSigmaPlus:	return -3222;
  case AntiXi0:		return -3322;
  case AntiXiPlus:	return -3312;
  case AntiOmegaPlus:	return -3332;
  case Geantino:	return  0;
  case Rho0:		return  113;
  case RhoPlus:		return  213;
  case RhoMinus:	return -213;
  case omega:		return  223;
  case EtaPrime:	return  331;
  case phiMeson:	return  333;
  case a0_980:		return  9000110;
  case f0_980:		return  9010221;
  case KStar_892_0: return  313;
  case AntiKStar_892_0: return  -313;
  case KStar_892_Plus: return  323;
  case KStar_892_Minus: return -323;
  case K1_1400_Plus: return  20323;
  case K1_1400_Minus: return  -20323;
  case b1_1235_Plus: return  10213;
  case Deuteron:		return  45;
  case Helium:		return  47;
  case Triton:	return  49;
  case Sigma_1385_Minus:	return 3114;
  case Sigma_1385_0:		return 3214;
  case Sigma_1385_Plus:	return 3224;
  case Pb208: return 1000822080; // see note 14 in PDG (pg. 416 of 2012 full listing)
  default:		return  0;
  }
}

inline static Particle_t PDGtoPType(int locPDG_PID)
{
  switch (locPDG_PID) {
  case 0:				return Unknown;
  case 22:			return Gamma;
  case -11:			return Positron;
  case 11:			return Electron;
  case 121416:		return Neutrino;
  case -13:			return MuonPlus;
  case 13:			return MuonMinus;
  case 111:			return Pi0;
  case 211:			return PiPlus;
  case -211:			return PiMinus;
  case 310:			return KShort;
  case 130:			return KLong;
  case 321:			return KPlus;
  case -321:			return KMinus;
  case 2112:			return Neutron;
  case 2212:			return Proton;
  case -2212:		return AntiProton;
  case 221:			return Eta;
  case 3122:			return Lambda;
  case 3222:			return SigmaPlus;
  case 3212:			return Sigma0;
  case 3112:			return SigmaMinus;
  case 3322:			return Xi0;
  case 3312:			return XiMinus;
  case 3332:			return OmegaMinus;
  case -2112:		return AntiNeutron;
  case -3122:		return AntiLambda;
  case -3112:		return AntiSigmaMinus;
  case -3212:		return AntiSigma0;
  case -3222:		return AntiSigmaPlus;
  case -3322:		return AntiXi0;
  case -3312:		return AntiXiPlus;
  case -3332:		return AntiOmegaPlus;
  case 113:			return Rho0;
  case 213:			return RhoPlus;
  case -213:			return RhoMinus;
  case 223:			return omega;
  case 331:			return EtaPrime;
  case 333:			return phiMeson;
  case 9000110:		return a0_980;
  case 9010221:		return f0_980;
  case 313: 			return KStar_892_0;
  case -313:			return AntiKStar_892_0;
  case 323:			return KStar_892_Plus;
  case -323:			return KStar_892_Minus;
  case 20323:		return K1_1400_Plus;
  case -20323:		return K1_1400_Minus;
  case 10213:		return b1_1235_Plus;
  case 45:			return Deuteron;
  case 47:			return Helium;
  case 49:			return Triton;
  case 3114:			return Sigma_1385_Minus;
  case 3214:			return Sigma_1385_0;
  case 3224:			return Sigma_1385_Plus;
  case 1000822080:	return Pb208; // see note 14 in PDG (pg. 416 of 2012 full listing)
  default:			return Unknown;
  }
}

inline static int Is_FinalStateParticle(Particle_t locPID)
{
	switch(locPID)
	{
		case Gamma:       return 1;
		case Positron:    return 1;
		case Electron:    return 1;
		case Neutrino:    return 1;
		case MuonPlus:    return 1;
		case MuonMinus:   return 1;
		case PiPlus:      return 1;
		case PiMinus:     return 1;
		case KLong:       return 1;
		case KPlus:       return 1;
		case KMinus:      return 1;
		case Neutron:     return 1;
		case Proton:      return 1;
		case AntiProton:  return 1;
		case AntiNeutron: return 1;
		case Deuteron:    return 1;
		case Helium:      return 1;
		case Triton:      return 1;
		case Pb208:       return 1;
		default: return 0; //decaying
	}
}

inline static int ParticleMultiplexPower(Particle_t locPID)
{
	switch(locPID)
	{
		//FINAL-STATE PARTICLES (+ pi0) (decimal: 10^power):
		case Gamma:       return 0;
		case Positron:    return 1;
		case Electron:    return 2;
		case Neutrino:    return 3;
		case MuonPlus:    return 4;
		case MuonMinus:   return 5;
		case Pi0:         return 6;
		case PiPlus:      return 7;
		case PiMinus:     return 8;
		case KLong:       return 9;
		case KPlus:       return 10;
		case KMinus:      return 11;
		case Neutron:     return 12;
		case Proton:      return 13;
		case AntiProton:  return 14;
		case AntiNeutron: return 15;

		//DECAYING PARTICLES (- pi0): (binary: 2^power)
		case KShort:           return 0;
		case Eta:              return 1;
		case Lambda:           return 2;
		case SigmaPlus:        return 3;
		case Sigma0:           return 4;
		case SigmaMinus:       return 5;
		case Xi0:              return 6;
		case XiMinus:          return 7;
		case OmegaMinus:       return 8;
		case AntiLambda:       return 9;
		case AntiSigmaMinus:   return 10;
		case AntiSigma0:       return 11;
		case AntiSigmaPlus:    return 12;
		case AntiXi0:          return 13;
		case AntiXiPlus:       return 14;
		case AntiOmegaPlus:    return 15;
		case Deuteron:         return 16;
		case Rho0:             return 17;
		case RhoPlus:          return 18;
		case RhoMinus:         return 19;
		case omega:            return 20;
		case EtaPrime:         return 21;
		case phiMeson:         return 22;
		case a0_980:           return 23;
		case f0_980:           return 24;
		case KStar_892_0:      return 25;
		case KStar_892_Plus:   return 26;
		case KStar_892_Minus:  return 27;
		case AntiKStar_892_0:  return 28;
		case K1_1400_Plus:     return 29;
		case K1_1400_Minus:    return 30;
		case b1_1235_Plus:     return 31;
		case Sigma_1385_Minus: return 32;
		case Sigma_1385_0:     return 33;
		case Sigma_1385_Plus:  return 34;
		default: return -1;
	}
}

#endif