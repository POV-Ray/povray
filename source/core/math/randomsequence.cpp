//******************************************************************************
///
/// @file core/math/randomsequence.cpp
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/math/randomsequence.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <limits>
#include <map>
#include <mutex>
#include <stdexcept>

// Boost header files
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

// POV-Ray header files (base module)
#include "base/povassert.h"

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/math/randcosweighted.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_base;

using boost::uniform_int;
using boost::uniform_real;
using boost::variate_generator;
using boost::mt19937;

using std::shared_ptr;
using std::vector;

#define PRIME_TABLE_COUNT 25
unsigned int primeTable[PRIME_TABLE_COUNT] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };



/*****************************************************************************
*
* FUNCTION
*
*   stream_rand
*
* INPUT
*
*   stream - number of random stream
*
* OUTPUT
*
* RETURNS
*
*   DBL - random value
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Standard pseudo-random function.
*
* CHANGES
*
*   Feb 1996 : Creation.
*   Mar 1996 : Return 2^32 random values instead of 2^16 [AED]
*
******************************************************************************/

DBL POV_rand(unsigned int& next_rand)
{
    next_rand = next_rand * 1812433253L + 12345L;

    return((DBL)(next_rand & 0xFFFFFFFFUL) / 0xFFFFFFFFUL);
}


/**********************************************************************************
 *  Legacy Code
 *********************************************************************************/

vector<int> RandomInts(int minval, int maxval, size_t count)
{
    mt19937 generator;
    uniform_int<int> distribution(minval, maxval);
    variate_generator<mt19937, uniform_int<int>> sequence(generator, distribution);
    vector<int> rands(count);

    for(size_t i = 0; i < count; i++)
        rands[i] = sequence();

    return rands;
}

vector<double> RandomDoubles(double minval, double maxval, size_t count)
{
    mt19937 generator;
    uniform_real<double> distribution(minval, maxval);
    variate_generator<mt19937, uniform_real<double>> sequence(generator, distribution);
    vector<double> rands(count);

    for(size_t i = 0; i < count; i++)
        rands[i] = sequence();

    return rands;
}

RandomIntSequence::RandomIntSequence(int minval, int maxval, size_t count) :
    values(RandomInts(minval, maxval, count))
{
}

RandomIntSequence::Generator::Generator(RandomIntSequence *seq, size_t seedindex) :
    sequence(seq),
    index(seedindex)
{
}

int RandomIntSequence::operator()(size_t seedindex)
{
    seedindex = seedindex % values.size();
    return values[seedindex];
}

int RandomIntSequence::Generator::operator()()
{
    index = (index + 1) % sequence->values.size();
    return (*sequence)(index);
}

int RandomIntSequence::Generator::operator()(size_t seedindex)
{
    return (*sequence)(seedindex);
}

size_t RandomIntSequence::Generator::GetSeed() const
{
    return index;
}

void RandomIntSequence::Generator::SetSeed(size_t seedindex)
{
    index = seedindex % sequence->values.size();
}

RandomDoubleSequence::RandomDoubleSequence(double minval, double maxval, size_t count) :
    values(RandomDoubles(minval, maxval, count))
{
}

RandomDoubleSequence::Generator::Generator(RandomDoubleSequence *seq, size_t seedindex) :
    sequence(seq),
    index(seedindex)
{
}

double RandomDoubleSequence::operator()(size_t seedindex)
{
    seedindex = seedindex % values.size();
    return values[seedindex];
}

double RandomDoubleSequence::Generator::operator()()
{
    index = (index + 1) % sequence->values.size();
    return (*sequence)(index);
}

double RandomDoubleSequence::Generator::operator()(size_t seedindex)
{
    return (*sequence)(seedindex);
}

size_t RandomDoubleSequence::Generator::GetSeed() const
{
    return index;
}

void RandomDoubleSequence::Generator::SetSeed(size_t seedindex)
{
    index = seedindex % sequence->values.size();
}


/**********************************************************************************
*  Random Distribution Functions
*********************************************************************************/


Vector2d Uniform2dOnSquare(SequentialDoubleGeneratorPtr source)
{
    double x = (*source)();
    double y = (*source)();
    return Vector2d(x, y);
}

Vector2d Uniform2dOnDisc(SequentialDoubleGeneratorPtr source)
{
    double r = sqrt((*source)());
    double theta = (*source)() * 2*M_PI;
    double x = r * cos(theta);
    double y = r * sin(theta);
    return Vector2d(x, y);
}

Vector3d Uniform3dOnSphere(SequentialDoubleGeneratorPtr source)
{
    double x = (*source)() * 2 - 1.0;
    double r = sqrt(1 - x*x);
    double theta = (*source)() * 2*M_PI;
    double y = r * cos(theta);
    double z = r * sin(theta);
    return Vector3d(x, y, z);
}

Vector3d CosWeighted3dOnHemisphere(SequentialDoubleGeneratorPtr source)
{
    Vector2d v = Uniform2dOnDisc(source);
    double y = sqrt (1 - v.lengthSqr());
    return Vector3d(v.x(), y, v.y());
}


/**********************************************************************************
 *  Local Types : Abstract Generators
 *********************************************************************************/

/**
 *  Abstract template class representing a generator for numbers that can be accessed both sequentially and by index.
 */
template<class Type>
class HybridNumberGenerator : public SeedableNumberGenerator<Type>, public IndexedNumberGenerator<Type>
{
    public:

        HybridNumberGenerator(size_t size = 0);
        virtual Type operator()() override;
        virtual shared_ptr<vector<Type>> GetSequence(size_t count) override;
        virtual size_t MaxIndex() const override;
        virtual size_t CycleLength() const override;
        virtual void Seed(size_t seed) override;

    protected:

        const size_t    size;
        size_t          index;
};


/**********************************************************************************
 *  Local Types : Linear Generators
 *********************************************************************************/

/**
 *  Template class representing a generator for uniformly distributed numbers in a given range.
 */
template<class Type, class BoostGenerator, class UniformType, size_t CYCLE_LENGTH = 0>
class UniformRandomNumberGenerator : public SeedableNumberGenerator<Type>
{
    public:

        struct ParameterStruct final
        {
            ParameterStruct(Type minval, Type maxval);
            Type minval, maxval;
            bool operator< (const ParameterStruct& other) const;
        };

        UniformRandomNumberGenerator(const ParameterStruct& param);
        UniformRandomNumberGenerator(Type minval, Type maxval);
        virtual Type operator()() override;
        virtual size_t CycleLength() const override;
        virtual void Seed(size_t seed) override;

    protected:
        variate_generator<BoostGenerator, UniformType> generator;
};

typedef UniformRandomNumberGenerator<int,    mt19937, uniform_int<int>>     Mt19937IntGenerator;
typedef UniformRandomNumberGenerator<double, mt19937, uniform_real<double>> Mt19937DoubleGenerator;

/**
 *  Generator for a 1-dimensional Halton sequence (aka van-der-Corput sequence).
 *  This class fulfills the boost UniformRandomNumberGenerator requirements,
 *  except that the numbers generated are actually sub-random.
 */
template<class Type>
class HaltonGenerator : public HybridNumberGenerator<Type>
{
    public:

        struct ParameterStruct {
            ParameterStruct(unsigned int base, Type minval, Type maxval);
            unsigned int base;
            Type minval, maxval;
            bool operator< (const ParameterStruct& other) const;
        };

        HaltonGenerator(const ParameterStruct& param);
        HaltonGenerator(unsigned int base, Type minval, Type maxval);
        /// Returns a particular number from the sequence.
        virtual double operator[](size_t index) const override;

    protected:

        unsigned int    base;
        Type            minval;
        Type            scale;
};

typedef HaltonGenerator<int>    HaltonIntGenerator;
typedef HaltonGenerator<double> HaltonDoubleGenerator;


/**********************************************************************************
 *  Local Types : Vector Generators
 *********************************************************************************/

/**
 *  Class generating a cosine-weighted hemispherical direction vector compatible with earlier POV-Ray versions.
 *  This class uses a 1600-element hard-coded directions originally used for radiosity.
 */
class LegacyCosWeightedDirectionGenerator : public HybridNumberGenerator<Vector3d>
{
    public:

        static const int NumEntries = kRandCosWeightedCount;

        struct ParameterStruct
        {
            bool operator< (const ParameterStruct& other) const;
        };

        LegacyCosWeightedDirectionGenerator(const ParameterStruct& dummy);
        virtual Vector3d operator[](size_t i) const override;
};


/**
 *  Abstract template class generating a vector based on a 2D Halton sequence.
 */
template<class Type, class TypeA, class TypeB = TypeA>
class Halton2dBasedGenerator : public HybridNumberGenerator<Type>
{
    public:

        struct ParameterStruct
        {
            ParameterStruct(unsigned int baseA, unsigned int baseB, TypeA minvalA, TypeA maxvalA, TypeB minvalB, TypeB maxvalB);
            unsigned int baseA, baseB;
            TypeA minvalA, maxvalA;
            TypeB minvalB, maxvalB;
            bool operator< (const ParameterStruct& other) const;
        };

        Halton2dBasedGenerator(const ParameterStruct& param);
        virtual Type operator[](size_t i) const override = 0;

    protected:

        shared_ptr<HaltonDoubleGenerator> generatorA;
        shared_ptr<HaltonDoubleGenerator> generatorB;
};

/**
 *  Class generating cosine-weighted hemispherical direction vectors, centered around the Y axis, based on a 2D Halton sequence.
 */
class HaltonCosWeightedDirectionGenerator : public Halton2dBasedGenerator<Vector3d, double>
{
    public:

        struct ParameterStruct : public Halton2dBasedGenerator<Vector3d, double>::ParameterStruct
        {
            ParameterStruct(unsigned int baseA, unsigned int baseB);
        };

        HaltonCosWeightedDirectionGenerator(const ParameterStruct& param);
        virtual Vector3d operator[](size_t i) const override;
};

/**
 *  Class generating uniformly distributed points within the unit circle based on a 2D Halton sequence.
 */
class HaltonOnDiscGenerator : public Halton2dBasedGenerator<Vector2d, double>
{
    public:

        struct ParameterStruct : public Halton2dBasedGenerator<Vector2d, double>::ParameterStruct
        {
            ParameterStruct(unsigned int baseA, unsigned int baseB, double radius);
        };

        HaltonOnDiscGenerator(const ParameterStruct& param);
        virtual Vector2d operator[](size_t i) const override;
};

/**
 *  Class generating uniformly distributed points on the unit sphere based on a 2D Halton sequence.
 */
class HaltonUniformDirectionGenerator : public Halton2dBasedGenerator<Vector3d, double>
{
    public:

        struct ParameterStruct : public Halton2dBasedGenerator<Vector3d, double>::ParameterStruct
        {
            ParameterStruct(unsigned int baseA, unsigned int baseB);
        };

        HaltonUniformDirectionGenerator(const ParameterStruct& param);
        virtual Vector3d operator[](size_t i) const override;
};

/**
 *  Class generating uniformly distributed points within a square based on a 2D Halton sequence.
 */
class Halton2dGenerator : public Halton2dBasedGenerator<Vector2d, double>
{
    public:
        Halton2dGenerator(const ParameterStruct& param);
        virtual Vector2d operator[](size_t i) const override;
};


/**********************************************************************************
 *  Local Types : Auxiliary
 *********************************************************************************/

/**
 *  Template class representing a factory for pre-computed number tables.
 */
template<class Type>
class NumberSequenceFactory
{
    public:

        /// Sets up the factory to use a given sequence.
        NumberSequenceFactory(shared_ptr<vector<Type> const> masterSequence);
        /// Sets up the factory to use a given number source.
        NumberSequenceFactory(shared_ptr<SequentialNumberGenerator<Type>> master);
        /// Sets up the factory to use a given number source, pre-computing a given number of elements.
        NumberSequenceFactory(shared_ptr<SequentialNumberGenerator<Type>> master, size_t count);
        /// Gets a reference to a table of pre-computed numbers having at least the given size.
        /// @note The vector returned may contain more elements than requested.
        shared_ptr<vector<Type> const> operator()(size_t count);

    protected:

        typedef SequentialNumberGenerator<Type> Generator;
        typedef shared_ptr<Generator>           GeneratorPtr;
        typedef vector<Type>                    Sequence;
        typedef shared_ptr<Sequence>            SequencePtr;
        typedef shared_ptr<Sequence const>      SequenceConstPtr;

        GeneratorPtr        master;
        SequenceConstPtr    masterSequence;
#if POV_MULTITHREADED
        std::mutex          masterMutex;
#endif
};

typedef NumberSequenceFactory<int>      IntSequenceFactory;
typedef NumberSequenceFactory<double>   DoubleSequenceFactory;
typedef NumberSequenceFactory<Vector3d> VectorSequenceFactory;


/**
 *  Template class representing a meta-factory for factories for pre-computed number tables.
 */
template<class ValueType, class GeneratorType>
class NumberSequenceMetaFactory
{
    public:

        static shared_ptr<NumberSequenceFactory<ValueType>> GetFactory(const typename GeneratorType::ParameterStruct& param);

    protected:

        typedef NumberSequenceFactory<ValueType>    Factory;
        typedef shared_ptr<Factory>                 FactoryPtr;
        typedef std::weak_ptr<Factory>              FactoryWeakPtr;
        typedef std::map<typename GeneratorType::ParameterStruct, FactoryWeakPtr> FactoryTable;

        static  FactoryTable*   lookupTable;
#if POV_MULTITHREADED
        static  std::mutex      lookupMutex;
#endif
};

typedef NumberSequenceMetaFactory<int,      Mt19937IntGenerator>                    Mt19937IntMetaFactory;
typedef NumberSequenceMetaFactory<double,   Mt19937DoubleGenerator>                 Mt19937DoubleMetaFactory;
typedef NumberSequenceMetaFactory<Vector3d, LegacyCosWeightedDirectionGenerator>    LegacyCosWeightedDirectionMetaFactory;
typedef NumberSequenceMetaFactory<Vector3d, HaltonCosWeightedDirectionGenerator>    HaltonCosWeightedDirectionMetaFactory;
typedef NumberSequenceMetaFactory<double,   HaltonDoubleGenerator>                  HaltonUniformDoubleMetaFactory;
typedef NumberSequenceMetaFactory<Vector3d, HaltonUniformDirectionGenerator>        HaltonUniformDirectionMetaFactory;
typedef NumberSequenceMetaFactory<Vector2d, HaltonOnDiscGenerator>                  HaltonOnDiscMetaFactory;
typedef NumberSequenceMetaFactory<Vector2d, Halton2dGenerator>                      Halton2dMetaFactory;


/**
 *  Template class representing a generator for pre-computed numbers using a shared values table.
 */
template<class Type>
class PrecomputedNumberGenerator : public HybridNumberGenerator<Type>
{
    public:

        /// Construct from a sequence factory.
        PrecomputedNumberGenerator(shared_ptr<NumberSequenceFactory<Type>> master, size_t size) :
            HybridNumberGenerator<Type>(size),
            values((*master)(size))
        {}

        /// Returns a particular number from the sequence.
        virtual Type operator[](size_t i) const override
        {
            // According to C++ standard, template classes cannot refer to parent template classes' members by unqualified name
            const size_t& size = HybridNumberGenerator<Type>::size;
            return (*values)[i % size];
        }
        /// Returns a particular subset from the sequence.
        virtual shared_ptr<vector<Type>> GetSequence(size_t index, size_t count) const override
        {
            // According to C++ standard, template classes cannot refer to parent template classes' members by unqualified name
            const size_t& size = HybridNumberGenerator<Type>::size;
            shared_ptr<vector<Type>> data(new vector<Type>);
            data->reserve(count);
            size_t i = index % size;
            while (count >= size - i) // handle wrap-around
            {
                data->insert(data->end(), values->begin() + i, values->begin() + size);
                count -= (size - i);
                i = 0;
            }
            data->insert(data->end(), values->begin() + i, values->begin() + i + count);
            return data;
        }

    protected:

        shared_ptr<vector<Type> const> values;
};

typedef PrecomputedNumberGenerator<int>         PrecomputedIntGenerator;
typedef PrecomputedNumberGenerator<double>      PrecomputedDoubleGenerator;
typedef PrecomputedNumberGenerator<Vector3d>    PrecomputedVectorGenerator;
typedef PrecomputedNumberGenerator<Vector2d>    PrecomputedVector2dGenerator;


/**********************************************************************************
 *  HybridNumberGenerator implementation
 *********************************************************************************/

template<class Type>
HybridNumberGenerator<Type>::HybridNumberGenerator(size_t size) :
    size(size),
    index(0)
{}

template<class Type>
Type HybridNumberGenerator<Type>::operator()()
{
    const Type& data = (*this)[index ++];
    if (size != 0)
        index = index % size;
    return data;
}

template<class Type>
shared_ptr<vector<Type>> HybridNumberGenerator<Type>::GetSequence(size_t count)
{
    shared_ptr<vector<Type>> data(IndexedNumberGenerator<Type>::GetSequence(index, count));
    index += count;
    if (size != 0)
        index = index % size;
    return data;
}

template<class Type>
size_t HybridNumberGenerator<Type>::MaxIndex() const
{
    return size - 1;
}

template<class Type>
size_t HybridNumberGenerator<Type>::CycleLength() const
{
    return size;
}

template<class Type>
void HybridNumberGenerator<Type>::Seed(size_t seed)
{
    index = seed % size;
}


/**********************************************************************************
 *  UniformRandomNumberGenerator implementation
 *********************************************************************************/

template<class Type, class BoostGenerator, class UniformType, size_t CYCLE_LENGTH>
UniformRandomNumberGenerator<Type,BoostGenerator,UniformType,CYCLE_LENGTH>::ParameterStruct::ParameterStruct(Type minval, Type maxval) :
    minval(minval), maxval(maxval)
{}

template<class Type, class BoostGenerator, class UniformType, size_t CYCLE_LENGTH>
bool UniformRandomNumberGenerator<Type,BoostGenerator,UniformType,CYCLE_LENGTH>::ParameterStruct::operator< (const ParameterStruct& other) const
{
    if (minval != other.minval)
        return (minval < other.minval);
    else
        return (maxval < other.maxval);
}

template<class Type, class BoostGenerator, class UniformType, size_t CYCLE_LENGTH>
UniformRandomNumberGenerator<Type,BoostGenerator,UniformType,CYCLE_LENGTH>::UniformRandomNumberGenerator(const ParameterStruct& param) :
    generator(BoostGenerator(), UniformType(param.minval, param.maxval))
{}

template<class Type, class BoostGenerator, class UniformType, size_t CYCLE_LENGTH>
UniformRandomNumberGenerator<Type,BoostGenerator,UniformType,CYCLE_LENGTH>::UniformRandomNumberGenerator(Type minval, Type maxval) :
    generator(BoostGenerator(), UniformType(minval, maxval))
{}

template<class Type, class BoostGenerator, class UniformType, size_t CYCLE_LENGTH>
Type UniformRandomNumberGenerator<Type,BoostGenerator,UniformType,CYCLE_LENGTH>::operator()()
{
    return generator();
}

template<class Type, class BoostGenerator, class UniformType, size_t CYCLE_LENGTH>
size_t UniformRandomNumberGenerator<Type,BoostGenerator,UniformType,CYCLE_LENGTH>::CycleLength() const
{
    if (CYCLE_LENGTH == 0)
        // SIZE_MAX is not mandatory in C++03, and std::numeric_limits<size_t>::max() can't be used
        // as a template parameter, so to indicate an "unknown or pretty huge" cycle length we're
        // using a template parameter value of 0 instead to convey that information.
        return std::numeric_limits<size_t>::max();
    else
        return CYCLE_LENGTH;
}

template<class Type, class BoostGenerator, class UniformType, size_t CYCLE_LENGTH>
void UniformRandomNumberGenerator<Type, BoostGenerator, UniformType, CYCLE_LENGTH>::Seed(size_t seed)
{
    generator.engine().seed((uint32_t)seed);
}


/**********************************************************************************
 *  HaltonGenerator implementation
 *********************************************************************************/

template<class Type>
HaltonGenerator<Type>::ParameterStruct::ParameterStruct(unsigned int base, Type minval, Type maxval) :
    base(base), minval(minval), maxval(maxval)
{}

template<class Type>
bool HaltonGenerator<Type>::ParameterStruct::operator< (const ParameterStruct& other) const
{
    if (base != other.base)
        return (base < other.base);
    else if (minval != other.minval)
        return (minval < other.minval);
    else
        return (maxval < other.maxval);
}

template<class Type>
HaltonGenerator<Type>::HaltonGenerator(const ParameterStruct& param) :
    base(param.base),
    minval(param.minval),
    scale(param.maxval-param.minval)
{
}

template<class Type>
HaltonGenerator<Type>::HaltonGenerator(unsigned int base, Type minval, Type maxval) :
    base(base),
    minval(minval),
    scale(maxval-minval)
{
}

template<class Type>
double HaltonGenerator<Type>::operator[](size_t index) const
{
    size_t i = 1 + index; // index starts at 0, but halton sequence as implemented here starts at 1

    double h = 0;
    double q = 1.0/base;
    unsigned int digit;

    while (i > 0)
    {
        digit = (unsigned int)(i % base);
        h = h + digit * q;
        i /= base;
        q /= base;
    }

    return minval + (Type)(h * scale);
}


/**********************************************************************************
 *  NumberSequenceFactory implementation
 *********************************************************************************/

template<class Type>
NumberSequenceFactory<Type>::NumberSequenceFactory(shared_ptr<vector<Type> const> masterSequence) :
    masterSequence(masterSequence)
{}

template<class Type>
NumberSequenceFactory<Type>::NumberSequenceFactory(shared_ptr<SequentialNumberGenerator<Type>> master) :
    master(master)
{}

template<class Type>
NumberSequenceFactory<Type>::NumberSequenceFactory(shared_ptr<SequentialNumberGenerator<Type>> master, size_t count) :
    master(master)
{
    (*this)(count); // force initial sequence to be generated
}

template<class Type>
shared_ptr<vector<Type> const> NumberSequenceFactory<Type>::operator()(size_t count)
{
#if POV_MULTITHREADED
    std::lock_guard<std::mutex> lock(masterMutex);
#endif
    if (!masterSequence)
    {
        // No values pre-computed yet; do it now.
        masterSequence = SequenceConstPtr(master->GetSequence(count));
    }
    else if ((masterSequence->size() < count) && master)
    {
        // Not enough values pre-computed; release the current values list and build a larger one.
        // NB: We're not simply appending to the current values list, because that might require re-allocation
        // and interfere with other threads trying to read from the list. To avoid having to synchronize
        // all read accesses, we're going for the less memory-efficient approach.
        size_t newCount = count;
        if (masterSequence->size() > newCount / 2)
        {
            // make sure to pre-compute at least twice the already-computed size
            if (masterSequence->size() > std::numeric_limits<size_t>::max() / 2) // play it safe (though that'll have us run out of memory anyway)
                newCount = std::numeric_limits<size_t>::max();
            else
                newCount = masterSequence->size() * 2;
        }
        // Pull more data from our master generator.
        // NB: We're using a temporary pointer to the new values list, so we can keep the master list const,
        // lest anyone might accidently modify it while other threads are reading it.
        SequenceConstPtr newSequence(master->GetSequence(newCount - masterSequence->size()));
        SequencePtr mergedSequence(new Sequence(*masterSequence));
        mergedSequence->insert(mergedSequence->end(), newSequence->begin(), newSequence->end());
        masterSequence = mergedSequence;
    }
    return masterSequence;
}


/**********************************************************************************
 *  NumberSequenceMetaFactory implementation
 *********************************************************************************/

template<class ValueType, class GeneratorType>
std::map<typename GeneratorType::ParameterStruct, std::weak_ptr<NumberSequenceFactory<ValueType>>>* NumberSequenceMetaFactory<ValueType, GeneratorType>::lookupTable;

#if POV_MULTITHREADED
template<class ValueType, class GeneratorType>
std::mutex NumberSequenceMetaFactory<ValueType, GeneratorType>::lookupMutex;
#endif

template<class ValueType, class GeneratorType>
shared_ptr<NumberSequenceFactory<ValueType>> NumberSequenceMetaFactory<ValueType, GeneratorType>::GetFactory(const typename GeneratorType::ParameterStruct& param)
{
#if POV_MULTITHREADED
    std::lock_guard<std::mutex> lock(lookupMutex);
#endif
    if (!lookupTable)
        lookupTable = new FactoryTable();
    FactoryPtr factory = (*lookupTable)[param].lock();
    if (!factory)
    {
        shared_ptr<GeneratorType> masterGenerator(new GeneratorType(param));
        factory = FactoryPtr(new Factory(shared_ptr<SequentialNumberGenerator<ValueType>>(masterGenerator)));
        (*lookupTable)[param] = factory;
    }
    return factory;
}


/**********************************************************************************
 *  LegacyCosWeightedDirectionGenerator implementation
 *********************************************************************************/

bool LegacyCosWeightedDirectionGenerator::ParameterStruct::operator< (const ParameterStruct& other) const
{
    return false; // all instances are equal
}

LegacyCosWeightedDirectionGenerator::LegacyCosWeightedDirectionGenerator(const ParameterStruct& dummy)
{}

Vector3d LegacyCosWeightedDirectionGenerator::operator[](size_t i) const
{
    Vector3d result;
    VUnpack(result, &(kaRandCosWeighted[i % NumEntries]));
    return result;
}


/**********************************************************************************
 *  Halton2dBasedGenerator implementation
 *********************************************************************************/

template<class Type, class TypeA, class TypeB>
Halton2dBasedGenerator<Type, TypeA, TypeB>::ParameterStruct::ParameterStruct(unsigned int baseA, unsigned int baseB, TypeA minvalA, TypeA maxvalA, TypeB minvalB, TypeB maxvalB) :
    baseA(baseA), baseB(baseB),
    minvalA(minvalA), maxvalA(maxvalA),
    minvalB(minvalB), maxvalB(maxvalB)
{}

template<class Type, class TypeA, class TypeB>
bool Halton2dBasedGenerator<Type, TypeA, TypeB>::ParameterStruct::operator< (const ParameterStruct& other) const
{
    if (baseA != other.baseA)
        return (baseA < other.baseA);
    else if (baseB != other.baseB)
        return (baseB < other.baseB);
    else if (minvalA != other.minvalA)
        return (minvalA < other.minvalA);
    else if (maxvalA != other.maxvalA)
        return (maxvalA < other.maxvalA);
    else if (minvalB != other.minvalB)
        return (minvalB < other.minvalB);
    else
        return (maxvalB < other.maxvalB);
}

template<class Type, class TypeA, class TypeB>
Halton2dBasedGenerator<Type, TypeA, TypeB>::Halton2dBasedGenerator(const ParameterStruct& param) :
    generatorA(new HaltonDoubleGenerator(param.baseA, param.minvalA, param.maxvalA)),
    generatorB(new HaltonDoubleGenerator(param.baseB, param.minvalB, param.maxvalB))
{}


/**********************************************************************************
 *  HaltonCosWeightedDirectionGenerator implementation
 *********************************************************************************/

HaltonCosWeightedDirectionGenerator::HaltonCosWeightedDirectionGenerator(const ParameterStruct& param) :
    Halton2dBasedGenerator<Vector3d,double,double>(param)
{}

HaltonCosWeightedDirectionGenerator::ParameterStruct::ParameterStruct(unsigned int baseA, unsigned int baseB) :
    Halton2dBasedGenerator<Vector3d,double,double>::ParameterStruct(baseA, baseB, 0.0, 1.0, 0.0, 2*M_PI)
{}

Vector3d HaltonCosWeightedDirectionGenerator::operator[](size_t i) const
{
    double r = sqrt((*generatorA)[i]);
    double theta = (*generatorB)[i];
    double x = r * cos(theta);
    double z = r * sin(theta);
    double y = sqrt (1 - x*x - z*z);
    return Vector3d(x, y, z);
}


/**********************************************************************************
 *  HaltonOnDiscGenerator implementation
 *********************************************************************************/

HaltonOnDiscGenerator::HaltonOnDiscGenerator(const ParameterStruct& param) :
    Halton2dBasedGenerator<Vector2d,double,double>(param)
{}

HaltonOnDiscGenerator::ParameterStruct::ParameterStruct(unsigned int baseA, unsigned int baseB, double radius) :
    Halton2dBasedGenerator<Vector2d,double,double>::ParameterStruct(baseA, baseB, 0.0, radius*radius, 0.0, 2*M_PI)
{}

Vector2d HaltonOnDiscGenerator::operator[](size_t i) const
{
    double r = sqrt((*generatorA)[i]);
    double theta = (*generatorB)[i];
    double x = r * cos(theta);
    double y = r * sin(theta);
    return Vector2d(x, y);
}


/**********************************************************************************
 *  Halton2dGenerator implementation
 *********************************************************************************/

Halton2dGenerator::Halton2dGenerator(const ParameterStruct& param) :
    Halton2dBasedGenerator<Vector2d,double,double>(param)
{}

Vector2d Halton2dGenerator::operator[](size_t i) const
{
    double x = (*generatorA)[i];
    double y = (*generatorB)[i];
    return Vector2d(x, y);
}


/**********************************************************************************
 *  HaltonUniformDirectionGenerator implementation
 *********************************************************************************/

HaltonUniformDirectionGenerator::HaltonUniformDirectionGenerator(const ParameterStruct& param) :
    Halton2dBasedGenerator<Vector3d,double,double>(param)
{}

HaltonUniformDirectionGenerator::ParameterStruct::ParameterStruct(unsigned int baseA, unsigned int baseB) :
    Halton2dBasedGenerator<Vector3d,double,double>::ParameterStruct(baseA, baseB, -1.0, 1.0, 0.0, 2*M_PI)
{}

Vector3d HaltonUniformDirectionGenerator::operator[](size_t i) const
{
    double x = (*generatorA)[i];
    double r = sqrt(1 - x*x);
    double theta = (*generatorB)[i];
    double y = r * cos(theta);
    double z = r * sin(theta);
    return Vector3d(x, y, z);
}


/**********************************************************************************
 *  Factory Functions
 *********************************************************************************/

SeedableIntGeneratorPtr GetRandomIntGenerator(int minval, int maxval, size_t count)
{
    POV_RANDOMSEQUENCE_ASSERT(count > 0);
    Mt19937IntGenerator::ParameterStruct param(minval, maxval);
    shared_ptr<NumberSequenceFactory<int>> factory = Mt19937IntMetaFactory::GetFactory(param);
    SeedableIntGeneratorPtr generator(new PrecomputedIntGenerator(factory, count));
    (void)(*generator)(); // legacy fix
    return generator;
}

SeedableDoubleGeneratorPtr GetRandomDoubleGenerator(double minval, double maxval, size_t count)
{
    POV_RANDOMSEQUENCE_ASSERT(count > 0);
    Mt19937DoubleGenerator::ParameterStruct param(minval, maxval);
    shared_ptr<NumberSequenceFactory<double>> factory(Mt19937DoubleMetaFactory::GetFactory(param));
    SeedableDoubleGeneratorPtr generator(new PrecomputedDoubleGenerator(factory, count));
    (void)(*generator)(); // legacy fix
    return generator;
}

SeedableDoubleGeneratorPtr GetRandomDoubleGenerator(double minval, double maxval)
{
    Mt19937DoubleGenerator::ParameterStruct param(minval, maxval);
    SeedableDoubleGeneratorPtr generator(new Mt19937DoubleGenerator(param));
    (void)(*generator)(); // legacy fix
    return generator;
}

IndexedDoubleGeneratorPtr GetIndexedRandomDoubleGenerator(double minval, double maxval, size_t count)
{
    POV_RANDOMSEQUENCE_ASSERT(count > 0);
    Mt19937DoubleGenerator::ParameterStruct param(minval, maxval);
    shared_ptr<NumberSequenceFactory<double>> factory(Mt19937DoubleMetaFactory::GetFactory(param));
    return IndexedDoubleGeneratorPtr(new PrecomputedDoubleGenerator(factory, count));
}

SequentialVectorGeneratorPtr GetSubRandomCosWeightedDirectionGenerator(unsigned int id, size_t count)
{
    if ((id == 0) && count && (count < LegacyCosWeightedDirectionGenerator::NumEntries))
    {
        LegacyCosWeightedDirectionGenerator::ParameterStruct param;
        shared_ptr<NumberSequenceFactory<Vector3d>> factory(LegacyCosWeightedDirectionMetaFactory::GetFactory(param));
        return SequentialVectorGeneratorPtr(new PrecomputedVectorGenerator(factory, count));
    }
    else
    {
        HaltonCosWeightedDirectionGenerator::ParameterStruct param(primeTable[id % PRIME_TABLE_COUNT], primeTable[(id+1) % PRIME_TABLE_COUNT]);
        if (count)
        {
            shared_ptr<NumberSequenceFactory<Vector3d>> factory(HaltonCosWeightedDirectionMetaFactory::GetFactory(param));
            return SequentialVectorGeneratorPtr(new PrecomputedVectorGenerator(factory, count));
        }
        else
            return SequentialVectorGeneratorPtr(new HaltonCosWeightedDirectionGenerator(param));
    }
}

SequentialDoubleGeneratorPtr GetSubRandomDoubleGenerator(unsigned int id, double minval, double maxval, size_t count)
{
    HaltonDoubleGenerator::ParameterStruct param(primeTable[id % PRIME_TABLE_COUNT], minval, maxval);
    if (count)
    {
        shared_ptr<NumberSequenceFactory<double>> factory(HaltonUniformDoubleMetaFactory::GetFactory(param));
        return SequentialDoubleGeneratorPtr(new PrecomputedDoubleGenerator(factory, count));
    }
    else
        return SequentialDoubleGeneratorPtr(new HaltonDoubleGenerator(param));
}

SequentialVectorGeneratorPtr GetSubRandomDirectionGenerator(unsigned int id, size_t count)
{
    HaltonUniformDirectionGenerator::ParameterStruct param(primeTable[id % PRIME_TABLE_COUNT], primeTable[(id+1) % PRIME_TABLE_COUNT]);
    if (count)
    {
        shared_ptr<NumberSequenceFactory<Vector3d>> factory(HaltonUniformDirectionMetaFactory::GetFactory(param));
        return SequentialVectorGeneratorPtr(new PrecomputedVectorGenerator(factory, count));
    }
    else
        return SequentialVectorGeneratorPtr(new HaltonUniformDirectionGenerator(param));
}

SequentialVector2dGeneratorPtr GetSubRandomOnDiscGenerator(unsigned int id, double radius, size_t count)
{
    HaltonOnDiscGenerator::ParameterStruct param(primeTable[id % PRIME_TABLE_COUNT], primeTable[(id+1) % PRIME_TABLE_COUNT], radius);
    if (count)
    {
        shared_ptr<NumberSequenceFactory<Vector2d>> factory(HaltonOnDiscMetaFactory::GetFactory(param));
        return SequentialVector2dGeneratorPtr(new PrecomputedVector2dGenerator(factory, count));
    }
    else
        return SequentialVector2dGeneratorPtr(new HaltonOnDiscGenerator(param));
}

SequentialVector2dGeneratorPtr GetSubRandom2dGenerator(unsigned int id, double minX, double maxX, double minY, double maxY, size_t count)
{
    Halton2dGenerator::ParameterStruct param(primeTable[id % PRIME_TABLE_COUNT], primeTable[(id+1) % PRIME_TABLE_COUNT], minX, maxX, minY, maxY);
    if (count)
    {
        shared_ptr<NumberSequenceFactory<Vector2d>> factory(Halton2dMetaFactory::GetFactory(param));
        return SequentialVector2dGeneratorPtr(new PrecomputedVector2dGenerator(factory, count));
    }
    else
        return SequentialVector2dGeneratorPtr(new Halton2dGenerator(param));
}

}
// end of namespace pov
