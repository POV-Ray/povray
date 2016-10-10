/*******************************************************************************
 * scene.h
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/scene/scene.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POVRAY_BACKEND_SCENE_H
#define POVRAY_BACKEND_SCENE_H

#include <vector>
#include <string>
#include <map>

#include <boost/thread.hpp>

#include "base/povmscpp.h"
#include "base/image/image.h" // TODO - this is just for the GammaCurve stuff; find a different place for that
#include "backend/frame.h"
#include "backend/povray.h"
#include "backend/scene/view.h"
#include "backend/scene/atmosph.h"
#include "backend/scene/camera.h"
#include "backend/lighting/photons.h"
#include "backend/lighting/radiosity.h"
#include "backend/control/renderbackend.h"
//#include "backend/support/bsptree.h"

#include "povrayold.h" // TODO

namespace pov
{

using namespace pov_base;

class View;
class Parser;
class FunctionVM;
struct FontFileInfo;

class BSPTree;

/**
 *	SceneData class representing holding scene specific data.
 *	For private use by Scene, View and Renderer classes only!
 *	This class just provides access to scene specific data
 *	needed in many parts of the code. Be aware that while most
 *	data is members are public, they shall *not* be modified
 *	carelessly. Code from POV-Ray 3.6 and earlier does depend
 *	on simple access of this data all over the old code, so
 *	this provides an efficient way to reuse all the old code.
 *	By no means shall this way of data access be used for any
 *	other newly created classes!!!
 */
class SceneData
{
		// Scene needs access to the private scene data constructor!
		friend class Scene;
	public:
		/**
		 *	Destructor.
		 */
		~SceneData();

		/// scene id
		RenderBackend::SceneId sceneId;
		/// backend address
		POVMSAddress backendAddress;
		/// frontend address
		POVMSAddress frontendAddress;

		/// list of all shape objects
		vector<ObjectPtr> objects;
		/// list of all global light sources
		vector<LightSource *> lightSources;
		/// list of all lights that are part of light groups
		vector<LightSource *> lightGroupLightSources;
		/// function vm managing all functions in scene
		FunctionVM *functionVM;
		/// atmosphere index of refraction
		DBL atmosphereIOR;
		/// atmosphere dispersion
		DBL atmosphereDispersion;
		/// atmospheric media
		vector<Media> atmosphere;
		/// background color - TODO - allow pattern here (useful for background image maps) [trf]
		Colour backgroundColour; // may have a filter/transmit component (but filter is ignored)
		/// ambient light in scene
		RGBColour ambientLight;
		/// TODO - what is this again? [trf]
		RGBColour iridWavelengths;
		/// fog in scene
		FOG *fog;
		/// rainbow in scene
		RAINBOW *rainbow;
		/// skyssphere around scene
		SKYSPHERE *skysphere;
		/// language version to assume
		int languageVersion;
		/// true if a #version statement has been encountered
		bool languageVersionSet;
		/// true if the first #version statement was found after other declarations
		bool languageVersionLate;
		/// warning level
		int warningLevel;
		/// string encoding of text
		int stringEncoding;
		/// default noise generator to use
		int noiseGenerator;
		/// whether or not the noise generator was explicitly set by the scene - TODO FIXME remove [trf]
		bool explicitNoiseGenerator;
		/// bounding method selector
		unsigned int boundingMethod;
		/// What gamma mode to use.
		/// One of kPOVList_GammaMode_*.
		int gammaMode;
		/// Working gamma.
		SimpleGammaCurvePtr workingGamma;
		/// Working gamma to sRGB encoding/decoding.
		GammaCurvePtr workingGammaToSRGB;
		/// Whether the user has explicitly specified a default input file gamma. [will be removed in 3.7x]
		bool inputFileGammaSet;
		/// Default assumed decoding gamma of input files.
		SimpleGammaCurvePtr inputFileGamma;

		/// distance scaling factor for subsurface scattering effects
		DBL mmPerUnit;

		/// whether the scene should be rendered with alpha channel
		bool outputAlpha;

		/// whether to use subsurface light transport
		bool useSubsurface;
		/// samples for subsurface scattering diffuse contribution
		int subsurfaceSamplesDiffuse;
		/// samples for subsurface scattering single scattering contribution
		int subsurfaceSamplesSingle;
		/// whether to compute radiosity contribution to subsurface effects
		bool subsurfaceUseRadiosity;

		// ********************************************************************************
		// temporary variables for BSP testing ... we may or may not keep these come 3.7
		// release, depending on whether or not a valid need for them has been demonstrated.
		// ********************************************************************************
		unsigned int bspMaxDepth;
		float bspObjectIsectCost;
		float bspBaseAccessCost;
		float bspChildAccessCost;
		float bspMissChance;

		// set if real-time raytracing is enabled.
		bool realTimeRaytracing;

		// ********************************************************************************
		// Old globals from 3.6 and earlier are temporarily kept below. Please carefully
		// consider what is added and mark it accordingly if it needs to go away again
		// prior to final release! [trf]
		// ********************************************************************************

		/// number of waves to use in waves and ripples pattern
		unsigned int numberOfWaves;

		/// maximum trace recursion level allowed
		unsigned int parsedMaxTraceLevel;
		/// adc bailout
		DBL parsedAdcBailout;
		/// radiosity settings
		SceneRadiositySettings radiositySettings;
		RadiositySettings parsedRadiositySettings;

		/// generated surface photon map data // TODO FIXME - technically camera-independent, but computed for every view [trf]
		PhotonMap surfacePhotonMap;
		/// generated media photon map data // TODO FIXME - technically camera-independent, but computed for every view [trf]
		PhotonMap mediaPhotonMap;

		ScenePhotonSettings photonSettings; // TODO FIXME - is modified! [trf]

		// TODO - decide if we want to keep this here
		FontFileInfo *TTFonts;

		// name of the parsed file
		UCS2String inputFile; // TODO - handle differently
		UCS2String headerFile;

		int defaultFileType;

		FrameSettings frameSettings; // TODO - move ???
		std::map<string, string> declaredVariables; // TODO - move to parser
		Camera parsedCamera; // TODO - handle differently or move to parser
		bool clocklessAnimation; // TODO - this is support for an experimental feature and may be changed or removed
		vector<Camera> cameras; // TODO - this is support for an experimental feature and may be changed or removed

		// this is for fractal support
		int Fractal_Iteration_Stack_Length; // TODO - move somewhere else
		int Max_Blob_Components; // TODO - move somewhere else
		// function pattern support
		unsigned int functionPatternCount; // TODO - move somewhere else
		// lathe and sor support (bounding cylinders)
		unsigned int Max_Bounding_Cylinders; // TODO - move somewhere else
		BBOX_TREE *boundingSlabs;

		// TODO FIXME move to parser somehow
		bool splitUnions; // INI option, defaults to false
		bool removeBounds; // INI option, defaults to true

		// experimental
		BSPTree *tree;
		unsigned int numberOfFiniteObjects;
		unsigned int numberOfInfiniteObjects;

		// BSP statistics // TODO - not sure if this is the best place for stats
		unsigned int nodes, splitNodes, objectNodes, emptyNodes, maxObjects, maxDepth, aborts;
		float averageObjects, averageDepth, averageAborts, averageAbortObjects;

		// ********************************************************************************
		// ********************************************************************************

		/**
		 *  Convenience function to determine the effective SDL version.
		 *  Given that version 3.7 and later require the first statement in the file to be
		 *  a #version statement, the absence of such a statement can be presumed to
		 *  indicate a pre-3.7 scene; this function assumes version 3.62 in that case
		 *  (which was the latest 3.6 version when 3.7.0 was released).
		 *  @note       It is recommended to use this function only where behaviour differs
		 *              significantly from pre-3.7 versions.
		 *  @return     The current language version in integer format (e.g. 370 for 3.70)
		 *              if explicitly specified, or 362 otherwise.
		 */
		inline unsigned int EffectiveLanguageVersion() const
		{
			if (languageVersionSet)
				return languageVersion;
			else
				return 362;
		}

		/**
		 *	Find a file for reading.
		 *	If the file is not available localy, the frontend will be queried.
		 *	Variants of the filename with extensions matching file type will
		 *	be tried. Only the first file found is returned.
		 *	@param	ctx				POVMS message context for the current thread.
		 *	@param	filename		Name and optional (partial) path.
		 *	@param	stype			File type.
		 *	@return					Name of found file or empty string.
		 */
		UCS2String FindFile(POVMSContext ctx, const UCS2String& filename, unsigned int stype);

		/**
		 *	Open a file for reading.
		 *	If the file is not available localy, the frontend will be queried.
		 *	If the frontend has the file, it will be assigned a local temporary
		 *	name that is mapped to the specified file name (so repeated access
		 *	does not require contacting the frontend) and the file will be
		 *	transferred from the frontend to the local system as necessary.
		 *	Note that for their first access the frontend will always be asked
		 *	to provide the location of the file. Local files are only accessed
		 *	within the system specific temporary directory. This prevents
		 *	access to files on local systems in case of remote rendering.
		 *	Returns NULL if the file could not be found.
		 *	@param	ctx				POVMS message context for the current thread.
		 *	@param  origname		The original name of the file as in the scene file (could be relative). // TODO FIXME - not needed, just a hack, the source [trf]
		 *	@param	filename		Name and optional (partial) path.
		 *	@param	stype			File type.
		 *	@return					Pointer to the file or NULL. The caller is
		 *							responsible for freeing the pointer!
		 */
		IStream *ReadFile(POVMSContext ctx, const UCS2String& origname, const UCS2String& filename, unsigned int stype); // TODO FIXME - see above and source code [trf]

		/**
		 *	Open a file given by name and optional (partial) path for writing.
		 *	Rather than creating the file in the specified location, a temporary
		 *	file will be created and the specified name will be mapped to that
		 *	local file. Local files are only accessed within the system specific
		 *	temporary directory. This prevents access to fileson local systems
		 *  in case of remote rendering. For each neealy created file the
		 *	frontend is notified and after rendering the frontend can decide
		 *	which files to access. In addition, this allows parsing the same
		 *	scene simultaniously more than once as each scene manages its own
		 *	set of unique temporary files and thus at no time a file is written
		 *	to or read from by more than one scene.
		 *	@param	ctx				POVMS message context for the current thread.
		 *	@param	filename		Name and optional (partial) path.
		 *	@param	stype			File type.
		 *	@param	append			True to append data to the file, false otherwise.
		 *	@return					Pointer to the file or NULL. The caller is
		 *							responsible for freeing the pointer!
		 */
		OStream *CreateFile(POVMSContext ctx, const UCS2String& filename, unsigned int stype, bool append);
	private:
#ifdef USE_SCENE_FILE_MAPPING
		/// maps scene file names to local file names
		std::map<UCS2String, UCS2String> scene2LocalFiles;
		/// maps local file names to scene file names
		std::map<UCS2String, UCS2String> local2SceneFiles;
		/// maps scene file names to temporary file names
		std::map<UCS2String, UCS2String> scene2TempFiles;
		/// maps temporary file names to scene file names
		std::map<UCS2String, UCS2String> temp2SceneFiles;
#endif

		/**
		 *	Create new scene specific data.
		 */
		SceneData();

		/// not available
		SceneData(const SceneData&);

		/// not available
		SceneData& operator=(const SceneData&);
};

/**
 *	Scene class representing a POV-Ray camera-independent scene.
 */
class Scene
{
	public:
		/**
		 *	Create a new POV-Ray scene. The scene is created with all
		 *	default values and it empty. Only after a scene file has
		 *	been parsed views can be created. Each scene may only be
		 *	parsed once, and it is assumed that the scene structure
		 *	remains static after parsing is complete. However, for
		 *	each scene,once parsed, many views many be created, and
		 *	each view may specify different render quality, size and
		 *	camera parameters. This limitation may be lifted in the
		 *	future to better support animationss and for example
		 *	motion blur, but this can only be supported in POV-Ray
		 *	4.0 when all the rendering code is rewritten!
		 *	@param	backendAddr		Address of the backend that owns this scene.
		 *	@param	frontendAddr	Address of the frontend that owns this scene.
		 *	@param	sid				Id of this scene to include with
		 *							POVMS messages sent to the frontend.
		 */
		Scene(POVMSAddress backendAddr, POVMSAddress frontendAddr, RenderBackend::SceneId sid);

		/**
		 *	Destructor. Parsing will be stopped as necessary.
		 */
		~Scene();

		/**
		 *	Start parsing a POV-Ray scene. Be aware that this
		 *	method is asynchronous! Threads will be started
		 *	to perform the parsing and this method will return.
		 *	The frontend is notified by messages of the state
		 *	of parsing and all warnings and errors found.
		 *	Options shall be in a kPOVObjectClass_ParserOptions
		 *	POVMS obect, which is created when parsing the INI
		 *	file or command line in the frontend.
		 *	@param	parseOptions	Options to use for parsing.
		 */
		void StartParser(POVMS_Object& parseOptions);

		/**
		 *	Stop parsing a POV-Ray scene. Parsing may take a few
		 *	seconds to stop. Internally stopping is performed by
		 *	throwing an exception at well-defined points.
		 *	Note that currently if parsing has been stopped, it
		 *	*cannot* be started again. Eventually this limitation
		 *	will be removed once we have verified that the old
		 *	3.6 parser code can properly handle this condition
		 *	without leaking memory. [trf]
		 *	If parsing is not in progress, no action is taken.
		 */
		void StopParser();

		/**
		 *	Pause parsing. Parsing may take a few seconds to
		 *	pause. Internally pausing is performed by checking
		 *	flag at well-defined points, and if it is true, a
		 *	loop will repeatedly set the parser thread to sleep
		 *	for a few milliseconds until the pause flag is
		 *	cleared again or parsing is stopped.
		 *	If parsing is not in progress, no action is taken.
		 */
		void PauseParser();

		/**
		 *	Resume parsing that has previously been stopped.
		 *	If parsing is not paussed, no action is taken.
		 */
		void ResumeParser();

		/**
		 *	Determine if any parser thread is currently running.
		 *	@return					True if running, false otherwise.
		 */
		bool IsParsing();

		/**
		 *	Determine if parsing is paused.
		 *	@return					True if paused, false otherwise.
		 */
		bool IsPaused();

		/**
		 *	Determine if a previously run parser thread failed.
		 *	@return					True if failed, false otherwise.
		 */
		bool Failed();

		/**
		 *	Create a new view of a parsed scene. Note that this method
		 *	may only be called after a scene has been parsed successfully.
		 *	Note that the view does remain valid even if the scene that
		 *	created it is deleted!
		 *	@param	width			Width of the view in pixels.
		 *	@param	height			Height of the view in pixels.
		 *	@param	vid				Id of this view to include with
		 *							POVMS messages sent to the frontend.
		 *	@return					New view bound to the scene's data.
		 */
		shared_ptr<View> NewView(unsigned int width, unsigned int height, RenderBackend::ViewId vid);

		/**
		 *	Get the POVMS frontend address to send messages to the frontend.
		 *	@return					Frontend address.
		 */
		POVMSAddress GetFrontendAddress() const { return sceneData->frontendAddress; }

		/**
		 *	Get the current parser statistics for the scene.
		 *	Note that this will query each thread, compute the total
		 *	and return it.
		 *	@param	stats			On return, the current statistics.
		 */
		void GetStatistics(POVMS_Object& stats);
	private:
		/// running and pending parser tasks for this scene
		TaskQueue parserTasks;
		/// scene thread data (i.e. statistics)
		vector<SceneThreadData *> sceneThreadData;
		/// scene data
		shared_ptr<SceneData> sceneData;
		/// stop request flag
		bool stopRequsted;
		/// parser control thread
		boost::thread *parserControlThread;

		/**
		 *	Send the parser statistics upon completion of a parsing.
		 *	@param	 taskq			The task queue that executed this method.
		 */
		void SendStatistics(TaskQueue& taskq);

		/**
		 *	Send done message (including compatibility data) after parsing.
		 *	@param	 taskq			The task queue that executed this method.
		 */
		void SendDoneMessage(TaskQueue& taskq);

		/**
		 *	Thread controlling the parser task queue.
		 */
		void ParserControlThread();
};

}

#endif // POVRAY_BACKEND_SCENE_H
