#include "stdafx.h"
#include "Fbx.h"



void main()
{
	CFbx *pFbx = new CFbx();

	//// 애니메이션 없음
	//pFbx->Import("Data\\Grass\\grass.fbx", "Grass");

	// 애니메이션 있음
	//pFbx->Import("Data\\monster\\mst@attack.FBX", "Slime", "attack");
	pFbx->Import("Data\\monster\\mst@dash.FBX", "Slime", "dash");
	//pFbx->Import("Data\\monster\\mst@dead.FBX", "Slime", "dead");
	//pFbx->Import("Data\\monster\\mst@idle.FBX", "Slime", "idle");
	//pFbx->Import("Data\\monster\\mst@jump.FBX", "Slime", "jump");
	//pFbx->Import("Data\\monster\\mst@jumpjump.FBX", "Slime", "jumpjump");
	//pFbx->Import("Data\\monster\\mst@move.FBX", "Slime", "move");

	exit(0);
}
