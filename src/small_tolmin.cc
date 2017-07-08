/* tolmin.f -- translated by f2c (version 20010821).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/
# include <problem.h>
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
#include "f2c.h"

static long iabs(long x)
{
	return x>0?x:-x;
}

#ifdef __cplusplus
extern "C" {
#endif

# define MAXCALLS	20
/* Common Block Declarations */

# define integer long
# define double double
# define real float
static struct {
    integer itnocs, ipartc, itder, ipder, ithess, iphess, itjac, ipjac;
} totcal_;

#define totcal_1 totcal_

static struct {
    integer iuinp, iuout;
} units_;

#define units_1 units_

/* Table of constant values */

static integer c__0 = 0;

/*  --------------------------------------------------------------------- */

/* Subroutine */ static int getmin_(integer *n, integer *m, integer *meq, double 
	*a, integer *ia, double *b, double *xl, double *xu, 
	double *x, double *acc, integer *iact, integer *nact, 
	double *par, integer *iprint, integer *info, double *w,
	MinInfo &Info)
{
    /* System generated locals */
    integer a_dim1, a_offset;

    /* Local variables */
     integer iztg, ixbig, ibres, id, ig, iu, iz;
    extern /* Subroutine */ int minflc_(integer *, integer *, integer *, 
	    double *, integer *, double *, double *, double *,
	     double *, double *, integer *, integer *, double *, 
	    integer *, integer *, double *, double *, double *, 
	    double *, double *, double *, double *, 
	    double *, double *, double *, double *,
	    MinInfo &);
     integer ireskt, igm, igs, ixs;


/*  This is the entry point to a package of subroutines that calculate */
/*     the least value of a differentiable function of several variables */
/*     subject to linear constraints on the values of the variables. */
/*  N is the number of variables and must be set by the user. */
/*  M is the number of linear constraints (excluding simple bounds) and */
/*     must be set by the user. */
/*  MEQ is the number of constraints that are equalities and must be set */
/*     by the user. */
/*  A(.,.) is a 2-dimensional array whose columns are the gradients of */
/*     the M constraint functions. Its entries must be set by the user */
/*     and its dimensions must be at least N by M. */
/*  IA is the actual first dimension of the array A that is supplied by */
/*     the user, so its value may not be less than N. */
/*  B(.) is a vector of constraint right hand sides that must also be set */
/*     by the user.  Specifically the constraints on the variables X(I) */
/*     I=1(1)N are */
/*          A(1,K)*X(1)+...+A(N,K)*X(N) .EQ. B(K)  K=1,...,MEQ */
/*          A(1,K)*X(1)+...+A(N,K)*X(N) .LE. B(K)  K=MEQ+1,...,M  . */
/*     Note that the data that define the equality constraints come */
/*     before the data of the inequalities. */
/*  XL(.) and XU(.) are vectors whose components must be set to lower and */
/*     upper bounds on the variables.  Choose very large negative and */
/*     positive entries if a component should be unconstrained, or set */
/*     XL(I)=XU(I) to freeze the I-th variable.  Specifically these */
/*     simple bounds are */
/*          XL(I) .LE. X(I) and X(I) .LE. XU(I)  I=1,...,N  . */
/*  X(.) is the vector of variables of the optimization calculation. Its */
/*     initial elements must be set by the user to an estimate of the */
/*     required solution.  The subroutines can usually cope with poor */
/*     estimates, and there is no need for X(.) to be feasible initially. */
/*     These variables are adjusted automatically and the values that */
/*     give the least feasible calculated value of the objective function */
/*     are available in X(.) on the return from GETMIN. */
/*  ACC is a tolerance on the first order conditions at the calculated */
/*     solution of the optimization problem.  These first order */
/*     conditions state that, if X(.) is a solution, then there is a set */
/*     of active constraints with indices IACT(K) K=1(1)NACT, say, */
/*     such that X(.) is on the boundaries of these constraints, */
/*     and the gradient of the objective function can be expressed */
/*     in the form */
/*           GRAD(F)=PAR(1)*GRAD(C(IACT(1)))+... */
/*                        ...+PAR(NACT)*GRAD(C(IACT(NACT)))  . */
/*     Here PAR(K) K=1(1)NACT are Lagrange multipliers that are */
/*     nonpositive for inequality constraints, and GRAD(C(IACT(K))) */
/*     is the gradient of the IACT(K)-th constraint function, so it is */
/*     A(.,IACT(K)) if IACT(K) .LE. M, and it is minus or plus the J-th */
/*     coordinate vector if the constraint is the lower or upper bound on */
/*     X(J) respectively. The normal return from the calculation occurs */
/*     when X(.) is feasible and the sum of squares of components of the */
/*     vector RESKT(.) is at most ACC**2, where RESKT(.) is the */
/*     N-component vector of residuals of the first order condition that */
/*     is displayed above. */
/*     Sometimes the package cannot satisfy this condition, because noise */
/*     in the function values can prevent a change to the variables, */
/*     no line search being allowed to increase the objective function. */
/*  IACT(.) is a working space array of integer variables that must be */
/*     provided by the user.  Its length must be at least (M+2*N).  Its */
/*     leading entries on the return from the subroutine are the indices */
/*     IACT(K) K=1(1)NACT that are mentioned in the previous paragraph: */
/*     in other words they are the indices of the final active */
/*     constraints. */
/*     Here the indices M+1,...,M+N and M+N+1,...,M+2*N denote the lower */
/*     and upper bounds respectively. */
/*  NACT is set automatically to the integer variable of this ilk that */
/*     has been introduced already. */
/*  PAR is a one-dimensional array that will hold the Lagrange */
/*     multipliers PAR(K) K=1(1)NACT on the return from GETMIN, these */
/*     parameters being defined in the above paragraph on ACC. */
/*     The length of PAR should be at least N. */
/*  IPRINT must be set by the user to specify the frequency of printing */
/*     during the execution of the optimization package.  There is no */
/*     printed output if IPRINT=0.  Otherwise, after ensuring */
/*     feasibility, information is given every IABS(IPRINT) iterations */
/*     and whenever a parameter called TOL is reduced. The printing */
/*     provides the values of X(.), F(.) and G(.)=GRAD(F) if IPRINT */
/*     is positive, while if IPRINT is negative this information is */
/*     augmented by the current values of IACT(K) K=1(1)NACT, */
/*     PAR(K) K=1(1)NACT and RESKT(I) I=1(1)N. The reason for returning */
/*     to the calling program is also displayed when IPRINT is nonzero. */
/*  INFO is an integer variable that should be set to zero initially, */
/*     unless the user wishes to impose an upper bound on the number of */
/*     evaluations of the objective function and its gradient, in which */
/*     case INFO should be set to the value of this bound.  On the exit */
/*     from GETMIN it will have one of the following integer values to */
/*     indicate the reason for leaving the optimization package: */
/*          INFO=1   X(.) is feasible and the condition that depends on */
/*     ACC is satisfied. */
/*          INFO=2   X(.) is feasible and rounding errors are preventing */
/*     further progress. */
/*          INFO=3   X(.) is feasible but the objective function fails to */
/*     decrease although a decrease is predicted by the current gradient */
/*     vector.  If this return occurs and KTRES(.) has large components */
/*     then the user's calculation of the gradient of the objective */
/*     function may be incorrect.  One should also question the coding of */
/*     the gradient when the final rate of convergence is slow. */
/*          INFO=4   In this case the calculation cannot begin because IA */
/*     is less than N or because the lower bound on a variable is greater */
/*     than the upper bound. */
/*          INFO=5   This value indicates that the equality constraints */
/*     are inconsistent.   These constraints include any components of */
/*     X(.) that are frozen by setting XL(I)=XU(I). */
/*          INFO=6   In this case there is an error return because the */
/*     equality constraints and the bounds on the variables are found to */
/*     be inconsistent. */
/*          INFO=7   This value indicates that there is no vector of */
/*     variables that satisfies all of the constraints.  Specifically, */
/*     when this return or an INFO=6 return occurs, the current active */
/*     constraints (whose indices are IACT(K) K=1(1)NACT) prevent any */
/*     change in X(.) that reduces the sum of constraint violations, */
/*     where only bounds are included in this sum if INFO=6. */
/*          INFO=8   In this case the limit on the number of calls of */
/*     subroutine FGCALC (see below) has been reached, and there would */
/*     have been further calculation otherwise. */
/*  W(.) is a working space array of real variables that must be provided */
/*     by the user.  Its length must be at least (M+11*N+N**2).  On exit */
/*     from the package one can find the final components of GRAD(F) and */
/*     RESKT(.) in W(1),...,W(N) and W(N+1),...,W(2*N) respectively. */
/*  Note 1.   The variables N, M, MEQ, IA, ACC and IPRINT and the */
/*     elements of the arrays A(,.,), B(.), XL(.) and XU(.) are not */
/*     altered by the optimization procedure. Their values, the value of */
/*     INFO and the initial components of X(.) must be set on entry */
/*     to GETMIN. */
/*  Note 2.   Of course the package needs the objective function and its */
/*     gradient.  Therefore the user must provide a subroutine called */
/*     FGCALC, its first two lines being */
/*          SUBROUTINE FGCALC (N,X,F,G) */
/*          DIMENSION X(*),G(*)   . */
/*     It is called automatically with N set as above and with X(.) set */
/*     to a feasible vector of variables.  It should calculate the value */
/*     of the objective function and its gradient for this X(.) and */
/*     should set them in F and G(I) I=1(1)N respectively, without */
/*     disturbing N or any of the components of X(.). */
/*  Note 3.   A paper on the method of calculation and a report on the */
/*     main features of the computer code are available from the author */
/*     M.J.D.Powell (D.A.M.T.P., University of Cambridge, Silver Street, */
/*     Cambridge CB3 9EW, England). */

/*     Partition the workspace array. */

    /* Parameter adjustments */
     *info = MAXCALLS;
    a_dim1 = *ia;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --b;
    --xl;
    --xu;
    --x;
    --iact;
    --par;
    --w;

    /* Function Body */
    ig = 1;
    ireskt = ig + *n;
    iz = ireskt + *n;
    iu = iz + *n * *n;
    ixbig = iu + *n;
    ibres = ixbig + *n;
    id = ibres + *m + *n + *n;
    iztg = id + *n;
    igm = iztg + *n;
    ixs = igm + *n;
    igs = ixs + *n;

/*     Call the optimization package. */

    minflc_(n, m, meq, &a[a_offset], ia, &b[1], &xl[1], &xu[1], &x[1], acc, &
	    iact[1], nact, &par[1], iprint, info, &w[ig], &w[iz], &w[iu], &w[
	    ixbig], &w[ireskt], &w[ibres], &w[id], &w[iztg], &w[igm], &w[ixs],
	     &w[igs],Info);
    return 0;
} /* getmin_ */


static double oldmin=1e+10;
double small_tolmin(Matrix &x,MinInfo &Info)
{
	double fmin;
	oldmin=1e+10;
	totcal_1.itnocs =0 ;
	long n=x.size();
	long m=0;
	long meq=0;
	double *a=new double[n*n];
	long ia=n;
	double b[n];
	double xl[n];
	double xu[n];
	double *xp=new double[n];
	Matrix x1,x2;
	x1.resize(Info.p->getDimension());
	x2.resize(Info.p->getDimension());
	Info.p->getLeftMargin(x1);
	Info.p->getRightMargin(x2);
	for(int i=0;i<n;i++) 
	{
		xp[i]=x[i];
		xl[i]=-1e+10;
		xu[i]=1e+10;
		if(x1.size())
		{	
			xl[i]=x1[i];
			xu[i]=x2[i];
		}
		
	}
	double acc=1e-19;
	long iact[3*n];
	long nact;
	double par[n];
	long iprint=0;
	long info=0;
	double *w=new double[12*n+n*n*n];
	

	int save=Info.fevals;
	Info.fevals=0;
	Info.gevals=0;
	getmin_(&n,&m,&meq,a,&ia,b,xl,xu,xp,&acc,iact,&nact,par,&iprint,&info,
			w,Info);
//	Info.fevals+=save;

	for(int i=0;i<n;i++) x[i]=xp[i];
	fmin=Info.p->funmin(x);
	delete[] w;
	delete[] a;
	delete[] xp;
	return fmin;
}

#ifdef __cplusplus
	}
#endif
