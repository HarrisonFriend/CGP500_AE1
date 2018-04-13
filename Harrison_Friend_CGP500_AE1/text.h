//ON SCREEN TEXT ATTEMPT

#pragma once

#include "renderer.h"
using namespace Solent;

#include <sce_font.h> //sceFontMemory
#include <libsysmodule.h> //sceSysmoduleLoadModule
#include <ces.h> //sceCesUtf8ToUtf32

class Text
{
public:
	int drawTextOnScreen();
};