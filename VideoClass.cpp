#include "VideoClass.hpp"

int VideoClass::openInput()
{
  int ret = 0;
  videoIndex = -1;
  inFmtCtx = avformat_alloc_context();
  assert(inFmtCtx != nullptr);
  
  ret = avformat_open_input(&inFmtCtx, "test.mp4", nullptr, nullptr);
  assert(ret>=0);
  
  ret = avio_open2(&inFmtCtx->pb, "test.mp4", AVIO_FLAG_READ, nullptr, nullptr);
  assert(ret>=0);
  
  ret = avformat_find_stream_info(inFmtCtx, nullptr);
  assert(ret>=0);
  
  for (int i=0;i<inFmtCtx->nb_streams;i++){
    if (inFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
      videoIndex = i;
      break;
    }
  }
  assert(videoIndex >= 0);
  
  AVCodec *decoder = avcodec_find_decoder(inFmtCtx->streams[videoIndex]->codec->codec_id);
  assert(decoder != nullptr);
  
  inDecCtx = inFmtCtx->streams[videoIndex]->codec;
//   inDecCtx = avcodec_alloc_context3(decoder);
  assert(inDecCtx != nullptr);
  
  ret = avcodec_open2(inDecCtx, decoder, nullptr);
  assert(ret>=0);
  
  this->width = inFmtCtx->streams[videoIndex]->codec->width;
  this->height = inFmtCtx->streams[videoIndex]->codec->height;
  
  return 0;
}

VideoClass::VideoClass()
{
  av_register_all();
  avcodec_register_all();
  
  openInput();
}

int VideoClass::start()
{
  AVPacket packet;
  AVFrame *frame;
  AVFrame *targetFrame;
  uint8_t *buffer;
  uint16_t bufferSize;
  int ret = 0;
  WindowView windowView(this->width, this->height);
  
  uint8_t* src_data[4];
  int src_linearsize[4];
  
  uint8_t* dst_data[4];
  int dst_linearsize[4];
  
//   bufferSize = avpicture_get_size(AV_PIX_FMT_YUV420P, this->width, this->height);
  bufferSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, this->width, this->height, 16);
  assert(bufferSize > 0);
  
//   uint8_t dstDataSize = av_image_alloc(dst_data, dst_linearsize, 
// 		 this->width, this->height,
// 		 AV_PIX_FMT_YUV420P, 16);
  
  
  buffer = (uint8_t*)av_malloc(bufferSize);
//   windowView.windowHandle(buffer);
  
  SwsContext *sws = sws_getContext(this->width, this->height,
				  inFmtCtx->streams[videoIndex]->codec->pix_fmt,
				  this->width, this->height,
				  AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr,nullptr, nullptr);
  
  frame = av_frame_alloc();
  targetFrame = av_frame_alloc();
  assert(targetFrame != nullptr);
  assert(frame != nullptr);
  
//   avpicture_fill((AVPicture* )frame, buffer, AV_PIX_FMT_YUV420P, this->width, this->height);
  av_image_fill_arrays(targetFrame->data, targetFrame->linesize, buffer, AV_PIX_FMT_YUV420P, this->width, this->height, 16);
  
  memset(&packet, 0, sizeof(AVPacket));
  av_init_packet(&packet);
  int got_picture = -1;
  int i=0;
  
  while(av_read_frame(inFmtCtx, &packet)>=0){
    
    if (packet.stream_index != videoIndex){
      continue;
    }
    ret = avcodec_decode_video2(inDecCtx, frame, &got_picture, &packet);
//     assert(ret>=0);
    
    if (got_picture != 0){
      std::cout<<"got picture."<<std::endl;
      windowView.windowHandle(frame->data[0]);
//       sws_scale(sws, frame->data, frame->linesize, 0, this->height, targetFrame->data, targetFrame->linesize);
      windowView.draw(i++);
      std::cout<<"clear i "<<i<<std::endl;
    }
  }
  
//   avio_close(inFmtCtx->pb);
  
  av_frame_free(&frame);
  avcodec_close(inFmtCtx->streams[videoIndex]->codec);  
  av_free(buffer);  
 
  avformat_close_input(&inFmtCtx);
  
  avformat_free_context(inFmtCtx);  
  av_free_packet(&packet);
  av_freep(&dst_data[0]);
  sws_freeContext(sws);
}


