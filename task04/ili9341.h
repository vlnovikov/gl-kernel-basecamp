#ifndef __ILI9341_H__
#define __ILI9341_H__

#define LCD_PIN_CS 7
#define LCD_PIN_RESET 27
#define LCD_PIN_DC 22

#define GPIO_X1 18
#define GPIO_X2 23
#define GPIO_X3 24

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

// default orientation
#define LCD_WIDTH  320
#define LCD_HEIGHT 240
#define LCD_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)

#define LCD_CASET 0x2A
#define LCD_RASET 0x2B
#define LCD_RAMWR 0x2C

/****************************/

// Color definitions
#define COLOR_BLACK   0x0000
#define COLOR_BLUE    0x001F
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_YELLOW  0xFFE0
#define COLOR_WHITE   0xFFFF
#define COLOR_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))


#endif // __ILI9341_H__
