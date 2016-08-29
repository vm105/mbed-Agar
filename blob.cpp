/**
 *  Created by Varun Malhotra
 *
 */

#include "blob.h"
#include "mbed.h"

extern Serial pc;


// Randomly initialize a blob's position, velocity, color, radius, etc.
// Set the valid flag to true and the delete_now flag to false.
// delete_now is basically the derivative of valid. It goes true for one
// fram when the blob is deleted, and then it is reset to false in the next frame
// when that blob is deleted.

int positions_x [22];
int length = 0;

int positions_y [22];


void BLOB_init(BLOB* b) {
    // ***
//
//    int min = 21, max = 32, r;
//    srand(time(NULL));
//    r = (rand() % (max + 1 - min)) + min;

    int rx = rand() % (513) + (-256);
    int ry = rand() % (513) + (-256);
    
    // prevent blobs from overlapping
    if (length){   
        
        for (int k = 0; k<length; k++) {
            if ((positions_x[k] - 10) <= rx && rx <=(positions_x[k] + 10)) {
                rx += 21;
            }
            if ((positions_y[k] - 10) <= ry && ry <=(positions_y[k] + 10)) {
                ry += 21;
            }
        }
    }
    positions_x[length] = rx;
    positions_y[length] = ry;
    length++;

    
    
    
    b->valid = true;
    b->delete_now = false;
    b->posx = rx;
    
    //r = (rand() % (max + 1 - min)) + min;

    b->posy = ry;
    b->old_x = b->posx;
    b->old_y = b->posy;
    b->vx = 100;
    b->vy = 100;
    b->color = 0x00FF00;
    b->rad = 5;

    
}


// Take in a blob and determine whether it is inside the world.
// If the blob has escaped the world, put it back on the edge
// of the world and negate its velocity so that it bounces off
// the boundary. Use WORLD_WIDTH and WORLD_HEIGHT defined in "misc.h"
void BLOB_constrain2world(BLOB* b) {
    // ***


// for x



    int w = WORLD_WIDTH/2;
    int h = WORLD_HEIGHT/2;
    


    
    
    
    
        if ((b[0].posx + b[0].rad) >= (WORLD_WIDTH/2)) {
            b[0].posx = WORLD_WIDTH/2 - b[0].rad;
            b[0].vx = 0;
        }
        
        if ((b[0].posx - b[0].rad) <= - (WORLD_WIDTH/2)) {
             b[0].vx = 0;
             b[0].posx = -WORLD_WIDTH/2 + b[0].rad;
             
        }
        
        if ((b[0].posy + b[0].rad) >=  (WORLD_HEIGHT/2)) {
            b[0].posy =  (WORLD_HEIGHT/2) - b[0].rad;
            b[0].vy = 0;
            
        }
        
        if ((b[0].posy - b[0].rad) < -  (WORLD_HEIGHT/2)) {
             b[0].posy = - (WORLD_HEIGHT/2) + b[0].rad;
             b[0].vy = 0;
       
        }
    
    
    
    
    
    
    
    
    
    
    b++;
        
    
    
    for (int i = 1; i < 22; i++) {
    
        if ((b->posx + b->rad) >= (w)) {
            b->posx = w - b->rad;
            b->vx = (-1 * b->vx);
        }
        
        if ((b->posx - b->rad) <= -w) {
             b->vx = -1*b->vx;
             b->posx = -w + b->rad;
             
        }
        
        if ((b->posy + b->rad) >=  (h)) {
            b->posy =  (h) - b->rad;
            b->vy = -1*b->vy;
            
        }
        
        if ((b->posy - b->rad) <  -h) {
             b->posy = -h + b->rad;
             b->vy = -1* b->vy;
       
        }
        b++;
    
    }

        
    
}

// Randomly initialize a blob. Then set the radius to the provided value. 
void BLOB_init(BLOB* b, int rad) {
    // *** DONT NEED
}

// Randomly initialize a blob. Then set the radius and color to the 
// provided values.
void BLOB_init(BLOB* b, int rad, int color) {
    
    int rx = rand() % (257) + (-128);
    int ry = rand() % (257) + (-128);
    
    // prevent blobs from overlapping
    if (length){   
        
        for (int k = 0; k<length; k++) {
            if ((positions_x[k] - 10) <= rx && rx <=(positions_x[k] + 10)) {
                rx += 21;
            }
            if ((positions_y[k] - 10) <= ry && ry <=(positions_y[k] + 10)) {
                ry += 21;
            }
        }
    }
    
    positions_x[length] = rx;
    positions_y[length] = ry;
    length++;    
    
    b->valid = true;
    b->delete_now = false;
    b->posx = rx;
    b->posy = ry;
    b->vx = 2;
    b->vy = 2;
    b->color = color;
    b->rad = rad;

}

// For debug purposes, you can use this to print a blob's properties to your computer's serial monitor.
void BLOB_print(BLOB b) {
    pc.printf("(%f, %f) <%f, %f> Color: 0x%x\n", b.posx, b.posy, b.vx, b.vy, b.color);
}

// Return the square of the distance from b1 to b2
float BLOB_dist2(BLOB b1, BLOB b2) {
    // ***
}
