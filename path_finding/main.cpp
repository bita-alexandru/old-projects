#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#define SIZE 32
#define COLUMNS 54
#define LINES 30
#define INF 1000000000
using namespace sf;
using namespace std;

int grid[LINES+1][COLUMNS+1], path[LINES+1][COLUMNS+1];
int di[4] = {-1, 0, 1, 0};
int dj[4] = {0, 1, 0, -1};
bool foundDFS, foundBFS, foundGreedy, foundAStar;
int minDFS, maxDFS;
vector< pair<int, int> > checkedDFS, trailDFS, bestDFS, worstDFS;
vector< pair<int, int> > bestGreedy, worstGreedy;
RectangleShape box(Vector2f(SIZE, SIZE));
Texture t_cell;
Sprite s_cell;
pair<int, int> start, finish;

void drawBoxes(RenderWindow &window)
{
    box.setFillColor(Color::White); box.setOutlineThickness(1.f); box.setOutlineColor(Color::Black);

    for(int i = 1; i <= LINES; i++)
        for(int j = 1; j <= COLUMNS; j++)
        {
            box.setPosition((j - 1) * SIZE, (i - 1) * SIZE);
            window.draw(box);
        }
}

void drawGrid(RenderWindow &window)
{
    grid[start.first][start.second] = -1; path[start.first][start.second] = 0;
    grid[finish.first][finish.second] = -2; path[finish.first][finish.second] = 0;

    for(int i = 1; i <= LINES; i++)
        for(int j = 1; j <= COLUMNS; j++)
        {
            if(grid[i][j] != 1)
            {
                switch(grid[i][j])
                {
                    case -1: t_cell.loadFromFile("resurse/start.png"); break;
                    case -2: t_cell.loadFromFile("resurse/finish.png"); break;
                    case 2: t_cell.loadFromFile("resurse/2.png"); break;
                    case 3: t_cell.loadFromFile("resurse/3.png"); break;
                    case 4: t_cell.loadFromFile("resurse/4.png"); break;
                    case 5: t_cell.loadFromFile("resurse/5.png"); break;
                    case 6: t_cell.loadFromFile("resurse/6.png"); break;
                    case 7: t_cell.loadFromFile("resurse/7.png"); break;
                    case 8: t_cell.loadFromFile("resurse/8.png"); break;
                    case 9: t_cell.loadFromFile("resurse/9.png"); break;
                }
                s_cell.setTexture(t_cell); s_cell.setPosition((j - 1) * SIZE, (i - 1) * SIZE);
                window.draw(s_cell);
            }

            switch(path[i][j])
            {
                case 0: break;
                case 3: t_cell.loadFromFile("resurse/checked.png"); break;
                case 4: t_cell.loadFromFile("resurse/trail.png"); break;
            }
            if(path[i][j] > 2)
            {
                s_cell.setTexture(t_cell); s_cell.setPosition((j - 1) * SIZE, (i - 1) * SIZE);
                window.draw(s_cell);            }
        }

    if(foundDFS)
    {
        int l, c;

        while(worstDFS.size() > 0)
        {
            l = worstDFS[0].first;
            c = worstDFS[0].second;

            worstDFS.erase(worstDFS.begin());

            path[l][c] = 3;
        }

        while(bestDFS.size())
        {
            l = bestDFS[0].first;
            c = bestDFS[0].second;

            bestDFS.erase(bestDFS.begin());

            path[l][c] = 4;
        }
    }

    if(foundGreedy)
    {
        int l, c;

        while(worstGreedy.size() > 0)
        {
            l = worstGreedy[0].first;
            c = worstGreedy[0].second;

            worstGreedy.erase(worstGreedy.begin());

            path[l][c] = 3;
        }

        while(bestGreedy.size() > 0)
        {
            l = bestGreedy[0].first;
            c = bestGreedy[0].second;

            bestGreedy.erase(bestGreedy.begin());

            path[l][c] = 4;
        }
    }
}

void randomizeGrid()
{
    srand(time(NULL));
    for(int i = 1; i <= LINES; i++)
        for(int j = 1; j <= COLUMNS; j++) grid[i][j] = rand() % (9 - 2) + 2;
}

void clearGrid()
{
    for(int i = 1; i <= LINES; i++)
        for(int j = 1; j <= COLUMNS; j++) grid[i][j] = 1;
}

void clearPath()
{
    for(int i = 1; i <= LINES; i++)
        for(int j = 1; j <= COLUMNS; j++) path[i][j] = 0;
}

bool inside(int l, int c)
{
    return l <= LINES && l > 0 && c <= COLUMNS && c > 0;
}

void pathDFS(int l, int c)
{
    if(maxDFS == minDFS) return;

    if(l == finish.first && c == finish.second)
    {
        foundDFS = true;

        if(maxDFS > minDFS)
        {
            maxDFS = minDFS;
            bestDFS = checkedDFS;
        }
    }
    else
        for(int d = 0; d < 4; d++)
        {
            int nl = l + di[d];
            int nc = c + dj[d];
            if(inside(nl, nc) && path[nl][nc] == 0)
            {
                path[l][c] = 1;
                minDFS++;
                checkedDFS.push_back(make_pair(l, c)); worstDFS.push_back(make_pair(l, c));
                pathDFS(nl, nc);
                checkedDFS.erase(checkedDFS.begin() + minDFS);
                minDFS--;
            }
        }
}

void pathGreedy(int l, int c)
{
    if(l == finish.first && c == finish.second)
    {
        foundGreedy = true;
        return;
    }
    int bl = -1, bc = -1;

    for(int d = 0; d < 4; d++)
    {
        int nl = l + di[d];
        int nc = c + dj[d];

        if(inside(nl, nc) && path[nl][nc] == 0)
        {
            worstGreedy.push_back(make_pair(nl, nc));

            if(bl == -1 && bc == -1) bl = nl, bc = nc;
            if(abs(finish.first - nl) < abs(finish.first - bl) && abs(finish.second - nc) < abs(finish.second - bc)) bl = nl, bc = nc;
        }
    }

    bestGreedy.push_back(make_pair(bl, bc));
    pathGreedy(bl, bc);
}

int main()
{
    RenderWindow window(VideoMode(COLUMNS * SIZE, LINES * SIZE), "Algoritmi de path-finding");
    window.setFramerateLimit(60);

    Texture t_grid; t_grid.loadFromFile("resurse/grid.png");
    Sprite s_grid(t_grid);

    start.first = 16; start.second = 2;
    finish.first = 16; finish. second = 53;
    clearGrid();

    while(window.isOpen())
    {
        Event e;

        while(window.pollEvent(e))
            if(e.type == Event::Closed) window.close();
            else if(e.type == Event::KeyPressed)
            {
                switch(e.key.code)
                {
                    case Keyboard::Escape: window.close(); break;
                    case Keyboard::Z:
                    {
                        clearPath(); foundDFS = false; foundBFS = false; foundGreedy = false; foundAStar = false;
                        int mi = Mouse::getPosition(window).y / SIZE + 1;
                        int mj = Mouse::getPosition(window).x / SIZE + 1;
                        if(!(mi == finish.first && mj == finish.second))
                        {
                            grid[start.first][start.second] = 1;
                            start.first = mi; start.second = mj;
                        }
                        break;
                    }
                    case Keyboard::X:
                    {
                        clearPath(); foundDFS = false; foundBFS = false; foundGreedy = false; foundAStar = false;
                        int mi = Mouse::getPosition(window).y / SIZE + 1;
                        int mj = Mouse::getPosition(window).x / SIZE + 1;
                        if(!(mi == start.first && mj == start.second))
                        {
                            grid[finish.first][finish.second] = 1;
                            finish.first = mi; finish.second = mj;
                        }
                        break;
                    }
                    case Keyboard::R: randomizeGrid(); break;
                    case Keyboard::Num0: clearGrid(); clearPath(); foundDFS = false; foundBFS = false; foundGreedy = false; foundAStar = false; break;
                    case Keyboard::Num1:
                    {
                        foundDFS = false; foundBFS = false; foundGreedy = false; foundAStar = false;
                        clearPath(); checkedDFS.clear(); trailDFS.clear(); worstDFS.clear(); bestDFS.clear(); minDFS=0; maxDFS=INF;
                        pathDFS(start.first, start.second);
                        break;
                    }
                    case Keyboard::Num4:
                    {
                        foundDFS = false; foundBFS = false; foundGreedy = false; foundAStar = false;
                        clearPath(); bestGreedy.clear(); worstGreedy.clear();
                        pathGreedy(start.first, start.second);
                        break;
                    }
                }
            }
            else if(e.type == Event::MouseButtonPressed)
            {
                switch(e.mouseButton.button)
                {
                    case Mouse::Left:
                    {
                        clearPath(); foundDFS = false; foundBFS = false; foundGreedy = false; foundAStar = false;
                        int mi = Mouse::getPosition(window).y / SIZE + 1;
                        int mj = Mouse::getPosition(window).x / SIZE + 1;
                        if(grid[mi][mj] > 0) grid[mi][mj] = min(grid[mi][mj] + 1, 9);
                        break;
                    }
                    case Mouse::Right:
                    {
                        clearPath(); foundDFS = false; foundBFS = false; foundGreedy = false; foundAStar = false;
                        int mi = Mouse::getPosition(window).y / SIZE + 1;
                        int mj = Mouse::getPosition(window).x / SIZE + 1;
                        if(grid[mi][mj] > 0) grid[mi][mj] = max(grid[mi][mj] - 1, 1);
                        break;
                    }
                }
            }

        window.clear(Color::White);

        if(finish.first > start.first) di[0] = 1, di[2] = -1;
        else di[0] = -1, di[2] = 1;
        if(finish.second < start.second) dj[1] = -1, dj[3] = 1;
        else dj[1] = 1, dj[3] = 1;

//        drawBoxes(window);
        window.draw(s_grid);
        drawGrid(window);

        window.display();
    }
    return 0;
}
