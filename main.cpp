
#include "mbed.h"

#include "SDFileSystem.h"
#include "wave_player.h"
#include "game_synchronizer.h"
#include "misc.h"
#include "blob.h"
#include "cmath"
#include "playSound.h"



#define NUM_BLOBS 22

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

DigitalIn pb_u(p21);                        // Up Button
DigitalIn pb_r(p22);                        // Right Button
DigitalIn pb_d(p23);                        // Down Button
DigitalIn pb_l(p24);                        // Left Button

Serial pc(USBTX, USBRX);                    // Serial connection to PC. Useful for debugging!
MMA8452 acc(p28, p27, 100000);              // Accelerometer (SDA, SCL, Baudrate)
uLCD_4DGL uLCD(p9,p10,p11);                 // LCD (tx, rx, reset)
SDFileSystem sd(p5, p6, p7, p8, "sd");      // SD  (mosi, miso, sck, cs)
AnalogOut DACout(p18);                      // speaker
wave_player player(&DACout);                // wav player
GSYNC game_synchronizer;                    // Game_Synchronizer
GSYNC* sync = &game_synchronizer;           //
Timer frame_timer;
DigitalOut green(p16);
DigitalOut red(p15);                          // Timer

int score1 = 0;                             // Player 1's score.
int score2 = 0;                             // Player 2's score.
int mode = 0;
typedef enum Level {Easy = 0, Hard = 1};
Level difficulty = Easy;
// ***
// Display a pretty game menu on the player 1 mbed. 
// Do a good job, and make it look nice. Give the player
// an option to play in single- or multi-player mode. 
// Use the buttons to allow them to choose.
int game_menu(void) {
    

    uLCD.background_color(16777215);
    uLCD.textbackground_color(16777215);
    uLCD.color(RED);
    uLCD.cls();

    
    for (int i = 0; i < 10; i++){
    int x = rand() % 127;
    int y = rand() % 127;
    int r = (rand() % 255); // red component of color
    int b = (rand() % 255); // green component of color
    int g = (rand() % 255); // blue component of color
    r = r << 16;
    g = g << 8;
    int rand_col = r | g | b;
    uLCD.filled_circle(x, y, 5, rand_col);
    }
    uLCD.locate(4,0);
    uLCD.text_height(1.5);
    uLCD.text_width(1.5);
    uLCD.printf("**AGRAR**");
    uLCD.locate(0,2);
    uLCD.printf("1P     down");
    uLCD.locate(0,3);
    uLCD.printf("2P     up");
    
    // Spin until a button is pressed. Depending which button is pressed, 
    // return either SINGLE_PLAYER or MULTI_PLAYER.
    

    while(1) {
        red = 0;
        green = 0;
        if(!pb_u) { 
            return SINGLE_PLAYER;
        }
        
        if (!pb_d) {
            
            return MULTI_PLAYER;        
        } 
    }
    

    
}

Level choose_difficulty(){


    while(1) {
        red = 0;
        green = 0;
        if(!pb_u) { 
            return Easy;
        }
        
        if (!pb_d) {
            
            return Hard;        
        } 
    }    
    
    
}




// Initialize the game hardware. 
// Call game_menu to find out which mode to play the game in (Single- or Multi-Player)
// Initialize the game synchronizer.
void game_init(void) {
    
    led1 = 0; led2 = 0; led3 = 0; led4 = 0;
    
    pb_u.mode(PullUp);
    pb_r.mode(PullUp); 
    pb_d.mode(PullUp);    
    pb_l.mode(PullUp);
    
    pc.printf("\033[2J\033[0;0H");              // Clear the terminal screen.
    pc.printf("I'm alive! Player 1\n");         // Let us know you made it this far

    // game_menu MUST return either SINGLE_PLAYER or MULTI_PLAYER
    int num_players = game_menu();
    uLCD.cls();
    if (num_players == SINGLE_PLAYER){
        uLCD.locate(0,0);
        uLCD.printf("single player!");
        
        uLCD.locate(0,2);
        uLCD.printf("easy :)");
        
        uLCD.locate(0,4);
        uLCD.printf("hard :O");        
        difficulty = choose_difficulty();
        
        
    }
    
    if (num_players == MULTI_PLAYER){
        uLCD.printf("multiplayer");
        mode = 1;
        wait(2);
        }
    
    GS_init(sync, &uLCD, &acc, &pb_u, &pb_r, &pb_d, &pb_l, num_players, PLAYER1); // Connect to the other player.
    pc.printf("Initialized...\n");              // Let us know you finished initializing.
    srand(time(NULL));                          // Seed the random number generator.

    GS_cls(sync, SCREEN_BOTH);
    GS_update(sync);
}



// Take in a pointer to the blobs array. Iterate over the array
// and initialize each blob with BLOB_init(). Set the first blob to (for example) blue
// and the second blob to (for example) red. Set the color(s) of the food blobs however you like.
// Make the radius of the "player blobs" equal and larger than the radius of the "food blobs".
void generate_blobs(BLOB* blobs) {
    
    

    GS_circle(sync, SCREEN_P1 , 0 , 0 , blobs[0].rad, blobs[0].color);
     
        
    for(int i = 1; i < NUM_BLOBS; i++) {
        if ((blobs[i].valid)){

            GS_circle(sync, SCREEN_P1 , blobs[i].posx - blobs[0].posx ,  blobs[i].posy - blobs[0].posy , blobs[i].rad, blobs[i].color);
        }
        
    }
    
    
    
    if (mode) {
        GS_circle(sync, SCREEN_P2 , 0 , 0 , blobs[1].rad, blobs[1].color);
        
        
        for(int i = 0; i < NUM_BLOBS; i++) {
            if (i == 1) {
                continue;
            }
            
            if ((blobs[i].valid)){
        
                GS_circle(sync, SCREEN_P2 , blobs[i].posx - blobs[1].posx ,  blobs[i].posy - blobs[1].posy , blobs[i].rad, blobs[i].color);
            }
            
        }        
        
        
    }
    
}




int main (void) {
int end = 0;
int scoreList[4];
int length = 0;
int scoreList1[4]; 
int tempList[4];
int tempList1[4];   
bool cont = true;
while(cont) {
        
    char blop[] = "/sd/wavfiles/BUZZER.wav";
    char zoop[] = "/sd/wavfiles/zoop.wav";

    
    int* p1_buttons;
    int* p2_buttons;
    
    float ax1, ay1, az1;
    float ax2, ay2, az2;
    
    
    // Ask the user to choose (via pushbuttons)
    // to play in single- or multi-player mode.
    
    //play buzzer
    playSound(blop);
    game_init();  
    GS_background_color(sync, SCREEN_BOTH, BGRD_COL); 

    
    // Keep an array of blobs. Use blob 0 for player 1 and
    // blob 1 for player 2.
    BLOB blobs[NUM_BLOBS];
    BLOB* pointer = blobs;
    BLOB* blob_0 = blobs;    
    BLOB* blob_1 = (blob_0+1);
    
    //blob 0
    BLOB_init(pointer, 20, 16777215);
    pointer-> posx = 0;
    pointer-> posy = 0;
    
    //blob 1
    BLOB_init(++pointer, 20, 16711680);
    if (mode == 1){
        pointer->vx = 0;
        pointer->vy = 0;
    } else {
        pointer->vx = 300;
        pointer->vy = 200;
    }
    
    //life blob
    BLOB_init(++pointer, 10, 8197526); 
    pointer->posx = 64;
    pointer->posy = 64;
    
    //boost blob
    BLOB_init(++pointer, 10, 13158400); 
    pointer->posx = -64;
    pointer->posy = -64;    
    

    
    

    
    int score = 0;


    
    
    
    for (int i = 4; i <NUM_BLOBS; i++) {
        BLOB_init(++pointer);
    }
    // pointer = &blobs[2];
     int lives = 1;
     int lives1 = 1;
    
    // if difficulty is hard make food blobs move more random and faster
    if (difficulty == Hard){
        for (int i = 1; i <NUM_BLOBS; i++) {
            int vx = rand() % (500) + (-500);
            int vy = rand() % (500) + (-500);            
            blobs[i].vx = vx;
            blobs[i].vy = vy;
            lives = 0;
        }  
    }
        
    // Pass in a pointer to the blobs array. Iterate over the array
    // and initialize each blob with BLOB_init(). 
    generate_blobs(blobs);
    
    int winner = 0;
    int exit = false;
    int score1 = 0;
    
    char strScore[13];
    char strScore1[13];
    char strLives[1];
    char strLives1[1];
    int boost = 0;
    int boost1 = 0;
    bool randCol = false;
    bool tie = false;


 
    
    //reset background color for both screens    
    GS_background_color(sync, SCREEN_BOTH, BGRD_COL);
    GS_cls(sync, SCREEN_BOTH);
    GS_textbackground_color(sync, SCREEN_BOTH, BGRD_COL);
    uLCD.color(WHITE);
    while(true) {
        
            // if push button r is pressed increase size
            if (!pb_r){
                GS_circle(sync, SCREEN_P1 , 0 , 0 , blob_0->rad, BGRD_COL);
                GS_circle(sync, SCREEN_P2 , blob_0->posx - blob_1->posx , blob_0->posy - blob_1->posy , blob_0->rad, BGRD_COL);
                blob_0->rad++;
                    
            }
            
            // if push button d is pressed change to random colors
            if (!pb_d){
                randCol = true;                    
            }
            
            if (randCol) {
                               
                for (int i = 4; i < NUM_BLOBS; i++) {
                                        
                    int r = (rand() % 255); // red component of color
                    int b = (rand() % 255); // green component of color
                    int g = (rand() % 255); // blue component of color
                    r = r << 16;
                    g = g << 8;
                    int rand_col = r | g | b;
                    
                    blobs[i].color = rand_col;
                
                }
                randCol = false;
                
            }
            
            
        
            //if all blobs have been eaten
            if ((score + score1) == 18){
                exit = true;
                
                if (score1 > score){
                    winner = 1;
                
                } else if (score == score1) {
                    tie = true;    
                }
            }

        
        
        

        
        
        // Use the game synchronizer API to get the button values from both players' mbeds.
        p1_buttons = GS_get_p1_buttons(sync);
        p2_buttons = GS_get_p2_buttons(sync);
        
        // Use the game synchronizer API to get the accelerometer values from both players' mbeds.
        GS_get_p1_accel_data(sync, &ax1, &ay1, &az1);
        GS_get_p2_accel_data(sync, &ax2, &ay2, &az2);
        
        
        
        // If the magnitude of the p1 x and/or y accelerometer values exceed ACC_THRESHOLD,
        // set the blob 0/blob 1 velocities to be proportional to the accelerometer values.
        if (abs(ax1) >= ACC_THRESHOLD || abs(ay1) >= ACC_THRESHOLD) {
            
            blob_0->vx = ay1*1000;
            blob_0->vy = ax1*1000;
                
        }
        //mode = multiplayer
        if (mode) {
            if (abs(ax2) >= ACC_THRESHOLD || abs(ay2) >= ACC_THRESHOLD) {
            
            blob_1->vx = ay2*1000;
            blob_1->vy = ax2*1000;
                
        }
            
        }
        
        // Boubndary coordinates for screen 1
        int rx = WORLD_WIDTH/2 - blob_0->posx;
        int lx = -WORLD_WIDTH/2 - blob_0->posx;
        
        int ty = -WORLD_HEIGHT/2 - blob_0->posy;
        int by = WORLD_HEIGHT/2 - blob_0->posy;
        
        // Boubndary coordinates for screen 2
        int rx1 = WORLD_WIDTH/2 - blob_1->posx;
        int lx1 = -WORLD_WIDTH/2 - blob_1->posx;
        
        int ty1 = -WORLD_HEIGHT/2 - blob_1->posy;
        int by1 = WORLD_HEIGHT/2 - blob_1->posy;        
        
        // right line 
        GS_line(sync, SCREEN_P1, rx, ty, rx, by, BGRD_COL);
        // left line
        GS_line(sync, SCREEN_P1, lx, ty, lx, by, BGRD_COL);
        // top line
        GS_line(sync, SCREEN_P1, lx, ty, rx, ty, BGRD_COL);
        // bottom line
        GS_line(sync, SCREEN_P1, lx, by, rx, by, BGRD_COL);

        
        // right line 
        GS_line(sync, SCREEN_P2, rx1, ty1, rx1, by1, BGRD_COL);
        // left line
        GS_line(sync, SCREEN_P2, lx1, ty1, lx1, by1, BGRD_COL);
        // top line
        GS_line(sync, SCREEN_P2, lx1, ty1, rx1, ty1, BGRD_COL);
        // bottom line
        GS_line(sync, SCREEN_P2, lx1, by1, rx1, by1, BGRD_COL);
                
        
        

        
        float time_step = .01; 
        

        
        // If the current blob is valid, or it was deleted in the last frame, (delete_now is true), then draw a background colored circle over
        // the old position of the blob. (If delete_now is true, reset delete_now to false.)          
        for(int i = 0; i < NUM_BLOBS; i++) {
            
            if(blobs[i].valid || blobs[i].delete_now) {
                GS_circle(sync, SCREEN_P1 , blobs[i].posx - blobs[0].posx ,blobs[i].posy -  blobs[0].posy , blobs[i].rad, BGRD_COL);
                if (mode) {
                    GS_circle(sync, SCREEN_P2 , blobs[i].posx - blobs[1].posx ,blobs[i].posy -  blobs[1].posy , blobs[i].rad, BGRD_COL);                    
                }
                if(blobs[i].delete_now){
                    blobs[i].delete_now = false;
                }
                    
            }       
        }
        
            

            
                // Use the blob positions and velocities, as well as the time_step to compute the new position of the blob.                
                blob_0->posx = blob_0->posx + (blob_0->vx * time_step);
                blob_0->posy = blob_0->posy + (blob_0->vy * time_step);

                
                for (int i = 1; i < NUM_BLOBS; i++) {
                    blobs[i].posx =   blobs[i].posx + (blobs[i].vx * time_step);
                    blobs[i].posy =  blobs[i].posy + (blobs[i].vy * time_step);                    
                }
                         
                                    
            
                // if exit is true we have a winner!
                if (exit){
                    break;
                }
            
            
                // Calculation of distance between blob1 and blob0
                float dist_thresh = blob_0->rad + blobs[1].rad;
                float distx = abs(blobs[1].posx - blobs[0].posx);
                float disty = abs(blobs[1].posy - blobs[0].posy);
                float dist = sqrt((distx*distx)+(disty*disty));            
            
                // if distance is less than the threshold one of them eats the other
                if (dist < dist_thresh) {
                    if (blob_0->rad > blobs[1].rad) {
                        //mode = multiplayer
                        if (mode) {
                            // If blob1 has a life and is being eaten: send blob1 away from blob0
                            if ((lives1)){
                                blobs[1].delete_now = true;
                                GS_circle(sync, SCREEN_P1 , blobs[1].posx - blobs[0].posx ,blobs[1].posy- blobs[0].posy , blobs[1].rad, BGRD_COL);
                                
                                blobs[0].rad = blobs[1].rad + 1;
                                blobs[1].posx = blobs[0].posy + 124;
                                blobs[1].posy = blobs[0].posy + 124;
                                exit = false;
                                lives1--;
                            } else {
                                blobs[1].valid = false;
                                blobs[1].delete_now = true;
                                GS_circle(sync, SCREEN_P1 ,blobs[1].posx - blobs[0].posx ,blobs[1].posy- blobs[0].posy , blobs[1].rad, BGRD_COL);
                                blobs[0].rad = blobs[0].rad + 1;
                                winner = 0;
                                exit = true;
                            }
                            
                        } else { 
                            blobs[1].valid = false;
                            blobs[1].delete_now = true;
                            GS_circle(sync, SCREEN_P1 ,blobs[1].posx - blobs[0].posx ,blobs[1].posy- blobs[0].posy , blobs[1].rad, BGRD_COL);
                            blob_0->rad = blob_0->rad + 1;
                            exit = true;
                        }                
                                    
                                    
                    } else if (blob_1->rad > blobs[0].rad) {
                        // if blob0 has a life and is being eaten: send blob0 away from blob1
                        if ((lives)){
                            blobs[0].delete_now = true;
                            GS_circle(sync, SCREEN_P1 , blobs[1].posx - blobs[0].posx ,blobs[1].posy- blobs[0].posy , blobs[1].rad, BGRD_COL);
                            blobs[1].rad = blobs[1].rad + 1;
                            blobs[1].posx = blobs[0].posy + 124;
                            blobs[1].posy = blobs[0].posy + 124;
                            exit = false;
                            lives--;
                        }else {
                            blobs[0].valid = false;
                            blobs[0].delete_now = true;
                            GS_circle(sync, SCREEN_P2 ,blobs[0].posx - blobs[1].posx ,blobs[0].posy- blobs[1].posy , blobs[1].rad, BGRD_COL);
                            blobs[1].rad = blobs[1].rad + 1;
                            winner = 1;
                            exit = true;
                        }
                        
                    } else {
                        exit = false;
                    }
                }
                
                //Make blob1 eat food blobs
                
                for (int i = 2; i < NUM_BLOBS; i++) {
                    dist_thresh = blob_1->rad + blobs[i].rad;
                    distx = abs(blob_1->posx - blobs[i].posx);
                    disty = abs(blob_1->posy -blobs[i].posy);
                    dist = sqrt((distx*distx)+(disty*disty));
 
                    if  ((dist < dist_thresh) && (blobs[i].valid)) {

                        blobs[i].valid = false;
                        blobs[i].delete_now = true;
                        GS_circle(sync, SCREEN_P1 , blobs[1].posx - blobs[0].posx ,blobs[1].posy- blobs[0].posy , blob_1->rad, BGRD_COL);
                        GS_circle(sync, SCREEN_P2 , 0 , 0, blob_1->rad, BGRD_COL);
                        blob_1->rad = blob_1->rad + 1 ;
                        if (mode){
                            if (i != 2 && i!=3){
                                blob_1->rad = blob_1->rad + 1 + boost1 ;
                                score1++;
                            } else if (i==2) {
                                lives1++;
                            } else {
                                boost1++;
                            }                                
                            
                        } else {    
                            score1++;
                        }
                    }    
                }
                    
                //Make blob0 eat food blobs
                for (int i = 2; i < NUM_BLOBS; i++) {
                    dist_thresh = blob_0->rad + blobs[i].rad;
                    distx = abs(blobs[i].posx - blobs[0].posx);
                    disty = abs(blobs[i].posy - blobs[0].posy);
                    dist = sqrt((distx*distx)+(disty*disty));
 
                    if  ((dist < dist_thresh) && (blobs[i].valid)) {
                        playSound(zoop);
                        blobs[i].valid = false;
                        blobs[i].delete_now = true;
                        GS_circle(sync, SCREEN_P1 , 0 , 0 , blob_0->rad, BGRD_COL);
                        GS_circle(sync, SCREEN_P2 , blobs[0].posx - blobs[1].posx ,blobs[0].posy- blobs[1].posy , blob_1->rad, BGRD_COL);
                        if (i != 2 && i!=3){
                        blob_0->rad = blob_0->rad + 1 + boost ;
                        score++;
                        } else if (i==2) {
                          lives++;
                        } else {
                          boost++;
                        }
                    }    
                }
            
            //if blob1 in screen turn on red else turn on green
            float xled = blob_1->posx - blobs[0].posx >=-(63+blob_1->rad) && blob_1->posx - blobs[0].posx <= (63 + blob_1->rad);
            float yled = blob_1->posy - blobs[0].posy >=-(63+blob_1->rad) && blob_1->posy - blobs[0].posy<= (63+blob_1->rad);

        
            if (xled && yled) {
                red = 1;
                green =0;
            } else { 
                green = 1;
                red = 0;
            } 
                
            
            // make food blobs move away from main blob
            
            if (difficulty == Hard) {
                for (int i = 2; i < 21; i++) {
                    if (((blobs[i].posx - blob_0->posx ) < 0 && (blobs[i].posx - blob_0->posx) > -30) && blobs[i].vx > 0) {
                        blobs[i].vx = -blobs[i].vx;
                    }
                    
                    if (((blobs[i].posx - blob_0->posx) > 0 && (blobs[i].posx - blob_0->posx) < 30) && blobs[i].vx < 0) {
                        blobs[i].vx = -blobs[i].vx;
                    }                                 
                    if (((blobs[i].posy - blob_0->posy) < 0 && (blobs[i].posy - blob_0->posy) > -30) && blobs[i].vy > 0) {
                        blobs[i].vy = -blobs[i].vy;
                    }
                    
                    if (((blobs[i].posy - blob_0->posy) > 0 && (blobs[i].posy - blob_0->posy) < 30)&& blobs[i].vy < 0) {
                        blobs[i].vy = -blobs[i].vy;
                    }                       
                            
                }                
                            
            }
                            
            

            
            // Make blobs conatraint to world
            BLOB_constrain2world(blobs); 
            
  
       
            // Draw blobs on appropriate screens
            generate_blobs(blobs);

            // prints scores and life count on screens accordingly
            int n1 = sprintf(strScore, "Score %d - %d", score, score1);
            int n1_1 = sprintf(strScore1, "Score %d - %d", score1, score);
            int n2 = sprintf(strLives, "L %d", lives);
            int n2_1 = sprintf(strLives1, "L %d", lives1);
            GS_locate(sync, SCREEN_P1, 0, 0);
            GS_puts(sync, SCREEN_P1, strScore, n1+1);
            GS_locate(sync, SCREEN_P2, 0, 0);
            GS_puts(sync, SCREEN_P2, strScore1, n1_1+1);            
            GS_locate(sync, SCREEN_P1, 15,0);
            GS_puts(sync, SCREEN_P1, strLives, n2+1);
            GS_locate(sync, SCREEN_P2, 15,0);
            GS_puts(sync, SCREEN_P2, strLives1, n2_1+1);


            

        
        
        // Redraw the world boundary rectangle.
        
        rx = WORLD_WIDTH/2 - blob_0->posx;
        lx = -WORLD_WIDTH/2 - blob_0->posx;
        
        ty = -WORLD_HEIGHT/2 - blob_0->posy;
        by = WORLD_HEIGHT/2 - blob_0->posy;
         
        rx1 = WORLD_WIDTH/2 - blob_1->posx;
        lx1 = -WORLD_WIDTH/2 - blob_1->posx;
        
        ty1 = -WORLD_HEIGHT/2 - blob_1->posy;
        by1 = WORLD_HEIGHT/2 - blob_1->posy; 
         
        // right line 
        GS_line(sync, SCREEN_P1, rx, ty, rx, by, 16711680);
        // left line
        GS_line(sync, SCREEN_P1, lx, ty, lx, by, 16711680);
        // top line
        GS_line(sync, SCREEN_P1, lx, ty, rx, ty, 16711680);
        // bottom line
        GS_line(sync, SCREEN_P1, lx, by, rx, by, 16711680);
        
        //right line 
        GS_line(sync, SCREEN_P2, rx1, ty1, rx1, by1, 16711680);
        //left line
        GS_line(sync, SCREEN_P2, lx1, ty1, lx1, by1, 16711680);
        //top line
        GS_line(sync, SCREEN_P2, lx1, ty1, rx1, ty1, 16711680);
        //bottom line
        GS_line(sync, SCREEN_P2, lx1, by1, rx1, by1, 16711680);        
        


        
        // Update the screens by calling GS_update.
        GS_update(sync);
        
        // If a button on either side is pressed, the corresponding LED on the player 1 mbed will toggle.
        // This is just for debug purposes, and to know that your button pushes are being registered.
        led1 = p1_buttons[0] ^ p2_buttons[0];
        led2 = p1_buttons[1] ^ p2_buttons[1];
        led3 = p1_buttons[2] ^ p2_buttons[2];
        led4 = p1_buttons[3] ^ p2_buttons[3];
    }
    
    
    GS_cls(sync, SCREEN_BOTH);
    GS_textbackground_color(sync, SCREEN_BOTH, BGRD_COL);  
    green = 0;
    red = 0;
    playSound(blop);
    
    //display winner
    if(tie){
        
        GS_locate(sync, SCREEN_BOTH, 7, 2);
        GS_puts(sync, SCREEN_BOTH,"Tie!" , 5);
      
    } else if (!(winner)) {

        GS_locate(sync, SCREEN_BOTH, 2, 2);
        GS_puts(sync, SCREEN_BOTH,"Player 1 wins!" , 15);
        
    } else {
        
        GS_locate(sync, SCREEN_BOTH, 2, 2);
        GS_puts(sync, SCREEN_BOTH,"Player 2 wins!" , 15);
 
    }
    //display options
    GS_locate(sync, SCREEN_BOTH, 1, 4);

    GS_puts(sync, SCREEN_BOTH,"Continue ? [Y/N]" , 17);

    GS_locate(sync, SCREEN_BOTH, 3, 6);

    uLCD.printf("***Scores***");
    GS_puts(sync, SCREEN_BOTH,"***Scores***" , 13);

    GS_locate(sync, SCREEN_BOTH, 3, 9);

    GS_puts(sync, SCREEN_BOTH,"PL1" , 4);

    GS_locate(sync, SCREEN_BOTH, 9, 9);

    GS_puts(sync, SCREEN_BOTH,"PL2" , 4);



    
    
    if (length < 4) {
        end = end + 1;    
    }
    
    for (int i = 0; i < end; i++) {
        tempList[i] = scoreList[i];
        tempList1[i] = scoreList1[i];    
    }
    
    for (int i = 0; i< end - 1; i++) {
        scoreList[i+1] = tempList[i];
        scoreList1[i+1] = tempList1[i];

    }  

    scoreList[0] = score;    
    scoreList1[0] = score1;    
    
    int row = 11;
    //display scores
    for (int i = 0; i < end; i++){
        GS_locate(sync, SCREEN_BOTH, 0, row);
        

        char str1 [7];
        int str1_n = sprintf(str1, "[%d] %d", (i+1), scoreList[i]); 
        GS_puts(sync, SCREEN_BOTH, str1, str1_n + 1);
        
        GS_locate(sync, SCREEN_BOTH, 10, row);

        char str2  [2];
        int str2_n = sprintf(str2, "%d", scoreList1[i]);
        GS_puts(sync, SCREEN_BOTH, str2, str2_n + 1);

        row++; 
         GS_update(sync);
      
    }
    length++;
    
    
    
        while(true){
            if(!pb_u) {
                cont = true;
                break;    
            } 
            if(!pb_d) {
                cont = false;
                break;    
            }      
        }
    }
    GS_cls(sync, SCREEN_BOTH);
    GS_textbackground_color(sync, SCREEN_BOTH, BGRD_COL);
  
    GS_locate(sync, SCREEN_BOTH, 7, 5);

    GS_puts(sync, SCREEN_BOTH, "Bye :)" , 7);
    GS_update(sync);


}