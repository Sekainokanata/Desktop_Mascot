#include <stdio.h>
#include "DxLib.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

////ツールバーウインドウハンドル
HWND hToolbarWnd = NULL;

///ID　ハンドル定義
#define ID_TOOLBAR_BUTTON1 101
#define ID_TOOLBAR_BUTTON2 102


/*//////////////

デスクトップマスコットの
開発方針大幅変更

まず、デスクトップマスコットになる画像はユーザーさんにアップロードしてもらうことにした。
そのため、画像のサイズによってウィンドウサイズを変更する必要がある。
さらに、画像のサイズによって吹き出しの位置も変更する必要がある。
台詞についても、大幅変更orユーザーさん独自で設定できるようにする。

*/
///

int Enter_chk(const char* centence)
{
	int counter = 0;
	const char* match = "\n";
	const char* centence_ptr = centence;
	while (true) {
		centence_ptr = strstr(centence_ptr, match);
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

///ツールバーウインドウの作成///
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
			case ID_TOOLBAR_BUTTON1:
				MessageBox(hWnd, "ボタン1がクリックされました！", "通知", MB_OK);
				break;
			case ID_TOOLBAR_BUTTON2:
				MessageBox(hWnd, "ボタン2がクリックされました！", "通知", MB_OK);
				break;
		}
		return 0;
	}
	case WM_DESTROY:
		DestroyWindow(hWnd);
		return 0;
	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
}




void Make_menu_window(POINT po) {
		
	if (hToolbarWnd != NULL) {
		return;
	}

	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "TOOLBAR";
	//wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClass(&wc);



	//hToolbarWnd = CreateWindow("Toolbar", "TOOL", WS_DLGFRAME, 0, 0, 200, 200, NULL, NULL, GetModuleHandle(NULL), NULL);
	InitCommonControls();  // 共通コントロールの初期化（ツールバー用）
	hToolbarWnd = CreateWindowEx(WS_EX_TOOLWINDOW,TOOLBARCLASSNAME,NULL, WS_POPUP | WS_BORDER | WS_CHILD,po.x-200,po.y-200,200,200,NULL,NULL,NULL,NULL);//問題ナシ
	// イメージリストの作成（ボタンのアイコン用）
	HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR32, 2, 2);
	HICON hIcon1 = LoadIcon(NULL, IDI_INFORMATION);
	HICON hIcon2 = LoadIcon(NULL, IDI_WARNING);
	ImageList_AddIcon(hImageList, hIcon1);
	ImageList_AddIcon(hImageList, hIcon2);
	SendMessage(hToolbarWnd, TB_SETIMAGELIST, 0, (LPARAM)hImageList);

	// ツールバーのボタン情報
	TBBUTTON tbb[2] = { 0 };
	tbb[0].iBitmap = 0; // アイコンのインデックス
	tbb[0].idCommand = ID_TOOLBAR_BUTTON1;
	tbb[0].fsState = TBSTATE_ENABLED;
	tbb[0].fsStyle = TBSTYLE_BUTTON;
	tbb[0].iString = (INT_PTR)"ボタン1";

	tbb[1].iBitmap = 1;
	tbb[1].idCommand = ID_TOOLBAR_BUTTON2;
	tbb[1].fsState = TBSTATE_ENABLED;
	tbb[1].fsStyle = TBSTYLE_BUTTON;
	tbb[1].iString = (INT_PTR)"ボタン2";

	// ツールバーにボタンを追加

	///ここが問題。TB_BUTTONSTRUCTSIZEが失敗している
    SendMessage(hToolbarWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    if (!SendMessageA(hToolbarWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0)) {
    MessageBox(NULL, "TB_BUTTONSTRUCTSIZE failed", "Error", MB_OK);
    }

	SendMessage(hToolbarWnd, TB_ADDBUTTONS, (WPARAM)2, (LPARAM)&tbb);

	//if (!SendMessageA(hToolbarWnd, TB_ADDBUTTONS, (WPARAM)2, (LPARAM)&tbb)) {
	//	MessageBox(NULL, "TB_AUTOSIZE failed", "Error", MB_OK);
	//}
	//こちらは成功した

	SetWindowPos(hToolbarWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(hToolbarWnd, SW_SHOW);
	UpdateWindow(hToolbarWnd);



	// メッセージループ
	//多分ループはきちんと回っているが、メッセージが送られていない
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!IsDialogMessage(hToolbarWnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

}

void mainsystem(int width, int height)
{
	int Cr = GetColor(255, 204, 255);
	int Arisa_Meido = LoadGraph(u8"アーリャメイド服.png");
	int Arisa_Seifuku = LoadGraph(u8"アーリャ制服.png");
	int Maria_Tenshi = LoadGraph(u8"マーシャ天使.png");
	int fukidashi = LoadGraph(u8"吹き出し画像.png");
	SetFontSize(35);
	int FontHandle = CreateFontToHandle(NULL, 40, 3);
	int SELECT_GRAPH = Arisa_Meido;
	int SELECT_WORD = 6;
	time_t now = time(NULL);
	struct tm pnow;
	localtime_s(&pnow, &now);
	POINT po;

	static char Serifu[][100] = {
			u8"Не уходи...♥",
			u8"テッテッテ、\nテーストー",
			u8"カッコいい系\n女子、正直スコです。",
			u8"なでなで検知\nいい感じ",
			u8"三行用の\nテスト\nしちゃう？！",
			u8"Адкий\nлюбитель\nженских\nножек",
			u8"うおおおおおおお\nこれでどうじゃあああ!!",
	};

	while (ProcessMessage() == 0)
	{
		ClearDrawScreen();
		DrawGraph(width - 250, height - 450, SELECT_GRAPH, TRUE);//キャラ描画
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

		if (CheckHitKey(KEY_INPUT_1)) {
			SELECT_GRAPH = Arisa_Meido;
		}
		if (CheckHitKey(KEY_INPUT_2)) {
			SELECT_GRAPH = Arisa_Seifuku;
		}
		if (CheckHitKey(KEY_INPUT_3)) {
			SELECT_GRAPH = Maria_Tenshi;
		}


		///////なでなで機能問題点
		///////撫でてる最中のアニメーションが欲しい
		if (GetKeyState(VK_LBUTTON) & 0x80) {//左クリ
			GetCursorPos(&po);
			if (po.x >= width - 250 and po.y >= height - 450)
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
			if (po.x >= width - 250 and po.y >= height - 450)
				Make_menu_window(po);
		}
		if (CheckHitKey(KEY_INPUT_Q)) {
			DeleteGraph(Arisa_Meido);
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
	//width = 2560;
	//height = 1440;
	width = 1920;
	height = 1080;

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
