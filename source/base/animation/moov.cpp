//******************************************************************************
///
/// @file base/animation/moov.cpp
///
/// Implementation of QuickTime (MooV) stream handling.
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
#include "base/animation/moov.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/pov_err.h"
#include "base/types.h"
#include "base/animation/animation.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

typedef POV_INT32 Type; // TODO - this is a lousy type name

struct PrivateData final
{
    unsigned int width;
    unsigned int height;
    Type componenttype;
    bool alphachannel;
    int timescale;
    int frameduration;
    POV_OFF_T mdatsize;
    std::vector<int> imagesizes;
};

namespace Moov
{

void WriteAtomHeader(OStream *file, Type type, POV_OFF_T size);
void WriteType(OStream *file, Type data);
void WriteInt2(OStream *file, POV_INT16 data);
void WriteInt4(OStream *file, POV_INT32 data);
void WriteInt8(OStream *file, POV_INT64 data);
void WriteN(OStream *file, size_t cnt, POV_INT8 data);

void *ReadFileHeader(IStream *file, float& lengthinseconds, unsigned int& lengthinframes, Animation::CodecType& codec, unsigned int& w, unsigned int& h, const Animation::ReadOptions& options, std::vector<std::string>& warnings)
{
    throw POV_EXCEPTION(kCannotHandleDataErr, "Reading QuickTime movie files is not supported (yet)!");
    return nullptr;
}

void PreReadFrame(IStream *file, unsigned int frame, POV_OFF_T& bytes, Animation::CodecType& codec, const Animation::ReadOptions& options, std::vector<std::string>& warnings, void *state)
{
}

void PostReadFrame(IStream *file, unsigned int frame, POV_OFF_T bytes, Animation::CodecType& codec, const Animation::ReadOptions& options, std::vector<std::string>& warnings, void *state)
{
}

void FinishReadFile(IStream *file, std::vector<std::string>& warnings, void *state)
{
}

void *WriteFileHeader(OStream *file, Animation::CodecType& codec, unsigned int w, unsigned int h, const Animation::WriteOptions& options, std::vector<std::string>& warnings)
{
    PrivateData pd;

    pd.width = w;
    pd.height = h;

    // determine codec equivalent

    switch(codec)
    {
        case Animation::LosslessCodec:
        case Animation::PNGCodec:
            codec = Animation::PNGCodec;
            pd.componenttype = 'png ';
            pd.alphachannel = options.alphachannel;
            break;
        case Animation::LossyCodec:
        case Animation::JPEGCodec:
            codec = Animation::JPEGCodec;
            pd.componenttype = 'jpeg';
            pd.alphachannel = false;
            break;
        case Animation::BMPCodec:
            codec = Animation::BMPCodec;
            pd.componenttype = 'WRLE';
            pd.alphachannel = false;
            break;
        default:
            return nullptr; // error - cannot handle format
    }

    if(pd.alphachannel != options.alphachannel)
        warnings.push_back("Alpha channel output not supported for this animation codec. Alpha channel output will be disabled.");

    // compute time scale

    double ts = double(options.framespersecond);
    int m = 1;

    for(m = 1; m <= 1000; m *= 10)
    {
        if((fabs((ts * double(m)) - double(int((ts * double(m))))) < 1.0e-5) && ((ts * double(m)) >= 1.0))
            break;
    }

    pd.timescale = std::max(int(double(options.framespersecond) * double(m)), 1);

    // frame duration according to time scale

    pd.frameduration = m;

    // movie data atom header

    WriteAtomHeader(file, 'mdat', -1);

    pd.mdatsize = 16;

    // NOTE: This allocation occurs at the end such that there cannot be a memory leak
    // should any of the previous operations fail. If and only if this function returns
    // a state other than `nullptr` shall it be assumed to have been successful!
    return reinterpret_cast<void *>(new PrivateData(pd));
}

void PreWriteFrame(OStream *, const Animation::WriteOptions&, std::vector<std::string>&, void *state)
{
    PrivateData *pd = reinterpret_cast<PrivateData *>(state);

    if (pd == nullptr)
        throw POV_EXCEPTION_CODE(kNullPointerErr);

    // there really is nothing to do here [trf]
}

void PostWriteFrame(OStream *file, POV_OFF_T bytes, const Animation::WriteOptions&, std::vector<std::string>&, void *state)
{
    PrivateData *pd = reinterpret_cast<PrivateData *>(state);

    if (pd == nullptr)
        throw POV_EXCEPTION_CODE(kNullPointerErr);

    // update mdat size

    file->seekg(0, IOBase::seek_end);
    pd->mdatsize = file->tellg() + 16;
    file->seekg(8, IOBase::seek_set);
    WriteInt8(file, pd->mdatsize);
    file->seekg(0, IOBase::seek_end);

    if(bytes > 2147483647) // 2^31 - 1
        throw POV_EXCEPTION(kInvalidDataSizeErr, "Cannot handle frame data larger than 2^31 bytes!");

    pd->imagesizes.push_back(int(bytes));
}

void FinishWriteFile(OStream *file, const Animation::WriteOptions& options, std::vector<std::string>& warnings, void *state)
{
    PrivateData *pd = reinterpret_cast<PrivateData *>(state);

    if (pd == nullptr)
        throw POV_EXCEPTION_CODE(kNullPointerErr);

    POV_OFF_T stsz_size = 20 + (pd->imagesizes.size() * 4);
    POV_OFF_T stsc_size = 28;
    POV_OFF_T stts_size = 32;
    POV_OFF_T stsd_size = 102;
    POV_OFF_T stbl_size = 8 + stsd_size + stts_size + stsc_size + stsz_size;
    POV_OFF_T vmhd_size = 20;
    POV_OFF_T minf_size = 8 + vmhd_size + stbl_size;
    POV_OFF_T hdlr_size = 32;
    POV_OFF_T mdhd_size = 32;
    POV_OFF_T mdia_size = 8 + mdhd_size + hdlr_size + minf_size;
    POV_OFF_T tkhd_size = 112;
    POV_OFF_T trak_size = 8 + tkhd_size + mdia_size;
    POV_OFF_T mvhd_size = 108;
    POV_OFF_T moov_size = 8 + mvhd_size + trak_size;

    int duration = pd->frameduration * pd->imagesizes.size();

    // write movie atom

    WriteAtomHeader(file, 'moov', moov_size);

    // write movie header atom

    WriteAtomHeader(file, 'mvhd', mvhd_size);

    WriteInt4(file, 0); // version and flags
    WriteInt4(file, 0); // creation time
    WriteInt4(file, 0); // modification time
    WriteInt4(file, pd->timescale); // time scale
    WriteInt4(file, duration); // duration
    WriteInt4(file, 1 << 16); // preferred playback rate
    WriteInt2(file, 1 << 8); // preferred sound volume
    WriteN(file, 10, 0); // ten reserved bytes
    WriteInt4(file, 1 << 16); // matrix a
    WriteInt4(file, 0); // matrix b
    WriteInt4(file, 0); // matrix u
    WriteInt4(file, 0); // matrix c
    WriteInt4(file, 1 << 16); // matrix d
    WriteInt4(file, 0); // matrix v
    WriteInt4(file, 0); // matrix tx
    WriteInt4(file, 0); // matrix ty
    WriteInt4(file, 1 << 30); // matrix w
    WriteInt4(file, 0); // preview time
    WriteInt4(file, 0); // preview duration
    WriteInt4(file, 0); // poster time
    WriteInt4(file, 0); // selection time
    WriteInt4(file, 0); // selection duration
    WriteInt4(file, 0); // current time
    WriteInt4(file, 2); // next track id (this code uses track 1)

    // write track atom

    WriteAtomHeader(file, 'trak', trak_size);

    // write track header atom

    WriteAtomHeader(file, 'tkhd', tkhd_size);

    WriteInt4(file, 0); // version and flags
    WriteInt4(file, 0); // creation time
    WriteInt4(file, 0); // modification time
    WriteInt4(file, 1); // track id
    WriteN(file, 4, 0); // four reserved bytes
    WriteInt4(file, duration); // duration
    WriteN(file, 8, 0); // eight reserved bytes
    WriteInt2(file, 1); // layer
    WriteInt2(file, 0); // alternate group
    WriteInt2(file, 1 << 8); // sound volume
    WriteN(file, 2, 0); // two reserved bytes
    WriteInt4(file, 1 << 16); // matrix a
    WriteInt4(file, 0); // matrix b
    WriteInt4(file, 0); // matrix u
    WriteInt4(file, 0); // matrix c
    WriteInt4(file, 1 << 16); // matrix d
    WriteInt4(file, 0); // matrix v
    WriteInt4(file, 0); // matrix tx
    WriteInt4(file, 0); // matrix ty
    WriteInt4(file, 1 << 30); // matrix w
    WriteInt4(file, pd->width << 16); // track width
    WriteInt4(file, pd->height << 16); // track height

    // write media atom

    WriteAtomHeader(file, 'mdia', mdia_size);

    // write header media atom

    WriteAtomHeader(file, 'mdhd', mdhd_size);

    WriteInt4(file, 0); // version and flags
    WriteInt4(file, 0); // creation time
    WriteInt4(file, 0); // modification time
    WriteInt4(file, pd->timescale); // time scale
    WriteInt4(file, duration); // duration
    WriteInt2(file, 0); // language
    WriteInt2(file, 0); // quality

    // write handler atom

    WriteAtomHeader(file, 'hdlr', hdlr_size);

    WriteInt4(file, 0); // version and flags
    WriteType(file, pd->componenttype); // component type
    WriteType(file, 'vide'); // component subtype (this media is video)
    WriteInt4(file, 0); // reserved
    WriteInt4(file, 0); // reserved
    WriteInt4(file, 0); // reserved

    // write media information atom

    WriteAtomHeader(file, 'minf', minf_size);

    // write video media information header atom

    WriteAtomHeader(file, 'vmhd', vmhd_size);

    WriteInt4(file, 0); // version and flags
    WriteInt2(file, 0); // graphics mode
    WriteInt2(file, 0); // opcolor red
    WriteInt2(file, 0); // opcolor green
    WriteInt2(file, 0); // opcolor blue

    // write sample table atom

    WriteAtomHeader(file, 'stbl', stbl_size);

    // write sample description atom

    WriteAtomHeader(file, 'stsd', stsd_size);

    WriteInt4(file, 0); // version and flags
    WriteInt4(file, 1); // number of entries (this code only needs one entry)
    WriteInt4(file, 86); // description size
    WriteType(file, pd->componenttype); // data format
    WriteInt4(file, 0); // reserved
    WriteInt2(file, 0); // reserved
    WriteInt2(file, 0); // data reference index
    WriteInt2(file, 0); // version
    WriteInt2(file, 0); // revision level
    WriteType(file, 'appl'); // vendor
    WriteInt4(file, 0); // temporal quality
    WriteInt4(file, 512); // spacial quality
    WriteInt2(file, pd->width); // width
    WriteInt2(file, pd->height); // height
    WriteInt4(file, 72 << 16); // horizontal resolution
    WriteInt4(file, 72 << 16); // vertical resolution
    WriteInt4(file, 0); // data size (required to be zero according to Apple documentation)
    WriteInt4(file, 1); // frame count
    WriteN(file, 1, 4); // name (32-byte Pascal string, so first byte is length!)
    WriteType(file, pd->componenttype); // name (continued, uses codec type for simplicity)
    WriteN(file, 27, 0); // name (continued, unused)
    WriteInt2(file, options.bpcc * (3 + (pd->alphachannel ? 1 : 0))); // depth
    WriteInt2(file, -1); // color table id

    // write time-to-sample atom

    WriteAtomHeader(file, 'stts', stts_size);

    WriteInt4(file, 0); // version and flags
    WriteInt4(file, 1); // number of entries (this code only needs one entry)
    WriteInt4(file, pd->imagesizes.size()); // sample count
    WriteInt4(file, pd->frameduration); // sample duration

    // write sample-to-chunk atom

    WriteAtomHeader(file, 'stsc', stsc_size);

    WriteInt4(file, 0); // version and flags
    WriteInt4(file, 1); // number of entries (this code only needs one entry)
    WriteInt4(file, 1); // first chunk
    WriteInt4(file, pd->imagesizes.size()); // samples per chunk
    WriteInt4(file, 1); // sample description id

    // write sample size atom

    WriteAtomHeader(file, 'stsz', stsz_size);

    WriteInt4(file, 0); // version and flags
    WriteInt4(file, 0); // sample size (all samples have different sizes, so this needs to be zero)
    WriteInt4(file, pd->imagesizes.size()); // number of entries

    for(std::vector<int>::const_iterator i = pd->imagesizes.begin(); i != pd->imagesizes.end(); i++)
        WriteInt4(file, *i); // sample size entry

    delete pd;
}
/*
void ReadAtomHeader(IStream *file, Type& type, POV_OFF_T& size)
{
    ReadInt4(file, size);

    if(size == 0) // atom goes up to end of file
    {
        ReadType(file, type);

        POV_OFF_T t = file->tellg();
        file->seekg(0, IOBase::seek_end);
        size = file->tellg() - t + 8;
        file->seekg(t, IOBase::seek_set);
    }
    else if(size == 1) // atom sizes is outside 32-bit range
    {
        ReadType(file, type);

        ReadInt8(file, size);
    }
    else
        ReadType(file, type);
}
*/
void WriteAtomHeader(OStream *file, Type type, POV_OFF_T size)
{
    if(size < 0) // temporary size - always assume 64-bit size
    {
        WriteInt4(file, 1);
        WriteType(file, type);
        WriteInt8(file, 0);
    }
    else if(size > UNSIGNED32_MAX) // size outside 32-bit range
    {
        WriteInt4(file, 1);
        WriteType(file, type);
        WriteInt8(file, size);
    }
    else // size within 32-bit range
    {
        WriteType(file, type);
        WriteInt4(file, size);
    }
}

void WriteType(OStream *file, Type data)
{
    file->Write_Byte((data >> 24) & 255);
    file->Write_Byte((data >> 16) & 255);
    file->Write_Byte((data >> 8) & 255);
    file->Write_Byte(data & 255);
}

void WriteInt2(OStream *file, POV_INT16 data)
{
    file->Write_Byte((data >> 8) & 255);
    file->Write_Byte(data & 255);
}

void WriteInt4(OStream *file, POV_INT32 data)
{
    file->Write_Byte((data >> 24) & 255);
    file->Write_Byte((data >> 16) & 255);
    file->Write_Byte((data >> 8) & 255);
    file->Write_Byte(data & 255);
}

void WriteInt8(OStream *file, POV_INT64 data)
{
    file->Write_Byte((data >> 56) & 255);
    file->Write_Byte((data >> 48) & 255);
    file->Write_Byte((data >> 40) & 255);
    file->Write_Byte((data >> 32) & 255);
    file->Write_Byte((data >> 24) & 255);
    file->Write_Byte((data >> 16) & 255);
    file->Write_Byte((data >> 8) & 255);
    file->Write_Byte(data & 255);
}

void WriteN(OStream *file, size_t cnt, POV_INT8 data)
{
    for(size_t i = 0; i < cnt; i++)
        file->Write_Byte(data);
}

}
// end of namespace Moov

}
// end of namespace pov_base
