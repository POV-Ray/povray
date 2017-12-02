//******************************************************************************
///
/// @file core/math/randomsequence.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifndef POVRAY_CORE_RANDOMSEQUENCE_H
#define POVRAY_CORE_RANDOMSEQUENCE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include <vector>

#include "core/math/vector.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreMathRandomsequence Random Number Sequences
/// @ingroup PovCoreMath
///
/// @{

using namespace pov_base;

vector<int> RandomInts(int minval, int maxval, size_t count);
vector<double> RandomDoubles(int minval, int maxval, size_t count);

DBL POV_rand(unsigned int& next_rand);

// need this to prevent VC++ v8 from thinking that Generator refers to boost::Generator
class Generator;

class RandomIntSequence
{
        friend class Generator;
    public:
        RandomIntSequence(int minval, int maxval, size_t count);

        int operator()(size_t seedindex);

        class Generator
        {
            public:
                Generator(RandomIntSequence *seq, size_t seedindex = 0);
                int operator()();
                int operator()(size_t seedindex);
                size_t GetSeed() const;
                void SetSeed(size_t seedindex);
            private:
                RandomIntSequence *sequence;
                size_t index;
        };
    private:
        vector<int> values;
};

class RandomDoubleSequence
{
        friend class Generator;
    public:
        RandomDoubleSequence(double minval, double maxval, size_t count);

        double operator()(size_t seedindex);

        class Generator
        {
            public:
                Generator(RandomDoubleSequence *seq, size_t seedindex = 0);
                double operator()();
                double operator()(size_t seedindex);
                size_t GetSeed() const;
                void SetSeed(size_t seedindex);
            private:
                RandomDoubleSequence *sequence;
                size_t index;
        };
    private:
        vector<double> values;
};


//******************************************************************************
///
/// @name Number Generator Classes
///
/// @{

/// Abstract class representing a generator for numbers that can be accessed sequentially.
template<class Type>
class SequentialNumberGenerator
{
    public:
        typedef Type result_type; // defined for compatibility with boost's NumberGenerator concept
        /// Returns the next number from the sequence.
        virtual Type operator()() = 0;
        /// Returns the next N numbers from the sequence.
        virtual shared_ptr<vector<Type> > GetSequence(size_t count)
        {
            shared_ptr<vector<Type> > data(new vector<Type>);
            data->reserve(count);
            for (size_t i = 0; i < count; i ++)
                data->push_back((*this)());
            return data;
        }
        /// Returns the number of values after which the generator must be expected to repeat (maximum size_t value if unknown or pretty huge).
        virtual size_t CycleLength() const = 0;
};

/// Abstract class representing a generator for numbers that can be accessed sequentially and depend on some seed.
template<class Type>
class SeedableNumberGenerator : public SequentialNumberGenerator<Type>
{
    public:
        /// Seeds the generator.
        virtual void Seed(size_t seed) = 0;
};

/// Abstract class representing a generator for numbers that can be accessed by index.
template<class Type>
class IndexedNumberGenerator
{
    public:
        /// Returns a particular number from the sequence.
        virtual Type operator[](size_t index) const = 0;
        /// Returns a particular subsequence from the sequence.
        virtual shared_ptr<vector<Type> > GetSequence(size_t index, size_t count) const
        {
            shared_ptr<vector<Type> > data(new vector<Type>);
            data->reserve(count);
            for (size_t i = 0; i < count; i ++)
                data->push_back((*this)[index + i]);
            return data;
        }
        /// Returns the maximum reasonable index.
        /// While larger indices are allowed, they may be mapped internally to lower ones.
        virtual size_t MaxIndex() const = 0;
};


/// @}
///
//******************************************************************************
///
/// @name Number Generator Pointers
///
/// The following types hold shared references to number generators.
///
/// @{

typedef shared_ptr<SequentialNumberGenerator<int> >         SequentialIntGeneratorPtr;
typedef shared_ptr<SequentialNumberGenerator<double> >      SequentialDoubleGeneratorPtr;
typedef shared_ptr<SequentialNumberGenerator<Vector3d> >    SequentialVectorGeneratorPtr;
typedef shared_ptr<SequentialNumberGenerator<Vector2d> >    SequentialVector2dGeneratorPtr;

typedef shared_ptr<SeedableNumberGenerator<int> >           SeedableIntGeneratorPtr;
typedef shared_ptr<SeedableNumberGenerator<double> >        SeedableDoubleGeneratorPtr;
typedef shared_ptr<SeedableNumberGenerator<Vector3d> >      SeedableVectorGeneratorPtr;
typedef shared_ptr<SeedableNumberGenerator<Vector2d> >      SeedableVector2dGeneratorPtr;

typedef shared_ptr<IndexedNumberGenerator<int> const>       IndexedIntGeneratorPtr;
typedef shared_ptr<IndexedNumberGenerator<double> const>    IndexedDoubleGeneratorPtr;
typedef shared_ptr<IndexedNumberGenerator<Vector3d> const>  IndexedVectorGeneratorPtr;
typedef shared_ptr<IndexedNumberGenerator<Vector2d> const>  IndexedVector2dGeneratorPtr;

/// @}
///
//******************************************************************************
///
/// @name Pseudo-Random Number Generator Factories
///
/// The following global functions provide sources for pseudo-random number sequences, that is, reproducible
/// sequences of numbers that are intended to appear non-correlated and... well, pretty random.
///
/// @note       For some purposes, sub-random number generators may be better suited.
///
/// @{

/// Gets a source for integer pseudo-random numbers satisfying the given properties.
/// The object returned is intended for sequential access.
///
/// @param[in]  minval          Lower bound of value interval (inclusive).
/// @param[in]  maxval          Upper bound of value interval (inclusive).
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
SeedableIntGeneratorPtr GetRandomIntGenerator(int minval, int maxval, size_t count);

/// Gets a source for floating-point pseudo-random numbers satisfying the given properties.
/// The object returned is intended for sequential access.
///
/// @param[in]  minval          Lower bound of value interval (inclusive).
/// @param[in]  maxval          Upper bound of value interval (inclusive).
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
SeedableDoubleGeneratorPtr GetRandomDoubleGenerator(double minval, double maxval, size_t count);

/// Gets a source for floating-point pseudo-random numbers satisfying the given properties.
/// The object returned is intended for sequential access.
///
/// @param[in]  minval          Lower bound of value interval (inclusive).
/// @param[in]  maxval          Upper bound of value interval (inclusive).
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
SequentialDoubleGeneratorPtr GetRandomDoubleGenerator(double minval, double maxval);

/// Gets a source for floating-point pseudo-random numbers satisfying the given properties.
/// The object returned is intended for access by index.
///
/// @param[in]  minval          Lower bound of value interval (inclusive).
/// @param[in]  maxval          Upper bound of value interval (inclusive).
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
IndexedDoubleGeneratorPtr GetIndexedRandomDoubleGenerator(double minval, double maxval, size_t count);

/// @}
///
//******************************************************************************
///
/// @name Sub-Random Number Generator Factories
///
/// The following global functions provide sources for low-discrepancy sequences (aka sub-random or quasi-random
/// number sequences - not to be confused with pseudo-random number sequences), that is, reproducible sequences
/// of values that cover an interval (or N-dimensional space) pretty uniformly, regardless how many consecutive
/// values are used from the sequence.
///
/// @{

/// Gets a source for sub-random (low discrepancy) floating-point numbers in the specified interval.
///
/// @param[in]  id              Selects one of multiple sources.
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
SequentialDoubleGeneratorPtr GetSubRandomDoubleGenerator(unsigned int id, double minval, double maxval, size_t count = 0);

/// Gets a source for cosine-weighted sub-random (low discrepancy) vectors on the unit hemisphere centered around +Y.
///
/// @note       If count is smaller than 1600, this function will return a generator for the hard-coded
///             radiosity sampling direction sequence used in POV-Ray v3.6.
///
/// @param[in]  id              Selects one of multiple sources.
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
SequentialVectorGeneratorPtr GetSubRandomCosWeightedDirectionGenerator(unsigned int id, size_t count = 0);

/// Gets a source for sub-random (low discrepancy) vectors on the unit sphere.
///
/// @param[in]  id              Selects one of multiple sources.
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
SequentialVectorGeneratorPtr GetSubRandomDirectionGenerator(unsigned int id, size_t count = 0);

/// Gets a source for sub-random (low discrepancy) 2D vectors on a disc.
///
/// @param[in]  id              Selects one of multiple sources.
/// @param[in]  radius          Radius of the disc.
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
SequentialVector2dGeneratorPtr GetSubRandomOnDiscGenerator(unsigned int id, double radius, size_t count = 0);

/// Gets a source for sub-random (low discrepancy) 2D vectors within a square.
///
/// @param[in]  id              Selects one of multiple sources.
/// @param[in]  minX            Lower bound of X coordinate.
/// @param[in]  maxX            Upper bound of X coordinate.
/// @param[in]  minY            Lower bound of Y coordinate.
/// @param[in]  maxY            Upper bound of Y coordinate.
/// @param[in]  count           Number of values to provide.
/// @return                     A shared pointer to a corresponding number generator.
///
SequentialVector2dGeneratorPtr GetSubRandom2dGenerator(unsigned int id, double minX, double maxX, double minY, double maxY, size_t count = 0);

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

}

#endif // POVRAY_CORE_RANDOMSEQUENCE_H
