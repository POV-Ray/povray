/* ---------------------------------------------------------------------------
*  LANGUE.C
*
*
*  from POVLAB 3D Modeller
*  Copyright 1994-1999 POVLAB Authors.
*  ---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POVLAB and to port the software to platforms other
*  than those supported by the POVLAB authors. There are strict rules under
*  which you are permitted to use this file. The rules are in the file
*  named LEGAL.TXT which should be distributed with this file.
*  If LEGAL.TXT is not available or for more info please contact the POVLAB
*  primary author by leaving a message on http://www.povlab.org
*  The latest and official version of POVLAB may be found at this site.
*
*  POVLAB was originally written by Denis Olivier.
*
*  ---------------------------------------------------------------------------*/

#include <MATH.H>
#include <FLOAT.H>

#include "GLOBAL.H"

// ----------------------------------------------------------------------------
// --------------- MESSAGES EN FRANCAIS (english)? ----------------------------
// ----------------------------------------------------------------------------

// ------------------------------------- Messages pour copyrights

char *RES_COPY[50]={
/*000*/  "About PovLab",
/*001*/  "3D modeller for",
/*002*/  "POV-RAY",
/*003*/  "3.0",
/*004*/  "1998"
};

// ------------------------------------- Menus droite

char *RES_BOUT[50]={
/*000*/  "Object",
/*001*/  "Camera",
/*002*/  "Light",
/*003*/  "Material",
/*004*/  "Selection",
/*005*/  "Render",
/*006*/  "Modify",
/*007*/  "Display",
/*008*/  "Ok",
/*009*/  "Cancel",
/*010*/  "Abort",
/*011*/  "Ok",
/*012*/  "CSG",
};

// ------------------------------------- Aides en ligne

char *RES_AIDE[200]={
/*000*/  "Create new object",
/*001*/  "Manage cameras",    //<-- singular || plural? possibly 'Edit cameras'.
/*002*/  "Manage lights",     //<-- singular || plural? possibly 'Edit lights'.
/*003*/  "Choose and assign", //<-- do you mean 'choose and asign'? possibly 'Select and assign'.
/*004*/  "Object selection",
/*005*/  "Render image with POV",
/*006*/  "Working on objects",  //<-- do you mean 'working on scene'?
/*007*/  "Display and hide objects",
/*008*/  "Display copyrights",
/*009*/  "Modify directories, ...",
/*010*/  "Machine parameters",
/*011*/  "Quit modeller",
/*012*/  "Shell to DOS",
/*013*/  "Clear all (new scene)",
/*014*/  "Rename current scene",
/*015*/  "Load a new scene",
/*016*/  "Save current scene",
/*017*/  "Make POV-Ray script",
/*018*/  "Redraw viewports",
/*019*/  "Redraw current viewport",
/*020*/  "Display or not axes",
/*021*/  "Display or not grid",       //<-- if this is a 'check' box this coulc be 'Display Grid'.
/*022*/  "Fast object display",
/*023*/  "Modeller color scheme",
/*024*/  "16 or 256 modeller color scheme",
/*025*/  "Enable online help (here)",
/*026*/  "Beep on command completion",
/*027*/  "640x480 interface",
/*028*/  "800x600 interface",
/*029*/  "1024x768 interface",
/*030*/  "Do command...",             //<-- where is this used, never seen it before!
/*031*/  "Don't make command...",     //<-- where is this used, never seen it before!
/*032*/  "Another day...",            //<-- where is this used, never seen it before!
/*033*/  "Background color for viewports",
/*034*/  "Background color for text zone",
/*035*/  "Axes color",
/*036*/  "Grid color",
/*037*/  "Create object 'cylinder'",
/*038*/  "Create object 'sphere'",
/*039*/  "Create object 'cube'",
/*040*/  "Create object 'simple cone'",
/*041*/  "Create object 'torus'",
/*042*/  "Create object 'tube'",
/*043*/  "Create object 'X normal plane (on Y/Z)'",
/*044*/  "Create object 'Y normal plane (on X/Z)'",
/*045*/  "Create object 'Z normal plane (on Y/X)'",
/*046*/  "Change color of an object",
/*047*/  "Load a raw triangles object",
/*048*/  "Move camera along x,y,z",
/*049*/  "Control focal blur",
/*050*/  "Control aperture",
/*051*/  "Camera color in scene",
/*052*/  "Hide camera in scene",
/*053*/  "Show camera in scene",
/*054*/  "Create a new omnilight",
/*055*/  "Delete an omnilight",
/*056*/  "Move an omnilight along x,y,z",
/*057*/  "Change light color",
/*058*/  "Change visual omni size",
/*059*/  "Hide/show omnis in scene",
/*060*/  "Hide/show spots in scene",
/*061*/  "Select an object",
/*062*/  "Select all objects",
/*063*/  "Remove selection",
/*064*/  "Invert current selection",
/*065*/  "Select objects by area",
/*066*/  "Select last created object",
/*067*/  "Change selection color",
/*068*/  "Select by object's name",
/*069*/  "Fast preview in 128x96",
/*070*/  "Fast preview in 320x240",
/*071*/  "Fast preview in 640x480",
/*072*/  "Final render parameters",
/*073*/  "Select objects by texture",
/*074*/  "Acquire texture from object",
/*075*/  "Assign texture to object",
/*076*/  "Rotate object(s) x,y,z",
/*077*/  "2D scale object(s)",
/*078*/  "3D scale object(s) x,y,z",
/*079*/  "Translate object(s) x,y,z",
/*080*/  "Copy object (clone)",
/*081*/  "Delete object from scene",
/*082*/  "Show attributes of object",
/*083*/  "Center object on x=y=z=0",
/*084*/  "Re-init an object",
/*085*/  "Enter coordinates x,y,z",
/*086*/  "Modify blob parameters",
/*087*/  "Hide object in scene",
/*088*/  "Display all objects in scene",
/*089*/  "Hide all objects in scene",
/*090*/  "Invert show and hide",
/*091*/  "Hide last object",
/*092*/  "Hide selection",
/*093*/  "Zoom - in viewport (ALTx3)",
/*094*/  "Zoom + out viewport (ALTx3)",
/*095*/  "Center scene in viewport",
/*096*/  "Select an area and zoom in",
/*097*/  "Pan scene in viewport",
/*098*/  "Full screen viewport",
/*099*/  "Use selection on/off",
/*100*/  "Display buttons on right",
/*101*/  "Enter object's coordinates",
/*102*/  "Normal display for object",
/*103*/  "Cubic diplay for object",
/*104*/  "Interface texture",
/*105*/  "Save scene on exit",
/*106*/  "Create object 'Ring'",
/*107*/  "Create object 'Disk'",
/*108*/  "Background trace color",
/*109*/  "Add new spot light",
/*110*/  "Spotlight fall off",
/*111*/  "Spotlight hot spot",
/*112*/  "Rotate texture x,y,z",
/*113*/  "Scale texture 2D",
/*114*/  "Scale texture 3D",
/*115*/  "Translate texture x,y,z",
/*116*/  "Power tools for creation",
/*117*/  "Fade when exit/enter modeller",
/*118*/  "Memory manager",
/*119*/  "100% Microsoft compatible mouse",
/*120*/  "Configure right mouse button",
/*121*/  "Select camera area to render",       //<-- possibly 'Select camera to render with'.
/*122*/  "Smooth or not a raw object",
/*123*/  "Fog Parameters",                   //<-- possibly 'Fog parameters'.
/*124*/  "Fog in raytraced image",
/*125*/  "Object selection by texture",
/*126*/  "Save current selection",
/*127*/  "Merge .scn file with current scene", //<-- sounds/feels clumsy, i'll work on it.
/*128*/  "Reset rotation, scale, ...",
/*129*/  "Align an create objects",            //<-- possibly 'Align with object'. tough one this.
/*130*/  "Show bounding box while searching",
/*131*/  "Freeze object - CTRL=unfreeze all ",
/*132*/  "Ignore object while re-centering",
/*133*/  "Create new object",
/*134*/  "Create a 'hemi-sphere'",             //<-- try and use a consistent naming scheme, 'a' or not to 'a', that is the question.
/*135*/  "Create a 'quarter of torus'",        //<-- as above and all other 'Create ......' fields.
/*136*/  "Create 'spherical blob'",
/*137*/  "Modify object's name",
/*138*/  "Edit special parmeters",
/*139*/  "Create a 'false prism'",
/*140*/  "Redraw viewport (ALTx3)",
/*141*/  "Create a 'quarter of tube'",
/*142*/  "Display scene parameters",
/*143*/  "Create a 'truncated cone'",
/*144*/  "Mirror object (TAB=Axis)",
/*145*/  "Show/hide area lights in scene",
/*146*/  "Boolean operation difference",
/*147*/  "Boolean operation union",
/*148*/  "Boolean operation intersection",
/*149*/  "Boolean operations (CSG) between objects",
/*150*/  "Remove an CSG operator (not the object)",    //<-- possibly 'Remove a Operator operation (not the object).
/*151*/  "Show all CSG objects",
/*152*/  "Hide all CSG objects",
/*153*/  "Add new camera",
/*154*/  "Delete camera from scene",
/*155*/  "Enable light source",
/*156*/  "Enable cast shadow",
/*157*/  "Copy objects by rotation",
/*158*/  "Select palette for rendering",
/*159*/  "Create a 'pyramid'",
/*160*/  "Boolean operation merge",
/*161*/  "Sources for arealight",          //<-- possibly 'Sources for arealight'.
/*162*/  "Align object top/bottom-left/right",
/*163*/  "Create 'cylindric blob'",
};

// ------------------------------------- Menu

char *RES_MENU[100]={
/*000*/  "Environment",
/*001*/  "System",
/*002*/  "Quit",
/*003*/  "Ms-dos",
/*004*/  "File",
/*005*/  "General",
/*006*/  "New",
/*007*/  "Rename",
/*008*/  "Load",
/*009*/  "Save",
/*010*/  " Display",
/*011*/  "Redraw view",
/*012*/  "Redraw viewports",
/*013*/  "Display axes",
/*014*/  "Display grid",
/*015*/  "Fast display",
/*016*/  "Interface",
/*017*/  "Colors",
/*018*/  "256c interface",
/*019*/  "In line help",
/*020*/  "Beep on completion",
/*021*/  "640x480",
/*022*/  "800x600",
/*023*/  "1024x768",
/*024*/  "Right mouse button",
/*025*/  "Normal display",
/*026*/  "Cubic display",
/*027*/  "Texturing interface",
/*028*/  "Save on exit",
/*029*/  "Extruder",
/*030*/  "Tools",
/*031*/  "Fading palette",
/*032*/  "Virtual memory",
/*033*/  "Microsoft mouse",
/*034*/  "Configure mouse",
/*035*/  "",
/*036*/  "Save selection",
/*037*/  "Merge .scn",
/*038*/  "Copy by translation",
/*039*/  "Trace selection",
/*040*/  "Objects",
/*041*/  "Cylinder",
/*042*/  "Sphere",
/*043*/  "Box",
/*044*/  "Cone",
/*045*/  "Torus",
/*046*/  "Tube",
/*047*/  "X-plane (YZ)",
/*048*/  "Y-plane (XZ)",
/*049*/  "Z-plane (XY)",
/*050*/  "Raw",
/*051*/  "Ring",
/*052*/  "Disk",
/*053*/  "1/2 sphere",
/*054*/  "1/4 torus",
/*055*/  "Spherical blob",   //<-- possibly 'Object blob (sphere).
/*056*/  "False prism",
/*057*/  "1/4 tube",
/*058*/  "Statistic scene",
/*059*/  "Truncated cone",
/*060*/  "Display area lights",
/*061*/  "Display omni lights",
/*062*/  "Display spot lights",
/*063*/  "Copy by rotation",
/*064*/  "Pyramid",
/*065*/  "Object align",
/*066*/  "Cylindrical blob",
};

// ------------------------------------- Message

char *RES_MESS[100]={
/*000*/  "Select camera or target (TAB=moving direction)",
/*001*/  "Sharp zone",
/*002*/  "FOV",
/*003*/  " RENDER SCENE WITH POV-RAY",
};

