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

void animate_ghost(Sprite &ghost, int frame)
{
    // animating ghost frames
   
    if (frame == 0)      ghost.setTextureRect(IntRect(  8, 9, 35, 29));
    else if (frame == 1) ghost.setTextureRect(IntRect( 57, 9, 35, 29));
    else if (frame == 2) ghost.setTextureRect(IntRect( 107, 9, 35, 29));
    else if (frame == 3) ghost.setTextureRect(IntRect(158, 9, 35, 29));
    else if (frame == 4) ghost.setTextureRect(IntRect(214, 9, 35, 29));
    else if (frame == 5) ghost.setTextureRect(IntRect(268, 9, 35, 29));
    else if (frame == 6) ghost.setTextureRect(IntRect(319, 9, 35, 29));
    else if (frame == 7) ghost.setTextureRect(IntRect(369, 9, 35, 29));
    
}

void animate_skeleton(Sprite &skele, int frame)
{
    // animating skeleton frames
   
    if (frame == 0)      skele.setTextureRect(IntRect(8,34,32,38));
    else if (frame == 1) skele.setTextureRect(IntRect(60,34,36,38));
    else if (frame == 2) skele.setTextureRect(IntRect(112,34,33,38));
    else if (frame == 3) skele.setTextureRect(IntRect(149,34,30,38));
    else if (frame == 4) skele.setTextureRect(IntRect(193,34,24,38));
    else if (frame == 5) skele.setTextureRect(IntRect(223,34,26,38));
    else if (frame == 6) skele.setTextureRect(IntRect(261,34,24,38));
    else if (frame == 7) skele.setTextureRect(IntRect(291,34,31,38));
    
}

void skele_platform_switch(
    int i,
    float& x, float& y,
    int& state, int& timer,
    float& startY, float& targetY,
    Sprite& skele,
    char** lvl, int height, int cell_size,
    int enemy_height)
{
    int col = (int)(x / cell_size);
    int feet_row = (int)((y + enemy_height) / cell_size);

    int jumpRows = 2; /// platforms are spaced 2 tiles apart

    //// CHECKING IF THERE IS PLATFORM ABOVE
    // We want a solid platform at (R-2)
    // and empty air above it (R-3) so skeleton fits.

    bool canGoUp = false;
    if (feet_row - jumpRows >= 0 && feet_row - jumpRows - 1 >= 0)
    {
        if (lvl[feet_row - jumpRows][col] == '#' &&      // platform exists
            lvl[feet_row - jumpRows - 1][col] != '#')    // space above is free
        {
            canGoUp = true;
        }
    }

   
    // CHECKING IF  PLATFORM BELOW
    // We want a solid platform at (R+2)
    // and empty air between (R+1) so skeleton can fall through.
 
    bool canGoDown = false;
    if (feet_row + jumpRows < height)
    {
        if (lvl[feet_row + jumpRows][col] == '#' &&   // platform below exists
            lvl[feet_row + 1][col] != '#')            // space under feet is empty
        {
            canGoDown = true;
        }
    }

    
    // STATE 0: NORMAL WALKING
 
    if (state == 0)
    {
        if (rand() % 300 == 0 && (canGoUp || canGoDown))
        {
            bool goUp;

            // RANDOM choice if both possible
            if (canGoUp && canGoDown)
                goUp = (rand() % 2 == 0);
            else if (canGoUp)
                goUp = true;
            else
                goUp = false;

            startY = y;

            if (goUp)
            {
                targetY = y - jumpRows * cell_size;
                state = 1;   // prep before jump
            }
            else
            {
                targetY = y + jumpRows * cell_size;
                state = 3;   // drop
            }

            timer = 0;
        }
    }

   
    // STATE 1: PREP POSE
   
    else if (state == 1)
    {
        timer++;

        // change sprite before jump
        skele.setTextureRect(IntRect(570, 38, 31, 34));

        if (timer > 25)
        {
            timer = 0;
            state = 2;
        }
    }

    // STATE 2: JUMP UP SLOWLY
   
    else if (state == 2)
    {
        timer++;

        float t = timer / 120.0f; // Slower jump
        if (t > 1) t = 1;

        y = startY + (targetY - startY) * t;

        if (timer >= 120)
        {
            y = targetY;
            state = 0;
            timer = 0;
        }
    }

    
    // STATE 3: DROP DOWN SLOWLY
    
    else if (state == 3)
    {
        timer++;

        float t = timer / 100.0f; // Slower drop
        if (t > 1) t = 1;

        y = startY + (targetY - startY) * t;

        if (timer >= 100)
        {
            y = targetY;
            state = 0;
            timer = 0;
        }
    }
}

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

// universal movement function
void update_enemy_logic(float& x, float& y, float& speed, Sprite& sprite, char** lvl, int width, int cell_size, int enemy_width, int enemy_height)
{
    x += speed;

    int feet_row = (y + enemy_height - 5) / cell_size; 
    int body_row = feet_row - 1; 

    if (speed > 0) // Moving RIGHT
    {
        int right_edge_px = x + enemy_width;
        int check_ahead_col = right_edge_px / cell_size;
        
        if (check_ahead_col >= width) {
            speed = -1.0f; //turn around, now moving left
            sprite.setScale(2.5, 2.5); //face left
        }
        else 
        {
            bool isWall = (lvl[body_row][check_ahead_col] == '#');
            bool isLedge = (lvl[feet_row][check_ahead_col] != '#');

            if (isWall || isLedge) {
                speed = -1.0f; //turns around
                sprite.setScale(2.5, 2.5); //face left
            }
        }
    }
    else // Moving LEFT
    {
        int left_edge_px = x - 5; //checks a few pixels in front of the left edge
        int check_ahead_col = left_edge_px / cell_size;
        
        if (left_edge_px <= 0) {
            speed = 1.0f; //turn around, now moving right 
            sprite.setScale(-2.5, 2.5); //face right
        }
        else if (check_ahead_col >= 0) 
        {
            bool isWall = (lvl[body_row][check_ahead_col] == '#');
            bool isLedge = (lvl[feet_row][check_ahead_col] != '#');

            if (isWall || isLedge) {
                speed = 1.0f; //turns around
                sprite.setScale(-2.5, 2.5); //face right
            }
        }
    }
    
    // Position sprite based on current direction   (this was done to invert the hitbox for the collision check)
    if (speed>=0) { //facing right
        sprite.setPosition(x + enemy_width, y);// when facing right, flipped sprite draws to the left, so add enemy_width
    }
    else { //facing left
        sprite.setPosition(x, y);
    }
}

/*2 problems were occuring with the orignal gravity function: 
1. Player's position isn't updated to the platform
2. and if it is , it is only updated when not colliding , hence some little changes are made 
*/
void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
    offset_y = player_y;
    offset_y += velocityY;
    int finalRow  = (int)(offset_y + Pheight) / cell_size ;     //the row , or platform , to which the player WILL be moved , calculating this for the easy transfer of player to that postion/row 
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
    if(player_y <0 ){   //top border
        player_y = 0;
        velocityY = 0;
    }
    if(player_x + playerwidth > screenx) //right border
        player_x = screen_x - playerwidth;
}

void movement(
    bool &onGround, float &velocityY, const float &jumpStrength, bool &isJumping, 
    float &player_x, float &player_y, Sprite &PlayerSprite, float &speed, 
    const int &cell_size, int &PlayerHeight, int selectedPlayer, int &walkFrame, 
    bool &facingRight, char &vacuumdir,
    Texture &P1Norm, Texture &P2Norm, Texture &P1Vac, Texture &P2Vac)
{
    // Checking if Vacuum is active
    bool isVacuuming = Keyboard::isKeyPressed(Keyboard::Space);


    if (isVacuuming) {
        if (selectedPlayer == 1) {
        PlayerSprite.setTexture(P1Vac);
         PlayerSprite.setScale(0.85, 0.85);
           }
            else
           { 
            PlayerSprite.setTexture(P2Vac);
        PlayerSprite.setScale(0.56, 0.56);
          } 
    } 
    else {
        if (selectedPlayer == 1) PlayerSprite.setTexture(P1Norm);
        else PlayerSprite.setTexture(P2Norm);
        PlayerSprite.setScale(0.40, 0.40);
    }

    // tracking vaccum direction
    if (Keyboard::isKeyPressed(Keyboard::W)) vacuumdir = 'W';
    if (Keyboard::isKeyPressed(Keyboard::S)) vacuumdir = 'S';
    if (Keyboard::isKeyPressed(Keyboard::A)) vacuumdir = 'A';
    if (Keyboard::isKeyPressed(Keyboard::D)) vacuumdir = 'D';


    // JUMPING
    if (Keyboard::isKeyPressed(Keyboard::Up)) {
        if (onGround) {
            velocityY = jumpStrength;
            onGround = false;
            isJumping = true;
        }
    }

    // LEFT MOVEMENT
    if (Keyboard::isKeyPressed(Keyboard::Left))
    {
        if (vacuumdir == 'D') vacuumdir = 'A'; 
        facingRight = false;
        player_x -= speed;

        // Only play walking animation if NOT using vacuum
        if (!isVacuuming) 
        {
            if (selectedPlayer == 1) {
                if (walkFrame == 0)      PlayerSprite.setTextureRect(IntRect(25, 39, 208, 261));
                else if (walkFrame == 1) PlayerSprite.setTextureRect(IntRect(260, 39, 208, 261));
                else if (walkFrame == 2) PlayerSprite.setTextureRect(IntRect(481, 39, 208, 261));
                else if (walkFrame == 3) PlayerSprite.setTextureRect(IntRect(702, 39, 208, 261));
                else if (walkFrame == 4) PlayerSprite.setTextureRect(IntRect(923, 39, 208, 261));
            } else {
                if (walkFrame == 0)      PlayerSprite.setTextureRect(IntRect(23, 56, 173, 218));
                else if (walkFrame == 1) PlayerSprite.setTextureRect(IntRect(219, 56, 173, 218));
                else if (walkFrame == 2) PlayerSprite.setTextureRect(IntRect(403, 56, 173, 218));
                else if (walkFrame == 3) PlayerSprite.setTextureRect(IntRect(587, 56, 173, 218));
                else if (walkFrame == 4) PlayerSprite.setTextureRect(IntRect(771, 56, 173, 218));
            }
        }
    }

    // RIGHT MOVEMENT
    if (Keyboard::isKeyPressed(Keyboard::Right))
    {
        if (vacuumdir == 'A') vacuumdir = 'D';
        facingRight = true;
        player_x += speed;

        // Only play walking animation if NOT using vacuum
        if (!isVacuuming)
        {
            if (selectedPlayer == 1) {
                if (walkFrame == 0)      PlayerSprite.setTextureRect(IntRect(233, 39, -208, 261));
                else if (walkFrame == 1) PlayerSprite.setTextureRect(IntRect(467, 39, -208, 261));
                else if (walkFrame == 2) PlayerSprite.setTextureRect(IntRect(688, 39, -208, 261));
                else if (walkFrame == 3) PlayerSprite.setTextureRect(IntRect(909, 39, -208, 261));
                else if (walkFrame == 4) PlayerSprite.setTextureRect(IntRect(1130, 39, -208, 261));
            } else {
                if (walkFrame == 0)      PlayerSprite.setTextureRect(IntRect(196, 56, -173, 218));
                else if (walkFrame == 1) PlayerSprite.setTextureRect(IntRect(392, 56, -173, 218));
                else if (walkFrame == 2) PlayerSprite.setTextureRect(IntRect(576, 56, -173, 218));
                else if (walkFrame == 3) PlayerSprite.setTextureRect(IntRect(760, 56, -173, 218));
                else if (walkFrame == 4) PlayerSprite.setTextureRect(IntRect(944, 56, -173, 218));
            }
        }
    }

    //  DOWN MOVEMENT
    if (Keyboard::isKeyPressed(Keyboard::Down) && onGround && (player_y + PlayerHeight < 10 * cell_size)) {
        player_y += cell_size;
        onGround = false;
        velocityY = 0;
    }

    // IDLE ANIMATION
    if (!Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right))
    {
        walkFrame = 0;
        // Only set Idle frame if NOT using vacuum
        if (!isVacuuming)
        {
            if (selectedPlayer == 1) {
                if (facingRight) PlayerSprite.setTextureRect(IntRect(233, 39, -208, 261));
                else PlayerSprite.setTextureRect(IntRect(25, 39, 208, 261));
            } else {
                if (facingRight) PlayerSprite.setTextureRect(IntRect(196, 56, -173, 218));
                else PlayerSprite.setTextureRect(IntRect(23, 56, 173, 218));
            }
        }
    }

    //  VACUUM ANIMATION 
    if (isVacuuming)
    {
        // PLAYER 1 VACUUM
        if (selectedPlayer == 1)
        {
            
            if      (vacuumdir == 'W') PlayerSprite.setTextureRect(IntRect(229, 17, 104, 226)); // UP
            else if (vacuumdir == 'S') PlayerSprite.setTextureRect(IntRect(356, 11, 85, 206)); // DOWN
            else if (vacuumdir == 'A') PlayerSprite.setTextureRect(IntRect(23, 231, 226, 122)); // LEFT
            else if (vacuumdir == 'D') PlayerSprite.setTextureRect(IntRect(407, 231, 230, 133)); // RIGHT 
        }
        // PLAYER 2 VACUUM
        else 
        {
            if      (vacuumdir == 'W') PlayerSprite.setTextureRect(IntRect(96, 20, 144, 297)); // UP
            else if (vacuumdir == 'S') PlayerSprite.setTextureRect(IntRect(275, 15, 107, 274)); // DOWN
            else if (vacuumdir == 'A') PlayerSprite.setTextureRect(IntRect(7, 338, 218, 167)); // LEFT
            else if (vacuumdir == 'D') PlayerSprite.setTextureRect(IntRect(240, 338, 218, 166)); // RIGHT
        }
    }
}

bool checkCollision(float player_x, float player_y, int player_width, int player_height, float enemy_x, float enemy_y, int enemy_width, int enemy_height, int &playerHealth){
    if(player_x < enemy_x + enemy_width && //checks for overlap from player's left side
            player_x + player_width> enemy_x && //''  '' '' '' right ''
            player_y < enemy_y + enemy_height&& // '' ''' '' top ''
            player_y + player_height  > enemy_y) //'' ''' '' bottom ''
            {
                playerHealth--;
                return true;
            } 
    else
        return false;
}

void vacuumMovement(char &vacuumdir){
    if(Keyboard::isKeyPressed(Keyboard::W)) vacuumdir = 'W';
    if(Keyboard::isKeyPressed(Keyboard::D)) vacuumdir = 'D';
    if(Keyboard::isKeyPressed(Keyboard::S)) vacuumdir = 'S';
    if(Keyboard::isKeyPressed(Keyboard::A)) vacuumdir = 'A';
}

void vacuumRangeCheck(float player_x, float player_y, char vacuumdir, float enemy_x[], float enemy_y[], int enemy_capture_state[], int num_enemies , int enemy_in_vacuum, int levelstorecap) //this is to modify enemy_state[i] to use them for suction later
{
    if (Keyboard::isKeyPressed(Keyboard::Space)) 
    {
        if (enemy_in_vacuum >= levelstorecap) return; //level cap for storing enemies (hence vacuum wont store any further than that)
        for (int i = 0; i < num_enemies; i++) //checks for all enemies
        {
            if (enemy_capture_state[i] == 0)  //means walking
            {
                bool inRange = false;

                if (vacuumdir == 'D') { // right
                    if (enemy_x[i] > player_x && enemy_x[i] < player_x + 150 && abs(player_y - enemy_y[i]) < 80) {
                        inRange = true;
                    }
                } 
                else if (vacuumdir == 'A') { // left
                    if (enemy_x[i] > player_x - 150 && enemy_x[i] < player_x && abs(player_y - enemy_y[i]) < 80) {
                        inRange = true;
                    }
                }
                else if (vacuumdir == 'W') { // up
                    //  y must be above player (player_y - 150 to player_y)
                    //  x must be close (abs(player_x - enemy_x[i]) < 80)
                    if ( enemy_y[i] > player_y - 150 && enemy_y[i] < player_y && abs(player_x - enemy_x[i]) < 80 ) {
                        inRange = true;
                    }
                }
                else if (vacuumdir == 'S') { // down
                     //  y must be below player (player_y to player_y + 150)
                     //  x must be close
                    if ( enemy_y[i] > player_y && enemy_y[i] < player_y + 150 && abs(player_x - enemy_x[i]) < 80 ) {
                        inRange = true;
                    }
                }

                if (inRange) {
                        enemy_capture_state[i] = 1; //being sucked
                }
            }
        }
    }
}
void adjustVacuumSprites(Sprite& sprite, char vacuumdir, int selectedPlayer)
{
    // We will  only apply offsets if the player is holding Space (Vacuuming)
    if (Keyboard::isKeyPressed(Keyboard::Space))
    {
        // PLAYER 1 
        if (selectedPlayer == 1)
        {
            if (vacuumdir == 'D') 
            {
                
                sprite.move(0, 0); 
            }
            else if (vacuumdir == 'S') 
            {
                
                sprite.move(0, 0); 
            }
            else if (vacuumdir == 'A') 
            {
                //Needs significant shift for P1
                sprite.move(-110, 0);
            }
            else if (vacuumdir == 'W')
            {
                // Needs shift up for P1
                sprite.move(0, -90);
            }
        }
        
        else 
        {
            // Player 2 
            if (vacuumdir == 'D') 
            {
                sprite.move(0, 0); 
            }
            else if (vacuumdir == 'S') 
            {
                sprite.move(0, 0); 
            }
            else if (vacuumdir == 'A') 
            {
                // Left shift for Player 2
                sprite.move(-60, 0); 
            }
            else if (vacuumdir == 'W')
            {
                // Up shift for Player 2
                sprite.move(0, -70); 
            }
        }
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
    
    ////game state variables
    int state = 0;
    int timer = 0; //timer variable for state changes  representing frames
    int walkFrame = 0;   // 0..3 for player  animation
    int enemyFrame = 0;  // 0..8 for ghost animation
    int selectedPlayer = 1; // 1 = Player 1, 2 = Player 2
    bool facingRight = true; // to track which way the player is facing
    int playerHealth = 3; 

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

    bgTex.loadFromFile("Data/bg3.jpeg"); 
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);
    playTex.loadFromFile("Data/play.png");
    PlaySprite.setTexture(playTex);
    PlaySprite.setPosition(400,400);
    PlaySprite.setScale(0.7,0.4);


    blockTexture.loadFromFile("Data/block3.png"); 
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

    const float jumpStrength = -16; // Initial jump velocity (changed from -20 to -16 because long jump was causing irregular collision detection)
    const float gravity = 1;  // Gravity acceleration

    bool isJumping = false;  // Track if jumping

    bool up_collide = false;
    bool left_collide = false;
    bool right_collide = false;

    
    bool onGround = false;

    float offset_x = 0;
    float offset_y = 0;
    float velocityY = 0;

    float terminal_Velocity = 20;

    int PlayerHeight;
    int PlayerWidth ;
   
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
    
    // User's Jumping State
    int   skelPlatformState[NUM_SKELETONS] = {0};   // 0=normal 1=prep 2=jumping up 3=dropping down
    int   skelTimer[NUM_SKELETONS] = {0};   // counts frames inside state
    float skelStartY[NUM_SKELETONS];        // where jump started
    float skelTargetY[NUM_SKELETONS];       // where jump ends

    // --- GHOST DATA ---
    const int NUM_GHOSTS = 8;
    float ghost_x[NUM_GHOSTS];
    float ghost_y[NUM_GHOSTS];
    float ghost_speed[NUM_GHOSTS];
    int ghostPause[NUM_GHOSTS] = {0};  // 0 = not paused  >0 = paused frames left
    Sprite ghosts[NUM_GHOSTS];
    int ghost_width = 35 * 2.5; 
    int ghost_height = 29 * 2.5; 
    Texture ghostTexture;
    ghostTexture.loadFromFile("Data/ghost.png");

    //player sprites
    Texture PlayerTexture;
    Sprite PlayerSprite;
    Texture Player1Texture;
    Sprite Player1Sprite;
  
    Texture Player2Texture;
    Sprite Player2Sprite;
	//VACUUM TEXTURES
    Texture Player1VacTex;
    Texture Player2VacTex;
    

    Player1VacTex.loadFromFile("Data/player1_vacuum.png"); 
    Player2VacTex.loadFromFile("Data/player2_vacuum.png");

    Player1Texture.loadFromFile("Data/player1.png");
    Player1Sprite.setTexture(Player1Texture);
    Player1Sprite.setTextureRect(IntRect(25,39,208, 261));
    Player1Sprite.setPosition(player_x, player_y);
    Player2Texture.loadFromFile("Data/player2.png");
    Player2Sprite.setTexture(Player2Texture);
    Player2Sprite.setTextureRect(IntRect(23,56,173, 218));

    //vacuum data
    char vacuumdir = 'D' ; //d for right , w for up , and so on , d is default cause facing right
    float suctionspeed = 6.0f;
    int enemies_in_vacuum = 0;
    const int levelstorecap = 3; //initally set for level 1
    
    // Capture States: 0=Walking, 1=Being Sucked, 2=Captured
    int skel_capture_state[NUM_SKELETONS] = {0}; //0 for walking , 1 for being sucked , 2 for completely sucked (i.e , not there anymore) , default is 0(walking)
    int ghost_capture_state[NUM_GHOSTS] = {0};
    int vacuumStorage[levelstorecap]; //currently for level 1 only


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
        } else {
            skel_speed[i] = -1.0f;
             skeletons[i].setScale(2.5, 2.5);
        }
        
        // FIXED CALL: Passing 'skel_height'
        spawn_enemy(skel_x[i], skel_y[i], skeletons[i], lvl, height, width, cell_size, skel_height);
        skel_capture_state[i] = 0;
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
        } else {
            ghost_speed[i] = -1.0f;
            ghosts[i].setScale(2.5, 2.5);
        }

        // Passing 'ghost_height'
        spawn_enemy(ghost_x[i], ghost_y[i], ghosts[i], lvl, height, width, cell_size, ghost_height);
        ghost_capture_state[i] = 0;
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
            
             // DISPLAYING CHARACTERS IN MENu
             menutext1Sprite.setPosition(300, 200);
             window.draw(menutext1Sprite);
             menutext2aSprite.setPosition(270,600);
             window.draw(menutext2aSprite);
             menutext2bSprite.setPosition(625, 600);
             window.draw(menutext2bSprite);
             Player1Sprite.setScale(0.65, 0.65); 
             Player2Sprite.setScale(0.8, 0.8);


             PlayerSprite.setPosition(300, 400); 
             window.draw(PlayerSprite);
             Player2Sprite.setPosition(650, 395);
             window.draw(Player2Sprite);

            if (Keyboard::isKeyPressed(Keyboard::Num1) ) {
                state = 2; 
            
                selectedPlayer = 1;
                Player1Sprite.setScale(0.38, 0.38); 
                Player2Sprite.setScale(0.41, 0.41); 
                //SET SIZE FOR PLAYER 1
                PlayerWidth  = 208 * 0.38;   // = 79
                PlayerHeight = 261 * 0.38;   // = 99
            }
            else if (Keyboard::isKeyPressed(Keyboard::Num2) ) {
                state = 2; 
            
                selectedPlayer = 2;
                Player1Sprite.setScale(0.38, 0.38); 
                Player2Sprite.setScale(0.41, 0.41); 

                //SET SIZE FOR PLAYER 2
                PlayerWidth  = 173 * 0.41;   // = 71
                PlayerHeight = 218 * 0.41;   // = 89
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
         // advance animation frame every 10 game frames
            timer++;
            if (timer > 9)           //  this number controls  animation speed faster/slower
            {
                walkFrame = (walkFrame + 1) % 5;   // 0,1,2,3,4 loop
                enemyFrame = (enemyFrame + 1) % 8;   // 0,1,2,3 .... ,8 loop
                timer = 0;
            }

            vacuumMovement(vacuumdir);
            vacuumRangeCheck(player_x, player_y, vacuumdir, skel_x, skel_y, skel_capture_state, NUM_SKELETONS , enemies_in_vacuum , levelstorecap); //for skeleton
            vacuumRangeCheck(player_x, player_y, vacuumdir, ghost_x, ghost_y, ghost_capture_state, NUM_GHOSTS , enemies_in_vacuum  , levelstorecap); //for ghost
            
          movement(onGround, velocityY, jumpStrength, isJumping, player_x, player_y, PlayerSprite, speed, cell_size, PlayerHeight, selectedPlayer, 
         walkFrame, facingRight, vacuumdir, Player1Texture, Player2Texture, Player1VacTex, Player2VacTex);
            window.clear();
            display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
            int Pwidth_scaled  = PlayerTexture.getSize().x * 0.40; //setting width according to player size 
            int Pheight_scaled = PlayerTexture.getSize().y * 0.40;
            player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
            screenborder(player_x , player_y , PlayerWidth , velocityY , screen_x); //function for setting borders of the window
            PlayerSprite.setPosition(player_x, player_y);
            // Apply the visual offset function
          adjustVacuumSprites(PlayerSprite, vacuumdir, selectedPlayer);
            window.draw(PlayerSprite);
        
            // update and draw skeletons
            for (int i = 0; i < NUM_SKELETONS; i++)
            {
                // process everyone who is not captured
                if (skel_capture_state[i] != 2) 
                {
                    // checking if walking
                    if (skel_capture_state[i] == 0) // Normal Behavior
                    {
                        // only move left/right when NOT jumping or prepping
                        if (skelPlatformState[i] == 0)
                        {
                            update_enemy_logic(skel_x[i], skel_y[i], skel_speed[i],
                                            skeletons[i], lvl, width, cell_size,
                                            skel_width, skel_height);
                        }

                        // slow jump/drop behavior
                        skele_platform_switch(
                            i, skel_x[i], skel_y[i],
                            skelPlatformState[i], skelTimer[i],
                            skelStartY[i], skelTargetY[i],
                            skeletons[i],
                            lvl, height, cell_size, skel_height
                        );

                        // normal walking animation ONLY if not in prep pose
                        // ONLY animate while walking on platform
                        if (skelPlatformState[i] == 0) animate_skeleton(skeletons[i], enemyFrame);
                    }
                    //  if not , then start sucking
                    else if (skel_capture_state[i] == 1) // Being Sucked
                    {
                        /*the movement logic below 
                        is temporary  and shall be replaced by animation later on*/
                        //Move X-axis
                        if (skel_x[i] < player_x) skel_x[i] += suctionspeed;
                        else skel_x[i] -= suctionspeed;

                        //move Y-axis
                        if (skel_y[i] < player_y) skel_y[i] += suctionspeed;
                        else skel_y[i] -= suctionspeed;
                        
                        //update the location  
                        skeletons[i].setPosition(skel_x[i], skel_y[i]);

                        // storing in the inventory
                        if (abs(player_x - skel_x[i]) < 10 && abs(player_y - skel_y[i]) < 10) 
                        {
                            if (enemies_in_vacuum < 3) {
                                skel_capture_state[i] = 2; // capture them
                                enemies_in_vacuum++; //adds to space
                            } else {
                                //not captured , walk..
                                skel_capture_state[i] = 0; 
                            }
                        }
                        animate_skeleton(skeletons[i], enemyFrame);
                    }

                    //player only dies if the enemy is not being sucked
                    if (skel_capture_state[i] == 0 && checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, skel_x[i], skel_y[i], skel_width, skel_height, playerHealth))
                    {
                        player_x = 50;
                        player_y = 538;
                    }
                    if(playerHealth <= 0) state = 3;
                    
                    window.draw(skeletons[i]);
                }
            }

            // update , draw  , and check suction for ghosts
            for (int i = 0; i < NUM_GHOSTS; i++)
            {
                // process everyone who is not captured
                if (ghost_capture_state[i] != 2) // Captured
                {
                    // checking if walking
                    if (ghost_capture_state[i] == 0) // Normal
                    {
                        //  PAUSE / STOP LOGIC
                        // If ghostPause[i] > 0 that ghost is currently frozen.
                        if (ghostPause[i] > 0)
                        {
                            ghostPause[i]--;   // count down one frame of pause

                            // When the pause finishes (counter reaches 0),
                            // we flip direction ONCE.
                            if (ghostPause[i] == 0)
                            {
                                ghost_speed[i] = -ghost_speed[i];  // reverse movement direction

                                // Update sprite facing to match new direction.
                                // +speed means moving right 
                                if (ghost_speed[i] > 0)
                                    ghosts[i].setScale(-2.5, 2.5); // face right
                                else
                                    ghosts[i].setScale( 2.5, 2.5); // face left
                            }
                        }
                        // If not paused ghost moves normally
                        else
                        {
                            // Random chance to start a pause.
                            // means  1 in 300 frames (about every 5 sec at 60fps).
                            if (rand() % 300 == 0)
                            {
                                ghostPause[i] = 120;  // freeze for 120 frames ( 2 seconds)
                            }
                            else
                            {
                                // Normal patrolling movement 
                                update_enemy_logic(ghost_x[i], ghost_y[i], ghost_speed[i],
                                    ghosts[i], lvl, width, cell_size, ghost_width, ghost_height);
                            }
                        }
                        
                        if (ghost_speed[i] >= 0)
                            ghosts[i].setPosition(ghost_x[i] + ghost_width, ghost_y[i]);
                        else
                            ghosts[i].setPosition(ghost_x[i], ghost_y[i]);
                    }
                    //  if not , then start sucking
                    else if (ghost_capture_state[i] == 1) // Being Sucked
                    {
                        /*the movement logic below 
                        is temporary  and shall be replaced by animation later on*/
                        //Move X-axis
                        if (ghost_x[i] < player_x) ghost_x[i] += suctionspeed;
                        else ghost_x[i] -= suctionspeed;

                        //move Y-axis
                        if (ghost_y[i] < player_y) ghost_y[i] += suctionspeed;
                        else ghost_y[i] -= suctionspeed;
                        
                        //update the location
                        ghosts[i].setPosition(ghost_x[i], ghost_y[i]);

                        // storing in the inventory
                        if (abs(player_x - ghost_x[i]) < 10 && abs(player_y - ghost_y[i]) < 10) 
                        {
                            if (enemies_in_vacuum < 3) {
                                ghost_capture_state[i] = 2; // capture them
                                enemies_in_vacuum++; // adds to space
                            } else {
                                // not captured , walk..
                                ghost_capture_state[i] = 0; 
                            } 
                        }
                    }

                    // ANIMATION
                    animate_ghost(ghosts[i], enemyFrame);

                    //COLLISION WITH PLAYER
                    // player only dies , if not being sucked
                    // If ghost hitbox overlaps player hitbox, reset player.
                    if (ghost_capture_state[i] == 0 && checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, ghost_x[i], ghost_y[i], ghost_width, ghost_height, playerHealth))
                    {
                        player_x = 50;   // respawn X
                        player_y = 538;  // respawn Y
                    }
                    if(playerHealth <= 0) state = 3;

                    window.draw(ghosts[i]);
                }
            }
            window.display();
        }
        else if(state == 3) //gameover state
        {
            window.close();
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
