/*
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <monapi.h>

#include "./facebook_service.h"
#include "./feed_view.h"
#include "./button.h"

namespace facebook {

FeedView::FeedView(int x, int y, int w, int h)
  : x_(x),
    y_(y),
    w_(w),
    h_(h),
    like_button_(new facebook::Button("Like")),
    comment_button_(new facebook::Button("comment")),
    text_(new TextField()),
    icon_(new ImageIcon(new WebImage())),
    feed_id_(""),
    num_likes_(0),
    num_comments_(0) {
  // todo w, h limit
  text_->setBounds(x + SIDE_BAR_WIDTH, y,
                   w - MARGIN - LIKE_BUTTON_WIDTH, h - 5);
  icon_->setBounds(0, y + IMAGE_HEIGHT + IMAGE_MARGIN_TOP,
                   IMAGE_WIDTH, IMAGE_HEIGHT);
  text_->setForeground(monagui::Color::black);
  text_->setEditable(false);
  text_->setBorderColor(monagui::Color::white);
  like_button_->setBackground(monagui::Color::white);
  like_button_->setBounds(
      x, y + IMAGE_HEIGHT + IMAGE_MARGIN_TOP + LIKE_BUTTON_MARGIN_TOP,
      LIKE_BUTTON_WIDTH, LIKE_BUTTON_HEIGHT);
  comment_button_->setBounds(x, y, 50, 50);
}

FeedView::~FeedView() {
}

void FeedView::components(Components* ret) {
  ret->push_back(like_button_.get());
  ret->push_back(text_.get());
  ret->push_back(comment_button_.get());
  ret->push_back(icon_.get());
}

void FeedView::set_image_path(const std::string& uri, const std::string& path) {
  (static_cast<WebImage*>(icon_->getImage()))->initialize(uri, path);
}

void FeedView::set_text(const std::string& text) {
  std::string content = fold_line(text, 70);
  if (num_likes_ > 0) {
    content += "\n";
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", num_likes_);
    content += buf;
    content += "人がいいね！と言っています。";
  }

  if (num_comments_ > 0) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", num_comments_);
    content += buf;
    content += "個のコメント";

    for (Comments::const_iterator it = comments_.begin();
         it != comments_.end(); ++it) {
      content += (*it).body;
      content += "\n";
    }
  }

  text_->setText(content.c_str());
}

void FeedView::setup_from_feed(const Feed& feed) {
  set_image_path(feed.profile_image_url(), feed.local_image_path());
  feed_id_ = feed.feed_id;
  num_likes_ = feed.num_likes;
  num_comments_ = feed.num_comments;
  set_text(feed.text);
  comments_ = feed.comments;
}

void FeedView::set_empty() {
  feed_id_ = "";
  num_likes_ = 0;
  set_text("");
}

void FeedView::draw(Graphics* g) {
  dword c = g->getColor();
  g->setColor(monagui::Color::gray);
  g->drawLine(x_, y_ + h_ - 2, x_ + w_, y_ + h_ - 2);
  g->setColor(c);
}

void FeedView::open_comment() {
  uint32_t tid;
  std::string command;
  if (monapi_file_exists("/MEM/FB.EX5")) {
    command = "/MEM/FB.EX5 ";
  } else {
    command = "/APPS/MONAGUI/FACEBOOK.EX5 ";
  }
  command += feed_id_;
  int result = monapi_process_execute_file_get_tid(
      command.c_str(),
      MONAPI_TRUE,
      &tid,
      MonAPI::System::getProcessStdinID(),
      MonAPI::System::getProcessStdoutID());
  if (result != 0) {
    monapi_fatal("can't exec Mosh");
  }
}

void FeedView::add_like() {
  if (!feed_id_.empty()) {
    FacebookService::add_like(feed_id_);
  }
}

std::string FeedView::fold_line(const std::string& line,
                                size_t max_line_length) {
  size_t len = 0;
  std::string ret;
  for (std::string::const_iterator it = line.begin(); it != line.end(); ++it) {
    ret += *it;
    if ((*it) == '\n') {
      len = 0;
    } else {
      len++;
    }

    if (len >= max_line_length) {
      ret += '\n';
      len = 0;
    }
  }
  return ret;
}
}
