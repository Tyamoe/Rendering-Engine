#include "stdafx.h"

#include "Window.h"
#include "Engine.h"
#include "Input.h"
#include "Globals.h"

#include "Structs.h"

#include "TyRenderer.h"

#ifdef HEADLESS

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>

/* the global Assimp scene object */
const aiScene* scene = NULL;
GLuint scene_list = 0;
aiVector3D scene_min, scene_max, scene_center;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

int loadasset(const char* path)
{
	/* we are taking one of the postprocessing presets to avoid
	   spelling out 20+ single postprocessing flags here. */
	scene = aiImportFile(path, 0);

	
	return 1;
}

int main()
{
	//aiLogStream stream;

	/* get a handle to the predefined STDOUT log stream and attach
	   it to the logging system. It remains active for all further
	   calls to aiImportFile(Ex) and aiApplyPostProcessing. */
	//stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
	//aiAttachLogStream(&stream);

	/* ... same procedure, but this stream now writes the
	   log messages to assimp_log.txt */
	//stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimp_log.txt");
	//aiAttachLogStream(&stream);

	/* the model name can be specified on the command line. If none
	  is specified, we try to locate one of the more expressive test
	  models from the repository (/models-nonbsd may be missing in
	  some distributions so we need a fallback from /models!). */
	/*if (0 != loadasset("models-nonbsd/FBX/2013_BINARY/multiple_animations_test.fbx")) 
	{

	}*/

	/* cleanup - calling 'aiReleaseImport' is important, as the library
	   keeps internal resources until the scene is freed again. Not
	   doing so can cause severe resource leaking. */
	//aiReleaseImport(scene);

	/* We added a log stream to the library, it's our job to disable it
	   again. This will definitely release the last resources allocated
	   by Assimp.*/
	//aiDetachAllLogStreams();

	/*Window window("Tray Racer", Settings(), Layout(800, 600, 400, 220)); // E

	//GenericDraw::Init(); // R

	EnginePtr engine = new Engine();
	InputPtr input = new Input(window.GetGLFWWindow()); // E

	window.AttachInput(input);
	window.AttachEngine(engine);

	TyRenderer tyRenderer; // E
	tyRenderer.Init();*/

	EnginePtr engine = new Engine();
	engine->Start();
}

#else

void Init()
{
	TYlog << "Init\n";
}

#endif