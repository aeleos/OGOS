#ifndef VIDEO_H
#define VIDEO_H

#define VESA_WIDTH 320
#define VESA_HEIGHT 200
#define VESA_MEMORY 0xA0000
#define VIDEO 0x10
#define CRTC_ADDR 0x3D4
#define SEQU_ADDR 0x3C4
#define ABS(a)   ((a < 0) ? -a : a)
#define SGN(a)   ((a < 0) ? -1 : 1)

void drawline(int x1, int y1, int x2, int y2, char col);
void hline(int x1, int x2, int y, char color);
void vline(int y1, int y2, int x, char color);
void fillrect(int x1, int y1, int x2, int y2, char color);

void vid_init();

#endif
