// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#ifndef STDAFX_H_
#define STDAFX_H_


/* 콘솔창 띄우기 */
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <d3d11.h>
#include <d3dx11.h>
// PCH(Precompiled Header)를 통하여 자주 변경되지 않는 헤더 파일에 대한
// 컴파일을 매번 하지 않아도 되도록 하는 기능을 제공

#include <fbxsdk.h>
#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <xnamath.h>
#include <string>

#include <Mmsystem.h>
#include <math.h>
// 시간과 관련된 멀티미디어 라이브러리 함수를 사용하기 위하여 다음 헤더 파일을 추가한다.
// 수학 라이브러리 함수를 사용하기 위하여 다음 헤더 파일을 추가한다.

#include <d3dcompiler.h>  	//쉐이더 컴파일 함수를 사용하기 위한 헤더 파일
#include <D3DX10Math.h>	//Direct3D 수학 함수를 사용하기 위한 헤더 파일

#include <D3D9Types.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.



#endif