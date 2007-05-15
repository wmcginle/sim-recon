
#include <iostream>
#include <iomanip>
using namespace std;

#include "DMagneticFieldMap.h"
#include "DMagneticFieldStepper.h"

#define qBr2p 0.003  // conversion for converting q*B*r to GeV/c

#define MAX_SWIM_DIST 2000.0 // Maximum distance to swim a track in cm

//-----------------------
// DMagneticFieldStepper
//-----------------------
DMagneticFieldStepper::DMagneticFieldStepper(const DMagneticFieldMap *bfield, double q)
{
	this->bfield = bfield;
	this->q = q;
	start_pos = pos = DVector3(0.0,0.0,0.0);
	start_mom = mom = DVector3(0.0,0.0,1.0);
	last_stepsize = stepsize = 1.0; // in cm
	CalcDirs();
}

//-----------------------
// DMagneticFieldStepper
//-----------------------
DMagneticFieldStepper::DMagneticFieldStepper(const DMagneticFieldMap *bfield, double q, const DVector3 *x, const DVector3 *p)
{
	this->bfield = bfield;
	this->q = q;
	start_pos = pos = *x;
	start_mom = mom = *p;
	last_stepsize = stepsize = 1.0; // in cm
	CalcDirs();
}

//-----------------------
// ~DMagneticFieldStepper
//-----------------------
DMagneticFieldStepper::~DMagneticFieldStepper()
{

}
	
//-----------------------
// SetStartingParams
//-----------------------
jerror_t DMagneticFieldStepper::SetStartingParams(double q, const DVector3 *x, const DVector3 *p)
{
	this->q = q;
	start_pos = pos = *x;
	start_mom = mom = *p;
	this->last_stepsize = this->stepsize;
	CalcDirs();

	return NOERROR;
}

//-----------------------
// SetMagneticFieldMap
//-----------------------
jerror_t DMagneticFieldStepper::SetMagneticFieldMap(const DMagneticFieldMap *bfield)
{
	this->bfield = bfield;

	return NOERROR;
}

//-----------------------
// SetStepSize
//-----------------------
jerror_t DMagneticFieldStepper::SetStepSize(double step)
{
	this->stepsize = step;
	this->last_stepsize = step;

	return NOERROR;
}

//-----------------------
// CalcDirs
//-----------------------
void DMagneticFieldStepper::CalcDirs(void)
{
	/// Calculate the directions of the "natural coordinates"
	/// (aka reference trajectory coordinate system) in the
	/// lab frame using the current momentum and magnetic field
	/// at the current position. The results are left in the
	/// private member fields, copies of which may be obtained
	/// by a subsequent call to the GetDirs(...) and GetRo()
	/// methods.

	// Get B-field
	double Bx,By,Bz;
	bfield->GetField(pos.x(), pos.y(), pos.z(), Bx, By, Bz);
	B.SetXYZ(Bx, By, Bz);

	// If the B-field is zero, then default to lab system
	double B2 = B.Mag2();
	if(B2==0.0){
		xdir.SetXYZ(1.0,0.0,0.0);
		ydir.SetXYZ(0.0,1.0,0.0);
		zdir.SetXYZ(0.0,0.0,1.0);
		cos_theta = 1.0;
		sin_theta = 0.0;
		Rp = Ro = 1.0E20; // This shouldn't really be used ever
		return;
	}

	// cross product of p and B (natural x-direction)
	xdir = mom.Cross(B);
	if(xdir.Mag2()<1.0E-6)xdir = mom.Orthogonal();
	xdir.SetMag(1.0);
	
	// cross product of B and pxB (natural y-direction)
	ydir = B.Cross(xdir);
	ydir.SetMag(1.0);
	
	// B-field is natural z-direction
	zdir = B;
	zdir.SetMag(1.0);

	// cosine of angle between p and B
	double theta = B.Angle(mom);
	cos_theta = cos(theta);
	sin_theta = sin(theta);

	// Calculate Rp and Ro for this momentum and position
	Rp = mom.Mag()/(q*sqrt(B2)*qBr2p); // qBr2p converts to GeV/c/cm so Rp will be in cm
	Ro = Rp*sin_theta;
}

#if 1
//extern "C" {
int grkuta_(double *CHARGE, double *STEP, double *VECT, double *VOUT,const DMagneticFieldMap *bfield);
//}

//-----------------------
// Step
//-----------------------
double DMagneticFieldStepper::Step(DVector3 *newpos, double stepsize)
{
	double VECT[7], VOUT[7];
	VECT[0] = pos.x();
	VECT[1] = pos.y();
	VECT[2] = pos.z();
	VECT[6] = mom.Mag();
	VECT[3] = mom.x()/VECT[6];
	VECT[4] = mom.y()/VECT[6];
	VECT[5] = mom.z()/VECT[6];
	if(stepsize==0.0)stepsize = this->stepsize;
	double &STEP = stepsize;
	double &CHARGE = q;

	// Call GEANT3 Runge Kutta step routine (see grkuta.c)
	grkuta_(&CHARGE, &STEP, VECT, VOUT, bfield);

	pos.SetXYZ(VOUT[0], VOUT[1], VOUT[2]);
	mom.SetXYZ(VOUT[3], VOUT[4], VOUT[5]);
	mom.SetMag(VOUT[6]);

	CalcDirs();

	if(newpos)*newpos = pos;

	return STEP;
}

#else

//-----------------------
// Step
//-----------------------
double DMagneticFieldStepper::Step(DVector3 *newpos, double stepsize)
{
	/// Advance the track one step. Copy the new position into the
	/// DVector3 pointer (if given). Returns distance along path
	/// traversed in step.
	
	// The idea here is to work in the coordinate system whose
	// axes point in directions defined in the following way:
	// z-axis is along direction of B-field
	// x-axis is in direction perpendicular to both B and p (particle momentum)
	// y-axis is then just cross product of z and x axes.
	//
	// These coordinates are referred to as the natural coordinates below.
	// The step is calculated based on moving along a perfect helix a distance
	// of "stepsize". This means that the new position will actually be
	// closer than stepsize to the current position (unless the magnetic field
	// is zero in which case they are equal).
	//
	// The values of the helix needed to project the step are
	// calculated for the current momentum and position in CalcDirs()
	// above. They should already be corect for the step we are
	// about to take. We call CalcDirs() after updating the position
	// and momentum in order to prepare for the next step.
	
	// If we weren't passed a step size, then calculate it using
	// info stored in the member data
	if(stepsize==0.0){
		// We always try growing the step size a little
		stepsize = 1.25*this->last_stepsize;
		
		// Don't grow past the set stepsize
		if(stepsize>this->stepsize)stepsize=this->stepsize;
	}

	// If the magnetic field is zero or the charge is zero, then our job is easy
	if(B.Mag2()==0.0 || q==0.0){
		DVector3 pstep = mom;
		pstep.SetMag(stepsize);
		pos += pstep;
		if(newpos)*newpos = pos;
		CalcDirs();

		return stepsize;
	}

	// Note that cos_theta, sin_theta, Rp, Ro, xdir, ydir, and zdir
	// should already be valid for the starting point of this step.
	// They are set via a call to CalcDirs()

	// delta_z is step size in z direction
	DVector3 delta_z = zdir*stepsize*cos_theta;

	// delta_phi is angle of rotation in natural x/y plane
	double delta_phi = stepsize/Rp;

	// delta_x is magnitude of step in natural x direction
	DVector3 delta_x = Ro*(1.0-cos(delta_phi))*xdir;
	
	// delta_y is magnitude of step in natural y direction
	DVector3 delta_y = Ro*sin(delta_phi)*ydir;
	
	// Calculate new position
	DVector3 mypos = pos + delta_x + delta_y + delta_z;

	// Adjust the step size if needed.
	// Check that the field hasn't changed too much here. If it has,
	// reduce the step size and try again.
	// We actually check three conditions:
	// 1.) That the step size is not already too small
	// 2.) that the radius of curvature is still not so big
	//     that it  is worth reducing the step size
	// 3.) that the difference of the field between the old
	//     and new points is small relative to the field size
	if(stepsize>0.1){
		if(fabs(stepsize/Ro) > 1.0E-4){
			double Bx,By,Bz;
			bfield->GetField(mypos.x(), mypos.y(), mypos.z(), Bx, By, Bz);
			DVector3 Bnew(Bx, By, Bz);
			DVector3 Bdiff = B-Bnew;
			double f = Bdiff.Mag()/B.Mag();
			if(f > 1.0E-4){
				return Step(newpos, stepsize/2.0);
			}
		}
	}

	// Step to new position
	pos = mypos;

	// Update momentum by rotating it by delta_phi about B
	mom.Rotate(-delta_phi, B);
	
	// Energy loss for 1.0GeV pions in Air is roughly 2.4keV/cm
	//double m = 0.13957;
	//double p = mom.Mag();
	//double E = sqrt(m*m + p*p) - 0.0000024*stepsize;
	//mom.SetMag(sqrt(E*E-m*m));

	// Calculate directions of natural coordinates at the new position
	CalcDirs();
	
	// return new position 
	if(newpos)*newpos = pos;
	
	// Record this step size for (possible) use on the next iteration
	last_stepsize = stepsize;

	return stepsize;
}
#endif

//-----------------------
// GetDirs
//-----------------------
void DMagneticFieldStepper::GetDirs(DVector3 &xdir, DVector3 &ydir, DVector3 &zdir)
{
	xdir = this->xdir;
	ydir = this->ydir;
	zdir = this->zdir;
}

//-----------------------
// SwimToPlane
//-----------------------
bool DMagneticFieldStepper::SwimToPlane(DVector3 &pos, DVector3 &mom, const DVector3 &origin, const DVector3 &norm)
{
	/// Swim the particle from the given position/momentum to
	/// the plane defined by origin and norm. "origin" should define a point
	/// somewhere in the plane and norm a vector normal to the plane.
	/// The charge of the particle is set by the constructor or last
	/// call to SetStartingParameters(...).
	///
	/// Note that a check is made that the particle is initially going
	/// toward the plane. If the particle appears to be going away
	/// from the plane, then the momentum is temporarily flipped as
	/// well as the charge so that the particle is swum backwards.
	///
	/// Particles will only be swum a distance of
	/// MAX_SWIM_DIST (currently hardwired to 20 meters) along their
	/// trajectory before returning boolean true indicating failure
	/// to intersect the plane.
	///
	/// On success, a value of false is returned and the values in
	/// pos and mom will be updated with the position and momentum at
	/// the point of intersection with the plane.
	
	// The method here is to step until we cross the plane.
	// This is indicated by the value k flipping its sign where
	// k is defined by:
	//
	//  k = (pos-origin).norm
	//
	// where pos, origin, and norm are all vectors and the "."
	// indicates a dot product.
	//
	// Once the plane is crossed, we know the intersection point is
	// less than one step away from the current step.
	
	// First, we determine whether we are going toward or away from
	// the plane for the given position/momentum. Note that we could
	// make the wrong choice here for certain geometries where the
	// curvature of the swimming changes direction enough.

_DBG_<<"This routine is not debugged!!!"<<endl;
	double b = norm.Dot(mom)*norm.Dot(pos-origin);
	bool momentum_and_charge_flipped = false;
	if(b<0.0){
		momentum_and_charge_flipped = true;
		//mom = -mom;
		//q = -q;
	}

	// Set the starting parameters and start stepping!
	SetStartingParams(q, &pos, &mom);
	double k, start_k = norm.Dot(pos-origin);
	double s = 0.0;
	do{
		s += Step(&pos);
		if(s>MAX_SWIM_DIST){
			if(momentum_and_charge_flipped){
				//mom = -mom;
				//q = -q;
			}
			return true;
		}
		k = norm.Dot(pos-origin);
	}while(k/start_k > 0.0);
	
	// OK, now pos should define a point
	// close enough to the plane that we can approximate the position
	// along the helix as a function of phi. phi is defined as the
	// phi angle about the center of the helix such that phi=0 points
	// to the current step position itself. This is similar to,
	// but not as complicated as, how the distance from the tracjectory
	// to a wire is calculated in DReferenceTrajectory::DistToRT.
	// See the comments there for more details.
	double dz_dphi = Ro*mom.Dot(zdir)/mom.Dot(ydir);
	DVector3 pos_diff = pos - origin;
	double A = xdir.Dot(norm);
	double B = ydir.Dot(norm);
	double C = zdir.Dot(norm);
	double D = pos_diff.Dot(norm);

	double alpha = -A*Ro/2.0;
	double beta = B*Ro + C*dz_dphi;
	
	// now we solve the quadratic equation for phi. It's not obvious
	// a priori which root will be correct so we calculate both and
	// choose the one closer to phi=0
	double d = sqrt(beta*beta - 4.0*alpha*D);
	double phi1 = (-beta + d)/(2.0*alpha);
	double phi2 = (-beta - d)/(2.0*alpha);
	double phi = fabs(phi1)<fabs(phi2) ? phi1:phi2;
	
	// Calculate position in plane
	pos += -Ro*phi*phi/2.0*xdir + Ro*phi*ydir + dz_dphi*phi*zdir;

	// Calculate momentum in plane
	mom.Rotate(phi, zdir);

	// If we had to flip the particle in order to hit the plane, flip it back
	if(momentum_and_charge_flipped){
		//mom = -mom;
		//q = -q;
	}

	return false;
}

//-----------------------
// DistToPlane
//-----------------------
bool DMagneticFieldStepper::DistToPlane(DVector3 &pos, const DVector3 &origin, const DVector3 &norm)
{

	return false;
}

//-----------------------
// SwimToRadius
//-----------------------
bool DMagneticFieldStepper::SwimToRadius(DVector3 &pos, DVector3 &mom, double R)
{

	return false;
}

//-----------------------
// DistToRadius
//-----------------------
bool DMagneticFieldStepper::DistToRadius(DVector3 &pos, double R)
{

	return false;
}

