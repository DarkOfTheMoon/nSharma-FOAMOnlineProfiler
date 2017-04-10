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
 Foam::procedureProfInfo

 Description
 Information needed for operation

 SourceFiles
 procedureProfInfo.C

 \*---------------------------------------------------------------------------*/

#ifndef procedureProfInfo_H
#define procedureProfInfo_H

#include "label.H"
#include "scalar.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
namespace Foam {

// Forward declaration of classes

// class Istream;
class Ostream;
class procedureProfStack;
class procedureProfPool;
class procedureProfInfo;
class loadManager;
class MPIfiedProcedure;

#define PROF_INFO_MAIN_SECTION_NAME "full"

Ostream& operator<<(Ostream&, const procedureProfInfo&);

/*---------------------------------------------------------------------------*\
                           Class procedureProfInfo Declaration
 \*---------------------------------------------------------------------------*/

class procedureProfInfo {

	friend class loadManager;

	// Private data

	// nr of times this was called
	label calls_;

	// total time spent
	scalar totalTime_;

	// time spent in children
	scalar childTime_;

	// unique id to identify it
	label id_;

	// pointer to the parent object (if there is any)
	procedureProfInfo &parent_;

	// what this does
	string description_;

	// is this currently on the stack?
	bool onStack_;

	// Private Member Functions

	//- Disallow default bitwise copy construct
	procedureProfInfo(const procedureProfInfo&);

	//- Disallow default bitwise assignment
	void operator=(const procedureProfInfo&);

	// Static data members

	//- Counter that is used to generate the ids
	static label nextId_;

	//- get a new ID and update the counter
	static label getID();

	//- raise the next possible ID (to avoid ID-clashes during reading)
	void raiseID(label maxVal);

	void addedToStack() {
		onStack_ = true;
	}

	void removedFromStack() {
		onStack_ = false;
	}

	void writeWithOffset(Ostream &os, bool offset = false, scalar time = 0,
			scalar childTime = 0) const;

public:

	//- Construct null - only the master-element
	procedureProfInfo();

	// Constructors

	//- Construct from components
	procedureProfInfo(procedureProfInfo &parent, const string &descr);

	//procedureProfInfo(procedureProfInfo &parent,const string &descr, infoType t);

//     //- Construct from Istream
//     procedureProfInfo(Istream&);

// Destructor

	virtual ~procedureProfInfo();

	// Member Functions

	// Access

	label id() const {
		return id_;
	}

	// - 1 because info id starts at 1
	label id0() const {
		return id_ - 1;
	}

	label calls() const {
		return calls_;
	}

	scalar totalTime() const {
		return totalTime_;
	}

	scalar childTime() const {
		return childTime_;
	}

	bool onStack() const {
		return onStack_;
	}

	const string &description() const {
		return description_;
	}

	const procedureProfInfo &parent() const {
		return parent_;
	}

	virtual bool isOperation()=0;

	virtual void MPIfy(MPIfiedProcedure& o)=0;

	//- Update it with a new timing information
	void update(scalar elapsedTime);

	friend class procedureProfStack;
	friend class procedureProfPool;
	friend class Operation;

	// IOstream Operators

	//        friend Istream& operator>>(Istream&, procedureProfInfo&);
	friend Ostream& operator<<(Ostream&, const procedureProfInfo&);
};

/*
 * a SECTION represents a block of code usally associated with a big task
 * It has lower level OPERATIONs which are measured or not , depending
 * on sectionlock. Only one section is allowed to measure operations
 * An OPERATION should either do only work without syncronization or
 * only comms/sync. A comm/sync operation shoulb be seen as idle time.
 */

class Section: public procedureProfInfo {
public:

	Section(procedureProfInfo &parent, const string &descr);

	Section() ;

	~Section();

	//- Update it with a new timing information
	void update(scalar elapsedTime);

	virtual void MPIfy(MPIfiedProcedure& o);

	bool isOperation() {
		return false;
	}

};

class Operation: public procedureProfInfo {
public:

	enum opType {
		IDLE, BUSY, NA
	};

	opType type_;

	Operation(procedureProfInfo &parent, const string &descr) ;

	~Operation();

	virtual void MPIfy(MPIfiedProcedure& o);

	bool isOperation() {
		return true;
	}

	//- Update it with a new timing information
	void update(scalar elapsedTime);

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}// End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
