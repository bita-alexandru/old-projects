#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
using namespace sf;
using namespace std;

RenderWindow window(VideoMode(480, 640), "fractals everywhere");
CircleShape point(1);
int chance;
float x, y, w, h, nx, ny, px, py, minx, maxx, miny, maxy;

float _map(float value, float start1, float stop1, float start2, float stop2)
{
	return (value - start1) / (stop1 - start1) * (stop2 - start2) + start2;
}

void BarnsleysFern()
{
	float a[4][2] = { { +0.00f, +0.00f },{ +0.85f, -0.04f },{ +0.20f, +0.23f },{ -0.15f, +0.26f } };
	float b[4][2] = { { +0.00f, +0.16f },{ +0.04f, +0.85f },{ -0.26f, +0.22f },{ +0.28f, +0.24f } };
	float c[4][2] = { { +0.00f, +0.00f },{ +0.00f, +1.60f },{ +0.00f, +1.60f },{ +0.00f, +0.44f } };
	chance = rand() % 101;

	if (chance < 1)
	{
		nx = a[0][0] * x + b[0][0] * y + c[0][0];
		ny = a[0][1] * x + b[0][1] * y + c[0][1];
		point.setFillColor(Color::Green);
	}
	else if (chance < 86)
	{
		nx = a[1][0] * x + b[1][0] * y + c[1][0];
		ny = a[1][1] * x + b[1][1] * y + c[1][1];
		point.setFillColor(Color::Red);
	}
	else if (chance < 93)
	{
		nx = a[2][0] * x + b[2][0] * y + c[2][0];
		ny = a[2][1] * x + b[2][1] * y + c[2][1];
		point.setFillColor(Color::Blue);
	}
	else
	{
		nx = a[3][0] * x + b[3][0] * y + c[3][0];
		ny = a[3][1] * x + b[3][1] * y + c[3][1];
		point.setFillColor(Color::Magenta);
	}
}

void SierpinskisTriangle()
{
	float a[3][2] = { { +0.50f, +0.00f },{ +0.50f, +0.00f },{ +0.50f, +0.00f } };
	float b[3][2] = { { +0.00f, +0.50f },{ +0.00f, +0.50f },{ +0.00f, +0.50f } };
	float c[3][2] = { { +0.00f, +0.00f },{ +0.50f, +0.00f },{ +0.25f, +0.43f } };
	chance = rand() % 3;

	switch (chance)
	{
	case 0:
		nx = a[0][0] * x + b[0][0] * y + c[0][0];
		ny = a[0][1] * x + b[0][1] * y + c[0][1];
		point.setFillColor(Color::Blue);
		break;
	case 1:
		nx = a[1][0] * x + b[1][0] * y + c[1][0];
		ny = a[1][1] * x + b[1][1] * y + c[1][1];
		point.setFillColor(Color::Red);
		break;
	case 2:
		nx = a[2][0] * x + b[2][0] * y + c[2][0];
		ny = a[2][1] * x + b[2][1] * y + c[2][1];
		point.setFillColor(Color::Yellow);
		break;
	}
}

void SierpinskisPentagon()
{
	chance = rand() % 5;

	switch (chance)
	{
	case 0:
		nx = 0.382f * x;
		ny = 0.382f * y;
		point.setFillColor(Color::Yellow);
		break;
	case 1:
		nx = 0.382f * x + 0.618f;
		ny = 0.382f * y;
		point.setFillColor(Color::Cyan);
		break;
	case 2:
		nx = 0.382f * x + 0.809f;
		ny = 0.382f * y + 0.588f;
		point.setFillColor(Color::Green);
		break;
	case 3:
		nx = 0.382f * x + 0.309f;
		ny = 0.382f * y + 0.951f;
		point.setFillColor(Color::Red);
		break;
	case 4:
		nx = 0.382f * x - 0.191f;
		ny = 0.382f * y + 0.588f;
		point.setFillColor(Color::Magenta);
		break;
	}
}

void SierpinskisCarpet()
{
	chance = rand() % 8;

	switch (chance)
	{
	case 0:
		nx = 0.333f * x;
		ny = 0.333f * y;
		point.setFillColor(Color::Magenta);
		break;
	case 1:
		nx = 0.333f * x;
		ny = 0.333f * y + 0.333f;
		point.setFillColor(Color::Yellow);
		break;
	case 2:
		nx = 0.333f * x;
		ny = 0.333f * y + 0.666f;
		point.setFillColor(Color::Cyan);
		break;
	case 3:
		nx = 0.333f * x + 0.333f;
		ny = 0.333f * y;
		point.setFillColor(Color::Green);
		break;
	case 4:
		nx = 0.333f * x + 0.333f;
		ny = 0.333f * y + 0.666f;
		point.setFillColor(Color::Red);
		break;
	case 5:
		nx = 0.333f * x + 0.666f;
		ny = 0.333f * y;
		point.setFillColor(Color::Blue);
		break;
	case 6:
	{
		nx = 0.333f * x + 0.666f;
		ny = 0.333f * y + 0.333f;
		Color color1(184, 68, 37);
		point.setFillColor(color1);
		break;
	}
	case 7:
	{
		nx = 0.333f * x + 0.666f;
		ny = 0.333f * y + 0.666f;
		Color color2(41, 203, 113);
		point.setFillColor(color2);
		break;
	}
	}
}

void SymmetricBinaryTrees()
{
	chance = rand() % 4;

	switch (chance)
	{
	case 0:
		nx = 0.01f * x;
		ny = 0.45f * y;
		point.setFillColor(Color::Yellow);
		break;
	case 1:
		nx = -0.01f * x;
		ny = -0.45f * y + 0.4f;
		point.setFillColor(Color::Magenta);
		break;
	case 2:
		nx = 0.42f * x - 0.42f * y;
		ny = 0.42f * x + 0.42f * y + 0.4f;
		point.setFillColor(Color::White);
		break;
	case 3:
		nx = 0.42f * x + 0.42f * y;
		ny = -0.42f * x + 0.42f * y + 0.4f;
		point.setFillColor(Color::Red);
		break;
	}
}

void getRange()
{
	if (nx < minx) minx = nx;
	if (nx > maxx) maxx = nx;
	if (ny < miny) miny = ny;
	if (ny > maxy) maxy = ny;

	cout << "x: \t" << minx << ' ' << maxx << '\n';
	cout << "y: \t" << miny << ' ' << maxy << '\n';
}

void nextPoint()
{
	//getRange();

	x = nx; y = ny;
}

void drawPoint()
{
	point.setPosition(px, py);

	window.draw(point);
}

bool active1 = false, active2 = false, active3 = false, active4 = false, active5 = false;

int main()
{
	srand(time(NULL));

	w = window.getSize().x * 1.f;
	h = window.getSize().y * 1.f;

	x = 0.f; y = 0.f;

	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		Event e;

		while (window.pollEvent(e))
			if (e.type == Event::Closed) window.close();
			else if (e.type == Event::KeyPressed)
			{
				switch (e.key.code)
				{
				case Keyboard::Num0: active1 = false; active2 = false; active3 = false; active4 = false; active5 = false; x = 0; y = 0; window.clear(); break;
				case Keyboard::Num1: active1 = true;  active2 = false; active3 = false; active4 = false; active5 = false; x = 0; y = 0; window.clear(); break;
				case Keyboard::Num2: active2 = true;  active1 = false; active3 = false; active4 = false; active5 = false; x = 0; y = 0; window.clear(); break;
				case Keyboard::Num3: active3 = true;  active1 = false; active2 = false; active4 = false; active5 = false; x = 0; y = 0; window.clear(); break;
				case Keyboard::Num4: active4 = true;  active1 = false; active2 = false; active3 = false; active5 = false; x = 0; y = 0; window.clear(); break;
				case Keyboard::Num5: active5 = true;  active1 = false; active2 = false; active3 = false; active4 = false; x = 0; y = 0; window.clear(); break;
				}
			}
		if (active1 || active2 || active3 || active4 || active5)
			for (int i = 1; i <= 250; i++)
			{
				if (active1) px = _map(x, -2.1820f, 2.6558f, 0, w), py = _map(y, 0.f, 9.9983f, h, 0); // barnsley's fern
				if (active2) px = _map(x, -0.308f, 1.308f, 0, w), py = _map(y, 0.f, 1.537f, h, 0); // sierpinski's pentagon
				if (active3) px = _map(x, 0.f, 0.998f, 0, w), py = _map(y, 0.f, 0.893f, h, 0); // sierpinski's triangle
				if (active4) px = _map(x, 0.f, 0.998f, 0, w), py = _map(y, 0.f, 0.998f, h, 0); // sierpinski's carpet
				if (active5) px = _map(x, -0.474, 0.472f, 0, w), py = _map(y, 0.f, 0.873f, h, 0); // symmetric binary trees
				drawPoint();

				if (active1) BarnsleysFern();
				if (active2) SierpinskisPentagon();
				if (active3) SierpinskisTriangle();
				if (active4) SierpinskisCarpet();
				if (active5) SymmetricBinaryTrees();
				nextPoint();
			}

		window.display();
	}
	return 0;
}
