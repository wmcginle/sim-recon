#ifndef _DKinFitConstraints_
#define _DKinFitConstraints_

#include <deque>
#include <map>
#include <utility>

#include "TVector3.h"
#include "TLorentzVector.h"

using namespace std;

class DKinFitter;
class DKinFitParticle;

class DKinFitConstraint //purely virtual: cannot directly instantiate class, can only inherit from it
{
	friend class DKinFitter;

	protected:
		virtual ~DKinFitConstraint(void) = 0; //forces abstractness
};

class DKinFitConstraint_VertexBase : public DKinFitConstraint //purely virtual: cannot directly instantiate class, can only inherit from it
{
	friend class DKinFitter;

	public:
		virtual TVector3 Get_CommonVertex(void) const = 0; //inheriting classes MUST define this method
		inline int Get_VxParamIndex(void) const{return dVxParamIndex;}
		int Get_FIndex(const DKinFitParticle* locKinFitParticle) const;

	protected:

		DKinFitConstraint_VertexBase(void);
		virtual ~DKinFitConstraint_VertexBase(void){}

		virtual void Reset(void);

		inline void Set_FIndex(const DKinFitParticle* locKinFitParticle, unsigned int locFIndex){dConstraintEquationParticleMap[locKinFitParticle] = locFIndex;}
		inline void Set_VxParamIndex(int locVxParamIndex){dVxParamIndex = locVxParamIndex;}

		int dVxParamIndex; //location of the Vx uncertainty in the dVXi covariance matrix term (Vy & Vz are the subsequent terms)

		map<const DKinFitParticle*, unsigned int> dConstraintEquationParticleMap; //key is particle (NULL for RF time), value is the constraint equation index (must check whether decaying or not to see what f-matrix it's in!)
};

class DKinFitConstraint_Vertex : public DKinFitConstraint_VertexBase
{
	friend class DKinFitter;

	public:
		inline TVector3 Get_CommonVertex(void) const{return dCommonVertex;}

		inline deque<const DKinFitParticle*> Get_ConstrainVertexParticles(void) const{return deque<const DKinFitParticle*>(dConstrainVertexParticles.begin(), dConstrainVertexParticles.end());}
		inline deque<const DKinFitParticle*> Get_NoConstrainParticles(void) const{return deque<const DKinFitParticle*>(dNoConstrainParticles.begin(), dNoConstrainParticles.end());}
		inline deque<pair<const DKinFitParticle*, bool> > Get_DecayingParticles(void) const{return deque<pair<const DKinFitParticle*, bool> >(dDecayingParticles.begin(), dDecayingParticles.end());}

		bool Get_DecayingParticleInInitialStateFlag(const DKinFitParticle* locKinFitParticle) const;

	private:
		DKinFitConstraint_Vertex(void);
		~DKinFitConstraint_Vertex(void){}

		void Reset(void);

		void Set_CommonVertex(TVector3 locCommonVertex){dCommonVertex = locCommonVertex;}

		inline void Set_ConstrainVertexParticles(deque<DKinFitParticle*>& locConstrainVertexParticles){dConstrainVertexParticles = locConstrainVertexParticles;}
		inline void Set_NoConstrainParticles(deque<DKinFitParticle*>& locNoConstrainParticles){dNoConstrainParticles = locNoConstrainParticles;}
		inline void Set_DecayingParticles(deque<pair<DKinFitParticle*, bool> >& locDecayingParticles){dDecayingParticles = locDecayingParticles;}

		inline void Add_NoConstrainParticle(DKinFitParticle* locKinFitParticle){dNoConstrainParticles.push_back(locKinFitParticle);}
		inline void Add_ConstrainVertexParticle(DKinFitParticle* locKinFitParticle){dConstrainVertexParticles.push_back(locKinFitParticle);}

		deque<DKinFitParticle*> dConstrainVertexParticles; //charged particles, decaying particles, beam particles
		deque<pair<DKinFitParticle*, bool> > dDecayingParticles; //bool is true if vertex is production vertex / particle in final state, false if decay vertex / initial state
		deque<DKinFitParticle*> dNoConstrainParticles; //missing particles & neutral showers //not used to constrain vertex or time, but fit vertex is set for this particle

		TVector3 dCommonVertex; //for propagating the track to this point
};

class DKinFitConstraint_Spacetime : public DKinFitConstraint_VertexBase
{
	friend class DKinFitter;

	public:
		inline TLorentzVector Get_CommonSpacetimeVertex(void) const{return dCommonSpacetimeVertex;}
		inline TVector3 Get_CommonVertex(void) const{return dCommonSpacetimeVertex.Vect();}
		inline double Get_CommonTime(void) const{return dCommonSpacetimeVertex.T();}

		inline bool Get_UseRFTimeFlag(void) const{return dUseRFTimeFlag;}

		inline int Get_TParamIndex(void) const{return dTParamIndex;}

		inline deque<const DKinFitParticle*> Get_ConstrainSpacetimeParticles(void) const{return deque<const DKinFitParticle*>(dConstrainSpacetimeParticles.begin(), dConstrainSpacetimeParticles.end());}
		inline deque<const DKinFitParticle*> Get_OnlyConstrainTimeParticles(void) const{return deque<const DKinFitParticle*>(dOnlyConstrainTimeParticles.begin(), dOnlyConstrainTimeParticles.end());} //neutral showers
		inline deque<const DKinFitParticle*> Get_NoConstrainParticles(void) const{return deque<const DKinFitParticle*>(dNoConstrainParticles.begin(), dNoConstrainParticles.end());}
		inline deque<pair<const DKinFitParticle*, bool> > Get_DecayingParticles(void) const{return deque<pair<const DKinFitParticle*, bool> >(dDecayingParticles.begin(), dDecayingParticles.end());}

		bool Get_DecayingParticleInInitialStateFlag(const DKinFitParticle* locKinFitParticle) const;

	private:
		DKinFitConstraint_Spacetime(void);
		~DKinFitConstraint_Spacetime(void){}

		void Reset(void);

		void Set_CommonVertex(TVector3 locCommonVertex){dCommonSpacetimeVertex.SetVect(locCommonVertex);}
		inline void Set_CommonTime(double locCommonTime){dCommonSpacetimeVertex.SetT(locCommonTime);}
		inline void Set_CommonSpacetimeVertex(TLorentzVector locCommonSpacetimeVertex){dCommonSpacetimeVertex = locCommonSpacetimeVertex;}

		inline void Set_UseRFTimeFlag(bool locUseRFTimeFlag){dUseRFTimeFlag = locUseRFTimeFlag;}
		inline void Set_TParamIndex(int locTParamIndex){dTParamIndex = locTParamIndex;}

		inline void Add_ConstrainSpacetimeParticle(DKinFitParticle* locKinFitParticle){dConstrainSpacetimeParticles.push_back(locKinFitParticle);}
		inline void Add_NoConstrainParticle(DKinFitParticle* locKinFitParticle){dNoConstrainParticles.push_back(locKinFitParticle);}

		inline void Set_ConstrainSpacetimeParticles(deque<DKinFitParticle*>& locConstrainSpacetimeParticles){dConstrainSpacetimeParticles = locConstrainSpacetimeParticles;}
		inline void Set_OnlyConstrainTimeParticles(deque<DKinFitParticle*>& locOnlyConstrainTimeParticles){dOnlyConstrainTimeParticles = locOnlyConstrainTimeParticles;}
		inline void Set_NoConstrainParticles(deque<DKinFitParticle*>& locNoConstrainParticles){dNoConstrainParticles = locNoConstrainParticles;}
		inline void Set_DecayingParticles(deque<pair<DKinFitParticle*, bool> >& locDecayingParticles){dDecayingParticles = locDecayingParticles;}

		deque<DKinFitParticle*> dConstrainSpacetimeParticles; //charged particles, decaying particles, beam particles
		deque<DKinFitParticle*> dOnlyConstrainTimeParticles; //neutral showers //not used to constrain vertex, but fit vertex is used for time constraint
		deque<DKinFitParticle*> dNoConstrainParticles; //missing particles //not used to constrain vertex or time, but fit vertex & time are set to this particle
		deque<pair<DKinFitParticle*, bool> > dDecayingParticles; //bool is true if vertex is production vertex / particle in final state, false if decay vertex / initial state

		bool dUseRFTimeFlag; //for time constraint
		TLorentzVector dCommonSpacetimeVertex; //for propagating the track to this point
		int dTParamIndex;
};

class DKinFitConstraint_P4 : public DKinFitConstraint //one will be implemented as a p4 constraint, the rest as invariant mass constraints
{
	friend class DKinFitter;

	public:
		inline size_t Get_FIndex(void) const{return dFIndex;}

		inline const DKinFitParticle* Get_ConstrainedP4Particle(void) const{return dConstrainedP4Particle;}

		inline deque<const DKinFitParticle*> Get_InitialParticles(void) const{return deque<const DKinFitParticle*>(dInitialParticles.begin(), dInitialParticles.end());}
		inline deque<const DKinFitParticle*> Get_FinalParticles(void) const{return deque<const DKinFitParticle*>(dFinalParticles.begin(), dFinalParticles.end());}

	private:
		DKinFitConstraint_P4(void);
		~DKinFitConstraint_P4(void){}

		void Reset(void);

		inline void Set_FIndex(size_t locFIndex){dFIndex = locFIndex;}
		inline void Set_ConstrainedP4Particle(DKinFitParticle* locConstrainedP4Particle){dConstrainedP4Particle = locConstrainedP4Particle;}
		inline void Set_InitialParticles(const deque<DKinFitParticle*>& locInitialParticles){dInitialParticles = locInitialParticles;}
		inline void Set_FinalParticles(const deque<DKinFitParticle*>& locFinalParticles){dFinalParticles = locFinalParticles;}

		size_t dFIndex; //starting row index of the equation(s) corresponding to these constraints in the dF matrix term

		DKinFitParticle* dConstrainedP4Particle;
		deque<DKinFitParticle*> dInitialParticles;
		deque<DKinFitParticle*> dFinalParticles;
};

#endif // _DKinFitConstraints_
