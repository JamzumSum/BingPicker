/*
* main.cpp
*
*  Created on: 2017��12��13��
*      Author: JamzumSum
*/
#pragma once
#include "GUIlib\GUI.hpp"
#include "resource.h"
#include <ShellAPI.h>

using namespace std;

extern string httpGet(string);
extern string regurl(string, string&);
extern bool download(string, string);
static void setWallpaper(string);

Label* notify = NULL;
ProgressBar* progress = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow){
	form frmain(NULL, "JamzumSum", "Bing Picker", 0, 0, 640, 360);
	form about(&frmain, "JamzumSum", "About");
	frmain.Event_On_Create = [&about, &frmain]() {
		frmain.enableRBmenu();
		frmain.createMenu(frmain.RBmenu, "About", [&]() {
			Label I(&about, 24, 24, 344, 216,
				"Developer:\r\n"
				"	JamzumSum\n\n"
				"Credit:\n"
				//"	@opencv - OpenCV Project\n"
				"	@sorcerer - Bing icon\n"
				"\nThanks for using.\n");
			about.Event_Load_Complete = [&]() {
				about.resize(392, 280);
				about.move(336, 112);
				I.setFont("΢���ź�", 20);
			};
			about.create();
			about.run();
			about.msgLoop.detach();
		});
	};
	frmain.setIcon(MAKEINTRESOURCE(IDI_ICON1), MAKEINTRESOURCE(IDI_ICON2));

	Label info(&frmain, 8, -60, 500, 36, "Tap `Download`");
	ProgressBar dld(&frmain, 8, -80, -32, 16, "downloading...");
	Button setWall(&frmain, 280, 120, 80, 45, "Download");

	string filename;
	frmain.feature ^= WS_MAXIMIZEBOX;
	bool downloaded = false;
	notify = &info;
	progress = &dld;

	auto showPic = [&frmain, &downloaded](string pic) {
		if (!downloaded) return;
		ShellExecute(frmain.hWnd(), "open", pic.c_str(), NULL, NULL, SW_SHOW);
	};

	info.Event_On_Click = [&showPic, &filename]() {
		showPic(filename + ".jpg");
	};

	frmain.Event_Load_Complete = [&]() {
		info.setFont("΢���ź�", 18);
		setWall.setFont("΢���ź�", 18);
	};
	setWall.Event_On_Click = [&]() {
		if (downloaded) setWallpaper(filename);
		else {
			string httpD = httpGet("https://cn.bing.com");
			string pic;
			try {
				pic = regurl(httpD, filename);
				download(pic, filename);
			}
			catch (string msg) {
				info.name = msg.c_str();
				return;
			}

			Sleep(500);
			downloaded = true;
			dld.full();
			showPic(filename + ".jpg");
			setWall.name = "Set as Wallpaper";
			setWall.resize(160, 0);
			setWall.move(240, 0);
			info.name = ("Today's Bing image: " + filename).c_str();
		}
	};
	frmain.run();
	frmain.msgLoop.join();
	return 0;
}

void setWallpaper(string path) {
	HRESULT hr = S_OK;
	HKEY hKey = NULL;
	hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_CURRENT_USER,
		"Control Panel\\Desktop", 0, KEY_READ | KEY_WRITE, &hKey));
	if (SUCCEEDED(hr))
	{
		LPCSTR pszWallpaperStyle;
		LPCSTR pszTileWallpaper;
		pszWallpaperStyle = "6";
		pszTileWallpaper = "0";
		// ���� WallpaperStyle �� TileWallpaper ��ע�����.  
		DWORD cbData = lstrlen(pszWallpaperStyle) * sizeof(*pszWallpaperStyle);
		hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, "WallpaperStyle", 0, REG_SZ,
			reinterpret_cast<const BYTE *>(pszWallpaperStyle), cbData));
		if (SUCCEEDED(hr)) {
			cbData = lstrlen(pszTileWallpaper) * sizeof(*pszTileWallpaper);
			hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, "TileWallpaper", 0, REG_SZ,
				reinterpret_cast<const BYTE *>(pszTileWallpaper), cbData));
		}
		RegCloseKey(hKey);
	}
	char current[MAXSIZE];
	if (GetCurrentDirectory(MAXSIZE, current)) {
		string fullpath = current;
		fullpath += "//";
		fullpath += path;
		if (SUCCEEDED(hr)) if (!SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)fullpath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE)) {
			//hr = HRESULT_FROM_WIN32(GetLastError());
			/*char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("���ñ�ֽʧ�ܣ�"), s, MB_OK);*/
			popError();
		}
	}
	
}