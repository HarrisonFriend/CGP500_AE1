//ON SCREEN TEXT ATTEMPT

#include "renderer.h"
#include "text.h"
using namespace Solent;

#include <sce_font.h> //sceFontMemory
#include <libsysmodule.h> //sceSysmoduleLoadModule
#include <ces.h> //sceCesUtf8ToUtf32

static void* fontMalloc(void* object, uint32_t size)
{
	(void)object;
	return malloc(size);
}
static void fontFree(void* object, void *p)
{
	(void)object;
	free(p);
}
static void* fontRealloc(void* object, void *p, uint32_t size)
{
	(void)object;
	return realloc(p, size);
}
static void* fontCalloc(void* object, uint32_t n, uint32_t size)
{
	(void)object;
	return calloc(n, size);
}
const SceFontMemoryInterface s_fontLibcMallocInterface = {
	.Malloc = fontMalloc,
	.Free = fontFree,
	.Realloc = fontRealloc,
	.Calloc = fontCalloc,
	.MspaceCreate = (SceFontMspaceCreateFunction *)0,
	.MspaceDestroy = (SceFontMspaceDestroyFunction*)0,
};

//callback to be called by sceFontMemoryTerm()
static void fontMemoryDestroyCallback(SceFontMemory* fontMemory, void*object, void* destroyArg)
{
	(void)object;
	(void)destroyArg;
	free((void*)fontMemory);
	return;
}

void DrawFontTextOnTexture(Gnm::Texture* texture, const char* text)
{
	//created 'texture'
	//e.g.: m->LoadTextureFile("test.bmp");
	//we access this texture to 'draw' text onto

	void* surfaceBuffer = texture->getBaseAddress();
	uint32_t displayWidth = texture->getWidth();
	uint32_t displayHeight = texture->getHeight();

	//start the font processing

	//font-related initialization processing
	//load font-related module
	DBG_ASSERT(sceSysmoduleLoadModule(SCE_SYSMODULE_FONT) == SCE_OK);
	DBG_ASSERT(sceSysmoduleLoadModule(SCE_SYSMODULE_FONT_FT) == SCE_OK);
	DBG_ASSERT(sceSysmoduleLoadModule(SCE_SYSMODULE_FREETYPE_OT) == SCE_OK);

	//initialization of memory definition ysed by font library
	//allocate the SceFontMemory structure
	SceFontMemory* fontMemory = (SceFontMemory*)calloc(1, sizeof(SceFontMemory));
	DBG_ASSERT(fontMemory);
	int ret;
	//prepare memory area for font processing
	ret = sceFontMemoryInit(fontMemory, (void*)0, 0,
		&s_fontLibcMallocInterface, (void*)0,
		fontMemoryDestroyCallback, (void*)0);
	DBG_ASSERT(ret == SCE_FONT_OK)

		//create library
		SceFontLibrary fontLib = SCE_FONT_LIBRARY_INVALID;
	DBG_ASSERT(sceFontCreateLibrary(fontMemory, sceFontSelectLibraryFt(0), &fontLib) == SCE_FONT_OK);

	//processing which should be done only once as necessary just after creating library

	//have the library support handling of system installed fonts
	DBG_ASSERT(sceFontSupportSystemFonts(fontLib) == SCE_FONT_OK);
	//have the library support handling of external font
	DBG_ASSERT(sceFontSupportExternalFonts(fontLib, 16, SCE_FONT_FORMAT_OPENTYPE) == SCE_FONT_OK);
	//temporary buffer for drawing
	DBG_ASSERT(sceFontAttachDeviceCacheBuffer(fontLib, NULL, 1 * 1024 * 1024) == SCE_FONT_OK);

	//create renderer
	SceFontRenderer fontrenderer = SCE_FONT_RENDERER_INVALID;
	DBG_ASSERT(sceFontCreateRenderer(fontMemory, sceFontSelectRendererFt(0), &fontrenderer) == SCE_OK);

	uint32_t openFlag = SCE_FONT_OPEN_FILE_STREAM;
	SceFontOpenDetail* openDetail = (SceFontOpenDetail*)0;
	SceFontHandle hfont = SCE_FONT_HANDLE_INVALID;
	sceFontOpenFontSet(fontLib, SCE_FONT_SET_SST_STD_JAPANESE_JP_W1G, openFlag, openDetail, &hfont);

	//font scaling
	float scaleX = 50.f;
	float scaleY = 50.f;

	SceFontHorizontalLayout horizontalLayout;
	//set the scale for the font
	sceFontSetScalePixel(hfont, scaleX, scaleY);
	//obtain the line layout information in the specified scale
	sceFontGetHorizontalLayout(hfont, &horizontalLayout);
	float lineH = horizontalLayout.lineHeight;

	//bind the renderer for use
	DBG_ASSERT(sceFontBindRenderer(hfont, fontrenderer) == SCE_FONT_OK);

	//set the rendering layout based on the font scale
	sceFontSetupRenderScalePixel(hfont, scaleX, scaleY);

	//define the surface to render
	SceFontRenderSurface renderSurface;
	sceFontRenderSurfaceInit(&renderSurface, surfaceBuffer, displayWidth * 4, 4, displayWidth, displayHeight);

	//zeroclear the background of the surface
	memset(surfaceBuffer, 0, displayWidth * displayHeight * 4); //ARGB

	const uint8_t* utf8addr = (const uint8_t*)text;
	float x = 0;
	float x0 = 0;
	float y = displayHeight * 0.5f - lineH;

	while (1)
	{
		uint32_t len;
		uint32_t ucode;

		//e retrieve UTF-32 character (unicode value) one by one from UTF-8 strings
		sceCesUtf8ToUtf32(utf8addr, 4, &len, &ucode);

		if (ucode == 0x00000000) break;
		utf8addr += len;

		//C0 control code processing
		if (ucode <= 0x0000001f)
		{
			if (ucode == 0x0000000a)
			{
				y += lineH;
				x = x0;
			}
			continue;
		}//end if

		 //render a Unicode character to the surface coordinate (x,y)
		if (y < displayHeight)
		{
			SceFontGlyphMetrics metrics;
			SceFontRenderResult result;

			int32_t ret = sceFontRenderCharGlyphImage(hfont, ucode, &renderSurface, x, y, &metrics, &result);
			if (ret != SCE_FONT_OK)
			{
				//error processing
				sceFontRenderCharGlyphImage(hfont, '_', &renderSurface, x, y, &metrics, &result);
			}
			//update the rendering position according to the information of character spacing
			x += metrics.Horizontal.advance;
		}//end if
	}//end while(1)

	 //unbind the used renderer
	sceFontUnbindRenderer(hfont);

	//close fonts
	sceFontCloseFont(hfont);

	//font-related termination processing
	DBG_ASSERT(fontrenderer);
	sceFontDestroyRenderer(&fontrenderer);

	DBG_ASSERT(fontLib);
	sceFontDestroyLibrary(&fontLib);

	DBG_ASSERT(fontMemory);
	sceFontMemoryTerm(fontMemory);

	//unload any modules
	sceSysmoduleUnloadModule(SCE_SYSMODULE_FONT);
	sceSysmoduleUnloadModule(SCE_SYSMODULE_FONT_FT);
	sceSysmoduleUnloadModule(SCE_SYSMODULE_FREETYPE_OT);
}//end DrawFontTextOnTexture

int Text::drawScoreOnScreen()
{
	//text class
	Text drawScoreOnScreen();

	//renderer class
	Renderer renderer;

	//initialize/setup
	renderer.Create();

	Mesh* m_screenText = renderer.CreateMesh();

	//load texture for triangles
	m_screenText->LoadTextureFile("test.bmp");

	//                        POSITION                COLOUR               UV
	m_screenText->AddVertex(Vertex(-0.5f, -0.5f, 0.0f, 0.7f, 0.7f, 1.0f, 0.0f, 0.0f));
	m_screenText->AddVertex(Vertex(0.5f, -0.5f, 0.0f, 0.7f, 0.7f, 1.0f, 1.0f, 0.0f));
	m_screenText->AddVertex(Vertex(-0.5f, 0.5f, 0.0f, 0.7f, 1.0f, 1.0f, 0.0f, 1.0f));
	m_screenText->AddVertex(Vertex(0.5f, 0.5f, 0.0f, 1.0f, 0.7f, 1.0f, 1.0f, 1.0f));

	m_screenText->AddIndex(0, 1, 2); //triangle 1
	m_screenText->AddIndex(1, 3, 2); //triangle 2

	m_screenText->BuildTriangleBuffer();

	DrawFontTextOnTexture(m_screenText->texture, "Score: 0000");

	//start drawing the triangles for 1000 frames then exit
	for (uint32_t frameIndex = 0; frameIndex < 1000; frameIndex++)
	{
		//m_screenText->rotation.setZ(m_screenText->rotation.getZ() + 0.005f);

		renderer.RenderLoop();
	}//end for

	 //tidy up before exiting
	renderer.Release();
}