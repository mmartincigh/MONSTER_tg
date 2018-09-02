#include <iostream>

extern "C"
{
#include <libswscale/swscale.h>
}

#include "avutils.h"
#include "utils.h"

using namespace std;

int AVUtils::decodeFrame(AVFormatContext* avFormatContext, int videoStreamIndex, AVCodecContext* avCodecContext, AVFrame* outAvFrame)
{
    if (!avFormatContext
            || videoStreamIndex < 0
            || !avCodecContext
            || !outAvFrame)
    {
        qCritical().nospace() << "AVU > ERROR: decodeFrame: Invalid argument(s)";

        return -1;
    }

    // Keep reading packets until we get a complete frame.
    AVPacket av_packet;
    int frame_finished = 0;
    int ret_val = 0;
    while (av_read_frame(avFormatContext, &av_packet) >= 0
           && !frame_finished)
    {
        // Check that the packet is from the video stream.
        if (av_packet.stream_index != videoStreamIndex)
        {
            // The packet is from another stream.
            continue;
        }

        // Decode the video frame.
        int av_ret_val = avcodec_decode_video2(avCodecContext, outAvFrame, &frame_finished, &av_packet);
        if (av_ret_val < 0)
        {
            qWarning().nospace() << "AVU > WARNING: Cannot decode video frame: " << AVUtils::avErrorToQString(av_ret_val);

            continue;
        }
    }
    if (av_packet.pos < 0)
    {
        qWarning().nospace() << "AVU > WARNING: Invalid frame";

        ret_val = -2;
    }

    // Cleanup.
    av_free_packet(&av_packet);

    return ret_val;
}

int AVUtils::convertAvFrameToCvMat(struct SwsContext* swsContext, const AVCodecContext* avCodecContext, const AVFrame* avFrame, const AVFrame* avFrameRgb, cv::Mat& outCvImage)
{
    if (!swsContext
            || !avCodecContext
            || !avFrame
            || !avFrameRgb)
    {
        qCritical().nospace() << "AVU > ERROR: convertAvFrameToCvMat: Invalid argument(s)";

        return -1;
    }

    // Convert the image from its native format to RGB.
    sws_scale(swsContext,
              reinterpret_cast<uint8_t const* const*>(avFrame->data),
              avFrame->linesize,
              0,
              avCodecContext->height,
              avFrameRgb->data,
              avFrameRgb->linesize);

    // Convert the frame to OpenCV Mat.
    cv::Mat cv_image(avFrame->height, avFrame->width, CV_8UC3, avFrameRgb->data[0]);

    // Return the CV image.
    outCvImage = move(cv_image);

    return 0;
}

string AVUtils::avErrorToStdString(int errnum)
{
    char buffer[AV_ERROR_MAX_STRING_SIZE] = { 0 };
    size_t buffer_size = AV_ERROR_MAX_STRING_SIZE;
    const char *av_error = av_make_error_string(buffer, buffer_size, errnum);
    return string(av_error);
}

QString AVUtils::avErrorToQString(int errnum)
{
    char buffer[AV_ERROR_MAX_STRING_SIZE] = { 0 };
    size_t buffer_size = AV_ERROR_MAX_STRING_SIZE;
    const char *av_error = av_make_error_string(buffer, buffer_size, errnum);
    return QString(av_error);
}
