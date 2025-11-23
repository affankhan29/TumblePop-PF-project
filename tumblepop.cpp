#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{

			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
		}
	}

}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;

	offset_y += velocityY;

	char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

	if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
	{
		onGround = true;
	}
	else
	{
		player_y = offset_y;
		onGround = false;
	}

	if (!onGround)
	{
		velocityY += gravity;
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
	}

	else
	{
		velocityY = 0;
	}
}
void screenborder(float &player_x , float &player_y , int &playerwidth , float &velocityY , int &screenx ){
	if(player_x < 0)
		player_x = 0;  //left border
	if(player_y <0 ){	//top border
		player_y = 0;
		velocityY = 0;
	}
	if(player_x + playerwidth > screenx) //right border
		player_x = screen_x - playerwidth;
}
void movement(bool &onGround , float &velocityY ,const float &jumpStrength ,bool &isJumping ,float &player_x , float &player_y , Sprite &PlayerSprite, float &speed , const int &cell_size , Texture &PlayerTexture , int &PlayerHeight){
	if(Keyboard::isKeyPressed(Keyboard::Up)){
			if(onGround){
				velocityY = jumpStrength;
				onGround = false;
				isJumping = true;
			}
		}
	
		int texW = (int)PlayerTexture.getSize().x;
		int texH = (int)PlayerTexture.getSize().y;
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			player_x -= speed;
			//for flipping
        	PlayerSprite.setTextureRect(IntRect(0, 0, texW, texH));//draws the picture normally
		}
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			player_x += speed;
			//for flipping
        	PlayerSprite.setTextureRect(IntRect(texW, 0, -texW, texH));  //starts from the right (hence flipped)
		}
		if(Keyboard :: isKeyPressed(Keyboard::Down)&& onGround && (player_y + PlayerHeight < 10 * cell_size)){  //last condition checks if the player is NOT on lowest row (player_Y + PlayerHeight means that the bottom of the player is being checked against the bottom most row instead of its any othe part )
			player_y += cell_size; //moves one total cell size down
			onGround = false; 		//is NOT on ground for that frame because its in air AND moving down
			velocityY = 0;			//vertical velocity should be zero for that while
		}
}

int main()
{

	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	//level specifics
	const int cell_size = 64;
	const int height = 14;
	const int width = 18;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);
	

	//Music initialisation
	Music lvlMusic;

	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	//player data
	float player_x = 500;
	float player_y = 150;

	float speed = 5;

	const float jumpStrength = -20; // Initial jump velocity
	const float gravity = 1;  // Gravity acceleration

	bool isJumping = false;  // Track if jumping

	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;

	Texture PlayerTexture;
	Sprite PlayerSprite;

	bool onGround = false;

	float offset_x = 0;
	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	int PlayerHeight = 102;
	int PlayerWidth = 96;

	bool up_button = false;

	char top_left = '\0';
	char top_right = '\0';
	char top_mid = '\0';

	char left_mid = '\0';
	char right_mid = '\0';

	char bottom_left = '\0';
	char bottom_right = '\0';
	char bottom_mid = '\0';

	char bottom_left_down = '\0';
	char bottom_right_down = '\0';
	char bottom_mid_down = '\0';

	char top_right_up = '\0';
	char top_mid_up = '\0';
	char top_left_up = '\0';

	PlayerTexture.loadFromFile("Data/player.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setScale(3.f,3.f); //player is scaled by 3 , i.e , player size on screen becomes (96*3 , 102 *3) 
	PlayerSprite.setPosition(player_x, player_y);


	//creating level array
lvl = new char* [height];
for (int i = 0; i < height; i++)
{
    lvl[i] = new char[width];
}



// bottom platform
for (int j = 0; j < width; j++)
{
    lvl[10][j] = '#';   /// bottom row is 10 in my row screen
}
for (int j = 0; j < width; j++)
{
	if (j <=6 || j >=11)
    lvl[8][j] = '#';  
}

for (int j = 0; j < width; j++)
{
	if ((j <=5 || j >=8 && j<=9 || j >=12) && j!=2 && j!=15)
    lvl[6][j] = '#';  
}
for (int j = 0; j < width; j++)
{
	if ((j>=1 && j<=4 || j >=8 && j<=15  ) && j!=11 && j!=12)
    lvl[4][j] = '#';  
}
for (int j = 0; j < width; j++)
{    if (j!=0 && j!=6 && j!=7 && j!=8 && j!=17)
    lvl[2][j] = '#';   /// bottom row is 10 in my row screen
}

	Event ev;
	//main loop
	while (window.isOpen())
	{

		while (window.pollEvent(ev)) //for one time execution (clicks , presses ,etc)
		{
			if (ev.type == Event::Closed) 
			{
				window.close();
			}

			if (ev.type == Event::KeyPressed)
			{
			}

		}

		//presing escape to close
		if (Keyboard::isKeyPressed(Keyboard::Escape)) //for elements that get checked every frame (holding)
		{
			window.close();
		}
		movement(onGround, velocityY, jumpStrength, isJumping, player_x, player_y, PlayerSprite, speed, cell_size, PlayerTexture , PlayerHeight); //player movement function call

		window.clear();

		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		int Pwidth_scaled  = PlayerTexture.getSize().x * 3; //setting width according to player size , i.e multiplying by 3
		int Pheight_scaled = PlayerTexture.getSize().y * 3;
		player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		screenborder(player_x , player_y , PlayerWidth , velocityY , screen_x); //function for setting borders of the window
		PlayerSprite.setPosition(player_x, player_y);
		window.draw(PlayerSprite);

		window.display();
	}

	//stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	return 0;
}

