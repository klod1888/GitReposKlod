#include <SFML/Graphics.hpp>
#include <QString>
#include <QFile>
#include <QTextStream>

using namespace sf;

float OffsetX=0, OffsetY=0;
const int H = 20, W = 460;
static int score=0;
bool startgame = false, endgame = false, win = false;

QString TestMap;
class Player
{
public:
    float pos_x, pos_y;
    FloatRect per_rect;
    bool in_air;
    Sprite MainHero;
    float CurFr;
    int leben;

    Player(Texture &textSP)
    {
        MainHero.setTexture(textSP);
        MainHero.setTextureRect(IntRect(0, 128, 48, 64));
        MainHero.setPosition(50, 30);
        per_rect = FloatRect(32, 32, 48, 64);
        pos_x = pos_y = 0;
        CurFr = 0;
    }

    void Collision(int Richtung)
    {
        for(int i=per_rect.top/32; i<(per_rect.top+per_rect.height)/32; i++)
            for(int j=per_rect.left/32; j<(per_rect.left+per_rect.width)/32; j++)
            {
                if(TestMap[i*W+j] == 'W' || TestMap[i*W+j] == '0')
                {
                    if(pos_x<0 && Richtung == 0) per_rect.left = j*32+32;
                    if(pos_x>0 && Richtung == 0) per_rect.left = j*32 - per_rect.width;
                    if(pos_y<0 && Richtung == 1) {per_rect.top = i*32+32; pos_y = 0;}
                    if(pos_y>0 && Richtung == 1) {per_rect.top = i*32 - per_rect.height; pos_y = 0; in_air = false;}
                }
                if(TestMap[i*W+j] == 'C')
                {
                    TestMap[i*W+j] = ' ';
                    score += 50;
                }
                if(TestMap[i*W+j] == 'V')
                {
                    win = true;
                }
                if(TestMap[i*W+j] == 'L')
                {
                    TestMap[i*W+j] = ' ';
                    leben++;
                }
            }
    }

    void Action(float timeclock)
    {
        per_rect.left += pos_x * timeclock;
        Collision(0);
        if(in_air) pos_y += 0.0005 * timeclock;
        per_rect.top += pos_y * timeclock;
        in_air = true;
        Collision(1);

        CurFr += 0.005*timeclock;
        if(CurFr>4) CurFr-=4;
            if(pos_x<0) MainHero.setTextureRect(IntRect(48*int(CurFr), 64, 48, 64));
            if(pos_x>0) MainHero.setTextureRect(IntRect(48*int(CurFr), 128, 48, 64));
        MainHero.setPosition(per_rect.left - OffsetX, per_rect.top - OffsetY);
        pos_x = 0;
    }
};

class Feind
{
public:
    float pos_x, pos_y;
    FloatRect f_rect;
    Sprite EinFeind;
    float CurFr;
    bool life;

    void Collision()
    {
        for(int i=f_rect.top/32; i<(f_rect.top+f_rect.height)/32; i++)
            for(int j=f_rect.left/32; j<(f_rect.left+f_rect.width)/32; j++)
            {
                if(TestMap[i*W+j] == 'W' || TestMap[i*W+j] == '0' || TestMap[i*W+j] == 'C')
                {

                    if(pos_x<0) f_rect.left = j*32+32;
                    if(pos_x>0) f_rect.left = j*32 - f_rect.width;
                    pos_x *= -1;
                }
            }
    }

    void einrichten(Texture &Abbildung, int Stellung_x, int Stellung_y)
    {
        EinFeind.setTexture(Abbildung);
        f_rect = FloatRect(Stellung_x, Stellung_y, 48, 64);

        pos_x = 0.05;
        CurFr = 0;
        life = true;
    }

    void Neuerung(float Zeit)
    {
        f_rect.left += pos_x * Zeit;
        Collision();

        CurFr += 0.005*Zeit;
        if(CurFr>4) CurFr-=4;
        if(pos_x<0) EinFeind.setTextureRect(IntRect(48*int(CurFr), 0, 48, 64));
        if(pos_x>0) EinFeind.setTextureRect(IntRect(48*int(CurFr), 64, 48, 64));
        if(!life)   EinFeind.setTextureRect(IntRect(0, 128, 48, 64));
        EinFeind.setPosition(f_rect.left - OffsetX, f_rect.top - OffsetY);
    }
};

void Feind_Einrichten(Texture &feind_texture, Feind soldat_feind[])
{
    soldat_feind[0].einrichten(feind_texture, 24*32, 17*32);
    soldat_feind[1].einrichten(feind_texture, 46*32, 17*32);
    soldat_feind[2].einrichten(feind_texture, 118*32, 17*32);
    soldat_feind[3].einrichten(feind_texture, 213*32, 17*32);
    soldat_feind[4].einrichten(feind_texture, 95*32, 17*32);
    soldat_feind[5].einrichten(feind_texture, 235*32, 17*32);
    soldat_feind[6].einrichten(feind_texture, 268*32, 17*32);
    soldat_feind[7].einrichten(feind_texture, 300*32, 17*32);
    soldat_feind[8].einrichten(feind_texture, 375*32, 17*32);
    soldat_feind[9].einrichten(feind_texture, 420*32, 17*32);
}

int main()
{
    RenderWindow window(VideoMode(800,450), "Hallo SFML 2.2");
    window.setMouseCursorVisible(false);
    Texture texture, tileset, feind, grund, start;
    texture.loadFromFile("Stoyan8bit.png");
    tileset.loadFromFile("tileset1.png");
    start.loadFromFile("start.png");
    feind.loadFromFile("soldat_feind.png");
    grund.loadFromFile("Grund.png");
    Sprite Grund, Start;
    Grund.setTexture(grund);
    Grund.setScale(1,1.125);
    Start.setTexture(start);
    Start.setPosition(200, 165);

    char sCode[25];

    QFile zMap("ztestmp228.kmp");
    QTextStream in(&zMap);
    if (!zMap.open(QIODevice::ReadOnly | QIODevice::Text))
            return 0;
    for(int i=0; !in.atEnd(); i++)
    {
        TestMap += in.readLine();
    }

    Clock tmclck, lbntmr;

    Player Homer(texture);
    Homer.leben =3;

    Feind soldat_feind[10];
    Feind_Einrichten(feind, soldat_feind);
    Sprite map;
    map.setTexture(tileset);
    while(window.isOpen())
    {
        Event event;
        float timeclock = tmclck.getElapsedTime().asMicroseconds(), mnlbntmr;
        tmclck.restart();

        timeclock/=400;
        while(window.pollEvent(event))
        {
            if(event.type == Event::Closed)
                window.close();
        }
        if(Keyboard::isKeyPressed(Keyboard::R))
        {
            startgame = false;
            endgame = false;
            win = false;
            Feind_Einrichten(feind, soldat_feind);
            Homer.per_rect = FloatRect(32, 32, 48, 64);
            Homer.MainHero.setTextureRect(IntRect(0, 128, 48, 64));
            Homer.leben = 3;
            score = 0;
            OffsetX=0, OffsetY=0;
            TestMap.clear();
            QFile zMap("ztestmp228.kmp");
            QTextStream in(&zMap);
            if (!zMap.open(QIODevice::ReadOnly | QIODevice::Text))
                    return 0;
            for(int i=0; !in.atEnd(); i++)
            {
                TestMap += in.readLine();
            }
        }
        if(Keyboard::isKeyPressed(Keyboard::X))
            startgame = true;
        if(startgame && !endgame && !win)
        {
            if(Keyboard::isKeyPressed(Keyboard::Left))
            {
                Homer.pos_x = -0.1;
            }
            if(Keyboard::isKeyPressed(Keyboard::Right))
            {
                Homer.pos_x = 0.1;
            }
            if(Keyboard::isKeyPressed(Keyboard::Up))
            {
                if(!Homer.in_air)
                {
                    Homer.pos_y = -0.3;
                    Homer.in_air = true;
                }
            }
        }

        Font FontVoba;
        if(!FontVoba.loadFromFile("lg0dOCRp.ttf"))
            Homer.MainHero.setColor(Color::Red);
        Text Lifes, Score, gameover, Win, RestartT;
        String nLife = itoa(Homer.leben, sCode, 10), enLife= "Lifes: "+nLife, nScore = itoa(score, sCode, 10), enScore = "Score: " + nScore, GameOver = "Game Over", TWin = "Congratulation!!!", RestartText = "FOR RESTART TAP R";
        Lifes.setFont(FontVoba);
        Lifes.setString(enLife);
        Lifes.setCharacterSize(24);
        Lifes.setColor(Color::Red);
        Lifes.setStyle(Lifes.Bold);
        Lifes.setPosition(700, 12);
        Score.setFont(FontVoba);
        Score.setString(enScore);
        Score.setCharacterSize(24);
        Score.setColor(Color::Yellow);
        Score.setStyle(Score.Bold);
        Score.setPosition(32, 12);
        gameover.setFont(FontVoba);
        gameover.setString(GameOver);
        gameover.setCharacterSize(48);
        gameover.setColor(Color::Red);
        gameover.setStyle(gameover.Bold);
        gameover.setPosition(282, 162);
        Win.setFont(FontVoba);
        Win.setString(TWin);
        Win.setCharacterSize(48);
        Win.setColor(Color::Yellow);
        Win.setStyle(Win.Bold);
        Win.setPosition(222, 162);
        RestartT.setFont(FontVoba);
        RestartT.setString(RestartText);
        RestartT.setCharacterSize(24);
        RestartT.setColor(Color::Green);
        RestartT.setStyle(RestartT.Bold);
        RestartT.setPosition(282, 252);

        Homer.Action(timeclock);
        for(int i=0; i<10; i++)
        {
            soldat_feind[i].Neuerung(timeclock);
            if(Homer.per_rect.intersects(soldat_feind[i].f_rect))
            {
                if(soldat_feind[i].life)
                {
                    if(Homer.pos_y>0)
                    {
                        soldat_feind[i].pos_x = 0;
                        Homer.pos_y = -0.2;
                        soldat_feind[i].life = false;
                        score += 500;
                    }
                    else
                    {
                        Homer.MainHero.setColor(Color::White);
                        mnlbntmr = lbntmr.getElapsedTime().asSeconds();
                        lbntmr.restart();

                        if(mnlbntmr>1)
                        {
                            if(Homer.leben>0 && !win)
                            {
                                Homer.leben--;
                                Homer.MainHero.setColor(Color::Red);
                            }
                        }
                    }
                }
            }
        }

        if(Homer.leben<1)  endgame = true;

        if(Homer.per_rect.left>400) OffsetX = Homer.per_rect.left - 400;
        if(Homer.per_rect.top>300)OffsetY = Homer.per_rect.top - 300;

        window.clear();
        window.draw(Grund);

        for(int i=0; i<H; i++)
            for(int j=0; j<W; j++)
            {
                if(TestMap[i*W+j] == 'W') map.setTextureRect(IntRect(64, 32, 32, 32));
                if(TestMap[i*W+j] == '0') map.setTextureRect(IntRect(0, 0, 32, 32));
                if(TestMap[i*W+j] == 'C') map.setTextureRect(IntRect(96, 96, 32, 32));
                if(TestMap[i*W+j] == 'L') map.setTextureRect(IntRect(128, 96, 32, 32));
                if(TestMap[i*W+j] == ' ') map.setTextureRect(IntRect(128, 128, 32, 32));
                map.setPosition(j*32 - OffsetX, i*32 - OffsetY);
                window.draw(map);
            }
        window.draw(Homer.MainHero);
        for(int i=0; i<10; i++)  window.draw(soldat_feind[i].EinFeind);
        if(!startgame)
            window.draw(Start);

        if(endgame)
        {
            Homer.MainHero.setTextureRect(IntRect(0, 192, 48, 64));
            window.draw(gameover);
            window.draw(RestartT);
        }
        if(win)
        {
            window.draw(Win);
            window.draw(RestartT);
            Score.setPosition(282, 222);
        }
        window.draw(Lifes);
        window.draw(Score);

        window.display();
    }
    return 0;
}
