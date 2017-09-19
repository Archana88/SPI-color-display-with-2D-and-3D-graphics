/*
=================================================================================
 Name        : SSP_Test_lcd.c
 Author      : Archana Ramalingam
 Version     :
 Copyright   : $(copyright)
 Description : This program allows us, using the 1.8" Color TFT LCD via ssp ports,
 	 	 	   to be able to display 3D world coordinates and three 3D cubes on the screen
=================================================================================
*/

#include <NXP/crp.h>
// Variable to store CRP value. Placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#include "LPC17xx.h"
#include "ssp.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "font.h"

// Port number is 1 for the circuit used here

#define PORT_NUM            1
#define LOCATION_NUM        0

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];
int colstart = 0;
int rowstart = 0;

struct coordinates{
		int x;
		int y;
	};
struct coordinate{
		int x;
		int y;
		int z;
	};

int xcamera = 150,ycamera = 150,zcamera = 150;

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
int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;
int cursor_x = 0, cursor_y = 0;

uint16_t colortext = GREEN, colorbg= YELLOW;
float textsize = 2;
int wrap = 1;

// To draw Vertical line
void drawFastVLine(int16_t x, int16_t y,int16_t h, uint32_t color) {
	drawline(x, y, x, y+h-1, color);
}

// To draw Horizontal line
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	drawline(x, y, x+w-1, y, color);
}

// To fill the background with given color and dimensions
void background(int16_t x, int16_t y, int16_t w, int16_t h,uint32_t color) {
	int16_t i;
    for (i=x; i<x+w; i++) {
    	drawFastVLine(i, y, h, color);
  }
}

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

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1,
                    uint16_t y1) {

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

// To provide delay to LCD
void lcddelay(int ms)
{
            int count = 24000;
            int i;
            for ( i = count*ms; i--; i > 0);
}

// Initialize the LCD
void lcd_init()
{

    uint32_t portnum = 1;
    int i;
    printf("LCD initialized\n");
    /* Notice the hack, for portnum 0 p0.16 is used */
    if ( portnum == 0 )
      {
        LPC_GPIO0->FIODIR |= (0x1<<16);        /* SSP1, P0.16 defined as Outputs */
      }
      else
      {
        LPC_GPIO0->FIODIR |= (0x1<<6);        /* SSP0 P0.6 defined as Outputs */
      }
    /* Set rs(dc) and rst as outputs */
    LPC_GPIO0->FIODIR |= (0x1<<21);        /* rs/dc P0.21 defined as Outputs */
    LPC_GPIO0->FIODIR |= (0x1<<22);        /* rst P0.22 defined as Outputs */


    /* Reset sequence */
    LPC_GPIO0->FIOSET |= (0x1<<22);

    lcddelay(500);                        /*delay 500 ms */
    LPC_GPIO0->FIOCLR |= (0x1<<22);
    lcddelay(500);                        /* delay 500 ms */
    LPC_GPIO0->FIOSET |= (0x1<<22);
    lcddelay(500);                        /* delay 500 ms */

             for ( i = 0; i < SSP_BUFSIZE; i++ )    /* Init RD and WR buffer */
        {
            src_addr[i] = 0;
            dest_addr[i] = 0;
        }

     /* do we need Sw reset (cmd 0x01) ? */

     /* Sleep out */
     SSP_SSELToggle( portnum, 0 );
     src_addr[0] = 0x11;    /* Sleep out */
     SSPSend( portnum, (uint8_t *)src_addr, 1 );
     SSP_SSELToggle( portnum, 1 );

     lcddelay(200);
    /* delay 200 ms */
    /* Disp on */
     SSP_SSELToggle( portnum, 0 );
     src_addr[0] = 0x29;    /* Disp On */
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

// To draw a line
void drawline(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint32_t color) {
            int16_t slope = abs(y1 - y0) > abs(x1 - x0);
            if (slope) {
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

            for (; x0<=x1; x0++) {
            if (slope) {
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

// To write a character
void writeChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
    int8_t i, j;
    if((x >= 128)            || // Clip right
       (y >= 280)           || // Clip bottom
       ((x + 6 * size - 1) < 0) || // Clip left
       ((y + 8 * size - 1) < 0))   // Clip top
        return;

    for (i=0; i<6; i++ ) {
        uint8_t line;
        if (i == 5)
            line = 0x0;
        else
            line = pgm_read_byte(font+(c*5)+i);
        for (j = 0; j<8; j++) {
            if (line & 0x1) {
                if (size == 1) // default size
                    drawPixel(x+i, y+j, color);
                else {  // big size
                    fillrect(x+(i*size), y+(j*size),
                             size + x+(i*size), size + y+(j*size), color);
                }
            } else if (bg != color) {
                if (size == 1) // default size
                    drawPixel(x+i, y+j, bg);
                else {  // big size
                    fillrect(x+i*size, y+j*size, (size + x+i*size), (size + y+j*size), bg);
                }
            }
            line >>= 1;
        }
    }
}

// Helper function to write a character on display
void writeHelp(uint8_t c) {

    if (c == '\n') {
        cursor_y += textsize*8;
        cursor_x  = 0;
    } else if (c == '\r') {

    } else {
        writeChar(cursor_x, cursor_y, c, colortext,colorbg, textsize);
        cursor_x += textsize*6;
        if (wrap && (cursor_x > (_width - textsize*6))) {
            cursor_y += textsize*8;
            cursor_x = 0;
        }
    }
}

// To write text on the display
void writeText(char *str,int x,int y,float size, uint32_t color)
   {
     char c;
     cursor_x=x;
     cursor_y=y;
     colortext = color;
     colorbg= 0;
     textsize=size;
     while(*str != NULL)
     {
		 c = *str++;
		 writeHelp(c);
     }
}

// To generate square screensaver
void squarePattern(int x1, int y1, int x2,int y2,int x3,int y3,int x4,int y4)
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
		drawline(x1_1,y1_1,x2_1,y2_1,ORANGE);
		drawline(x2_1,y2_1,x3_1,y3_1,ORANGE);
		drawline(x4_1,y4_1,x3_1,y3_1,ORANGE);
		drawline(x1_1,y1_1,x4_1,y4_1,ORANGE);

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

// To convert world to viewer coordinates
struct coordinates Transformation_pipeline (int xw, int yw, int zw)
{
	int xdoubleprime, ydoubleprime, D=100, length1=80, length2=50;
	double xPrime, yPrime, zPrime, theta, phi, rho;
	struct coordinates projection;

	theta = acos(xcamera/sqrt(pow(xcamera,2)+pow(ycamera,2)));
	phi = acos(zcamera/sqrt(pow(xcamera,2)+pow(ycamera,2)+pow(zcamera,2)));
	rho= sqrt((pow(xcamera,2))+(pow(ycamera,2))+(pow(zcamera,2)));

	xPrime = (yw*cos(theta))-(xw*sin(theta));
	yPrime = (zw*sin(phi))-(xw*cos(theta)*cos(phi))-(yw*cos(phi)*sin(theta));
	zPrime = rho-(yw*sin(phi)*cos(theta))-(xw*sin(phi)*cos(theta))-(zw*cos(phi));

	xdoubleprime = xPrime*D/zPrime;
	ydoubleprime = yPrime*D/zPrime;
	xdoubleprime = length1+xdoubleprime;
	ydoubleprime = length2-ydoubleprime;

	projection.x = xdoubleprime;
	projection.y = ydoubleprime;
	return projection;
}

// To draw the world 3D coordinate system - x,y,z
void drawCoordinate()
{
	struct coordinates axis;
	int x1,y1,x2,y2,x3,y3,x4,y4;

	 	axis = Transformation_pipeline (0,0,0);
	 	x1=axis.x;
	 	y1=axis.y;

	 	axis = Transformation_pipeline (180,0,0);
	 	x2=axis.x;
	 	y2=axis.y;

	 	axis = Transformation_pipeline (0,180,0);
	 	x3=axis.x;
	 	y3=axis.y;

	 	axis = Transformation_pipeline (0,0,180);
	 	x4=axis.x;
	 	y4=axis.y;

	 	drawline(x1,y1,x2,y2,RED);			   //x axis  Red
	 	drawline(x1,y1,x3,y3,GREEN);		  //y axis  Green
	 	drawline(x1,y1,x4,y4,BLUE);  		   //z axis  Blue
}

// To draw the tree
void treePattern(int x, int y, float angle, int length, int level, int color){

	int x1,y1,len;
	float ang;

	if(level>0){
		// To calculate the x,y vertices for the branch after rotation
		x1 = x+length*cos(angle);
	    y1 = y+length*sin(angle);

	    // To draw the tree branch
	    drawline(x,y,x1,y1,color);

	    // Add 30 degree to angle to rotate it to right
	    ang = angle + 0.52;
	    // To calculate 80% of the line length
	    len = 0.8 * length;

	    // Call drawTree2d function recursively to draw tree pattern
	    treePattern(x1,y1,ang,len,level-1,color);

	    // Subtract 30 degree from the angle to rotate it to right
	    ang = angle - 0.52;
	    len = 0.8 * length;

	    treePattern(x1,y1,ang,len,level-1,color);

	    // Draw the next level
	    ang = angle;
	    len = 0.8 * length;
	    treePattern(x1,y1,ang,len,level-1,color);
	}
}

// To fill the shadow
void fillTriangle(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color) {

	int16_t a, b, y, last;

	// Sort the coordinates by the Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
	swap(y0, y1); swap(x0, x1);
	}
	if (y1 > y2) {
	swap(y2, y1); swap(x2, x1);
	}
	if (y0 > y1) {
	swap(y0, y1); swap(x0, x1);
	}
	if(y0 == y2) {
	a = b = x0;
	if(x1 < a) a = x1;
	else if(x1 > b) b = x1;
	if(x2 < a) a = x2;
	else if(x2 > b) b = x2;
	drawFastHLine(a, y0, b-a+1, color);
	return;
	}

	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
	int32_t sa = 0, sb = 0;

	// For upper part of triangle:
	// find scanline crossings for segments 0-1 and 0-2.
	// If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop, which also avoids a /0 error here if y0=y1 (flat-topped triangle).

	if(y1 == y2) last = y1; // y1 scanline
	else last = y1-1;
	for(y=y0; y<=last; y++) {
	a = x0 + sa / dy01;
	b = x0 + sb / dy02;
	sa += dx01;
	sb += dx02;

	if(a > b) swap(a,b);
	drawFastHLine(a, y, b-a+1, color);
	}

	// For lower part of triangle:
	// find scanline crossings for segments 0-2 and 1-2. This loop is skipped if y1=y2.

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) {
	a = x1 + sa / dy12;
	b = x0 + sb / dy02;
	sa += dx12;
	sb += dx02;

	if(a > b) swap(a,b);
	drawFastHLine(a, y, b-a+1, color);
	}
}

// To draw a 3D cube
void draw3dcube1(int point, int cube_size)
{
	struct coordinates cube;
	int x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,x6,y6,x7,y7,i;
	xcamera = 110;
	ycamera = 110;
	zcamera = 110;
	cube = Transformation_pipeline(point,point,(cube_size+point));
	x1=cube.x;
	y1=cube.y;
	cube = Transformation_pipeline((cube_size+point),point,(cube_size+point));
	x2=cube.x;
	y2=cube.y;
	cube = Transformation_pipeline((cube_size+point),(cube_size+point),(cube_size+point));
	x3=cube.x;
	y3=cube.y;
	cube = Transformation_pipeline(point,(cube_size+point),(cube_size+point));
	x4=cube.x;
	y4=cube.y;
	cube = Transformation_pipeline((cube_size+point),point,point);
	x5=cube.x;
	y5=cube.y;
	cube = Transformation_pipeline ((cube_size+point),(cube_size+point),point);
	x6=cube.x;
	y6=cube.y;
	cube = Transformation_pipeline (point,(cube_size+point),point);
	x7=cube.x;
	y7=cube.y;
	drawline(x1, y1, x2, y2,BLACK);
	lcddelay(500);
	drawline(x2, y2, x3, y3,BLACK);
	lcddelay(500);
	drawline(x3, y3, x4, y4,BLACK);
	lcddelay(500);
	drawline(x4, y4, x1, y1,BLACK);
	lcddelay(500);
	drawline(x2, y2, x5, y5,BLACK);
	lcddelay(500);
	drawline(x5, y5, x6, y6,BLACK);
	lcddelay(500);
	drawline(x6, y6, x3, y3,BLACK);
	lcddelay(500);
	drawline(x6, y6, x7, y7,BLACK);
	lcddelay(500);
	drawline(x7, y7, x4, y4,BLACK);

	// decorate cube
	decorate3dcube1(point, cube_size);
	writeText("A",x1-7,y1+7,1.0, YELLOW); // Writes initials
	squarePattern(x2+2, y2+5, x3-5, y3-5, x6-2, y6-2, x5+2, y5+5); // Draws square pattern on
	// To draw tree pattern
	drawline(x3+15,y3+10,x6+15,y6-15,GREEN); // draws tree trunk
	treePattern(x3+15,y3+10,5.23,4,4,GREEN); // draws with right branch (angle = 5.23 rad/300 deg)
	treePattern(x3+15,y3+10,4.18,4,4,GREEN); // draws with left branch (angle = 4.18 rad/240 deg)
	treePattern(x3+15,y3+10,4.71,4,4,GREEN); // draws with center branch (angle = 4.71 rad/0 deg)
}

// To decorate a 3D cube
void decorate3dcube1(int point,int cube_size)
{	struct coordinates filler;
	int i,j,a[cube_size][cube_size];
	cube_size=cube_size+point;
	for(i=0;i<cube_size;i++)
	{
		for(j=0;j<cube_size;j++)
		{
				filler=Transformation_pipeline(j,i,cube_size);	//S2
				drawPixel(filler.x,filler.y,RED);
				filler=Transformation_pipeline(i,cube_size,j);	//S1
				drawPixel(filler.x,filler.y,BLUE);
				filler=Transformation_pipeline(cube_size,j,i);	//S3
				drawPixel(filler.x,filler.y,YELLOW);
		}
	}
}

// To draw shadow for cube 1
void cube1shadow(double point, double cube_size, double xs, double ys, double zs)
{
	int xp[8]={0}, yp[8]={0}, zp[8]={0},k=0,j=0,i=0;
	struct coordinates s1,s2,s3,s4;
	struct coordinates filler;
	double x[8] = {point,(point+cube_size),(point+cube_size),point,point,(point+cube_size),(point+cube_size),point};
	double y[8] = {point, point, point+cube_size, point+cube_size, point, point, (point+cube_size), (point+cube_size) };
	double z[8] = {point, point, point, point, (point+cube_size), (point+cube_size), (point+cube_size), (point+cube_size)};

	for(i=0; i<8; i++){
	xp[i]=x[i]-((z[i]/(zs-z[i]))*(xs-x[i]));
	yp[i]=y[i]-((z[i]/(zs-z[i]))*(ys-y[i]));
	zp[i]=z[i]-((z[i]/(zs-z[i]))*(zs-z[i]));
	}
	s1 = Transformation_pipeline (xp[4],yp[4],zp[4]- cube_size);
	s2 = Transformation_pipeline (xp[5],yp[5],zp[5] - cube_size);
	s3 = Transformation_pipeline (xp[6],yp[6],zp[6] - cube_size);
	s4 = Transformation_pipeline (xp[7],yp[7],zp[7] - cube_size);

	drawline(s1.x,s1.y,s2.x,s2.y,BLACK);
	drawline(s2.x,s2.y,s3.x,s3.y,BLACK);
	drawline(s3.x,s3.y,s4.x,s4.y,BLACK);
	drawline(s4.x,s4.y,s1.x,s1.y,BLACK);
	fillTriangle( s1.x,s1.y,s2.x,s2.y,s4.x,s4.y, BLACK);
	fillTriangle( s3.x,s3.y,s2.x,s2.y,s4.x,s4.y, BLACK);
}

// To draw cube 2
void draw3dcube2(int point, int cube_size,int zn)
{
	struct coordinates cube;
	int x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,x6,y6,x7,y7,x8,y8,i;
	xcamera = 110;
	ycamera = 110;
	zcamera = 110;
	cube = Transformation_pipeline((point+zn),point,(cube_size+point+20));
	x1=cube.x;
	y1=cube.y;
	cube = Transformation_pipeline((cube_size+point+zn),point,(cube_size+point));
	x2=cube.x;
	y2=cube.y;
	cube = Transformation_pipeline((cube_size+point+zn),(cube_size+point),(cube_size+point));
	x3=cube.x;
	y3=cube.y;
	cube = Transformation_pipeline((point+zn),(cube_size+point),(cube_size+point+20));
	x4=cube.x;
	y4=cube.y;
	cube = Transformation_pipeline((cube_size+point+zn),point,point);
	x5=cube.x;
	y5=cube.y;
	cube = Transformation_pipeline ((cube_size+point+zn),(cube_size+point),point);
	x6=cube.x;
	y6=cube.y;
	cube = Transformation_pipeline ((point+zn),(cube_size+point),point+20);
	x7=cube.x;
	y7=cube.y;
	cube = Transformation_pipeline ((point+zn),point,point+20);
	x8=cube.x;
	y8=cube.y;
	drawline(x1, y1, x2, y2,BLACK);
	lcddelay(500);
	drawline(x2, y2, x3, y3,BLACK);
	lcddelay(500);
	drawline(x3, y3, x4, y4,BLACK);
	lcddelay(500);
	drawline(x4, y4, x1, y1,BLACK);
	lcddelay(500);
	drawline(x2, y2, x5, y5,BLACK);
	lcddelay(500);
	drawline(x5, y5, x6, y6,BLACK);
	lcddelay(500);
	drawline(x6, y6, x3, y3,BLACK);
	lcddelay(500);
	drawline(x6, y6, x7, y7,BLACK);
	lcddelay(500);
	drawline(x7, y7, x4, y4,BLACK);
	lcddelay(500);
	drawline(x8, y8, x1, y1,BLACK);
	lcddelay(500);
	drawline(x8, y8, x7, y7,BLACK);
	lcddelay(500);
	drawline(x8, y8, x5, y5,BLACK);

	decorate3dcube2(point, cube_size, zn);
	writeText("A",x1-9,y1+13,1.0, YELLOW); // Writes initials
	squarePattern(x2+1, y2+2, x3-2, y3-2, x6-2, y6-2, x5+2, y5+2); // draws square pattern
	drawline(x3+15,y3-2,x6+10,y6-15,GREEN); // draws tree trunk
	treePattern(x3+15,y3-2,5.23,4,4,GREEN); // draws the right branch (angle = 5.23 rad/300 deg)
	treePattern(x3+15,y3-2,4.18,4,4,GREEN); // draws the left branch (angle = 4.18 rad/240 deg)
	treePattern(x3+15,y3-2,4.71,4,4,GREEN); // draws the center branch (angle = 4.71 rad/0 deg)
}

// To decorate cube 2
void decorate3dcube2(int point,int cube_size, int zn)
{
	struct coordinates filler;
	int i,j,a[cube_size][cube_size];
	cube_size=cube_size+point;
	for(i=0;i<cube_size;i++)
	{
		for(j=0;j<cube_size;j++)
		{
				filler=Transformation_pipeline(zn+j,i,(cube_size+20)-(j/2+0.4));	//S2
				drawPixel(filler.x,filler.y,CYAN);
				filler=Transformation_pipeline(zn+i,cube_size,(20+j)-(i/2+0.8));	//S1
				drawPixel(filler.x,filler.y,PURPLE);
				filler=Transformation_pipeline(cube_size+zn,j,i);	//S3
				drawPixel(filler.x,filler.y,BROWN);
		}
	}
}

// To draw shadow for cube 2
void cube2shadow(double point, double cube_size,int zn, double xs, double ys, double zs)
{
	int xp[8]={0}, yp[8]={0}, zp[8]={0},k=0,j=0,i=0;
	struct coordinates s1,s2,s3,s4;
	double x[8] = {(point+zn),(point+cube_size+zn),(point+cube_size+zn),(point+zn),(point+zn),(point+cube_size+zn),(point+cube_size+zn),(point+zn)};
	double y[8] = {point, point, point+cube_size, point+cube_size, point, point, (point+cube_size), (point+cube_size) };
	double z[8] = {point, point, point, point, (point+cube_size), (point+cube_size), (point+cube_size), (point+cube_size)};

	for(i=0; i<8; i++){
	xp[i]=x[i]-((z[i]/(zs-z[i]))*(xs-x[i]));
	yp[i]=y[i]-((z[i]/(zs-z[i]))*(ys-y[i]));
	zp[i]=z[i]-((z[i]/(zs-z[i]))*(zs-z[i]));
	}
	s1 = Transformation_pipeline (xp[4],yp[4],zp[4] - cube_size);
	s2 = Transformation_pipeline (xp[5],yp[5],zp[5] - cube_size);
	s3 = Transformation_pipeline (xp[6],yp[6],zp[6] - cube_size);
	s4 = Transformation_pipeline (xp[7],yp[7],zp[7] - cube_size);

	drawline(s1.x,s1.y,s2.x,s2.y,BLACK);
	drawline(s2.x,s2.y,s3.x,s3.y,BLACK);
	drawline(s3.x,s3.y,s4.x,s4.y,BLACK);
	drawline(s4.x,s4.y,s1.x,s1.y,BLACK);
	fillTriangle( s1.x,s1.y,s2.x,s2.y,s4.x,s4.y, BLACK);
	fillTriangle( s3.x,s3.y,s2.x,s2.y,s4.x,s4.y, BLACK);
}

// To draw cube number 3
void draw3dcube3(int point, int cube_size, int zx, int zy)
{
	struct coordinates cube;
	int x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,x6,y6,x7,y7,x8,y8,i;
	xcamera = 110;
	ycamera = 110;
	zcamera = 110;
	cube = Transformation_pipeline((point+zx),(point+zy),(cube_size+point));
	x1=cube.x;
	y1=cube.y;
	cube = Transformation_pipeline((cube_size+point+zx),(point+zy),(cube_size+point));
	x2=cube.x;
	y2=cube.y;
	cube = Transformation_pipeline((cube_size+point+zx),(cube_size+point+zy),(cube_size+point));
	x3=cube.x;
	y3=cube.y;
	cube = Transformation_pipeline((point+zx),(cube_size+point+zy),(cube_size+point));
	x4=cube.x;
	y4=cube.y;
	cube = Transformation_pipeline((cube_size+point+zx),(point+zy),point);
	x5=cube.x;
	y5=cube.y;
	cube = Transformation_pipeline ((cube_size+point+zx),(cube_size+point+zy),point);
	x6=cube.x;
	y6=cube.y;
	cube = Transformation_pipeline ((point+zx),(cube_size+point+zy),point);
	x7=cube.x;
	y7=cube.y;
	cube = Transformation_pipeline ((point+zx),(point+zy),point);
	x8=cube.x;
	y8=cube.y;
	drawline(x1-5, y1-5, x2-5, y2-5,BLACK);
	lcddelay(500);
	drawline(x2-5, y2-5, x3, y3,BLACK);
	lcddelay(500);
	drawline(x3, y3, x4-5, y4-5,BLACK);
	lcddelay(500);
	drawline(x4-5, y4-5, x1-5, y1-5,BLACK);
	lcddelay(500);
	drawline(x2-5, y2-5, x5-5, y5-5,BLACK);
	lcddelay(500);
	drawline(x5-5, y5-5, x6, y6,BLACK);
	lcddelay(500);
	drawline(x6, y6, x3, y3,BLACK);
	lcddelay(500);
	drawline(x6, y6, x7-5, y7-5,BLACK);
	lcddelay(500);
	drawline(x7-5, y7-5, x4-5, y4-5,BLACK);
	lcddelay(500);
	drawline(x8-5, y8-5, x1-5, y1-5,BLACK);
	lcddelay(500);
	drawline(x8-5, y8-5, x7-5, y7-5,BLACK);
	lcddelay(500);
	drawline(x8-5, y8-5, x5-5, y5-5,BLACK);

	decorate3dcube3(point, cube_size, zx-5, zy-5);
	writeText("A",x1-5,y1+10,1.0, YELLOW); // writes initials
	squarePattern(x2+4, y2+2, x3-1, y3-1, x6-1, y6-1, x5+1, y5+1); // draws square pattern
	drawline(x3+10,y3-1,x6+10,y6-20,GREEN); // draws tree trunk
	treePattern(x6+10,y6-20,5.23,1,1,GREEN); // draws the right branch (angle = 5.23 rad/300 deg)
	treePattern(x6+10,y6-20,4.18,1,1,GREEN); // draws the left branch (angle = 4.18 rad/240 deg)
	treePattern(x6+10,y6-20,4.71,1,1,GREEN); // draws the center branch (angle = 4.71 rad/0 deg)
}

// To decorate cube 3
void decorate3dcube3(int point,int cube_size, int zx, int zy)
{
	struct coordinates filler;
	int i,j,a[cube_size][cube_size];
	cube_size=cube_size+point;
	for(i=0;i<cube_size+5;i++)
	{
		for(j=0;j<cube_size+5;j++)
		{
				filler=Transformation_pipeline(zx+j,zy+i,cube_size+5);	//S2
				drawPixel(filler.x,filler.y,BROWN);
				filler=Transformation_pipeline(zx+i,zy+cube_size+5,j);	//S1
				drawPixel(filler.x,filler.y,YELLOW);
				filler=Transformation_pipeline(cube_size+zx+5,zy+j,i);	//S3
				drawPixel(filler.x,filler.y,GREEN);
		}
	}
}

void pointoflight(int xs, int ys, int zs){
			struct coordinates filler;
			filler=Transformation_pipeline(xs,ys,zs);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs,ys,zs+1);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs,ys+1,zs);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs,ys+1,zs+1);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs+1,ys,zs);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs+1,ys,zs+1);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs+1,ys+1,zs);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs+1,ys+1,zs+1);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs,ys,zs+2);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs,ys+2,zs);	//S2
			drawPixel(filler.x,filler.y,BLUE);
			filler=Transformation_pipeline(xs,ys+2,zs+2);	//S2
			drawPixel(filler.x,filler.y,BLUE);
}

// To draw shadow for cube 3
void cube3shadow(double point, double cube_size,int zx, int zy, double xs, double ys, double zs)
{
	int xp[8]={0}, yp[8]={0}, zp[8]={0},k=0,j=0,i=0;
	struct coordinates s1,s2,s3,s4;
	double x[8] = {(point+zx),(point+cube_size+zx),(point+cube_size+zx),(point+zx),(point+zx),(point+cube_size+zx),(point+cube_size+zx),(point+zx)};
	double y[8] = {(point+zy), (point+zy), (point+cube_size+zy), (point+cube_size+zy), point+zy, point+zy, (point+cube_size+zy), (point+cube_size+zy) };
	double z[8] = {point, point, point, point, (point+cube_size), (point+cube_size), (point+cube_size), (point+cube_size)};

	for(i=0; i<8; i++){
	xp[i]=x[i]-((z[i]/(zs-z[i]))*(xs-x[i]));
	yp[i]=y[i]-((z[i]/(zs-z[i]))*(ys-y[i]));
	zp[i]=z[i]-((z[i]/(zs-z[i]))*(zs-z[i]));
	}
	s1 = Transformation_pipeline (xp[4],yp[4],zp[4]- cube_size);
	s2 = Transformation_pipeline (xp[5],yp[5],zp[5] - cube_size);
	s3 = Transformation_pipeline (xp[6],yp[6],zp[6] - cube_size);
	s4 = Transformation_pipeline (xp[7],yp[7],zp[7] - cube_size);

	drawline(s1.x,s1.y,s2.x,s2.y,BLACK);
	drawline(s2.x,s2.y,s3.x,s3.y,BLACK);
	drawline(s3.x,s3.y,s4.x,s4.y,BLACK);
	drawline(s4.x,s4.y,s1.x,s1.y,BLACK);
	fillTriangle( s1.x,s1.y,s2.x,s2.y,s4.x,s4.y, BLACK);
	fillTriangle( s3.x,s3.y,s2.x,s2.y,s4.x,s4.y, BLACK);
}

/*************************************************
                 Main Function  main()
**********************************************/
int main (void)
{
  uint32_t i, portnum = PORT_NUM;
  portnum = 1 ; /* For LCD use 1 */

   if ( portnum == 0 )
    SSP0Init();            /* initialize SSP port */
   else if ( portnum == 1 )
    SSP1Init();

  for ( i = 0; i < SSP_BUFSIZE; i++ )
  {
    src_addr[i] = (uint8_t)i;
    dest_addr[i] = 0;
  }

	// To initialize the LCD
	lcd_init();

  	// To draw the 3D world coordinate system
    background(0, 0,_width,_height,PINK);
    drawCoordinate();
    lcddelay(1000);

    // To draw the three 3D cubes
    int xs = 120, ys = 40, zs = 120, point=0;
    pointoflight(xs, ys, zs);
    lcddelay(100);

    int cube1size = 40;
    cube1shadow(point,cube1size,xs,ys,zs);
    draw3dcube1(point,cube1size);
    lcddelay(500);

    int zn= 80;
    int cube2size= 30;
    cube2shadow(point,cube2size,zn,xs,ys,zs);
    draw3dcube2(point,cube2size,zn);
    lcddelay(500);

    int zy = 70;
    int zx = 60;
    int cube3size= 25;
    cube3shadow(point,cube3size,zx,zy,xs,ys,zs);
    draw3dcube3(point,cube3size,zx,zy);

    return 0;
}
