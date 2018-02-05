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
  uint8_t *buffer;
  uint16_t bufferSize;
  int ret = 0;
  
  uint8_t* src_data[4];
  int src_linearsize[4];
  
  uint8_t* dst_data[4];
  int dst_linearsize[4];
  
  bufferSize = avpicture_get_size(AV_PIX_FMT_YUV420P, inFmtCtx->streams[videoIndex]->codec->width, inFmtCtx->streams[videoIndex]->codec->height);
  assert(bufferSize > 0);
  
  uint8_t dstDataSize = av_image_alloc(dst_data, dst_linearsize, 
		 inFmtCtx->streams[videoIndex]->codec->width, inFmtCtx->streams[videoIndex]->codec->height,
		 AV_PIX_FMT_YUV420P, 16);
  
  buffer = (uint8_t*)av_malloc(bufferSize);
  
  SwsContext *sws = sws_getContext(inFmtCtx->streams[videoIndex]->codec->width, inFmtCtx->streams[videoIndex]->codec->height,
				  inFmtCtx->streams[videoIndex]->codec->pix_fmt,
				  inFmtCtx->streams[videoIndex]->codec->width, inFmtCtx->streams[videoIndex]->codec->height,
				  AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr,nullptr, nullptr);
  
  frame = av_frame_alloc();
  assert(frame != nullptr);
  
  avpicture_fill((AVPicture* )frame, buffer, AV_PIX_FMT_YUV420P, inFmtCtx->streams[videoIndex]->codec->width, inFmtCtx->streams[videoIndex]->codec->height);
  
  memset(&packet, 0, sizeof(AVPacket));
  av_init_packet(&packet);
  int got_picture = -1;
  
  while(av_read_frame(inFmtCtx, &packet)>=0){
    
    ret = avcodec_decode_video2(inDecCtx, frame, &got_picture, &packet);
//     assert(ret>=0);
    
    if (got_picture != 0){
      std::cout<<"got picture."<<std::endl;
      sws_scale(sws, src_data, src_linearsize, 0, inFmtCtx->streams[videoIndex]->codec->height, dst_data, dst_linearsize);
    }
  }
  
  av_frame_free(&frame);
  avio_close(inFmtCtx->pb);
  
  avcodec_close(inFmtCtx->streams[videoIndex]->codec);  
  av_free(buffer);  

  avio_close(inFmtCtx->pb);  
  avformat_close_input(&inFmtCtx);
  
  avformat_free_context(inFmtCtx);  
  av_free_packet(&packet);
  av_freep(&dst_data[0]);
  sws_freeContext(sws);
}


