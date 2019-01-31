#include <SFML/Graphics.hpp>
#include <iostream> // input
#include <cstdlib> // rand()
#include <ctime> // time()
#include <queue> // queue
#define MAX 100
#define SIZE 32.f
using namespace std;
using namespace sf;

int W, H, S, N; // marimea ferestrei + fps + nr de mine
int grid[MAX][MAX], box[MAX][MAX]; // grid pt logica jocului, box pt afisare
int checked[MAX][MAX];
Texture t; // pt imagini
Sprite s; // pt afisare
int m_i, m_j, m_b; // coordonate mouse + buton (0=niciunul, 1=stanga, 2=dreapta)
int di[8] = { -1,-1,0,1,1,1,-1 }, dj[8] = { 0,1,1,1,0,-1,-1,-1 };

void setup()
{
	/*cout << "W: "; cin >> W;
	cout << "H: "; cin >> H;
	cout << "S: "; cin >> S;
	cout << "N: "; cin >> N;*/
	W = 10; H = 20; S = 60; N = 20;
}

inline bool inside(int i, int j) { return(i > 0 && i <= H && j > 0 && j <= W); } // inauntrul matricei (true)

void addMines()
{
	srand(time(NULL)); // initializeaza seedul

	int i, j, m = N;
	while (m) // cat timp nu am terminat de adaugat cele N mine
	{
		i = rand() % H + 1; j = rand() % W + 1;
		if (grid[i][j] != 9) // loc liber
		{
			grid[i][j] = 9; // minez
			m--;
			for (int d = 0; d < 8; d++) // maresc nr de pericole in casetele libere adiacente acestei mine
				if (inside(i + di[d], j + dj[d]) && grid[i+di[d]][j+dj[d]] != 9) grid[i + di[d]][j + dj[d]]++;
		}
	}
}

void restart()
{
	int i, j;
	for (i = 1; i <= H; i++)
		for (j = 1; j <= W; j++)
		{
			grid[i][j] = 0; // liber
			box[i][j] = 10; // caseta nedezvaluita
			checked[i][j] = 0;
		}
}

void drawBox(RenderWindow &window)
{
	int i, j;
	for (i = 1; i <= H; i++)
		for (j = 1; j <= W; j++)
		{
			s.setPosition((j - 1)*SIZE, (i - 1)*SIZE);
			s.setTextureRect(IntRect(box[i][j] * 32, 0, 32, 32));
			window.draw(s);
		}
}

int main()
{
	// initializari
	setup();
	restart();
	addMines();
	if (!t.loadFromFile("resurse/tiles.jpg")) throw "Error"; // incarca imaginea
	s.setTexture(t); // o setam pe sprite-ul s

					 // fereastra
	RenderWindow window(VideoMode(W*SIZE, H*SIZE), "Minesweeper");
	window.setFramerateLimit(S);
	while (window.isOpen())
	{
		// evenimente
		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed) window.close();
			if (e.type == Event::MouseButtonPressed)
			{
				m_i = Mouse::getPosition(window).y; m_j = Mouse::getPosition(window).x;
				m_i /= SIZE; m_j /= SIZE; m_i++; m_j++; // astfel, m_i devine nr de ordine al liniei unde am facut click, iar m_j devine nr de ordine al coloanei unde am facut click
				if (e.mouseButton.button == Mouse::Left) m_b = 1;
				else if (e.mouseButton.button == Mouse::Right) m_b = 2;
			}
			if (e.type == Event::KeyPressed&&e.key.code == Keyboard::R) // r pt restart
			{
				restart();
				addMines();
			}
		}

		// logica
		window.clear();
		switch (m_b) // in functie de ce buton apasam
		{
		case 1: // stanga
			if (box[m_i][m_j] == 10) // am facut click pe o caseta nedezvaluita
			{
				if (grid[m_i][m_j] == 9) // caseta este de fapt minata
					for (int i = 1; i <= H; i++)
						for (int j = 1; j <= W; j++) box[i][j] = grid[i][j]; // dezvaluim toate celelalte casete, jocul s-a sfarsit
				else if (grid[m_i][m_j] == 0) // caseta nu are niciun vecin minat -> dezvaluim restul casetelor libere din apropiere
				{
					queue <pair <int, int> > q; // aici vom plasa vecinii pe care va trebui sa ii analizam
					q.push(make_pair(m_i, m_j));
					int ia, ja, iv, jv;
					while (!q.empty())
					{
						ia = q.front().first; ja = q.front().second; // coordonatele celulei actuale
						q.pop(); // am terminat cu celula asta, o pot scoate
						box[ia][ja] = grid[ia][ja]; // retin celula pt afisare
						checked[ia][ja] = 1; // demarchez celula actuala
						for (int d = 0; d < 8; d++)
						{
							iv = ia + di[d]; jv = ja + dj[d]; // coordonatele celulei vecine actuale
							if (inside(iv, jv) && grid[iv][jv] != 9 && !checked[iv][jv] && grid[ia][ja] == 0) q.push(make_pair(iv, jv));
						}
					}
				}
				else box[m_i][m_j] = grid[m_i][m_j]; // caseta are vecini minati; o dezvaluim doar pe aceasta
			}
			break;
		case 2: // dreapta
			if (box[m_i][m_j] == 10) box[m_i][m_j] = 11; // caseta nu a fost dezvaluita inca deci ii putem pune un stegulet pt siguranta
			else if (box[m_i][m_j] == 11) box[m_i][m_j] = 10; // daca are deja stegulet, putem sa il scoatem
			break;
		default: break;
		}
		m_b = 0;

		// afisare
		drawBox(window);

		window.display();
	}
	return 0;
}
