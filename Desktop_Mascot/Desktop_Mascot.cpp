#include "DxLib.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
#include <commctrl.h>

#include <tchar.h>

#pragma comment(lib, "comctl32.lib")

///ID　ハンドル定義
#define ID_TOOLBAR_BUTTON1 101
#define ID_TOOLBAR_BUTTON2 102

int UserIcon = 0;
int main_width = 0;
int main_height = 0;

bool FLAG = TRUE;//画像を変更したかどうかのフラグ

/*//////////////

デスクトップマスコットの
開発方針大幅変更

まず、デスクトップマスコットになる画像はユーザーさんにアップロードしてもらうことにした。
そのため、画像のサイズによってウィンドウサイズを変更する必要がある。
さらに、画像のサイズによって吹き出しの位置も変更する必要がある。
台詞についても、大幅変更orユーザーさん独自で設定できるようにする。

*/
///

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

void GetImageSize(const TCHAR* filePath, int* width, int* height)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	{
		Gdiplus::Image image(filePath);
		*width = image.GetWidth();
		*height = image.GetHeight();
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
}
//Gdiplusライブラリ







int Enter_chk(const TCHAR *centence)
{
	int counter = 0;
	const TCHAR* match = _T("\n");
	const TCHAR *centence_ptr = centence;
	while (true) {
		centence_ptr = _tcsstr(centence_ptr, match);
		if (centence_ptr != NULL)
		{
			counter++;
			centence_ptr++;
		}
		else
			break;
	}
	return counter + 1;
}

void preInitialize(int* width, int* height)
{
	SetGraphMode(*width, *height, 32);
	ChangeWindowMode(TRUE);
	SetWindowStyleMode(2);
	SetUseBackBufferTransColorFlag(TRUE);
}

void afterInitialize() {
	HWND hWnd = GetMainWindowHandle();
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetAlwaysRunFlag(TRUE);
	SetDrawScreen(DX_SCREEN_BACK);
}

////ツールバーウインドウハンドル
HWND hMenuWindow = NULL;

///ツールバーウインドウの作成///
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case ID_TOOLBAR_BUTTON1:
		{
			TCHAR szFileName[MAX_PATH] = { 0 };

			// OPENFILENAME 構造体の初期化
			OPENFILENAME ofn = { 0 };
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd; // オーナーウィンドウのハンドル
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = TEXT("画像ファイル\0*.jpg;*.png;*.bmp\0すべてのファイル\0*.*\0\0");
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

			// ファイル選択ダイアログの表示
			if (GetOpenFileName(&ofn))
			{
				//MessageBox(hWnd, szFileName, TEXT(szFileName), MB_OK);
				// 選択されたファイルパスが szFileName に格納されます
				// 必要な処理をここに追加します。例としてメッセージボックスに表示します。
				//MessageBox(hWnd, szFileName,NULL, MB_OK);
				
				//ここで問題が起こっている
				//UserIconに正常なファイルパスが入っていない
				GetImageSize(szFileName, &main_width, &main_height);

				// utf8FileName を使用して画像を読み込む
				UserIcon = LoadGraph(szFileName);
				DestroyWindow(hWnd);
				hMenuWindow = NULL;  // ウィンドウハンドルをクリア
				return 0;

			}
			else
			{
				// ユーザーがキャンセルした場合の処理
				MessageBox(hWnd, TEXT("ファイルの選択がキャンセルされました。"), TEXT("情報"), MB_OK);
				DestroyWindow(hWnd);
				hMenuWindow = NULL;  // ウィンドウハンドルをクリア
				return 0;
			}
			return 0;
		}
		case ID_TOOLBAR_BUTTON2:
			MessageBox(hWnd, L"ボタン2がクリックされました！", L"通知", MB_OK);
			return 0;
		}
		break;
	}
	case WM_DESTROY:
		DestroyWindow(hWnd);
		hMenuWindow = NULL;  // ウィンドウハンドルをクリア
		return 0;
	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return 0;
}




void Make_menu_window(POINT po) {

	if (hMenuWindow != NULL) {
		return;
	}

	// ウィンドウクラスの登録
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"MenuWindowClass";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClass(&wc);

	// メニューウィンドウの作成
    hMenuWindow = CreateWindowEx(WS_EX_TOOLWINDOW, L"MenuWindowClass", NULL,
		WS_POPUP ,
		po.x - 200, po.y - 200, 200, 200,
		NULL, NULL, GetModuleHandle(NULL), NULL);

	if (!hMenuWindow) {
		MessageBox(NULL, L"メニューウィンドウの作成に失敗しました。", L"エラー", MB_OK | MB_ICONERROR);
		return;
	}
	
	// ボタンの作成
	HWND hButton1 = CreateWindow(L"BUTTON", L"設定",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		20, 20, 160, 30,
		hMenuWindow,
		(HMENU)ID_TOOLBAR_BUTTON1,
		GetModuleHandle(NULL),
		NULL);

	HWND hButton2 = CreateWindow(L"BUTTON", L"ボタン2",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		20, 60, 160, 30,
		hMenuWindow,
		(HMENU)ID_TOOLBAR_BUTTON2,
		GetModuleHandle(NULL),
		NULL);

	// ウィンドウを最前面に設定
	SetWindowPos(hMenuWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


	// ウィンドウを表示
	ShowWindow(hMenuWindow, SW_SHOW);
	UpdateWindow(hMenuWindow);

	// メッセージループの開始
	//
	/*MSG msg;
	while (IsWindow(hMenuWindow) && GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(hMenuWindow, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}*/
}

void mainsystem(int width, int height)
{
	int Cr = GetColor(255, 204, 255);
	//int Arisa_Meido = LoadGraph(L"アーリャメイド服.png");
	//int Arisa_Seifuku = LoadGraph(L"アーリャ制服.png");
	//int Maria_Tenshi = LoadGraph(L"マーシャ天使.png");
	int default_icon = LoadGraph(L"DEFALT.png");
	GetImageSize(L"DEFALT.png", &main_width, &main_height);
	int fukidashi = LoadGraph(L"吹き出し画像.png");
	SetFontSize(35);
	int FontHandle = CreateFontToHandle(NULL, 40, 3);
	int SELECT_GRAPH = default_icon;
	UserIcon = SELECT_GRAPH;
	int SELECT_WORD = 6;
	time_t now = time(NULL);
	struct tm pnow;
	localtime_s(&pnow, &now);
	POINT po;

	static const TCHAR* Serifu[] = {
		_T("Не уходи...♥"),
		_T("テッテッテ、\nテーストー"),
		_T("カッコいい系\n女子、正直スコです。"),
		_T("なでなで検知\nいい感じ"),
		_T("三行用の\nテスト\nしちゃう？！"),
		_T("Адкий\nлюбитель\nженских\nножек"),
		_T("うおおおおおおお\nこれでどうじゃあああ!!"),
	};


	while (ProcessMessage() == 0)
	{
		/*if (FLAG == TRUE)
		{
			FLAG = FALSE;
			DrawGraph(width - 250, height - 450, SELECT_GRAPH, TRUE);//キャラ描画
		}*/
		SELECT_GRAPH = UserIcon;
		ClearDrawScreen();
		DrawGraph(width - main_width, height - main_height, SELECT_GRAPH, TRUE);//キャラ描画
		DrawGraph(width - 400, height - 650, fukidashi, TRUE);//吹き出し描写
		///\nの数によって行数を判別//
		if (Enter_chk(Serifu[SELECT_WORD]) == 1)
		{
			SetFontSize(35);
			DrawString(width - 365, height - 540, Serifu[SELECT_WORD], Cr);//台詞描写１行
		}

		else if (Enter_chk(Serifu[SELECT_WORD]) == 2)
		{
			SetFontSize(35);
			DrawString(width - 365, height - 560, Serifu[SELECT_WORD], Cr);//台詞描写２行
		}

		else if (Enter_chk(Serifu[SELECT_WORD]) == 3)
		{
			SetFontSize(30);
			DrawString(width - 365, height - 570, Serifu[SELECT_WORD], Cr);//台詞描写3行
		}
		else if (Enter_chk(Serifu[SELECT_WORD]) == 4)
		{
			SetFontSize(25);
			DrawString(width - 365, height - 580, Serifu[SELECT_WORD], Cr);//台詞描写4行
		}

		ScreenFlip();

		/*if (CheckHitKey(KEY_INPUT_1)) {
			SELECT_GRAPH = Arisa_Meido;
		}
		if (CheckHitKey(KEY_INPUT_2)) {
			SELECT_GRAPH = Arisa_Seifuku;
		}
		if (CheckHitKey(KEY_INPUT_3)) {
			SELECT_GRAPH = Maria_Tenshi;
		}*/


		///////なでなで機能問題点
		///////撫でてる最中のアニメーションが欲しい
		if (GetKeyState(VK_LBUTTON) & 0x80) {//左クリ
			GetCursorPos(&po);
			if (po.x >= width - main_width and po.y >= height - main_height)
			{
				//SELECT_GRAPH = Arisa_Seifuku;
				ULONGLONG start_time = ::GetTickCount64();
				ULONGLONG now_time = 0;
				int move_distance = 0;
				int defalt_point = po.x;
				while (now_time < 500)//マウスの移動距離計算
				{
					now_time = ::GetTickCount64() - start_time;
					Sleep(5);
					move_distance += abs(po.x - defalt_point);
					GetCursorPos(&po);
				}
				if (GetKeyState(VK_LBUTTON) & 0x80 and move_distance >= 500 * 2 * 5 and abs(po.x - defalt_point) < 500)//基準値*なでなで（往復）回数をチェック、同時にポインターが離れ過ぎてないか確認
				{
					SELECT_WORD = 3;
				}
			}
		}
		if (GetKeyState(VK_RBUTTON) & 0x80) {//右クリ
			GetCursorPos(&po);
			if (po.x >= width - main_width and po.y >= height - main_height) {
				Make_menu_window(po);
				//SELECT_GRAPH = UserIcon;
			}
				
		}
		if (CheckHitKey(KEY_INPUT_Q)) {
			DeleteGraph(UserIcon);
			DeleteGraph(default_icon);
			break;
		}
	}
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	int width = 0;
	int height = 0;
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	width = 2560;
	height = 1440;
	//width = 1920;
	//height = 1080;

	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
	preInitialize(&width, &height);
	if (DxLib_Init() == -1)
		return -1;
	afterInitialize();
	////////////////////////////////////////

	////メニューウインドウ画面///

	mainsystem(width, height);
	DxLib_End();
	return 0;
}