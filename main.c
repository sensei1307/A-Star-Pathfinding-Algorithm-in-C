#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> 

#define WIDTH 9
#define HEIGHT 9

int startx,starty;
int endx,endy;

int distancemode = 0;
int obstaclemode = 0;

//arrays that will be used to check neighbor cells
int dx[8] = {1,1,0,-1,-1,-1,0,1};
int dy[8] = {0,1,1,1,0,-1,-1,-1};

//non-diagonal
int dnx[4] = {1,0,-1,0};
int dny[4] = {0,1,0,-1};

char symbols[] = " #SE.x";
//empty node number = 0
//wall node number = 1
//start node number = 2
//end node number = 3
//tracing node number 4
//final path node number 5

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

float dist(int x1, int y1, int x2, int y2) {

    switch (distancemode) {
        case 0:
        //euclidean
        return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        break;
        case 1:
        //manhattan
        return abs(x2-x1) + abs(y2-y1);
        break;
    }

}

void generateMaze(int x, int y){

    //29th may 2025: Algorithm
    //choose a random cell
    //choose a random direction with a wall within bounds
    //if there is a wall then knock down the wall between the new and the old wall
    //recursively run this function at the new cell as well

    grid[y][x].nodetype = 0;

    int dirs[4] = {0, 1, 2, 3};
    srand(time(NULL));

    int i;
    for (i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = dirs[i];
        dirs[i] = dirs[j];
        dirs[j] = temp;
    }

    for (i=0; i<4; i++){

        int nx = x + dnx[ dirs[i] ];
        int ny = y + dny[ dirs[i] ];

        int ndx = x + 2*dnx[ dirs[i] ];
        int ndy = y + 2*dny[ dirs[i] ];

        if (( ndx >= 0 && ndx < WIDTH && ndy >= 0 && ndy < HEIGHT ) && ( grid[ndy][ndx].nodetype == 1 ) ) {

            grid[ny][nx].nodetype = 0;

            generateMaze( ndx , ndy );

        }            

    }

}

void initGrid(){

    srand(time(NULL));

    int randomseed = rand()%70;

    int x,y;
    for (y=0;y<HEIGHT;y++){
    
        for (x=0;x<WIDTH;x++){
        
            grid[y][x].x = x;
            grid[y][x].y = y;
            grid[y][x].nodetype = 1;
            grid[y][x].parentx = -1;
            grid[y][x].parenty = -1;
            grid[y][x].isopen = 0;
            grid[y][x].isclosed = 0;
            grid[y][x].g = 0;

        }  

    }

    switch (obstaclemode) {

        case 0:
            for (y=0;y<HEIGHT;y++){
        
                for (x=0;x<WIDTH;x++){
                    grid[y][x].nodetype = noise(x+randomseed,y+randomseed) + 0.5 ;
                }  

            }
        break;
        
        case 1:
            generateMaze( 1+2*(rand()%((WIDTH-1)/2)) , 1+2*(rand()%((HEIGHT-1)/2)) );
        break;

    }

}

void setStartAndEnd(){
    grid[starty][startx].nodetype = 2;
    grid[starty][startx].isopen = 1;
    grid[endy][endx].nodetype = 3;
}

void printGrid(){

    int x,y;

    printf("    ");

    for (x=0;x<WIDTH;x++){
        printf("%02d  ",x);
    }
    printf("\n");

    for (y=0;y<HEIGHT;y++){
    
        printf("%02d   ",y);

        for (x=0;x<WIDTH;x++){
        
            printf("%c   ", symbols[grid[y][x].nodetype] );

        }


        printf("\n\n");

    }

}

void traceback(int x , int y){

    grid[y][x].nodetype = 5;

    if ( grid[y][x].parentx != -1 && grid[y][x].parenty != -1 ) {

        traceback( grid[y][x].parentx , grid[y][x].parenty );

        int dirx = grid[y][x].parentx - x;
        int diry = grid[y][x].parenty - y;

        int p = (atan2(diry,-dirx)*180/M_PI)/45;

        if (p<0) {
            p = 7 - abs(p);
        }

        //grid[y][x].nodetype = 6 + p;

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
                float nh = dist(x,y,endx,endy);
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

            float ng = grid[y][x].g + dist(x,y,nx,ny);
            
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

    printf("Enter obstacle generation mode:\n 0) Perlin noise\n 1) Maze generation");
    scanf("%d",&obstaclemode);

    printf("Enter distance mode:\n 0) Euclidean distance\n 1) Manhattan distance");
    scanf("%d",&distancemode);

    initGrid();

    printGrid();

    printf("Enter x [space] y coordinates for start: 0 to %d: ", WIDTH - 1 );
    scanf("%d%d",&startx,&starty);

    printf("Enter x [space] y coordinates for start: 0 to %d: ", WIDTH - 1 );
    scanf("%d%d",&endx,&endy);

    setStartAndEnd();

    a_star_step();

    return 0;

}
