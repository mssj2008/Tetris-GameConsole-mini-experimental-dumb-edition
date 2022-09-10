/*
 * main.c
 *
 * Created: 7/27/2022 4:11:38 PM
 *  Author: ysj78
 */ 

# define F_CPU 8000000UL

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define CS 0
#define RST 2
#define DC 1
#define Left_Pressed ((PINC & 0x08) == 0)
#define Left_Index 3
#define Right_Pressed ((PINC & 0x02) == 0)
#define Right_Index 1
#define Up_Pressed ((PINC & 0x01) == 0)
#define Up_Index 0
#define Down_Pressed ((PINC & 0x04) == 0)
#define Down_Index 2
#define Shift_Pressed ((PINC & 0x10) == 0)
#define Normal_Tile 0x01
#define Tetromino_Tile 0x02


unsigned char display[84][6];
char gdisplay[21][3];
const char font_7x8[7*98] PROGMEM = {
0x00,0x00,0x00,0xBE,0x00,0x00,0x00,
0x00,0x00,0x06,0x00,0x06,0x00,0x00,
0x00,0x28,0x7C,0x28,0x7C,0x28,0x00,
0x00,0x0C,0x92,0xFF,0x92,0x64,0x00,
0x0E,0x4A,0x2E,0x10,0xE8,0xA4,0xE0,
0x00,0x76,0x99,0x99,0xA9,0x4E,0xA0,
0x00,0x00,0x00,0x06,0x00,0x00,0x00,
0x3C,0x42,0x81,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x81,0x42,0x3C,
0x00,0x00,0x0A,0x04,0x0A,0x00,0x00,
0x00,0x00,0x10,0x38,0x10,0x00,0x00,
0x00,0x00,0x40,0x30,0x00,0x00,0x00,
0x00,0x00,0x10,0x10,0x10,0x00,0x00,
0x00,0x00,0x00,0x40,0x00,0x00,0x00,
0x00,0x40,0x20,0x10,0x08,0x04,0x00,
0x00,0x7C,0x82,0x82,0x82,0x7C,0x00,
0x00,0x00,0x82,0xFF,0x80,0x00,0x00,
0x00,0xC2,0xA1,0xA1,0x91,0x8E,0x00,
0x00,0x44,0x92,0x92,0x92,0x6C,0x00,
0x00,0x1C,0x10,0x10,0xFC,0x10,0x00,
0x00,0x9E,0x92,0x92,0x92,0x72,0x00,
0x00,0x78,0x94,0x92,0x91,0x60,0x00,
0x00,0x82,0x42,0x22,0x12,0x0E,0x00,
0x00,0x6C,0x92,0x92,0x92,0x6C,0x00,
0x00,0x0C,0x92,0x52,0x32,0x1C,0x00,
0x00,0x00,0x00,0x48,0x00,0x00,0x00,
0x00,0x00,0x40,0x34,0x00,0x00,0x00,
0x00,0x00,0x10,0x28,0x44,0x00,0x00,
0x00,0x00,0x28,0x28,0x28,0x00,0x00,
0x00,0x00,0x44,0x28,0x10,0x00,0x00,
0x00,0x04,0x02,0xA2,0x12,0x0C,0x00,
0x7C,0x82,0xBA,0xAA,0xBA,0xA2,0x9E,
0x00,0xFC,0x12,0x12,0x12,0xFC,0x00,
0x00,0xFE,0x92,0x92,0x92,0x6C,0x00,
0x00,0x7C,0x82,0x82,0x82,0x82,0x00,
0x00,0xFE,0x82,0x82,0x82,0x7C,0x00,
0x00,0xFE,0x92,0x92,0x92,0x92,0x00,
0x00,0xFE,0x12,0x12,0x12,0x02,0x00,
0x00,0xFE,0x82,0x92,0x92,0xF2,0x00,
0x00,0xFE,0x10,0x10,0x10,0xFE,0x00,
0x00,0x00,0x82,0xFE,0x82,0x00,0x00,
0x00,0x40,0x80,0x82,0x7E,0x02,0x00,
0x00,0xFE,0x10,0x28,0x44,0x82,0x00,
0x00,0xFE,0x80,0x80,0x80,0x80,0x00,
0x00,0xFE,0x0C,0x18,0x0C,0xFE,0x00,
0x00,0xFE,0x1C,0x38,0x70,0xFE,0x00,
0x7C,0x82,0x82,0x82,0x82,0x82,0x7C,
0x00,0xFE,0x12,0x12,0x12,0x0C,0x00,
0x00,0x3E,0x42,0x52,0x62,0x7C,0x80,
0x00,0xFE,0x32,0x52,0x52,0x8C,0x00,
0x00,0x4C,0x92,0x92,0x92,0x64,0x00,
0x00,0x02,0x02,0xFE,0x02,0x02,0x00,
0x00,0x7E,0x80,0x80,0x80,0x7E,0x00,
0x00,0x0E,0x70,0x80,0x70,0x0E,0x00,
0x00,0x7E,0x80,0x40,0x80,0x7E,0x00,
0x00,0x82,0x6C,0x10,0x6C,0x82,0x00,
0x00,0x02,0x04,0xF8,0x04,0x02,0x00,
0x00,0xC2,0xA2,0x92,0x8A,0x86,0x00,
0x00,0xFE,0x82,0x00,0x00,0x00,0x00,
0x00,0x04,0x08,0x10,0x20,0x40,0x00,
0x00,0x00,0x00,0x00,0x82,0xFE,0x00,
0x00,0x08,0x04,0x02,0x04,0x08,0x00,
0x00,0x40,0x40,0x40,0x40,0x40,0x00,
0x00,0x00,0x02,0x04,0x00,0x00,0x00,
0x00,0x00,0xE0,0xA0,0xE0,0x80,0x00,
0x00,0x00,0xFE,0x90,0x90,0x60,0x00,
0x00,0x00,0x60,0x90,0x90,0x00,0x00,
0x00,0x60,0x90,0x90,0xFE,0x00,0x00,
0x00,0x00,0x78,0xA8,0x38,0x00,0x00,
0x00,0x08,0xFC,0x0A,0x0A,0x00,0x00,
0x00,0x00,0xDC,0x94,0xFC,0x00,0x00,
0x00,0xFE,0x10,0x10,0xF0,0x80,0x00,
0x00,0x00,0x00,0xF4,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x7A,0x00,0x00,
0x00,0x00,0xFE,0x30,0xC8,0x80,0x00,
0x00,0x00,0x00,0xFE,0x80,0x00,0x00,
0x00,0xF8,0x10,0x30,0x10,0xF0,0x00,
0x00,0xF8,0x10,0x10,0x10,0xF0,0x00,
0x00,0x70,0x88,0x88,0x88,0x70,0x00,
0x00,0x00,0xFC,0x24,0x24,0x18,0x00,
0x00,0x18,0x24,0x24,0xFC,0x00,0x00,
0x00,0x00,0xF8,0x10,0x08,0x00,0x00,
0x00,0x10,0xA8,0xA8,0xA8,0x40,0x00,
0x00,0x00,0x10,0xF8,0x90,0x00,0x00,
0x00,0x70,0x80,0x80,0xF0,0x80,0x00,
0x00,0x00,0x70,0x80,0x70,0x00,0x00,
0x00,0x70,0x80,0x40,0x80,0x70,0x00,
0x00,0x88,0x50,0x20,0x50,0x88,0x00,
0x00,0x08,0x10,0xE0,0x10,0x08,0x00,
0x00,0x00,0xC8,0xA8,0x98,0x00,0x00,
0x10,0xFE,0x82,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x82,0xFE,0x10,
0x00,0x00,0x10,0x08,0x10,0x08,0x00,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,
0x00,0x26,0x46,0x40,0x46,0x26,0x00,
};
const char Game_Over_img[21*2] PROGMEM = {
0xFC,0x90,0x01,0xF9,0x03,0x01,0x01,0x03,0xF9,0x09,0x0B,0x09,0xF9,0x03,0xF9,0x01,0x03,0x01,0x01,0x90,0xFC,
0x03,0x00,0x08,0x09,0x0D,0x09,0x09,0x0C,0x09,0x09,0x0D,0x09,0x09,0x0C,0x09,0x09,0x0D,0x09,0x08,0x00,0x03
} ;
const char MM_img[21*2] PROGMEM = {
	0xFE,0x03,0x09,0x0B,0xF9,0x0B,0x09,0x03,0xF9,0x4B,0x49,0x4B,0x49,0x03,0x09,0x0B,0xF9,0x0B,0x09,0x03,0xFE,
	0x07,0x0C,0x08,0x08,0x0B,0x08,0x08,0x08,0x0B,0x0A,0x0A,0x0A,0x0A,0x08,0x08,0x08,0x0B,0x08,0x08,0x0C,0x07,
};
#define TILE_SIZE 4
int Score = 0;
#define Row_Score 50
int ii=0;
int iithresh = 2500; // timing variable for gravity intervals! :)
const int min_iithresh = 1500; // minimum
int Tetrominoes_Placed = 0;
#define iithresh_Decrement_Amount 100
#define iiThresh_Decrement_Interval 5 // every 10 blocks placed
unsigned char j = 0;
char g = 0;
char inputFlag = 0;
char Game = 1;


struct point
{
	char x,y;
};
struct Tet_tile
{
	struct point gpos;
	struct point t2lpos,t3lpos,t4lpos;	
};

struct Tet_tile tets[5];
struct Tet_tile curTTile;

struct point Point(char x, char y)
{
	struct point result;
	result.x = x;
	result.y = y;
	return result;
}
struct point Add_Point(struct point p1,struct point p2)
{
	return Point(p1.x+p2.x,p1.y+p2.y);
}
struct point Rotate_Point(struct point p) // *90 degree rotation   dir ---- 1 -> clockwise 2-> anti-clockwise
{
	return Point(p.y,-p.x);
}

char digit_counter(int num)
{
	char len = 0;
	if(num == 0)
		return 1;
	while (num > 0)
	{
		num /= 10;
		PORTA = num;
		len++;
	}
	return len;
}

void itoa(int num,char *out)
{
	char tmpp[digit_counter(num)];
	char resultt[digit_counter(num)];
	char tmp;
	char curindex = 0;
	if(num == 0)
	{
		out[0] = '0';
		return;
	}
	while(num > 0)
	{
		tmp = (num%10)+48;
		tmpp[curindex] = tmp;
		num /= 10;
		curindex++;
	}
	for(int i = 0; i < curindex; i++)
	{
		out[curindex-i-1] = tmpp[i];
	}
}

// PB stuff

void PB_ClearBit(int bi)
{
	PORTB &= (0xFF - (1<<bi));
}
void PB_SetBit(int bi)
{
	PORTB |= (1<<bi);
}

// SPI stuff

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDRB |= (1<<PB5)|(1<<PB7)|(1<<PB4);
	//PORTB |= (1<<PB4);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}
void SPI_MasterTransmit(unsigned char cData)
{
	unsigned char c;
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	c = SPDR;
}

// PCD8544 stuff

void GLCD_SEND(unsigned char stuff,int dc)
{
	// cofigure the dc pin
	if(dc) // data
		PB_SetBit(DC);
	else // command
		PB_ClearBit(DC);
	// send the stuff :)
	SPI_MasterTransmit(stuff);
	//_delay_us(1);
}

void GLCD_Init()
{
		PB_SetBit(CS);
		PB_ClearBit(RST);
		SPI_MasterInit();
		_delay_ms(1);
		PB_SetBit(RST);
		PB_ClearBit(CS);
		GLCD_SEND(0x21,0);
		GLCD_SEND(0x90,0);
		GLCD_SEND(0x20,0);
		GLCD_SEND(0x0C,0);
		PB_SetBit(CS);
}
void GLCD_Update()
{
	PB_ClearBit(CS);
	GLCD_SEND(0x80,0);
	GLCD_SEND(0x40,0);
	for(char i = 0; i < 6; i++)
		for(char j = 0; j < 84; j++)
		{
			GLCD_SEND(display[j][i],1);
		}
	PB_SetBit(CS);
}

void GLCD_SetPixel(struct point p)
{
	if(p.x > 83 || p.x < 0 || p.y > 47 || p.y < 0)
		return;
	display[p.x][p.y/8] |= (1<<(p.y%8));
	
	PB_ClearBit(CS);
	GLCD_SEND(0x80 + p.x,0);
	GLCD_SEND(0x40 + (p.y/8),0);
	GLCD_SEND(display[p.x][p.y/8],1);
	PB_SetBit(CS);
	
	//GLCD_Update();
}
void GLCD_ClearPixel(struct point p)
{
	if(p.x > 83 || p.x < 0 || p.y > 47 || p.y < 0)
	return;
	display[p.x][p.y/8] &= ~(1<<(p.y%8));
	PB_ClearBit(CS);
	GLCD_SEND(0x80 + p.x,0);
	GLCD_SEND(0x40 + (p.y/8),0);
	GLCD_SEND(display[p.x][p.y/8],1);
	PB_SetBit(CS);
}
void GLCD_SetPixel2(struct point p)
{
	if(p.x > 83 || p.x < 0 || p.y > 47 || p.y < 0)
	return;
	display[p.x][p.y/8] |= (1<<(p.y%8));
}
void GLCD_ClearPixel2(struct point p)
{
	if(p.x > 83 || p.x < 0 || p.y > 47 || p.y < 0)
	return;
	display[p.x][p.y/8] &= ~(1<<(p.y%8));
}

void GLCD_Clear()
{
	for(char i = 0; i < 6; i++)
		for(char j = 0; j < 84; j++)
		{
			display[j][i] = 0x00;
		}
	GLCD_Update();
}

void GLCD_putchar(struct point p,char c)
{
	if(p.x > 77 || p.x < 0 || p.y > 39 || p.y < 0)
		return;
	for(int i = 0; i < 7; i++)
	{
		for(int j = 0; j < 8; j++)
		if(c != ' ')
		{
			if(font_7x8[((c-33)*7)+i] & (1<<j))
				GLCD_SetPixel(Add_Point(p,Point(i,j)));
			else
				GLCD_ClearPixel(Add_Point(p,Point(i,j)));
		}
		else
		{
			GLCD_ClearPixel(Add_Point(p,Point(i,j)));
		}
	}
}
void GLCD_Puts(struct point p,const char *c)
{
	struct point curp = p;
	int i = 0;
	if(curp.y > 39)
		return;
	if(curp.y < 0)
		curp.y = 0;
	if(curp.x < 0)
		curp.x = 0;
	if(curp.x > 77)
	{
		curp.x = 0;
		curp.y += 8;
		if(curp.y > 39)
			return;
	}
	while(c[i] != NULL)
	{
		if(curp.y > 39)
		return;
		if(curp.y < 0)
		curp.y = 0;
		if(curp.x < 0)
		curp.x = 0;
		if(curp.x > 77)
		{
			curp.x = 0;
			curp.y += 8;
			if(curp.y > 39)
			return;
		}
		GLCD_putchar(curp,c[i]);
		curp.x += 7;
		i++;
	}
}

// GameStuff

void Gdisplay_ClearTile(struct point p)
{
	gdisplay[p.x][p.y/4] &= ~(0x03<<((p.y%4)*2));
}
void Gdisplay_SetTile(struct point p,char TM)
{
	Gdisplay_ClearTile(p); // I putted this because we are using OR to set tile in the next line, so the previous Tile Data Could be still sitting there after setting the tile 
	gdisplay[p.x][p.y/4] |= (TM<<((p.y%4)*2));
}
char Gdisplay_GetTile(struct point p)
{
	return ((gdisplay[p.x][p.y/4] & (0x03<<((p.y%4)*2)))>>((p.y%4)*2));
}

void TTs_init() // the tetrominoes in the game
{
	tets[0].gpos = Point(11,2);
	tets[0].t2lpos = Point(-1,0);
	tets[0].t3lpos = Point(1,0);
	tets[0].t4lpos = Point(0,-1);
	tets[1].gpos = Point(11,2);
	tets[1].t2lpos = Point(0,-1);
	tets[1].t3lpos = Point(1,0);
	tets[1].t4lpos = Point(2,0);
	tets[2].gpos = Point(11,2);
	tets[2].t2lpos = Point(-1,0);
	tets[2].t3lpos = Point(0,-1);
	tets[2].t4lpos = Point(1,-1);
	tets[3].gpos = Point(11,2);
	tets[3].t2lpos = Point(1,0);
	tets[3].t3lpos = Point(0,-1);
	tets[3].t4lpos = Point(1,-1);
	tets[4].gpos = Point(11,2);
	tets[4].t2lpos = Point(-1,0);
	tets[4].t3lpos = Point(1,0);
	tets[4].t4lpos = Point(2,0);
}

void Clear_Tile(struct point p)
{
	if(p.x > 20 || p.x < 0 || p.y < 0 || p.y > 11)
		return;
	Gdisplay_ClearTile(p);
	for(char i = 0; i < 4; i++)
		for(char j = 0; j < 4; j++)
			GLCD_ClearPixel2(Point(p.x*4+i,p.y*4+j));
}
void Draw_Tile(struct point p,char TM) // 4x4 tiles  0 <= x < 21 --> 84/4 = 21       0 <= y < 12 -->  48/4 = 12   TM(TileMode/Type) is either 1 or 2(Normal Tile or Tetromino tile)
{
	if(p.x > 20 || p.x < 0 || p.y < 0 || p.y > 11)
	return;
	Clear_Tile(p);
	Gdisplay_SetTile(p,TM);
	for(char i = 0; i < 4; i++)
	{
		GLCD_SetPixel2(Point(p.x*4,p.y*4+i));
		GLCD_SetPixel2(Point(p.x*4+3,p.y*4+i));
	}
	GLCD_SetPixel2(Point(p.x*4+1,p.y*4));
	GLCD_SetPixel2(Point(p.x*4+2,p.y*4));
	GLCD_SetPixel2(Point(p.x*4+1,p.y*4+3));
	GLCD_SetPixel2(Point(p.x*4+2,p.y*4+3));
}
void Fill_Tile(struct point p)
{
	if(p.x > 20 || p.x < 0 || p.y < 0 || p.y > 11)
		return;
	for(char i = 0; i < 4; i++)
		for(char j = 0; j < 4; j++)
			GLCD_SetPixel2(Point(p.x*4+i,p.y*4+j));
}

void Show_Game_Image(const char *img) // 'F' -> Fill 'T' -> Tile
{
	GLCD_Clear();
	for(char i = 0; i < 21; i++)
	{
		for(char j = 0; j < 12; j++)
		{
			if((img[i+((j/8)*21)] & (1<<(j%8))) != 0)
			{
				Fill_Tile(Point(i,j));
			}
		}
		GLCD_Update();
		_delay_ms(20);
	}
}

void Update_Tiles()
{
	for(char i = 0; i < 21; i++)
		for(char j = 0; j < 12; j++)
		{
			if(!Gdisplay_GetTile(Point(i,j)))
				Clear_Tile(Point(i,j));
			else
				Draw_Tile(Point(i,j),Gdisplay_GetTile(Point(i,j)));
		}
}

void Clear_Row(char row,int fast)
{
	for(char i = 0; i < 21; i++)
	{
		Clear_Tile(Point(i,row));
		GLCD_Update();
		if(fast)
		_delay_ms(2);
		else
		_delay_ms(50);
	}
}

void Clear_TTile()
{
	Clear_Tile(curTTile.gpos);
	Clear_Tile(Add_Point(curTTile.gpos,curTTile.t2lpos));
	Clear_Tile(Add_Point(curTTile.gpos,curTTile.t3lpos));
	Clear_Tile(Add_Point(curTTile.gpos,curTTile.t4lpos));
}
void Draw_TTile()
{
	Draw_Tile(curTTile.gpos,Tetromino_Tile);
	Draw_Tile(Add_Point(curTTile.gpos,curTTile.t2lpos),Tetromino_Tile);
	Draw_Tile(Add_Point(curTTile.gpos,curTTile.t3lpos),Tetromino_Tile);
	Draw_Tile(Add_Point(curTTile.gpos,curTTile.t4lpos),Tetromino_Tile);
}
void Rotate_TTile()
{
	
	struct point np1 = Rotate_Point(curTTile.t2lpos);
	struct point np2 = Rotate_Point(curTTile.t3lpos);
	struct point np3 = Rotate_Point(curTTile.t4lpos);
	struct point np11 = Add_Point(np1,curTTile.gpos);
	struct point np22 = Add_Point(np2,curTTile.gpos);
	struct point np33 = Add_Point(np3,curTTile.gpos);
	if(np11.x > 20 || np11.x < 0 || np11.y > 11 || Gdisplay_GetTile(Add_Point(np1,curTTile.gpos)) == Normal_Tile)
		return;
	if(np22.x > 20 || np22.x < 0 || np22.y > 11 || Gdisplay_GetTile(Add_Point(np2,curTTile.gpos)) == Normal_Tile)
		return;
	if(np33.x > 20 || np33.x < 0 || np33.y > 11 || Gdisplay_GetTile(Add_Point(np3,curTTile.gpos)) == Normal_Tile)
		return;
	Clear_TTile();
	curTTile.t2lpos = np1;
	curTTile.t3lpos = np2;
	curTTile.t4lpos = np3;
}
void Move_TTile(struct point mv)
{
	Clear_TTile();
	curTTile.gpos = Add_Point(curTTile.gpos,mv);
}

char Is_LegitMove(struct point mv)
{
	struct point np1,np2,np3,np4;
	np1 = Add_Point(curTTile.gpos,mv);
	np2 = Add_Point(Add_Point(curTTile.t2lpos,curTTile.gpos),mv);
	np3 = Add_Point(Add_Point(curTTile.t3lpos,curTTile.gpos),mv);
	np4 = Add_Point(Add_Point(curTTile.t4lpos,curTTile.gpos),mv);
	if(np1.x > 20 || np1.x < 0 || np1.y > 11 || Gdisplay_GetTile(Point(np1.x,np1.y)) == Normal_Tile)
	{
		return 0;
	}
	else if(np2.x > 20 || np2.x < 0 || np2.y > 11 || Gdisplay_GetTile(Point(np2.x,np2.y)) == Normal_Tile)
	{
		return 0;
	}
	else if(np3.x > 20 || np3.x < 0 || np3.y > 11 || Gdisplay_GetTile(Point(np3.x,np3.y)) == Normal_Tile)
	{
		return 0;
	}
	else if(np4.x > 20 || np4.x < 0 || np4.y > 11 || Gdisplay_GetTile(Point(np4.x,np4.y)) == Normal_Tile)
	{
		return 0;
	}
	return 1;
}

char row_check(char row)
{
	for(char i = 0; i < 21; i++)	
	{
		if(!Gdisplay_GetTile(Point(i,row)))
			return 0;
	}
	return 1;
}

void Full_Row_Check()
{
	int full_rows = 0;
	rr:
	for(char i = 11; i > 0; i--)
	{
		if(row_check(i))
		{
			full_rows++;
			Score += Row_Score*full_rows;
			Clear_Row(i,0);
			for(char j = i; j > 0; j--)
				for(char k = 0; k < 21; k++)
				{
					Gdisplay_SetTile(Point(k,j),Gdisplay_GetTile(Point(k,j-1)));
				}
			for(char j = 0; j < 21; j++)
				Gdisplay_ClearTile(Point(j,0));
			Update_Tiles();
			GLCD_Update();
			_delay_ms(500);
			goto rr;
		}
	}
}

char Lose_Check()
{
	if(Gdisplay_GetTile(curTTile.gpos) != 0)
		return 1;
	else if(Gdisplay_GetTile(Add_Point(curTTile.gpos,curTTile.t2lpos)) != 0)
		return 1;
	else if(Gdisplay_GetTile(Add_Point(curTTile.gpos,curTTile.t3lpos)) != 0)
		return 1;
	else if(Gdisplay_GetTile(Add_Point(curTTile.gpos,curTTile.t4lpos)) != 0)
		return 1;
	return 0;
}

void PlaceT()
{
	Draw_Tile(curTTile.gpos,Normal_Tile);
	Draw_Tile(Add_Point(curTTile.t2lpos,curTTile.gpos),Normal_Tile);
	Draw_Tile(Add_Point(curTTile.t3lpos,curTTile.gpos),Normal_Tile);
	Draw_Tile(Add_Point(curTTile.t4lpos,curTTile.gpos),Normal_Tile);
	Full_Row_Check();
	srand(j);
	for(int i = 0; i < (rand()%(rand()%1000)); i++)
	{
		rand();
	}
	curTTile = tets[rand()%5];
	if(Lose_Check())
	{
		Game = 0;
	}
	Tetrominoes_Placed++;
	if(Tetrominoes_Placed % iiThresh_Decrement_Interval == 0 && iithresh > min_iithresh)
	{
		if(iithresh - iithresh_Decrement_Amount >= min_iithresh)
			iithresh -= iithresh_Decrement_Amount;
		else
			iithresh = min_iithresh;
	}
}

void GameLoop() // Main game code
{
	if(!Game)
		return;
	if(Up_Pressed && !(inputFlag & (1<<Up_Index)))
	{	
	Rotate_TTile();
	inputFlag |= (1<<Up_Index);
	_delay_ms(20);
	}
	else if(!Up_Pressed)
	{
		inputFlag &= ~(1<<Up_Index);
	}
	if(Left_Pressed && !(inputFlag & (1<<Left_Index)))
	{
		if(Shift_Pressed)
		{
			if(Is_LegitMove(Point(-2,0)))
				Move_TTile(Point(-2,0));
			else
			{
				if(Is_LegitMove(Point(-1,0)))
					Move_TTile(Point(-1,0));
			}
		}
		else
		{
			if(Is_LegitMove(Point(-1,0)))
				Move_TTile(Point(-1,0));
		}
		inputFlag |= (1<<Left_Index);
		_delay_ms(20);
	}
	else if(!Left_Pressed)
	{
		inputFlag &= ~(1<<Left_Index);
	}
	if(Right_Pressed && !(inputFlag & (1<<Right_Index)))
	{
		if(Shift_Pressed)
		{
			if(Is_LegitMove(Point(2,0)))
			Move_TTile(Point(2,0));
			else
			{
				if(Is_LegitMove(Point(1,0)))
					Move_TTile(Point(1,0));
			}
		}
		else
		{
			if(Is_LegitMove(Point(1,0)))
			Move_TTile(Point(1,0));
		}
		inputFlag |= (1<<Right_Index);
		_delay_ms(20);
	}
	else if(!Right_Pressed)
	{
		inputFlag &= ~(1<<Right_Index);
	}
	if(Down_Pressed && !(inputFlag & (1<<Down_Index)))
	{
		while(Is_LegitMove(Point(0,1)))
		{
			Move_TTile(Point(0,1));
			_delay_ms(40);
		}
		PlaceT();
		inputFlag |= (1<<Down_Index);
		_delay_ms(20);
	}
	else if(g) // applied if the Down button isn't pressed
	{
		if(Is_LegitMove(Point(0,1)))
			Move_TTile(Point(0,1));
		else
			PlaceT();
		g = 0;
		if(!Down_Pressed)
			inputFlag &= ~(1<<Down_Index);
	}
	else if(!Down_Pressed)
	{
		inputFlag &= ~(1<<Down_Index);
	}
	if(!Game)
	{
		for(int i = 0; i < 12; i++)
			for(int j = 0; j < 21; j++)
			{
				Fill_Tile(Point(j,i));
				GLCD_Update();
				_delay_us(100);
			}
		for(int i = 0; i < 12; i++)
			for(int j = 0; j < 21; j++)
			{
				Clear_Tile(Point(j,i));
				GLCD_Update();
				_delay_us(100);
			}	
		Show_Game_Image(Game_Over_img);
		_delay_ms(2000);
		for(int i = 0; i < 12; i++)
			for(int j = 0; j < 21; j++)
			{
				Fill_Tile(Point(j,i));
				GLCD_Update();
				_delay_us(100);
			}
		for(int i = 0; i < 12; i++)
			for(int j = 0; j < 21; j++)
			{
				Clear_Tile(Point(j,i));
				GLCD_Update();
				_delay_us(100);
			}
		char c[digit_counter(Score)];
		itoa(Score,c);
		GLCD_Puts(Point(0,10),"SCORE:");
		GLCD_Puts(Point(45,10),c);
		_delay_ms(2000);
		GLCD_Clear();
		_delay_ms(1000);
		return;
	}
	Draw_TTile();
	GLCD_Update();
	}

void G_Init() // Game Init 
{
	for(char i = 0; i < 21; i++)
		Draw_Tile(Point(i,11),Normal_Tile);
	GLCD_Update();
	_delay_ms(1000);
	Clear_Row(11,0);
	for(char i = 0; i < 21; i++)
		for(char j = 0; j < 12; j++)
			Clear_Tile(Point(i,j));
	GLCD_Update();
}

int main(void) // the main function :) the program start from here
{
	DDRA = 0xFF;
	DDRB = 0x07;
	PORTC = 0xFF;
	TTs_init();
	curTTile = tets[4];
	
	_delay_ms(2);
	GLCD_Init();
	GLCD_Clear();
	
	TCCR0 = 0x02;
	TIMSK = 0x01;
	sei();
	rrrrr:
	Game = 1;
	G_Init();
	for(int i = 0; i < 84; i++)
		for(int j = 0; j < 48; j++)
		{
			if((j+i) % 6 == 0)
				GLCD_SetPixel(Point(i,j));
			_delay_us(100);
		}
	for(int i = 0; i < 84; i++)
		for(int j = 0; j < 48; j++)
		{
			if((j+i) % 6 == 0)
				GLCD_ClearPixel(Point(i,j));
			_delay_us(100);
		}
	GLCD_Puts(Point(0,20),"Hello World!");
	_delay_ms(2000);
	Show_Game_Image(MM_img);
	_delay_ms(1000);
	GLCD_Puts(Point(0,15)," PRESS LEFT ");
	GLCD_Puts(Point(13,23)," TO PLAY ");
	while(!Left_Pressed);
	for(int i = 0; i < 84; i++)
		for(int j = 0; j < 48; j++)
		{
			GLCD_ClearPixel(Point(i,j));
			_delay_us(100);
		}
	_delay_ms(250);
	GLCD_Puts(Point(0,20),"GET READY!!!");
	_delay_ms(750);
	GLCD_Puts(Point(0,20),"            ");
	_delay_ms(250);
	GLCD_Puts(Point(20,20),"3");
	_delay_ms(1000);
	GLCD_Puts(Point(20,20),"2");
	_delay_ms(1000);
	GLCD_Puts(Point(20,20),"1");
	_delay_ms(1000);
	GLCD_Puts(Point(20,20),"GO!");
	_delay_ms(1000);
	GLCD_Puts(Point(20,20),"   ");
	_delay_ms(500);
	while(1)
    {
		GameLoop();
    }
}

ISR(TIMER0_OVF_vect)
{
	ii++;
	if(ii == iithresh)
	{
		ii=0;
		g=1;
		srand(j);
		j+= rand()%5+1;
		if(j == 200)
		 j = 0;
	}
}