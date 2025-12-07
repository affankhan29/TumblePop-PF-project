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


///////////////////////////////////////////////////////////////////////////
/////                added death animation function                 ///////
/////                                                              ///////
//////////////////////////////////////////////////////////////////////////  


void animate_invisible_man(Sprite& sprite, int frame)
{
    // Modulo 6 ensures the frame cycle stays within 0-5
    int f = frame % 6; 

    // Y is 18, Width is 26, Height is 36 (based on your initialization)
    if (f == 0)      sprite.setTextureRect(IntRect(8, 18, 26, 36));
    else if (f == 1) sprite.setTextureRect(IntRect(76, 18, 26, 36));
    else if (f == 2) sprite.setTextureRect(IntRect(115, 18, 26, 36));  // Estimated X
    else if (f == 3) sprite.setTextureRect(IntRect(157, 18, 26, 36)); // Estimated X
    else if (f == 4) sprite.setTextureRect(IntRect(190, 18, 26, 36)); // Estimated X
    else             sprite.setTextureRect(IntRect(226, 18, 26, 36)); // Estimated X
}


void animate_chelnov(Sprite &chel, int frame)
{

    if (frame == 0)      chel.setTextureRect(IntRect(12,  4, 26, 36));
    else if (frame == 1) chel.setTextureRect(IntRect(41, 5, 27, 37));
    else if (frame == 2) chel.setTextureRect(IntRect(77, 6, 23, 36));
    else if (frame == 3) chel.setTextureRect(IntRect(106, 6, 29, 36));
    else if (frame == 4) chel.setTextureRect(IntRect(141,  5, 23, 37)); 
    else                 chel.setTextureRect(IntRect(171, 5, 24, 37));
}

void animate_death(Sprite &playerDeath, int frame, int selectedPlayer)
{
    // PLAYER 1 DEATH FRAMES
    if (selectedPlayer == 1)
    {
        if (frame == 0)      playerDeath.setTextureRect(IntRect(32, 8, 112, 156)); 
        else if (frame == 1) playerDeath.setTextureRect(IntRect(199, 8, 115, 149));
        else if (frame == 2) playerDeath.setTextureRect(IntRect(373, 8, 111, 146));
        else if (frame == 3) playerDeath.setTextureRect(IntRect(369, 10, 111, 146));
        else if (frame == 4) playerDeath.setTextureRect(IntRect(553, 8, 101, 150));
        else if (frame == 5) playerDeath.setTextureRect(IntRect(723, 8, 101, 150));
        else if (frame == 6) playerDeath.setTextureRect(IntRect(894, 8, 100, 150));
        else if (frame == 7) playerDeath.setTextureRect(IntRect(1074, 8, 76, 150));
        else if (frame == 8) playerDeath.setTextureRect(IntRect(1196, 8, 24, 150));
    }
    // PLAYER 2 DEATH FRAMES
    else if (selectedPlayer == 2)
    {

        if (frame == 0)      playerDeath.setTextureRect(IntRect(28, 10, 112, 157)); 
        else if (frame == 1) playerDeath.setTextureRect(IntRect(195, 0, 50, 50));
        else if (frame == 2) playerDeath.setTextureRect(IntRect(100, 10, 115, 150));
        else if (frame == 3) playerDeath.setTextureRect(IntRect(369, 10, 111, 146));
        else if (frame == 4) playerDeath.setTextureRect(IntRect(549, 10, 101, 150));
        else if (frame == 5) playerDeath.setTextureRect(IntRect(719, 10, 101, 150));
        else if (frame == 6) playerDeath.setTextureRect(IntRect(890, 10, 100, 150));
        else if (frame == 7) playerDeath.setTextureRect(IntRect(1070, 10, 76, 150));
        else if (frame == 8) playerDeath.setTextureRect(IntRect(1192, 10, 24, 150));
    }
}



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

    int jumpRows = 2; /// platforms are spaced 2 cells apart

    //// CHECKING IF THERE IS PLATFORM ABOVE
    // We want a solid platform at (R-2)
    // and empty air above it (R-3) so skeleton fits.

    bool canGoUp = false;
    if (feet_row - jumpRows >= 0 && feet_row - jumpRows - 1 >= 0)
    {
        if (lvl[feet_row - jumpRows][col] == '#' && lvl[feet_row - jumpRows][col] == '@'&&     // platform exists
            lvl[feet_row - jumpRows - 1][col] != '#')    // space above is free
        {
            canGoUp = true;
        }
    }

   
    // CHECKING IF  PLATFORM BELOW
    // We want a solid platform at (R+2)
    // and empty air between (R+1) so skeleton can fall through.
 
    bool canGoDown = false;
    if (feet_row + jumpRows < height&& (feet_row + jumpRows)!=10)
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

        // changing sprite before jump
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

        float t = timer / 120.0f; 
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

        float t = timer / 100.0f;
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

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite,Sprite& slantedSprite, const int height, const int width, const int cell_size)
{
    window.draw(bgSprite);

    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {

            if (lvl[i][j] == '#'|| lvl[i][j] == '@')
            {
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
           else  if (lvl[i][j] == '&')
            {
                slantedSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(slantedSprite);
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

       if (lvl[r][c] != '#' && lvl[r][c] != '@' && lvl[r][c] != '&' && // Ensure not spawning INSIDE a block
            (lvl[r + 1][c] == '#' || lvl[r + 1][c] == '@') &&           // Ensure floor is solid
            lvl[r + 1][c] != '&' &&                                     // Ensure floor is NOT a slope
            (r + 1) != 10)
        {
            x = c * cell_size;
            // (r + 1) * cell_size is the floor line
            // We subtract height to find the top position
            // We add +10 to push the feet slightly INTO the floor block for better collision
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
           // Check if wall is '#' OR '@'
            bool isWall = (lvl[body_row][check_ahead_col] == '#' || lvl[body_row][check_ahead_col] == '@');
            
            // Ledge exists if floor is NOT '#' AND NOT '@' AND NOT '&'
            bool isLedge = (lvl[feet_row][check_ahead_col] != '#' && lvl[feet_row][check_ahead_col] != '&');
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
           // Check if wall is '#' OR '@'
            bool isWall = (lvl[body_row][check_ahead_col] == '#' || lvl[body_row][check_ahead_col] == '@');
            
            // Ledge exists if floor is NOT '#' AND NOT '@' AND NOT '&'
            bool isLedge = (lvl[feet_row][check_ahead_col] != '#' && lvl[feet_row][check_ahead_col] != '&');
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
    // Predict where the player will be in the next frame
    offset_y = player_y;
    offset_y += velocityY;
    
    // Calculate which row the player's feet are falling into
    int finalRow  = (int)(offset_y + Pheight) / cell_size ;
    
    // Check 3 points at the bottom of the player (Left, Center, Right) to ensure they don't fall off edge too early
    char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
    char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
    char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

    // Helper logic: Is there any floor beneath us? 
    // We treat '#' (platforms) AND '@' (walls) as valid floors to stand on.
    bool isFloorLeft = (bottom_left_down == '#' || bottom_left_down == '@');
    bool isFloorRight = (bottom_right_down == '#' || bottom_right_down == '@');
    bool isFloorMid = (bottom_mid_down == '#' || bottom_mid_down == '@');

    // If we are falling down (velocityY >= 0) AND we hit a floor...
    if (velocityY >= 0 && (isFloorLeft || isFloorMid || isFloorRight))
    {
        // SNAP to the top of the platform so we don't vibrate or sink in
        player_y = (finalRow* cell_size) - Pheight;
        velocityY = 0;  // Stop falling
        onGround = true; // Tell the rest of the code we are safe on the ground
    }
    else
    {
        // No floor detected, keep applying the movement
        player_y = offset_y;
        onGround = false;
    }

    // Apply Gravity Acceleration if in the air
    if (!onGround)
    {
        velocityY += gravity;
        // Cap the speed so we don't fall infinitely fast (Terminal Velocity)
        if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
    }
    else
    {
        // If on ground, reset vertical speed
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
    const int &cell_size, int &PlayerHeight, int &PlayerWidth, int selectedPlayer, int &walkFrame, 
    bool &facingRight, char &vacuumdir,
    Texture &P1Norm, Texture &P2Norm, Texture &P1Vac, Texture &P2Vac,
    char** currentLvl, int width, int height) 
{
    // Checking if Vacuum is active (holding Space)
    bool isVacuuming = Keyboard::isKeyPressed(Keyboard::Space);

    // -- Texture Management --
    // Swap textures based on whether we are vacuuming or just standing/walking
    if (isVacuuming) {
        if (selectedPlayer == 1) {
            PlayerSprite.setTexture(P1Vac);
            PlayerSprite.setScale(0.85, 0.85); // P1 needs a specific scale
        } else { 
            PlayerSprite.setTexture(P2Vac);
            PlayerSprite.setScale(0.56, 0.56); // P2 is a bit smaller
        } 
    } else {
        // Normal textures when not sucking up enemies
        if (selectedPlayer == 1) PlayerSprite.setTexture(P1Norm);
        else PlayerSprite.setTexture(P2Norm);
        PlayerSprite.setScale(0.40, 0.40);
    }

    // Tracking vacuum direction for the hitbox logic later
    if (Keyboard::isKeyPressed(Keyboard::W)) vacuumdir = 'W';
    if (Keyboard::isKeyPressed(Keyboard::S)) vacuumdir = 'S';
    if (Keyboard::isKeyPressed(Keyboard::A)) vacuumdir = 'A';
    if (Keyboard::isKeyPressed(Keyboard::D)) vacuumdir = 'D';

    // -- JUMPING LOGIC --
    if (Keyboard::isKeyPressed(Keyboard::Up)) {
        // Can only jump if we are actually touching the floor
        if (onGround) {
            velocityY = jumpStrength; // Apply upward force (negative Y)
            onGround = false;
            isJumping = true;
        }
    }

    // -- LEFT MOVEMENT --
    if (Keyboard::isKeyPressed(Keyboard::Left))
    {
        if (vacuumdir == 'D') vacuumdir = 'A'; // Face left
        facingRight = false;
        
        float next_x = player_x - speed;
        bool collide = false;

        // COLLISION CHECK
        // We only care about walls if we are falling or walking on the ground (velocityY >= 0).
        // If we are jumping UP, we ignore walls mostly to allow smooth movement.
        if (velocityY >= 0) 
        {
            // Calculate which grid cell the player is trying to move into
            int col = (int)(next_x) / cell_size;
            int rowTop = (int)(player_y + 10) / cell_size; 
            int rowBot = (int)(player_y + PlayerHeight - 5) / cell_size;
            
            // Check boundaries so game doesn't crash
            if (col >= 0 && col < width && rowTop >= 0 && rowBot < height) {
                
                // *** THE FIX IS HERE ***
                // We ONLY stop the player if they hit a SOLID WALL ('@').
                // If they hit a platform ('#'), we ignore it so they can walk past it or jump through.
                if (currentLvl[rowTop][col] == '@' || currentLvl[rowBot][col] == '@') {
                    collide = true;
                }
            }
        }

        if (!collide) {
            player_x = next_x; // Path is clear, move the player
        } else {
             // We hit a wall, so snap the player to the edge of the block 
             // This prevents them from getting stuck inside the wall
             int col = (int)(next_x) / cell_size;
             player_x = (col + 1) * cell_size;
        }

        // ANIMATION: Only play walking animation if NOT using vacuum
        if (!isVacuuming) {
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

    // -- RIGHT MOVEMENT --
    if (Keyboard::isKeyPressed(Keyboard::Right))
    {
        if (vacuumdir == 'A') vacuumdir = 'D'; // Face right
        facingRight = true;
        
        float next_x = player_x + speed;
        bool collide = false;

        if (velocityY >= 0) 
        {
            // Checking the right side of the player
            int col = (int)(next_x + PlayerWidth) / cell_size;
            int rowTop = (int)(player_y + 10) / cell_size; 
            int rowBot = (int)(player_y + PlayerHeight - 5) / cell_size;

            if (col >= 0 && col < width && rowTop >= 0 && rowBot < height) {
                // *** FIX: ONLY COLLIDE WITH SOLID WALLS ('@') ***
                if (currentLvl[rowTop][col] == '@' || currentLvl[rowBot][col] == '@') {
                    collide = true;
                }
            }
        }

        if (!collide) {
            player_x = next_x; // Move freely
        } else {
             // Snap to the left side of the wall we just hit
             int col = (int)(next_x + PlayerWidth) / cell_size;
             player_x = (col * cell_size) - PlayerWidth;
        }

        // Animation logic (standard walking cycle)
        if (!isVacuuming) {
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

    // -- DOWN MOVEMENT (Falling through platforms) --
    if (Keyboard::isKeyPressed(Keyboard::Down) && onGround && (player_y + PlayerHeight < 10 * cell_size)) {
        player_y += cell_size; // Force player down one block
        onGround = false;      // Now gravity will take over
        velocityY = 0;
    }

    // -- IDLE ANIMATION --
    if (!Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right))
    {
        walkFrame = 0; // Reset animation to standing
        if (!isVacuuming) {
            if (selectedPlayer == 1) {
                if (facingRight) PlayerSprite.setTextureRect(IntRect(233, 39, -208, 261));
                else PlayerSprite.setTextureRect(IntRect(25, 39, 208, 261));
            } else {
                if (facingRight) PlayerSprite.setTextureRect(IntRect(196, 56, -173, 218));
                else PlayerSprite.setTextureRect(IntRect(23, 56, 173, 218));
            }
        }
    }

    // -- VACUUM ANIMATION (Directional) --
    if (isVacuuming)
    {
        // Adjust texture rects based on which way we are pointing the vacuum
        if (selectedPlayer == 1) {
            if      (vacuumdir == 'W') PlayerSprite.setTextureRect(IntRect(229, 17, 104, 226));
            else if (vacuumdir == 'S') PlayerSprite.setTextureRect(IntRect(356, 11, 85, 206));
            else if (vacuumdir == 'A') PlayerSprite.setTextureRect(IntRect(23, 231, 226, 122));
            else if (vacuumdir == 'D') PlayerSprite.setTextureRect(IntRect(407, 231, 230, 133));
        } else {
            if      (vacuumdir == 'W') PlayerSprite.setTextureRect(IntRect(96, 20, 144, 297));
            else if (vacuumdir == 'S') PlayerSprite.setTextureRect(IntRect(275, 15, 107, 274));
            else if (vacuumdir == 'A') PlayerSprite.setTextureRect(IntRect(7, 338, 218, 167));
            else if (vacuumdir == 'D') PlayerSprite.setTextureRect(IntRect(240, 338, 218, 166));
        }
    }
}
  
///////////////////////////////////////////////////////////////////////////
/////                updated check collison                         ///////
/////                                                              ///////
//////////////////////////////////////////////////////////////////////////     

bool checkCollision(float player_x, float player_y, int player_width, int player_height, float enemy_x, float enemy_y, int enemy_width, int enemy_height){
    if(player_x < enemy_x + enemy_width && //checks for overlap from player's left side
            player_x + player_width> enemy_x && //''  '' '' '' right ''
            player_y < enemy_y + enemy_height&& // '' ''' '' top ''
            player_y + player_height  > enemy_y) //'' ''' '' bottom ''
            {
                // playerHealth--;  <<<<< REMOVED  Health is now decreased in main loop after animation
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

void vacuumRangeCheck(float player_x, float player_y, char vacuumdir, float enemy_x[], float enemy_y[], int enemy_capture_state[], int num_enemies , int enemy_in_vacuum, int levelstorecap,bool rangeBoost) //this is to modify enemy_state[i] to use them for suction later
{
    if (Keyboard::isKeyPressed(Keyboard::Space)) 
    {
        if (enemy_in_vacuum >= levelstorecap) return; //level cap for storing enemies (hence vacuum wont store any further than that)

        // CHECK RANGE BOOST
        float reach = 150.0f;
        if (rangeBoost) reach = 300.0f; // Increase radius if picked up powerup
        for (int i = 0; i < num_enemies; i++) //checks for all enemies
        {
            if (enemy_capture_state[i] == 0)  //means walking
            {
                bool inRange = false;

                if (vacuumdir == 'D') { // right
                    if (enemy_x[i] > player_x && enemy_x[i] < player_x + reach && abs(player_y - enemy_y[i]) < 80) {
                        inRange = true;
                    }
                } 
                else if (vacuumdir == 'A') { // left
                    if (enemy_x[i] > player_x - reach && enemy_x[i] < player_x && abs(player_y - enemy_y[i]) < 80) {
                        inRange = true;
                    }
                }
                else if (vacuumdir == 'W') { // up
                    //  y must be above player (player_y - 150 to player_y)
                    //  x must be close (abs(player_x - enemy_x[i]) < 80)
                    if ( enemy_y[i] > player_y - reach && enemy_y[i] < player_y && abs(player_x - enemy_x[i]) < 80 ) {
                        inRange = true;
                    }
                }
                else if (vacuumdir == 'S') { // down
                     //  y must be below player (player_y to player_y + 150)
                     //  x must be close
                    if ( enemy_y[i] > player_y && enemy_y[i] < player_y + reach && abs(player_x - enemy_x[i]) < 80 ) {
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


///////////////////////////////////////////////////////////////////////////
/////                made lvl2 function                             ///////
/////                                                              ///////
//////////////////////////////////////////////////////////////////////////  
void generate_level2(char** lvl, int height, int width)
{
    // 1. Clear Level
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            lvl[i][j] = ' ';
        }
    }

    // 2. Set Floor (Row 10) - Keep as '#' (pass-through) or change to '@' if you want walls
    for (int j = 0; j < width; j++) lvl[10][j] = '#';

    // 3. Place Solid Stairs (Use '@' for solid blocks)
    lvl[9][8] = '@';    // Staircase start
    lvl[8][7] = '@';    
    lvl[7][6] = '@';    
    lvl[6][5] = '@';    
    lvl[5][4] = '@';    
    lvl[4][3] = '@';    
    lvl[3][2] = '@';  lvl[3][0] = '#';  lvl[3][1] = '#';  
    lvl[9][9] = '&';  
    lvl[8][8] = '&';    
    lvl[7][7] = '&';    
    lvl[6][6] = '&';    
    lvl[5][5] = '&';    
    lvl[4][4] = '&';    
    lvl[3][3] = '&'; 

    // Other platforms (Keep as '#' so you can jump through them)
    for (int j = 0; j < width; j++) if (j <=6 || j >=11) lvl[8][j] = '#';  
    for (int j = 0; j < width; j++) if (j!=6 && j!=7 && j!=8 && j!=16 &&j!=15 && j!=17) lvl[6][j] = '#'; 
    for (int j = 0; j < width; j++) if ((( j>=7 || j >=10 && j<=17 )&& j!=7) ) lvl[4][j] = '#';  
    for (int j = 0; j < width; j++) if (j!=0 && j!=1 &&j!=3&&j!=4&&j!=5 && j!=1 && j!=2 && j!=16 &&j!=15 && j!=17) lvl[2][j] = '#'; 
}



// Function to handle diagonal slope movement
void handle_slope(float &x, float &y, char** lvl, int p_h, int p_w, int c_s) 
{
    int feet_row = (y + p_h) / c_s;
    int center_col = (x + p_w / 2) / c_s;

    if (lvl[feet_row][center_col] == '&') 
    {
        x += 5;
        y += 5;
    }
}
void initialize_enemy_group(  int count, Sprite* sprites, float* x_arr,  float* y_arr,float* speed_arr, int* state_arr,Texture& texture, IntRect initialRect,       
    int enemy_w, int enemy_h,  char** lvl, int lvl_h, int lvl_w, int cell_s)
{
    for(int i = 0; i < count; i++)
    {
        sprites[i].setTexture(texture);
        sprites[i].setTextureRect(initialRect);
        
        // Randomize Direction
        if (rand() % 2 == 0){ 
            speed_arr[i] = 1.0f;
            sprites[i].setScale(-2.5, 2.5); // Face Right
        } else {
            speed_arr[i] = -1.0f;
            sprites[i].setScale(2.5, 2.5);  // Face Left
        }
        
        state_arr[i] = 0; // Set to Walking
        
        // Use the universal spawn logic
        spawn_enemy(x_arr[i], y_arr[i], sprites[i], lvl, lvl_h, lvl_w, cell_s, enemy_h);
    }
}


void update_invisibility_feature(Sprite& sprite, int& timer, bool& isVisible, int capture_state)
{
    // If being sucked (State 1), force visibility so player sees what they are catching
    if (capture_state == 1) {
        sprite.setColor(Color(255, 255, 255, 255));
        return;
    }

    timer++;
    
    // Toggle state every 3 seconds (180 frames at 60 FPS)
    if (timer > 180) {
        isVisible = !isVisible;
        timer = 0;
    }

    // Apply Transparency
    if (isVisible) {
        sprite.setColor(Color(255, 255, 255, 255)); // Visible
    } else {
        sprite.setColor(Color(255, 255, 255, 0));   // Completely Invisible
    }
}

void spawn_powerups(char** lvl, int h, int w, int cell_s,
                    float* pup_x, float* pup_y, int* pup_type, bool* pup_active, int max_powerups)
{
    // 1. Check if a powerup already exists. If yes, do not spawn another.
    for (int i = 0; i < max_powerups; i++) {
        if (pup_active[i]) return; 
    }

    // 2. Find an empty slot in the array
    int index = -1;
    for(int i = 0; i < max_powerups; i++) {
        if (!pup_active[i]) {
            index = i;
            break;
        }
    }

    // 3. Spawn the powerup
    if (index != -1) {
        while (true) 
        {
            int r = rand() % (h - 2); 
            int c = rand() % w;        

            // Ensure valid spawn location (same logic as before)
            if (lvl[r][c] != '#' && lvl[r][c] != '@' && lvl[r][c] != '&' && 
                (lvl[r + 1][c] == '#' || lvl[r + 1][c] == '@') &&            
                lvl[r + 1][c] != '&' && (r + 1) != 10)
            {
                pup_x[index] = c * cell_s + 10; 
                pup_y[index] = (r + 1) * cell_s - 40; 
                pup_active[index] = true;
                pup_type[index] = rand() % 3; 
                break; 
            }
        }
    }
}


void update_powerups(RenderWindow& window, float px, float py, int pw, int ph, int& health, 
                     Sprite& sSpeed, Sprite& sHealth, Sprite& sRange,
                     float* pup_x, float* pup_y, int* pup_type, bool* pup_active, int max_powerups,
                     bool& speedBoost, bool& rangeBoost)
{
    for(int i = 0; i < max_powerups; i++)
    {
        if (pup_active[i])
        {
            // Draw the correct sprite based on type
            if (pup_type[i] == 0) {
                sSpeed.setPosition(pup_x[i], pup_y[i]);
                window.draw(sSpeed);
            }
            else if (pup_type[i] == 1) {
                sHealth.setPosition(pup_x[i], pup_y[i]);
                window.draw(sHealth);
            }
            else if (pup_type[i] == 2) {
                sRange.setPosition(pup_x[i], pup_y[i]);
                window.draw(sRange);
            }

            // Simple Collision Check (Overlap)
            if (px < pup_x[i] + 40 && px + pw > pup_x[i] &&
                py < pup_y[i] + 40 && py + ph > pup_y[i])
            {
                // Apply Effect
                if (pup_type[i] == 0) {
                    speedBoost = true; // Speed x2
                }
                else if (pup_type[i] == 1) {
                    if (health < 3) health++; // Health +1
                }
                else if (pup_type[i] == 2) {
                    rangeBoost = true; // Range Increase
                }

                // Remove powerup
                pup_active[i] = false;
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
     char** lvl2;
    
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
    Texture bgLvl2Tex;
    Sprite bgLvl2Sprite;
    Texture lvl1Tex;
    Sprite lvl1Sprite;
      Texture lvl2Tex;
    Sprite lvl2Sprite;
    Texture blockTexture;
    Sprite blockSprite;
    Texture blockLvl2Texture;
    Sprite blockLvl2Sprite;
    Texture slantedTexture;
    Sprite slantedSprite;
    Texture menutext1Tex;
    Sprite menutext1Sprite;
    Texture menutext2Tex;
    Sprite menutext2aSprite;
    Sprite menutext2bSprite;
     Texture menutext3Tex;
    Sprite menutext3Sprite;
    Texture menutext4Tex;
    Sprite menutext4aSprite;
    Sprite menutext4bSprite;
    Texture menutext5Tex;
    Sprite menutext5aSprite;
    Sprite menutext5bSprite;
    Texture powerupsTex;
    Sprite SpeedSprite;
    Sprite HealthSprite;
    Sprite rangeSprite;
    Texture GameoverTex;
    Sprite GameoverSprite;

    
    bgstartTex.loadFromFile("Data/tumblepop.png");
    bgstartSprite.setTexture(bgstartTex);
    bgstartSprite.setPosition(0,0);
    playermenuTex.loadFromFile("Data/menu.png");
    PlayermenuSprite.setTexture(playermenuTex);
    PlayermenuSprite.setPosition(0,0);
    PlayermenuSprite.setScale(3.6,3.2);
    lvl1Tex.loadFromFile("Data/lvl1.png");
    lvl1Sprite.setTexture(lvl1Tex);
    lvl1Sprite.setPosition(0,0);
     lvl2Tex.loadFromFile("Data/lvl2.png");
    lvl2Sprite.setTexture(lvl2Tex);
    lvl2Sprite.setPosition(0,0);
 

    bgTex.loadFromFile("Data/bg3.jpeg"); 
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);
    playTex.loadFromFile("Data/play.png");
    PlaySprite.setTexture(playTex);
    PlaySprite.setPosition(400,400);
    PlaySprite.setScale(0.7,0.4);
     
     bgLvl2Tex.loadFromFile("Data/bglvl2.png"); 
    bgLvl2Sprite.setTexture(bgLvl2Tex);
    bgLvl2Sprite.setPosition(0,0);

    blockTexture.loadFromFile("Data/block3.png"); 
    blockSprite.setTexture(blockTexture);
     blockLvl2Texture.loadFromFile("Data/blocklvl2.png"); 
    blockLvl2Sprite.setTexture(blockLvl2Texture);
      slantedTexture.loadFromFile("Data/slanted.png"); 
      slantedSprite.setTexture(slantedTexture);
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
    GameoverTex.loadFromFile("Data/gameover.jpg");
    GameoverSprite.setTexture(GameoverTex);


     menutext3Tex.loadFromFile("Data/menutext3.png");
    menutext3Sprite.setTexture(menutext3Tex);
    menutext3Sprite.setScale(0.7,0.4);
    menutext4Tex.loadFromFile("Data/menutext5.png");
    menutext4aSprite.setTexture(menutext4Tex);
    menutext4aSprite.setTextureRect(IntRect(26,12,660,159));
    menutext4aSprite.setScale(0.2,0.2);
    menutext4bSprite.setTexture(menutext4Tex);
    menutext4bSprite.setTextureRect(IntRect(18,186,660,159));
    menutext4bSprite.setScale(0.2,0.2);
    menutext5Tex.loadFromFile("Data/menutext4.png");
    menutext5aSprite.setTexture(menutext5Tex);
    menutext5aSprite.setTextureRect(IntRect(18,12,660,159));
    menutext5aSprite.setScale(0.2,0.2);
    menutext5bSprite.setTexture(menutext5Tex);
    menutext5bSprite.setTextureRect(IntRect(18,186,660,159));
    menutext5bSprite.setScale(0.2,0.2);
    powerupsTex.loadFromFile("Data/powerups.png");
    SpeedSprite.setTexture(powerupsTex);
    SpeedSprite.setTextureRect(IntRect(8,164,142,149));
    SpeedSprite.setScale(0.3,0.3);
    HealthSprite.setTexture(powerupsTex);
    HealthSprite.setTextureRect(IntRect(588,2,156,162));
    HealthSprite.setScale(0.3,0.3);
    rangeSprite.setTexture(powerupsTex);
    rangeSprite.setTextureRect(IntRect(433,165,117,140));
    rangeSprite.setScale(0.3,0.3);
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
    // Death State Variables
    bool isDying = false;
    int deathTimer = 0;   // to count how long the animation plays
    int deathFrame = 0;   // to track the animation frame
   
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
    
    // users Jumping State
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



    // CHELNOV DATA 
const int NUM_CHELNOVS = 4;
float chel_x[NUM_CHELNOVS];
float chel_y[NUM_CHELNOVS];
float chel_speed[NUM_CHELNOVS];
Sprite chelnovs[NUM_CHELNOVS];
int chel_capture_state[NUM_CHELNOVS] = {0}; // 0=Walk, 1=Suck, 2=Dead
// CHELNOV BULLET DATA
bool chel_bullet_active[NUM_CHELNOVS];      // Is the bullet flying?
float chel_bullet_x[NUM_CHELNOVS];          // Bullet X position
float chel_bullet_y[NUM_CHELNOVS];          // Bullet Y position
float chel_bullet_vx[NUM_CHELNOVS];         // Bullet speed (left or right)
int chel_shoot_timer[NUM_CHELNOVS];         // Timer for shooting
int chel_width = 24 * 2.5; 
int chel_height = 36 * 2.5; 
Texture chelnovTexture;
chelnovTexture.loadFromFile("Data/chelnov.png");
Sprite chelnovFireSprite;
chelnovFireSprite.setTexture(chelnovTexture);
chelnovFireSprite.setTextureRect(IntRect(233, 61, 13, 13));



// --- INVISIBLE MAN DATA ---
const int NUM_INVISIBLE = 3;
float inv_x[NUM_INVISIBLE];
float inv_y[NUM_INVISIBLE];
float inv_speed[NUM_INVISIBLE];
Sprite invisibleMen[NUM_INVISIBLE];
int inv_capture_state[NUM_INVISIBLE] = {0}; // 0=Walk, 1=Suck, 2=Dead

// Invisibility specific variables
int inv_vis_timer[NUM_INVISIBLE] = {0};      // Timer to swap visibility
bool inv_is_visible[NUM_INVISIBLE];          // Is he currently seen?
int inv_width = 26 * 2.5;                    // Dimensions 
int inv_height = 40 * 2.5;                   
Texture invisibleTexture;

// --- POWERUP DATA ---
const int MAX_POWERUPS = 3;
float pup_x[MAX_POWERUPS];
float pup_y[MAX_POWERUPS];
int pup_type[MAX_POWERUPS]; // 0 = Speed, 1 = Health, 2 = Range
bool pup_active[MAX_POWERUPS]; // Is the powerup currently on screen?

// Effect Flags
bool speedBoost = false;
bool rangeBoost = false;
int powerup_timer = 0; // To control how often they spawn

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
    //playerdeath
    Texture Player1DeathTex;
    Sprite  Player1DeathSprite;
    Texture Player2DeathTex;
    Sprite  Player2DeathSprite;
    

    Player1VacTex.loadFromFile("Data/player1_vacuum.png"); 
    Player2VacTex.loadFromFile("Data/player2_vacuum.png");
    
    Player1DeathTex.loadFromFile("Data/player1death.png"); 
    Player1DeathSprite.setTexture(Player1DeathTex);
    Player1DeathSprite.setScale(0.75,0.75);

    Player2DeathTex.loadFromFile("Data/player2death.png"); 
    Player2DeathSprite.setTexture(Player2DeathTex);
    Player2DeathSprite.setScale(0.6,0.6);

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
    //creating level 2 array
    lvl2 = new char* [height];
    for (int i = 0; i < height; i++)
    {
        lvl2[i] = new char[width];
    }
     generate_level2(lvl2, height, width);

    // --- INITIALIZE SKELETONS ---
    initialize_enemy_group(NUM_SKELETONS, skeletons, skel_x, skel_y, skel_speed, skel_capture_state, 
                           skeletonTexture, IntRect(8,34,32,38), skel_width, skel_height, 
                           lvl, height, width, cell_size);

    for(int i=0; i<NUM_SKELETONS; i++) { 
        skelPlatformState[i] = 0; }


    // --- INITIALIZE GHOSTS ---
    initialize_enemy_group(NUM_GHOSTS, ghosts, ghost_x, ghost_y, ghost_speed, ghost_capture_state, 
                           ghostTexture, IntRect(8,9,35,29), ghost_width, ghost_height, 
                           lvl, height, width, cell_size);


    // --- INITIALIZE CHELNOVS ---
    initialize_enemy_group(NUM_CHELNOVS, chelnovs, chel_x, chel_y, chel_speed, chel_capture_state, 
                           chelnovTexture, IntRect(12,4,26,40), chel_width, chel_height, 
                           lvl, height, width, cell_size);

// Load Texture
invisibleTexture.loadFromFile("Data/invisible.png");

// Initialize positions and sprites
initialize_enemy_group(NUM_INVISIBLE, invisibleMen, inv_x, inv_y, inv_speed, inv_capture_state, 
                       invisibleTexture, IntRect(8,18,26,36), inv_width, inv_height, 
                       lvl, height, width, cell_size);


// Initialize powerups to false
    for(int i=0; i<MAX_POWERUPS; i++) {
        pup_active[i] = false;
    }



    bool level2 = false; // Flag to track if Level 2 setup is done
    
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
        

      else if (state == 2)
        {
             window.draw(PlayermenuSprite); 
            
             menutext3Sprite.setPosition(300, 50);
             window.draw(menutext3Sprite);
             menutext4aSprite.setPosition(290,320);
             window.draw(menutext4aSprite);
             menutext4bSprite.setPosition(645,320);
             window.draw(menutext4bSprite);
             menutext5aSprite.setPosition(293,620);
             window.draw(menutext5aSprite);
             menutext5bSprite.setPosition(650, 620);
             window.draw(menutext5bSprite);
             lvl1Sprite.setScale(0.3, 0.3); 
             lvl2Sprite.setScale(0.3, 0.3);


             lvl1Sprite.setPosition(250, 400); 
             window.draw(lvl1Sprite);
             lvl2Sprite.setPosition(610, 395);
             window.draw(lvl2Sprite);

            if (Keyboard::isKeyPressed(Keyboard::A) ) {
                state = 3; 
            
               
            }
            else if (Keyboard::isKeyPressed(Keyboard::B) ) {
                state = 4 ;
            
              

            }
           

            window.display();
             

        }
        
///////////////////////////////////////////////////////////////////////////
/////                updated gameplay level 1 state                 ///////
/////                                                              ///////
////////////////////////////////////////////////////////////////////////// 


        else if(state == 3) //gameplay state level 1
        {
            window.clear();
            display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite,slantedSprite, height, width, cell_size);

            
            // DEATH ANIMATION STATE
        
            if (isDying)
            {
                deathTimer++;

                // slow down animation (update sprite every 10 frames)
                if (deathTimer % 10 == 0) {
                    deathFrame++;
                    if (deathFrame > 9)
                     deathFrame = 9; // Stop at last frame
                }

                // draw death sprite at the position where player died
               if (selectedPlayer == 1)
                        {
                            Player1DeathSprite.setPosition(player_x, player_y);
                            animate_death(Player1DeathSprite, deathFrame, 1);
                            window.draw(Player1DeathSprite);
                        }
                else
                        {
                            Player2DeathSprite.setPosition(player_x, player_y);
                            animate_death(Player2DeathSprite, deathFrame, 2);
                            window.draw(Player2DeathSprite);
                        }
                                    // wait for approx 2s before respawn
                if (deathTimer > 120) 
                {
                    playerHealth--; // Decrease health  after animation
                    
                                        // This ensures it only happens exactly when a life is lost
                      // Check if player still has lives
                    if (playerHealth > 0) 
                    {
                        // 1. Spawn Powerup logic
                        spawn_powerups(lvl, height, width, cell_size, 
                                        pup_x, pup_y, pup_type, pup_active, MAX_POWERUPS);

                        // 2. Respawn Logic
                        player_x = 50;
                        player_y = 538;
                        isDying = false;
                        deathTimer = 0;
                        deathFrame = 0;
                        velocityY = 0; // Reset physics
                    } 
                    else 
                    {
                        // Health is 0, Go to Game Over
                        state = 5; 
                    }

                   
                }
            }
            //
            // NORMAL GAMEPLAY STATE (Only runs if NOT dying)
            // 
            else 
            {
                //  TIMER
                timer++;
                if (timer > 9) {
                    walkFrame = (walkFrame + 1) % 5;
                    enemyFrame = (enemyFrame + 1) % 8;
                    timer = 0;
                }

               float currentSpeed = speed;
            if (speedBoost) currentSpeed = speed * 2;

            // DRAW POWERUPS
            update_powerups(window, player_x, player_y, PlayerWidth, PlayerHeight, playerHealth, SpeedSprite, HealthSprite, rangeSprite,
                            pup_x, pup_y, pup_type, pup_active, MAX_POWERUPS,
                            speedBoost, rangeBoost);

            // 3. UPDATED VACUUM CALLS (Added rangeBoost at the end)
            vacuumMovement(vacuumdir);
            vacuumRangeCheck(player_x, player_y, vacuumdir, skel_x, skel_y, skel_capture_state, NUM_SKELETONS , enemies_in_vacuum , levelstorecap, rangeBoost);
            vacuumRangeCheck(player_x, player_y, vacuumdir, ghost_x, ghost_y, ghost_capture_state, NUM_GHOSTS , enemies_in_vacuum  , levelstorecap, rangeBoost);

            // 4. UPDATED MOVEMENT CALL (Passed currentSpeed instead of speed)
            movement(onGround, velocityY, jumpStrength, isJumping, player_x, player_y, PlayerSprite, 
                currentSpeed, cell_size, PlayerHeight, PlayerWidth, selectedPlayer,   walkFrame, facingRight, vacuumdir, Player1Texture, Player2Texture, Player1VacTex, Player2VacTex, lvl, width, height);
                player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
                screenborder(player_x , player_y , PlayerWidth , velocityY , screen_x); 
                
                PlayerSprite.setPosition(player_x, player_y);
                adjustVacuumSprites(PlayerSprite, vacuumdir, selectedPlayer);
                window.draw(PlayerSprite);
            }

            //
            // ENEMY LOGIC Always draw enemies
            // 
            
            // SKELETONS
            for (int i = 0; i < NUM_SKELETONS; i++)
            {
                if (skel_capture_state[i] != 2) 
                {
                    if (skel_capture_state[i] == 0) {
                        // only move if player is not dying
                        if (!isDying && skelPlatformState[i] == 0)
                            update_enemy_logic(skel_x[i], skel_y[i], skel_speed[i], skeletons[i], lvl, width, cell_size, skel_width, skel_height);
                        
                        skele_platform_switch(i, skel_x[i], skel_y[i], skelPlatformState[i], skelTimer[i], skelStartY[i], skelTargetY[i], skeletons[i], lvl, height, cell_size, skel_height);
                        
                        if (skelPlatformState[i] == 0) animate_skeleton(skeletons[i], enemyFrame);
                    }
                    else if (skel_capture_state[i] == 1) {
                         /// Vacuum logic 
                         if (skel_x[i] < player_x) skel_x[i] += suctionspeed;
                         else skel_x[i] -= suctionspeed;
                         if (skel_y[i] < player_y) skel_y[i] += suctionspeed;
                         else skel_y[i] -= suctionspeed;
                         skeletons[i].setPosition(skel_x[i], skel_y[i]);
                         
                         if (abs(player_x - skel_x[i]) < 10 && abs(player_y - skel_y[i]) < 10) {
                            if (enemies_in_vacuum < 3) {
                                skel_capture_state[i] = 2; 
                                enemies_in_vacuum++; 
                            } else { skel_capture_state[i] = 0; }
                        }
                        animate_skeleton(skeletons[i], enemyFrame);
                    }

                    // 
                    // only check collision if player is not already dying
                    if (!isDying && skel_capture_state[i] == 0) 
                    {
                        // Check collision
                        if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, skel_x[i], skel_y[i], skel_width, skel_height))
                        {
                            isDying = true;  // Start death sequence
                            deathTimer = 0;
                            deathFrame = 0;
                        }
                    }
                    window.draw(skeletons[i]);
                }
            }

            // --- GHOSTS ---
            for (int i = 0; i < NUM_GHOSTS; i++)
            {
                if (ghost_capture_state[i] != 2)
                {
                    if (ghost_capture_state[i] == 0) {
                        if (ghostPause[i] > 0) {
                            ghostPause[i]--;
                            if (ghostPause[i] == 0) {
                                ghost_speed[i] = -ghost_speed[i];
                                if (ghost_speed[i] > 0) ghosts[i].setScale(-2.5, 2.5);
                                else ghosts[i].setScale( 2.5, 2.5);
                            }
                        } else {
                            if (rand() % 300 == 0) ghostPause[i] = 120;
                            else if (!isDying) // Only move if not dying
                                update_enemy_logic(ghost_x[i], ghost_y[i], ghost_speed[i], ghosts[i], lvl, width, cell_size, ghost_width, ghost_height);
                        }
                        if (ghost_speed[i] >= 0) ghosts[i].setPosition(ghost_x[i] + ghost_width, ghost_y[i]);
                        else ghosts[i].setPosition(ghost_x[i], ghost_y[i]);
                    }
                    else if (ghost_capture_state[i] == 1) {
                        if (ghost_x[i] < player_x) ghost_x[i] += suctionspeed;
                        else ghost_x[i] -= suctionspeed;
                        if (ghost_y[i] < player_y) ghost_y[i] += suctionspeed;
                        else ghost_y[i] -= suctionspeed;
                        ghosts[i].setPosition(ghost_x[i], ghost_y[i]);
                        if (abs(player_x - ghost_x[i]) < 10 && abs(player_y - ghost_y[i]) < 10) {
                            if (enemies_in_vacuum < 3) { ghost_capture_state[i] = 2; enemies_in_vacuum++; } 
                            else { ghost_capture_state[i] = 0; } 
                        }
                    }

                    animate_ghost(ghosts[i], enemyFrame);

                    
                    if (!isDying && ghost_capture_state[i] == 0)
                    {
                    
                        if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, ghost_x[i], ghost_y[i], ghost_width, ghost_height))
                        {
                            isDying = true; // Start death sequence
                            deathTimer = 0;
                            deathFrame = 0;
                        }
                    }
                    window.draw(ghosts[i]);
                }
            }
            window.display();
        }
        
        
      
        // --- STATE 4: LEVEL 2 GAMEPLAY (Updated with Physics/Enemies) ---
       else if(state == 4) // LEVEL 2 GAMEPLAY
        {
            window.clear();
            
            // 1. INITIALIZE LEVEL 2 
            if (!level2)
            {
                // Reset Player Position
                player_x = 50;
                player_y = 538;
                velocityY = 0;
                
                enemies_in_vacuum = 0; 

               // 1. SKELETONS
                     initialize_enemy_group(NUM_SKELETONS, skeletons, skel_x, skel_y, skel_speed, skel_capture_state, 
                           skeletonTexture, IntRect(8,34,32,38), skel_width, skel_height, 
                           lvl2, height, width, cell_size);
                     for(int i=0; i<NUM_SKELETONS; i++) skelPlatformState[i] = 0; 

                    // 2. GHOSTS
                    initialize_enemy_group(NUM_GHOSTS, ghosts, ghost_x, ghost_y, ghost_speed, ghost_capture_state, 
                                        ghostTexture, IntRect(8,9,35,29), ghost_width, ghost_height, 
                                        lvl2, height, width, cell_size);

                    // 3. CHELNOVS
                    initialize_enemy_group(NUM_CHELNOVS, chelnovs, chel_x, chel_y, chel_speed, chel_capture_state, 
                                        chelnovTexture, IntRect(0,0,32,32), chel_width, chel_height, 
                                        lvl2, height, width, cell_size);
                    // ADD THIS: RESET BULLETS
                    for(int i=0; i<NUM_CHELNOVS; i++) {
                        chel_bullet_active[i] = false;
                        chel_shoot_timer[i] = 0;
                    }
                    
                    // Set bullet sprite scale to match game style
                    chelnovFireSprite.setScale(2.5, 2.5);
                    initialize_enemy_group(NUM_INVISIBLE, invisibleMen, inv_x, inv_y, inv_speed, inv_capture_state, 
                           invisibleTexture, IntRect(8,18,26,36), inv_width, inv_height, 
                           lvl2, height, width, cell_size);
                        
                        for(int i=0; i<NUM_INVISIBLE; i++) {
                         // This makes them disappear at different times
                          inv_vis_timer[i] = rand() % 180;
                           if (rand() % 2 == 0) {
                            inv_is_visible[i] = true;
                        } else {
                            inv_is_visible[i] = false;
                        }
                          }

                level2 = true; // Stop this block from running again
            }

            // 2. DISPLAY LEVEL (Using lvl2 array and textures)
            display_level(window, lvl2, bgLvl2Tex, bgLvl2Sprite, blockLvl2Texture, blockLvl2Sprite,slantedSprite, height, width, cell_size);

            // 3. PLAYER DEATH LOGIC
            if (isDying)
            {
                deathTimer++;
                if (deathTimer % 10 == 0) {
                    deathFrame++;
                    if (deathFrame > 9) deathFrame = 9;
                }

                if (selectedPlayer == 1) {
                    Player1DeathSprite.setPosition(player_x, player_y);
                    animate_death(Player1DeathSprite, deathFrame, 1);
                    window.draw(Player1DeathSprite);
                } else {
                    Player2DeathSprite.setPosition(player_x, player_y);
                    animate_death(Player2DeathSprite, deathFrame, 2);
                    window.draw(Player2DeathSprite);
                }

                if (deathTimer > 120) {
                    playerHealth--;
                   // Check if player still has lives
                    if (playerHealth > 0) 
                    {
                        // 1. Spawn Powerup logic
                        spawn_powerups(lvl, height, width, cell_size, 
                                        pup_x, pup_y, pup_type, pup_active, MAX_POWERUPS);

                        // 2. Respawn Logic
                        player_x = 50;
                        player_y = 538;
                        isDying = false;
                        deathTimer = 0;
                        deathFrame = 0;
                        velocityY = 0; // Reset physics
                    } 
                    else 
                    {
                        // Health is 0, Go to Game Over
                        state = 5; 
                    }
                }
            }
            // 4. NORMAL GAMEPLAY (Only if not dying)
            else 
            {
                timer++;
                if (timer > 9) {
                    walkFrame = (walkFrame + 1) % 5;
                    enemyFrame = (enemyFrame + 1) % 8;
                    timer = 0;
                }

                   float currentSpeed = speed;
            if (speedBoost) currentSpeed = speed * 2;

            // 2. SPAWN AND DRAW POWERUPS
           
            update_powerups(window, player_x, player_y, PlayerWidth, PlayerHeight, playerHealth, SpeedSprite, HealthSprite, rangeSprite,
                            pup_x, pup_y, pup_type, pup_active, MAX_POWERUPS,
                            speedBoost, rangeBoost);

            // 3. UPDATED VACUUM CALLS (Added rangeBoost at the end)
            vacuumMovement(vacuumdir);
            vacuumRangeCheck(player_x, player_y, vacuumdir, skel_x, skel_y, skel_capture_state, NUM_SKELETONS , enemies_in_vacuum , levelstorecap, rangeBoost);
            vacuumRangeCheck(player_x, player_y, vacuumdir, ghost_x, ghost_y, ghost_capture_state, NUM_GHOSTS , enemies_in_vacuum  , levelstorecap, rangeBoost);

            // 4. UPDATED MOVEMENT CALL (Passed currentSpeed instead of speed)
            movement(onGround, velocityY, jumpStrength, isJumping, player_x, player_y, PlayerSprite, 
                currentSpeed, cell_size, PlayerHeight, PlayerWidth, selectedPlayer,   walkFrame, facingRight, vacuumdir, Player1Texture, Player2Texture, Player1VacTex, Player2VacTex, lvl2, width, height);
                // *** CRITICAL: Pass lvl2 here for physics ***
                player_gravity(lvl2, offset_y, velocityY, onGround, gravity, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
            
                 handle_slope(player_x, player_y, lvl2, PlayerHeight, PlayerWidth, cell_size);
                screenborder(player_x , player_y , PlayerWidth , velocityY , screen_x); 
                
                PlayerSprite.setPosition(player_x, player_y);
                adjustVacuumSprites(PlayerSprite, vacuumdir, selectedPlayer);
                window.draw(PlayerSprite);
            }

            // 5. ENEMY LOGIC (SKELETONS) for Level 2
            for (int i = 0; i < 4; i++)
            {
                if (skel_capture_state[i] != 2) 
                {
                    if (skel_capture_state[i] == 0 && !isDying) {
                        if (skelPlatformState[i] == 0)
                        
                            update_enemy_logic(skel_x[i], skel_y[i], skel_speed[i], skeletons[i], lvl2, width, cell_size, skel_width, skel_height);
                        
                        
                        skele_platform_switch(i, skel_x[i], skel_y[i], skelPlatformState[i], skelTimer[i], skelStartY[i], skelTargetY[i], skeletons[i], lvl2, height, cell_size, skel_height);
                        
                        if (skelPlatformState[i] == 0) animate_skeleton(skeletons[i], enemyFrame);
                    }
                    else if (skel_capture_state[i] == 1) {
                        // Vacuum Logic
                         if (skel_x[i] < player_x) skel_x[i] += suctionspeed; else skel_x[i] -= suctionspeed;
                         if (skel_y[i] < player_y) skel_y[i] += suctionspeed; else skel_y[i] -= suctionspeed;
                         skeletons[i].setPosition(skel_x[i], skel_y[i]);
                         
                         if (abs(player_x - skel_x[i]) < 10 && abs(player_y - skel_y[i]) < 10 && enemies_in_vacuum < 3) {
                            skel_capture_state[i] = 2; enemies_in_vacuum++; 
                        }
                        animate_skeleton(skeletons[i], enemyFrame);
                    }

                    if (!isDying && skel_capture_state[i] == 0) 
                        if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, skel_x[i], skel_y[i], skel_width, skel_height)) {
                            isDying = true; deathTimer = 0; deathFrame = 0;
                        }
                    window.draw(skeletons[i]);
                }
            }

            // 6. ENEMY LOGIC (GHOSTS) for Level 2
            for (int i = 0; i < 4; i++)
            {
                if (ghost_capture_state[i] != 2)
                {
                    if (ghost_capture_state[i] == 0 && !isDying) {
                        if (ghostPause[i] > 0) {
                            ghostPause[i]--;
                            if (ghostPause[i] == 0) {
                                ghost_speed[i] = -ghost_speed[i];
                                if (ghost_speed[i] > 0) ghosts[i].setScale(-2.5, 2.5);
                                else ghosts[i].setScale( 2.5, 2.5);
                            }
                        } else {
                            if (rand() % 300 == 0) ghostPause[i] = 120;
                            else 
                                // *** CRITICAL: Pass lvl2 here ***
                                update_enemy_logic(ghost_x[i], ghost_y[i], ghost_speed[i], ghosts[i], lvl2, width, cell_size, ghost_width, ghost_height);
                        }
                        if (ghost_speed[i] >= 0) ghosts[i].setPosition(ghost_x[i] + ghost_width, ghost_y[i]);
                        else ghosts[i].setPosition(ghost_x[i], ghost_y[i]);
                    }
                    else if (ghost_capture_state[i] == 1) {
                        if (ghost_x[i] < player_x) ghost_x[i] += suctionspeed; else ghost_x[i] -= suctionspeed;
                        if (ghost_y[i] < player_y) ghost_y[i] += suctionspeed; else ghost_y[i] -= suctionspeed;
                        ghosts[i].setPosition(ghost_x[i], ghost_y[i]);
                        if (abs(player_x - ghost_x[i]) < 10 && abs(player_y - ghost_y[i]) < 10 && enemies_in_vacuum < 3) {
                            ghost_capture_state[i] = 2; enemies_in_vacuum++; 
                        }
                    }

                    animate_ghost(ghosts[i], enemyFrame);
                    if (!isDying && ghost_capture_state[i] == 0)
                        if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, ghost_x[i], ghost_y[i], ghost_width, ghost_height)) {
                            isDying = true; deathTimer = 0; deathFrame = 0;
                        }
                    window.draw(ghosts[i]);
                }
            }

// Iterate through every Chelnov enemy in the game
for (int i = 0; i < NUM_CHELNOVS; i++)
{
    // --- SHOOTING & BULLET LOGIC (Runs as long as Chelnov is alive) ---
    
    // Check if the enemy is NOT captured/dead (State 2 usually means dead)
    if (chel_capture_state[i] != 2)
    {
        // 1. Fire Logic: 
        // Only allow shooting if the enemy is walking normally (State 0) 
        // and the player is not currently in the death animation (!isDying).
        if (chel_capture_state[i] == 0 && !isDying) 
        {
            chel_shoot_timer[i]++; // Increment the frame counter for this enemy
            
            // Timer Check: 180 frames at 60FPS is approximately 3 seconds.
            if (chel_shoot_timer[i] > 180) 
            {
                chel_shoot_timer[i] = 0; // Reset timer for the next shot
                
                // Set the bullet flag to true so it is processed and drawn
                chel_bullet_active[i] = true;
                
                // Calculate spawn position:
                // We add half the width and 1/3 height to spawn the bullet 
                // near the center/gun of the enemy sprite, not the top-left corner.
                chel_bullet_x[i] = chel_x[i] + (chel_width / 2);
                chel_bullet_y[i] = chel_y[i] + (chel_height / 3); 

                // Determine Bullet Direction:
                // If enemy speed is positive, they face Right. Bullet Velocity = +8.
                // If enemy speed is negative, they face Left. Bullet Velocity = -8.
                if (chel_speed[i] > 0) chel_bullet_vx[i] = 8.0f;
                else chel_bullet_vx[i] = -8.0f;
            }
        }

        // 2. Update Bullet Physics
        // This block only runs if the bullet has been fired and hasn't hit anything yet.
        if (chel_bullet_active[i])
        {
            // Move the bullet horizontally
            chel_bullet_x[i] += chel_bullet_vx[i];

            // A. Screen Bounds Check
            // If the bullet goes off the left side (<0) or right side (> width), deactivate it.
            if (chel_bullet_x[i] < 0 || chel_bullet_x[i] > width * cell_size) {
                chel_bullet_active[i] = false;
            }
            
            // B. Wall Collision Check (Grid check)
            // Convert the bullet's pixel coordinates (x,y) into grid coordinates (col, row).
            int b_col = (int)(chel_bullet_x[i] / cell_size);
            int b_row = (int)(chel_bullet_y[i] / cell_size);
            
            // Ensure the grid coordinates are within the array bounds to prevent crashes
            if (b_row >= 0 && b_row < height && b_col >= 0 && b_col < width) {
                // Check the level array (lvl2). 
                // If the grid cell contains a wall ('@') or platform ('#'), destroy the bullet.
                if (lvl2[b_row][b_col] == '@' || lvl2[b_row][b_col] == '#') {
                    chel_bullet_active[i] = false;
                }
            }

            // C. Player Collision Check (Bullet hits Player)
            // If the bullet overlaps with the player's hitbox...
            if (!isDying && chel_bullet_active[i]) {
                // 32, 32 represents the bullet width/height for collision purposes
                if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, 
                                   chel_bullet_x[i], chel_bullet_y[i], 32, 32)) 
                {
                    isDying = true;                // Set Game State to "Dying"
                    deathTimer = 0;                // Reset death animation timer
                    deathFrame = 0;                // Reset death animation frame
                    chel_bullet_active[i] = false; // Destroy bullet immediately on impact
                }
            }

            // D. Draw Bullet
            // Update the sprite position to the calculated physics variables and draw it.
            if (chel_bullet_active[i]) {
                chelnovFireSprite.setPosition(chel_bullet_x[i], chel_bullet_y[i]);
                window.draw(chelnovFireSprite);
            }
        }
    }

    // --- EXISTING CHELNOV MOVEMENT/VACUUM LOGIC ---
    if (chel_capture_state[i] != 2) // Double check enemy isn't dead
    {
        // 1. Walking Logic
        // If state is 0, the enemy patrols normally using gravity and wall detection.
        if (chel_capture_state[i] == 0) {
            if (!isDying) { // Stop enemies moving if player is dead
                update_enemy_logic(chel_x[i], chel_y[i], chel_speed[i], chelnovs[i], 
                                   lvl2, width, cell_size, chel_width, chel_height);
            }
        }
        // 2. Vacuum Logic
        // If state is 1, the enemy is being sucked into the vacuum.
        else if (chel_capture_state[i] == 1) {
            // Move X and Y coordinates toward the Player's position
            if (chel_x[i] < player_x) chel_x[i] += suctionspeed; else chel_x[i] -= suctionspeed;
            if (chel_y[i] < player_y) chel_y[i] += suctionspeed; else chel_y[i] -= suctionspeed;
            chelnovs[i].setPosition(chel_x[i], chel_y[i]);

            // Check if enemy is close enough to be "Captured" (State 2)
            if (abs(player_x - chel_x[i]) < 10 && abs(player_y - chel_y[i]) < 10 && enemies_in_vacuum < levelstorecap) {
                chel_capture_state[i] = 2; // Enemy removed from map
                enemies_in_vacuum++;       // Player ammo increased
            }
        }

        // 3. Animation & Collision (Body Contact)
        animate_chelnov(chelnovs[i], enemyFrame);
        
        // Check if the player touches the enemy BODY (not the bullet)
        if (!isDying && chel_capture_state[i] == 0) {
            if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, chel_x[i], chel_y[i], chel_width, chel_height)) {
                isDying = true; deathTimer = 0; deathFrame = 0;
            }
        }
        window.draw(chelnovs[i]);
    }
}

// --- INVISIBLE MEN LOGIC (LEVEL 2) ---
vacuumRangeCheck(player_x, player_y, vacuumdir, inv_x, inv_y, inv_capture_state, NUM_INVISIBLE, enemies_in_vacuum, levelstorecap, rangeBoost);

for (int i = 0; i < NUM_INVISIBLE; i++)
{
    if (inv_capture_state[i] != 2) 
    {
        // 1. Handle Invisibility
        update_invisibility_feature(invisibleMen[i], inv_vis_timer[i], inv_is_visible[i], inv_capture_state[i]);

        // 2. Walking (Pass lvl2)
        if (inv_capture_state[i] == 0 && !isDying) {
             update_enemy_logic(inv_x[i], inv_y[i], inv_speed[i], invisibleMen[i], 
                                lvl2, width, cell_size, inv_width, inv_height);
        }
        // 3. Vacuum
        else if (inv_capture_state[i] == 1) {
            if (inv_x[i] < player_x) inv_x[i] += suctionspeed; else inv_x[i] -= suctionspeed;
            if (inv_y[i] < player_y) inv_y[i] += suctionspeed; else inv_y[i] -= suctionspeed;
            invisibleMen[i].setPosition(inv_x[i], inv_y[i]);

            if (abs(player_x - inv_x[i]) < 10 && abs(player_y - inv_y[i]) < 10 && enemies_in_vacuum < levelstorecap) {
                inv_capture_state[i] = 2; enemies_in_vacuum++; 
            }
        }

            // 4. Animation
        animate_invisible_man(invisibleMen[i], enemyFrame);
                // 5. Collision
        if (!isDying && inv_capture_state[i] == 0) {
            if (checkCollision(player_x, player_y, PlayerWidth, PlayerHeight, inv_x[i], inv_y[i], inv_width, inv_height)) {
                isDying = true; deathTimer = 0; deathFrame = 0;
            }
        }
        window.draw(invisibleMen[i]);
    }
}
            window.display();
        }

        else if (state == 5) // gameover state
    {   
        window.clear();

        GameoverSprite.setPosition(0,0);

       
 GameoverSprite.setScale(1,1);
        window.draw(GameoverSprite);
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
    for (int i = 0; i < height; i++)
    {
        delete[] lvl2[i];
    }
    delete[] lvl2;

    return 0;
}
