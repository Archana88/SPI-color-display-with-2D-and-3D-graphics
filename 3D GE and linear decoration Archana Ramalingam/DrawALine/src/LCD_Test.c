/*
=================================================================================
 Name        : SSP_Test_lcd.c
 Author      : Archana Ramalingam
 Version     :
 Copyright   : $(copyright)
 Description : This program allows us, using the 1.8" Color TFT LCD via ssp ports,
 	 	 	   to be able to display Tree pattern, 3D world coordinates and 3D cube on the screen
=================================================================================
*/

#include <NXP/crp.h>

// Variable to store CRP value. Placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

#include "LPC17xx.h"
#include "ssp.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static unsigned int width = 100;
static unsigned int height = 134;

// Port number is 1 for the circuit used here

#define PORT_NUM 1
#define LOCATION_NUM 0

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];
int colstart = 0;
int rowstart = 0;

/*****************************************************************************
** Function name:       LCD_TEST
**
** Descriptions:        Draw line function
**
** parameters:            None
** Returned value:        None
**
*****************************************************************************/
// LCD
#define ST7735_TFTWIDTH  127
#define ST7735_TFTHEIGHT 159
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define swap(x, y) { x = x + y; y = x - y; x = x - y; }

// Colours
#define GREEN 0x00FF00
#define BLACK 0x000000
#define RED 0xFF0000
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define PINK 0xFFC0CB
#define PURPLE 0x800080
#define YELLOW 0xFFFF00
#define LIME 0x00FF00
#define MAGENTA 0xFF00FF
#define CYAN 0x00FFFF
#define SILVER 0xC0C0C0
#define GREY 0x808080
#define ORANGE 0xFFA500
#define BROWN 0xA52A2A
#define MAROON 0x800000

// Axes
int _height = 320;
int _width = 240;
int cursor_x = 0, cursor_y = 0;

// To write data into the SPI
void spiwrite(uint8_t c)
{
	int portnum = 1;
	src_addr[0] = c;
	SSP_SSELToggle( portnum, 0 );
	SSPSend( portnum, (uint8_t *)src_addr, 1 );
	SSP_SSELToggle( portnum, 1 );
}

// To write commands into the SPI
void writecommand(uint8_t c) {
	LPC_GPIO0->FIOCLR |= (0x1<<21);
	spiwrite(c);
}

// To make LCD ready to write data
void writedata(uint8_t c) {

	LPC_GPIO0->FIOSET |= (0x1<<21);
	spiwrite(c);
}

// To write data to the LCD
void writeword(uint16_t c) {
	uint8_t d;
	d = c >> 8;
	writedata(d);
	d = c & 0xFF;
	writedata(d);
}

// To write color
void write888(uint32_t color, uint32_t repeat) {
	uint8_t red, green, blue;
	int i;
	red = (color >> 16);
	green = (color >> 8) & 0xFF;
	blue = color & 0xFF;
	for (i = 0; i< repeat; i++) {
	writedata(red);
	writedata(green);
	writedata(blue);
	}
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1) {

	 writecommand(ST7735_CASET);
	 writeword(x0);
	 writeword(x1);
	 writecommand(ST7735_RASET);
	 writeword(y0);
	 writeword(y1);
}

// To draw a Pixel
void drawPixel(int16_t x, int16_t y, uint32_t color) {
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

    setAddrWindow(x,y,x+1,y+1);
    writecommand(ST7735_RAMWR);
    write888(color, 1);
}

// For LCD delay
void lcddelay(int ms)
{
	int count = 24000;
	int i;

	for ( i = count*ms; i--; i > 0);
}

// Initialize LCD
void lcd_init()
{
	/*
	 * portnum = 0 ;
	 * cs = p0.16 / p0.6 ?
	 * rs = p0.21
	 * rst = p0.22
	 */
	uint32_t portnum = 1;
	int i;
	/* Notice the hack, for portnum 0 p0.16 is used */
	if ( portnum == 0 )
	 {
	LPC_GPIO0->FIODIR |= (0x1<<16); /* SSP1, P0.16 defined as Outputs */
	 }
	 else
	 {
	LPC_GPIO0->FIODIR |= (0x1<<6); /* SSP0 P0.6 defined as Outputs */
	 }
	/* Set rs(dc) and rst as outputs */
	LPC_GPIO0->FIODIR |= (0x1<<21); /* rs/dc P0.21 defined as Outputs */
	LPC_GPIO0->FIODIR |= (0x1<<22); /* rst P0.22 defined as Outputs */


	/* Reset sequence */
	LPC_GPIO0->FIOSET |= (0x1<<22);

	lcddelay(500); /*delay 500 ms */
	LPC_GPIO0->FIOCLR |= (0x1<<22);
	lcddelay(500); /* delay 500 ms */
	LPC_GPIO0->FIOSET |= (0x1<<22);
	lcddelay(500); /* delay 500 ms */
	for ( i = 0; i < SSP_BUFSIZE; i++ ) /* Init RD and WR buffer */
	   {
	   src_addr[i] = 0;
	   dest_addr[i] = 0;
	   }

	/* Sleep out */
	SSP_SSELToggle( portnum, 0 );
	src_addr[0] = 0x11; /* Sleep out */
	SSPSend( portnum, (uint8_t *)src_addr, 1 );
	SSP_SSELToggle( portnum, 1 );

	lcddelay(200);
	/* delay 200 ms */
	/* Disp on */
	SSP_SSELToggle( portnum, 0 );
	src_addr[0] = 0x29; /* Disp On */
	SSPSend( portnum, (uint8_t *)src_addr, 1 );
	SSP_SSELToggle( portnum, 1 );
	/* delay 200 ms */
	lcddelay(200);
}

// To fill a rectangle, with given parameters, with a given color
void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{
	int16_t i;
	int16_t width, height;

	width = x1-x0+1;
	height = y1-y0+1;
	setAddrWindow(x0,y0,x1,y1);
	writecommand(ST7735_RAMWR);
	write888(color,width*height);
}

// Draw line function in 2D
void drawLine2d(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color) {
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
	swap(x0, y0);
	swap(x1, y1);
	}
	if (x0 > x1) {
	swap(x0, x1);
	swap(y0, y1);
	}
	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);
	int16_t err = dx / 2;
	int16_t ystep;
	if (y0 < y1) {
	ystep = 1;
	} else {
	ystep = -1;
	}
	for (; x0 <= x1; x0++) {
	if (steep) {
	drawPixel(y0, x0, color);
	} else {
	drawPixel(x0, y0, color);
	}
	err -= dy;
	if (err < 0) {
	y0 += ystep;
	err += dx;
	}
	}
}

// To draw a horizontal line
void HLine(int16_t x0,int16_t x1,int16_t y,uint16_t color){
	if(x0<x1){
	width = x1-x0+1;
	setAddrWindow(x0,y,x1,y);
	}else{
	width = x0-x1+1;
	setAddrWindow(x1,y,x0,y);
	}
	writecommand(ST7735_RAMWR);
	write888(color,width);
}

// To draw a vertical line
void VLine(int16_t x,int16_t y0,int16_t y1,uint16_t color){
	if(y0<y1){
	width = y1-y0+1;
	setAddrWindow(x,y0,x,y1);
	}else{
	width = y0-y1+1;
	setAddrWindow(x,y1,x,y0);
	}
	writecommand(ST7735_RAMWR);
	write888(color,width);
}

// Draw line function
void drawLine(float x1, float y1,float x2, float y2,uint32_t color)
{
	float x,y;
	double slope;

	// Draw a vertical line
	if((int32_t)x1==(int32_t)x2){
	VLine(x1,y1,y2,color);
	}
	// Draw a horizontal line
	else if((int32_t)y1==(int32_t)y2){
	HLine(x1,x2,y1,color);
	}
	// Draw a non-horizontal & non-vertical line
	else{
		slope = (y2-y1)/(x2-x1);
		for(x=((x1>x2)?x2:x1);x<(((x1>x2)?x1:x2)+1);x++) {
			y = slope*(x-((x1>x2)?x2:x1))+((x1>x2)?y2:y1);
			drawPixel(x,(y),color);
		}
	}
}

// Rotate the point by the given angle (R-Rotation matrix)
pointrotate(int *x2,int *y2,int x1, int y1,int angle){
	int xt,yt,xr,yr;
	float a,b,r;

	r = angle*(3.14159265359/180);

	// Translated x,y coordinates
	xt = *x2 - x1;
	yt = *y2 - y1;

	a = cos(r);
	b = sin(r);

	// Rotated x,y coordinates
	xr = xt * a - yt * b;
	yr = xt * b + yt * a;

	*x2 = xr + x1;
	*y2 = yr + y1;
	return;
}

//To draw a square in 2D
void drawSqaure2d(int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4)
{
	int x1_1=0, x2_1=0, x3_1=0, x4_1=0, y1_1=0,y2_1=0, y3_1=0, y4_1=0;
	float lambda=0.8;

	// Draw 10 levels of squares in each pattern
	for(int i=1;i<=10;i++){
		lcddelay(100);
		// Use the equation given in class to calculate the 4 vertices' coordinates of the recursive squares
		x1_1 = (x2+(lambda*(x1-x2)));
		y1_1 = (y2+(lambda*(y1-y2)));
		x2_1 = (x3+(lambda*(x2-x3)));
		y2_1 = (y3+(lambda*(y2-y3)));
		x3_1 = (x4+(lambda*(x3-x4)));
		y3_1 = (y4+(lambda*(y3-y4)));
		x4_1 = (x1+(lambda*(x4-x1)));
		y4_1 = (y1+(lambda*(y4-y1)));

		// Draw a square (4 drawline() for 4 sides)
		drawLine(x1_1,y1_1,x2_1,y2_1,CYAN);
		drawLine(x2_1,y2_1,x3_1,y3_1,CYAN);
		drawLine(x4_1,y4_1,x3_1,y3_1,CYAN);
		drawLine(x1_1,y1_1,x4_1,y4_1,CYAN);

	  	// Initiate the original vertices' values with the new calculated vertices' values
		x1 = x1_1;
		x2 = x2_1;
		x3 = x3_1;
		x4 = x4_1;

		y1 = y1_1;
		y2 = y2_1;
		y3 = y3_1;
		y4 = y4_1;
	}
}

// To draw a square pattern on a 3D cube surface
void drawSquare3d(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color,int num)
{
	int16_t x1_1, y1_1, x2_1, y2_1;
	int a,b,c;
	int size = 20;

	if (num < 1)
	return;

	//Draw square with the rotated vertices by an angle
	// Calculate the rotated vertices
	drawLine(x1, y1, x1, y2, color);
	a = x1+size*cos(0.785);
	b = y2+size*sin(0.785);
	drawLine(x1, y2, a, b, color);
	c = y1+size*sin(0.785);
	drawLine(x2, y1, a, c, color);
	drawLine(a, c, a, b, color);
	lcddelay(100);

	// Draw a 2D square pattern using the computed vertices
	drawSqaure2d(x1,a,a,x1,y2,b,c,y1);
}

void drawTree2d(int x, int y, float angle, int length, int level, int color){

	int x1,y1,len;
	float ang;

	if(level>0){
		// To calculate the x,y vertices for the branch after rotation
		x1 = x+length*cos(angle);
	    y1 = y+length*sin(angle);

	    // To draw the tree branch
	    drawLine2d(x,y,x1,y1,color);

	    // Add 30 degree to angle to rotate it to right
	    ang = angle + 0.52;
	    // To calculate 80% of the line length
	    len = 0.8 * length;

	    // Call drawTree2d function recursively to draw tree pattern
	    drawTree2d(x1,y1,ang,len,level-1,color);

	    // Subtract 30 degree from the angle to rotate it to right
	    ang = angle - 0.52;
	    len = 0.8 * length;

	    drawTree2d(x1,y1,ang,len,level-1,color);

	    // Draw the next level
	    ang = angle;
	    len = 0.8 * length;
	    drawTree2d(x1,y1,ang,len,level-1,color);
	}
}

// Recursive function to draw a tree pattern
drawTree3d(int x1, int y1, int x2, int y2,uint16_t color, int num )
{
	int x3,y3,count;

	if(num<=0)
	return;

	// Move to next level, swap vertices
	x3 = x1, y3 = y1;
	x1 = x2 ,y1 = y2;

	// Calculate 80% point of length to draw branches
	y2 = y2+ (y1 - y3)*.8;
	x2 = x2+ (x1 - x3)*.8;

	// Use pointrotate function to rotate by the angle
	pointrotate(&x2,&y2,x1,y1,30);

	for(count=num*-1;count<num;count++){
		drawLine(x1+count,y1+count,x2+count,y2+count,color);
	}

	// Repeat by calling drawTree3d function recursively
	drawTree3d(x1,y1,x2,y2,YELLOW,num-1);
	pointrotate(&x2,&y2,x1,y1,330);

	for(count=num*-1;count<num;count++){
		drawLine(x1+count,y1+count,x2+count,y2+count,color);
	}

	drawTree3d(x1,y1,x2,y2,BLUE,num-1);
	pointrotate(&x2,&y2,x1,y1,330);

	for(count=num*-1;count<num;count++){
		drawLine(x1+count,y1+count,x2+count,y2+count,color);
	}

	drawTree3d(x1,y1,x2,y2,CYAN,num-1);
}

// To generate random x,y vertices
int random_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
	uint32_t  i, portnum = PORT_NUM;
	portnum = 1; /* For LCD use 1 */

    int16_t size = 20; // square size

	/* SystemClockUpdate() updates the SystemFrequency variable */
	// SystemClockUpdate();
	if ( portnum == 0 )
		SSP0Init(); /* initialize SSP port */
	else if ( portnum == 1 )
		SSP1Init();

	for (i = 0; i < SSP_BUFSIZE; i++ )
	  {
	    src_addr[i] = (uint8_t)i;
	    dest_addr[i] = 0;
	  }

	// To define range to avoid patterns display out of screen
	int xrange = ST7735_TFTWIDTH;
	int yrange = ST7735_TFTHEIGHT;

	// To randomize the colors of the patterns
	uint32_t colorList[] = {GREEN,RED,BLUE,WHITE,PINK,PURPLE,YELLOW,LIME,MAGENTA,CYAN,SILVER,GREY,ORANGE,BROWN,MAROON};
	int colorIndex;

    // Initialize the LCD
	lcd_init();

	printf("LCD initialized");

	/**************************Tree pattern**********************************/

	fillrect(0,0,ST7735_TFTWIDTH,ST7735_TFTHEIGHT,BLACK);
	fillrect(75,0,ST7735_TFTWIDTH,ST7735_TFTHEIGHT,BLUE);

	// Draw tree pattern for 5 levels
	for(int i=1;i<=5;i++)
	{
		// Randomize the vertex of the tree starting point
		int x1t=rand()%75;
		int y1t=rand()%150;
		float angt=rand()%1;
		int lent=rand()%20;
		int levt=(rand()%(10-7))+7;

		for(int i=-7;i<=7;i++){
		drawLine2d(x1t,y1t,x1t,y1t,YELLOW);
		}
		drawTree2d(x1t+i,y1t,angt,lent,levt,GREEN);

		for(int i=-7;i<=7;i++){
		drawLine2d(x1t,y1t,x1t,y1t,YELLOW);
		}
		drawTree2d(x1t+i,y1t,angt,lent,levt,GREEN);
	}

	while(getchar() != '\n')

	printf("\nTree pattern complete!");


	/************************** 3D world coordinate system ************************/

	fillrect(0, 0, 220, 220, BLACK);
	drawLine2d(95,159,30,100, RED); //X-axis - Red
	drawLine2d(127,80,30,100, GREEN); //Y-axis - Green
	drawLine2d(30,0,30,100, BLUE); //Z-axis - Blue

	while(getchar() != '\n');

	/************************** 3D cube with decoration**********************/

	fillrect(0, 0, 220, 220, BLACK); // To provide background color for the patterns to be visible

	int cubeSize =60; // Size of the cube

	// x and y vertices for the center of a surface
	int xcenter =14;
	int ycenter=30;

	// The range of the vertices for the surfaces S1, S2, S3
	int x1 = xcenter+cubeSize*cos(0.785); //49.36
	int y1 = (ycenter+cubeSize)+cubeSize*sin(0.785); //115.34
	int x2 = (xcenter+cubeSize)+cubeSize*cos(0.785); //99.36
	int y2 = (ycenter+cubeSize)+cubeSize*sin(0.785); //115.34

	y1 = y1-cubeSize;
	y2 = y2-cubeSize;


	// Draw the 3D world Coordinate axes
	drawLine2d(95,159,xcenter,ycenter+cubeSize,RED); //X-axis - Red
	drawLine2d(127,ycenter+cubeSize-10,xcenter,ycenter+cubeSize, GREEN); //Y-axis - Green
	drawLine2d(xcenter,0,xcenter,ycenter+cubeSize, BLUE); //Z-axis - Blue

	// To draw the 3 visible surfaces S1, S2, S3 of the 3D cube
	for(int i=0;i<cubeSize;i++){
		 drawLine(xcenter,ycenter+i,x1,y1+i,RED); // Surface S1
		 drawLine(x1,y1+i,x2,y2+i,CYAN); // Surface S2
		 drawLine(xcenter+i,ycenter,x1+i,y1,YELLOW); // Surface S3
	}

	 // To draw the font initial - 'A' (Archana)

	drawLine2d(42,32,55,55, BROWN); // '/'
	drawLine2d(42,32,80,50, PURPLE); // '\'
	drawLine2d(49,45,70,45, PINK); // '-'

	// To draw  tree pattern

	int x1tree, x2tree,y1tree,y2tree;
	x1tree = x2tree= (x2-x1)/2 +x1;
	y1tree= y1+cubeSize;
	y2tree= cubeSize;

	// To draw the bark of the tree
	for(int i=-3;i<=3;i++){
		drawLine(x1tree+i,y1tree ,x2tree+i,y2tree*2,RED);
	}
	// To draw the tree
	drawTree3d(x1tree,y1tree,x2tree,y2tree*2,GREEN,7);

	// To draw square pattern

	int squareSize = cubeSize-15;
	int x1square= xcenter+5; // 19
	int y1square= ycenter+15; // 45
	int x2square = x1square; // 19
	int y2square = y1square + squareSize; // 80

	drawSquare3d(x1square,y1square,x2square,y2square,CYAN,4);

	return 0;
}

/**************End*****************/
