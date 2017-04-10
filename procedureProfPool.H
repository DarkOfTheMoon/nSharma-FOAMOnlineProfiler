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
 Foam::procedureProfPool

 Description
 Collects all the data for operation

 SourceFiles
 procedureProfPool.C

 \*---------------------------------------------------------------------------*/

#ifndef procedureProfPool_H
#define procedureProfPool_H

#include "clockTime.H"

#include <map>
#include <list>
#include "OPstream.H"

#include "procedureProfInfo.H"
#include "procedureProfStack.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam {

// Forward declaration of classes
// class Istream;
class Ostream;
class fvMesh;
class Time;

// // Forward declaration of friend functions and operators
// Istream& operator>>(Istream&, procedureProfPool&);
// Ostream& operator<<(Ostream&, const procedureProfPool&);

/*---------------------------------------------------------------------------*\
                           Class procedureProfPool Declaration
 \*---------------------------------------------------------------------------*/

class procedureProfPool {

	friend class loadManager;
	template<class T> friend class procedureProfTrigger;
	friend class Operation;
	friend class Section;

	// Private typedefs

public:
	typedef std::multimap<Foam::string, Foam::procedureProfInfo*> mapType;
	typedef std::pair<Foam::string, Foam::procedureProfInfo*> mapValues;
	typedef mapType::iterator mapIterator;
	typedef mapType::const_iterator mapConstIterator;

private:


	// Private data

	mapType allInfo_;

	procedureProfStack theStack_;

	clockTime globalTime_;

	/**
	 * This should be in loadManager. The reason why it's here
	 * is to remove the dependency from loadManager because these
	 * factors are used in triggers.
	 */
	List<float> heterogeneityFactors;

	// Private Member Functions

	//- Disallow default bitwise copy construct
	procedureProfPool(const procedureProfPool&);

	//- Disallow default bitwise assignment
	void operator=(const procedureProfPool&);

	// Static data members

	//- The only possible Pool-Object
	static procedureProfPool *thePool_;

	fvMesh& mesh_;
	Time& runTime_;

	bool enabled;

	scalar totalIdleTime_;
	scalar totalBusyTime_;

	/**
	 * specifies which section will be used to measure the low
	 * level operations. Only one at the time. And one instance per proc
	 */
	class targetWorkloadSection {

		string name_;
		scalar accIdleTime_;
		scalar accBusyTime_;
		scalar totalTime_;

		//if we do time resets we need a separate timer
		// in order to busy and idle %'s make sense
		clockTime timer_;

		Section* section_ptr_;

	public:

		string& name() {
			return name_;

		}

		Section* section_ptr() {
			return section_ptr_;

		}

		targetWorkloadSection(string sectionT) {
			name_ = sectionT;
			accIdleTime_ = 0;
			accBusyTime_ = 0;
			totalTime_ = 0;
			section_ptr_ = NULL;
			timer_ = clockTime();
		}

		targetWorkloadSection(string sectionT, Section *p) {
			name_ = sectionT;
			accIdleTime_ = 0;
			accBusyTime_ = 0;
			totalTime_ = 0;
			section_ptr_ = p;
			timer_ = clockTime();
		}

		void startTimer() {
			timer_ = clockTime();
			//totalTime_ = 0;
		}

		void updateTimers(Operation::opType t, scalar elapsedTimee) {
			if (t == Operation::BUSY) {
				accBusyTime_ += elapsedTimee;
			} else if (t == Operation::IDLE)
				accIdleTime_ += elapsedTimee;
		}
		void updateTotal() {
			totalTime_ += timer_.elapsedTime();
		}

		void setPointer(Section* p) {
			section_ptr_ = p;
		}

		void getPercentages(scalar busy_idle[]) {

			scalar targetSectionElapsed = 0;

			if (section_ptr_->onStack_) {
				updateTotal();
				startTimer();
				targetSectionElapsed = totalTime_;
			} else {
				targetSectionElapsed = totalTime_;
			}

//			Pout << "targetSectionElapsed - "
//					<< totalTime_
//					<<  " - "
//					<< accIdleTime_
//					<< " - "
//					<< accBusyTime_
//					<< " - "
//					<< endl;

			//based on IDLE , BUSY and target section, calculate busy and idle %'s
			if (targetSectionElapsed != 0.f) {

				scalar ratio = accBusyTime_ / targetSectionElapsed;

				busy_idle[0] = ratio * 100.0f;

				ratio = accIdleTime_ / targetSectionElapsed;

				busy_idle[1] = ratio * 100.0f;

				busy_idle[2] = accBusyTime_;

				busy_idle[3] = accIdleTime_;

			} else {
				busy_idle[0] = 0.0f;
				busy_idle[1] = 0.0f;
				busy_idle[2] = 0.0f;
				busy_idle[3] = 0.0f;
			}
		}

	};

	targetWorkloadSection targetSection_;

	// Constructors

	//- Construct null
	procedureProfPool(Time& runTime, fvMesh& mesh, string sectionT);

	//- Destructor
	virtual ~procedureProfPool();

	// Member functions


	procedureProfStack& stack() {
		return theStack_;
	}

	const procedureProfStack& stack() const {
		return theStack_;
	}

	void setEnable(bool v);

public:

	mapType& map() {
			return allInfo_;
		}

		const mapType& map() const {
			return allInfo_;
		}


	void updateTimers(Operation::opType t, scalar elapsedTimee) {

		procedureProfPool::thePool_->getTargetWorkloadSection().updateTimers(
				t, elapsedTimee);

		if (t == Operation::BUSY) {
			totalBusyTime_ += elapsedTimee;
		} else if (t == Operation::IDLE)
			totalIdleTime_ += elapsedTimee;
	}

	static procedureProfPool* getThePoolInstance();
	static fvMesh& getMesh();
	static Time& getRunTime();

	//label nCells();

	void getOpsIDsOfType(std::list<Operation*>& pw);

	//string getTargetWorkloadSectionName();

	//static void setTargetWorkloadSectionName(string name);

	static targetWorkloadSection& getTargetWorkloadSection();

	bool isEnabled();

	/**
	 * Sets "full" as main top section and enbables it as sectionlock
	 */
	static void initProfiling(Time& runTime,fvMesh& mesh, string sectionT =
			PROF_INFO_MAIN_SECTION_NAME);

//	/**
//	 * Checks if trigger is eliguble based on sectionlock name. Searchs
//	 * from stack top for the parent with sectionlock namem if found trigger is
//	 * commited
//	 */
//	static bool isTriggerEligible(const string &name, infoType);

	template<class T>
	static T& getInfo(const string& name);

	static void remove(const procedureProfInfo& info);

	static void rememberTimer(const procedureProfInfo& info, clockTime& timer);

	virtual bool writeData(Ostream&) const;
};

template<class T>
T& Foam::procedureProfPool::getInfo(const string& name) {

	if (!thePool_) {
		FatalErrorIn("procedureProfPool::addInfo(const string& name)")
				<< "Singleton not initialized\n" << endl << abort(FatalError);
	}

	procedureProfStack& stack = thePool_->stack();
	mapType& map = thePool_->map();

	procedureProfInfo* found = NULL;

	for (mapIterator it = map.lower_bound(name); it != map.upper_bound(name);
			++it) {
		if (it->second->parent().id() == stack.top().id()) {
			found = it->second;
			break;
		}
	}

	if (!found) {
		found = new T(stack.top(), name);
		map.insert(make_pair(name, found));
	}

	stack.push(*found);
	return *((T*) found);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}// End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
