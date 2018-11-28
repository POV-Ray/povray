import numpy as np
import getopt, sys

from BlueNoise import GetVoidAndClusterBlueNoise, StoreNoiseTextureLDR, StoreNoiseTextureHDR

defaultInit  = "0.1";
defaultSigma = "1.5";

def printerr(s):
    sys.stderr.write("%s\n" % s);

def UsageError():
    printerr("metagen-bluenoise.py");
    printerr("    Create tiling blue noise pattern as a C-style array of integers,");
    printerr("    using a void-and-cluster algorithm.");
    printerr("Usage:");
    printerr("    python metagen-bluenoise.py [options] <name> <width> [<height> [<slices>]]");
    printerr("Parameters:");
    printerr("    <name>:     Identifier base name for the pattern.");
    printerr("    <width>:    Width of the pattern to generate.");
    printerr("    <height>:   Height of the pattern to generate.");
    printerr("    <slices>:   3D slices of the pattern to generate.");
    printerr("Pattern generation options:");
    printerr("    -i<FLOAT>   Initial seed fraction. Defaults to %s" % defaultInit);
    printerr("    -r<INT>     Random seed. Allows to re-produce patterns.");
    printerr("    -s<FLOAT>   Standard deviation of filter used in pattern generation.");
    printerr("                Higher values generate a coarser pattern. Defaults to %s." % defaultSigma);
    printerr("Output options:");
    printerr("    -h          Generates declaration only.");
    printerr("    -l          Generates linear array instead of 2D array.");
    printerr("    -n<STRING>  Wraps the declaration/definition in a namespace.");
    printerr("    -p<FILE>    Write a copy of the pattern to PNG file. Primarily intended");
    printerr("                for testing.");
    sys.exit(2);

def Error(s):
    printerr(s);
    sys.exit(2);

header    = False;
initStr   = defaultInit;
init      = float(initStr);
linear    = False;
pngfile   = "";
sigmaStr  = defaultSigma;
sigma     = float(sigmaStr);
haveSeed  = False;
namespace = "";

try:
    opts, args = getopt.getopt(sys.argv[1:], "hi:ln:p:r:s:");

except getopt.GetoptError as err:
    UsageError();

try:
    for o, a in opts:
        if (o == "-h"):
            header = True;
        elif (o == "-i"):
            initStr = a;
            init = float(a);
        elif (o == "-l"):
            linear = True;
        elif (o == "-n"):
            namespace = a;
        elif (o == "-p"):
            pngfile = a;
        elif (o == "-r"):
            haveSeed = True;
            seed = int(a);
        elif (o == "-s"):
            sigmaStr = a;
            sigma = float(a);
        else:
            Error("Unrecognized option %s" % (o, a));
            assert False, "unhandled option";

    argcount = len(args);
    if (argcount < 2):
        printerr("Not enough parameters.");
        UsageError();
    if (argcount > 4):
        printerr("Too many parameters.");
        UsageError();

    name = args[0];
    width = int(args[1]);
    if (argcount > 2):
        height = int(args[2]);
    else:
        height = width;
    if (argcount > 3):
        slices = int(args[3]);
    else:
        slices = 0;

except ValueError as err:
    UsageError();

size = width * height;
if (slices == 0):
    zCount = 1;
else:
    zCount = slices;
size3d = size * zCount;

if (size > 1024 * 1024):
    Error("Excessively large pattern. 1024x1024 should be enough for everybody.");

if (size3d <= 256):
    type = "unsigned char";
elif (size3d <= 65536):
    type = "unsigned short";
else:
    type = "unsigned int";

if (size3d <= 10):
    format = "%1i";
elif (size3d <= 100):
    format = "%2i";
elif (size3d <= 1000):
    format = "%3i";
elif (size3d <= 10000):
    format = "%4i";
elif (size3d <= 100000):
    format = "%5i";
elif (size3d <= 1000000):
    format = "%6i";
elif (size3d <= 10000000):
    format = "%7i";
else:
    format = "%i";
itemsPerLine = 16;

if (namespace):
    print("namespace %s {" % namespace)
    print

if (linear):
    if (slices == 0):
        declaration = "extern const %s %s[%i]" % (type, name, size);
    else:
        declaration = "extern const %s %s[%i][%i]" % (type, name, slices, size);
else:
    if (slices == 0):
        declaration = "extern const %s %s[%i][%i]" % (type, name, height, width);
    else:
        declaration = "extern const %s %s[%i][%i][%i]" % (type, name, slices, height, width);

if (header):
    print("%s;" % declaration);
else:
    if (haveSeed):
        np.random.seed(seed);
    if (slices == 0):
        Texture=GetVoidAndClusterBlueNoise((width,height),sigma,init);
    else:
        Texture=GetVoidAndClusterBlueNoise((width,height,slices),sigma,init);
    if (pngfile != ""):
        if (size3d <= 256):
            StoreNoiseTextureLDR(Texture,pngfile);
        else:
            StoreNoiseTextureHDR(Texture,pngfile);
    print("/// %ix%i blue noise pattern data." % (width, height));
    info = "-i%s -s%s" % (initStr, sigmaStr);
    if (haveSeed):
        info = info + " -r%i" % seed;
    print("/// Generated using `make-bluenoise.py %s`." % info);
    if (slices == 0):
        print("%s = {" % declaration);
    else:
        print("%s = {{" % declaration);
    for z in range(zCount):
        if ((slices != 0) and (z > 0)):
            print("  },{");
        for y in range(width):
            data = ""
            for x in range(height):
                if (x > 0):
                    data = data + ",";
                    if (x % itemsPerLine == 0):
                        if (linear):
                            data = data + "\n  ";
                        else:
                            data = data + "\n    ";
                if (slices == 0):
                    value = Texture.item((x,y));
                else:
                    value = Texture.item((x,y,z));
                data = data + (format % value);
            if (linear):
                print("  %s," % data);
            else:
                print("  { %s }," % data);
    if(slices == 0):
        print("  };");
    else:
        print("  }};");

if (namespace):
    print
    print("}")
