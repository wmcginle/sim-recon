//************************************************************************
// DKalmanFilter.cc
//************************************************************************

#include "DKalmanFilter.h"
#include "DANA/DApplication.h"
#include "HDGEOMETRY/DMagneticFieldMap.h"
#include "HDGEOMETRY/DGeometry.h"
#include <TDecompLU.h>

#include <math.h>

#define qBr2p 0.003  // conversion for converting q*B*r to GeV/c
#define EPS 1.0e-8
#define DEDX_ENDPLATE (-3.95e-3) // Carbon
#define DEDX_AIR (-2.19e-6) // GeV/cm
#define DEDX_LH2 ( -2.856e-4) 
#define DEDX_ROHACELL (-0.4e-3) // low density carbon for now!!
#define BEAM_RADIUS  0.1 

bool DKalmanHit_cmp(DKalmanHit_t *a, DKalmanHit_t *b){
  return a->z>b->z;
}

DKalmanFilter::DKalmanFilter(const DMagneticFieldMap *bfield,
			     const DGeometry *dgeom){
  this->bfield=bfield;
  this->geom=dgeom;
  hits.clear();

  // Get the position of the exit of the CDC endplate from DGeometry
  geom->GetCDCEndplate(endplate_z,endplate_dz,endplate_rmin,endplate_rmax);

  // Start counter material
 

   // Get dimensions of target wall
  vector<double>target_center;
  geom->Get("//posXYZ[@volume='Target']/@X_Y_Z",target_center);
  geom->Get("//tubs[@name='CYLW']/@Rio_Z",targ_wall);
  targ_wall[2]+=target_center[2];
  // Target material
  geom->Get("//tubs[@name='LIH2']/@Rio_Z",target);
  target[2]+=target_center[2];
}

// Initialize the state vector
jerror_t DKalmanFilter::SetSeed(double q,DVector3 pos, DVector3 mom){
  x_=pos(0);
  y_=pos(1);
  z_=pos(2);
  tx_= mom(0)/mom(2);
  ty_= mom(1)/mom(2);
  q_over_p_=q/mom.Mag();
  
  return NOERROR;
}

// Return the momentum at the distance of closest approach to the origin.
void DKalmanFilter::GetMomentum(DVector3 &mom){
  double p=fabs(1./q_over_p_);
  double factor=sqrt(1.+tx_*tx_+ty_*ty_);
  mom.SetXYZ(p*tx_/factor,p*ty_/factor,p/factor);
}

// Return the "vertex" position (position at which track crosses beam line)
void DKalmanFilter::GetPosition(DVector3 &pos){
  pos.SetXYZ(x_,y_,z_);
}



/// Add a hit to the list of hits using Cartesian coordinates
jerror_t DKalmanFilter::AddHit(double x,double y, double z,double covx,
                                double covy, double covxy, double dE){
  DKalmanHit_t *hit = new DKalmanHit_t;
  hit->x = x;
  hit->y = y;
  hit->z = z;
  hit->covx=covx;
  hit->covy=covy;
  hit->covxy=covxy;
  hit->dE=dE;
  
  hits.push_back(hit);

  return NOERROR;
}

// Calculate the derivative of the state vector with respect to z
jerror_t DKalmanFilter::CalcDeriv(double z,DMatrix S, double dEdx, DMatrix &D){
  double x=S(0,0), y=S(1,0),tx=S(2,0),ty=S(3,0),q_over_p=S(4,0);
  double factor=sqrt(1.+tx*tx+ty*ty);
  double mass=0.14; // pion for now
  double E=sqrt(1./q_over_p/q_over_p+mass*mass); 

  //B-field at (x,y,z)
  double Bx,By,Bz;
  bfield->GetField(x,y,z, Bx, By, Bz);

  D(0,0)=tx;
  D(1,0)=ty;
  D(2,0)=qBr2p*q_over_p*factor*(tx*ty*Bx-(1.+tx*tx)*By+ty*Bz);
  D(3,0)=qBr2p*q_over_p*factor*((1.+ty*ty)*Bx-tx*ty*By-tx*Bz);
  D(4,0)=-q_over_p*q_over_p*q_over_p*E*dEdx*factor;

  return NOERROR;
}


// Calculate the derivative of the state vector with respect to z and the 
// Jacobian matrix relating the state vector at z to the state vector at z+dz.
jerror_t DKalmanFilter::CalcDerivAndJacobian(double z,DMatrix S,double dEdx,
					     DMatrix &J,DMatrix &D){
  double x=S(0,0), y=S(1,0),tx=S(2,0),ty=S(3,0),q_over_p=S(4,0);
  double factor=sqrt(1.+tx*tx+ty*ty);
  double mass=0.14; // pion for now
  double E=sqrt(1./q_over_p/q_over_p+mass*mass); 

  //B-field and field gradient at (x,y,z)
  double Bx,By,Bz,dBxdx,dBxdy,dBxdz,dBydx,dBydy,dBydz,dBzdx,dBzdy,dBzdz;
  bfield->GetField(x,y,z, Bx, By, Bz);
  bfield->GetFieldGradient(x,y,z,dBxdx,dBxdy,dBxdz,dBydx,dBydy,dBydz,dBzdx,
			   dBzdy,dBzdz);

  // Derivative of S with respect to z
  D(0,0)=tx;
  D(1,0)=ty;
  D(2,0)=qBr2p*q_over_p*factor*(tx*ty*Bx-(1.+tx*tx)*By+ty*Bz);
  D(3,0)=qBr2p*q_over_p*factor*((1.+ty*ty)*Bx-tx*ty*By-tx*Bz);
  D(4,0)=-q_over_p*q_over_p*q_over_p*E*dEdx*factor;

  // Jacobian
  J(0,2)=J(1,3)=1.;
  J(2,4)=qBr2p*factor*(Bx*tx*ty-By*(1.+tx*tx)+Bz*ty);
  J(3,4)=qBr2p*factor*(Bx*(1.+ty*ty)-By*tx*ty-Bz*tx);
  J(2,2)=qBr2p*q_over_p*(Bx*ty*(1.+2.*tx*tx+ty*ty)-By*tx*(3.+3.*tx*tx+2.*ty*ty)
			 +Bz*tx*ty)/factor;
  J(2,0)= qBr2p*q_over_p*factor*(ty*dBzdx+tx*ty*dBxdx-(1.+tx*tx)*dBydx);
  J(3,3)=qBr2p*q_over_p*(Bx*ty*(3.+2.*tx*tx+3.*ty*ty)-By*tx*(1.+tx*tx+2.*ty*ty)
			 -Bz*tx*ty)/factor;
  J(3,1)= qBr2p*q_over_p*factor*((1.+ty*ty)*dBxdy-tx*ty*dBydy-tx*dBzdy);
  J(2,3)=qBr2p*q_over_p*((Bx*tx+Bz)*(1.+tx*tx+2.*ty*ty)-By*ty*(1.+tx*tx))
    /factor;
  J(2,1)= qBr2p*q_over_p*factor*(tx*dBzdy+tx*ty*dBxdy-(1.+tx*tx)*dBydy);
  J(3,2)=-qBr2p*q_over_p*((By*ty+Bz)*(1.+2.*tx*tx+ty*ty)-Bx*tx*(1.+ty*ty))
    /factor;
  J(3,0)=qBr2p*q_over_p*factor*((1.+ty*ty)*dBxdx-tx*ty*dBydx-tx*dBzdx);
  J(4,2)=D(4,0)*tx/factor/factor;
  J(4,3)=D(4,0)*ty/factor/factor;
  J(4,4)=-dEdx*factor/E*(2.+3.*mass*mass*q_over_p*q_over_p);
  
  return NOERROR;
}


// Step the state vector through the field from oldz to newz.
// Uses the 4th-order Runga-Kutte algorithm.
double DKalmanFilter::Step(double oldz,double newz, double dEdx,DMatrix &S){
  double delta_z=newz-oldz;
  DMatrix D1(5,1),D2(5,1),D3(5,1),D4(5,1);

  double s=sqrt(1.+S(2,0)*S(2,0)+S(3,0)*S(3,0))*delta_z;
  CalcDeriv(oldz,S,dEdx,D1);
  CalcDeriv(oldz+delta_z/2.,S+0.5*delta_z*D1,dEdx,D2);
  CalcDeriv(oldz+delta_z/2.,S+0.5*delta_z*D2,dEdx,D3);
  CalcDeriv(oldz+delta_z,S+delta_z*D3,dEdx,D4);
	
  S+=delta_z*((1./6.)*D1+(1./3.)*D2+(1./3.)*D3+(1./6.)*D4);

  return s;
}

// Step the state vector through the magnetic field and compute the Jacobian
// matrix.  Uses the 4th-order Runga-Kutte algorithm.
double DKalmanFilter::StepJacobian(double oldz,double newz,DMatrix &S,
				     double dEdx,DMatrix &J){
   // Initialize the Jacobian matrix
  J.Zero();
  for (int i=0;i<5;i++) J(i,i)=1.;
  // Matrices for intermediate steps
  DMatrix J1(5,5),J2(5,5),J3(5,5),J4(5,5);  
  DMatrix D1(5,1),D2(5,1),D3(5,1),D4(5,1);
  
  double delta_z=newz-oldz;
  double s=sqrt(1.+S(2,0)*S(2,0)+S(3,0)*S(3,0))*delta_z;
  CalcDerivAndJacobian(oldz,S,dEdx,J1,D1);
  CalcDerivAndJacobian(oldz+delta_z/2.,S+0.5*delta_z*D1,dEdx,J2,D2);
  CalcDerivAndJacobian(oldz+delta_z/2.,S+0.5*delta_z*D2,dEdx,J3,D3);
  CalcDerivAndJacobian(oldz+delta_z,S+delta_z*D3,dEdx,J4,D4);
	
  S+=delta_z*((1./6.)*D1+(1./3.)*D2+(1./3.)*D3+(1./6.)*D4);
  J+=delta_z*((1./6.)*J1+(1./3.)*J2+(1./3.)*J3+(1./6.)*J4);

  return s;
}

// Compute contributions to the covariance matrix due to multiple scattering
jerror_t DKalmanFilter::GetProcessNoise(double mass_hyp,double ds,
					double X0,DMatrix S,DMatrix &Q){
  DMatrix Q1(5,5);
  double tx=S(2,0),ty=S(3,0),one_over_p_sq=S(4,0)*S(4,0);
  double my_ds=fabs(ds);

  Q1(2,2)=(1.+tx*tx)*(1.+tx*tx+ty*ty);
  Q1(3,3)=(1.+ty*ty)*(1.+tx*tx+ty*ty);
  Q1(2,3)=Q1(3,2)=tx*ty*(1.+tx*tx+ty*ty);

  double sig2_ms= 0.0136*0.0136*(1.+one_over_p_sq*mass_hyp*mass_hyp)
    *one_over_p_sq*my_ds/X0*(1.+0.038*log(my_ds/X0))*(1.+0.038*log(my_ds/X0));

  Q=sig2_ms*Q1;

  return NOERROR;
}


// Routine that performs the main loop of the Kalman engine
jerror_t DKalmanFilter::KalmanLoop(double mass_hyp){
  DMatrix M(2,1);  // measurement vector
  DMatrix M_pred(2,1); // prediction for hit position
  DMatrix H(2,5);  // Track projection matrix
  DMatrix H_T(5,2); // Transpose of track projection matrix
  DMatrix J(5,5);  // State vector Jacobian matrix
  DMatrix J_T(5,5); // transpose of this matrix
  DMatrix Q(5,5);  // Process noise covariance matrix
  DMatrix K(5,2);  // Kalman gain matrix
  DMatrix V(2,2);  // Measurement covariance matrix
  DMatrix V1(2,2);  // same, with the contribution from C
  DMatrix X(2,1);  // Position on track
  DMatrix R(2,1);  // Filtered residual
  DMatrix R_T(1,2);  // ...and its transpose
  DMatrix RC(2,2);  // Covariance of filtered residual
  DMatrix InvRC(2,2); // and its inverse
  DMatrix S(5,1),S0(5,1); //State vector
  DMatrix C0(5,5),C(5,5);   // Covariance matrix for state vector
  DMatrix InvV(2,2); // Inverse of error matrix

  // path length increment
  double ds=0;
  // Path length in active volume
  path_length=0;

  chisq=0;
  // Initialize the state vector 
  S(0,0)=S0(0,0)=x_;
  S(1,0)=S0(1,0)=y_;
  S(2,0)=S0(2,0)=tx_;
  S(3,0)=S0(3,0)=ty_;
  S(4,0)=S0(4,0)=q_over_p_;

  // Initialize the covariance matrix
  for (unsigned int i=0;i<5;i++) C(i,i)=C0(i,i)=1.0;

  // Order the hits from the most downstream to the most upstream
  sort(hits.begin(),hits.end(),DKalmanHit_cmp);

  // Track projection matrix
  H(0,0)=H(1,1)=1.;
  H_T=DMatrix(DMatrix::kTransposed,H);

  // Loop over hits, updating the state vector at each step 
  double endz=hits[0]->z,newz,oldz; // we increment in z
  // Start at the most downstream measurement
  if (z_!=endz) z_=endz;
  for (unsigned int k=0;k<hits.size();k++){      
    endz=hits[k]->z;

    // The next measurement 
    M(0,0)=hits[k]->x;
    M(1,0)=hits[k]->y; 
    
    // ... and its covariance matrix  
    V(0,0)=hits[k]->covx;
    V(1,0)=V(0,1)=hits[k]->covxy;
    V(1,1)=hits[k]->covy;
    
    // Propagate state vector and covariance matrices to next measurement   
    int num_inc=int((endz-z_)/0.5);
    oldz=z_;
    newz=oldz;
    for (int j=0;j<num_inc;j++){
      newz=oldz-0.5;
      // Step the improved state vector through the field
      ds=StepJacobian(oldz,newz,S,DEDX_AIR,J);
      // Get the covariance matrix due to the multiple scattering
      GetProcessNoise(mass_hyp,ds,30420.,S,Q); // use air for now
      // Rotate the covariance matrix and add the multiple scattering elements
      J_T=DMatrix(DMatrix::kTransposed,J);
      C=J*(C*J_T)+Q; 

      // Step the seed state vector through the field and get the Jacobian 
      ds=StepJacobian(oldz,newz,S0,DEDX_AIR,J);
      // Get the covariance matrix due to the multiple scattering
      GetProcessNoise(mass_hyp,ds,30420.,S0,Q); // use air for now
      // Rotate the covariance matrix and add the multiple scattering elements
      J_T=DMatrix(DMatrix::kTransposed,J);
      C0=J*(C0*J_T)+Q; 

      oldz=newz;			  
    }
    if (fabs(endz-oldz)>EPS){
      ds=StepJacobian(oldz,endz,S,DEDX_AIR,J);
      J_T=DMatrix(DMatrix::kTransposed,J);  
      GetProcessNoise(mass_hyp,ds,30420.,S,Q); // use air for now
      C=J*(C*J_T)+Q;

      ds=StepJacobian(oldz,endz,S0,DEDX_AIR,J);
      J_T=DMatrix(DMatrix::kTransposed,J);  
      GetProcessNoise(mass_hyp,ds,30420.,S0,Q); // use air for now
      C0=J*(C0*J_T)+Q;
    }

    // State vector S is a perturbation about the seed S0
    S=S0+J*(S-S0);
    C=C0+J*((C-C0)*J_T);

    // Updated error matrix
    V1=V+H*(C*H_T);

    // Calculate the inverse of V
    double det=V1(0,0)*V1(1,1)-V1(0,1)*V1(1,0);
    if (det!=0){
      InvV(0,0)=V1(1,1)/det;
      InvV(1,0)=-V1(1,0)/det;
      InvV(0,1)=-V1(0,1)/det;
      InvV(1,1)=V1(0,0)/det;
    }
    else{
      _DBG_ << "Kalman filter:  Singular matrix..." << endl;
      return UNRECOVERABLE_ERROR;
    }

    // Compute Kalman gain matrix
    K=C*(H_T*InvV);

    // Update the state vector 
    M_pred(0,0)=S0(0,0);
    M_pred(1,0)=S0(1,0);
    M_pred = M_pred + H*(S-S0);
    S=S+K*(M-M_pred); 

    // Path length in active volume
    path_length+=1.0*sqrt(1.+S(2,0)*S(2,0)+S(3,0)*S(3,0));

    // Update state vector covariance matrix
    C=C-K*(H*C);
    
    // Residuals
    R(0,0)=M(0,0)-S(0,0);
    R(1,0)=M(1,0)-S(1,0);
    R_T=DMatrix(DMatrix::kTransposed,R);
    RC=V-H*(C*H_T);

    // Calculate the inverse of RC
    det=RC(0,0)*RC(1,1)-RC(0,1)*RC(1,0);
    if (det!=0){
      InvRC(0,0)=RC(1,1)/det;
      InvRC(1,0)=-RC(1,0)/det;
      InvRC(0,1)=-RC(0,1)/det;
      InvRC(1,1)=RC(0,0)/det;
    }
    else{
      _DBG_ << "Kalman filter:  Singular matrix RC..." << endl;
      return UNRECOVERABLE_ERROR;
    }

    // Update chi2 for this segment
    chisq+=(R_T*(InvRC*R))(0,0);

    // increment z position
    z_=endz;
  }

  // Propagate track to entrance to CDC endplate
  int num_inc=(int)(endz-endplate_z);
  oldz=endz;
  endz=endplate_z;
  for (int i=0;i<num_inc;i++){
    newz=oldz-0.5;
    StepJacobian(oldz,newz,S,DEDX_AIR,J);  
    J_T=DMatrix(DMatrix::kTransposed,J);  
    C=J*(C*J_T);
    oldz=newz;
  }
  if (oldz!=endz){
    StepJacobian(oldz,endz,S,DEDX_AIR,J);  
    J_T=DMatrix(DMatrix::kTransposed,J);  
    C=J*(C*J_T);    
  }

  // Next treat the CDC endplate
  oldz=endz;
  double r=sqrt(S(0,0)*S(0,0)+S(1,0)*S(1,0)); // current radius
  if (r>endplate_rmin){
    for (int i=0;i<4;i++){
       newz=oldz-endplate_dz/4.;
       StepJacobian(oldz,newz,S,DEDX_ENDPLATE,J);  
       J_T=DMatrix(DMatrix::kTransposed,J);  
       C=J*(C*J_T);
       oldz=newz;
    }
    endz=newz;
  }

  // Propagate track to point of distance of closest approach to origin
  num_inc=int((endz-0.)/0.5);
  DMatrix SBest(5,1);
  SBest=S;
  oldz=endz;
  r=sqrt(S(0,0)*S(0,0)+S(1,0)*S(1,0));
  double rmin=r;
  double dedx=DEDX_AIR;
  while (oldz>0. && rmin>BEAM_RADIUS){
    if (r<rmin){
      rmin=r;
      SBest=S;
      z_=oldz;
    }
    if (oldz>targ_wall[2])
      newz=oldz-0.5;
    else
      newz=oldz-0.1;
    if (r<targ_wall[1] && r>targ_wall[0] && newz<=targ_wall[2])
      dedx=DEDX_ROHACELL;
    else if (r<target[1] && newz<=target[2]) 
      dedx=DEDX_LH2;
    StepJacobian(oldz,newz,S,dedx,J);  
    J_T=DMatrix(DMatrix::kTransposed,J);  
    C=J*(C*J_T);
    r=sqrt(S(0,0)*S(0,0)+S(1,0)*S(1,0));   
    oldz=newz;
  }
  
  // Fitted track parameters at vertex
  x_=SBest(0,0);
  y_=SBest(1,0);
  tx_=SBest(2,0);
  ty_=SBest(3,0);
  q_over_p_=SBest(4,0);

  return NOERROR;
}
