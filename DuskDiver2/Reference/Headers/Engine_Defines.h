
#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 4996)

#include "PxPhysics.h"
#include "PxPhysicsAPI.h"
using namespace physx;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <FX11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <DirectXCollision.h>


#include <Assimp/scene.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>



#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXMath.h>
using namespace DirectX;

// 파일 입출력
#include <fstream>
#include <iostream>

// 클래스명 읽기
#include <typeinfo>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
//#include <typeinfo.h>


#include <process.h>

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Functor.h"
#include "Engine_Function.h"

#include <io.h>
#include <Vfw.h>

#pragma comment(lib, "vfw32.lib")

#ifdef ENGINE_EXPORTS
#pragma comment(lib, "../Bin/fmod_vc.lib")
#else
#pragma comment(lib, "fmod_vc.lib")
#endif

#pragma comment(lib, "msimg32.lib")

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace std;
using namespace Engine;


