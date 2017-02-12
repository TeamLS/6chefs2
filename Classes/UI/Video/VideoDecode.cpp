/**
 * 视频解码类
 */
//#include "CCVideoDecode.h"
//#include "CCVideoTextureCache.h"
//NS_CC_BEGIN
#include "VideoDecode.h"
#include "VideoTextureCache.h"

//CCVideoPic::CCVideoPic()
VideoPic::VideoPic()
{
    m_pPicture = NULL;
}

//CCVideoPic::~CCVideoPic()
VideoPic::~VideoPic()
{  
    if(m_pPicture)
    {
        delete m_pPicture; 
    }
}

//bool CCVideoPic::init(const char *path, int frame,unsigned int width,  unsigned int height, unsigned char* data)
bool VideoPic::init(const char *path, int frame,unsigned int width,  unsigned int height, unsigned char* data)
{
    //unsigned char* data = pic.data[m_videoStream];
    m_width = width;
    m_height = height;
    m_frame = frame;
    m_path = path;
    m_length = m_width * m_height * 4;
    m_pPicture = new unsigned char[m_length];
    for(unsigned int i = 0; i < m_length; ++i)
    {
        m_pPicture[i] = data[i];
    }
    return true;
}

//CCVideoDecode::CCVideoDecode()
VideoDecode::VideoDecode()
{
	m_filepath = NULL;
	m_pFormatCtx = NULL;
	m_videoStream = -1;
    m_pSwsCtx = NULL;
    m_pCodecCtx = NULL;
    m_frameCount = 0;
}

//bool CCVideoDecode::init(const char *path)
bool VideoDecode::init(const char *path)
{
    m_filepath = path;
   
    // Register all formats and codecs  
    av_register_all();  
      
    /* 1、构建avformat */
    if(avformat_open_input(&m_pFormatCtx, path, NULL, NULL) != 0) {  
         CCLOG("avformat_open_input false");
         return false;  
    }
    
    /* 2、获取流信息 */
    if(avformat_find_stream_info(m_pFormatCtx, NULL) < 0) {  
        CCLOG("avformat_find_stream_info false");
        return false;  
    } 

    m_videoStream = -1;
   
    for(int i=0; i<m_pFormatCtx->nb_streams; i++) {  
        
        //if(m_videoStream == -1 && m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
        if(m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            m_videoStream = i; 
            break;
        }
    } 
    
    //  没有视频流，无法播放  
    if(m_videoStream == -1) {
        CCLOGERROR("没有视频流，无法播放");  
        return false;  
    } 

    m_pCodecCtx = m_pFormatCtx->streams[m_videoStream]->codec;
    // 获取基本信息
    if (m_pCodecCtx->width) 
    {
        m_width = m_pCodecCtx->width;
        m_height = m_pCodecCtx->height;
    }
    else
    {
        CCLOGERROR("获取视频尺寸失败");  
        return false;
    }

     /*
      Duration: 00:00:07.32, start: 0.000000, bitrate: 579 kb/s
    Stream #0:0: Video: vp6a, yuva420p, 224x240,
     31 fps, 31 tbr, 1k tbn, 1k tbc
     */
    
    //算时间
    int64_t duration = 0;
    if (m_pFormatCtx->duration != AV_NOPTS_VALUE) {
        /*int hours, mins, secs, us;*/
        duration = m_pFormatCtx->duration + 5000;
        /*secs = duration / AV_TIME_BASE;
        us = duration % AV_TIME_BASE;
        mins = secs / 60;
        secs %= 60;
        hours = mins / 60;
        mins %= 60;
        CCLOG("%02d:%02d:%02d.%02d", hours, mins, secs, (100 * us) / AV_TIME_BASE);*/
    } else {
        CCLOGERROR("duration is null");
        return false;
    }
    
    AVRational rational;

    if(m_pFormatCtx->streams[m_videoStream]->avg_frame_rate.den && m_pFormatCtx->streams[m_videoStream]->avg_frame_rate.num)
    {           
        rational = m_pFormatCtx->streams[m_videoStream]->avg_frame_rate;
    }
    else if(m_pFormatCtx->streams[m_videoStream]->r_frame_rate.den && m_pFormatCtx->streams[m_videoStream]->r_frame_rate.num)
    {
        rational = m_pFormatCtx->streams[m_videoStream]->r_frame_rate;
    }
    else
    {
        CCLOGERROR("fps 获取失败");  
        return false;
    }

    double fps = av_q2d(rational);
    m_frameRate = 1.0 / fps;
    m_frames = (int)((fps * duration) / AV_TIME_BASE);
    CCLOG("m_frameRate = %f , frames = %d", m_frameRate, m_frames);



    AVCodec *pCodec = NULL;
    pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);

    if(pCodec == NULL) { 
        CCLOGERROR("avcodec_find_decoder error");
        return false;
    }  
      
    //if(avcodec_open2(m_pCodecCtx, pCodec, NULL)) {
    if(avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0) {
        CCLOGERROR("avcodec_open2 error");
        return false;
    }  

   /* m_pFrame=avcodec_alloc_frame();
    CCLOG("width: %d, hight: %d", m_pCodecCtx->width, m_pCodecCtx->height);
    m_pSwsCtx = sws_getContext(m_pCodecCtx->width,  
                                     m_pCodecCtx->height,  
                                     m_pCodecCtx->pix_fmt,
                                     m_pCodecCtx->width,  
                                     m_pCodecCtx->height,  
                                     PIX_FMT_RGBA,  
                                     SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if(!m_pSwsCtx)
    {
        CCLOGERROR("sws_getContext error");
        return false;
    }*/
    return true;
}


//unsigned int CCVideoDecode::getWidth()
unsigned int VideoDecode::getWidth()
{
    return m_width;
}

//unsigned int CCVideoDecode::getHeight()
unsigned int VideoDecode::getHeight()
{
    return m_height;
}

//double CCVideoDecode::getFrameRate(){
double VideoDecode::getFrameRate()
{
    return m_frameRate;
}

//unsigned int CCVideoDecode::getFrames()
unsigned int VideoDecode::getFrames()
{
    return m_frames;
}

//const char* CCVideoDecode::getFilePath()
const char* VideoDecode::getFilePath()
{
    return m_filepath;
}

//CCVideoDecode::~CCVideoDecode()
VideoDecode::~VideoDecode()
{
    CCLOGINFO("cocos2d: deallocing CCVideoDecode.");
    //if(m_pSwsCtx) sws_freeContext(m_pSwsCtx); 
    

    // Free the YUV frame 
    //if(m_pFrame) av_free(m_pFrame);  
    
    // Close the codec  
    if (m_pCodecCtx) avcodec_close(m_pCodecCtx);  
    if (m_pFormatCtx) avformat_close_input(&m_pFormatCtx);

}

/**
 * 解码
 * @return [description]
 */
//bool CCVideoDecode::decode()
bool VideoDecode::decode()
{
    if(m_frameCount == -1)
        return false;

    AVPacket packet;
   // av_init_packet(&packet);
    int frameFinished = 0;
    m_pFrame = NULL;
    while(!frameFinished && av_read_frame(m_pFormatCtx, &packet)>=0)
    { 
        if(packet.stream_index== m_videoStream)
        {
            //m_pFrame = avcodec_alloc_frame();
            m_pFrame = av_frame_alloc();

            int lentmp = avcodec_decode_video2(m_pCodecCtx, m_pFrame, &frameFinished, &packet);
            if(lentmp <=0)
            {
                av_free(m_pFrame);
                return false;
            }
        }
        //av_free_packet(&packet);
        av_packet_unref(&packet);
    }

    if(m_pFrame == NULL)
    {
        return false;
    }
    
    m_pSwsCtx = sws_getContext(m_pCodecCtx->width,
        m_pCodecCtx->height,
        m_pCodecCtx->pix_fmt,
        m_width,
        //m_height, PIX_FMT_RGBA, SWS_FAST_BILINEAR, NULL, NULL, NULL);
        m_height, AV_PIX_FMT_RGBA, SWS_SPLINE, NULL, NULL, NULL);
    if(!m_pSwsCtx)
    {
        CCLOGERROR("sws_getContext error");
        return false;
    }

    //AVPicture pic;
    AVFrame *m_pFrameRGB;
    m_pFrameRGB = av_frame_alloc();
    m_pFrameRGB->width = m_width;
    m_pFrameRGB->height = m_height;
    //unsigned char* pointers[4];
    //int linesizes[4];
    //avpicture_alloc(&pic, PIX_FMT_RGBA, m_width, m_height);
    //avpicture_alloc(&pic, AV_PIX_FMT_RGBA, m_width, m_height);
    av_image_alloc(m_pFrameRGB->data, m_pFrameRGB->linesize, m_width, m_height, AV_PIX_FMT_RGBA, 32);
    
    CCLOG("avpicture_alloc width = %d height = %d m_videoStream = %d", m_width, m_height, m_videoStream);
    //sws_scale(m_pSwsCtx, m_pFrame->data, m_pFrame->linesize, 0, m_height, pic.data, pic.linesize);
    sws_scale(m_pSwsCtx, m_pFrame->data, m_pFrame->linesize, 0, m_height, m_pFrameRGB->data, m_pFrameRGB->linesize);
    
    m_frameCount++;

    //CCVideoPic *pVideoPic = new CCVideoPic();
    VideoPic *pVideoPic = new VideoPic();
    //pVideoPic->init(m_filepath, m_frameCount, m_width, m_height, m_pFrame->data[m_videoStream]);
    pVideoPic->init(m_filepath, m_frameCount, m_width, m_height, m_pFrameRGB->data[m_videoStream]);
    //CCVideoTextureCache::sharedTextureCache()->addPicData(pVideoPic);
    VideoTextureCache::sharedTextureCache()->addPicData(pVideoPic);
    

    //avpicture_free(&pic);
    av_free(m_pFrameRGB);
    av_free(m_pFrame);

    if (frameFinished == 0)
    {
        //重头开始解码
        //av_seek_frame(m_pFormatCtx, m_videoStream , 0, AVSEEK_FLAG_ANY);
        m_frameCount = -1;
    }
    //av_free_packet(&packet);
    av_packet_unref(&packet);
    return true;
}

//NS_CC_END
