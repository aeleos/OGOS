#include <stdint.h>
#include <string.h>
#include <kernel/int32.h>
#include <kernel/video.h>

uint32_t font_data_lookup_table[16] = {
    0x00000000,
    0x000000FF,
    0x0000FF00,
    0x0000FFFF,
    0x00FF0000,
    0x00FF00FF,
    0x00FFFF00,
    0x00FFFFFF,
    0xFF000000,
    0xFF0000FF,
    0xFF00FF00,
    0xFF00FFFF,
    0xFFFF0000,
    0xFFFF00FF,
    0xFFFFFF00,
    0xFFFFFFFF
};

static uint16_t* vid_buffer;

void vid_init(){
  vid_buffer = (uint16_t*) VESA_MEMORY;
  regs16_t regs;

  // switch to 320x200x256 graphics mode
  regs.ax = 0x0013;
  int32(0x10, &regs);

  // full screen with blue color (1)
  //memset((char *)0xA0000, 1, (320*200));
  for (int x = 0; x < 320; x++){
    for (int y = 0; y < 200; y++){
      put_pixel(x,y,0);
    }
  }

  // draw horizontal line from 100,80 to 100,240 in multiple colors
  //for(int y = 0; y < 200; y++)
    //  memset((char *)0xA0000 + (y*320+80), y, 160);

  drawrect(0,100,319,120,1);
  fillrect(1,99,318,119,2);
  // wait for key
  regs.ax = 0x0000;
  int32(0x16, &regs);

  // switch to 80x25x16 text mode
  regs.ax = 0x0003;
  int32(0x10, &regs);

}


void put_pixel(int x,int y, int color){
   if (x<0 || x>VESA_WIDTH || y<0 || y>VESA_HEIGHT) return;
   //vid_buffer[y*VESA_WIDTH+x]=color;
   memset((char*)0xA0000 + (y*VESA_WIDTH + x), color, sizeof(color));
}



 void drawline(int x1, int y1, int x2, int y2, char col)
 {
    int d, x, y, ax, ay, sx, sy, dx, dy;

    dx = x2-x1;
    ax = ABS(dx) << 1;
    sx = SGN(dx);
    dy = y2-y1;
    ay = ABS(dy) << 1;
    sy = SGN(dy);

    x = x1;
    y = y1;
    if( ax > ay )
    {
       d = ay - (ax >> 1);
       while( x != x2 )
       {
 	      put_pixel( x, y, col );
   	      if( d >= 0 )
 	      {
 	         y += sy;
 	         d -= ax;
 	      }
 	      x += sx;
 	      d += ay;
       }
    }
    else
    {
       d = ax - (ay >> 1);
       while( y != y2 )
       {
 	      put_pixel( x, y, col );
 	      if( d >= 0 )
 	      {
 	         x += sx;
 	         d -= ay;
 	      }
 	      y += sy;
 	      d += ax;
       }
    }
    return;
 }

 void drawrect(int x1, int y1, int x2, int y2, char color)
{
   drawline(x1,y1,x2,y1,color);
   drawline(x1,y2,x2,y2,color);
   drawline(x1,y1,x1,y2,color);
   drawline(x2,y1,x2,y2+1,color);
}

void hline(int x1, int x2, int y, char color)
{
   drawline(x1,y,x2,y,color);
}

void vline(int y1, int y2, int x, char color)
{
   drawline(x,y1,x,y2,color);
}

void fillrect(int x1, int y1, int x2, int y2, char color)
{
   for (int i = y1; i < y2; i++)
      hline(x1,x2,i,color);
}
