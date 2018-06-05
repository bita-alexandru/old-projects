#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#define WIDTH 20
#define HEIGHT 16
#define SIZE 32.f
using namespace sf;
using namespace std;

int n_tail = 2, dir = 2, head_i, head_j, f_i, f_j; // lungime coada, directie, coordonate cap, coordonate fruct
bool fruit_on = false; // e fructul pe ecran?
RectangleShape piece(Vector2f(SIZE,SIZE)); // patratul afisat pe ecran (galben sau rosu)
vector< pair<int,int> > v_tail; // vector ce contine coordonatele fiecarui patrat galben (coada)

void fruit()
{
	bool ok;
	do
	{
		ok = true;
		f_i = rand() % HEIGHT;
		f_j = rand() % WIDTH;
		for (size_t i = 0; i < n_tail; i++)
			if (f_i == v_tail[i].first&&f_j == v_tail[i].second) // se spawneaza intr-o bucata din coada
			{
				ok = false; // atunci nu-i ok
				break;
			}
	} while (!ok);
	fruit_on = true; // fructul s-a spawnat
}

int main()
{
	/* initializare */
	head_i = HEIGHT / 2; head_j = WIDTH / 2;
	v_tail.push_back(make_pair(head_i, head_j)); v_tail.push_back(make_pair(head_i, head_j - 1)); // primele 2 bucati din sarpe
	srand(time(NULL)); // initializez seedul
	fruit(); // spawnez un fruct

	/* audio */
	SoundBuffer sb_eat, sb_ouch;
	if (!sb_eat.loadFromFile("resurse/eat.wav"));
	if (!sb_ouch.loadFromFile("resurse/ouch.wav"));
	Sound s_eat(sb_eat), s_ouch(sb_ouch);

	/* window */
	RenderWindow window(VideoMode(SIZE*WIDTH, SIZE*HEIGHT), "Sneic");
	window.setFramerateLimit(10);

	while (window.isOpen())
	{
		Event e;
		while (window.pollEvent(e))
			if (e.type == Event::Closed) window.close();

		/* logica */
		if (Keyboard::isKeyPressed(Keyboard::Up) && dir != 3) dir = 1;
		if (Keyboard::isKeyPressed(Keyboard::Right) && dir != 4) dir = 2;
		if (Keyboard::isKeyPressed(Keyboard::Down) && dir != 1) dir = 3;
		if (Keyboard::isKeyPressed(Keyboard::Left) && dir != 2) dir = 4;

		if (!fruit_on) fruit(); // nu e niciun fruct pe ecran -> spawnez unul

		for (size_t i = n_tail - 1; i > 0; i--) // actualizez bucatile sarpelui
		{
			v_tail[i].first = v_tail[i - 1].first; 
			v_tail[i].second = v_tail[i - 1].second;
			// ultima bucata devine bucata urmatoare si tot asa
		}
		switch (dir) // actualizez pozitia capului in functie de directie
		{
		case 1: head_i--; break; // sus
		case 2: head_j++; break; // dreapta
		case 3: head_i++; break; // jos
		case 4: head_j--; break; // stanga
		}
		if (head_i >= HEIGHT) head_i = 0; // trec de sus in jus
		if (head_i < 0) head_i = HEIGHT; // de jos in sus
		if (head_j >= WIDTH) head_j = 0; // din dreapta in stanga
		if (head_j < 0) head_j = WIDTH; // din stanga in dreapta

		v_tail[0].first = head_i; v_tail[0].second = head_j; // salvez pozitia capului

		if (head_i == f_i && head_j == f_j) // am luat fructul
		{
			s_eat.play(); // sunet
			n_tail++; // se mareste coada
			v_tail.push_back(make_pair(1, 1)); // mai adaug o bucata
			for (size_t i = n_tail - 1; i > 0; i--) // actualizez coada
			{
				v_tail[i].first = v_tail[i - 1].first;
				v_tail[i].second = v_tail[i - 1].second;
				// ultima bucata devine bucata urmatoare si tot asa
			}
			// noua bucata o plasez la inceput
			if (dir == 1) head_i--; // daca directia e catre dreapta, asez bucata in dreapta celei anterioare (care anterior fusese capul)
			else if (dir == 2) head_j++; // etc
			else if (dir == 3) head_i++; 
			else head_j--;
			v_tail[0].first = head_i; v_tail[0].second = head_j; // salvez pozitia capului in vector
			fruit_on = false; // am mancat fructul deci nu se mai afiseaza
		}
		for (size_t i = 1; i < n_tail; i++)
			if (head_i == v_tail[i].first&&head_j == v_tail[i].second) // capul atinge coada deci sarpele se musca singur
			{
				s_ouch.play(); // sunet
				n_tail = 2; // revenim la 2 bucati
				v_tail.resize(n_tail); // reactualizam marimea vectorului
			}
		window.clear();

		// draw
		for (size_t i = 0; i < n_tail; i++) // afisare bucati sarpe
		{
			piece.setFillColor(Color::Yellow); piece.setOutlineThickness(0.f); piece.setOutlineColor(Color::Black);
			piece.setPosition(v_tail[i].second*SIZE, v_tail[i].first*SIZE);
			window.draw(piece);
		}
		if (fruit_on) // afisare fruct
		{
			piece.setFillColor(Color::Red); piece.setOutlineThickness(0.f);
			piece.setPosition(f_j*SIZE, f_i*SIZE);
		}
		window.draw(piece);

		window.display();
	}
	return 0;
}
