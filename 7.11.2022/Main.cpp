#include <windows.h>
#include <tchar.h>
#include "resource.h"

#define DEFAULT_STR_LEN 100

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)DlgProc); 
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR str[DEFAULT_STR_LEN]{};
	switch (message)
	{
	case  WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON)
		{
			int result = 0;
			static const int SCORE_PER_ANSW = 20;

			//проверка первых вопросов
			static enum
			{
				NO_ANSW,

				ANSW_1,
				ANSW_2,
				ANSW_3,
				ANSW_4,

				ANSW_END
			} RAD_ANSW[(LAST_RADIO - FIRST_RADIO + 1) / (ANSW_END - 1)] = {
					ANSW_2, ANSW_4, ANSW_2, ANSW_4, ANSW_2 
				};
			for (int i = 0; i < ANSW_END; i++)
			{
				if (SendDlgItemMessage(
					hWnd, FIRST_RADIO + i * (ANSW_END - 1) + RAD_ANSW[i] - 1,
					BM_GETCHECK, NULL, NULL
				)) result += SCORE_PER_ANSW;
			}

			//Проверка вторых вопросов
			static const bool CHE_ANSW[LAST_CHECK - FIRST_CHECK + 1] =
			{
				false, true, true, false,	 // вопрос 1
				true, true, false, false,	 // вопрос 2
				false, true, true, true,	 // вопрос 3
				true, false, true, false,	 // вопрос 4
				false, true, false, true,	 // вопрос 5
			};
			for (int i = FIRST_CHECK; i <= LAST_CHECK; i++)
			{
				if (SendDlgItemMessage(hWnd, i, BM_GETCHECK, NULL, NULL) 
					== BST_CHECKED == CHE_ANSW[i - FIRST_CHECK]) 
					result += SCORE_PER_ANSW / (ANSW_END - 1);
			}

			//проверка третих вопросов
			static const TCHAR EDIT_ANSW[][DEFAULT_STR_LEN] = { 
				TEXT("120"), 
				TEXT("156"), 
			};

			for (int i = FIRST_EDIT; i <= LAST_EDIT; i++)
			{
				GetWindowText(GetDlgItem(hWnd, i), str, DEFAULT_STR_LEN);
				if (!memcmp(str, EDIT_ANSW[i - FIRST_EDIT], DEFAULT_STR_LEN))
					result += SCORE_PER_ANSW;
			}

			//Вывод
			wsprintf(str, TEXT("Результат: %i / %i"), result, 
				((LAST_RADIO - FIRST_RADIO + 1) +
				(LAST_CHECK - FIRST_CHECK + 1)) *
				SCORE_PER_ANSW / (ANSW_END - 1) +
				(LAST_EDIT - FIRST_EDIT + 1) *
				SCORE_PER_ANSW
			);

			MessageBox(hWnd, str, TEXT(""), MB_OK);
		}
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}