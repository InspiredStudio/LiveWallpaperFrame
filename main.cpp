#include <iostream>
#include <windows.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <tlhelp32.h>

using namespace std;

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM Lparam){
	HWND hDefView = FindWindowExW(hwnd, 0, (wchar_t*)"SHELLDLL_DefView", 0);
	if (hDefView != 0) {
		HWND hWorkerw = FindWindowExW(0, hwnd, (wchar_t*)"WorkerW", 0);
		ShowWindow(hWorkerw, SW_HIDE);
		
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK ResetWorkerWindow(_In_ HWND hwnd, _In_ LPARAM Lparam){
	HWND hDefView = FindWindowExW(hwnd, 0, (wchar_t*)"SHELLDLL_DefView", 0);
	if (hDefView != 0) {
		HWND hWorkerw = FindWindowExW(0, hwnd, (wchar_t*)"WorkerW", 0);
		ShowWindow(hWorkerw, SW_SHOW);
		
		return FALSE;
	}
	return TRUE;
}

void ShowDWPWindow(){
	Sleep(200);
	HWND hProgman = FindWindowW((wchar_t*)"Progman", 0);
	SendMessageTimeout(hProgman, 0x52C, 0, 0, 0, 100, 0);
	HWND hFfplay = FindWindowW((wchar_t*)"SDL_app", 0);
	SetParent(hFfplay, hProgman);
	EnumWindows(EnumWindowsProc, 0);
}

void PlayDWP(const char* file){
	char* scrx=(char*)GetSystemMetrics(SM_CXSCREEN);
	char* scry=(char*)GetSystemMetrics(SM_CYSCREEN);
	char* runParam = "-noborder -x ";
	strcat(runParam,scrx);
	strcat(runParam," -y ");
	strcat(runParam,scry);
	strcat(runParam," -loop 0 ");
	strcat(runParam,file);
	STARTUPINFOW sinfo { 0 };
	PROCESS_INFORMATION pinfo { 0 };
	WINBOOL proc = CreateProcessW((wchar_t*)"./ffplay.exe",(LPWSTR)runParam,0,0,0,0,0,0,&sinfo,&pinfo);
	if(proc){
		ShowDWPWindow();
	}
}

void KillProcess(char* szImageName){
	PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32) };
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BOOL bRet = Process32First(hProcess,&pe);
	while(bRet){
		if(lstrcmp(szImageName,pe.szExeFile)==0) {
			TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE,pe.th32ProcessID), 0);
		}
		bRet = Process32Next(hProcess,&pe);
	}
}

void StopDWP(){
	HWND hFFPlay = FindWindowW((wchar_t*)"SDL_app", 0);
	PostMessageW(hFFPlay, WM_CLOSE, 0, 0);
	if(WaitForSingleObject(hFFPlay,WAIT_OBJECT_0) != WAIT_OBJECT_0){
		KillProcess("ffplay.exe");
	}
	EnumWindows(ResetWorkerWindow,0);
}

void ReplaceDWP(const char* file){
	StopDWP();
	PlayDWP(file);
}

void ShowHelp(){
	cout << "usage: dwpcore-mingw.exe command" << endl << endl;
	cout << "commands:" << endl;
	cout << "    -h, --help: Show this help" << endl;
	cout << "    -p <file>, --play <file>: Play video wallpaper, then put it on desktop." << endl;
	cout << "    -s, --stop: Reset desktop static wallpaper and close video wallpaper." << endl;
	cout << "    -r <file>, --replace <file>: Replace video wallpaper to <file>." << endl;
	system("pause");
}

int main(int argc, char** argv) {
	cout << "LiveWallpaperFrame v1.0 by InspiredStudio" << endl;
	cout << "Copyright (C) 2022 InspiredStudio" << endl << endl;
	int opt;
	int digit_optind = 0;
	int option_index = 0;
	const char *optstring = "hp:r:s";
	static struct option long_options[] = {
		{"play", required_argument, NULL, 'p'},
		{"stop", no_argument, NULL, 's'},
		{"replace", required_argument, NULL, 'r'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1){
		if(opt == 'h'){
			ShowHelp();
		}else if(opt == 'p'){
			PlayDWP(optarg);
		}else if(opt == 's'){
			StopDWP();
		}else if(opt == 'r'){
			ReplaceDWP(optarg);
		}else if(opt == '?'){
			break;
		}else{
			cout << "dwpcore-mingw.exe: missing command." << endl;
			break;
		}
		return 0;
	}
	cout << "type \"dwpcore-mingw.exe --help\" for more informations." << endl;
	return 1;
}
