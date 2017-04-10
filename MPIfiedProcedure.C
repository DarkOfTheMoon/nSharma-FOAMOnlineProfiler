/*---------------------------------------------------------------------------*\


 Class
 Foam::MPIfiedProcedure

 Description
 MPI datatype connected, careful when changing. Update init

 \*---------------------------------------------------------------------------*/

#include "MPIfiedProcedure.H"
#include "procedureProfInfo.H"

namespace Foam {

MPIfiedProcedure::MPIfiedProcedure() {

	id_ = 0;
	calls_ = 0;
	totalTime_ = 0;
	childTime_ = 0;
	opType_ = Foam::Operation::NA;
}

MPIfiedProcedure::MPIfiedProcedure(procedureProfInfo &p) {
	id_ = p.id();
	calls_ = p.calls();
	totalTime_ = p.totalTime();
	childTime_ = p.childTime();
	opType_ = Foam::Operation::NA;
}

MPIfiedProcedure::MPIfiedProcedure(Operation &p) {
	id_ = p.id();
	calls_ = p.calls();
	totalTime_ = p.totalTime();
	childTime_ = p.childTime();
	opType_ = p.type_;
}

MPIfiedProcedure::~MPIfiedProcedure() {
}

}

