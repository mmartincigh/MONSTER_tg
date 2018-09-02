#ifndef AVUTILS_H
#define AVUTILS_H

#include <QString>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include <opencv2/imgproc/imgproc.hpp>

struct AVFormatContextDeleter
{
    void operator()(AVFormatContext* avfc)
    {
        if (avfc)
        {
            avformat_close_input(&avfc);
        }
    }
};

struct AVCodecContextDeleter
{
    void operator()(AVCodecContext* avcc)
    {
        if (avcc)
        {
            avcodec_close(avcc);
        }
    }
};

struct AVFrameDeleter
{
    void operator()(AVFrame* avf)
    {
        if (avf)
        {
            av_frame_free(&avf);
        }
    }
};

struct AVDeleter
{
    void operator()(void* avp)
    {
        if (avp)
        {
            av_free(avp);
        }
    }
};

struct SwsContextDeleter
{
    void operator()(struct SwsContext* swsc)
    {
        if (swsc)
        {
            sws_freeContext(swsc);
        }
    }
};

class AVUtils
{
public:
    static int decodeFrame(AVFormatContext* avFormatContext, int videoStreamIndex, AVCodecContext* avCodecContext, AVFrame* outAvFrame);
    static int convertAvFrameToCvMat(struct SwsContext* swsContext, const AVCodecContext* avCodecContext, const AVFrame* avFrame, const AVFrame* avFrameRgb, cv::Mat& outCvImage);
    static std::string avErrorToStdString(int errnum);
    static QString avErrorToQString(int errnum);
};

#endif // AVUTILS_H
