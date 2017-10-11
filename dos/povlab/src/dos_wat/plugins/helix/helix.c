/*
	helix.c

	COMPILER:
		Borland C++ 3.1
		Watcom 10.x

	DESCRIPTION:
		This file is a plugin for POVLAB 3.2 onwards.
		This file creates a helix of objects with the ability
			to change various parameters.

	CONTENTS:
		1 to 6 heli of objects.
		Clockwise or anticlockwise helix creation.
		Y axis rotation offset for the entire helix.
		Variable length Helix.
		Static or changing helix radius over length.
		Variable number of objects per revolution of helix.
		Scaling of objects in helix from unity in X, Y and Z directions.
		Rotation of objects within the helix in X, Y and Z directions.
		The ability to face all objects towards the center of the helix.

	NOTE:
		All of the above can be performed in _any_ combination.
		This file was written with the TAB key defined as 3 spaces.
		The commandline argument created by POVLAB using this plugin is VERY
		  long, in some cases you may run into truncation problems.  Hopefully
		  this risk will be fixed at sometime in the future.

	CopyLeft Robert Hodkinson (r.j.hodkinson@bradford.ac.uk).
		Started: early March 1996.  Current: early  April 1996.
*/


#if __WATCOMC__
  #include <i86.h>
#else
  #include <dos.h>
#endif

#include <io.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>

/* global declarations */
#define RAD			(180/M_PI)	// number of degrees in a radian


/* declare prototypes */
int    OutDataInterface(void);
void	 MakeHelix(double);
void	 ExtractParameters(char *argv[]);
void   CalculateHelix(void);
void   CalculateSetValues(void);

double CalcXcoord(double, double);
double CalcYcoord(double);
double CalcZcoord(double, double);

double CalcXrotation(void);
double CalcYrotation(double);
double CalcZrotation(void);


/* global variables filled from argv[] (in this order) */
double HELIXRADSTART;	// start radius of the helix
double HELIXRADEND;		// end   radius of the helix

double HELIXLENGTH;		// length of helix

int    OBJECTSPERREV;	// number of spheres per 360 degrees revolution
double LENGTHPERREV;		// distance covered per helix revolution

double HELIXOFFSETX;		// translation offset for the helix
double HELIXOFFSETY;		// translation offset for the helix
double HELIXOFFSETZ;		// translation offset for the helix

double HELIXROTATEY;		// rotation of the entire helix on the Y axis

double OBJECTSCALEX;		// object scale on the X axis
double OBJECTSCALEY;    // object scale on the Y axis
double OBJECTSCALEZ;    // object scale on the Z axis

int    ROTATETOFACE;		// if enabled, will allow mass rotation objects
double OBJECTROTATEX;	// object rotate on the X axis
double OBJECTROTATEY;	// object rotate on the Y axis
double OBJECTROTATEZ;	// object rotate on the Z axis

int    STARTOBJNO;		// which object number to start at

int    NOOFHELIX;			// number of helix in cylinder

int    OBJECTTYPE;		// type of object to use in the helix

int    HELIXCLOCKWISE;	// is the helix clockwise or anticlock


/* global variables caclulated before making helix */
FILE *Helix;				// file stream to be used by helix1.c

double RadiansPerObject;	// rotation per object in helix (radians)

double HelixRadiusStart;			// acutal minimum raidus of helix
double HelixRadiusIncPerObject;	// radius incramented/decramented per sphere in helix

double LengthPerObject;		// length incramented for each degree incrament

double HelixRotationY;		// helix rotation offset

int    Objno;					// incramental object numbering


/*	main program */

/*
	function - root of main loop to create helix
*/
void main (int argc,char *argv[])
{
	union REGS regs;
	int        i;

	// do we have arguments and do they start correctly?
	if(argc>1)
	{
		if(strcmp(strupr(argv[1]),"/ASK")==NULL)
		{
			// create PLG file
			OutDataInterface();
			exit(0);
		}
	}
	else
	{
		exit(0);
	}

	// test stuff for argument length, KEEP!!
/*	printf("argv's:");

	for(i=0;i<=20;i++)
		printf("%s:", argv[ i]);

	printf("end\n");
	printf("paused....\n");
	getch();
*/
	// clear screen
	#if __WATCOMC__
		regs.w.ax=3;
	#else
		regs.x.ax=3;
	#endif
	int86(0x10,&regs,&regs); // Textmode with int 10H

	// copyleft and all that useless junk
	puts("");
	printf("Helix maker (Helix.exe), (L) Robert Hodkinson, 1996.\n");
	printf("External process for POVLAB - \n");
	printf("To Generate all sorts of wierd helix.\n");
	puts("");

	// extract parameters sent by POVLAB
	ExtractParameters(argv);

	// create .inc file to store all the stuff in.
	Helix = fopen("Helix.inc","w+t");

	// calculate all set values for use
	CalculateSetValues();

	// calculate helix ALL GO!
	CalculateHelix();
}




/*
	function - to create the plg file for povlab to use
*/
int OutDataInterface()
{
	// open the plg file
	if(!(Helix = fopen("Helix.plg","wt")))
		return 0;

	// fill the plg file
	fprintf(Helix, "TITLE: The_wierd_and_wonderful_Helix_maker_(Helix)_v1.0\n");
	fprintf(Helix, "COPYRIGHT: CopyLeft_Robert_Hodkinson_1996_-_All_rights_reserved.\n");
	fprintf(Helix, "WINDOW:   530 290 \n");

	// start of first column of options
	fprintf(Helix, "TEXTZONE:  90  50 Helix_radius_start  5 Radius_of_Helix_to_create \n");
	fprintf(Helix, "TEXTZONE:  90  70 Helix_radius_end    5 Radius_of_Helix_to_create \n");

	fprintf(Helix, "TEXTZONE:  90 100 Helix_length       10 Length of_Helix_to_create \n");

	fprintf(Helix, "TEXTZONE:  90 130 Objects_per_rev    20 Number_of_Helix_per_revolution   \n");
	fprintf(Helix, "TEXTZONE:  90 150 Length_per_rev     10 Distance_covered_pre_revolution  \n");

	fprintf(Helix, "TEXTZONE:  90 180 Helix_offset_X      0 Helix_translation_in_the_X_axis  \n");
	fprintf(Helix, "TEXTZONE:  90 200 Helix_offset_Y      0 Helix_translation_in_the_Y_axis  \n");
	fprintf(Helix, "TEXTZONE:  90 220 Helix_offset_Z      0 Helix_translation_in_the_Z_axis  \n");

	fprintf(Helix, "TEXTZONE:  90 250 Helix_rotate_Y      0 Helix_rotation_in_the_Y_axis     \n");

	// start of second column of options
	fprintf(Helix, "TEXTZONE: 280  50 Object_No_Start   100 First_Object_Number_to_be_used   \n");

	fprintf(Helix, "TEXTZONE: 280  80 Object_scale_X      1 Scale_object_in_the_X_dimension  \n");
	fprintf(Helix, "TEXTZONE: 280 100 Object_scale_Y      1 Scale_object_in_the_Y_dimension  \n");
	fprintf(Helix, "TEXTZONE: 280 120 Object_scale_Z      1 Scale_object_in_the_Z_dimension  \n");

	fprintf(Helix, "TEXTZONE: 280 150 Object_rotate_X     0 Rotate_object_in_the_X_dimension \n");
	fprintf(Helix, "TEXTZONE: 280 170 Object_rotate_Y     0 Rotate_object_in_the_Y_dimension \n");
	fprintf(Helix, "TEXTZONE: 280 190 Object_rotate_Z     0 Rotate_object_in_the_Z_dimension \n");

	fprintf(Helix, "RADIO:    280 220 Object_Sphere       1 Create_a_Helix_using_spheres   1 \n");
	fprintf(Helix, "RADIO:    280 240 Object_Cube         0 Create_a_Helix_using_cubes     1 \n");

	// start of thrid row of options
	fprintf(Helix, "RADIO:    390  50 Single_Helix        1 Create_a_single_Helix_shape    2 \n");
	fprintf(Helix, "RADIO:    390  70 Double_Helix        0 Create_a_double_Helix_shape    2 \n");
	fprintf(Helix, "RADIO:    390  90 Triple_Helix        0 Create_a_triple_Helix_shape    2 \n");
	fprintf(Helix, "RADIO:    390 110 Quadruple_Helix     0 Create_a_quadruple_Helix_shape 2 \n");
	fprintf(Helix, "RADIO:    390 130 Quintple_Helix      0 Create_a_quintple_Helix_shape  2 \n");
	fprintf(Helix, "RADIO:    390 150 Hextaple_Helix      0 Create_a_hextaple_Helix_shape  2 \n");

	fprintf(Helix, "RADIO:    390 180 Clockwise_Helix     1 Create_a_clockwise_Helix       3 \n");
	fprintf(Helix, "RADIO:    390 200 Anticlock_Helix     0 Create_an_anticlockwise_Helix  3 \n");

	fprintf(Helix, "CASE:     390 230 Face_Object_inwards 0 If_not_enabled,_objects_will_all_face_the_same_way \n");

	fprintf(Helix, "MESSAGE: The_Helix_cylinder_is_created_in_the_X_&_Y_axis_from_<0,0,0> \n");
	fprintf(Helix, "END:\n");

	fclose(Helix);

	return 1;
}



/*
	function - to extract the needed parameters from POVLAB into helix1.c
*/
void ExtractParameters(char *argv[])
{
	int i;

	i = 1;

	HELIXRADSTART  = atof(argv[ i]);
	i++;
	HELIXRADEND	   = atof(argv[ i]);
	i++;
	HELIXLENGTH    = atof(argv[ i]);
	i++;
	OBJECTSPERREV  = atof(argv[ i]);
	i++;
	LENGTHPERREV   = atof(argv[ i]);
	i++;
	HELIXOFFSETX   = atof(argv[ i]);
	i++;
	HELIXOFFSETY   = atof(argv[ i]);
	i++;
	HELIXOFFSETZ   = atof(argv[ i]);
	i++;
	HELIXROTATEY   = atof(argv[ i]);
	i++;

	// end of first column
	STARTOBJNO     = atof(argv[ i]);
	i++;
	OBJECTSCALEX   = atof(argv[ i]);
	i++;
	OBJECTSCALEY   = atof(argv[ i]);
	i++;
	OBJECTSCALEZ   = atof(argv[ i]);
	i++;

	OBJECTROTATEX  = atof(argv[ i]);
	i++;
	OBJECTROTATEY  = atof(argv[ i]);
	i++;
	OBJECTROTATEZ  = atof(argv[ i]);
	i++;
	OBJECTTYPE     = atof(argv[ i]) + 1; // 1 = sphere, 2 = cube
	i++;

	// end of second column
	NOOFHELIX      = atof(argv[ i]);
	i++;
	HELIXCLOCKWISE = atof(argv[ i]);
	i++;
	ROTATETOFACE   = atof(argv[ i]);


	// give a status for the user to jaw wag at (not!)
	// first column of options
	printf("Helix Radius Start       :%f:\n", HELIXRADSTART);
	printf("Helix Radius End         :%f:\n", HELIXRADEND);
	printf("Helix Length             :%f:\n", HELIXLENGTH);
	printf("Objects per revolution   :%d:\n", OBJECTSPERREV);
	printf("Length  per revolution   :%f:\n", LENGTHPERREV);
	printf("Helix translation X      :%f:\n", HELIXOFFSETX);
	printf("Helix translation Y      :%f:\n", HELIXOFFSETY);
	printf("Helix translation Z      :%f:\n", HELIXOFFSETZ);
	printf("Helix rotation Y         :%f:\n", HELIXOFFSETZ);

	// second column of options
	printf("Object type              :%d:\n", OBJECTTYPE);
	printf("Object scale X           :%f:\n", OBJECTSCALEX);
	printf("Object scale Y           :%f:\n", OBJECTSCALEY);
	printf("Object scale Z           :%f:\n", OBJECTSCALEZ);
	printf("Object rotation X        :%f:\n", OBJECTROTATEX);
	printf("Object rotation Y        :%f:\n", OBJECTROTATEY);
	printf("Object rotation Z        :%f:\n", OBJECTROTATEZ);
	printf("Object start number      :%d:\n", STARTOBJNO);

	// third column of options
	printf("Number of Helix          :%d:\n", NOOFHELIX);
	printf("Anticlock or clock helix :%d:\n", HELIXCLOCKWISE);
	printf("Object face helix enable :%d:\n", ROTATETOFACE);
}



/*
	function - calculate all preset values for use in main engine
*/
void CalculateSetValues()
{
	//calculate radian incrament per object
	RadiansPerObject = (double) (360/RAD) / OBJECTSPERREV;

	//correct above value to clockwise (0)/anticlockwise (1) helix
	if(HELIXCLOCKWISE == 0)
		RadiansPerObject = RadiansPerObject;
	else
		RadiansPerObject = RadiansPerObject * (-1);

	// calculate incramented helix length for each sphere
	LengthPerObject = (double) LENGTHPERREV / OBJECTSPERREV;

	// is the helix being made going down
	if(HELIXLENGTH < 0)
		LengthPerObject = LengthPerObject * (-1);


	// calculate the minimum helix size
	if(HELIXRADSTART == HELIXRADEND)
	{  // start and end radius are the same, helix is a cylinder
		HelixRadiusStart        = HELIXRADSTART;
		HelixRadiusIncPerObject = 0;
	}
	else
	{  // start and end radius are diferent, helix is a cone
		HelixRadiusStart        = HELIXRADSTART;
		HelixRadiusIncPerObject = (double) ((HELIXRADEND-HELIXRADSTART) / (HELIXLENGTH/LengthPerObject));
	}

	// load up the starting object number in the helix
	Objno = STARTOBJNO;

	// calculate the helix rotation offset for the helix in radians
	HelixRotationY = (float) HELIXROTATEY / RAD;

	// tmp stuff for debugging
	printf("RadiansPerObject         :%f:\n", RadiansPerObject);
	printf("LengthPerObject          :%f:\n", LengthPerObject);
	printf("HelixRadiusIncPerObject  :%f:\n", HelixRadiusIncPerObject);
	printf("HelixRadiusStart         :%f:\n", HelixRadiusStart);
	printf("HelixRotationY           :%f:\n", HelixRotationY);
}


/*
	function - root of the calculation for the helix
*/
void CalculateHelix()
{
	double tmp;

	// how many helix we have to do?
	if(NOOFHELIX == 0)
	{
		tmp = 0;
		MakeHelix( (double) 0*tmp );
	}
	else
	if(NOOFHELIX == 1)
	{
		tmp = (double) (M_PI);
		MakeHelix( (double) 0*tmp );
		MakeHelix( (double) 1*tmp );
	}
	else
	if(NOOFHELIX == 2)
	{
		tmp = (double) ((2*M_PI)/3);
		MakeHelix( (double) 0*tmp );
		MakeHelix( (double) 1*tmp );
		MakeHelix( (double) 2*tmp );
	}
	else
	if(NOOFHELIX == 3)
	{
		tmp = (double) ((2*M_PI)/4);
		MakeHelix( (double) 0*tmp );
		MakeHelix( (double) 1*tmp );
		MakeHelix( (double) 2*tmp );
		MakeHelix( (double) 3*tmp );
	}
	else
	if(NOOFHELIX == 4)
	{
		tmp = (double) ((2*M_PI)/5);
		MakeHelix( (double) 0*tmp );
		MakeHelix( (double) 1*tmp );
		MakeHelix( (double) 2*tmp );
		MakeHelix( (double) 3*tmp );
		MakeHelix( (double) 4*tmp );
	}
	else
	if(NOOFHELIX == 5)
	{
		tmp = (double) ((2*M_PI)/6);
		MakeHelix( (double) 0*tmp);
		MakeHelix( (double) 1*tmp);
		MakeHelix( (double) 2*tmp);
		MakeHelix( (double) 3*tmp);
		MakeHelix( (double) 4*tmp);
		MakeHelix( (double) 5*tmp);
	}

	// close down helix1.inc file
	fclose(Helix);

	// say that we are finished
	printf("\nYour column of spheres await!\n");
	printf("\nHit a key to continue...\n");
	getch();
}


/*
	function to acutally produce a helix
*/
void MakeHelix(double rotationcounter)
{

	double x;	// x co-ord
	double y;	// y co-ord
	double z;	// z co-ord

	double xrot; // x rotation
	double yrot; // y rotation
	double zrot; // z rotation

	double lengthcounter;  // Helix length change accumulator counter
	double radiuscounter;  // Helix radius change accumulator counter

	// add any user rotation offset to the rotation start value
	rotationcounter = rotationcounter + HelixRotationY;

	// continue until helix length has been covered
	for(lengthcounter = 0, radiuscounter = 0;
		 fabs(lengthcounter) < fabs(HELIXLENGTH);
		 lengthcounter   += LengthPerObject,
		 rotationcounter += RadiansPerObject,
		 radiuscounter   += HelixRadiusIncPerObject)
	{
		// update x co-ord
		x    = CalcXcoord(rotationcounter, radiuscounter);

		// update y co-ord
		y    = CalcYcoord(lengthcounter);

		// update z co-ord
		z    = CalcZcoord(rotationcounter, radiuscounter);

		// update the x rotation value
		xrot = CalcXrotation();

		// update the y rotation value
		yrot = CalcYrotation(rotationcounter);
		// update the z rotation value
		zrot = CalcZrotation();

		// print current information to helixrod.inc
		fprintf(Helix,"\n"); // You need this line first

		fprintf(Helix,"Object %05d: %d\n",Objno,OBJECTTYPE);

		fprintf(Helix,"Object %05d: 0 0 0\n",Objno);		// special vectors

		fprintf(Helix,"Object %05d: %.4f %.4f %.4f\n",Objno, (float) OBJECTSCALEX,  (float) OBJECTSCALEY,  (float) OBJECTSCALEZ);	// scale
		fprintf(Helix,"Object %05d: %.4f %.4f %.4f\n",Objno, (float) xrot,          (float) yrot,          (float) zrot);									// rotate
		fprintf(Helix,"Object %05d: %.4f %.4f %.4f\n",Objno, (float) x,             (float) y,             (float) z);				// translate

		fprintf(Helix,"Object %05d: 7\n",Objno);
		fprintf(Helix,"Object %05d: Default\n",Objno);

		Objno = Objno + 1;

		// give the user a status report
		printf("\rCurrent object being created:%.4d:.", Objno);

		if ((kbhit()) && getch()==27)
		{
			fclose(Helix);
			remove("Helix.INC");
			exit(0);
		}
	}
}

// this is a list of what the above fprintf parameters do.

// object number, object type

// object number, special vectors (blob, torus vectors)
// object number, x,y,z scale (reference by 1,1,1 object)
// object number, x,y,z rotate object
// object number, x,y,z translate object

// object number, colour
// object number, texture

/*
	functions - 3 of them
					calculate the X, Y and Z co-ordinate for the object in question
	NOTE: the (-1)'s are there because of the co-ordinate description
			system used in POVLAB (Y and Z co-ords only.
*/
double CalcXcoord(double rotationcounter, double radiuscounter)
{
	return (((HelixRadiusStart + radiuscounter) * sin(rotationcounter)) +
				 HELIXOFFSETX);
}


double CalcYcoord(double lengthcounter)
{
	return ( (lengthcounter + HELIXOFFSETY) * (-1));
}


double CalcZcoord(double rotationcounter, double radiuscounter)
{
	return ((((HelixRadiusStart + radiuscounter) * cos(rotationcounter)) +
				 HELIXOFFSETZ) * (-1));
}



/*
	functions - 3 of them
					All caclulate an axis of rotation, Y is the only one
					that really does something.
*/
double CalcXrotation()
{
	return OBJECTROTATEX;
}


double CalcYrotation(double rotationcounter)
{
	double a;

	if(ROTATETOFACE == 1)
		a = (rotationcounter * RAD) + OBJECTROTATEY;
	else
		a = OBJECTROTATEY;

	// remove multiples of 360 degrees
	while(a > 360)
		a = a - 360;

	while(a < 0)
		a = a + 360;

	return (a * (-1));
}


double CalcZrotation()
{
	return OBJECTROTATEZ;
}

/* END OF FILE */