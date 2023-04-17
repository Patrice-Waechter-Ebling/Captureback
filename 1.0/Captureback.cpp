#define WINVER  0x0400 
#define TTS_BALLOON	0x40
#define TTM_SETTITLE 0x420
#define CCM_SETWINDOWTHEME 0x200B
#define IDM_TIMER1 0x01000
#define WM_TRAY_ICONE WM_USER + 101
#include <windows.h> 
#include <windowsx.h>
#include <wingdi.h>
#include <commctrl.h>
#include <Winuser.h>
#include <Commdlg.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdio.h>
#include <conio.h>
#include "resource.h"

#pragma comment (lib,"kernel32.lib") 
#pragma comment (lib,"user32.lib") 
#pragma comment (lib,"gdi32.lib") 
#pragma comment (lib,"winspool.lib") 
#pragma comment (lib,"comdlg32.lib") 
#pragma comment (lib,"advapi32.lib") 
#pragma comment (lib,"shell32.lib") 
#pragma comment (lib,"ole32.lib") 
#pragma comment (lib,"oleaut32.lib") 
#pragma comment (lib,"uuid.lib") 
#pragma comment (lib,"odbc32.lib") 
#pragma comment (lib,"odbccp32.lib") 
#pragma comment (lib,"comctl32.lib") 
#pragma comment (lib,"shlwapi.lib") 

           
__declspec( dllexport ) HINSTANCE hInst;
__declspec( dllexport )OPENFILENAME ofn;
__declspec( dllexport )TCHAR szPath[MAX_PATH];
__declspec( dllexport )TCHAR appPath[MAX_PATH];
__declspec( dllexport )TCHAR buffer[MAX_PATH];
__declspec( dllexport )TCHAR Result[MAX_PATH];
__declspec( dllexport )TCHAR szFile[MAX_PATH];
__declspec( dllexport )INITCOMMONCONTROLSEX iccex; 
__declspec( dllexport )WNDCLASS wc;
__declspec( dllexport )HWND hWnd,hwndTT,imghWnd;
__declspec( dllexport )NOTIFYICONDATAA nf;
__declspec( dllexport )TOOLINFO ti;
__declspec( dllexport )RECT rc,rect,ec;
__declspec( dllexport )HDC hdcScreen,hdcCompatible; 
__declspec( dllexport )HBITMAP hbmScreen,bmp,hbmScaled;
__declspec( dllexport )BITMAP btmp;
__declspec( dllexport )HMENU mnu;
__declspec( dllexport )SYSTEMTIME st;
__declspec( dllexport )BOOL AffichageMenu=FALSE;
//Tableaux
__declspec( dllexport ) char *jours[7] = {"dimanche", "lundi","mardi","mercredi","jeudi","vendredi","samedi"};
__declspec( dllexport ) char *mois[12] = {"janvier", "février","mars", "avril", "mai", "juin","juillet","aout","septembre", "octobre", "novembre", "décembre"};

__declspec( dllexport )void CreateMyTooltip (HWND hDlg,char *ToolTipTitre);
__declspec( dllexport )void ActiveToolTips();
static BOOL CALLBACK DialogFunc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow);
__declspec( dllexport )VOID APIENTRY DisplayContextMenu(HWND hDlg,POINT pt);
__declspec( dllexport )BOOL WINAPI OnContextMenu(int x, int y) ;
__declspec( dllexport )OPENFILENAME CommunFichiers(LPCTSTR Titre,LPCTSTR Filtre);
__declspec( dllexport )void SauverFichier(LPCTSTR Titre);
__declspec( dllexport )PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
__declspec( dllexport )void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,HBITMAP hBMP, HDC hDC);
__declspec( dllexport )void CaptureEcran(HWND hDlg);
__declspec( dllexport )void SysMenu(HWND hDlg);

__declspec( dllexport )void errhandler(LPCTSTR Texte,HWND hwnd){
	MessageBox(hwnd,Texte,wc.lpszClassName,0);
}

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow){
	hInst=hinst;
	iccex.dwICC=ICC_WIN95_CLASSES;
	iccex.dwSize=sizeof(iccex);
	InitCommonControlsEx(&iccex);
	GetCurrentDirectory(0xFF,appPath);
	memset(&wc,0,sizeof(wc));
	wc.hCursor=LoadCursor(hinst, (LPCTSTR)IDC_CURSOR1);
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.lpszClassName = "CapureScreen";
	wc.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(128,202,128));
	wc.hIcon=LoadIcon(hinst,(LPCTSTR)IDI_ICON1);
	wc.style = CS_VREDRAW  | CS_HREDRAW | CS_SAVEBITS | CS_DBLCLKS;
	wc.hIcon=LoadIcon(wc.hInstance,(LPCTSTR)IDI_ICON1);
	RegisterClass(&wc);
	return DialogBox(hinst, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC) DialogFunc);
}

static BOOL CALLBACK DialogFunc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
	hWnd =hDlg; 
	POINT mouse;
	switch (msg) {
	return 1;
	case WM_INITDIALOG:
		SetTimer(hDlg,IDM_TIMER1,1000,(TIMERPROC) NULL);
		SysMenu(hDlg);
		GetCurrentDirectory(0xFF,appPath);
		PathAddBackslash(appPath);
		SendMessage(GetDlgItem(hDlg,IDCANCEL), WM_SETFONT, (WPARAM)GetStockObject(0x1E), MAKELPARAM(TRUE, 0)); 
		SendMessage(GetDlgItem(hDlg,IDOK), WM_SETFONT, (WPARAM)GetStockObject(0x1F), MAKELPARAM(TRUE, 0));
		SendMessage(GetDlgItem(hDlg,0xE140), STM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(wc.hInstance,(LPCTSTR)IDI_ICON2));
		nf.cbSize=sizeof(nf);
		nf.hIcon=wc.hIcon;
		nf.hWnd=hDlg; 
		sprintf(nf.szTip,"%s\n© Patrice Waechter-Ebling 2005\n%s\0",wc.lpszClassName,__argv[0]);
		nf.uCallbackMessage=WM_TRAY_ICONE;
		nf.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP; 
		Shell_NotifyIcon(NIM_ADD,&nf);
		imghWnd = CreateWindowEx(0, "STATIC", NULL, WS_VISIBLE|WS_CHILD|SS_ICON,1, 1, 10, 10,hDlg , (HMENU)45000, wc.hInstance, NULL);
		SendMessage(imghWnd, STM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(wc.hInstance,(LPCTSTR)IDI_ICON1));
		sprintf(buffer,"%s (C)opyright Patice Waechter-Ebling 2005",wc.lpszClassName);
		SetWindowText(hDlg,buffer); 
		GetLocalTime(&st);
		sprintf(buffer,"Nous sommes %s, le %2d %s %4d",jours[st.wDayOfWeek],st.wDay,mois[st.wMonth-1],st.wYear); //creation du string de date
		SetDlgItemText(hDlg,IDC_DATE,buffer);
		GetClientRect(GetDesktopWindow(),&ec);
		GetClientRect(hDlg,&rc);
		StretchBlt(GetDC(GetDesktopWindow()),0,0,ec.right,ec.bottom,GetDC(hDlg),0,0,rc.right,rc.bottom,0);
		return TRUE;
	case WM_SYSCOMMAND:
		switch (LOWORD(wParam)) {
			case IDOK:
				CaptureEcran(hDlg);//creation tempon
				CaptureEcran(hDlg);// capture comme telle
				break;
			case 0xE145:
				SauverFichier("Sauvegarder la capture");
				break;
			case IDCANCEL:
				Shell_NotifyIcon(NIM_DELETE,&nf); 
				KillTimer(hDlg,IDM_TIMER1);
				PostQuitMessage(0);
				return 1;
		}
	   break;
	case WM_TRAY_ICONE:{ 
		GetSubMenu(LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU1)), 0);
		switch(lParam){
			case WM_RBUTTONUP:
					GetCursorPos(&mouse);
					mnu=GetSubMenu(LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU1)), 0);
					AppendMenu(mnu,MF_STRING,IDOK,"Capturer");
					AppendMenu(mnu,MF_STRING,0xE145,"Sauvegarder la capture");
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_QUITTER);
					SetMenuItemBitmaps(mnu,IDCANCEL,MF_BYCOMMAND,bmp,bmp);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_CAPTURE);
					SetMenuItemBitmaps(mnu,IDOK,MF_BYCOMMAND,bmp,bmp);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_SAVE);
					SetMenuItemBitmaps(mnu,0xE145,MF_BYCOMMAND,bmp,bmp);
					SetMenuDefaultItem(GetSubMenu(LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU1)), 0), 0xE140, FALSE);
					SetForegroundWindow(hDlg);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_QUITTER); 
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_SAVE);
					SetMenuItemBitmaps(mnu,0xE145,MF_BYCOMMAND,bmp,bmp);
					TrackPopupMenu(mnu, TPM_RETURNCMD || TPM_RIGHTALIGN, mouse.x, mouse.y, 0, hDlg, NULL);
					DestroyMenu(GetSubMenu(LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU1)), 0));
					break;
			case WM_LBUTTONUP:
					GetCursorPos(&mouse);
					if (OnContextMenu( mouse.x,mouse.y)==FALSE){
						return DefWindowProc(hDlg, msg, wParam, lParam);
					}else{
						MessageBox(0,"Il n'y a pas de menu installé pour le clic Gauche","",0);
					}
					break;
			}
		return 1;
		}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDOK:
				CaptureEcran(hDlg);//creation tempon
				CaptureEcran(hDlg);// capture comme telle
				break;
			case 0xE145:
				SauverFichier("Sauvegarder la capture");
				break;
			case IDCANCEL:
				Shell_NotifyIcon(NIM_DELETE,&nf); 
				KillTimer(hDlg,IDM_TIMER1);
				PostQuitMessage(0);
				return 1;
		}
	   break;
	case WM_MOUSEMOVE:
			ActiveToolTips();
			break;
	case WM_TIMER:
		GetLocalTime(&st);
		sprintf(buffer,"Nous sommes %s, le %2d %s %4d",jours[st.wDayOfWeek],st.wDay,mois[st.wMonth-1],st.wYear);
		SetDlgItemText(hDlg,IDC_DATE,buffer);
		sprintf(buffer,"%.2d:%.2d:%.2d",st.wHour,st.wMinute,st.wSecond);
		SetDlgItemText(hDlg,IDC_TIME,buffer);
		break;
	case WM_CONTEXTMENU:
			if (OnContextMenu( LOWORD(lParam),HIWORD(lParam))==FALSE)
				return DefWindowProc(hDlg, msg, wParam, lParam);
			break;
	case WM_CTLCOLORDLG:
		return (long)wc.hbrBackground;
		break;
	case WM_CTLCOLORSTATIC:
		SetTextColor( (HDC)wParam, RGB(255,0, 255) );
		SetBkMode( (HDC)wParam, TRANSPARENT ); 
		return (LONG)wc.hbrBackground;
	case WM_CTLCOLOREDIT:
		SetTextColor( (HDC)wParam, RGB(0xA0,0xFF,0xAA ) );
		SetBkMode( (HDC)wParam, TRANSPARENT ); 
		return (LONG)(HBRUSH)CreateSolidBrush(RGB(0,0,128));
	case WM_CTLCOLORLISTBOX: 
		SetTextColor( (HDC)wParam, RGB(0,255, 255) );
		SetBkMode( (HDC)wParam, TRANSPARENT ); 
		return (LONG)(HBRUSH)CreateSolidBrush(RGB(0,128,0));
	case WM_CTLCOLORBTN: 
		SetTextColor( (HDC)wParam, RGB(0,255, 255) );
		SetBkMode( (HDC)wParam, TRANSPARENT ); 
		return (LONG)(HBRUSH)CreateSolidBrush(RGB(128,0,0));
	case WM_CTLCOLORMSGBOX: 
		return (long)(HBRUSH)CreateSolidBrush(RGB(255,0,128));
		break;
	case WM_CLOSE: 
		Shell_NotifyIcon(NIM_DELETE,&nf); 
		KillTimer(hDlg,IDM_TIMER1);
		PostQuitMessage(0);
		return TRUE;
	}
	return FALSE;
}

__declspec( dllexport )VOID APIENTRY DisplayContextMenu(POINT pt) {
    HMENU hmenu,hmenuTrackPopup;
	if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1))) == NULL) 
        return; 
     hmenuTrackPopup = GetSubMenu(hmenu, 0); 
	 AppendMenu(hmenuTrackPopup,MF_STRING,0xE145,"Sauvegarder");
	 AppendMenu(hmenuTrackPopup,MF_STRING,IDOK,"Caputer");
	 bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_CAPTURE);
 	SetMenuItemBitmaps(hmenuTrackPopup,IDOK,MF_BYCOMMAND,bmp,bmp);
	 bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_QUITTER);
 	SetMenuItemBitmaps(hmenuTrackPopup,IDCANCEL,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_SAVE);
	SetMenuItemBitmaps(hmenu,0xE145,MF_BYCOMMAND,bmp,bmp);
	TrackPopupMenu(hmenuTrackPopup,TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.x, pt.y, 0, hWnd, NULL); 
    DestroyMenu(hmenu); 
} 

__declspec( dllexport )BOOL WINAPI OnContextMenu(int x, int y) {
    RECT rc;
	POINT pt;
    pt.x = x;
    pt.y= y;
    GetClientRect(hWnd, &rc);
    ScreenToClient(hWnd, &pt); 
    if (PtInRect(&rc, pt))     {
        ClientToScreen(hWnd, &pt); 
        DisplayContextMenu(pt); 
        return TRUE; 
    } 
    return FALSE; 
} 

__declspec( dllexport )void CreateMyTooltip (HWND hDlg,char *ToolTipTitre){
    unsigned int uid = 0;
     LPTSTR lptstr = ToolTipTitre;
    iccex.dwICC = ICC_WIN95_CLASSES|ICC_ANIMATE_CLASS|ICC_TAB_CLASSES;
    iccex.dwSize = sizeof(iccex);
    InitCommonControlsEx(&iccex);
    hwndTT = CreateWindowEx(WS_EX_TOPMOST,TOOLTIPS_CLASS,NULL,WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP/*transforme en bulle */ |TTS_BALLOON,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT ,hWnd,NULL,wc.hInstance,NULL); //crée la fenetre
    SetWindowPos(hwndTT,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hDlg;
    ti.hinst = wc.hInstance;
    ti.uId = uid;
    ti.lpszText = lptstr;
    ti.rect.left = 0;    
    ti.rect.top = 0;
    ti.rect.right = 300;
    ti.rect.bottom = 200;
   SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
   SendMessage(hwndTT, WM_USER + 20, (WPARAM) RGB(0,255,0),0);
   SendMessage(hwndTT, WM_SETFONT, (WPARAM)GetStockObject(0x0E), MAKELPARAM(TRUE, 0)); 
   SendMessage(hwndTT, TTM_SETTITLE,  (WPARAM) (int) 1, (LPARAM)(LPCTSTR) wc.lpszClassName);
   SendMessage(hwndTT, WM_USER + 19,(WPARAM)RGB(204,100,204),0 );
   SendMessage(hwndTT, CCM_SETWINDOWTHEME,0,(LPARAM) (LPWSTR) "BUTTON");  
   SendMessage(hwndTT,WM_USER + 26,0,0);

} 

__declspec( dllexport )void ActiveToolTips(){
	CreateMyTooltip (hWnd,"\nv:1.00\n(C)opyright Patice Waechter-Ebling 2005 ");
	CreateMyTooltip (GetDlgItem(hWnd,2),"Met fin à ce programme");
	CreateMyTooltip (GetDlgItem(hWnd,0xE140),"Affiche les informations sur ce programme");
	CreateMyTooltip (GetDlgItem(hWnd,IDOK),"Vous allez cliquer sur OK");
}


__declspec( dllexport )OPENFILENAME CommunFichiers(LPCTSTR Titre,LPCTSTR Filtre){
	SHGetSpecialFolderPath(hWnd,szPath,CSIDL_PERSONAL,FALSE);
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrInitialDir = szPath;
	ofn.lpstrFilter =Filtre;
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle=Titre;
	ofn.lpstrFileTitle =NULL;
	ofn.nMaxFileTitle = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
return ofn;
}

__declspec( dllexport )void SauverFichier(LPCTSTR Titre){
	CommunFichiers(Titre,"Fichiers images\0*.bmp\0\0");
	sprintf(szPath,"%s$CaptureBack$.bmp",appPath);
	if (GetSaveFileName(&ofn)==TRUE) { 
	HANDLE hf = CreateFile(ofn.lpstrFile, GENERIC_WRITE,0, (LPSECURITY_ATTRIBUTES) NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,(HANDLE) NULL);
		if(MessageBox(hWnd,"Voulez vous supprimer le fichier tempon après la sauvegarde?\n\nCela permet une économie de ±3.00Mo d'espace Disque\n(Recommandé sous Windows 95,98,Me,NT4)",wc.lpszClassName,MB_YESNO)==0x06){
			MoveFile(szPath,ofn.lpstrFile);
		}else{
			if(CopyFile(szPath,ofn.lpstrFile,TRUE)==0){
				if(MessageBox(0,"Le fichier existe déjà \n Outrepasser ?",wc.lpszClassName,MB_YESNO)==0x06){
					 if (GetFileAttributes(ofn.lpstrFile)==INVALID_FILE_ATTRIBUTES) return; 
					 if (!(GetFileAttributes(ofn.lpstrFile) & FILE_ATTRIBUTE_READONLY)) { 
						 SetFileAttributes(ofn.lpstrFile, GetFileAttributes(ofn.lpstrFile) | FILE_ATTRIBUTE_READONLY); 
					 } 
					 CopyFile(szPath,ofn.lpstrFile,FALSE);
				}
			}
		}
	}
}

__declspec( dllexport )PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp){ 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&btmp)) 
        errhandler("GetObject", hwnd); 
    cClrBits = (WORD)(btmp.bmPlanes * btmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 
     if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits)); 
     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = btmp.bmWidth; 
    pbmi->bmiHeader.biHeight = btmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = btmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = btmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
    pbmi->bmiHeader.biCompression = BI_RGB; 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8* pbmi->bmiHeader.biHeight; 
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
} 

__declspec( dllexport )void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,HBITMAP hBMP, HDC hDC) { 
     HANDLE hf;
	 BITMAPFILEHEADER hdr;
	 PBITMAPINFOHEADER pbih;
	 LPBYTE lpBits;
	 DWORD dwTotal,cb,dwTmp; 
	 BYTE *hp; 
	 pbih = (PBITMAPINFOHEADER) pbi; 
	 lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
	 if (!lpBits)          errhandler("Alloction des tempons globaux", hwnd); 
	 if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi,DIB_RGB_COLORS)) {     errhandler("Détection des bist couleur de l'image", hwnd);     }
    hf = CreateFile(pszFile,GENERIC_READ | GENERIC_WRITE, (DWORD) 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) errhandler("Création du fichier Image", hwnd); 
    hdr.bfType = 0x4d42;
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD); 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL)){ errhandler("Écriture du fichier Image", hwnd); }
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, ( NULL)) )   errhandler("Écriture du fichier Image", hwnd); 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))    errhandler("Écriture du fichier Image", hwnd); 
     if (!CloseHandle(hf))     errhandler("Fermeture de la main", hwnd); 
	 GlobalFree((HGLOBAL)lpBits);
}
__declspec( dllexport )void CaptureEcran(HWND hDlg){
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL); 
	hdcCompatible = CreateCompatibleDC(hdcScreen); 
	hbmScreen = CreateCompatibleBitmap(hdcScreen, GetDeviceCaps(hdcScreen, HORZRES), GetDeviceCaps(hdcScreen, VERTRES)); 
 	GetClientRect(GetDesktopWindow(),&ec);
	GetClientRect(GetDlgItem(hDlg,ID_PREVIEW),&rc);
	sprintf(buffer,"Résolution: %dx%d\nRééchantionnée en %dx%d",ec.right,ec.bottom,rc.right,rc.bottom);
	SetDlgItemText(hDlg,ID_RESOLUTION,buffer);
	if (hbmScreen == 0) errhandler("Écher lors de l'obtention DC de l'écran", hDlg); 
 	if (!SelectObject(hdcCompatible, hbmScreen)) 	errhandler("Échec lors de la sélection Bitmap", hDlg); 
 	MoveWindow(hDlg,0,0,405,55,TRUE);
	if (!BitBlt(hdcCompatible, 0,0,  btmp.bmWidth, btmp.bmHeight, hdcScreen, 0,0, SRCCOPY))	errhandler("Échec lors de la copie des données de l'écran", hDlg); 
	CreateBMPFile(hDlg, "$CaptureBack$.bmp", CreateBitmapInfoStruct(hDlg,hbmScreen),hbmScreen,hdcCompatible);
	EnableMenuItem(GetSystemMenu(hDlg,FALSE),0xE145,MF_BYCOMMAND|MF_ENABLED);

}

__declspec( dllexport )void SysMenu(HWND hDlg){
	RemoveMenu(GetSystemMenu(hDlg,FALSE),SC_CLOSE,MF_STRING);
	RemoveMenu(GetSystemMenu(hDlg,FALSE),SC_MOVE,MF_STRING);
	RemoveMenu(GetSystemMenu(hDlg,FALSE),SC_SIZE,MF_STRING);
	RemoveMenu(GetSystemMenu(hDlg,FALSE),SC_MINIMIZE,MF_STRING);
	RemoveMenu(GetSystemMenu(hDlg,FALSE),SC_MAXIMIZE,MF_STRING);
	RemoveMenu(GetSystemMenu(hDlg,FALSE),SC_RESTORE,MF_STRING);
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING|MF_MENUBARBREAK,SC_MINIMIZE,"Réduire");
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING,SC_MAXIMIZE,"Agrandur");
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING,SC_RESTORE,"Restaurer");
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING,SC_SIZE,"Redimensionner");
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_SEPARATOR,MF_STRING,"");
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING,IDOK,"Capturer");
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING|MF_DISABLED,0xE145,"Sauvegarder la capture");
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_SEPARATOR,MF_STRING,"");
	AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING,2,"Fermer ce programme");
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_MOVE);
	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),SC_MOVE,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_BITMAP1);
	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),SC_MAXIMIZE,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_BITMAP3);
	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),SC_MINIMIZE,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_BITMAP4);
	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),SC_SIZE,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_BITMAP2);
	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),SC_RESTORE,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_QUITTER);
	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),IDCANCEL,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_CAPTURE);
	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),IDOK,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_SAVE);
	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),0xE145,MF_BYCOMMAND,bmp,bmp);
}
