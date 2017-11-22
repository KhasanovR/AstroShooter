#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <list>
#include <cstring>
#include <vector>

using namespace sf;
using namespace std;

int num_ast = 0;
int num_of_bullets = 10;
int level = 0;
float DEGTORAD = 0.017453f;
string username;
bool missed, damaged;
bool intro_check = false;
bool load = false;

const int W = 1366;
const int H = 768;


string fill_string(string str)
{
	while (str.length() != 25)
	{
		str.push_back(' ');
	}
	return str;
}



class User
{
public:
	int rate;
	int level;
	char login[20];
	char dt[25];
	User()
	{
		rate = 0;
		level = 0;
	}	

	void write()
	{
		ofstream fout("user.dat", ios::binary | ios::app);
		fout.write((char*)this, sizeof(User));
		fout.close();
	}

	string getdate()
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		return to_string(time.wHour) + ":" + to_string(time.wMinute) + ":" + to_string(time.wSecond) + " " + to_string(time.wDay) + "/" + to_string(time.wMonth) + "/" + to_string(time.wYear);
	}
	
};

class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
	std::vector<IntRect> frames;

	Animation(){}

	Animation(Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
		Frame = 0;
		speed = Speed;

		for (int i = 0; i<count; i++)
			frames.push_back(IntRect(x + i*w, y, w, h));

		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);
	}


	void update()
	{
		Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n>0) sprite.setTextureRect(frames[int(Frame)]);
	}

	bool isEnd()
	{
		return Frame + speed >= frames.size();
	}

};

class Entity
{
public:
	float x, y, dx, dy, R, angle;
	bool life;
	std::string name;
	Animation anim;

	Entity()
	{
		life = 1;
	}
	void settings(Animation &a, int X, int Y, float Angle = -90, int radius = 1)
	{
		anim = a;
		x = X; y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void update(){};

	void draw(RenderWindow &app)
	{
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		app.draw(anim.sprite);

		
	}

	virtual ~Entity(){};
};

class asteroid : public Entity
{
public:
	asteroid()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroid";
	}

	void  update()
	{
		x += 0;
		if (level/3 < 1)
		{
			y += 1;
		}
		else{
			y += level / 3;
		}
	}

};

class bullet : public Entity
{
public:
	bullet()
	{
		name = "bullet";
	}

	void  update()
	{
		dx = cos(angle*DEGTORAD) * 6;
		dy = sin(angle*DEGTORAD) * 6;
		x += dx;
		y += dy;
		if (y < 0) life = false;
		if (y > H) life = false;
	}

};

class player : public Entity
{
public:
	bool thrust;
	
	int damage = 0;
	player()
	{
		name = "player";
	}

	void update(){
		if (x>W) x = W; if (x<0) x = 0;
		if (y<(float)2 * H / 3) y = (float)2 * H / 3; if (y>H) y = H;
	}

};

bool isCollide(Entity *a, Entity *b)
{
	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y)<
		(a->R + b->R)*(a->R + b->R);
}

int main(){

	RenderWindow game(VideoMode(W, H), "Asteroids!", Style::Fullscreen);
	game.setFramerateLimit(60);
	game.setMouseCursorVisible(false);
	
	SoundBuffer buffer_intro;
	buffer_intro.loadFromFile("sound/mainmenu.wav");
	
	Sound intro;
	intro.setBuffer(buffer_intro);
	intro.setLoop(true);

	SoundBuffer buffer_blaster;
	buffer_blaster.loadFromFile("sound/repeat-1.wav");
	
	Sound blaster;
	blaster.setBuffer(buffer_blaster);

	SoundBuffer buffer_hover;
	buffer_hover.loadFromFile("sound/hover.wav");
	
	Sound hover;
	hover.setBuffer(buffer_hover);

	SoundBuffer collidebuffer;
	collidebuffer.loadFromFile("sound/glock.wav");
	
	Sound collide;
	collide.setBuffer(collidebuffer);

	Font font;
	font.loadFromFile("font/Electro Shackle.otf");

	//for about part
	Font ab;
	ab.loadFromFile("font/arial.ttf");
	Text _about, _rate;
	_about.setOutlineThickness(5);
	_about.setFont(ab);
	_about.setCharacterSize(24);
	_about.getLocalBounds();
	string str, str1;
	ifstream fin("about.txt");

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //														Choose New or Load																       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
choose:
	User *user = new User();
	load = true;
	intro.play();
	intro_check = false;
	
	Texture bac;
	Texture t1, t2, t3, t4, t5, t6, t7;
	t1.loadFromFile("images/spaceship.png");
	t2.loadFromFile("images/03cd9b196e48e0426d61fb17635ef83a.jpg");
	t3.loadFromFile("images/explosions/type_C.png");
	t4.loadFromFile("images/rock.png");
	t5.loadFromFile("images/fire_blue.png");
	t6.loadFromFile("images/rock_small.png");
	t7.loadFromFile("images/explosions/type_B.png");
	
	//about part
	Texture me;
	me.loadFromFile("images/3x4.jpg");
	Sprite _me(me);
	_me.scale(0.115, 0.115);
	_me.setPosition(100, 100);
	bac.setSmooth(true);
	bac.loadFromFile("images/background.jpg");
	Sprite back(bac);

	bool choice = true;
	/////menu main loop/////
	while (game.isOpen())
	{
		Event event;
		while (game.pollEvent(event))
		{
			Text New_Profile;
			New_Profile.setFont(font);
			New_Profile.setOutlineThickness(5);
			New_Profile.setCharacterSize(48);
			New_Profile.getLocalBounds();
			if (choice == false)
				New_Profile.setFillColor(sf::Color::Red);
			else
				New_Profile.setFillColor(sf::Color::Blue);
			New_Profile.setString("New Profile");
			New_Profile.setPosition(W / 2 - 350, H / 2 - 100);

			Text Load_Profile;
			Load_Profile.setFont(font);
			Load_Profile.setOutlineThickness(5);
			Load_Profile.setCharacterSize(48);
			Load_Profile.getLocalBounds();
			if (choice == true)
				Load_Profile.setFillColor(sf::Color::Red);
			else
				Load_Profile.setFillColor(sf::Color::Blue);
			Load_Profile.setString("Load Profile");
			Load_Profile.setPosition(W / 2 - 175, H / 2);

			if (Keyboard::isKeyPressed)
			{
				if (Keyboard::isKeyPressed(Keyboard::Down))
				{
					hover.play();
					if (choice)
					{
						choice = false;
					}
					else{
						choice = true;
					}
				}

				if (Keyboard::isKeyPressed(Keyboard::Up))
				{
					hover.play();
					if (choice)
					{
						choice = false;
					}
					else{
						choice = true;
					}
				}

				if (Keyboard::isKeyPressed(Keyboard::Escape))
				{
					return 0;
				}

				if (Keyboard::isKeyPressed(Keyboard::Space))
				{
					hover.play();
					if (choice)
					{
						load = false;
						goto input;
					}
					else
					{
						goto input;
					}
				}
			}
			game.clear();
			game.draw(back);
			game.draw(New_Profile);
			game.draw(Load_Profile);
			game.display();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //														Get Name Input																         //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
input:
	Text ENTER;
	ENTER.setFont(font);
	ENTER.setOutlineThickness(5);
	ENTER.setCharacterSize(24);
	ENTER.getLocalBounds();
	ENTER.setFillColor(sf::Color::Red);
	std::string enter = "ENTER YOUR NAME : ";
	ENTER.setString(enter);
	ENTER.setPosition(100, H / 2 - 100);
	
	Text warning;
	warning.setFont(font);
	warning.setOutlineThickness(5);
	warning.setCharacterSize(24);
	warning.getLocalBounds();
	warning.setFillColor(sf::Color::Green);
	warning.setString("Please, Enter Valid Name");
	warning.setPosition(100, H / 2 - 50);

	Text Name;
	Name.setFont(ab);
	Name.setOutlineThickness(5);
	Name.setCharacterSize(24);
	Name.getLocalBounds();
	Name.setFillColor(sf::Color::Red);
	std::string name;
	Name.setPosition(100 + 11 * 24, H / 2 - 100);

	Texture t;
	t.loadFromFile("images/background.jpg");
	Sprite backgr(t);
	bool check = false;
	while (game.isOpen())
	{
		Event event;

		while (game.pollEvent(event))
		{
			while (game.pollEvent(event))
			{
				if (Keyboard::isKeyPressed(Keyboard::Escape))
				{
					goto choose;
				}

				if (Keyboard::isKeyPressed){
					if (Keyboard::isKeyPressed(Keyboard::A) && name.length() <= 20)
						name.push_back('A');
					else if (Keyboard::isKeyPressed(Keyboard::B) && name.length() <= 20)
						name.push_back('B');
					else if (Keyboard::isKeyPressed(Keyboard::C) && name.length() <= 20)
						name.push_back('C');
					else if (Keyboard::isKeyPressed(Keyboard::D) && name.length() <= 20)
						name.push_back('D');
					else if (Keyboard::isKeyPressed(Keyboard::E) && name.length() <= 20)
						name.push_back('E');
					else if (Keyboard::isKeyPressed(Keyboard::F) && name.length() <= 20)
						name.push_back('F');
					else if (Keyboard::isKeyPressed(Keyboard::G) && name.length() <= 20)
						name.push_back('G');
					else if (Keyboard::isKeyPressed(Keyboard::H) && name.length() <= 20)
						name.push_back('H');
					else if (Keyboard::isKeyPressed(Keyboard::I) && name.length() <= 20)
						name.push_back('I');
					else if (Keyboard::isKeyPressed(Keyboard::J) && name.length() <= 20)
						name.push_back('J');
					else if (Keyboard::isKeyPressed(Keyboard::K) && name.length() <= 20)
						name.push_back('K');
					else if (Keyboard::isKeyPressed(Keyboard::L) && name.length() <= 20)
						name.push_back('L');
					else if (Keyboard::isKeyPressed(Keyboard::M) && name.length() <= 20)
						name.push_back('M');
					else if (Keyboard::isKeyPressed(Keyboard::N) && name.length() <= 20)
						name.push_back('N');
					else if (Keyboard::isKeyPressed(Keyboard::O) && name.length() <= 20)
						name.push_back('O');
					else if (Keyboard::isKeyPressed(Keyboard::P) && name.length() <= 20)
						name.push_back('P');
					else if (Keyboard::isKeyPressed(Keyboard::Q) && name.length() <= 20)
						name.push_back('Q');
					else if (Keyboard::isKeyPressed(Keyboard::R) && name.length() <= 20)
						name.push_back('R');
					else if (Keyboard::isKeyPressed(Keyboard::S) && name.length() <= 20)
						name.push_back('S');
					else if (Keyboard::isKeyPressed(Keyboard::T) && name.length() <= 20)
						name.push_back('T');
					else if (Keyboard::isKeyPressed(Keyboard::U) && name.length() <= 20)
						name.push_back('U');
					else if (Keyboard::isKeyPressed(Keyboard::V) && name.length() <= 20)
						name.push_back('V');
					else if (Keyboard::isKeyPressed(Keyboard::W) && name.length() <= 20)
						name.push_back('W');
					else if (Keyboard::isKeyPressed(Keyboard::X) && name.length() <= 20)
						name.push_back('X');
					else if (Keyboard::isKeyPressed(Keyboard::Y) && name.length() <= 20)
						name.push_back('Y');
					else if (Keyboard::isKeyPressed(Keyboard::Z) && name.length() <= 20)
						name.push_back('Z');
					else if (Keyboard::isKeyPressed(Keyboard::Num0) && name.length() <= 20)
						name.push_back('0');
					else if (Keyboard::isKeyPressed(Keyboard::Num1) && name.length() <= 20)
						name.push_back('1');
					else if (Keyboard::isKeyPressed(Keyboard::Num2) && name.length() <= 20)
						name.push_back('2');
					else if (Keyboard::isKeyPressed(Keyboard::Num3) && name.length() <= 20)
						name.push_back('3');
					else if (Keyboard::isKeyPressed(Keyboard::Num4) && name.length() <= 20)
						name.push_back('4');
					else if (Keyboard::isKeyPressed(Keyboard::Num5) && name.length() <= 20)
						name.push_back('5');
					else if (Keyboard::isKeyPressed(Keyboard::Num6) && name.length() <= 20)
						name.push_back('6');
					else if (Keyboard::isKeyPressed(Keyboard::Num7) && name.length() <= 20)
						name.push_back('7');
					else if (Keyboard::isKeyPressed(Keyboard::Num8) && name.length() <= 20)
						name.push_back('8');
					else if (Keyboard::isKeyPressed(Keyboard::Num9) && name.length() <= 20)
						name.push_back('9');
					else if (Keyboard::isKeyPressed(Keyboard::BackSpace))
					{
						if (name.length() > 0)
							name.erase(name.length() - 1);
					}
					else if (Keyboard::isKeyPressed(Keyboard::Space))
					{
						hover.play();
						if (name != "")
						{
							if (!load)
							{
								ifstream fin("user.dat", ios::binary);
								while (fin.read((char*)user, sizeof(User)))
								{
									if (name == user->login)
									{
										check = true;
										goto dis;
									}
								}
								username = name;
								goto main_menu;
							}
							else
							{
								ifstream fin("user.dat", ios::binary);
								while (fin.read((char*)user, sizeof(User)))
								{
									if (name == user->login)
									{
										goto main_menu;
									}
									else
									{
										check = true;
									}
								}
							}
						}
						else
						{
							check = true;
						}
					}
				}
			}
		}

	dis:
		game.clear();
		game.draw(backgr);
		game.draw(ENTER);
		Name.setString(name);
		game.draw(Name);
		if (check)
		game.draw(warning);
		game.display();
	}
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 //														Main Menu        																       //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
main_menu:
	if (intro_check)
		intro.play();
	int cho = 1;
	while (game.isOpen())
	{
		Event event;
		while (game.pollEvent(event))
		{
			if (Keyboard::isKeyPressed)
			if (Keyboard::isKeyPressed(Keyboard::Escape))
			{
				goto choose;
			}

			Text Start;
			Start.setFont(font);
			Start.setCharacterSize(48);
			Start.getLocalBounds();
			Start.setOutlineThickness(5);
			if (cho == 1)
				Start.setFillColor(sf::Color::Blue);
			else
				Start.setFillColor(sf::Color::Red);
			Start.setString("Start");
			Start.setPosition(W / 2 - 350, H / 2 - 300);

			Text BACK;
			BACK.setOutlineThickness(5);
			BACK.setFont(font);
			BACK.setCharacterSize(48);
			BACK.getLocalBounds();
			if (cho == 2)
				BACK.setFillColor(sf::Color::Blue);
			else
				BACK.setFillColor(sf::Color::Red);
			BACK.setString("Back Menu");
			BACK.setPosition(W / 2 - 350, H / 2 - 200);

			Text Rating;
			Rating.setOutlineThickness(5);
			Rating.setFont(font);
			Rating.setCharacterSize(48);
			Rating.getLocalBounds();
			if (cho == 3)
				Rating.setFillColor(sf::Color::Blue);
			else
				Rating.setFillColor(sf::Color::Red);
			Rating.setString("Top Rating");
			Rating.setPosition(W / 2 - 350, H / 2 - 100);

			Text About;
			About.setOutlineThickness(5);
			About.setFont(font);
			About.setCharacterSize(48);
			About.getLocalBounds();
			if (cho == 4)
				About.setFillColor(sf::Color::Blue);
			else
				About.setFillColor(sf::Color::Red);
			About.setString("About");
			About.setPosition(W / 2 - 350, H / 2);

			Text EXIT;
			EXIT.setOutlineThickness(5);
			EXIT.setFont(font);
			EXIT.setCharacterSize(48);
			EXIT.getLocalBounds();
			if (cho == 5)
				EXIT.setFillColor(sf::Color::Blue);
			else
				EXIT.setFillColor(sf::Color::Red);
			EXIT.setString("Exit");
			EXIT.setPosition(W / 2 - 350, H / 2 + 100);

			game.clear();
			game.draw(back);
			game.draw(Start);
			game.draw(BACK);
			game.draw(Rating);
			game.draw(About);
			game.draw(EXIT);
			game.display();

			if (Keyboard::isKeyPressed)
			{
				if (Keyboard::isKeyPressed(Keyboard::Down))
				{
					hover.play();
					if (cho == 1)
					{
						cho = 2;
					}
					else if (cho == 2)
					{
						cho = 3;
					}
					else if (cho == 3)
					{
						cho = 4;
					}
					else if (cho == 4)
					{
						cho = 5;
					}
					else if (cho == 5)
					{
						cho = 1;
					}
				}
				if (Keyboard::isKeyPressed(Keyboard::Up))
				{
					hover.play();
					if (cho == 1)
					{
						cho = 5;
					}
					else if (cho == 5)
					{
						cho = 4;
					}
					else if (cho == 4)
					{
						cho = 3;
					}
					else if (cho == 3)
					{
						cho = 2;
					}
					else if (cho == 2)
					{
						cho = 1;
					}
				}

				if (Keyboard::isKeyPressed(Keyboard::Space))
				{

					hover.play();
					if (cho == 1)
					{
						goto game;
					}
					else if (cho == 2)
					{
						goto choose;
					}
					else if (cho == 3)
					{
						goto rating;
					}
					else if (cho == 4)
					{
						goto about;
					}
					else if (cho == 5)
					{
						return 0;
					}
				}
			}
		}
	}

	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 //														Top Rating      																	   //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
rating:
	if (intro_check)
		intro.play();
	_rate.setOutlineThickness(5);
	_rate.setFont(ab);
	_rate.setCharacterSize(24);
	_rate.setPosition(300, 100);
	_rate.getLocalBounds();
	
	
	while (game.isOpen())
	{
		Event event;
		while (game.pollEvent(event))
		{
			if (Keyboard::isKeyPressed)
			if (Keyboard::isKeyPressed(Keyboard::Escape))
			{
				goto main_menu;
			}

		}
		game.clear();

		fin.open("user.dat", ios::binary);
		vector<User> users;
		for (int i = 0; fin.read((char*)user, sizeof(User)); i++)
		{
			users.push_back(*user);
		}
		fin.close();

		int is, js;
		for (js = 1; js < users.size(); js++)    // Start with 1 (not 0)
		{
			User temp = users[js];
			for (is = js - 1; (is >= 0) && (users[is].rate < temp.rate); is--)   // Smaller values move up
			{
				users[is + 1] = users[is];
			}
			users[is + 1] = temp;    //Put key into its proper location
		}

		
		fin.open("user.dat", ios::binary);
		str1 = fill_string("Player") + fill_string("    Rate") + fill_string(" Level") + fill_string("Time") + "\n\n";
		for (int i = 0; i < 15 && i < users.size(); i++)
		{
			str1.append(fill_string(to_string(i + 1) + ". " + string(users[i].login)) + fill_string(to_string(users[i].rate)) + fill_string(to_string(users[i].level)) + fill_string(" " + string(users[i].dt)) + "\n\n");
		}
		fin.close();
		_rate.setString(str1);
		
		game.draw(backgr);
		game.draw(_rate);
		game.display();
	}
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 //														About Game      																       //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
about:
	if (intro_check)
		intro.play();
	while (fin)
	{
		string s;
		getline(fin, s);
		str += s + "\n\n";
	}
	_about.setPosition(500, H);
	int pos = H;
	_about.setString(str);
	fin.close();
	while (game.isOpen())
	{
		Event event;
		
		while (game.pollEvent(event))
		{
			if (Keyboard::isKeyPressed)
			if (Keyboard::isKeyPressed(Keyboard::Escape))
			{
				goto main_menu;
			}
		}
		
		if(pos != 100){
			_about.setPosition(500, pos);
			pos--;
			for (int i = 0; i < 1000; i++)
			for (int j = 0; j < 1000; j++);
		}
			game.clear();
			game.draw(back);
			game.draw(_me);
			game.draw(_about);
			game.display();
	}
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 //														Game Begins     																       //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
game:
	num_ast = 0;
	num_of_bullets = 10;
	level = 0;
	intro.pause();
	intro_check = true;

	Sprite background(t2);

	Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
	Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
	Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
	Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
	Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
	Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
	Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);

	std::list<Entity*> entities;

	srand(time(0));

	for (int i = 0; i < 15; i++)
	{
		asteroid *a = new asteroid();
		a->settings(sRock, rand() % (W - 50) + 25, (rand() % 20), rand() % 360, 25);
		entities.push_back(a);
	}

	player *p = new player();
	p->settings(sPlayer, W / 2 + 25, H - 100);
	entities.push_back(p);

	/////main loop/////
	while (game.isOpen())
	{
		Event event;

		bool unpause = true;

		while (game.pollEvent(event))
		{
			if (event.type == Event::Closed)
				game.close();

			if (event.type == sf::Event::MouseButtonPressed)
			if (Mouse::isButtonPressed(Mouse::Left))
			{
				if (num_of_bullets > 0)
				{
					blaster.play();
					bullet *b = new bullet();
					b->settings(sBullet, p->x, p->y, p->angle, 10);
					entities.push_back(b);
					num_of_bullets--;
				}
			}

			if (Mouse::isButtonPressed(Mouse::Right))
			if (event.type == sf::Event::MouseMoved)
			{
				p->x = event.mouseMove.x;
				p->y = event.mouseMove.y;
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::D)) p->x += 10;
		if (Keyboard::isKeyPressed(Keyboard::A))  p->x -= 10;
		if (Keyboard::isKeyPressed(Keyboard::W)) p->y -= 10;
		if (Keyboard::isKeyPressed(Keyboard::S)) p->y += 10;

		if (Keyboard::isKeyPressed)
		if (Keyboard::isKeyPressed(Keyboard::Up))
		{
			if (num_of_bullets > 0){
				blaster.play();
				bullet *b = new bullet();
				b->settings(sBullet, p->x, p->y, p->angle, 10);
				entities.push_back(b);
				num_of_bullets--;
			}
		}

		if (Keyboard::isKeyPressed)
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			goto main_menu;
		}


		if (event.type == sf::Event::MouseMoved)
		{
			p->x = event.mouseMove.x;
			p->y = event.mouseMove.y;
		}

		for (auto a : entities)
		for (auto b : entities)
		{
			if (a->name == "asteroid" && b->name == "bullet")
			if (isCollide(a, b))
			{
				collide.play();
				num_ast++;
				num_of_bullets += 2;
				a->life = false;
				b->life = false;

				Entity *e = new Entity();
				e->settings(sExplosion, a->x, a->y);
				e->name = "explosion";
				entities.push_back(e);


				for (int i = 0; i < 2; i++)
				{
					if (a->R == 15) continue;
					Entity *e = new asteroid();
					e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
					entities.push_back(e);
				}

			}

			bool s = true;
			if (p->y < 2 * H / 3){
				s = false;
			}

			if (a->name == "player" && b->name == "asteroid"){
				if (isCollide(a, b) && s)
				{
					collide.play();
					p->damage++;
					b->life = false;
					Entity *e = new Entity();
					e->settings(sExplosion_ship, p->x, p->y);
					e->name = "explosion";
					entities.push_back(e);

					if (p->damage == 3){
						damaged = true;
						goto end;
					}
					else{
						p->settings(sPlayer, W / 2, H - 100);
						p->dx = 0; p->dy = 0;
					}
				}
			}
		}

		if (p->thrust)  p->anim = sPlayer_go;
		else   p->anim = sPlayer;

		for (auto e : entities)
		if (e->name == "explosion")
		if (e->anim.isEnd()) e->life = 0;

		int fix = 160 / (level + 1);
		if (rand() % fix == 0)
		{
			asteroid *a = new asteroid();
			a->settings(sRock, rand() % (W - 100) + 50, 0, rand() % 360, 25);
			entities.push_back(a);
		}

		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity *e = *i;
			e->update();
			e->anim.update();
			if (e->y > H && e->name != "bullet" && e->name != "player")
			{
				missed = true;
				goto end;
			}
			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}

		game.clear();
		//////draw//////
		game.draw(background);

		for (auto i : entities)
			i->draw(game);

		Text rate;
		rate.setFont(font);
		rate.setOutlineThickness(1);
		rate.setOutlineColor(Color::White);
		string str = std::to_string(num_ast);
		string l = "Rate: " + str;
		rate.setCharacterSize(24);
		rate.getLocalBounds();
		rate.setFillColor(sf::Color::Red);
		rate.setString(l);
		rate.setPosition(50, 75);
		game.draw(rate);

		Text Life;
		Life.setOutlineThickness(1);
		Life.setOutlineColor(Color::White);
		Life.setFont(font);
		str = std::to_string(3 - p->damage);
		l = "Life: " + str;
		Life.setCharacterSize(24);
		Life.getLocalBounds();
		Life.setFillColor(sf::Color::Red);
		Life.setString(l);
		Life.setPosition(W - 150, 25);
		game.draw(Life);

		Text Patron;
		Patron.setOutlineThickness(1);
		Patron.setOutlineColor(Color::White);
		Patron.setFont(font);
		str = std::to_string(num_of_bullets);
		l = "Patron: " + str;
		Patron.setCharacterSize(24);
		Patron.getLocalBounds();
		Patron.setFillColor(sf::Color::Red);
		Patron.setString(l);
		Patron.setPosition(W - 150, 50);
		game.draw(Patron);

		level = sqrt(sqrt(num_ast));
		Text Level;
		Level.setOutlineThickness(1);
		Level.setOutlineColor(Color::White);
		Level.setFont(font);
		str = std::to_string(level);
		l = "Level: " + str;
		Level.setCharacterSize(24);
		Level.getLocalBounds();
		Level.setFillColor(sf::Color::Red);
		Level.setString(l);
		Level.setPosition(W / 2, 25);
		game.draw(Level);

		Text User;
		User.setOutlineThickness(1);
		User.setOutlineColor(Color::White);
		User.setFont(font);
		l = "Player: "; // +user;
		User.setCharacterSize(24);
		User.getLocalBounds();
		User.setFillColor(sf::Color::Red);
		User.setString(l);
		User.setPosition(50, 25);
		game.draw(User);

		game.display();
	}
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 //														Game Over        																       //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
end:
	intro.play();



	if (!load)
	{
		for (int i = 0; i < 20; i++)
		{
			user->login[i] = username[i];
		}
	}
	user->level = level;
	user->rate = num_ast;
	
	for (int i = 0; i < 25; i++)
	{
		user->dt[i] = string(user->getdate())[i];
	}
	cout << user->login << " " << user->rate << " " << user->level << " " << user->dt << endl;
	
	user->write();

	int over = 1;
	/////menu main loop/////
	while (game.isOpen())
	{
		Event event;
		while (game.pollEvent(event))
		{

			Text dec;
			dec.setOutlineColor(Color::Red);
			dec.setOutlineThickness(2);
			dec.setFont(font);
			dec.setCharacterSize(28);
			dec.getLocalBounds();
			dec.setOutlineColor(Color::Black);
			dec.setOutlineThickness(3);
			dec.setFillColor(sf::Color::Green);

			if (missed && damaged)
			{
				dec.setString("Your Spaceship Was Seriously Damaged!!!\n\n               You missed An asteroid!!!");
				dec.setPosition(W / 2 - 5 * 64 - 28, 222);
			}
			else if (damaged){
				dec.setString("Your Spaceship Was Seriously Damaged!!!");
				dec.setPosition(W / 2 - 5 * 64 - 28, 260);
			}
			else if (missed){
				dec.setString("You missed An asteroid!!!");
				dec.setPosition(W / 2 - 4 * 64 + 26, 260);
			}

			Text end;
			end.setFont(font);
			end.setCharacterSize(64);
			end.getLocalBounds();
			end.setFillColor(sf::Color::Red);
			end.setOutlineColor(Color::Black);
			end.setOutlineThickness(3);
			end.setString("Game Over!!!");
			end.setPosition(W / 2 - 4 * 64, 136);

			Text main;
			main.setFont(font);
			main.setCharacterSize(48);
			main.getLocalBounds();
			main.setOutlineColor(Color::Black);
			main.setOutlineThickness(3);
			if (over == 1)
				main.setFillColor(sf::Color::Blue);
			else
				main.setFillColor(sf::Color::Red);
			main.setString("Main Menu");
			main.setPosition(W / 2 - 4 * 48, 330);

			Text again;
			again.setFont(font);
			again.setCharacterSize(48);
			again.getLocalBounds();
			again.setOutlineColor(Color::Black);
			again.setOutlineThickness(3);
			if (over == 2)
				again.setFillColor(sf::Color::Blue);
			else
				again.setFillColor(sf::Color::Red);
			again.setString("Try Again");
			again.setPosition(W / 2 - 3.5 * 48, 426);

			Text close_it;
			close_it.setFont(font);
			close_it.setCharacterSize(48);
			close_it.getLocalBounds();
			close_it.setOutlineColor(Color::Black);
			close_it.setOutlineThickness(3);
			if (over == 3)
				close_it.setFillColor(sf::Color::Blue);
			else
				close_it.setFillColor(sf::Color::Red);
			close_it.setString("Exit");
			close_it.setPosition(W / 2 - 2 * 48, 426 + 96);

			game.clear();
			game.draw(back);
			game.draw(dec);
			game.draw(main);
			game.draw(end);
			game.draw(again);
			game.draw(close_it);
			game.display();

			if (Keyboard::isKeyPressed)
			{
				if (Keyboard::isKeyPressed(Keyboard::Down))
				{
					hover.play();
					if (over == 1)
					{
						over = 2;
					}
					else if (over == 2)
					{
						over = 3;
					}
					else if (over == 3)
					{
						over = 1;
					}
				}

				if (Keyboard::isKeyPressed(Keyboard::Up))
				{
					hover.play();
					if (over == 1)
					{
						over = 3;
					}
					else if (over == 3)
					{
						over = 2;
					}
					else if (over == 2)
					{
						over = 1;
					}
				}

				if (Keyboard::isKeyPressed(Keyboard::Escape))
				{
					goto main_menu;
				}

				if (Keyboard::isKeyPressed(Keyboard::Space))
				{
					//fix this
					hover.play();
					switch (over)
					{
					case 1:
						goto main_menu;
						break;
					case 2:
						goto game;
						break;
					case 3:
						return 0;
						break;
					}
				}
			}
		}
	}
}