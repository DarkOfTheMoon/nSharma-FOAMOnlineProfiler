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
#include "procedureProfPool.H"
//#include "loadManager.H"
#include "Pstream.H"
#include <unistd.h>
#include <time.h>

using namespace Foam;

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

namespace Foam {

template<>
Foam::procedureProfTrigger<Section>::procedureProfTrigger(const string &name) :
		clock_(), running_(false) {

	info_ = NULL;
	if (procedureProfPool::thePool_
			&& procedureProfPool::thePool_->isEnabled()) {

		running_ = true;

		info_ = &procedureProfPool::getInfo<Section>(name);

//		if (info_->description() == "updateLM")
//		sleep(1);

		procedureProfPool::rememberTimer(info(), clock());

//		if (info_->description() == "updateLM") {
//			Pout << "Trigger cretaed"
//					<< procedureProfPool::thePool_->stack().timers_[info_->id()]->elapsedTime()
//					<< " "
//					<< clock().elapsedTime()
//					<< " "
//					<< info_->id()
//					<< endl;
//		}

		if (info_->description()
				== procedureProfPool::thePool_->getTargetWorkloadSection().name()) {

			procedureProfPool::thePool_->getTargetWorkloadSection().startTimer();

		}

	}

}

template<>
Foam::procedureProfTrigger<Operation>::procedureProfTrigger(const string &name,
		Operation::opType t) :
		clock_(), running_(false) {

	info_ = NULL;
	if (procedureProfPool::thePool_
			&& procedureProfPool::thePool_->isEnabled()) {

		running_ = true;

		const procedureProfInfo* top =
				&(procedureProfPool::thePool_->stack().top());

		bool allowed = false;

		if (procedureProfPool::thePool_->getTargetWorkloadSection().name() == PROF_INFO_MAIN_SECTION_NAME) {
			allowed = true;
		}

		while (!allowed && top != &top->parent()) {

			if (top->description()
					== procedureProfPool::thePool_->getTargetWorkloadSection().name()) {
				allowed = true;
			} else {
				top = &top->parent();
			}
		}

		if (allowed) {
			string prefix = "op_";

			if (t == Operation::BUSY) {
				prefix += "b";
			} else if (t == Operation::IDLE) {
				prefix += "i";
			}
			info_ = &procedureProfPool::getInfo<Operation>(prefix + ":" + name);
			info_->type_ = t;
			procedureProfPool::rememberTimer(info(), clock());
		}

//		struct timespec tim;
//		tim.tv_sec = 0;
//		tim.tv_nsec = loadManager::PARAMS().disturbingFactor * procedureProfPool::thePool_->nCells();
//		if (Pstream::myProcNo() == loadManager::PARAMS().disturbingProc && t == Operation::BUSY) {
//			nanosleep(&tim, NULL);
//		}
	}

}

/**
 * remove procedureProfPool.H header dependecy
 */
template<>
void Foam::procedureProfTrigger<Operation>::stop() {
	if (running_ && info_) {

		float hF=procedureProfPool::getThePoolInstance()->heterogeneityFactors[Pstream::myProcNo()];

		if (hF && info_->type_ == Operation::BUSY) {
			scalar opTime = clock_.elapsedTime() * 1000000000;

			struct timespec tim;
			tim.tv_sec = 0;
			tim.tv_nsec = hF * opTime;

			//if (Pstream::myProcNo() % 2 == 0) {

				nanosleep(&tim, NULL);

			//}
		}

		scalar elapsed = clock_.elapsedTime();

		//Pout << info_->description() << " " << elapsed * 100000 << endl;

		info_->update(elapsed);
		procedureProfPool::remove(*info_);
		running_ = false;
	}
}

template<>
void Foam::procedureProfTrigger<Section>::stop() {
	if (running_ && info_) {
		scalar elapsed = clock_.elapsedTime();
		info_->update(elapsed);
		procedureProfPool::remove(*info_);
		running_ = false;
	}
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

// ************************************************************************* //

// ************************************************************************* //

}
