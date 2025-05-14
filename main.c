#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> 

#define WIDTH 8
#define HEIGHT 8

int startx,starty;
int endx,endy;

//arrays that will be used to check neighbor cells
int dx[8] = {1,1,0,-1,-1,-1,0,1};
int dy[8] = {0,1,1,1,0,-1,-1,-1};

char symbols[6] = " #SE.x";
//empty node number = 0
//wall node number = 1
//start node number = 2
//end node number = 3

typedef struct {
    int x;
    int y;
    int nodetype;
    int parentx;
    int parenty;
    int isopen; //open means to be further explored
    int isclosed; //closed means this path has been explored before
    float g; // g stands for the cost required to get to this cell
} node;

node grid[HEIGHT][WIDTH];

float noise(int x, int y) {
    int n;

    n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0 - ( (n * ((n * n * 15731) + 789221) +  1376312589) & 0x7fffffff) / 1073741824.0);
}

float eucl_dist(int x1, int y1, int x2, int y2) {
    return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void initGrid(){

    srand(time(NULL));

    int randomseed = rand()%70;

    int x,y;
    for (y=0;y<HEIGHT;y++){
    
        for (x=0;x<WIDTH;x++){
        
            grid[y][x].x = x;
            grid[y][x].y = y;
            grid[y][x].nodetype = noise(x+randomseed,y+randomseed) + 0.5 ;
            grid[y][x].parentx = -1;
            grid[y][x].parenty = -1;
            grid[y][x].isopen = 0;
            grid[y][x].isclosed = 0;
            grid[y][x].g = 0;

        }  

    }

    startx = 0;
    starty = 0;
    endx = 7;
    endy = 7;

    grid[0][0].nodetype = 2;
    grid[0][0].isopen = 1;
    grid[7][7].nodetype = 3;

}

void printGrid(){

    int x,y;

    printf("   ");

    for (x=0;x<WIDTH;x++){
        printf("%d ",x);
    }
    printf("\n");

    for (y=0;y<HEIGHT;y++){
    
        printf("%d  ",y);

        for (x=0;x<WIDTH;x++){
        
            printf("%c ", symbols[grid[y][x].nodetype] );

        }


        printf("\n");

    }

}

/*
//check if given node is within the grid
                    if ( nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT ) {

                        printf("%d  %d  \n",nx,ny);

                        //setting neighbor to open to explore in future

                        grid[ny][nx].isopen == 1;
                        grid[ny][nx].isclosed == 0;

                    }

                    //checking for neighbors
                for (i=0;i<8;i++) {

                    //new x and new y values of neighbor node
                    int nx = x + dx[i];
                    int ny = y + dy[i];

                    float ng = grid[y][x]

                }
*/

void traceback(int x , int y){

    grid[y][x].nodetype = 5;

    if ( grid[y][x].parentx != -1 && grid[y][x].parenty != -1 ) {

        traceback( grid[y][x].parentx , grid[y][x].parenty );

    }

}

void a_star_step(){

    int reachedgoal = 0;

    int x,y;

    node* bestnode;
    float bestcost = 99999;
    
    for (y=0;y<HEIGHT;y++){

        for (x=0;x<WIDTH;x++){

            //checking if given node is open and can be explored and is NOT A WALL
            if (grid[y][x].isopen == 1 && grid[y][x].nodetype != 1) {

                float ng = grid[y][x].g;
                float nh = eucl_dist(x,y,endx,endy);
                float nc = nh + ng; //to minimize this cost

                if (nc <= bestcost) {

                    bestcost = nc;
                    bestnode = &grid[y][x];

                }
            
            }

        }

    }

    bestnode->isopen=0;
    bestnode->isclosed=1;

    bestnode->nodetype=4;

    int i;

    //checking for neighbors
    for (i=0;i<8;i++) {

        //new x and new y values of neighbor node
        int nx = bestnode->x + dx[i];
        int ny = bestnode->y + dy[i];

        //printf("%d  %d\n",nx,ny);

        if ( nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && grid[ny][nx].isclosed == 0 ) {

            float ng = grid[y][x].g + eucl_dist(x,y,nx,ny);
            
            if ( (grid[ny][nx].isopen == 1 && ng <= grid[ny][nx].g) || (grid[ny][nx].isopen == 0) ){

                grid[ny][nx].parentx = bestnode->x;
                grid[ny][nx].parenty = bestnode->y;

            }

            grid[ny][nx].isopen = 1;
            grid[ny][nx].isclosed = 0;
            grid[ny][nx].g = ng;

            if (nx==endx && ny==endy) {
                reachedgoal = 1;   
            }

        }

    }
    
    if ( reachedgoal == 0 ){
        printGrid();
        getchar();
        a_star_step();
    }else{
        traceback(bestnode->x,bestnode->y);
        printGrid();
    }

}

int main() {

    initGrid();

    printGrid();

    a_star_step();

    return 0;

}