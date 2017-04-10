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

 Class
 Foam::procedureProfTrigger

 Description
 The object that does the actual measuring

 SourceFiles
 procedureProfTrigger.C

 \*---------------------------------------------------------------------------*/

#ifndef procedureProfTrigger_H
#define procedureProfTrigger_H

#include "procedureProfInfo.H"

#include "clockTime.H"
#include "foamString.H"
#include "scalar.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam {

/*---------------------------------------------------------------------------*\
                           Class procedureProfTrigger Declaration
 \*---------------------------------------------------------------------------*/

template<class T>
class procedureProfTrigger {
	// Private data

	clockTime clock_;

	T *info_;

	bool running_;

	// Private Member Functions

	//- Disallow default bitwise copy construct
	procedureProfTrigger(const procedureProfTrigger&);

	//- Disallow default bitwise assignment
	void operator=(const procedureProfTrigger&);

protected:

	clockTime &clock() {
		return clock_;
	}

	const T &info() const {
		return *info_;
	}

public:

	// Constructors

	procedureProfTrigger(const string &name);
	procedureProfTrigger(const string &name, Operation::opType);


	~procedureProfTrigger();

	void stop();
};

template<class T>
Foam::procedureProfTrigger<T>::~procedureProfTrigger() {
	stop();
}

//template<class T>
//void Foam::procedureProfTrigger<T>::stop() {
//	if (running_ && info_) {
//		scalar elapsed = clock_.elapsedTime();
//		info_->update(elapsed);
//		procedureProfPool::remove(*info_);
//		running_ = false;
//	}
//}

//template<class T>
//Foam::procedureProfTrigger<T>::procedureProfTrigger(const string &name) :
//		clock_(), running_(true) {
//
//	if (procedureProfPool::thePool_
//			&& procedureProfPool::thePool_->isEnabled()) {
//
//	}
//
//	info_ = &procedureProfPool::getInfo<T>(name);
//
//	if (info_)
//		procedureProfPool::rememberTimer(info(), clock());
//
//}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}// End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
