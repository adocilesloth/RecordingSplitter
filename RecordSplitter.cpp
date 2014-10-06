/**************************
2014 adocilesloth@gmail.com
***************************/
/*************************************************
Note: Will drop ~3 secs between recording switches
**************************************************/
#include "RecordSplitter.h"
#include "timer.h"

#include <fstream>
#include <string>

using namespace std;

ifstream settings;
HINSTANCE   hInstance;

HANDLE RecThread;
bool active;
long int loops;

INT_PTR CALLBACK ConfigDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int hours, mins, secs;

	wstring path = OBSGetPluginDataPath().Array();
	settings.open(path + L"\\RecordingSplitter.ini");
	settings >> active;
	settings >> hours;
	settings >> mins;
	settings >> secs;
	settings.close();

	switch (message)
	{
		case WM_INITDIALOG:
		{
			SendMessage(GetDlgItem(hWnd, IDC_ENBL), BM_SETCHECK, active ? BST_CHECKED : BST_UNCHECKED, 0);
			SetDlgItemInt(hWnd, IDC_EHRS, hours, NULL);
			SendMessage(GetDlgItem(hWnd, IDC_SHRS), UDM_SETRANGE32, 0, 24);
			SetDlgItemInt(hWnd, IDC_EMIN, mins, NULL);
			SendMessage(GetDlgItem(hWnd, IDC_SMIN), UDM_SETRANGE32, 0, 60);
			SetDlgItemInt(hWnd, IDC_ESEC, secs, NULL);
			SendMessage(GetDlgItem(hWnd, IDC_SSEC), UDM_SETRANGE32, 0, 60);
			if(active)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_EHRS), true);
				EnableWindow(GetDlgItem(hWnd, IDC_EMIN), true);
				EnableWindow(GetDlgItem(hWnd, IDC_ESEC), true);
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_EHRS), false);
				EnableWindow(GetDlgItem(hWnd, IDC_EMIN), false);
				EnableWindow(GetDlgItem(hWnd, IDC_ESEC), false);
			}
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					{
						active = SendMessage(GetDlgItem(hWnd, IDC_ENBL), BM_GETCHECK, 0, 0) == BST_CHECKED;
						hours = GetDlgItemInt(hWnd, IDC_EHRS, NULL, NULL);
						mins = GetDlgItemInt(hWnd, IDC_EMIN, NULL, NULL);
						secs = GetDlgItemInt(hWnd, IDC_ESEC, NULL, NULL);

						if(hours == 0 && mins == 0 && secs < 10)			//min splitting time is 30 secs
						{
							secs = 10;
						}

						ofstream create(path + L"\\RecordingSplitter.ini");
						create << active << endl;
						create << hours << endl;
						create << mins << endl;
						create << secs << endl;
						create.close();

						loops = ((hours * 60 * 60) + (mins * 60) + secs) * 10;
						EndDialog(hWnd, LOWORD(wParam));
						break;
					}
				case IDCANCEL:
					EndDialog(hWnd, LOWORD(wParam));
					break;

				case IDC_ENBL:
					{
						bool benbl = SendMessage(GetDlgItem(hWnd, IDC_ENBL), BM_GETCHECK, 0, 0) == BST_CHECKED;
						if(benbl)
						{
							EnableWindow(GetDlgItem(hWnd, IDC_EHRS), true);
							EnableWindow(GetDlgItem(hWnd, IDC_EMIN), true);
							EnableWindow(GetDlgItem(hWnd, IDC_ESEC), true);
						}
						else
						{
							EnableWindow(GetDlgItem(hWnd, IDC_EHRS), false);
							EnableWindow(GetDlgItem(hWnd, IDC_EMIN), false);
							EnableWindow(GetDlgItem(hWnd, IDC_ESEC), false);
						}
					}
			}
	}
	return 0;
}

void ConfigPlugin(HWND hWnd)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_RECCFG), hWnd, ConfigDlgProc);
}

bool LoadPlugin()
{
	wstring path = OBSGetPluginDataPath().Array();
	settings.open(path + L"\\RecordingSplitter.ini");

	int hours, mins, secs;

	if (!settings.is_open())
	{
		ofstream create(path + L"\\RecordingSplitter.ini");
		create << "1" << endl;
		create << "0" << endl;		//hours
		create << "30" << endl;		//mins
		create << "0";				//secs
		create.close();		//stop using settings file

		active = true;
		hours = 0;
		mins = 30;
		secs = 0;
	}
	else
	{
		settings >> active;
		settings >> hours;
		settings >> mins;
		settings >> secs;
		settings.close();
	}

	loops = ((hours * 60 * 60) + (mins * 60) + secs) * 10;

	AppWarning(TEXT("Record Splitter Loaded"));
	return true;
}

void UnloadPlugin()
{
}

CTSTR GetPluginName()
{
	return TEXT("Recording Splitter");
}

CTSTR GetPluginDescription()
{
	return TEXT("Splits Recordings into smaller files\n\n NOTE: There will be around 5 seconds of lost footage between the split recordings");
}

void OnStartRecording()
{
	if(active)
	{
		WaitForSingleObject(RecThread, INFINITE);
		long int* t = new long int(loops);
		RecThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartTimer, t, 0, 0);
	}
	return;
}

void OnStopRecording()
{
	if(active)
	{
		if(!OBSGetRecording())
		{
			StopTimer();
			WaitForSingleObject(RecThread, INFINITE);
			ResetTimer();
			return;
		}
	}
	return;
}

BOOL CALLBACK DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		hInstance = hinstDLL;

	return TRUE;
}