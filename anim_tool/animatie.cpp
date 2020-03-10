#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

ifstream in("scenariu.txt");

const int WIDTH = 1280;
const int HEIGHT = 720;
const int SPEED = 60;
const int TALK_SPEED = rand()%(40-10)+10;

RenderWindow window(VideoMode(WIDTH,HEIGHT), "animatie", Style::Close);
vector<pair<View,vector<int>>> cadre;

int index_sunet, index_entitate, index_cadru;
unordered_map<string,int> id_sunete, id_entitati, id_cadre;
unordered_map<int,bool> cadre_ascunse, entitati_ascunse;

vector<Sprite> entitati;
vector<Texture*> texturi;
vector<Music*> sunete;

View *cadru = nullptr;
Sprite *entitate = nullptr, *entitate_focusata = nullptr;
Music *sunet = nullptr;

struct animatie
{
    string cmd;

    float x = 0.f, y = 0.f, r = 0.f;
    float v_x = 0.f, v_y = 0.f, v_r = 0.f;
    int z = 0;
};

vector<pair<Sprite*,animatie>> anim_entitati;
vector<pair<View*,animatie>> anim_cadre;
vector<pair<Music*,animatie>> anim_sunete;

Texture fade;
vector<pair<bool,Sprite>> fadeuri;

void curata_memoria()
{
    cadre.clear();
    id_sunete.clear(); id_entitati.clear(); id_cadre.clear();
    cadre_ascunse.clear(); entitati_ascunse.clear();

    entitati.clear();
    sunete.clear();

    anim_entitati.clear(); anim_cadre.clear(); anim_sunete.clear();
    fadeuri.clear();

    for(size_t i = 0; i < texturi.size(); i++) delete texturi[i];
    texturi.clear();

    for(size_t i = 0; i < sunete.size(); i++) delete sunete[i];
    sunete.clear();
}

void evenimente()
{
    Event e;

    while(window.pollEvent(e))
    {
        if(e.type == Event::Closed)
        {
            window.close();
            curata_memoria();
        }
    }
}

void animeaza()
{
    for(int i = 0; i < anim_entitati.size(); i++)
    {
        if(anim_entitati[i].second.cmd == "TALK")
        {
            anim_entitati[i].first->move(0,anim_entitati[i].second.v_x);
            anim_entitati[i].second.x -= (anim_entitati[i].second.v_x);

            if(anim_entitati[i].second.x <= 0.f)
            {
                anim_entitati[i].second.v_x *= -1.f;
                anim_entitati[i].second.r--;
            }
            else if(anim_entitati[i].second.x >= anim_entitati[i].second.y)
            {
                anim_entitati[i].second.x = 1.f * TALK_SPEED;
                anim_entitati[i].second.v_x = anim_entitati[i].second.x / (anim_entitati[i].second.v_r*SPEED);
                anim_entitati[i].second.y = anim_entitati[i].second.x;
                anim_entitati[i].second.r--;
            }

            if(anim_entitati[i].second.r == 0.f)
            {
                anim_entitati.erase(anim_entitati.begin() + i);
                i--;
            }
            continue;
        }
        else if(anim_entitati[i].second.cmd == "SLIDE")
        {
            anim_entitati[i].first->move(anim_entitati[i].second.v_x, anim_entitati[i].second.v_y);
            anim_entitati[i].second.x -= anim_entitati[i].second.v_x;
            anim_entitati[i].second.y -= anim_entitati[i].second.v_y;
            anim_entitati[i].second.z--;

            if(anim_entitati[i].second.z == 0)
            {
                anim_entitati.erase(anim_entitati.begin() + i);
                i--;
            }
            continue;
        }
        if(anim_entitati[i].second.cmd == "SCALE")
        {
            anim_entitati[i].first->setScale(anim_entitati[i].first->getScale().x + anim_entitati[i].second.v_x,
                                             anim_entitati[i].first->getScale().y + anim_entitati[i].second.v_y);
            anim_entitati[i].second.x -= anim_entitati[i].second.v_x;
            anim_entitati[i].second.y -= anim_entitati[i].second.v_y;
            anim_entitati[i].second.z--;

            if(anim_entitati[i].second.z == 0)
            {
                anim_entitati.erase(anim_entitati.begin() + i);
                i--;
            }
            continue;
        }
        if(anim_entitati[i].second.cmd == "ROTATE")
        {
            anim_entitati[i].first->rotate(anim_entitati[i].second.v_r);
            anim_entitati[i].second.r -= anim_entitati[i].second.v_r;
            anim_entitati[i].second.z--;

            if(anim_entitati[i].second.z == 0)
            {
                anim_entitati.erase(anim_entitati.begin() + i);
                i--;
            }
            continue;
        }
        if(anim_entitati[i].second.cmd == "FADEIN")
        {
            Color c = anim_entitati[i].first->getColor();
            anim_entitati[i].second.x += anim_entitati[i].second.v_r;

            if(anim_entitati[i].second.x >= 1.f)
            {
                c.a += 1;
                anim_entitati[i].second.y += anim_entitati[i].second.x - 1.f;
                anim_entitati[i].second.x = 0.f;
                if(anim_entitati[i].second.y >= 1.f && c.a != 255)
                {
                    c.a += 1;
                    anim_entitati[i].second.x =  anim_entitati[i].second.y  - 1.f;
                    anim_entitati[i].second.y = 0.f;
                }

                anim_entitati[i].first->setColor(c);
            }

            anim_entitati[i].second.r += anim_entitati[i].second.v_r;

            if(c.a == 255)
            {
                anim_entitati.erase(anim_entitati.begin() + i);
                i--;
            }
            continue;
        }
        if(anim_entitati[i].second.cmd == "FADEOUT")
        {
            Color c = anim_entitati[i].first->getColor();
            anim_entitati[i].second.x += anim_entitati[i].second.v_r;

            if(anim_entitati[i].second.x >= 1.f)
            {
                c.a -= 1;
                anim_entitati[i].second.y += anim_entitati[i].second.x - 1.f;
                anim_entitati[i].second.x = 0.f;
                if(anim_entitati[i].second.y >= 1.f && c.a != 0)
                {
                    c.a -= 1;
                    anim_entitati[i].second.x =  anim_entitati[i].second.y  - 1.f;
                    anim_entitati[i].second.y = 0.f;
                }

                anim_entitati[i].first->setColor(c);
            }

            anim_entitati[i].second.r -= anim_entitati[i].second.v_r;

            if(c.a == 0)
            {
                anim_entitati.erase(anim_entitati.begin() + i);
                i--;
            }
            continue;
        }
    }

    for(int i = 0; i < anim_cadre.size(); i++)
    {
        if(anim_cadre[i].second.cmd == "SLIDE")
        {
            anim_cadre[i].first->move(anim_cadre[i].second.v_x, anim_cadre[i].second.v_y);
            anim_cadre[i].second.x -= anim_cadre[i].second.v_x;
            anim_cadre[i].second.y -= anim_cadre[i].second.v_y;
            anim_entitati[i].second.z--;

            if(anim_entitati[i].second.z == 0)
            {
                anim_cadre.erase(anim_cadre.begin() + i);
                i--;
            }
            continue;
        }
        if(anim_cadre[i].second.cmd == "ROTATE")
        {
            anim_cadre[i].first->rotate(anim_cadre[i].second.v_r);
            anim_cadre[i].second.r -= anim_cadre[i].second.v_r;
            anim_entitati[i].second.z--;

            if(anim_entitati[i].second.z == 0)
            {
                anim_cadre.erase(anim_cadre.begin() + i);
                i--;
            }
            continue;
        }
        if(anim_cadre[i].second.cmd == "FADEIN")
        {
            entitate = &fadeuri[anim_cadre[i].second.z].second;

            Color c = entitate->getColor();
            anim_cadre[i].second.x += anim_cadre[i].second.v_r;

            if(anim_cadre[i].second.x >= 1.f)
            {
                c.a -= 1;
                anim_cadre[i].second.y += anim_cadre[i].second.x - 1.f;
                anim_cadre[i].second.x = 0.f;
                if(anim_cadre[i].second.y >= 1.f && c.a != 0)
                {
                    c.a -= 1;
                    anim_cadre[i].second.x =  anim_cadre[i].second.y  - 1.f;
                    anim_cadre[i].second.y = 0.f;
                }

                entitate->setColor(c);
            }

            anim_cadre[i].second.r -= anim_cadre[i].second.v_r;

            if(c.a == 0)
            {
                fadeuri[anim_cadre[i].second.z].first = false;
                anim_cadre.erase(anim_cadre.begin() + i);
                i--;
            }
            continue;
        }
        if(anim_cadre[i].second.cmd == "FADEOUT")
        {
            entitate = &fadeuri[anim_cadre[i].second.z].second;

            Color c = entitate->getColor();
            anim_cadre[i].second.x += anim_cadre[i].second.v_r;

            if(anim_cadre[i].second.x >= 1.f)
            {
                c.a += 1;
                anim_cadre[i].second.y += anim_cadre[i].second.x - 1.f;
                anim_cadre[i].second.x = 0.f;
                if(anim_cadre[i].second.y >= 1.f && c.a != 255)
                {
                    c.a += 1;
                    anim_cadre[i].second.x =  anim_cadre[i].second.y  - 1.f;
                    anim_cadre[i].second.y = 0.f;
                }

                entitate->setColor(c);
            }

            anim_cadre[i].second.r += anim_cadre[i].second.v_r;

            if(c.a == 255)
            {
                anim_cadre.erase(anim_cadre.begin() + i);
                i--;
            }
            continue;
        }
        if(anim_cadre[i].second.cmd == "ZOOM")
        {
            anim_cadre[i].first->zoom(1.f/anim_cadre[i].second.r);
            anim_cadre[i].second.r -= anim_cadre[i].second.v_r;
            anim_cadre[i].first->zoom(anim_cadre[i].second.r);
            anim_cadre[i].second.z--;

            anim_cadre[i].second.x -= anim_cadre[i].second.v_x;
            anim_cadre[i].second.y -= anim_cadre[i].second.v_y;
            anim_cadre[i].first->move(anim_cadre[i].second.v_x,anim_cadre[i].second.v_y);

            if(anim_cadre[i].second.z == 0)
            {
                anim_cadre.erase(anim_cadre.begin() + i);
                i--;
            }
            continue;
        }
    }

    for(int i = 0; i < anim_sunete.size(); i++)
    {
        if(anim_sunete[i].second.cmd == "FADEIN")
        {
            anim_sunete[i].first->setVolume(anim_sunete[i].second.x);
            anim_sunete[i].second.x += anim_sunete[i].second.v_r;
            anim_sunete[i].second.z--;

            if(anim_sunete[i].second.z == 0)
            {
                anim_sunete.erase(anim_sunete.begin() + i);
                i--;
            }
        }
        else if(anim_sunete[i].second.cmd == "FADEOUT")
        {
            anim_sunete[i].first->setVolume(anim_sunete[i].second.x);
            anim_sunete[i].second.x -= anim_sunete[i].second.v_r;
            anim_sunete[i].second.z--;

            if(anim_sunete[i].second.z == 0)
            {
                anim_sunete.erase(anim_sunete.begin() + i);
                i--;
            }
            continue;
        }
    }
}

void afiseaza()
{
    window.clear();

    for(size_t i = 0; i < cadre.size(); i++)
        if(cadre_ascunse[i] == false)
        {
            window.setView(cadre[i].first);

            for(size_t j = 0; j < cadre[i].second.size(); j++)
                if(entitati_ascunse[cadre[i].second[j]] == false)
                    window.draw(entitati[cadre[i].second[j]]);

            if(fadeuri.size() && fadeuri[i].first) window.draw(fadeuri[i].second);
        }

    window.display();
}

void UPDATE()
{
    evenimente();
    animeaza();
    afiseaza();
}

Vector2f centru_entitate(Sprite*& e)
{
    return (static_cast<Vector2f>(e->getTexture()->getSize()) / 2.f);
}

void AUDIO(stringstream& ss)
{
    evenimente();

    bool lant;

    do
    {
        lant = false;
        if(ss.eof()) throw "lipsesc argumente [ajutor AUDIO]";

        string id;
        ss >> id;
        if(id_sunete.find(id) != id_sunete.end()) throw "id deja existent";
        if(ss.eof()) throw "lipsesc argumente [ajutor AUDIO]";

        string fisier;
        ss >> fisier;

        Music *m = new Music;
        if(!m->openFromFile(fisier)) throw "fisierul specificat nu se gaseste";

        id_sunete[id] = index_sunet++;

        sunete.push_back(m);
        if(!ss.eof())
        {
            string s;
            ss >> s;

            if(s == "&") lant = true;
            else throw "prea multe argumente [ajutor AUDIO]";
        }
    }while(lant);
}

void SOUND(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor SOUND]";

    string id, optiune;
    ss >> id;
    if(id_sunete.find(id) == id_sunete.end()) throw "sunet nonexistent";
    if(ss.eof()) throw "lipsesc argumente [ajutor SOUND]";

    sunet = sunete[id_sunete[id]];

    bool lant;

    do
    {
        lant = false;
        ss >> optiune;

        if(optiune == "PAUSE")
        {
            sunet->pause();
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SOUND]";
            }
        }
        else if(optiune == "STOP")
        {
            sunet->stop();
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SOUND]";
            }
        }
        else if(optiune == "RESUME")
        {
            sunet->play();
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SOUND]";
            }
        }
        else if(optiune == "REPLAY")
        {
            sunet->stop();
            sunet->play();
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SOUND]";
            }
        }
        else if(optiune == "LOOP")
        {
            sunet->setLoop(!sunet->getLoop());
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SOUND]";
            }
        }
        else if(optiune == "PLAY")
        {
            sunet->play();
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SOUND]";
            }
        }
        else if(optiune == "VOLUME")
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor SOUND]";

            float volum;
            ss >> volum;
            sunet->setVolume(volum);
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SOUND]";
            }
        }
        else throw "argument invalid [ajutor SOUND]";
    }while(lant);
}

void VIEW(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor VIEW]";

    string id, optiune;
    ss >> id;
    if(ss.eof()) throw "lipsesc argumente [ajutor VIEW]";

    bool lant;

    do
    {
        lant = false;
        ss >> optiune;

        if(optiune == "NEW")
        {
            if(id_cadre.find(id) != id_cadre.end()) throw "id deja existent";

            id_cadre[id] = index_cadru++;
            cadre_ascunse[index_cadru-1] = false;

            View cadru = window.getDefaultView();
            vector<int> v;

            cadre.push_back({cadru,v});
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor NEW]";
            }
        }
        else if(optiune == "HIDE")
        {
            if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";

            cadre_ascunse[index_cadru-1] = true;
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor HIDE]";
            }
        }
        else if(optiune == "SHOW")
        {
            if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";

            cadre_ascunse[index_cadru-1] = false;
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SHOW]";
            }
        }
        else if(optiune == "ADD")
        {
            if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";

            string id_entitate;

            while(ss >> id_entitate)
            {
                if(id_entitati.find(id_entitate) == id_entitati.end()) throw "entitate nonexistenta";

                int index_c = id_cadre[id];
                int index_e = id_entitati[id_entitate];

                if(find(cadre[index_c].second.begin(), cadre[index_c].second.end(), index_e) != cadre[index_c].second.end())
                    throw "entitatea se afla deja in cadru";

                cadre[index_c].second.push_back(index_e);

                if(ss.eof()) break;
            }
        }
        else if(optiune == "REMOVE")
        {
            if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";

            string id_entitate;

            while(ss >> id_entitate)
            {
                if(id_entitate == "ALL")
                {
                    cadre[id_cadre[id]].second.clear();
                    break;
                }
                else if(id_entitate == "&") lant = true;

                if(id_entitati.find(id_entitate) == id_entitati.end()) throw "entitate nonexistenta";

                int index_c = id_cadre[id];
                int index_e = id_entitati[id_entitate];

                auto it = find(cadre[index_c].second.begin(), cadre[index_c].second.end(), index_e);
                if(it != cadre[index_c].second.end()) cadre[index_c].second.erase(it);

                if(ss.eof()) break;
            }
        }
        else if(optiune == "SWAP")
        {
            if(id == "-")
            {
                string id1, id2;

                while(ss >> id1)
                {
                    if(id1 == "&")
                    {
                        lant = true;
                        break;
                    }
                    if(ss.eof()) throw "lipsesc argumente [ajutor SWAP]";

                    ss >> id2;

                    if(id_cadre.find(id1) != id_cadre.end() && id_cadre.find(id2) != id_cadre.end())
                    {
                        int index_c1 = id_cadre[id1];
                        int index_c2 = id_cadre[id2];

                        swap(cadre[index_c1], cadre[index_c2]);
                    }
                    else throw "cadre nonexistente";

                    if(ss.eof()) break;
                }
            }
            else
            {
                if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";

                string id1, id2;

                while(ss >> id1)
                {
                    if(ss.eof()) throw "lipsesc argumente [ajutor SWAP]";

                    ss >> id2;

                    if(id_entitati.find(id1) != id_entitati.end() && id_entitati.find(id2) != id_entitati.end())
                    {
                        int index_c = id_cadre[id];
                        int index_e1 = id_entitati[id1];
                        int index_e2 = id_entitati[id2];

                        auto it1 = find(cadre[index_c].second.begin(), cadre[index_c].second.end(), index_e1);
                        auto it2 = find(cadre[index_c].second.begin(), cadre[index_c].second.end(), index_e2);

                        if(it1 == cadre[index_c].second.end()) throw "entitate1 nu se afla in cadru";
                        if(it2 == cadre[index_c].second.end()) throw "entitate2 nu se afla in cadru";

                        iter_swap(it1,it2);
                    }
                    else throw "entitati nonexistente";

                    if(ss.eof()) break;
                }
            }
        }
        else if(optiune == "FOCUS")
        {
            if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";
            if(ss.eof()) throw "lipsesc argumente [ajutor FOCUS]";

            string id_entitate;
            ss >> id_entitate;

            if(id_entitate == "CENTER")
            {
                entitate_focusata = nullptr;
                if(!ss.eof())
                {
                    string s;
                    ss >> s;

                    if(s == "&") lant = true;
                    else throw "prea multe argumente [ajutor FOCUS]";
                }
            }
            else
            {
                if(id_entitati.find(id_entitate) == id_entitati.end()) throw "entitate nonexistenta";

                entitate_focusata = &entitati[id_entitati[id_entitate]];
                if(!ss.eof())
                {
                    string s;
                    ss >> s;

                    if(s == "&") lant = true;
                    else throw "prea multe argumente [ajutor FOCUS]";
                }
            }
        }
        else if(optiune == "ZOOM")
        {
            if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";
            if(ss.eof()) throw "lipsesc argumente [ajutor ZOOM]";

            bool centru = false;

            float valoare;
            ss >> valoare;
            if(ss.eof()) throw "lipsesc argumente [ajutor ZOOM]";

            float zoom;
            ss >> zoom;
            if(ss.eof()) throw "lipsesc argumente [ajutor ZOOM]";

            float durata;
            ss >> durata;

            cadru = &cadre[id_cadre[id]].first;
            entitate = entitate_focusata;

            if(entitate == nullptr) centru = true;

            window.setView(window.getDefaultView());

            int x, y;
            Vector2f poz(0.f,0.f), mij(0.f,0.f);

            if(centru)
            {
                x = round(0.5f*WIDTH);
                y = round(0.5f*HEIGHT);
            }
            else
            {
                poz = entitate->getPosition();
                mij = centru_entitate(entitate);
                x = poz.x >= 0.5f*WIDTH ? round(poz.x - mij.x) : round(poz.x + mij.x);
                y = poz.y >= 0.5f*HEIGHT ? round(poz.y + mij.y) : round(poz.y - mij.y);
            }

            Vector2i v1(x,y);
            Vector2f c1 = cadru->getCenter();

            cadru->zoom(1.f/valoare);

            if(!centru) cadru->setCenter(entitate->getPosition());
            else cadru->setCenter(window.getDefaultView().getCenter());
            cadru->zoom(zoom);
            window.setView(*cadru);

            Vector2f v2 = window.mapPixelToCoords(v1);

            float x_offset = 0.f, y_offset = 0.f;;

            if(zoom < 1.0f)
            {
                if(v2.x < 0.f) x_offset = -(v2.x - mij.x);
                else if(v2.x > 1.f*WIDTH) x_offset = 1.f*WIDTH - (v2.x + mij.x);
                if(v2.y < 0.f) y_offset = -v2.y;
                else if(v2.y > 1.f*HEIGHT) y_offset = 1.f*HEIGHT - v2.y;
            }
            else if(zoom == 1.f)
            {
                x_offset = 1.f*v1.x - v2.x;
                y_offset = 1.f*v1.y - v2.y;
            }

            if(durata == 0.f) cadru->move(x_offset,y_offset);
            else
            {
                float x1 = cadru->getCenter().x - c1.x;
                float y1 = cadru->getCenter().y - c1.y;

                cadru->zoom(valoare/zoom);
                cadru->setCenter(c1);

                animatie a;
                a.cmd = "ZOOM";
                a.r = valoare; a.x = x_offset+x1; a.y = y_offset+y1;
                a.v_r = (valoare-zoom) / (durata*SPEED);
                a.v_x = (x_offset+x1) / (durata*SPEED);
                a.v_y = (y_offset+y1) / (durata*SPEED);
                a.z = round(durata*SPEED);

                anim_cadre.push_back({cadru,a});
            }

            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor ZOOM]";
            }
        }
        else if(optiune == "FRAME")
        {
            if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";
            if(ss.eof()) throw "lipsesc argumente [ajutor FRAME]";

            float x1;
            ss >> x1;
            if(ss.eof()) throw "lipsesc argumente [ajutor FRAME]";

            float y1;
            ss >> y1;
            if(ss.eof()) throw "lipsesc argumente [ajutor FRAME]";

            float x2;
            ss >> x2;
            if(ss.eof()) throw "lipsesc argumente [ajutor FRAME]";

            float y2;
            ss >> y2;

            cadru = &cadre[id_cadre[id]].first;
            cadru->setViewport(FloatRect(x1,y1,x2,y2));
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor NEW]";
            }
        }
        else if(optiune == "RESET")
        {
            if(id_cadre.find(id) == id_cadre.end()) throw "cadru nonexistent";

            cadru = &cadre[id_cadre[id]].first;
            *cadru = window.getDefaultView();
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor NEW]";
            }
        }
        else throw "argument invalid [ajutor VIEW]";
    }while(lant);
}

void FADEIN(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor FADEIN]";

    string id;
    bool lant;

    do
    {
        lant = false;
        ss >> id;

        if(id_sunete.find(id) != id_sunete.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor FADEIN]";

            float valoare;
            ss >> valoare;
            if(ss.eof()) throw "lipsesc argumente [ajutor FADEIN]";

            float durata;
            ss >> durata;

            sunet = sunete[id_sunete[id]];

            if(durata == 0.f) sunet->setVolume(valoare);
            else
            {
                animatie a;
                a.cmd = "FADEIN";
                a.r = valoare; a.x = sunet->getVolume();
                a.v_r = (valoare - a.x) / (durata*SPEED);
                a.z = round(durata*SPEED);

                anim_sunete.push_back({sunet,a});
            }
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor FADEIN]";
            }
        }
        else if(id_entitati.find(id) != id_entitati.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor FADEIN]";

            float durata;
            ss >> durata;

            entitate = &entitati[id_entitati[id]];

            Color c = entitate->getColor();
            c.a = 0;
            entitate->setColor(c);

            if(durata == 0.f)
            {
                c.a = 255;
                entitate->setColor(c);
            }
            else
            {
                animatie a;
                a.cmd = "FADEIN";
                a.r = 0.f;
                a.v_r = 255.f / (durata*SPEED);
                a.z = round(durata*SPEED);

                anim_entitati.push_back({entitate,a});
            }
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor FADEIN]";
            }
        }
        else if(id_cadre.find(id) != id_cadre.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor FADEIN]";

            float durata;
            ss >> durata;

            int index = id_cadre[id];
            cadru = &cadre[index].first;

            fadeuri.resize(cadre.size());
            fadeuri[index].first = true;
            fadeuri[index].second.setTexture(fade);

            Color c = fadeuri[index].second.getColor(); c.a = 255;
            fadeuri[index].second.setColor(c);

            if(durata == 0.f)
            {
                c.a = 0;
                fadeuri[index].second.setColor(c);
            }
            else
            {
                animatie a;
                a.cmd = "FADEIN";
                a.r = 0.f; a.z = index;
                a.v_r = 255.f / (durata*SPEED);
                a.z = index;

                anim_cadre.push_back({cadru,a});
            }
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor FADEIN]";
            }
        }
        else throw "id inexistent";
    }while(lant);

}

void FADEOUT(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor FADEOUT]";

    string id;
    bool lant;

    do
    {
        lant = false;
        ss >> id;

        if(id_sunete.find(id) != id_sunete.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor FADEOUT]";

            float valoare;
            ss >> valoare;
            if(ss.eof()) throw "lipsesc argumente [ajutor FADEOUT]";

            float durata;
            ss >> durata;

            sunet = sunete[id_sunete[id]];

            if(durata == 0.f) sunet->setVolume(valoare);
            else
            {
                animatie a;
                a.cmd = "FADEOUT";
                a.r = valoare; a.x = sunet->getVolume();
                a.v_r = (a.x - valoare) / (durata*SPEED);
                a.z = round(durata*SPEED);

                anim_sunete.push_back({sunet,a});
            }
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor FADEOUT]";
            }
        }
        else if(id_entitati.find(id) != id_entitati.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor FADEOUT]";

            float durata;
            ss >> durata;

            entitate = &entitati[id_entitati[id]];

            Color c = entitate->getColor();
            c.a = 255;
            entitate->setColor(c);

            if(durata == 0.f)
            {
                c.a = 0;
                entitate->setColor(c);
            }
            else
            {
                animatie a;
                a.cmd = "FADEOUT";
                a.r = 255.f;
                a.v_r = 255.f / (durata*SPEED);
                a.z = round(durata*SPEED);

                anim_entitati.push_back({entitate,a});
            }
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor FADEOUT]";
            }
        }
        else if(id_cadre.find(id) != id_cadre.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor FADEIN]";

            float durata;
            ss >> durata;

            int index = id_cadre[id];
            cadru = &cadre[index].first;

            fadeuri.resize(cadre.size());
            fadeuri[index].first = true;
            fadeuri[index].second.setTexture(fade);

            Color c = fadeuri[index].second.getColor(); c.a = 0;
            fadeuri[index].second.setColor(c);

            if(durata == 0.f)
            {
                c.a = 255;
                fadeuri[index].second.setColor(c);
            }
            else
            {
                animatie a;
                a.cmd = "FADEOUT";
                a.r = 0.f; a.z = index;
                a.v_r = 255.f / (durata*SPEED);
                a.z = index;

                anim_cadre.push_back({cadru,a});
            }
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor FADEOUT]";
            }
        }
        else  throw "id inexistent";
    }while(lant);
}

void SPRITE(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor SPRITE]";

    string id, textura;
    ss >> id;
    if(ss.eof()) throw "lipsesc argumente [ajutor SPRITE]";

    bool lant;

    do
    {
        lant = false;
        ss >> textura;

        if(textura == "HIDE")
        {
            if(id_entitati.find(id) == id_entitati.end()) throw "entitate nonexistenta";
            entitati_ascunse[id_entitati[id]] = true;
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor HIDE]";
            }
        }
        else if(textura == "SHOW")
        {
            if(id_entitati.find(id) == id_entitati.end()) throw "entitate nonexistenta";
            entitati_ascunse[id_entitati[id]] = false;
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SHOW]";
            }
        }
        else
        {
            if(id_entitati.find(id) != id_entitati.end()) throw "id deja existent";
            if(ss.eof()) throw "lipsesc argumente [ajutor SPRITE]";

            float x;
            ss >> x;
            if(ss.eof()) throw "lipsesc argumente [ajutor SPRITE]";

            float y;
            ss >> y;

            Texture *t = new Texture; texturi.push_back(t);
            if(!t->loadFromFile(textura)) throw "fisierul specificat nu se gaseste";

            int index = id_entitati[id] = index_entitate++;

            entitati_ascunse[index] = false;

            Sprite *s = new Sprite;
            s->setTexture(*t);
            s->setOrigin(centru_entitate(s));
            s->setPosition(x+centru_entitate(s).x, y+centru_entitate(s).y);

            entitati.push_back(*s);
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SPRITE]";
            }
        }
    }while(lant);

}

void TALK(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor TALK]";

    string id1, id2;
    float viteza;
    bool lant;

    do
    {
        lant = false;
        ss >> id1;
        if(id_entitati.find(id1) == id_entitati.end()) throw "entitate inexistenta";
        if(ss.eof()) throw "lipsesc argumente [ajutor TALK]";

        ss >> id2;
        if(id_sunete.find(id2) == id_sunete.end()) throw "sunet inexistent";
        if(ss.eof()) throw "lipsesc argumente [ajutor TALK]";

        entitate = &entitati[id_entitati[id1]];
        sunet = sunete[id_sunete[id2]];

        ss >> viteza;

        animatie a;
        a.cmd = "TALK";
        a.x = sunet->getDuration().asSeconds(); a.r = viteza*round(a.x); a.v_r = 1.f/viteza;
        a.z = round(a.x*SPEED);
        a.x = 1.f * TALK_SPEED; a.v_x = a.x / (a.v_r*SPEED); a.y = a.x;
        anim_entitati.push_back({entitate,a});

        if(!ss.eof())
        {
            string s;
            ss >> s;

            if(s == "&") lant = true;
            else throw "prea multe argumente [ajutor SLIDE]";
        }

        sunet->play();
    }while(lant);
}

void TELEPORT(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor TELEPORT]";

    string id;
    bool lant;

    do
    {
        lant = false;
        ss >> id;

        if(id_entitati.find(id) == id_entitati.end()) throw "entitate nonexistenta";
        if(ss.eof()) throw "lipsesc argumente [ajutor TELEPORT]";

        float x;
        ss >> x;
        if(ss.eof()) throw "lipsesc argumente [ajutor TELEPORT]";

        float y;
        ss >> y;
        if(!ss.eof())
        {
            string s;
            ss >> s;

            if(s == "&") lant = true;
            else throw "prea multe argumente [ajutor TELEPORT]";
        }

        entitate = &entitati[id_entitati[id]];
        entitate->setPosition(x+centru_entitate(entitate).x, y+centru_entitate(entitate).y);
        if(!ss.eof())
        {
            string s;
            ss >> s;

            if(s == "&") lant = true;
            else throw "prea multe argumente [ajutor TELEPORT]";
        }
    }while(lant);

}

void SLIDE(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor SLIDE]";

    string id;
    bool lant;

    do
    {
        lant = false;
        ss >> id;

        if(id_entitati.find(id) != id_entitati.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor SLIDE]";

            float x;
            ss >> x;
            if(ss.eof()) throw "lipsesc argumente [ajutor SLIDE]";

            float y;
            ss >> y;
            if(ss.eof()) throw "lipsesc argumente [ajutor SLIDE]";

            float durata;
            ss >> durata;

            entitate = &entitati[id_entitati[id]];

            animatie a;
            a.cmd = "SLIDE";
            a.x = x; a.y = y;
            a.v_x = x / (durata*SPEED);
            a.v_y = y / (durata*SPEED);
            a.z = round(durata*SPEED);

            anim_entitati.push_back({entitate,a});
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SLIDE]";
            }
        }
        else if(id_cadre.find(id) != id_cadre.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor SLIDE]";

            float x;
            ss >> x;
            if(ss.eof()) throw "lipsesc argumente [ajutor SLIDE]";

            float y;
            ss >> y;
            if(ss.eof()) throw "lipsesc argumente [ajutor SLIDE]";

            float durata;
            ss >> durata;

            cadru = &cadre[id_cadre[id]].first;

            animatie a;
            a.cmd = "SLIDE";
            a.x = x; a.y = y;
            a.v_x = x / (durata*SPEED);
            a.v_y = y / (durata*SPEED);
            a.z = round(durata*SPEED);

            anim_cadre.push_back({cadru,a});
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SLIDE]";
            }
        }
        else throw "id inexistent";
    }while(lant);
}

void SCALE(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor SCALE]";

    string id;
    bool lant;

    do
    {
        lant = false;
        ss >> id;


        if(id_entitati.find(id) == id_entitati.end()) throw "entitate nonexistenta";
        if(ss.eof()) throw "lipsesc argumente [ajutor SCALE]";

        float x;
        ss >> x;
        if(ss.eof()) throw "lipsesc argumente [ajutor SCALE]";

        float y;
        ss >> y;
        if(ss.eof()) throw "lipsesc argumente [ajutor SCALE]";

        float durata;
        ss >> durata;

        entitate = &entitati[id_entitati[id]];

        if(durata == 0.f) entitate->setScale(x,y);
        else
        {
            x -= 1.f; y -= 1.f;

            animatie a;
            a.cmd = "SCALE";
            a.x = x; a.y = y;
            a.v_x = x / (durata*SPEED);
            a.v_y = y / (durata*SPEED);
            a.z = round(durata*SPEED);

            anim_entitati.push_back({entitate,a});
            if(!ss.eof())
            {
                string s;
                ss >> s;

                if(s == "&") lant = true;
                else throw "prea multe argumente [ajutor SCALE]";
            }
        }
    }while(lant);

}

void ROTATE(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor ROTATE]";

    string id;
    bool lant;

    do
    {
        lant = false;
        ss >> id;

        if(id_entitati.find(id) != id_entitati.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor ROTATE]";

            float valoare;
            ss >> valoare;
            if(ss.eof()) throw "lipsesc argumente [ajutor ROTATE]";

            float durata;
            ss >> durata;

            entitate = &entitati[id_entitati[id]];

            if(durata == 0.f) entitate->rotate(valoare);
            else
            {
                animatie a;
                a.cmd = "ROTATE";
                a.r = valoare;
                a.v_r = valoare / (durata*SPEED);
                a.z = round(durata*SPEED);

                anim_entitati.push_back({entitate,a});
                if(!ss.eof())
                {
                    string s;
                    ss >> s;

                    if(s == "&") lant = true;
                    else throw "prea multe argumente [ajutor ROTATE]";
                }
            }
        }
        else if(id_cadre.find(id) != id_cadre.end())
        {
            if(ss.eof()) throw "lipsesc argumente [ajutor ROTATE]";

            float valoare;
            ss >> valoare;
            if(ss.eof()) throw "lipsesc argumente [ajutor ROTATE]";

            float durata;
            ss >> durata;

            cadru = &cadre[id_cadre[id]].first;

            if(durata == 0.f) cadru->rotate(valoare);
            else
            {
                animatie a;
                a.cmd = "ROTATE";
                a.r = valoare;
                a.v_r = valoare / (durata*SPEED);
                a.z = round(durata*SPEED);

                anim_cadre.push_back({cadru,a});
                if(!ss.eof())
                {
                    string s;
                    ss >> s;

                    if(s == "&") lant = true;
                    else throw "prea multe argumente [ajutor ROTATE]";
                }
            }
        }
        else throw "id inexistent";
    }while(lant);
}

void WAIT(stringstream& ss)
{
    evenimente();

    if(ss.eof()) throw "lipsesc argumente [ajutor WAIT]";

    string id;
    ss >> id;
    if(ss.eof()) throw "lipsesc argumente [ajutor WAIT]";

    float durata;

    if(id == "NONE")
    {
        if(ss.eof()) throw "lipsesc argumente [ajutor WAIT]";

        ss >> durata;
        if(!ss.eof()) throw "prea multe argumente [ajutor WAIT]";

        Clock c;
        Time t;

        while(t.asSeconds() < durata)
        {
            t = c.getElapsedTime();
            UPDATE();
        }
        return;
    }

    string anim;
    ss >> anim;
    if(ss.eof()) throw "lipsesc argumente [ajutor WAIT]";

    if(id_sunete.find(id) != id_sunete.end())
    {
        sunet = sunete[id_sunete[id]];

        if(anim != "-")
            for(size_t i = 0; i < anim_sunete.size(); i++)
                while(anim_sunete.size() && sunet == anim_sunete[i].first && anim_sunete[i].second.cmd == anim)
                    UPDATE();

        else while(sunet->getStatus() == Music::Playing) UPDATE();

        ss >> durata;
        if(!ss.eof()) throw "prea multe argumente [ajutor WAIT]";

        Clock c;
        Time t;

        while(t.asSeconds() < durata)
        {
            t = c.getElapsedTime();
            UPDATE();
        }
    }
    else if(id_entitati.find(id) != id_entitati.end())
    {
        entitate = &entitati[id_entitati[id]];

        for(size_t i = 0; i < anim_entitati.size(); i++)
            while(anim_entitati.size() && entitate == anim_entitati[i].first && anim_entitati[i].second.cmd == anim)
                UPDATE();

        ss >> durata;
        if(!ss.eof()) throw "prea multe argumente [ajutor WAIT]";

        Clock c;
        Time t;

        while(t.asSeconds() < durata)
        {
            t = c.getElapsedTime();
            UPDATE();
        }
    }
    else if(id_cadre.find(id) != id_cadre.end())
    {
        cadru = &cadre[id_cadre[id]].first;
        for(size_t i = 0; i < anim_cadre.size(); i++)
            while(anim_cadre.size() && cadru == anim_cadre[i].first  && anim_cadre[i].second.cmd == anim)
                UPDATE();

        ss >> durata;
        if(!ss.eof()) throw "prea multe argumente [ajutor WAIT]";

        Clock c;
        Time t;

        while(t.asSeconds() < durata)
        {
            t = c.getElapsedTime();
            UPDATE();
        }
    }
    else throw "id inexistent";
}

bool initializare()
{
    if(!fade.loadFromFile("imagini/negru.png")) throw "imagini/negru.png nu exista";
    srand(time(NULL));

    string cmd;
    while(getline(in,cmd) && !cmd.size());

    if(cmd == "INIT")
    {
        while(getline(in,cmd) && cmd != "ENDINIT")
        {
            if(in.eof()) return false;
            stringstream ss(cmd);
            while(getline(ss,cmd,';'))
            {
                stringstream ss_cmd(cmd);
                string s_cmd;

                ss_cmd >> s_cmd;

                if(s_cmd == "AUDIO")
                {
                    try
                    {
                        AUDIO(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "SOUND")
                {
                    try
                    {
                        SOUND(ss_cmd);
                    }
                    catch(const char* e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "VIEW")
                {
                    try
                    {
                        VIEW(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "SPRITE")
                {
                    try
                    {
                        SPRITE(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "SCALE")
                {
                    try
                    {
                        SCALE(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "ROTATE")
                {
                    try
                    {
                        ROTATE(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "WAIT")
                {
                    try
                    {
                        WAIT(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "DRAW")
                {
                    evenimente();
                    afiseaza();

                    continue;
                }

                if(s_cmd[0] == '#') continue;
            }
        }
        return true;
    }

    return false;
}

bool inregistrare()
{
    string cmd;
    while(getline(in,cmd) && !cmd.size());

    if(cmd == "RECORD")
    {
        while(getline(in,cmd) && cmd != "ENDRECORD")
        {
            if(in.eof()) return false;
            stringstream ss(cmd);
            while(getline(ss,cmd,';'))
            {
                stringstream ss_cmd(cmd);
                string s_cmd;

                ss_cmd >> s_cmd;

                if(s_cmd == "SOUND")
                {
                    try
                    {
                        SOUND(ss_cmd);
                    }
                    catch(const char* e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "VIEW")
                {
                    try
                    {
                        VIEW(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "FADEIN")
                {
                    try
                    {
                        FADEIN(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "FADEOUT")
                {
                    try
                    {
                        FADEOUT(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "SPRITE")
                {
                    try
                    {
                        SPRITE(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "TALK")
                {
                    try
                    {
                        TALK(ss_cmd);
                    }
                    catch(const char* e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "TELEPORT")
                {
                    try
                    {
                        TELEPORT(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "SLIDE")
                {
                    try
                    {
                        SLIDE(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "SCALE")
                {
                    try
                    {
                        SCALE(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "ROTATE")
                {
                    try
                    {
                        ROTATE(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "WAIT")
                {
                    try
                    {
                        WAIT(ss_cmd);
                    }
                    catch(const char*& e)
                    {
                        cout << s_cmd << ':' << e;
                        return false;
                    }

                    continue;
                }

                if(s_cmd == "DRAW")
                {
                    evenimente();
                    afiseaza();

                    continue;
                }

                if(s_cmd == "#") continue;
            }
        }
        return true;
    }

    return false;
}

int main()
{
    window.setFramerateLimit(SPEED);

    if(!initializare() || !inregistrare())
    {
        curata_memoria();
        return 0;
    }

    while(window.isOpen()) evenimente();

    curata_memoria();

    return 0;
}
