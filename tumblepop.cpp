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
// UNIVERSAL SPAWN FUNCTION
void spawn_enemy(float& x, float& y, Sprite& sprite, char** lvl, int height, int width, int cell_size, int enemy_height)
{
    while (true) 
    {
        int r = rand() % (height - 2); 
        int c = rand() % width;        

        if (lvl[r][c] != '#' && lvl[r + 1][c] == '#'&& (r + 1) != 10) 
        {
            x = c * cell_size;
            
            // FIX: Calculate Y based on height so feet touch the floor.
            // (r + 1) * cell_size is the floor line.
            // We subtract height to find the top position.
            // We add +10 to push the feet slightly INTO the floor block for reliable collision.
            y = (r + 1) * cell_size - enemy_height + 10; 
            
            sprite.setPosition(x, y);
            break; 
        }
    }
}

// UNIVERSAL MOVEMENT FUNCTION
void update_enemy_logic(float& x, float& y, float& speed, Sprite& sprite, char** lvl, int width, int cell_size, int enemy_width, int enemy_height)
{
    x += speed;

    // Because we adjusted Spawn to be +10 into the floor, this check is now reliable for short ghosts too.
    int feet_row = (y + enemy_height - 5) / cell_size; 
    int body_row = feet_row - 1; 

    if (speed > 0) // Moving RIGHT
    {
        int right_edge_px = x + enemy_width;
        int check_ahead_col = right_edge_px / cell_size;
        
        if (check_ahead_col >= width) {
            speed = -1.0f;
            sprite.setScale(2.5, 2.5);
        }
        else 
        {
            bool isWall = (lvl[body_row][check_ahead_col] == '#');
            bool isLedge = (lvl[feet_row][check_ahead_col] != '#');

            if (isWall || isLedge) {
                speed = -1.0f;
                sprite.setScale(2.5, 2.5);
            }
        }
    }
    else // Moving LEFT
    {
        int left_edge_px = x - enemy_width - 5; 
        int check_ahead_col = left_edge_px / cell_size;
        
        if (left_edge_px <= 0) {
            speed = 1.0f; 
            sprite.setScale(-2.5, 2.5);
        }
        else
        {
            if (check_ahead_col >= 0) 
            {
                bool isWall = (lvl[body_row][check_ahead_col] == '#');
                bool isLedge = (lvl[feet_row][check_ahead_col] != '#');

                if (isWall || isLedge) {
                    speed = 1.0f; 
                    sprite.setScale(-2.5, 2.5);
                }
            }
        }
    }
    sprite.setPosition(x, y);
}
/*2 problems were occuring with the orignal gravity function: 
1. Player's position isn't updated to the platform
2. and if it is , it is only updated when not colliding , hence some little changes are made 
*/
void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;
	offset_y += velocityY;
	int finalRow  = (int)(offset_y + Pheight) / cell_size ; 	//the row , or platform , to which the player WILL be moved , calculating this for the easy transfer of player to that postion/row 
	char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

	if (velocityY >= 0 &&(bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')) //velocityY is also added because it checks IF moving down AND there IS a platform
	{
		//to actually land on the platform
		player_y = (finalRow* cell_size) - Pheight;
		velocityY = 0;
		onGround = true;
	}
	else
	{
		//if no collision , keep moving
		player_y = offset_y;
		onGround = false;
	}

	if (!onGround) //applies gravity , if in air
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
void movement(bool &onGround , float &velocityY ,const float &jumpStrength ,bool &isJumping ,float &player_x , float &player_y , Sprite &PlayerSprite, float &speed , const int &cell_size , Texture &PlayerTexture , int &PlayerHeight, int selectedPlayer){
	if(Keyboard::isKeyPressed(Keyboard::Up)){
			if(onGround){
				velocityY = jumpStrength;
				onGround = false;
				isJumping = true;
			}
		}
	
		 if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            player_x -= speed;
            // LEFT FACING LOGIC
            if (selectedPlayer == 1) {
                // Player 1 Sprite Sheet coordinates
                PlayerSprite.setTextureRect(IntRect(10, 40, 35, 40)); 
            } else {
                // Player 2 Sprite Sheet coordinates
                PlayerSprite.setTextureRect(IntRect(10, 230, 35, 40));
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            player_x += speed;
            // RIGHT FACING (FLIPPED) LOGIC
            // Standard flip: X becomes (X + Width), Width becomes negative (-Width)
            if (selectedPlayer == 1) {
                PlayerSprite.setTextureRect(IntRect(45, 40, -35, 40)); // 10+35=45
            } else {
                PlayerSprite.setTextureRect(IntRect(45, 230, -35, 40)); // 10+35=45
            }
        }
		if(Keyboard :: isKeyPressed(Keyboard::Down)&& onGround && (player_y + PlayerHeight < 10 * cell_size)){  //last condition checks if the player is NOT on lowest row (player_Y + PlayerHeight means that the bottom of the player is being checked against the bottom most row instead of its any othe part )
			player_y += cell_size; //moves one total cell size down
			onGround = false; 		//is NOT on ground for that frame because its in air AND moving down
			velocityY = 0;			//vertical velocity should be zero for that while
		}
}
bool checkCollision(float player_x, float player_y, int player_width, int player_height, float enemy_x, float enemy_y, int enemy_width, int enemy_height){
    return (player_x < enemy_x + enemy_width && //checks for overlap from player's left side
			player_x + player_width> enemy_x && //''  '' '' '' right ''
            player_y < enemy_y + enemy_height&& // '' ''' '' top ''
            player_y + player_height  > enemy_y); //'' ''' '' bottom ''
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
	////game state variables
	 int state = 0;
	 int timer = 0; //timer variable for state changes  representing frames
     int selectedPlayer = 1; // 1 = Player 1, 2 = Player 2
	//level and background textures and sprites
	Texture bgstartTex;
	Sprite bgstartSprite;
	Texture playTex;
	Sprite PlaySprite;
	Texture playermenuTex;
	Sprite PlayermenuSprite;
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;
	Texture menutext1Tex;
	Sprite menutext1Sprite;
	Texture menutext2Tex;
	Sprite menutext2aSprite;
	Sprite menutext2bSprite;
    bgstartTex.loadFromFile("Data/tumblepop.png");
	bgstartSprite.setTexture(bgstartTex);
	bgstartSprite.setPosition(0,0);
	playermenuTex.loadFromFile("Data/menu.png");
	PlayermenuSprite.setTexture(playermenuTex);
	PlayermenuSprite.setPosition(0,0);
	PlayermenuSprite.setScale(3.6,3.2);

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);
	playTex.loadFromFile("Data/play.png");
	PlaySprite.setTexture(playTex);
	PlaySprite.setPosition(400,400);
	PlaySprite.setScale(0.7,0.4);


	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);
	menutext1Tex.loadFromFile("Data/text1.png");
	menutext1Sprite.setTexture(menutext1Tex);
	menutext1Sprite.setScale(0.7,0.4);
	menutext2Tex.loadFromFile("Data/text2.png");
	menutext2aSprite.setTexture(menutext2Tex);
	menutext2aSprite.setTextureRect(IntRect(16,41,1050,81));
	menutext2aSprite.setScale(0.2,0.2);
	menutext2bSprite.setTexture(menutext2Tex);
	menutext2bSprite.setTextureRect(IntRect(25,273,1050,81));
	menutext2bSprite.setScale(0.2,0.2);
	//Music initialisation
	Music lvlMusic;

	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	//player data
	float player_x = 50; //starting at bottom left hence x changed to 50 
	float player_y = 538; //and y changed to 10(bottom platform)* cell_size - P_height 

	float speed = 5;

	const float jumpStrength = -17; // Initial jump velocity (changed from -20 to -17 because long jump was causing irregular collision detection)
	const float gravity = 1;  // Gravity acceleration

	bool isJumping = false;  // Track if jumping

	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;

	Sprite PlayerSprite;
    Texture PlayerTexture;
	Sprite Player1Sprite;
  
	bool onGround = false;

	float offset_x = 0;
	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	int PlayerHeight =93;
	int PlayerWidth = 100;

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
    
    // --- SKELETON DATA ---
    const int NUM_SKELETONS = 5; 
    float skel_x[NUM_SKELETONS];
    float skel_y[NUM_SKELETONS];
    float skel_speed[NUM_SKELETONS];
    Sprite skeletons[NUM_SKELETONS]; 
    int skel_width = 32 * 2.5; //32
    int skel_height = 38 * 2.5;//38
    Texture skeletonTexture;
    skeletonTexture.loadFromFile("Data/skeleton.png");

    // --- GHOST DATA ---
    const int NUM_GHOSTS = 8;
    float ghost_x[NUM_GHOSTS];
    float ghost_y[NUM_GHOSTS];
    float ghost_speed[NUM_GHOSTS];
    Sprite ghosts[NUM_GHOSTS];
    int ghost_width = 35 * 2.5; 
    int ghost_height = 29 * 2.5; 
    Texture ghostTexture;
    ghostTexture.loadFromFile("Data/ghost.png");

    Texture Player2Texture;
	Sprite Player2Sprite;

	PlayerTexture.loadFromFile("Data/player2.png");
	Player1Sprite.setTexture(PlayerTexture);
	Player1Sprite.setTextureRect(IntRect(10,40,35, 40));
	Player1Sprite.setScale(2.5,2.5); 
	Player1Sprite.setPosition(player_x, player_y);
    Player2Texture.loadFromFile("Data/player2.png");
	Player2Sprite.setTexture(Player2Texture);
	Player2Sprite.setTextureRect(IntRect(10,230,35, 40));
	Player2Sprite.setScale(2.0,2.0);


	//creating level array
lvl = new char* [height];
for (int i = 0; i < height; i++)
{
    lvl[i] = new char[width];
}

// Level Design
    for (int j = 0; j < width; j++) lvl[10][j] = '#';   
    for (int j = 0; j < width; j++) if (j <=6 || j >=11) lvl[8][j] = '#';  
    for (int j = 0; j < width; j++) if (j!=0 && j!=1 && j!=2 && j!=16 &&j!=15 && j!=17) lvl[6][j] = '#'; 
    for (int j = 0; j < width; j++) if ((j>=0 && j<=6 || j >=10 && j<=17)) lvl[4][j] = '#';  
    for (int j = 0; j < width; j++) if (j!=0 && j!=1 && j!=2 && j!=16 &&j!=15 && j!=17) lvl[2][j] = '#'; 

    srand(time(0)); 

    // SKELETON SPAWN LOOP
    for(int i = 0; i < NUM_SKELETONS; i++)
    {
        skeletons[i].setTexture(skeletonTexture);
        skeletons[i].setTextureRect(IntRect(8,34,32,38));
        skeletons[i].setScale(-2.5, 2.5);
        
        if (rand() % 2 == 0){ 
			skel_speed[i] = 1.0f;
			 skeletons[i].setScale(-2.5, 2.5); // so that it faces right
		}
        else {

			skel_speed[i] = -1.0f;
			 skeletons[i].setScale(2.5, 2.5);
		}
        
        // FIXED CALL: Passing 'skel_height'
        spawn_enemy(skel_x[i], skel_y[i], skeletons[i], lvl, height, width, cell_size, skel_height);
    }

    // GHOST SPAWN LOOP
    for(int i = 0; i < NUM_GHOSTS; i++)
    {
        ghosts[i].setTexture(ghostTexture);
        ghosts[i].setTextureRect(IntRect(8,9,35,29));
        ghosts[i].setScale(-2.5, 2.5);
        
        if (rand() % 2 == 0) {
			
			ghost_speed[i] = 1.0f;
			ghosts[i].setScale(-2.5, 2.5); // so that it faces right
		}
        else {
			ghost_speed[i] = -1.0f;
			ghosts[i].setScale(2.5, 2.5);
		}



        // Passing 'ghost_height'
        spawn_enemy(ghost_x[i], ghost_y[i], ghosts[i], lvl, height, width, cell_size, ghost_height);
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
		
	    if(state == 0) //start state
		{
             window.draw(bgstartSprite); 
			 window.draw(PlaySprite); 
		
	     // check if timer is greater  10 (represent frames) and weather enter  key is pressed
            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                state = 1; ///represent menu state
              
            }
		window.display();
	}

	else if (state == 1)
	{
		 window.draw(PlayermenuSprite); 
	       // check if timer is greater  10 (represent frames) and weather enter  key is pressed
		   // DISPLAYING CHARACTERS IN MENu
         menutext1Sprite.setPosition(300, 200);
		 window.draw(menutext1Sprite);
		  menutext2aSprite.setPosition(270,600);
		 window.draw(menutext2aSprite);
		  menutext2bSprite.setPosition(625, 600);
		 window.draw(menutext2bSprite);
		 PlayerSprite.setScale(4.7, 4.7); 
         Player2Sprite.setScale(4.7, 4.7);


         PlayerSprite.setPosition(300, 400); 
         window.draw(PlayerSprite);
         Player2Sprite.setPosition(650, 395);
         window.draw(Player2Sprite);

            if (Keyboard::isKeyPressed(Keyboard::Num1) ) {
                state = 2; 
            
				selectedPlayer = 1;
				PlayerSprite.setScale(2.5, 2.5); 
                Player2Sprite.setScale(2.3, 2.3);
            }
			else if (Keyboard::isKeyPressed(Keyboard::Num2) ) {
                state = 2; 
            
				selectedPlayer = 2;
				PlayerSprite.setScale(2.5, 2.5); 
                Player2Sprite.setScale(2.3, 2.3);
            }
			if (selectedPlayer == 1) {
			PlayerSprite = Player1Sprite;
	       	} else {
			PlayerSprite = Player2Sprite;
		    }
		window.display();
	}
	    
        else if(state == 2) //gameplay state
		{

		movement(onGround, velocityY, jumpStrength, isJumping, player_x, player_y, PlayerSprite, speed, cell_size, PlayerTexture , PlayerHeight,selectedPlayer); //player movement function call
		window.clear();

		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		int Pwidth_scaled  = PlayerTexture.getSize().x * 3; //setting width according to player size , i.e multiplying by 3
		int Pheight_scaled = PlayerTexture.getSize().y * 3;
		player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		screenborder(player_x , player_y , PlayerWidth , velocityY , screen_x); //function for setting borders of the window
		PlayerSprite.setPosition(player_x, player_y);
		window.draw(PlayerSprite);
        
		 // update and draw skeletons
        for (int i = 0; i < NUM_SKELETONS; i++)
        {
            update_enemy_logic(skel_x[i], skel_y[i], skel_speed[i], skeletons[i], lvl, width, cell_size, skel_width, skel_height);
            if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight,skel_x[i], skel_y[i], skel_width, skel_height)){
        		//if collision occurs , resets player's position
				player_x = 50; 
				player_y = 538;
       		}
			window.draw(skeletons[i]);
        }

        // update and draw ghosts
        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            update_enemy_logic(ghost_x[i], ghost_y[i], ghost_speed[i], ghosts[i], lvl, width, cell_size, ghost_width, ghost_height);
            if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight,ghost_x[i], ghost_y[i], ghost_width, ghost_height)){
        		//if collision occurs , resets player's position
				player_x = 50; 
				player_y = 538;
       		}
			window.draw(ghosts[i]);
        }
		// enemy collision check 


		window.display();
	}
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

