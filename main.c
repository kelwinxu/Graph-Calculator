//--headers-------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//--configurations------------------------------------------------------------------------
#define DEBUG 0
#define height 80                       //Display height (int > 0)
#define width 240                       //Display width (best if a*height)
#define background_density -1           //Display dotted background (-1 for none)
#define axis_density 10                 //Pixel/graph display proportion (int > 0)
#define axis_numbered_graduation 0      //whether to display a value on each graduation (0-1)   ///broken
#define axis_graduation_size 1          //axis number indication size extension (int <= 0)
#define line_precision 100              //Number of repeated calculations (int > 0)
#define numbered_precision 0            //whether to display the calculation index or not (0-1)
#define error_tolerance 0.001           //error tolerance on mode 1 calculations (float)
#define mode 0                          //0 = function, 1 = general
#define graph_character '.'

//Display pixels values
char pixel[width][height];

//function used on mode 1, input the formula in return
float function(float x, float a){
    return pow(tan(a*sqrt(x)),3);
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
    int value = -width/2;
    for(int i = 0; i < height; i++){ //y axis
        if(value%axis_density==0){
            if(axis_numbered_graduation == 0) pixel[width/2][i] = '+';
            else pixel[width/2][i] = abs(value/axis_density)%10 + '0';
            for(int j = 1; j <= axis_graduation_size*2; j++){
                if((width/2)-j > 0){
                    pixel[(width/2)+j][i] = '-';
                    pixel[(width/2)-j][i] = '-';
                }
            }
        }
        else pixel[width/2][i] = '|';
        value++;
    }
    for(int i = 0; i < width; i++){ //x axis
        if(value%(axis_density*2)==0) {
            if(axis_numbered_graduation == 0) pixel[i][height/2] = '+';
            else pixel[i][height/2] = abs(value/(axis_density*2))%10 + '0';
            for(int j = 1; j <= axis_graduation_size; j++){
                if((height/2)-j > 0){
                    pixel[i][(height/2)+j] = '|';
                    pixel[i][(height/2)-j] = '|';
                }
            }
        }
        else pixel[i][height/2] = '-';
        value++;
    }
    value = -height/2;
}

//draw the graph 
void drawGraph(float a){
    double x, y;
    switch(mode){
        case 0: ; //mode 0: checking each graph x values for display pixels
            x = -(width)/(axis_density*4.0);                //x starts on the further left
            for(int i = 0; i < width; i++){                 //i = x equivalent of pixel position
                for(int j = 0; j < line_precision; j++){    //x precision subdivisions
                    //y pixel calculation: y_further_up-f(x + precision_subdivision)*proportion
                    y = round(-(function(x+ (j*(1.0/(axis_density*2.0))/line_precision -1.0/(axis_density*4.0)) , a)*axis_density)+((height)/2.0));
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

int main(){
    struct timespec ts, ts2; //request, remaining
    ts.tv_nsec = 100000000L; //delay in nanoseconds (long int)
    ts.tv_sec = 0;           //delay in seconds

    float va = 0.3;
    float a = 1;
    for(;; a+=va){
        clearScreen();
        drawAxis();
        drawGraph(a);
        system("cls");
        printf("a = %.2f\n", a);
        printScreen();

        nanosleep(&ts, &ts2);
        if(a > 6 || a < -6) va *= -1;
    }
}

//Calculation break down
//y = (height)/(2.0*axis_density)               //y starts at top of the display, its value defined by proportion
//-(float)i/(axis_density)                      //each i lowers y until its on the bottom of the display
//+(i1*(1.0/(axis_density*2.0))/line_precision) //for each i the value is subdivided by <precision> up and down
//-1.0/(axis_density*4.0);                      //starting from lower values and incrementing to top values by i1

