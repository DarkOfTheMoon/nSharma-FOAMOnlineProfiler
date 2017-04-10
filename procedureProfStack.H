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
    Foam::procedureProfStack

Description
    Stack of the operationInfo-items that are currently used

SourceFiles
    procedureProfStack.C

\*---------------------------------------------------------------------------*/

#ifndef procedureProfStack_H
#define procedureProfStack_H

#include "LIFOStack.H"
#include "clockTime.H"
#include <map>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
    class procedureProfInfo;
    class procedureProfPool;
    class Ostream;

/*---------------------------------------------------------------------------*\
                           Class procedureProfStack Declaration
\*---------------------------------------------------------------------------*/

class procedureProfStack
:
    private LIFOStack<procedureProfInfo *>
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    procedureProfStack(const procedureProfStack&);

    //- Disallow default bitwise assignment
    void operator=(const procedureProfStack&);

    //- remember the timers for the correct stack-output

    //HashTable<clockTime *,label> timers_;
      std::map<label, clockTime *> timers_;

protected:
    void writeStackContents(Ostream &) const;

    void addTimer(const procedureProfInfo &info,clockTime &timer);
public:


    // Constructors

    //- Construct null
    procedureProfStack();

    // Destructor

    ~procedureProfStack();


    // Members that encapsulate the original stack-class
    procedureProfInfo &top() const;

    procedureProfInfo &bottom() const;

    bool empty() const;

    void push(procedureProfInfo &);

    procedureProfInfo &pop();

    friend class procedureProfPool;
    friend class loadManager;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
