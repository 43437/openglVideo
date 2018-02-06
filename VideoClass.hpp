#ifndef __VIDEO_CLASS__
#define __VIDEO_CLASS__

#include <iostream>
#include <mutex>
#include "WindowView.hpp"

extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}
#include <cassert>

class VideoClass{
  
  AVFormatContext *inFmtCtx;
  AVCodecContext *inDecCtx;
  int64_t width;
  int64_t height;
  
  int videoIndex;
  int openInput();
public:
  VideoClass();
  int start();
};
#endif