//--headers-------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <conio.h>

//--configurations------------------------------------------------------------------------
#define DEBUG 0
#define height 20                       //Display height (int > 0)
#define width 60                       //Display width (best if a*height)
#define background_density -1           //Display dotted background (-1 for none)
int     axis_density = 1;               //Pixel/graph display proportion (int > 0) (also known as Zoom)
#define axis_numbered_graduation 1      //whether to display a value on each graduation (0-1)   ///broken
#define axis_graduation_size 0        //axis number indication size extension (int <= 0)
#define line_precision 100              //Number of repeated calculations (int > 0)
#define numbered_precision 0            //whether to display the calculation index or not (0-1)
#define error_tolerance 0.001           //error tolerance on mode 1 calculations (float)
#define mode 0                       //0 = function, 1 = general
#define graph_character '@'

//Global Variables------------------------------------------------------------------------
char getchKey;
int xOffset = 0;
int yOffset = 0;
int moveSpeed = 1;

//Display pixels values
char pixel[width][height];
int pixelx, pixely;

//function used on mode 0, input the formula in return
#define aMax 2
#define aMin -2
#define aStep 0
#define aStart 1
float function(float x, float a){
    return 2*sin(a*pow(x,2));
}

//procedure to clear all screen and draw borders
void clearScreen(){
    for(int i = 0; i < width; i++){ //horizontal borders walls
        pixel[i][0] = 'h';
        pixel[i][height-1] = 'h';
    }
    for(int i = 0; i < height; i++){ //vertical borders walls
        pixel[0][i] = 'v';
        pixel[width-1][i] = 'v';
    }
    for(int i = 1; i < width-1; i++){ //fill screen background
        for(int j = 1; j < height-1; j++){
            if((i+j)%background_density == 1) pixel[i][j] = '.';
            else pixel[i][j] = ' ';
        }
    }
}

//print all pixels on the console
void printScreen(){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("%c", pixel[j][i]);
        }
        printf("\n");
    }
}

//draw the xy axis lines (x axis is 2* wider to compensate tall character dimensions)
void drawAxis(){
    int value = -height/2;
    pixelx = width/2 - xOffset*axis_density*2;
    if(pixelx < 0) pixelx = 0;
    if(pixelx >= width) pixelx = width-1;
    for(int i = 0; i < height; i++){ //y axis
        if(value%axis_density==0 ){
            if(axis_numbered_graduation == 0) pixel[pixelx][i] = '+';
            else pixel[pixelx][i] = abs(value/axis_density-yOffset)%10 + '0';
            for(int j = 1; j <= axis_graduation_size*2; j++){
                if((width/2)-j > 0){
                    pixel[(width/2)+j - xOffset*axis_density*2][i] = '-';
                    pixel[(width/2)-j - xOffset*axis_density*2][i] = '-';
                }
            }
        }
        else pixel[pixelx][i] = '|';
        value++;
    }
    value = -width/2;
    pixely = height/2 + yOffset*axis_density;
    if(pixely < 0) pixely = 0;
    if(pixely >= height) pixely = height-1;
    for(int i = 0; i < width; i++){ //x axis
        if(value%(axis_density*2)==0) {
            if(axis_numbered_graduation == 0) pixel[i][pixely] = '+';
            else pixel[i][pixely] = abs(value/(axis_density*2)+xOffset)%10 + '0';
            for(int j = 1; j <= axis_graduation_size; j++){
                if((height/2)-j > 0){
                    pixel[i][(height/2)+j + yOffset*axis_density] = '|';
                    pixel[i][(height/2)-j + yOffset*axis_density] = '|';
                }
            }
        }
        else pixel[i][pixely] = '-';
        value++;
        pixel[width/2][height/2] = 'X';
    }
}

//draw the graph 
void drawGraph(float a){
    double x, y;
    switch(mode){
        case 0: ; //mode 0: checking each graph x values for display pixels
            x = -(width)/(axis_density*4.0)+xOffset;                //x starts on the further left
            for(int i = 0; i < width; i++){                 //i = x equivalent of pixel position
                for(int j = 0; j < line_precision; j++){    //x precision subdivisions
                    //y pixel calculation: y_further_up-f(x + precision_subdivision)*proportion
                    y = round(-(function(x+ (j*(1.0/(axis_density*2.0))/line_precision -1.0/(axis_density*4.0)) , a)*axis_density)+((height)/2.0))+yOffset*axis_density;
                    if(y >= 0 && y <= height-1){            //if inside display, draw it
                        if(numbered_precision) pixel[i][(int)y] = j + '0';
                        else pixel[i][(int)y] = graph_character;
                    }
                }
                x += (float)(1.0/(axis_density*2.0));       //i++, x increment equivalent
            }
        break;
        case 1: ; //mode 1: checking each display pixels for graph correlation
            for(int i = 0; i < height; i++){                    //for every vertical pixel i
                for(int i1 = 0; i1 < line_precision; i1++){     //y precision subdivisions
                    //calculate value equivalent of y in that pixel height for every i and subdivisions
                    y = (height)/(2.0*axis_density) -(float)i/(axis_density) +(i1*(1.0/(axis_density*2.0))/line_precision) -1.0/(axis_density*4.0);
                    if(DEBUG) printf("y%d: %f \n", i, y);       //for every vertical pixel j
                    for(int j = 0; j < width; j++){             //y precision subdivisions
                        for(int j1 = 0; j1 < line_precision; j1++){
                            //calculate value equivalent of x in that pixel height for every j and subdivisions
                            x = (float)j/(axis_density)/2.0 -(width)/(2.0*axis_density)/2.0 +(j1*(1.0/(axis_density))/line_precision) -1.0/(axis_density*2.0);
                            if(DEBUG)printf("x%d: %f \n", j, x);
                            //y=x -> y-x~0 //input the equation in the fabs funcion
                            if(fabs( pow(pow(y,2)+pow(x,2),2)-a*(pow(x,2)-pow(y,2)) ) < error_tolerance) { //check if pixel is in the graph
                                if(numbered_precision) pixel[j][i] = j1 + '0';
                                else pixel[j][i] = graph_character;
                                if(DEBUG) printf("yay");
                            }
                       }
                    }
                }
            }
        break;
    }
}

void inputUpdate(){
    if(kbhit() || aStep == 0){
    printf("w,a,s,d to move; o,p to zoom; u,i to change speed");
    getchKey = getch();
    
    switch(getchKey){
        case 'p': axis_density++; break;
        case 'o': axis_density--; break;
        case 'w': yOffset+=moveSpeed; break;
        case 'a': xOffset-=moveSpeed; break;
        case 's': yOffset-=moveSpeed; break;
        case 'd': xOffset+=moveSpeed; break;
        case 'u': moveSpeed--; break;
        case 'i': moveSpeed++; break;
    }
    if(axis_density < 1) axis_density = 1;
    }
}

int main(){
    struct timespec ts, ts2; //request, remaining
    ts.tv_nsec = 200000000L; //delay in nanoseconds (long int)
    ts.tv_sec = 0;           //delay in seconds

    float va = aStep;
    float a = aStart;
    for(;; a+=va){
    
        inputUpdate();
        clearScreen();
        drawAxis();
        drawGraph(a);
        system("cls");
        printf("a = %.2f da = %d  (z = %d; x = %d; y = %d) [dxy = %d]\n", a, aStep, axis_density, xOffset, yOffset, moveSpeed);
        printScreen();

        if(aStep != 0) nanosleep(&ts, &ts2);
        if(a > aMax || a < aMin) va *= -1;
    }
}

//Calculation break down
//y = (height)/(2.0*axis_density)               //y starts at top of the display, its value defined by proportion
//-(float)i/(axis_density)                      //each i lowers y until its on the bottom of the display
//+(i1*(1.0/(axis_density*2.0))/line_precision) //for each i the value is subdivided by <precision> up and down
//-1.0/(axis_density*4.0);                      //starting from lower values and incrementing to top values by i1