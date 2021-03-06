#include "ScreenWriter.h"

using namespace MonAPI;
using namespace MonAPI::terminal;

#define FONT_WIDTH   8
#define FONT_HEIGHT 16
#define FOREGROUND 0x000000
#define BACKGROUND 0xffffff

extern Screen screen;

static void cursor(bool erase)
{
    int x, y;
    syscall_get_cursor(&x, &y);
    screen.fillRect16(x * FONT_WIDTH, y * FONT_HEIGHT + FONT_HEIGHT - 2, 8, 2,
                      erase ? BACKGROUND : FOREGROUND);
}


ScreenWriter::ScreenWriter() : x_(0), y_(0)
{

}

ScreenWriter::~ScreenWriter()
{

}

int ScreenWriter::clearScreen()
{
    return 0; // ToDo
}

int ScreenWriter::moveCursor(uint32_t x, uint32_t y)
{
    cursor(true);
    syscall_set_cursor(x, y);
    x_ = x;
    y_ = y;
    cursor(false);
    return 0;
}

int ScreenWriter::moveCursorUp(uint32_t n)
{
    y_ -= n;
    if (y_ < 0) y_ = 0;
    return 0;
}

int ScreenWriter::moveCursorDown(uint32_t n)
{
    y_ += n;
    return 0;
}

int ScreenWriter::moveCursorLeft(uint32_t n)
{
    int x, y;
    cursor(true);
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x - n, y);
    syscall_get_cursor(&x, &y);
    cursor(false);
    return 0;
}

int ScreenWriter::moveCursorRight(uint32_t n)
{
    int x, y;
    cursor(true);
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x + n, y);
    cursor(false);
    return 0;
}

int ScreenWriter::write(uint8_t* buf, uint32_t length)
{
    syscall_print((char*)buf);
    cursor(false);
    return 0;
}

int ScreenWriter::lineFeed()
{
   cursor(true);
   syscall_print("\n");
   cursor(false);
   return 0;
}

int ScreenWriter::backSpace()
{
   cursor(true);
   moveCursorLeft(1);
   cursor(false);
   return 0;
}
