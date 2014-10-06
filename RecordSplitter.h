/**************************
2014 adocilesloth@gmail.com
***************************/
#pragma once
#include "OBSApi.h"
#include "ending.h"
#include "resource1.h"

//entry points
extern "C" __declspec(dllexport) void ConfigPlugin(HWND);

extern "C" __declspec(dllexport) bool LoadPlugin();
extern "C" __declspec(dllexport) void UnloadPlugin();
extern "C" __declspec(dllexport) CTSTR GetPluginName();
extern "C" __declspec(dllexport) CTSTR GetPluginDescription();

extern "C" __declspec(dllexport) void OnStartRecording();
extern "C" __declspec(dllexport) void OnStopRecording();