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

#include "procedureProfTrigger.H"
#include "procedureProfInfo.H"
#include "dictionary.H"
#include "procedureProfPool.H"
#include "MPIfiedProcedure.H"
#include "OPstream.H"

using namespace Foam;

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

Foam::label Foam::procedureProfInfo::nextId_(0);

Foam::label Foam::procedureProfInfo::getID() {
	nextId_++;
	return nextId_;
}

void Foam::procedureProfInfo::raiseID(label maxVal) {
	if (maxVal > nextId_) {
		nextId_ = maxVal;
	}
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::procedureProfInfo::procedureProfInfo() :
		calls_(0), totalTime_(0.), childTime_(0.), id_(getID()), parent_(*this), description_(
				PROF_INFO_MAIN_SECTION_NAME), onStack_(false) {
}

Foam::procedureProfInfo::procedureProfInfo(procedureProfInfo &parent,
		const string &descr) :
		calls_(0), totalTime_(0.), childTime_(0.), id_(getID()), parent_(
				parent), description_(descr), onStack_(false) {
}

Foam::Section::Section(procedureProfInfo &parent, const string &descr) :
		procedureProfInfo(parent, descr) {

	if (descr
			== procedureProfPool::thePool_->getTargetWorkloadSection().name()) {

		procedureProfPool::thePool_->getTargetWorkloadSection().setPointer(
				this);
		// not sure if needed
		procedureProfPool::thePool_->getTargetWorkloadSection().startTimer();

	}

}

Foam::Section::Section() :
		procedureProfInfo() {

	if (PROF_INFO_MAIN_SECTION_NAME
			== procedureProfPool::thePool_->getTargetWorkloadSection().name()) {

		procedureProfPool::thePool_->getTargetWorkloadSection().setPointer(
				this);
		// not sure if needed
		procedureProfPool::thePool_->getTargetWorkloadSection().startTimer();

	}

}

Foam::Operation::Operation(procedureProfInfo &parent, const string &descr) :
		procedureProfInfo(parent, descr) {

}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::procedureProfInfo::~procedureProfInfo() {
}

Foam::Section::~Section() {
}

Foam::Operation::~Operation() {
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

//bool Foam::procedureProfInfo::isOperation(){
//
//	Info << "FATAL..." << endl;
//
//}

void Foam::procedureProfInfo::update(scalar elapsedTimee) {
	calls_++;
	totalTime_ += elapsedTimee;
	if (id() != parent().id()) {
		parent_.childTime_ += elapsedTimee;
	}

}

void Foam::Operation::update(scalar elapsedTimee) {

	procedureProfInfo::update(elapsedTimee);

	procedureProfPool::thePool_->updateTimers(type_,
			elapsedTimee);

//	procedureProfPool::thePool_->getTargetWorkloadSection().updateTimers(type_,
//			elapsedTimee);

}

void Foam::Section::update(scalar elapsedTimee) {

	procedureProfInfo::update(elapsedTimee);

	if (description()
			== procedureProfPool::thePool_->getTargetWorkloadSection().name()) {
		procedureProfPool::thePool_->getTargetWorkloadSection().updateTotal();

	}
}

void Foam::procedureProfInfo::writeWithOffset(Ostream &os, bool offset,
		scalar time, scalar childTimes) const {
	dictionary tmp;

	tmp.add("id", id());
	if (id() != parent().id()) {
		tmp.add("parentId", parent().id());
	}
	tmp.add("description", description());
	tmp.add("calls", calls() + (offset ? 1 : 0));
	tmp.add("totalTime", totalTime() + time);
	tmp.add("childTime", childTime() + childTimes);
	tmp.add("onStack", onStack());

	os << tmp;
}

void Foam::Section::MPIfy(Foam::MPIfiedProcedure& o) {

	o = MPIfiedProcedure(*this);

}

void Foam::Operation::MPIfy(Foam::MPIfiedProcedure& o) {

	o = MPIfiedProcedure(*this);

}

// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

Foam::Ostream& Foam::operator<<(Ostream& os, const procedureProfInfo& info) {
	info.writeWithOffset(os);

	return os;
}

// ************************************************************************* //

