//ON SCREEN TEXT ATTEMPT

#pragma once

#include "renderer.h"
#include <sce_font.h> //sceFontMemory
#include <libsysmodule.h> //sceSysmoduleLoadModule
#include <ces.h> //sceCesUtf8ToUtf32

using namespace Solent;

namespace Solent
{
	class Text
	{
	public:
		int drawScoreOnScreen();
	};
}
