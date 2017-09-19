
/*
=================================================================================
 Name        : SSP_Test_lcd.c
 Author      : Archana Ramalingam
 Version     :
 Copyright   : $(copyright)
 Description : This program allows us, using the 1.8" Color TFT LCD via ssp ports,
 	 	 	   to be able to display screensaver of recursive shrinking square patterns on the
 	 	 	   screen.
=================================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "longhorn_sunset.h"

// Variable to store CRP value. Placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#include "LPC17xx.h"  /* LPC17xx definitions */
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Port number is 1 for the circuit used here
#define PORT_NUM            1
#define LOCATION_NUM        0

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

//LCD
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

// To write colour
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

// For LCD delay
void delay(int ms)
{
	int count = 24000;
	int i;

	for ( i = count*ms; i--; i > 0);
}

// Initialize LCD
void lcd_init()
{
/*
 * portnum     = 0 ;
 * cs         = p0.16 / p0.6 ?
 * rs        = p0.21
 * rst        = p0.22
 */
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
    LPC_GPIO0->FIODIR |= (0x1<<21);        /* rs/dc P0.22 defined as Outputs */
    LPC_GPIO0->FIODIR |= (0x1<<22);        /* rst P0.21 defined as Outputs */


    /* Reset sequence */
    LPC_GPIO0->FIOSET |= (0x1<<22);

 delay(500);                        /*delay of 500 ms */
    LPC_GPIO0->FIOCLR |= (0x1<<22);
 delay(500);                        /* delay of 500 ms */
    LPC_GPIO0->FIOSET |= (0x1<<22);
 delay(500);                        /* delay of 500 ms */

     for ( i = 0; i < SSP_BUFSIZE; i++ )    /* Init RD and WR buffer */
        {
            src_addr[i] = 0;
            dest_addr[i] = 0;
        }

     /* Sleep out */
     SSP_SSELToggle( portnum, 0 );
     src_addr[0] = 0x11;    /* Sleep out */
     SSPSend( portnum, (uint8_t *)src_addr, 1 );
     SSP_SSELToggle( portnum, 1 );

     delay(200); /* delay 200 ms */

    /* Disp on */
     SSP_SSELToggle( portnum, 0 );
     src_addr[0] = 0x29;    /* Disp On */
     SSPSend( portnum, (uint8_t *)src_addr, 1 );
     SSP_SSELToggle( portnum, 1 );
     delay(200); /* delay 200 ms */
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

// Draw line function
void drawline(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint32_t color) {

	int16_t slope = abs(y1 - y0) > abs(x1 - x0);
	if (slope)
	{
	swap(x0, y0);
	swap(x1, y1);
	}

	if (x0 > x1)
	{
	swap(x0, x1);
	swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1)
	{
	ystep = 1;
	} else
	{
	ystep = -1;
	}

	for (; x0<=x1; x0++)
	{
	if (slope)
	{
	drawPixel(y0, x0, color);
	} else
	{
	drawPixel(x0, y0, color);
	}
	err -= dy;
	if (err < 0)
	{
	y0 += ystep;
	err += dx;
	}
	}
}

// To draw a vertical line
void VLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	drawLine(x, y, x, y+h-1, color);
 }

// To draw a horizontal line
 void HLine(int16_t x, int16_t y,  int16_t w, uint16_t color)
 {
	 drawLine(x, y, x+w-1, y, color);
 }

// To draw a recursively shrinking square pattern from a given vertex, color and lambda
void squaredraw(int16_t x, int16_t y, uint32_t color, float lambda, int16_t size)
{
	int16_t x0,x1,x2,x3;
	int16_t y0,y1,y2,y3;

	uint32_t k;

	int16_t temp_x0,temp_x1,temp_x2,temp_x3;
	int16_t temp_y0,temp_y1,temp_y2,temp_y3;

	// Define the 4 vertices values along with offset (square size) from the first given vertex (x,y)
	x0=x;
	y0=y;
	x1=x+size;
	y1=y;
	x2=x+size;
	y2=y+size;
	x3=x;
	y3=y+size;

	// Draw the first square (4 drawline() for 4 sides)
	drawline(x0,y0,x1,y1,color);
	delay(15);
	drawline(x1,y1,x2,y2,color);
	delay(15);
	drawline(x2,y2,x3,y3,color);
	delay(15);
	drawline(x3,y3,x0,y0,color);

	// Draw 10 levels of squares in each pattern
	for(k=0;k<10;k++)
	{
		// Use the equation given in class to calculate the 4 vertices' coordinates of the recursive squares
		temp_x0=(lambda*(x1-x0))+x0;
		temp_y0=(lambda*(y1-y0))+y0;

	  	temp_x1=(lambda*(x2-x1))+x1;
	  	temp_y1=(lambda*(y2-y1))+y1;

	  	temp_x2=(lambda*(x3-x2))+x2;
	  	temp_y2=(lambda*(y3-y2))+y2;

	  	temp_x3=(lambda*(x0-x3))+x3;
	  	temp_y3=(lambda*(y0-y3))+y3;

	  	// Draw the next next level square with the new calculated vertices
	  	 drawline(temp_x0,temp_y0,temp_x1,temp_y1,color);
	  	 delay(15);
	  	 drawline(temp_x1,temp_y1,temp_x2,temp_y2,color);
	  	 delay(15);
	  	 drawline(temp_x2,temp_y2,temp_x3,temp_y3,color);
	  	 delay(15);
	  	 drawline(temp_x3,temp_y3,temp_x0,temp_y0,color);

	  	 // Initiate the original vertices' values with the new calculated vertices' values
	  	 x0=temp_x0;
	  	 x1=temp_x1;
	  	 x2=temp_x2;
	  	 x3=temp_x3;
	  	 y0=temp_y0;
	  	 y1=temp_y1;
	  	 y2=temp_y2;
	  	 y3=temp_y3;
	}
}

// To generate random starting vertices for the first square in the patterns
int random_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  uint32_t i, portnum = PORT_NUM;
  portnum = 1 ; /* For LCD use 1 */

  /* SystemClockUpdate() updates the SystemFrequency variable */
  // SystemClockUpdate();
   if ( portnum == 0 )
    SSP0Init();            // Initialize the SSP port
  else if ( portnum == 1 )
    SSP1Init();
  for ( i = 0; i < SSP_BUFSIZE; i++ )
  {
    src_addr[i] = (uint8_t)i;
    dest_addr[i] = 0;
  }

  // Code for Square pattern screen saver

    // First square's vertex coordinates generated randomly
    int16_t x;
    int16_t y;

    float lambda; // To point the location of vertices of next level of square, on previous square's sides
    int16_t size = 30; // square size

    // To define range to avoid patterns display out of screen
    int xrange = ST7735_TFTWIDTH - size;
    int yrange = ST7735_TFTHEIGHT - size;

    // To randomize the colors of the patterns
    uint32_t colorList[] = {GREEN,RED,BLUE,WHITE,PINK,PURPLE,YELLOW,LIME,MAGENTA,CYAN,SILVER,GREY,ORANGE,BROWN,MAROON};
    int colorIndex;

    // Initialize the LCD
  	lcd_init();

	fillrect(0, 0, 220, 220, BLACK); // To provide background color for the patterns to be visible

	// User defined Lambda value
	printf("Enter the desired Lambda value: ");
	scanf("%f",&lambda);
	//printf("%f\n",lambda);

// Draw infinite number of square patterns
while(1)
{
	colorIndex = rand() % 16;
	//printf("%d\n",colorIndex);

	// Randomize the first vertex of the square and draw a square pattern
	x=random_range(0,xrange);
	y=random_range(0,yrange);
	squaredraw(x,y,colorList[colorIndex],lambda,size);
}
  return 0;
}

// End
