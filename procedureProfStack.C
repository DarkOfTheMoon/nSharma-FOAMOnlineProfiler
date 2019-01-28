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

#include "procedureProfStack.H"
#include "procedureProfInfo.H"
#include "OPstream.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::procedureProfStack::procedureProfStack() :
		timers_(), std::vector<procedureProfInfo*>() {


}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::procedureProfStack::~procedureProfStack() {
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::procedureProfInfo &Foam::procedureProfStack::top() const {
	return *std::vector<procedureProfInfo*>::back();
}

//Foam::procedureProfInfo &Foam::procedureProfStack::bottom() const {
//	return *std::stack<procedureProfInfo*>::bottom();
//}

bool Foam::procedureProfStack::empty() const {
	return std::vector<procedureProfInfo*>::empty();
}

void Foam::procedureProfStack::push(procedureProfInfo &a) {
	std::vector<procedureProfInfo*>::push_back(&a);
	top().addedToStack();
}

Foam::procedureProfInfo &Foam::procedureProfStack::pop() {
	top().removedFromStack();
	std::vector<procedureProfInfo*>::pop_back();
}

void Foam::procedureProfStack::writeStackContents(Ostream &os) const {
//	if (empty()) {
//		return;
//	}
//	const_iterator it = begin();
//	scalar oldElapsed = 0;
//	do {
//		const procedureProfInfo &info = *(*it);
//		scalar elapsed = timers_[info.id()]->elapsedTime();
//
//		info.writeWithOffset(os, true, elapsed, oldElapsed);
//
//		oldElapsed = elapsed;
//		++it;
//	} while (it != end());
}

void Foam::procedureProfStack::addTimer(const procedureProfInfo &info,
		clockTime &timer) {


	timers_.erase(info.id());
	timers_.insert( std::pair<label,clockTime*>(info.id(), &timer));

	//timers_.insert(info.id(),&timer);

//	clockTime* t = new clockTime();
//    timers_.insert(info.id(),t);
//
//
//    Pout << info.description() << "before" <<
//    info.id() << " " << timers_[info.id()]->elapsedTime() << endl;
//
//
//    timers_.erase(info.id());
//
//    timers_.insert(info.id(),&timer);
//
//
//    Pout << info.description() << " after " <<
//    	info.id() << " " << timers_[info.id()]->elapsedTime() << endl;

}

// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

// ************************************************************************* //
