#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstring>
using namespace sf;
using namespace std;
RenderWindow window(VideoMode(330,380),"X si 0",Style::Close);
bool hasFocus=true;

// cells
RectangleShape cell;
vector<RectangleShape> cells;

// x/o vector
vector<Sprite> xos;

// used cells
int usedCells[9];

// initialize the game
void initCells()
{
    // reset
    for(size_t i=0;i<cells.size();) cells.erase(cells.begin()+i);
    for(size_t i=0;i<xos.size();) xos.erase(xos.begin()+i);
    for(int i=0;i<9;i++) usedCells[i]=0;

    Vector2f cellPos(60,60);
    for(int i=1;i<=9;i++)
    {
        cell.setSize(Vector2f(100,100));
        cell.setOrigin(50,50);
        cell.setPosition(cellPos);
        cell.setFillColor(Color::White);
        cell.setOutlineColor(Color::Black);
        cell.setOutlineThickness(2.f);
        cells.push_back(cell);
        cellPos.x+=105;
        if(i%3==0)
            cellPos.x=60,
            cellPos.y+=105;
    }
}

// UI
int scoreP1, scoreP2;
void initText(Text &txt1, Text &txt2, Text &txt3, Text &txt4, Font &f1)
{
    txt1.setFont(f1);
    txt1.setCharacterSize(18);
    txt1.setFillColor(Color::Black);
    txt1.setPosition(10,330);
    txt1.setString("Player 1 (X)");

    txt2.setFont(f1);
    txt2.setCharacterSize(18);
    txt2.setFillColor(Color::Black);
    txt2.setPosition(180,330);
    txt2.setString("Player 2 (O)");

    txt3.setFont(f1);
    txt3.setCharacterSize(20);
    txt3.setFillColor(Color::Black);
    txt3.setPosition(10,355);
    txt3.setString(to_string(scoreP1)+" - "+to_string(scoreP2));

    txt4.setFont(f1);
    txt4.setCharacterSize(14);
    txt4.setFillColor(Color::Black);
    txt4.setPosition(210,355);
    txt4.setString("playing...");
}

int playerMove=1;
// winning condition
bool gameWon()
{
    // horizontally
    if(usedCells[0]==usedCells[1]&&usedCells[0]==usedCells[2]&&usedCells[0]!=0) return true;
    if(usedCells[3]==usedCells[4]&&usedCells[3]==usedCells[5]&&usedCells[3]!=0) return true;
    if(usedCells[6]==usedCells[7]&&usedCells[6]==usedCells[8]&&usedCells[6]!=0) return true;

    // vertically
    if(usedCells[0]==usedCells[3]&&usedCells[0]==usedCells[6]&&usedCells[0]!=0) return true;
    if(usedCells[1]==usedCells[4]&&usedCells[1]==usedCells[7]&&usedCells[1]!=0) return true;
    if(usedCells[2]==usedCells[5]&&usedCells[2]==usedCells[8]&&usedCells[2]!=0) return true;

    // diagonally
    if(usedCells[0]==usedCells[4]&&usedCells[0]==usedCells[8]&&usedCells[0]!=0) return true;
    if(usedCells[2]==usedCells[4]&&usedCells[2]==usedCells[6]&&usedCells[2]!=0) return true;

    return false;
}

// draw
bool gameDraw()
{
    for(int i=0;i<9;i++) if(!usedCells[i]) return false;
    return true;
}

int main()
{
    // x/o sprites
    Texture t1, t2;
    if(!t1.loadFromFile("resurse/x.png")) throw "Couldn't load from file.";
    if(!t2.loadFromFile("resurse/0.png")) throw "Couldn't load from file.";
    Sprite s1;

    // text data
    Font f1;
    f1.loadFromFile("resurse/LCD_Solid.ttf");
    Text txtP1, txtP2, txtScore, txtResult;
    initText(txtP1, txtP2, txtScore, txtResult, f1);

    // game state
    enum State{PAUSE, PLAYING, OVER} gameState;

    // init
    gameState=PAUSE;
    initCells();
    window.setFramerateLimit(60);

    while(window.isOpen())
    {
        Event event;
        while(window.pollEvent(event))
            if(event.type==Event::Closed)
                window.close();
            else if(event.type==Event::LostFocus)
                hasFocus=false;
            else if(event.type==Event::GainedFocus)
                hasFocus=true;
        window.clear(Color::White);

        // draw cells
        for(size_t i=0;i<cells.size();i++)
        {
            window.draw(cells[i]);
            if(Mouse::isButtonPressed(Mouse::Left)&&gameState!=OVER) // place xos
            {
                Vector2f mousePos;
                mousePos=Vector2f(Mouse::getPosition(window));
                if(cells[i].getGlobalBounds().contains(mousePos)&&!usedCells[i]&&hasFocus)
                {
                    if(playerMove==1) // place X
                        s1.setTexture(t1),
                        usedCells[i]=1;
                    else // place O
                        s1.setTexture(t2),
                        usedCells[i]=2;
                    s1.setOrigin(40,40);
                    s1.setPosition(cells[i].getPosition());
                    xos.push_back(s1);
                    playerMove++;
                    playerMove=(playerMove>2)? 1:2;
                    gameState=PLAYING;
                }
            }
        }

        // draw xos
        for(size_t i=0;i<xos.size();i++)
            window.draw(xos[i]);

        // check if over
        if(gameWon()&&gameState==PLAYING)
        {
            if(playerMove==1) scoreP2++;
            else scoreP1++;
            txtResult.setString("game over...");
            gameState=OVER;
        }
        else if(gameDraw()&&gameState==PLAYING)
        {
            gameState=OVER;
            txtResult.setString("game over...");
        }

        // draw&update ui
        txtScore.setString(to_string(scoreP1)+" - "+to_string(scoreP2));
        window.draw(txtP1), window.draw(txtP2), window.draw(txtScore), window.draw(txtResult);

        // rematch
        if(gameState==OVER)
            if(Keyboard::isKeyPressed(Keyboard::R))
            {
                playerMove=1;
                txtResult.setString("playing...");
                gameState=PAUSE;
                initCells();
            }

        // display
        window.display();
    }
    return 0;
}
