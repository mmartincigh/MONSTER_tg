#ifndef AVUTILS_H
#define AVUTILS_H

#include <QString>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <opencv2/imgproc/imgproc.hpp>

class AVUtils
{
public:
    static int decodeFrame(AVFormatContext* avFormatContext, int videoStreamIndex, AVCodecContext* avCodecContext, AVFrame* outAvFrame);
    static int convertAvFrameToCvMat(struct SwsContext* swsContext, const AVCodecContext* avCodecContext, const AVFrame* avFrame, const AVFrame* avFrameRgb, cv::Mat& outCvImage);
    static QString avErrorToString(int errnum);
};

#endif // AVUTILS_H
