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
        cerr << "Invalid argument(s)" << endl;

        return -1;
    }

    // Keep reading packets until we get a complete frame.
    AVPacket av_packet;
    int frame_finished = 0;
    while (av_read_frame(avFormatContext, &av_packet) >= 0
           && !frame_finished)
    {
        // Check that it's a packet from the video stream.
        if (av_packet.stream_index != videoStreamIndex)
        {
            // The packet is from another stream.
            continue;
        }

        // Decode the video frame.
        int av_ret_val = avcodec_decode_video2(avCodecContext, outAvFrame, &frame_finished, &av_packet);
        if (av_ret_val < 0)
        {
            cerr << "Cannot decode video frame: " << AVUtils::avErrorToString(av_ret_val).toStdString() << endl;

            continue;
        }
    }

    // Cleanup.
    av_free_packet(&av_packet);

    return 0;
}

int AVUtils::convertAvFrameToCvMat(struct SwsContext* swsContext, const AVCodecContext* avCodecContext, const AVFrame* avFrame, const AVFrame* avFrameRgb, cv::Mat& outCvImage)
{
    if (!swsContext
            || !avCodecContext
            || !avFrame
            || !avFrameRgb)
    {
        cerr << "Invalid argument(s)" << endl;

        return -1;
    }

    // Convert the image from its native format to RGB.
    sws_scale(swsContext, reinterpret_cast<uint8_t const* const*>(avFrame->data), avFrame->linesize, 0, avCodecContext->height, avFrameRgb->data, avFrameRgb->linesize);

    // Convert the frame to OpenCV Mat.
    cv::Mat cv_image(avFrame->height, avFrame->width, CV_8UC3, avFrameRgb->data[0]);

    // Swap channels from BGR to RGB.
    cv::cvtColor(cv_image, cv_image, CV_BGR2RGB);

    // Return the CV image.
    outCvImage = move(cv_image);

    return 0;
}

QString AVUtils::avErrorToString(int errnum)
{
    char buffer[AV_ERROR_MAX_STRING_SIZE] = { 0 };
    size_t buffer_size = AV_ERROR_MAX_STRING_SIZE;
    const char *av_error = av_make_error_string(buffer, buffer_size, errnum);
    return QString(av_error);
}
