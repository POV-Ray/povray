//******************************************************************************
///
/// @file base/image/dither.cpp
///
/// Implementations related to image dithering.
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
#include "base/image/dither.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/povassert.h"
#include "base/data/bluenoise64a.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

//*******************************************************************************

void NoDither::GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt)
{
    offLin.clear();
    offQnt.clear();
}

//*******************************************************************************

class OrderedDither::Pattern final
{
public:
    template<typename T> Pattern(unsigned int size, const T* raw, unsigned int rank);
    template<typename T> Pattern(unsigned int size, const T* raw);
    Pattern(std::initializer_list<std::initializer_list<int>> raw);
    ~Pattern();
    inline unsigned int Size() const { return mSize; }
    inline const ColourChannel& operator[](size_t i) const { return maData[i]; }
    const ColourChannel& operator()(unsigned int x, unsigned int y) const;
private:
    unsigned int mSize;
    ColourChannel* maData;
};

template<typename T>
OrderedDither::Pattern::Pattern(unsigned int size, const T* raw, unsigned int rank) :
    mSize(size),
    maData(nullptr)
{
    auto flatSize = size_t(mSize)*size_t(mSize);
    maData = new ColourChannel[flatSize];
    ColourChannel invRank = 1.0 / rank;
    for (unsigned int i = 0; i < flatSize; ++i)
    {
        POV_ASSERT(raw[i] >= 0);
        POV_ASSERT(raw[i] < rank);
        maData[i] = ColourChannel(raw[i] + 0.5) * invRank - 0.5;
    }
}

template<typename T>
OrderedDither::Pattern::Pattern(unsigned int size, const T* raw) :
    Pattern(size, raw, size_t(size)*size_t(size))
{}

OrderedDither::Pattern::Pattern(std::initializer_list<std::initializer_list<int>> raw) :
    mSize(raw.size()),
    maData(nullptr)
{
    auto flatSize = size_t(mSize)*size_t(mSize);
    maData = new ColourChannel[flatSize];
    auto rank = flatSize;
    ColourChannel invRank = 1.0 / rank;
    ColourChannel* pCoeff = maData;
    for (auto&& rawRow : raw)
    {
        POV_ASSERT(rawRow.size() == mSize);
        for (auto&& rawCoeff : rawRow)
        {
            POV_ASSERT(rawCoeff < rank);
            *(pCoeff++) = ColourChannel(rawCoeff + 0.5) * invRank - 0.5;
        }
    }
}

OrderedDither::Pattern::~Pattern()
{
    if (maData != nullptr)
        delete[] maData;
}

const ColourChannel& OrderedDither::Pattern::operator()(unsigned int x, unsigned int y) const
{
    return (*this)[wrap(y, mSize) * mSize + wrap(x, mSize)];
}

//------------------------------------------------------------------------------

OrderedDither::OrderedDither(const Pattern& pattern, unsigned int width, bool invertRB) :
    mPattern(pattern),
    mImageWidth(width),
    mInvertRB(invertRB)
{}

void OrderedDither::GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt)
{
    offLin.clear();
    ColourChannel off = mPattern(x, y);
    offQnt.red   = (mInvertRB ? -off : off);
    offQnt.green = off;
    offQnt.blue  = (mInvertRB ? -off : off);
    offQnt.alpha = off;
}

//*******************************************************************************

void DiffusionDither1D::GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt)
{
    if (x == 0)
        offLin.clear();
    else
        offLin = lastErr;
    offQnt.clear();
}

void DiffusionDither1D::SetError(unsigned int x, unsigned int y, const ColourOffset& err)
{
    lastErr = err;
}

//*******************************************************************************

SierraLiteDither::SierraLiteDither(unsigned int width) :
    imageWidth(width),
    maErr(new ColourOffset[width+2])
{}

SierraLiteDither::~SierraLiteDither()
{
    delete[] maErr;
}

void SierraLiteDither::GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt)
{
    offLin = maErr[x+1];
    offQnt.clear();
}

void SierraLiteDither::SetError(unsigned int x, unsigned int y, const ColourOffset& err)
{
    // NB: We're storing the propagated error for both the current and the next
    // line in a single-line buffer, using the following scheme upon entering
    // this function:
    //
    //     | Offset to x    |-1 | 0 | 1 | 2 | 3 |
    //     |----------------|---|---|---|---|---|
    //     | Current row    |   |   |(B)| B | B |
    //     | Next row       | B | B |   |   |   |
    //
    // and the following scheme upon leaving this function:
    //
    //     | Offset to x    |-1 | 0 | 1 | 2 | 3 |
    //     |----------------|---|---|---|---|---|
    //     | Current row    |   |   |( )| B | B |
    //     | Next row       | B | B | B |   |   |
    //
    // where "()" marks the current pixel, and "B" indicates that the error is
    // stored in the buffer at the corresponding offset. Empty cells indicate
    // that the corresponding error is not stored anywhere.

    maErr[x+2] += err * (2/4.0); // pixel to the right
    maErr[x]   += err * (1/4.0); // pixel below left
    maErr[x+1]  = err * (1/4.0); // pixel below (overwritten instead of added to)
}

//*******************************************************************************

FloydSteinbergDither::FloydSteinbergDither(unsigned int width) :
    imageWidth(width),
    maErr(new ColourOffset[width+2])
{}

FloydSteinbergDither::~FloydSteinbergDither()
{
    delete[] maErr;
}

void FloydSteinbergDither::GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt)
{
    if (x == 0)
        mErrX.clear();
    offLin = maErr[x+1];
    offQnt.clear();
}

void FloydSteinbergDither::SetError(unsigned int x, unsigned int y, const ColourOffset& err)
{
    // NB: We're storing the propagated error for both the current and the next
    // line in a single-line buffer, using the following scheme upon entering
    // this function:
    //
    //     | Offset to x    |-1 | 0 | 1 | 2 | 3 |
    //     |----------------|---|---|---|---|---|
    //     | Current row    |   |   |(B)| B | B |
    //     | Next row       | B | B | X |   |   |
    //
    // and the following scheme upon leaving this function:
    //
    //     | Offset to x    |-1 | 0 | 1 | 2 | 3 |
    //     |----------------|---|---|---|---|---|
    //     | Current row    |   |   |( )| B | B |
    //     | Next row       | B | B | B | X |   |
    //
    // where "()" marks the current pixel, "B" indicates that the error is
    // stored in the buffer at the corresponding offset, and "X" indicates that
    // the error is stored in `mErrX`. Empty cells indicate that the
    // corresponding error is not stored anywhere.

    maErr[x+1] = mErrX;
    maErr[x+2] += err * (7/16.0); // pixel to the right
    maErr[x]   += err * (3/16.0); // pixel below left
    maErr[x+1] += err * (5/16.0); // pixel below
    mErrX       = err * (1/16.0); // pixel below right (overwritten instead of added to)
}

//*******************************************************************************

class DiffusionDither::Filter final
{
public:
    Filter(std::initializer_list<std::initializer_list<int>> raw, int drop = 0);
    ~Filter();
    inline int Rows() const { return mRows; }
    inline int Cols() const { return mCols; }
    inline int ColX() const { return mColX; }
    inline const ColourChannel& operator[](unsigned int i) const { return maData[i]; }
private:
    unsigned int mRows;
    unsigned int mCols;
    unsigned int mColX;
    ColourChannel* maData;
};

DiffusionDither::Filter::Filter(std::initializer_list<std::initializer_list<int>> raw, int drop) :
    mRows(raw.size()),
    mCols((raw.end() - 1)->size()),
    mColX(mCols - raw.begin()->size() - 1),
    maData(new ColourChannel[mRows * mCols - mColX - 1])
{
    POV_ASSERT(mRows > 0);
    for (auto&& rawRow : raw)
        POV_ASSERT((&rawRow == raw.begin()) || (rawRow.size() == mCols));

    int sum = drop;
    for (auto&& rawRow : raw)
        for (auto&& rawCoeff : rawRow)
            sum += rawCoeff;
    POV_ASSERT(sum > 0);
    ColourChannel invSum = 1.0 / sum;
    ColourChannel* pCoeff = maData;
    for (auto&& rawRow : raw)
        for (auto&& rawCoeff : rawRow)
            *(pCoeff++) = ColourChannel(rawCoeff) * invSum;
}

DiffusionDither::Filter::~Filter()
{
    if (maData != nullptr)
        delete[] maData;
}

//------------------------------------------------------------------------------

DiffusionDither::DiffusionDither(const Filter& matrix, unsigned int width) :
    mMatrix(matrix),
    mImageWidth(width),
    maaErrorBuffer(new ColourOffset*[matrix.Rows()])
{
    for (unsigned int i = 0; i < matrix.Rows(); ++i)
        maaErrorBuffer[i] = new ColourOffset[width + matrix.Cols() - 1];
}

DiffusionDither::~DiffusionDither()
{
    for (unsigned int i = 0; i < mMatrix.Rows(); ++i)
        delete[] maaErrorBuffer[i];
    delete[] maaErrorBuffer;
}

void DiffusionDither::GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt)
{
    if (x == 0)
    {
        ColourOffset* tmp = maaErrorBuffer[0];
        for (unsigned int i = 1; i < mMatrix.Rows(); ++i)
            maaErrorBuffer[i - 1] = maaErrorBuffer[i];
        maaErrorBuffer[mMatrix.Rows() - 1] = tmp;
        for (unsigned int i = 0; i < mImageWidth + mMatrix.Cols() - 1; ++i)
            maaErrorBuffer[mMatrix.Rows() - 1][i].clear();
    }
    offLin = maaErrorBuffer[0][x + mMatrix.ColX()];
    offQnt.clear();
}

void DiffusionDither::SetError(unsigned int x, unsigned int y, const ColourOffset& err)
{
    unsigned int im = 0;
    for (unsigned int iy = 0; iy < mMatrix.Rows(); ++iy)
        for (unsigned int ix = (iy == 0 ? mMatrix.ColX() + 1 : 0); ix < mMatrix.Cols(); ++ix)
            maaErrorBuffer[iy][x + ix] += err * mMatrix[im++];
}

//-------------------------------------------------------------------------------

extern const OrderedDither::Pattern BayerMatrix2({
    { 0, 2 },
    { 3, 1 }});

extern const OrderedDither::Pattern BayerMatrix3({
    { 0, 3, 6 },
    { 7, 1, 4 },
    { 5, 8, 2 }});

extern const OrderedDither::Pattern BayerMatrix4({
    {  0,  8,  2, 10 },
    { 12,  4, 14,  6 },
    {  3, 11,  1,  9 },
    { 15,  7, 13,  5 }});

extern const OrderedDither::Pattern BlueNoise64a(64, kBlueNoise64a, 64 * 64);

//*******************************************************************************

extern const DiffusionDither::Filter AtkinsonMatrix(
    {{       1, 1 },
     { 1, 1, 1, 0 },
     { 0, 1, 0, 0 }}, 2);

extern const DiffusionDither::Filter BurkesMatrix(
    {{          8, 4 },
     { 2, 4, 8, 4, 2 }});

extern const DiffusionDither::Filter JarvisJudiceNinkeMatrix(
    {{          7, 5 },
     { 3, 5, 7, 5, 3 },
     { 1, 3, 5, 3, 1 }});

extern const DiffusionDither::Filter Sierra2Matrix(
    {{          4, 3 },
     { 1, 2, 3, 2, 1 }});

extern const DiffusionDither::Filter Sierra3Matrix(
    {{          5, 3 },
     { 2, 4, 5, 4, 2 },
     { 0, 2, 3, 2, 0 }});

extern const DiffusionDither::Filter StuckiMatrix(
    {{          8, 4 },
     { 2, 4, 8, 4, 2 },
     { 1, 2, 4, 2, 1 }});

//*******************************************************************************

DitherStrategySPtr GetDitherStrategy(DitherMethodId method, unsigned int imageWidth)
{
    DitherStrategySPtr s;
    switch (method)
    {
        case DitherMethodId::kNone:             s = std::make_shared<NoDither> ();                                          break;
        case DitherMethodId::kDiffusion1D:      s = std::make_shared<DiffusionDither1D> ();                                 break;
        case DitherMethodId::kSierraLite:       s = std::make_shared<SierraLiteDither> (imageWidth);                        break;
        case DitherMethodId::kFloydSteinberg:   s = std::make_shared<FloydSteinbergDither> (imageWidth);                    break;
        case DitherMethodId::kBayer2x2:         s = std::make_shared<OrderedDither> (BayerMatrix2, imageWidth);             break;
        case DitherMethodId::kBayer3x3:         s = std::make_shared<OrderedDither> (BayerMatrix3, imageWidth);             break;
        case DitherMethodId::kBayer4x4:         s = std::make_shared<OrderedDither> (BayerMatrix4, imageWidth);             break;
        case DitherMethodId::kBlueNoise:        s = std::make_shared<OrderedDither> (BlueNoise64a, imageWidth);             break;
        case DitherMethodId::kBlueNoiseX:       s = std::make_shared<OrderedDither> (BlueNoise64a, imageWidth, true);       break;
        case DitherMethodId::kAtkinson:         s = std::make_shared<DiffusionDither> (AtkinsonMatrix, imageWidth);         break;
        case DitherMethodId::kBurkes:           s = std::make_shared<DiffusionDither> (BurkesMatrix, imageWidth);           break;
        case DitherMethodId::kJarvisJudiceNinke:s = std::make_shared<DiffusionDither> (JarvisJudiceNinkeMatrix, imageWidth);break;
        case DitherMethodId::kSierra3:          s = std::make_shared<DiffusionDither> (Sierra3Matrix, imageWidth);          break;
        case DitherMethodId::kSierra2:          s = std::make_shared<DiffusionDither> (Sierra2Matrix, imageWidth);          break;
        case DitherMethodId::kStucki:           s = std::make_shared<DiffusionDither> (StuckiMatrix, imageWidth);           break;
    }
    return s;
}

DitherStrategySPtr GetNoOpDitherStrategy()
{
    return DitherStrategySPtr(new NoDither());
}

ColourChannel GetDitherOffset(unsigned int x, unsigned int y)
{
    return BlueNoise64a(x, y);
}

}
// end of namespace pov_base
