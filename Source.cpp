#include <Windows.h>
#include <random>
#include <chrono>

//TODO: Сделать статическое окно (Выполнено)
//TODO: Нарисовать поле для крестиков ноликов (Выполнено)
//TODO: Сделать вывод крестиков и ноликов (Выполнено)
//TODO: Сделать обработку нажатий (Выполнено)
//TODO: Сделать логику крестиков ноликов

enum class TCell
{
	EMPTY,
	CROSS,
	ZERO
};

enum class TStatus
{
	IN_PROGRESS,
	WON_X,
	WON_O,
	DRAW
};

struct TCoord
{
	int x1, x2, y1, y2;
};

struct TGame
{
	TCell** field = nullptr;
	TCell human = TCell::CROSS, ai = TCell::ZERO;
	TCell* turn = &human;
	TStatus status = TStatus::IN_PROGRESS;
	int size = 3;
	size_t to_win = 3;
	TCoord** coord = nullptr;
};

TGame game; // Структура игры

int32_t getRandomNum(int32_t min, int32_t max) {
	const static auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	static std::mt19937_64 generator(seed);
	std::uniform_int_distribution<int32_t> dis(min, max);
	return dis(generator);
}

void InitCoord(RECT* clientRect)
{
	game.coord = new TCoord * [game.size];
	for (int i = 0; i < game.size; i++)
	{
		game.coord[i] = new TCoord[game.size];
	}

	int x, y;

	x = clientRect->right / game.size;
	y = clientRect->bottom / game.size;

	for (int i = 0; i < game.size; i++)
	{
		for (int j = 0; j < game.size; j++)
		{
			int x1, x2, y1, y2;
			x1 = x * j;
			x2 = x * (j + 1);
			y1 = y * i;
			y2 = y * (i + 1);
			game.coord[i][j].x1 = x1;
			game.coord[i][j].x2 = x2;
			game.coord[i][j].y1 = y1;
			game.coord[i][j].y2 = y2;
		}
	}
}

void InitField()
{
	game.field = new TCell * [game.size];
	for (int i = 0; i < game.size; i++)
	{
		game.field[i] = new TCell[game.size];
	}
	for (int i = 0; i < game.size; i++)
	{
		for (int j = 0; j < game.size; j++)
		{
			game.field[i][j] = TCell::EMPTY;
		}
	}
}

void InitGame(RECT* clientRect)
{
	game.size = 3;
	game.to_win = 3;
	game.status = TStatus::IN_PROGRESS;
	if (getRandomNum(0, 1000) < 500) {
		game.human = TCell::CROSS;
		game.ai = TCell::ZERO;
		game.turn = &game.human;
	}
	else {
		game.human = TCell::ZERO;
		game.ai = TCell::CROSS;
		game.turn = &game.ai;
	}
	InitField();
	InitCoord(clientRect);
}

void FreeGame()
{
	for (int i = 0; i < game.size; i++)
	{
		delete[] game.field[i];
		delete[] game.coord[i];
	}
	delete[] game.field;
	delete[] game.coord;
}

BOOL DrawLine(HDC hdc, WORD x1, WORD y1, WORD x2, WORD y2)
{
	POINT pt;
	MoveToEx(hdc, x1, y1, &pt);
	return LineTo(hdc, x2, y2);
}

void CheckGame()
{
	// Ряды
	for (int i = 0; i < game.size; i++)
	{
		size_t count = 0;
		TCell cell = TCell::EMPTY;
		for (int j = 0; j < game.size; j++)
		{
			if (count == game.to_win)
			{
				break;
			}
			else if (cell != game.field[i][j])
			{
				cell = game.field[i][j];
				count = 1;
			}
			else if (cell == game.field[i][j] && cell != TCell::EMPTY)
			{
				count++;
			}
		}
		if (count == game.to_win) {
			if (cell == TCell::CROSS)
				game.status = TStatus::WON_X;
			else if (cell == TCell::ZERO)
				game.status = TStatus::WON_O;
			return;
		}
	}

	// Столбцы
	for (int i = 0; i < game.size; i++)
	{
		size_t count = 0;
		TCell cell = TCell::EMPTY;
		for (int j = 0; j < game.size; j++)
		{
			if (count == game.to_win)
			{
				break;
			}
			else if (cell != game.field[j][i])
			{
				cell = game.field[j][i];
				count = 1;
			}
			else if (cell == game.field[j][i] && cell != TCell::EMPTY)
			{
				count++;
			}
		}
		if (count == game.to_win) {
			if (cell == TCell::CROSS)
				game.status = TStatus::WON_X;
			else if (cell == TCell::ZERO)
				game.status = TStatus::WON_O;
			return;
		}
	}

	// Диагональ слева направо
	for (int q = 0; q < game.size; q++)
	{
		for (int i = 0; i < game.size; i++)
		{
			size_t count = 0;
			TCell cell = TCell::EMPTY;
			for (int j = 0; j < game.size; j++)
			{
				if (count == game.to_win || (i + j) >= game.size || (j + q) >= game.size)
				{
					break;
				}
				else if (cell != game.field[j + q][i + j])
				{
					cell = game.field[j + q][i + j];
					count = 1;
				}
				else if (cell == game.field[j + q][i + j] && cell != TCell::EMPTY)
				{
					count++;
				}
			}
			if (count == game.to_win) {
				if (cell == TCell::CROSS)
					game.status = TStatus::WON_X;
				else if (cell == TCell::ZERO)
					game.status = TStatus::WON_O;
				return;
			}
		}
	}

	// Диагональ справа налево
	for (int q = 0; q < game.size; q++)
	{
		for (int i = 0; i < game.size; i++)
		{
			size_t count = 0;
			TCell cell = TCell::EMPTY;
			for (int j = 0; j < game.size; j++)
			{
				if (count == game.to_win || (game.size - 1 - i - j) < 0 || (j + q) >= game.size)
				{
					break;
				}
				else if (cell != game.field[j + q][game.size - 1 - i - j])
				{
					cell = game.field[j + q][game.size - 1 - i - j];
					count = 1;
				}
				else if (cell == game.field[j + q][game.size - 1 - i - j] && cell != TCell::EMPTY)
				{
					count++;
				}
			}
			if (count == game.to_win) {
				if (cell == TCell::CROSS)
					game.status = TStatus::WON_X;
				else if (cell == TCell::ZERO)
					game.status = TStatus::WON_O;
				return;
			}
		}
	}

	// Ничья
	bool draw = true;
	for (int i = 0; i < game.size; i++)
		for (int j = 0; j < game.size; j++)
			if (game.field[i][j] == TCell::EMPTY)
				draw = false;
	if (draw)
		game.status = TStatus::DRAW;
} 

void AIMove()
{
	/*
	// Ряды
	for (int q = 0; q < game.size; q++)
	{
		for (int i = 0; i < game.size - game.to_win + 1; i++)
		{
			size_t count = 0;
			bool cntr = false;
			int x = 0, y = 0;
			int cntrx = 0, cntry = 0;
			int x1 = 0, y1 = 0;;
			for (int j = 0; j < game.to_win && (i + j) < game.size; j++)
			{
				x1 = q;
				y1 = i + j;
				if (game.field[x1][y1] == game.ai)
				{
					count = 0;
					cntr = false;
					break;
				}
				else if ((game.size - game.to_win) ? count == game.to_win - 2 : count == game.to_win - 1)
				{
					if (game.field[x1][y1] == TCell::EMPTY)
					{
						game.field[x1][y1] = game.ai;
						game.turn = &game.human;
						return;
					}
				}
				else if (game.field[x1][y1] == game.human)
				{
					count++;
					x = x1;
					y = y1;
				}
				else if (game.field[x1][y1] == TCell::EMPTY)
				{
					if (y1 - 1 >= 0 && y1 + 1 < game.to_win)
					{
						if (game.field[x1][y1 - 1] == game.human && game.field[x1][y1 + 1] == game.human)
						{
							cntrx = x1;
							cntry = y1;
							continue;
						}
					}
				}
			}
			if ((game.size - game.to_win) ? count == game.to_win - 2 : count == game.to_win - 1)
			{
				if (y - count >= 0 && game.field[x][y - count] == TCell::EMPTY)
				{
					game.field[x][y - count] = game.ai;
					game.turn = &game.human;
					return;
				}
				else if (cntr)
				{
					game.field[cntrx][cntry] = game.ai;
					game.turn = &game.human;
					return;
				}
			}
		}
	}

	// Столбцы
	for (int q = 0; q < game.size; q++)
	{
		for (int i = 0; i < game.size - game.to_win + 1; i++)
		{
			size_t count = 0;
			bool cntr = false;
			int x = 0, y = 0;
			int cntrx = 0, cntry = 0;
			int x1 = 0, y1 = 0;
			for (int j = 0; j < game.to_win && (i + j) < game.size; j++)
			{
				x1 = i + j;
				y1 = q;
				if (game.field[x1][y1] == game.ai)
				{
					count = 0;
					cntr = false;
					break;
				}
				else if ((game.size - game.to_win) ? count == game.to_win - 2 : count == game.to_win - 1)
				{
					if (game.field[x1][y1] == TCell::EMPTY)
					{
						game.field[x1][y1] = game.ai;
						game.turn = &game.human;
						return;
					}
				}
				else if (game.field[x1][y1] == game.human)
				{
					count++;
					x = x1;
					y = y1;
				}
				else if (game.field[x1][y1] == TCell::EMPTY)
				{
					if (x1 - 1 >= 0 && x1 + 1 < game.to_win)
					{
						if (game.field[x1 - 1][i] == game.human && game.field[x1 + 1][i] == game.human)
						{
							cntrx = x1;
							cntry = y1;
							continue;
						}
					}
				}
			}
			if ((game.size - game.to_win) ? count == game.to_win - 2 : count == game.to_win - 1)
			{
				if (x - count >= 0 && game.field[x - count][i] == TCell::EMPTY)
				{
					game.field[x - count][y] = game.ai;
					game.turn = &game.human;
					return;
				}
				else if (cntr)
				{
					game.field[cntrx][cntry] = game.ai;
					game.turn = &game.human;
					return;
				}
			}
		}
	}

	// Диагональ слева направо
	for (int w = 0; w < game.size; w++)
	{
		for (int q = 0; q < game.size; q++)
		{
			for (int i = 0; i < game.size - game.to_win + 1; i++)
			{
				size_t count = 0;
				bool cntr = false;
				int x = 0, y = 0;
				int cntrx = 0, cntry = 0;
				int x1 = 0, y1 = 0;
				for (int j = 0; j < game.to_win && (w + j + i) < game.size && (q + i + j) < game.size; j++)
				{
					x1 = w + j + i;
					y1 = q + i + j;
					if (game.field[x1][y1] == game.ai)
					{
						count = 0;
						cntr = false;
						break;
					}
					else if ((game.size - game.to_win) ? count == game.to_win - 2 : count == game.to_win - 1)
					{
						if (game.field[x1][y1] == TCell::EMPTY)
						{
							game.field[x1][y1] = game.ai;
							game.turn = &game.human;
							return;
						}
					}
					else if (game.field[x1][y1] == game.human)
					{
						count++;
						x = x1;
						y = y1;
					}
					else if (game.field[x1][y1] == TCell::EMPTY)
					{
						if (x1 - 1 >= 0 && x1 + 1 < game.size)
						{
							if (y1 - 1 >= 0 && y1 + 1 < game.size)
							{
								cntr = true;
								cntrx = x1;
								cntry = y1;
							}
						}
					}

				}
				if ((game.size - game.to_win) ? count == game.to_win - 2 : count == game.to_win - 1)
				{
					if (x - count >= 0 && y - count >= 0)
					{
						if (game.field[x - count][y - count] == TCell::EMPTY)
						{
							game.field[x - count][y - count] = game.ai;
							game.turn = &game.human;
							return;
						}
					}
					else if (cntr)
					{
						game.field[cntrx][cntry] = game.ai;
						game.turn = &game.human;
						return;
					}
				}
			}
		}
	}

	// Диагональ справа налево
	for (int w = 0; w < game.size; w++)
	{
		for (int q = 0; q < game.size; q++)
		{
			for (int i = 0; i < game.size - game.to_win + 1; i++)
			{
				size_t count = 0;
				bool cntr = false;
				int x = 0, y = 0;
				int cntrx = 0, cntry = 0;
				int x1 = 0, y1 = 0;
				for (int j = 0; j < game.to_win && (w + j + i) < game.size && (game.size - j - 1 - q - i) >= 0; j++)
				{
					x1 = w + j + i;
					y1 = game.size - j - 1 - q - i;
					if (game.field[x1][y1] == game.ai)
					{
						count = 0;
						cntr = -1;
						break;
					}
					else if ((game.size - game.to_win) ? count == game.to_win - 2 : count == game.to_win - 1)
					{
						if (game.field[x1][y1] == TCell::EMPTY)
						{
							game.field[x1][y1] = game.ai;
							game.turn = &game.human;
							return;
						}
					}
					else if (game.field[x1][y1] == game.human)
					{
						count++;
						x = x1;
						y = y1;
					}
					else if (game.field[x1][y1] == TCell::EMPTY)
					{
						if (x1 - 1 >= 0 && x1 + 1 < game.size)
						{
							if (y1 - 1 >= 0 && y1 + 1 < game.size)
							{
								cntr = true;
								cntrx = x1;
								cntry = y1;
							}
						}
					}

				}
				if ((game.size - game.to_win) ? count == game.to_win - 2 : count == game.to_win - 1)
				{
					if (x - count >= 0 && y + count < game.size)
					{
						if (game.field[x - count][y + count] == TCell::EMPTY)
						{
							game.field[x - count][y + count] = game.ai;
							game.turn = &game.human;
							return;
						}
					}
					else if (cntr)
					{
						game.field[cntrx][cntry] = game.ai;
						game.turn = &game.human;
						return;
					}
				}
			}
		}
	}
	*/

	while (true)
	{
		int x = getRandomNum(0, game.size - 1);
		int y = getRandomNum(0, game.size - 1);
		if (game.field[x][y] == TCell::EMPTY)
		{
			game.field[x][y] = game.ai;
			game.turn = &game.human;
			return;
		}
	}
}

void HumanMove(int x, int y)
{
	if (game.field[x][y] == TCell::EMPTY)
	{
		game.field[x][y] = game.human;
		game.turn = &game.ai;
	}
}

void ClickHandl(WORD xPos, WORD yPos)
{
	for (int i = 0; i < game.size; i++)
	{
		if (yPos < game.coord[i][0].y2)
		{
			for (int j = 0; j < game.size; j++)
			{
				if (xPos < game.coord[i][j].x2)
				{
					HumanMove(i, j);
					return;
				}
			}
		}
	}
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT clientRect;
	HPEN hPen;
	HBRUSH hBrush;
	POINT pt;
	int x3, y3, k;

	GetClientRect(hWnd, &clientRect);

	switch (message)
	{
	case (WM_CREATE):
		InitGame(&clientRect);
		break;
	case (WM_PAINT):
		k = clientRect.right / game.size / 15;

		hdc = BeginPaint(hWnd, &ps);

		hBrush = CreateSolidBrush(RGB(42, 180, 201));

		FillRect(hdc, &clientRect, hBrush);

		hPen = CreatePen(PS_SOLID, k, BLACK_PEN);

		SelectObject(hdc, hPen);
		SelectObject(hdc, hBrush);

		CheckGame();

		if (game.status == TStatus::IN_PROGRESS)
		{
			if (*game.turn == game.ai)
			{
				AIMove();
			}

			x3 = clientRect.right;
			y3 = clientRect.bottom;

			for (int i = 0; i < game.size - 1; i++)
			{
				DrawLine(hdc, game.coord[i][i].x2, 0, game.coord[i][i].x2, y3);
				DrawLine(hdc, 0, game.coord[i][i].y2, x3, game.coord[i][i].y2);
			}

			for (int i = 0; i < game.size; i++)
			{
				for (int j = 0; j < game.size; j++)
				{
					if (game.field[i][j] == TCell::CROSS)
					{
						DrawLine(hdc, game.coord[i][j].x1 + k, game.coord[i][j].y1 + k, game.coord[i][j].x2 - k, game.coord[i][j].y2 - k);
						DrawLine(hdc, game.coord[i][j].x2 - k, game.coord[i][j].y1 + k, game.coord[i][j].x1 + k, game.coord[i][j].y2 - k);
					}
					else if (game.field[i][j] == TCell::ZERO)
					{
						Ellipse(hdc, game.coord[i][j].x1 + k, game.coord[i][j].y1 + k, game.coord[i][j].x2 - k, game.coord[i][j].y2 - k);
					}
				}
			}
			
		}
		else
		{
			WORD nSize = 0;
			TCHAR szBuf[80];
			
			if (game.status == TStatus::WON_X)
			{
				nSize = wsprintf(szBuf, L"X WON");
			}
			else if (game.status == TStatus::WON_O)
			{
				nSize = wsprintf(szBuf, L"O WON");
			}
			else if (game.status == TStatus::DRAW)
			{
				nSize = wsprintf(szBuf, L"DRAW");
			}
			DrawText(hdc, szBuf, nSize, &clientRect, DT_CENTER);
		}

		DeleteObject(hBrush);
		DeleteObject(hPen);

		EndPaint(hWnd, &ps);
		break;
	case (WM_LBUTTONDOWN):
		CheckGame();

		if (game.status != TStatus::IN_PROGRESS)
		{
			PostQuitMessage(0);
		}

		WORD xPos, yPos;

		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);

		if (*game.turn == game.human)
		{
			ClickHandl(xPos, yPos);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case (WM_DESTROY):
		PostQuitMessage(0);
		break;	
	case (WM_QUIT):
		FreeGame();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	TCHAR szWindowClass[] = L"DesktopApp";
	TCHAR szTitle[] = L"TicTacToe";
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(nullptr, L"Call to RegisterClassEx failed!", L"Checkout", NULL);
		return 1;
	}

	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, szWindowClass, szTitle, WS_EX_STATICEDGE | WS_SYSMENU, 300, 300, 300, 300, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		MessageBox(nullptr, L"Call to CreateWindowEx failed!", L"Checkout", NULL);
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}