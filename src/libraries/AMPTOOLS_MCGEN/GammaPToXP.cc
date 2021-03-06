/*
 *  GammaPToXP.cc
 *  GlueXTools
 *
 *  Created by Matthew Shepherd on 1/22/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#include "TLorentzVector.h"

#include "AMPTOOLS_MCGEN/GammaPToXP.h"

// FORTRAN routines
extern "C"{
void cobrems_(float* Emax, float* Epeak, float* Dist);
float dntdx_(float* x);
};

// Wrapper function for total
double dNtdx(double x)
{
        float xx = x;
        return (double)dntdx_(&xx);
}

GammaPToXP::GammaPToXP( float massX, float beamMaxE, float beamPeakE, float beamLowE, float beamHighE) : 
m_target( 0, 0, 0, 0.938 ),
m_childMass( 0 ) {

  m_childMass.push_back( massX );
 
  // Initialize coherent brem table
  float Emax =  beamMaxE;
  float Epeak = beamPeakE;
  float Elow = beamLowE;
  float Ehigh = beamHighE;
  float Dist = 76.0;
  cobrems_(&Emax, &Epeak, &Dist);

  // Create histogram
  cobrem_vs_E = new TH1D("cobrem_vs_E", "Coherent Bremstrahlung vs. E_{#gamma}", (int)(100*Emax), 0.0, Emax);
  
  // Fill histogram
  for(int i=1; i<=cobrem_vs_E->GetNbinsX(); i++){
	  double x = cobrem_vs_E->GetBinCenter(i)/Emax;
	  double y = dNtdx(x);
	  if(cobrem_vs_E->GetBinCenter(i) > Elow && cobrem_vs_E->GetBinCenter(i) < Ehigh) 
		  cobrem_vs_E->SetBinContent(i, y);
  }

}

Kinematics* 
GammaPToXP::generate(){

  double beamE = cobrem_vs_E->GetRandom();
  m_beam.SetPxPyPzE(0,0,beamE,beamE);
  TLorentzVector cm = m_beam + m_target;

  Double_t masses[2] = {0.938,m_childMass[0]};
  TGenPhaseSpace phsp;
  phsp.SetDecay(cm,2,masses);

  double phsp_wt_max = phsp.GetWtMax();
  double genWeight;
  do {
     genWeight = phsp.Generate();
  }
  while( random(0., phsp_wt_max) >= genWeight || genWeight != genWeight);
  
  TLorentzVector *recoil = phsp.GetDecay(0);
  TLorentzVector *pX = phsp.GetDecay(1);

  vector< TLorentzVector > allPart;
  allPart.push_back( m_beam );
  allPart.push_back( *recoil );
  allPart.push_back( *pX );
 
  return new Kinematics( allPart, genWeight );
}

double
GammaPToXP::random( double low, double hi ) const {

        return( ( hi - low ) * drand48() + low );
}

