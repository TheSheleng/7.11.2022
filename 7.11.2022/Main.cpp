#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#define DEFAULT_STR_LEN 100

enum ANSW
{
	NO_ANSW,

	ANSW_1,
	ANSW_2,
	ANSW_3,
	ANSW_4,

	ANSW_END
};

const int cFQues = 5;
const int cSQues = 5;
const int cTQues = 2;

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
HWND hProg, hPGroupe;

DWORD WINAPI OnTimer(_In_ LPVOID hWnd)
{
	DWORD pr = GetPriorityClass(GetCurrentProcess());
	SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	TCHAR str[DEFAULT_STR_LEN]{};
	unsigned short time = 600;

	while (time > 0)
	{
		--time;
		wsprintf(str, TEXT("������� ��������: %i�. %i�."), time / 60, time - time / 60 * 60);
		SetWindowText(hPGroupe, str);

		Sleep(1000);
	}

	MessageBox((HWND)hWnd, TEXT("����� �����. �� �� ������ ����� ����.\n���������� � ��������� ���."), 
		TEXT(""), MB_OK | MB_ICONINFORMATION);
	EndDialog((HWND)hWnd, 0);

	SetPriorityClass(GetCurrentProcess(), pr);
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)DlgProc); 
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR str[DEFAULT_STR_LEN]{};

	switch (message)
	{
	case WM_INITDIALOG:
	{
		hProg = GetDlgItem(hWnd, IDC_PROGRESS1);
		hPGroupe = GetDlgItem(hWnd, IDC_TIME_PROG);

		SendMessage(hProg, PBM_SETRANGE, 0, MAKELPARAM(0, cFQues + cSQues + cTQues));
		SendMessage(hProg, PBM_SETSTEP, 1, 0);

		CreateThread(NULL, 0, OnTimer, hWnd, 0, NULL);
	}
	return TRUE;

	case  WM_COMMAND:
	{
		//�������� ��������
		int progress = 0;

		//������ ��������
		for (int i = 0; i < cFQues; i++)
			for (int y = 0; y < ANSW_END; y++)
				if (SendDlgItemMessage(
					hWnd, FIRST_RADIO + i * (ANSW_END - 1) + y,
					BM_GETCHECK, NULL, NULL)) 
				{ progress += 1; break; }

		//������ �������
		for (int i = 0; i < cSQues; i++)
		{
			for (int y = 0; y < ANSW_END; y++)
				if (SendDlgItemMessage(
					hWnd, FIRST_CHECK + i * (ANSW_END - 1) + y,
					BM_GETCHECK, NULL, NULL))
				{
					progress += 1; break;
				}
		}

		//������ �������
		for (int i = FIRST_EDIT; i <= LAST_EDIT; i++)
		{
			GetWindowText(GetDlgItem(hWnd, i), str, DEFAULT_STR_LEN);
			if (str[NULL] != '\0') progress += 1;
		}

		SendMessage(hProg, PBM_SETPOS, progress, 0);

		//�������� �����
		if (LOWORD(wParam) == IDC_BUTTON)
		{
			int result = 0;
			static const int SCORE_PER_ANSW = 20;

			//�������� ������ ��������
			ANSW RAD_ANSW[cFQues] = {
					ANSW_2, ANSW_4, ANSW_2, ANSW_4, ANSW_2
			};
			for (int i = 0; i < cFQues; i++)
			{
				if (SendDlgItemMessage(
					hWnd, FIRST_RADIO + i * (ANSW_END - 1) + RAD_ANSW[i] - 1,
					BM_GETCHECK, NULL, NULL
				)) result += SCORE_PER_ANSW;
			}

			//�������� ������ ��������
			static const bool CHE_ANSW[cSQues * (ANSW_END - 1)] =
			{
				false, true, true, false,	 // ������ 1
				true, true, false, false,	 // ������ 2
				false, true, true, true,	 // ������ 3
				true, false, true, false,	 // ������ 4
				false, true, false, true,	 // ������ 5
			};
			for (int i = FIRST_CHECK; i <= LAST_CHECK; i++)
			{
				if (SendDlgItemMessage(hWnd, i, BM_GETCHECK, NULL, NULL)
					== BST_CHECKED == CHE_ANSW[i - FIRST_CHECK])
					result += SCORE_PER_ANSW / (ANSW_END - 1);
			}

			//�������� ������ ��������
			static const TCHAR EDIT_ANSW[cTQues][DEFAULT_STR_LEN] = {
				TEXT("120"),
				TEXT("156"),
			};

			for (int i = FIRST_EDIT; i <= LAST_EDIT; i++)
			{
				GetWindowText(GetDlgItem(hWnd, i), str, DEFAULT_STR_LEN);
				if (!memcmp(str, EDIT_ANSW[i - FIRST_EDIT], DEFAULT_STR_LEN))
					result += SCORE_PER_ANSW;
			}

			//�����
			wsprintf(str, TEXT("���������: %i / %i"), result,
				((LAST_RADIO - FIRST_RADIO + 1) +
				(LAST_CHECK - FIRST_CHECK + 1)) *
				SCORE_PER_ANSW / (ANSW_END - 1) +
				(LAST_EDIT - FIRST_EDIT + 1) *
				SCORE_PER_ANSW
			);

			MessageBox(hWnd, str, TEXT(""), MB_OK);
			EndDialog(hWnd, 0);
		}
	}
	return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}