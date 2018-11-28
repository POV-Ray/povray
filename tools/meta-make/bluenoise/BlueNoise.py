# BlueNoise.py - An implementation of the void and cluster method for generation of 
#                blue noise dither arrays and related utilities.
#
# Written in 2016 by Christoph Peters, Christoph(at)MomentsInGraphics.de
#
# To the extent possible under law, the author(s) have dedicated all copyright and 
# related and neighboring rights to this software to the public domain worldwide. 
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with 
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 

from os import path,makedirs
import numpy as np
from scipy import ndimage
from matplotlib import pyplot
import png
import threading
import struct

def GetBayerPattern(Log2Width):
    """Creates a two-dimensional Bayer pattern with a width and height of 
       2**Log2Width."""
    X,Y=np.meshgrid(range(2**Log2Width),range(2**Log2Width));
    Result=np.zeros_like(X);
    for i in range(Log2Width):
        StripesY=np.where(np.bitwise_and(Y,2**(Log2Width-1-i))!=0,1,0);
        StripesX=np.where(np.bitwise_and(X,2**(Log2Width-1-i))!=0,1,0);
        Checker=np.bitwise_xor(StripesX,StripesY);
        Result+=np.bitwise_or(StripesY*2**(2*i),Checker*2**(2*i+1));
    return Result;


def FindLargestVoid(BinaryPattern,StandardDeviation):
    """This function returns the indices of the largest void in the given binary 
       pattern as defined by Ulichney.
      \param BinaryPattern A boolean array (should be two-dimensional although the 
             implementation works in arbitrary dimensions).
      \param StandardDeviation The standard deviation used for the Gaussian filter 
             in pixels. This can be a single float for an isotropic Gaussian or a 
             tuple with one float per dimension for an anisotropic Gaussian.
      \return A flat index i such that BinaryPattern.flat[i] corresponds to the 
              largest void. By definition this is a majority pixel.
      \sa GetVoidAndClusterBlueNoise"""
    # The minority value is always True for convenience
    if(np.count_nonzero(BinaryPattern)*2>=np.size(BinaryPattern)):
        BinaryPattern=np.logical_not(BinaryPattern);
    # Apply the Gaussian. We do not want to cut off the Gaussian at all because even 
    # the tiniest difference can change the ranking. Therefore we apply the Gaussian 
    # through a fast Fourier transform by means of the convolution theorem.
    FilteredArray=np.fft.ifftn(ndimage.fourier.fourier_gaussian(np.fft.fftn(np.where(BinaryPattern,1.0,0.0)),StandardDeviation)).real;
    # Find the largest void
    return np.argmin(np.where(BinaryPattern,2.0,FilteredArray));


def FindTightestCluster(BinaryPattern,StandardDeviation):
    """Like FindLargestVoid() but finds the tightest cluster which is a minority 
       pixel by definition.
      \sa GetVoidAndClusterBlueNoise"""
    if(np.count_nonzero(BinaryPattern)*2>=np.size(BinaryPattern)):
        BinaryPattern=np.logical_not(BinaryPattern);
    FilteredArray=np.fft.ifftn(ndimage.fourier.fourier_gaussian(np.fft.fftn(np.where(BinaryPattern,1.0,0.0)),StandardDeviation)).real;
    return np.argmax(np.where(BinaryPattern,FilteredArray,-1.0));


def GetVoidAndClusterBlueNoise(OutputShape,StandardDeviation=1.5,InitialSeedFraction=0.1):
    """Generates a blue noise dither array of the given shape using the method 
       proposed by Ulichney [1993] in "The void-and-cluster method for dither array 
       generation" published in Proc. SPIE 1913. 
      \param OutputShape The shape of the output array. This function works in 
             arbitrary dimension, i.e. OutputShape can have arbitrary length. Though 
             it is only tested for the 2D case where you should pass a tuple 
             (Height,Width).
      \param StandardDeviation The standard deviation in pixels used for the 
             Gaussian filter defining largest voids and tightest clusters. Larger 
             values lead to more low-frequency content but better isotropy. Small 
             values lead to more ordered patterns with less low-frequency content.
             Ulichney proposes to use a value of 1.5. If you want an anisotropic 
             Gaussian, you can pass a tuple of length len(OutputShape) with one 
             standard deviation per dimension.
      \param InitialSeedFraction The only non-deterministic step in the algorithm 
             marks a small number of pixels in the grid randomly. This parameter 
             defines the fraction of such points. It has to be positive but less 
             than 0.5. Very small values lead to ordered patterns, beyond that there 
             is little change.
      \return An integer array of shape OutputShape containing each integer from 0 
              to np.prod(OutputShape)-1 exactly once."""
    nRank=np.prod(OutputShape);
    # Generate the initial binary pattern with a prescribed number of ones
    nInitialOne=max(1,min(int((nRank-1)/2),int(nRank*InitialSeedFraction)));
    # Start from white noise (this is the only randomized step)
    InitialBinaryPattern=np.zeros(OutputShape,dtype=np.bool);
    InitialBinaryPattern.flat=np.random.permutation(np.arange(nRank))<nInitialOne;
    # Swap ones from tightest clusters to largest voids iteratively until convergence
    while(True):
        iTightestCluster=FindTightestCluster(InitialBinaryPattern,StandardDeviation);
        InitialBinaryPattern.flat[iTightestCluster]=False;
        iLargestVoid=FindLargestVoid(InitialBinaryPattern,StandardDeviation);
        if(iLargestVoid==iTightestCluster):
            InitialBinaryPattern.flat[iTightestCluster]=True;
            # Nothing has changed, so we have converged
            break;
        else:
            InitialBinaryPattern.flat[iLargestVoid]=True;
    # Rank all pixels
    DitherArray=np.zeros(OutputShape,dtype=np.int);
    # Phase 1: Rank minority pixels in the initial binary pattern
    BinaryPattern=np.copy(InitialBinaryPattern);
    for Rank in range(nInitialOne-1,-1,-1):
        iTightestCluster=FindTightestCluster(BinaryPattern,StandardDeviation);
        BinaryPattern.flat[iTightestCluster]=False;
        DitherArray.flat[iTightestCluster]=Rank;
    # Phase 2: Rank the remainder of the first half of all pixels
    BinaryPattern=InitialBinaryPattern;
    for Rank in range(nInitialOne,int((nRank+1)/2)):
        iLargestVoid=FindLargestVoid(BinaryPattern,StandardDeviation);
        BinaryPattern.flat[iLargestVoid]=True;
        DitherArray.flat[iLargestVoid]=Rank;
    # Phase 3: Rank the last half of pixels
    for Rank in range(int((nRank+1)/2),nRank):
        iTightestCluster=FindTightestCluster(BinaryPattern,StandardDeviation);
        BinaryPattern.flat[iTightestCluster]=True;
        DitherArray.flat[iTightestCluster]=Rank;
    return DitherArray;


def AnalyzeNoiseTexture(Texture,SingleFigure=True,SimpleLabels=False):
    """Given a 2D array of real noise values this function creates one or more 
       figures with plots that allow you to analyze it, especially with respect to 
       blue noise characteristics. The analysis includes the absolute value of the 
       Fourier transform, the power distribution in radial frequency bands and an 
       analysis of directional isotropy.
      \param A two-dimensional array.
      \param SingleFigure If this is True, all plots are shown in a single figure, 
             which is useful for on-screen display. Otherwise one figure per plot 
             is created.
      \param SimpleLabels Pass True to get axis labels that fit into the context of 
             the blog post without further explanation.
      \return A list of all created figures.
      \note For the plots to show you have to invoke pyplot.show()."""
    FigureList=list();
    if(SingleFigure):
        Figure=pyplot.figure();
        FigureList.append(Figure);
    def PrepareAxes(iAxes,**KeywordArguments):
        if(SingleFigure):
            return Figure.add_subplot(2,2,iAxes,**KeywordArguments);
        else:
            NewFigure=pyplot.figure();
            FigureList.append(NewFigure);
            return NewFigure.add_subplot(1,1,1,**KeywordArguments);
    # Plot the dither array itself
    PrepareAxes(1,title="Blue noise dither array");
    pyplot.imshow(Texture.real,cmap="gray",interpolation="nearest");
    # Plot the Fourier transform with frequency zero shifted to the center
    PrepareAxes(2,title="Fourier transform (absolute value)",xlabel="$\\omega_x$",ylabel="$\\omega_y$");
    DFT=np.fft.fftshift(np.fft.fft2(Texture))/float(np.size(Texture));
    Height,Width=Texture.shape;
    ShiftY,ShiftX=(int(Height/2),int(Width/2));
    pyplot.imshow(np.abs(DFT),cmap="viridis",interpolation="nearest",vmin=0.0,vmax=np.percentile(np.abs(DFT),99),extent=(-ShiftX-0.5,Width-ShiftX-0.5,-ShiftY+0.5,Height-ShiftY+0.5));
    pyplot.colorbar();
    # Plot the distribution of power over radial frequency bands
    PrepareAxes(3,title="Radial power distribution",xlabel="Distance from center / pixels" if SimpleLabels else "$\\sqrt{\\omega_x^2+\\omega_y^2}$");
    X,Y=np.meshgrid(range(DFT.shape[1]),range(DFT.shape[0]));
    X-=int(DFT.shape[1]/2);
    Y-=int(DFT.shape[0]/2);
    RadialFrequency=np.asarray(np.round(np.sqrt(X**2+Y**2)),dtype=np.int);
    RadialPower=np.zeros((np.max(RadialFrequency)-1,));
    DFT[int(DFT.shape[0]/2),int(DFT.shape[1]/2)]=0.0;
    for i in range(RadialPower.shape[0]):
        RadialPower[i]=np.sum(np.where(RadialFrequency==i,np.abs(DFT),0.0))/np.count_nonzero(RadialFrequency==i);
    pyplot.plot(np.arange(np.max(RadialFrequency)-1)+0.5,RadialPower);
    # Plot the distribution of power over angular frequency ranges
    PrepareAxes(4,title="Anisotropy (angular power distribution)",aspect="equal",xlabel="Frequency x" if SimpleLabels else "$\\omega_x$",ylabel="Frequency y" if SimpleLabels else "$\\omega_y$");
    CircularMask=np.logical_and(0<RadialFrequency,RadialFrequency<int(min(DFT.shape[0],DFT.shape[1])/2));
    NormalizedX=np.asarray(X,dtype=np.float)/np.maximum(1.0,np.sqrt(X**2+Y**2));
    NormalizedY=np.asarray(Y,dtype=np.float)/np.maximum(1.0,np.sqrt(X**2+Y**2));
    BinningAngle=np.linspace(0.0,2.0*np.pi,33);
    AngularPower=np.zeros_like(BinningAngle);
    for i,Angle in enumerate(BinningAngle):
        DotProduct=NormalizedX*np.cos(Angle)+NormalizedY*np.sin(Angle);
        FullMask=np.logical_and(CircularMask,DotProduct>=np.cos(np.pi/32.0));
        AngularPower[i]=np.sum(np.where(FullMask,np.abs(DFT),0.0))/np.count_nonzero(FullMask);
    MeanAngularPower=np.mean(AngularPower[1:]);
    DenseAngle=np.linspace(0.0,2.0*np.pi,256);
    pyplot.plot(np.cos(DenseAngle)*MeanAngularPower,np.sin(DenseAngle)*MeanAngularPower,color=(0.7,0.7,0.7));
    pyplot.plot(np.cos(BinningAngle)*AngularPower,np.sin(BinningAngle)*AngularPower);
    return FigureList;


def PlotBinaryPatterns(Texture,nPatternRow,nPatternColumn):
    """This function creates a figure with a grid of thresholded versions of the 
       given 2D noise texture. It assumes that each value from 0 to 
       np.size(Texture)-1 is contained exactly once.
      \return The created figure.
      \note For the plots to show you have to invoke pyplot.show()."""
    Figure=pyplot.figure();
    nPattern=nPatternRow*nPatternColumn+1;
    for i in range(1,nPattern):
        Figure.add_subplot(nPatternRow,nPatternColumn,i,xticks=[],yticks=[]);
        pyplot.imshow(np.where(Texture*nPattern<i*np.size(Texture),1.0,0.0),cmap="gray",interpolation="nearest");
    return Figure;


def StoreNoiseTextureLDR(Texture,OutputPNGFilePath,nRank=-1):
    """This function stores the given texture to a standard low-dynamic range png 
       file with four channels and 8 bits per channel.
      \param Texture An array of shape (Height,Width) or (Height,Width,nChannel). 
             The former is handled like (Height,Width,1). If nChannel>4 the 
             superfluous channels are ignored. If nChannel<4 the data is expanded. 
             The alpha channel is set to 255, green and blue are filled with black 
             or with duplicates of red if nChannel==1. It is assumed that each 
             channel contains every integer value from 0 to nRank-1 exactly once. 
             The range of values is remapped linearly to span the range from 0 to 
             255.
      \param OutputPNGFilePath The path to the output png file including the file 
             format extension.
      \param nRank Defaults to Width*Height if you pass a non-positive value."""
    # Scale the array to an LDR version
    if(nRank<=0):
        nRank=Texture.shape[0]*Texture.shape[1];
    Texture=np.asarray((Texture*256)//nRank,dtype=np.uint8);
    # Get a three-dimensional array
    if(len(Texture.shape)<3):
        Texture=Texture[:,:,np.newaxis];
    # Generate channels as needed
    if(Texture.shape[2]==1):
        Texture=np.dstack([Texture]*3+[255*np.ones_like(Texture[:,:,0])]);
    elif(Texture.shape[2]==2):
        Texture=np.dstack([Texture[:,:,0],Texture[:,:,1]]+[np.zeros_like(Texture[:,:,0])]+[255*np.ones_like(Texture[:,:,0])]);
    elif(Texture.shape[2]==3):
        Texture=np.dstack([Texture[:,:,0],Texture[:,:,1],Texture[:,:,2]]+[255*np.ones_like(Texture[:,:,0])]);
    elif(Texture.shape[2]>4):
        Texture=Texture[:,:,:4];
    # Save the image
    png.from_array(Texture,"RGBA;8").save(OutputPNGFilePath);


def StoreNoiseTextureHDR(Texture,OutputPNGFilePath,nRank=-1):
    """This function stores the given texture to an HDR png file with 16 bits per 
       channel and the specified number of channels.
      \param Texture An array of shape (Height,Width) or (Height,Width,nChannel). 
             The former is handled like (Height,Width,1). It is assumed that each 
             channel contains each integer value from 0 to nRank-1 exactly once. The 
             range of values is remapped linearly to span the range from 0 to 
             2**16-1 supported by the output format. nChannel can be 1, 2, 3 or 4.
      \param OutputPNGFilePath The path to the output *.png file including the file 
             format extension.
      \param nRank Defaults to Width*Height if you pass a non-positive value."""
    # Scale the array to an HDR version
    if(nRank<=0):
        nRank=Texture.shape[0]*Texture.shape[1];
    Texture=np.asarray((np.asarray(Texture,dtype=np.uint64)*(2**16))//nRank,dtype=np.uint16);
    # Get a three-dimensional array
    if(len(Texture.shape)<3):
        Texture=Texture[:,:,np.newaxis];
    # Save the image
    Mode=["L","LA","RGB","RGBA"][Texture.shape[2]-1]+";16";
    png.from_array(Texture,Mode).save(OutputPNGFilePath);


def StoreNDTextureHDR(Array,OutputFilePath):
    """This function stores the given unsigned integer array in a minimalist binary 
       file format. The last dimension is interpreted as corresponding to the 
       channels of the image. The file format consists of a sequence of unsigned, 
       least significant bit first 32-bit integers. The contained data is described 
       below:
      - Version: File format version, should be 1.
      - nChannel: The number of color channels in the image. This should be a value 
        between 1 (greyscale) and 4 (RGBA).
      - nDimension: The number of dimensions of the stored array, i.e. the number of 
        indices required to uniquely identify one pixel, voxel, etc..
      - Shape[nDimension]: nDimension integers providing the size of the array along 
        each dimension. By convention the first dimension is height, second width 
        and third depth.
      - Data[Shape[0]*...*Shape[nDimension-1]*nChannel]: The uncompressed data of 
        the array. The channels are unrolled first, followed by all dimensions in 
        reverse order. Thus, an RG image of size 3*2 would be stored in the 
        following order: 00R, 00G, 01R, 01G, 10R, 10G, 11R, 11G, 20R, 20G, 21R, 
        21G"""
    # Prepare all the meta data and the data itself
    Array=np.asarray(Array,dtype=np.uint32);
    Version=1;
    nDimension=len(Array.shape)-1;
    nChannel=Array.shape[nDimension];
    Shape=Array.shape[0:nDimension];
    Data=Array.flatten("C");
    # Write it to the file
    OutputFile=open(OutputFilePath,"wb");
    OutputFile.write(struct.pack("LLL",Version,nChannel,nDimension));
    OutputFile.write(struct.pack("L"*nDimension,*Shape));
    OutputFile.write(struct.pack("L"*np.size(Data),*Data));
    OutputFile.close();


def LoadNDTextureHDR(SourceFilePath):
    """Loads a file generated by StoreNDTextureHDR() and returns it as an array like 
       the one that goes into StoreNDTextureHDR() using data type np.uint32. On 
       failure it returns None."""
    # Load the meta data
    File=open(SourceFilePath,"rb");
    Version,nChannel,nDimension=struct.unpack_from("LLL",File.read(12));
    if(Version!=1):
        return None;
    Shape=struct.unpack_from("L"*nDimension,File.read(4*nDimension));
    nScalar=np.prod(Shape)*nChannel;
    Data=struct.unpack_from("L"*nScalar,File.read(4*nScalar));
    File.close();
    # Prepare the output
    return np.asarray(Data,dtype=np.uint32).reshape(tuple(list(Shape)+[nChannel]),order="C");


def GenerateBlueNoiseDatabase(RandomSeedIndexList=range(1),MinResolution=16,MaxResolution=1024,ChannelCountList=[1,2,3,4],StandardDeviation=1.5):
    """This function generates a database of blue noise textures for all sorts of 
       use cases. It includes power-of-two resolutions from MinResolution**2 up 
       to MaxResolution**2. Textures are generated with each given number of 
       channels. Each texture is generated multiple times using different random 
       numbers per entry in RandomSeedIndexList and the entries become part of the 
       file name. StandardDeviation forwards to GetVoidAndClusterBlueNoise(). The 
       results are stored as LDR and HDR files to a well-organized tree of 
       of directories."""
    Resolution=MinResolution;
    while(Resolution<=MaxResolution):
        OutputDirectory="../Data/%d_%d"%(Resolution,Resolution);
        if(not path.exists(OutputDirectory)):
            makedirs(OutputDirectory);
        for nChannel in ChannelCountList:
            for i in RandomSeedIndexList:
                Texture=np.dstack([GetVoidAndClusterBlueNoise((Resolution,Resolution),StandardDeviation) for j in range(nChannel)]);
                LDRFormat=["LLL1","RG01","RGB1","RGBA"][nChannel-1];
                HDRFormat=["L","LA","RGB","RGBA"][nChannel-1];
                StoreNoiseTextureLDR(Texture,path.join(OutputDirectory,"LDR_%s_%d.png"%(LDRFormat,i)));
                StoreNoiseTextureHDR(Texture,path.join(OutputDirectory,"HDR_%s_%d.png"%(HDRFormat,i)));
                print("%d*%d, %s, %d"%(Resolution,Resolution,LDRFormat,i));
        Resolution*=2;


def Generate3DBlueNoiseTexture(Width,Height,Depth,nChannel,StandardDeviation=1.5):
    """This function generates a single 3D blue noise texture with the specified 
       dimensions and number of channels. It then outputs it to a sequence of Depth 
       output files in LDR and HDR in a well-organized tree of directories. It also 
       outputs raw binary files.
      \sa StoreNDTextureHDR() """
    OutputDirectory="../Data/%d_%d_%d"%(Width,Height,Depth);
    if(not path.exists(OutputDirectory)):
        makedirs(OutputDirectory);
    # Generate the blue noise for the various channels using multi-threading
    ChannelTextureList=[None]*nChannel;
    ChannelThreadList=[None]*nChannel;
    def GenerateAndStoreTexture(Index):
        ChannelTextureList[Index]=GetVoidAndClusterBlueNoise((Height,Width,Depth),StandardDeviation);
    for i in range(nChannel):
        ChannelThreadList[i]=threading.Thread(target=GenerateAndStoreTexture,args=(i,));
        ChannelThreadList[i].start();
    for Thread in ChannelThreadList:
        Thread.join();
    Texture=np.concatenate([ChannelTextureList[i][:,:,:,np.newaxis] for i in range(nChannel)],3);
    LDRFormat=["LLL1","RG01","RGB1","RGBA"][nChannel-1];
    HDRFormat=["L","LA","RGB","RGBA"][nChannel-1];
    StoreNDTextureHDR(Texture,path.join(OutputDirectory,"HDR_"+HDRFormat+".raw"));
    for i in range(Depth):
        StoreNoiseTextureLDR(Texture[:,:,i,:],path.join(OutputDirectory,"LDR_%s_%d.png"%(LDRFormat,i)),Height*Width*Depth);
        StoreNoiseTextureHDR(Texture[:,:,i,:],path.join(OutputDirectory,"HDR_%s_%d.png"%(HDRFormat,i)),Height*Width*Depth);


def GenerateNDBlueNoiseTexture(Shape,nChannel,OutputFilePath,StandardDeviation=1.5):
    """This function generates a single n-dimensional blue noise texture with the 
       specified shape and number of channels. It then outputs it to the specified 
       raw binary file.
      \sa StoreNDTextureHDR() """
    OutputDirectory=path.split(OutputFilePath)[0];
    if(not path.exists(OutputDirectory)):
        makedirs(OutputDirectory);
    # Generate the blue noise for the various channels using multi-threading
    ChannelTextureList=[None]*nChannel;
    ChannelThreadList=[None]*nChannel;
    def GenerateAndStoreTexture(Index):
        ChannelTextureList[Index]=GetVoidAndClusterBlueNoise(Shape,StandardDeviation);
    for i in range(nChannel):
        ChannelThreadList[i]=threading.Thread(target=GenerateAndStoreTexture,args=(i,));
        ChannelThreadList[i].start();
    for Thread in ChannelThreadList:
        Thread.join();
    Texture=np.concatenate([ChannelTextureList[i][...,np.newaxis] for i in range(nChannel)],len(Shape));
    StoreNDTextureHDR(Texture,OutputFilePath);


def UniformToTriangularDistribution(UniformTexture):
    """Given an array with a uniform distribution of values, this function 
       constructs an array of equal shape with a triangular distribution of values. 
       This is accomplished by applying a differentiable, monotonously growing 
       function per entry.
      \param UniformTexture An integer array containing each value from 0 to 
             np.size(UniformTexture)-1 exactly once.
      \return A floating-point array with values between -1 and 1 where the density 
              grows linearly between -1 and 0 and falls linearly between 0 and 1."""
    Normalized=(np.asarray(UniformTexture,dtype=np.float)+0.5)/float(np.size(UniformTexture));
    return np.where(Normalized<0.5,np.sqrt(2.0*Normalized)-1.0,1.0-np.sqrt(2.0-2.0*Normalized));


if(__name__=="__main__"):
    #GenerateBlueNoiseDatabase(range(64),16,64,range(1,5),1.9);
    #GenerateBlueNoiseDatabase(range(16),128,128,range(1,5),1.9);
    #GenerateBlueNoiseDatabase(range(8),256,256,range(1,5),1.9);
    #GenerateBlueNoiseDatabase(range(1),512,512,range(1,5),1.9);
    #GenerateBlueNoiseDatabase(range(1),1024,1024,[4],1.9);
    #for nChannel in range(1,5):
        #Generate3DBlueNoiseTexture(16,16,16,nChannel,1.9);
        #Generate3DBlueNoiseTexture(32,32,32,nChannel,1.9);
        #Generate3DBlueNoiseTexture(64,64,64,nChannel,1.9);
        #ChannelNames=["","L","LA","RGB","RGBA"][nChannel];
        #GenerateNDBlueNoiseTexture((8,8,8,8),nChannel,"../Data/8_8_8_8/HDR_"+ChannelNames+".raw",1.9);
        #GenerateNDBlueNoiseTexture((16,16,16,16),nChannel,"../Data/16_16_16_16/HDR_"+ChannelNames+".raw",1.9);
    Texture=GetVoidAndClusterBlueNoise((64,64),1.9);
    #Texture=GetVoidAndClusterBlueNoise((32,32,32),1.9)[:,:,0];
    AnalyzeNoiseTexture(Texture,True);
    PlotBinaryPatterns(Texture,3,5);
    pyplot.show();
