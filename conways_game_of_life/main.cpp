#include <SFML/Graphics.hpp>
#include <cstdlib> // rand()
#include <ctime> // time()
#include <iostream> // input
#define S 16.f
#define MAX 500
using namespace std;
using namespace sf;

int WIDTH, HEIGHT, SPEED; // marimea ferestrei + fps
int cells[MAX][MAX]; // vectorul de celule
int di[8]={-1,-1,0,1,1,1,0,-1}, dj[8]={0,1,1,1,0,-1,-1,-1}; // vectorul de directii N,NE,E,SE,S,SV,V,NV
enum state{DEAD=0, LIVING=1, WILL_DIE=2, WILL_BE_BORN=3}; // starile celulelor
RectangleShape cell(Vector2f(S,S)); // celula in forma de patrat

void windowSetup()
{
//    cout<<"WIDTH: "; cin>>WIDTH;
//    cout<<"HEIGHT: "; cin>>HEIGHT;
//    cout<<"SPEED: "; cin>>SPEED;
    WIDTH=60; HEIGHT=50; SPEED=10;
}

void cellSetup()
{
    srand(time(NULL)); // initializez seed-ul
    for(int i=1;i<=HEIGHT;i++)
        for(int j=1;j<=WIDTH;j++) cells[i][j]=rand()%2; // 1=living; 0=dead; aleatoriu
//    cells[10][20]=1;
//    cells[10][21]=1;
//    cells[11][21]=1;
//    cells[9][21]=1;
//    cells[9][22]=1;
}

bool inside(int i, int j)
{
    return (i>0&&i<=HEIGHT&&j>0&&j<=WIDTH); // celula este inauntrul matricei
}

void updateCells()
{
    int n, in, jn;
    for(int i=1;i<=HEIGHT;i++)
        for(int j=1;j<=WIDTH;j++)
        {
            // numaram vecinii celulei actuale
            n=0;
            for(int d=0;d<8;d++)
            {
                in=i+di[d]; jn=j+dj[d]; // coordonatele celulei vecine
                if(inside(in,jn) && (cells[in][jn]==LIVING || cells[in][jn]==WILL_DIE)) n++; // celula vecina VIE momentan
            }
            if(cells[i][j]==LIVING) cells[i][j] = (n==2||n==3)? LIVING:WILL_DIE; // are 2 sau 3 vecini -> traieste; altfel va muri
            else cells[i][j] = (n==3)? WILL_BE_BORN:DEAD; // are 3 vecini -> va renaste; altfel ramane moarta
        }
}

void updateGeneration()
{
    for(int i=1;i<=HEIGHT;i++)
        for(int j=1;j<=WIDTH;j++)
            if(cells[i][j]==WILL_BE_BORN) cells[i][j]=LIVING;
            else if(cells[i][j]==WILL_DIE) cells[i][j]=DEAD;
}

void drawCells(RenderWindow &window)
{
    for(int i=1;i<=HEIGHT;i++)
        for(int j=1;j<=WIDTH;j++)
            if(cells[i][j]==LIVING||cells[i][j]==WILL_DIE) // celule vii MOMENTAN
            {
                cell.setPosition((j-1)*S,(i-1)*S); // pozitia actuala este (x,y); x=coloana*marimea celulei; y=linia*marimea celulei
                window.draw(cell);
            }
}

int main()
{
    // initializari
    windowSetup(); // fereastra
    cellSetup(); // celule de inceput
    cell.setFillColor(Color::Black); // culoare celula

    // fereastra
    RenderWindow window(VideoMode(S*WIDTH,S*HEIGHT),"Conway's Game of Life");
    window.setFramerateLimit(SPEED);
    while(window.isOpen())
    {
        // evenimente
        Event e;
        while(window.pollEvent(e))
            if(e.type==Event::Closed) window.close();

        // logica
        window.clear(Color::White);
        if(Keyboard::isKeyPressed(Keyboard::Space))
        {
            updateGeneration(); // actualizez generatia
            updateCells(); // actualizam starile celulelor
        }
        if(Keyboard::isKeyPressed(Keyboard::R)) cellSetup(); // resetez jocul

        // afisare
        drawCells(window);

        window.display();
    }

    return 0;
}
