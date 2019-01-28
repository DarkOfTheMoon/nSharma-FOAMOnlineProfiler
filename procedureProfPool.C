/*---------------------------------------------------------------------------*\
  =========                 |
 \\      /  F ield         | foam-extend: Open Source CFD
 \\    /   O peration     |
 \\  /    A nd           | For copyright notice see file Copyright
 \\/     M anipulation  |
 -------------------------------------------------------------------------------
 License
 This file is part of foam-extend.

 foam-extend is free software: you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the
 Free Software Foundation, either version 3 of the License, or (at your
 option) any later version.

 foam-extend is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

 \*---------------------------------------------------------------------------*/

#include "procedureProfPool.H"
#include "procedureProfInfo.H"
//#include "fvMesh.H"
#include "IOdictionary.H"
#include "Time.H"
#include "IOobjectList.H"

using namespace Foam;

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

Foam::procedureProfPool* Foam::procedureProfPool::thePool_(NULL);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::procedureProfPool::procedureProfPool(Time& runTime, fvMesh& mesh,
		string sectionT) :
		globalTime_(), targetSection_(sectionT), mesh_(mesh), runTime_(runTime) {

	totalIdleTime_ =0;
	totalBusyTime_ =0;

}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::procedureProfPool::~procedureProfPool() {
	for (mapIterator it = map().begin(); it != map().end(); ++it) {
		delete it->second;
	}

	map().erase(allInfo_.begin(), allInfo_.end());
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

fvMesh& Foam::procedureProfPool::getMesh() {

	return thePool_->mesh_;

}

Time& Foam::procedureProfPool::getRunTime() {

	return thePool_->runTime_;

}

procedureProfPool* Foam::procedureProfPool::getThePoolInstance() {

	return thePool_;

}

void Foam::procedureProfPool::setEnable(bool v) {

	enabled = v;
}

bool Foam::procedureProfPool::isEnabled() {

	return enabled;
}

void Foam::procedureProfPool::getOpsIDsOfType(std::list<Operation*>& pw) {

	for (procedureProfPool::mapConstIterator it = allInfo_.begin();
			it != allInfo_.end(); ++it) {

		if (it->second->isOperation()) {
			// TODO: memory pool, push_back is bad
			if (static_cast<Operation*>(it->second)->type_ == Operation::BUSY)
				pw.push_back(static_cast<Operation*>(it->second));

		}
	}
}

//Foam::string Foam::procedureProfPool::getTargetWorkloadSectionName() {
//	return procedureProfPool::thePool_->targetSection_.name;
//}
//
//void Foam::procedureProfPool::setTargetWorkloadSectionName(string name) {
//	procedureProfPool::thePool_->targetSection_.name = name;
//
//}

procedureProfPool::targetWorkloadSection& Foam::procedureProfPool::getTargetWorkloadSection() {
	return procedureProfPool::thePool_->targetSection_;
}

void Foam::procedureProfPool::initProfiling(Time& runTime, fvMesh& m,
		string targetSection) {
	if (!thePool_) {
		thePool_ = new procedureProfPool(runTime, m, targetSection);
		procedureProfInfo *master = new Section();
		thePool_->map().insert(make_pair(master->description(), master));
		thePool_->stack().push(*master);

		thePool_->setEnable(true);

		procedureProfPool::rememberTimer(*master, thePool_->globalTime_);

		IOdictionary loadManagerDict(IOobject("nSharmaDict", runTime.system(),
				runTime,
				IOobject::MUST_READ, IOobject::NO_WRITE));

		thePool_->heterogeneityFactors = List<float>(Pstream::nProcs(), 0.0);

		loadManagerDict.subDict("heterogeneity").readIfPresent("factors",
				thePool_->heterogeneityFactors);

	}

}

//label Foam::procedureProfPool::nCells() {
//	return mesh_.nCells();
//}

void Foam::procedureProfPool::rememberTimer(const procedureProfInfo& info,
		clockTime& timer) {
	if (!thePool_) {
		FatalErrorIn
		(
				"procedureProfPool::rememberTimer(const procedureProfInfo Foam&info, "
				"clockTime& timer)"
		)<< "Singleton not initialized\n" << endl << abort(FatalError);
	}

	thePool_->stack().addTimer(info, timer);

}

void Foam::procedureProfPool::remove(const procedureProfInfo &info) {
	if (!thePool_) {
		FatalErrorIn("procedureProfPool::addInfo(const string& name)")<< "Singleton not initialized\n" << endl << abort(FatalError);
	}

	procedureProfStack& stack = thePool_->stack();

	if (info.id() != stack.top().id()) {
		FatalErrorIn("procedureProfPool::update(const string &name)")
		<< "The id " << info.id() << " of the updated info "
		<< info.description()
		<< " is no the same as the one on top of the stack: "
		<< stack.top().id() << " (" << stack.top().description()
		<< ")\n" << endl << abort(FatalError);
	}

	stack.pop();
}

bool Foam::procedureProfPool::writeData(Ostream& os) const {
	os << "procedureProfInfo" << nl << indent << token::BEGIN_LIST << incrIndent
			<< nl;

	stack().writeStackContents(os);

	for (mapConstIterator it = map().begin(); it != map().end(); ++it) {
		if (!it->second->onStack()) {
			os << *(it->second);
		}
	}

	os << decrIndent << indent << token::END_LIST << token::END_STATEMENT
			<< endl;

	return os;
}

}

// ************************************************************************* //
