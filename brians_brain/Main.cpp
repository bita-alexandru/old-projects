#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#define SIZE 8.f
#define WIDTH 130
#define HEIGHT 70
#define MAX 500
using namespace std;
using namespace sf;

int cells[MAX][MAX];
enum state { DEAD = 0, LIVE = 1, DYING = 2, WILL_LIVE = 3 };
RectangleShape cell(Vector2f(SIZE - 1, SIZE - 1));
Color cl_dying(130, 210, 211), cl_live(220, 30, 67);

void randomCells()
{
	srand(time(NULL));
	for (int i = 1; i <= HEIGHT; i++)
		for (int j = 1; j <= WIDTH; j++) cells[i][j] = rand() % 2;
}

void clearCells()
{
	for (int i = 1; i <= HEIGHT; i++)
		for (int j = 1; j <= WIDTH; j++) cells[i][j] = 0;
}

void drawCells(RenderWindow &window)
{
	for (int i = 1; i <= HEIGHT; i++)
		for (int j = 1; j <= WIDTH; j++)
			if (cells[i][j] == LIVE)
			{
				cell.setPosition((j - 1)*SIZE, (i - 1)*SIZE); cell.setFillColor(cl_live);
				window.draw(cell);
			}
			else if (cells[i][j] == DYING)
			{
				cell.setPosition((j - 1)*SIZE, (i - 1)*SIZE); cell.setFillColor(cl_dying);
				window.draw(cell);
			}
}

int di[8] = { -1,-1,0,1,1,1,0,-1 }, dj[8] = { 0,1,1,1,0,-1,-1,-1 };

inline bool inside(int i, int j) { return(i > 0 && i <= HEIGHT && j > 0 && j <= WIDTH); }

void updateCells()
{
	int n, in, jn;
	for (int i = 1; i <= HEIGHT; i++)
		for (int j = 1; j <= WIDTH; j++)
			if (cells[i][j] == DEAD)
			{
				n = 0;
				for (int d = 0; d < 8; d++)
				{
					in = i + di[d]; jn = j + dj[d];
					if (inside(in, jn) && cells[in][jn] == LIVE) n++;
					if (n == 3) break;
				}
				if (n == 2) cells[i][j] = WILL_LIVE;
			}
}

void updateGeneration()
{
	for (int i = 1; i <= HEIGHT; i++)
		for (int j = 1; j <= WIDTH; j++)
			if (cells[i][j] == LIVE) cells[i][j] = DYING;
			else if (cells[i][j] == DYING) cells[i][j] = DEAD;
			else if (cells[i][j] == WILL_LIVE) cells[i][j] = LIVE;
}

int main()
{
	// initializari
	Texture txt_grid;
	if (!txt_grid.loadFromFile("resurse/bg_tiles.png"));
	Sprite spr_grid(txt_grid);

	// fereastra
	RenderWindow window(VideoMode(SIZE*WIDTH, SIZE*HEIGHT), "Brian's Brain");
	window.setFramerateLimit(20);

	while (window.isOpen())
	{
		// evenimente
		Event e;
		while (window.pollEvent(e))
			if (e.type == Event::Closed) window.close();
			else if (e.type == Event::MouseButtonPressed)
			{
				if (e.mouseButton.button == Mouse::Left)
				{
					int mi = e.mouseButton.y / SIZE + 1, mj = e.mouseButton.x / SIZE + 1;
					cells[mi][mj] = (cells[mi][mj] + 1) % 2;
				}
				else if (e.mouseButton.button == Mouse::Right)
				{
					int mi = e.mouseButton.y / SIZE + 1, mj = e.mouseButton.x / SIZE + 1;
					cells[mi][mj] = (cells[mi][mj] == 2) ? 0 : 2;
				}
			}

		// logica
		window.clear(Color::White);
		if (Keyboard::isKeyPressed(Keyboard::C)) clearCells();
		if (Keyboard::isKeyPressed(Keyboard::R)) randomCells();
		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			updateCells();
			updateGeneration();
		}

		// afisare
		window.draw(spr_grid);
		drawCells(window);

		window.display();
	}

	return 0;
}