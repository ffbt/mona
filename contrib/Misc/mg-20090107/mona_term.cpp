#include "mona_term.h"
#include "chrdef.h"
#include <monapi.h>
#include <baygui.h>
#include <string>

using namespace MonAPI;

int ncol;
int nrow;

class MgFrame : public Frame {
private:
    enum {
        MODE_LINE_COLS = 2,
        MAX_NUM_ROWS = 24 + MODE_LINE_COLS,
        MAX_NUM_COLS = 80,
        TIMER_INTERVAL = 500
    };
    uint32_t parentTid_;
    std::string lines[MAX_NUM_ROWS];
    bool colors[MAX_NUM_ROWS];
    uint16_t currentRow_;
    uint16_t currentCol_;
    bool cursorEnabled_;

    int fontWidth()
    {
        return getFontMetrics()->getWidth("W");
    }

    int fontHeight()
    {
        return getFontMetrics()->getHeight("W");
    }

public:
    MgFrame(uint32_t parentTid) : parentTid_(parentTid), currentRow_(0), currentCol_(0), cursorEnabled_(false)
    {
        std::fill(colors, colors + MAX_NUM_ROWS, false);
        setTitle("mg");
        setBounds(450, 40, MAX_NUM_COLS * fontWidth(), MAX_NUM_ROWS * fontHeight() + 4);
        setForeground(baygui::Color::white);
        setForeground(baygui::Color::black);
        setTimer(TIMER_INTERVAL);
    }

    ~MgFrame()
    {
    }

    void processEvent(Event* event)
    {
        if (event->getType() == KeyEvent::KEY_PRESSED) {
            int keycode = ((KeyEvent *)event)->getKeycode();
            int modifiers = ((KeyEvent*)event)->getModifiers();
            if (Message::send(parentTid_, MSG_KEY_VIRTUAL_CODE, keycode, modifiers) != M_OK) {
                monapi_fatal("parent is not exist");
            }
        } else if (event->getType() == Event::TIMER) {
            cursorEnabled_ = !cursorEnabled_;
            repaint();
            setTimer(TIMER_INTERVAL);
        } else if (event->getType() == Event::CUSTOM_EVENT) {
            if (event->header == MSG_STOP) {
                stop();
            }
        }

    }

    void setColor(bool reversed)
    {
        colors[currentRow_] = reversed;
    }

    void moveCursor(uint16_t col, uint16_t row)
    {
        if (col >= MAX_NUM_COLS) {
            _logprintf("col=%d\n", col);

        }
        ASSERT(row < MAX_NUM_ROWS);
        ASSERT(col < MAX_NUM_COLS);
        currentRow_ = row;
        currentCol_ = col;
    }

    void eraseToEndOfPage()
    {
        std::string& head = lines[currentRow_];
        head.erase(head.begin() + currentCol_, head.end());
        for (uint16_t i = currentRow_ + 1; i < MAX_NUM_ROWS; i++) {
            lines[i].clear();
        }
    }

    void eraseToEndOfLine()
    {
        std::string& line = lines[currentRow_];
        line.erase(line.begin() + currentCol_, line.end());
    }

    void clearLines(int startRow, int endRow)
    {
        for (uint16_t i = startRow; i <= endRow; i++) {
            lines[i].clear();
        }
    }

    void putc(int c)
    {
        _logprintf("%s %s:%d<%c=%x> row=%d col=%d\n", __func__, __FILE__, __LINE__, c, c, currentRow_, currentCol_);
        ASSERT(currentRow_ < MAX_NUM_ROWS);
        ASSERT(currentCol_ < MAX_NUM_COLS);
        std::string& line = lines[currentRow_];
        if (c == '\b') {
            ASSERT(currentRow_ != 0);
            line.erase(line.begin() + currentCol_, line.begin() + currentCol_);
            currentCol_--;
            return;
        }
        if (line.size() == currentCol_) {
            line += c;
        } else if (line.size() < currentCol_) {
            for (uint16_t i = 0; i < currentCol_ - line.size() + 1; i++) {
                line += ' ';
            }
            line += c;
        } else {
            line.erase(line.begin() + currentCol_, line.end());
            line += c;
        }
        currentCol_++;
    }

// void repaint()
// {
//     if (this->__g == NULL) return;

//     int w = getWidth();
//     int h = getHeight();
//     __g->setColor(33, 32, 33);
//     __g->fillRect(0, 0, w, h);

//     Container::repaint();
// }

    virtual void paint(Graphics* g)
    {
        for (int i = 0; i < MAX_NUM_ROWS; i++) {
            std::string& line = lines[i];
            g->setColor(colors[i] ? baygui::Color::white : baygui::Color::black);
            g->fillRect(0, i * fontHeight(), MAX_NUM_COLS * fontWidth(), fontHeight());
            if (line.empty()) {
                continue;
            }
            g->setColor(colors[i] ? baygui::Color::black : baygui::Color::white);
            _logprintf("line:<%s:%d>\n", line.c_str(), colors[i]);
            g->drawString(line.c_str(), 0, fontHeight() * i);
        }
        if (cursorEnabled_) {
            g->setColor(baygui::Color::white);
            int leftOffset = 0;
            std::string& line = lines[currentRow_];
            for (int i = 0; i < currentCol_; i++) {
                char buf[2];
                buf[0] = line[i];
                buf[1] = 0;
                leftOffset += getFontMetrics()->getWidth(buf);
            }
            int cursorWidth = fontWidth();
            if (line.size() > currentCol_) {
                char buf[2];
                buf[0] = line[currentCol_];
                buf[1] = 0;
                cursorWidth = getFontMetrics()->getWidth(buf);
            }
            g->fillRect(leftOffset, currentRow_ * fontHeight(), cursorWidth, fontHeight());
        }
  }

        // if (event->getType() == Event::CUSTOM_EVENT) {
        //     if (event->header == MSG_TEXT) {
        //         size_t length = MESSAGE_INFO_MAX_STR_LENGTH < event->arg1 ? MESSAGE_INFO_MAX_STR_LENGTH : event->arg1;
        //         string text(event->str, length);
        //         string content(textArea_->getText());
        //         content += text;printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        //         textArea_->setText(content.c_str());
        //         repaint();
        //     }
        // }
};

MgFrame* g_frame;

// wrapperPlayLoop for thread
static void __fastcall frameThread(void* arg)
{
    uint32_t parent = (uint32_t)arg;
    g_frame = new MgFrame(parent);
    if (Message::send(parent, MSG_STARTED) != M_OK) {
        monapi_fatal("parent not found");
    }
    g_frame->run();
    delete g_frame;
}

void mona_frame_stop(uint32_t tid)
{
    if (Message::send(tid, MSG_STOP) != M_OK) {
        monapi_fatal("frame stop failed");
    }
}

uint32_t mona_frame_init()
{
    uint32_t parentTid = System::getThreadID();
    uint32_t tid = mthread_create_with_arg(frameThread, (void*)parentTid);
    return tid;
}

void mona_frame_wait_startup(uint32_t tid)
{
    MessageInfo info;
    while(M_OK == Message::receive(&info)) {
        if(info.header == MSG_STARTED && info.from == tid) {
            break;
        }
    }
}


int vasprintf(char **strp, const char *fmt, va_list ap)
{
    const int BUFFER_SIZE = 1024 * 10;
    *strp = (char*)malloc(BUFFER_SIZE);
    if (*strp == NULL) {
        return -1;
    }
    _logprintf("vasprintf"); // tekito
    return vsprintf(*strp, fmt, ap);
}

char* getcwd(char* buf, size_t size)
{
    if (size > 1) {
        strcpy(buf, "/");
        return buf;
    } else {
        return NULL;
    }
}

int chdir(const char* path)
{
    _logprintf("chdir to <%s>\n", path);
    return 0;
}


void bcopy(const void* src, void* dest, size_t n)
{
    memcpy(dest, src, n);
}

void bzero(void* to, size_t count)
{
    memset (to, 0, count);
}

void mona_ttmove(int row, int col)
{
    g_frame->moveCursor(col, row);
}
void mona_tteeol()
{
    g_frame->eraseToEndOfLine();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_tteeop()
{
    g_frame->eraseToEndOfPage();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttbeep()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttinsl(int row, int bot, int nchunk)
{
//    ASSERT(bot - row + 1 == nchunk);
    g_frame->clearLines(row, bot);
}
void mona_ttdell(int row, int bot, int nchunk)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttwindow(int top, int bot)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttnowindow()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttcolor(int color)
{
    ASSERT(color == 0 || color == 1 || color == 2);
    if (color == 0 || color == 1) {
//        g_frame->setColor(false);
    } else {
        g_frame->setColor(true);
    }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
int mona_ttwait(int msec)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
int mona_charswaiting()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

// only support often-used char for a while
static inline char keyToChar(int keycode, int modifiers, int charcode)
{
  if(modifiers & KEY_MODIFIER_CTRL) {
    int basecode = keycode-'A'+1;
    if(basecode >= 0 && basecode <= 32)
      return basecode;
    return -1;
  }
  if(charcode)
    return charcode;
  switch(keycode) {
  case MonAPI::Keys::Enter:
    return 0x0d;
  case MonAPI::Keys::Right:
    return 6;
  case MonAPI::Keys::Left:
    return 2;
  // case MonAPI::Keys::Tab:
  case MonAPI::Keys::Back:
    return MonAPI::Keys::Back;
  case MonAPI::Keys::Delete:
    return  127;
  }
  return -1;
}

// int mona_ttgetc()
// {
//     MessageInfo info;
//     while(M_OK == MonAPI::Message::receive(&info)) {
//         if(info.header == MSG_KEY_VIRTUAL_CODE) {
//             int keycode  = info.arg1;
//             int modifiers = info.arg2;
//             logprintf("keycode=%x", keycode);
//             if (modifiers == KeyEvent::VKEY_CTRL) {
//                 logprintf("ctrl!!!");
//                 return keycode - 'a' + 1; // Ctrl-A = 1, Ctrl-Z = 26
//             }
//             if (keycode == KeyEvent::VKEY_ENTER) {
//                 _logprintf("ENTER");
//                 return 0x0d;
//             } else if (keycode == KeyEvent::VKEY_TAB) {
//                 return '\t';
//             } else {
//                 return keycode;
//             }
//         }
//     }
//     return '\n';
// }

int mona_ttgetc()
{
    static bool hasUngetc = false;
    static int ungetc;
    if (hasUngetc) {
        hasUngetc = false;
        return ungetc;
    }
    MessageInfo info;
    while(M_OK == MonAPI::Message::receive(&info)) {
        if(info.header != MSG_KEY_VIRTUAL_CODE) {
            continue;
        }
        int keycode  = info.arg1;
        int modifiers = info.arg2;
        logprintf("keycode=%x", keycode);
        if (modifiers == KeyEvent::VKEY_CTRL) {
            if ('a' <= keycode && keycode <= 'z') {
                return keycode - 'a' + 1; // Ctrl-A = 1, Ctrl-Z = 26
            } else if (keycode == '/') {
                return 0x1f;
            } else {
                ASSERT(false);
            }

        } else if (modifiers == KeyEvent::VKEY_ALT) {
            hasUngetc = true;
            ungetc = keycode;
            return '\e';
        }
        switch (keycode) {
        case KeyEvent::VKEY_ENTER:
            return 0x0d;
        case KeyEvent::VKEY_TAB:
            return '\t';
        case KeyEvent::VKEY_BACKSPACE:
            return '\b';
        // case KeyEvent::VKEY_ALT:
        // case KeyEvent::VKEY_LMENU:
        //     return '\e';
        default:
            return keycode;
        }
    }
    return '\n';
}

void mona_ttflush()
{
    ASSERT(g_frame);
    // todo message でやるほうが行儀が良い
    g_frame->repaint();
}
int mona_ttputc(int c)
{
    #ifdef MONA
    // todo ttputc と同様にバッファリングすべき
    #endif
    ASSERT(g_frame);
    g_frame->putc(c);
}
int mona_ttcooked()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttclose()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
void mona_ttopen()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}
int mona_ttraw()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

void mona_ttresize()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

void mona_get_file_datetime(const char* file, int* year, int* month, int* day, int* hour, int* min, int* sec)
{
    MonAPI::Date date;
    intptr_t ret = monapi_file_get_date(file, date);
    ASSERT(ret == M_OK);
    *year = date.year();
    *month = date.month();
    *day = date.day();
    *hour = date.hour();
    *min = date.min();
    *sec = date.sec();
}

/*      $OpenBSD: strsep.c,v 1.6 2005/08/08 08:05:37 espie Exp $        */

/*-
 * Copyright (c) 1990, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>

/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */
char *
strsep(char **stringp, const char *delim)
{
        char *s;
        const char *spanp;
        int c, sc;
        char *tok;

        if ((s = *stringp) == NULL)
                return (NULL);
        for (tok = s;;) {
                c = *s++;
                spanp = delim;
                do {
                        if ((sc = *spanp++) == c) {
                                if (c == 0)
                                        s = NULL;
                                else
                                        s[-1] = 0;
                                *stringp = s;
                                return (tok);
                        }
                } while (sc != 0);
        }
        /* NOTREACHED */
}