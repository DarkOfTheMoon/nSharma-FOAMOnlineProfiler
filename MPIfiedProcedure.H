/*---------------------------------------------------------------------------*\


 Class
 Foam::MPIfiedProcedure

 Description
 MPI datatype connected, careful when changing. Update init

 \*---------------------------------------------------------------------------*/

#ifndef MPIfiedProcedure_H
#define MPIfiedProcedure_H

#include "label.H"
#include "scalar.H"
//#include "mpi.h"

namespace Foam {

class Operation;
class procedureProfInfo;

class MPIfiedProcedure {
public:
	// nr of times this was called
	label calls_;

	// total time spent
	scalar totalTime_;

	// time spent in children
	scalar childTime_;

	// unique id to identify it
	label id_;

	label opType_;

	//static MPI_Datatype MPI_LOADPROCEDURE;

	MPIfiedProcedure();

	MPIfiedProcedure(procedureProfInfo &p);

	MPIfiedProcedure(Operation &p);

	~MPIfiedProcedure();
};

}

#endif
