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

Description
    Add everything necessary for profiling plus a macro

    Originally proposed in
    http://www.cfd-online.com/Forums/openfoam-bugs/64081-feature-proposal-application-level-profiling.html

SourceFiles

\*---------------------------------------------------------------------------*/

#ifndef loadManagerProf_H
#define loadManagerProf_H

#include "procedureProfTrigger.H"

// to be used at the beginning of a  section to be profiled
// operation ends automatically at the end of a block
#define ADD_BUSY_OPERATION(name) Foam::procedureProfTrigger<Operation> \
		procedureProfTriggerFor##name (#name, Operation::BUSY)


#define ADD_IDLE_OPERATION(name) Foam::procedureProfTrigger<Operation> \
		procedureProfTriggerFor##name (#name, Operation::IDLE)

// Use this if a description with spaces, colons etc should be added
//#define ADD_OPERATION2(name,descr) Foam::procedureProfTrigger procedureProfTriggerFor##name (descr, OPERATION)

// this is only needed if operation should end before the end of a block
#define END_OPERATION(name) procedureProfTriggerFor##name.stop()


// to be used at the beginning of a section to be profiled
// operation ends automatically at the end of a block
#define ADD_SECTION(name) Foam::procedureProfTrigger<Section> procedureProfTriggerFor##name (#name)

// Use this if a description with spaces, colons etc should be added
//#define ADD_SECTION2(name,descr) Foam::procedureProfTrigger procedureProfTriggerFor##name (descr, SECTION)

// this is only needed if operation should end before the end of a block
#define END_SECTION(name) procedureProfTriggerFor##name.stop()

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
