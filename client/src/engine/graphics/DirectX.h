#ifndef __DIRECTX_H__
#define __DIRECTX_H__

#pragma once


#include "..\utils\Memory.h"
#include "..\utils\Debug.h"

class CDirectX
{
public:
	static CDirectX* GetSingleton();

	bool Init(HWND hwnd, UINT screenWidth, UINT screenHeight);
	void Release();

private:
	CDirectX();

private:
	static CDirectX* s_singleton;
};

#endif