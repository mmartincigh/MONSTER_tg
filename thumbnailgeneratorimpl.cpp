#if defined(Q_OS_WIN)
#include <Windows.h>
#else
#define MAX_PATH 260
#endif

#include <memory>

#include <QDir>
#include <QTemporaryDir>
#include <QThread>
#include <QProcess>
#include <QTime>
#include <QImageReader>
#include <QtMath>
#include <QPainter>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "thumbnailgeneratorimpl.h"
#include "utils.h"
#include "avutils.h"

const QString ThumbnailGeneratorImpl::m_CURRENT_INPUT_FILE_NONE("none");
const QString ThumbnailGeneratorImpl::m_OUTPUT_FILE_EXTENSION(".png");

ThumbnailGeneratorImpl::ThumbnailGeneratorImpl(QMutex *mutex, QWaitCondition *waitCondition, QObject *parent) :
    Base("TGI", parent),
    m_isEnabled(false),
    m_thumbnailRows(0),
    m_thumbnailColumns(0),
    m_thumbnailMaxWidth(0),
    m_thumbnailMaxHeight(0),
    m_noThumbnailImage(),
    m_state(Enums::Idle),
    m_stateDescription(Utils::thumbnailGenerationStateToString(m_state)),
    m_pause(false),
    m_stop(false),
    m_progress(0),
    m_progressString(Utils::progressToString(m_progress)),
    m_errors(0),
    m_warnings(0),
    m_skipped(0),
    m_overwritten(0),
    m_processed(0),
    m_currentInputFile(m_CURRENT_INPUT_FILE_NONE),
    m_thumbnailUrl(),
    m_mutex(mutex),
    m_waitCondition(waitCondition)
{
    QObject::connect(this, SIGNAL(stateChanged(Enums::State)), this, SLOT(onStateChanged(Enums::State)));
    QObject::connect(this, SIGNAL(progressChanged(float)), this, SLOT(onProgressChanged(float)));

    // Register all formats and codecs.
    av_register_all();

    this->debug("Thumbnail generator implementation created");
}

ThumbnailGeneratorImpl::~ThumbnailGeneratorImpl()
{
    this->debug("Thumbnail generator implementation disposed of");
}

void ThumbnailGeneratorImpl::initialize()
{
    int thumbnail_rows = 0;
    emit this->thumbnailRows(&thumbnail_rows);
    this->setThumbnailRows(thumbnail_rows);

    int thumbnail_columns = 0;
    emit this->thumbnailColumns(&thumbnail_columns);
    this->setThumbnailColumns(thumbnail_columns);

    int thumbnail_max_width = 0;
    emit this->thumbnailMaxWidth(&thumbnail_max_width);
    this->setThumbnailMaxWidth(thumbnail_max_width);

    int thumbnail_max_height = 0;
    emit this->thumbnailMaxHeight(&thumbnail_max_height);
    this->setThumbnailMaxHeight(thumbnail_max_height);

    QFile no_thumbnail_image_file(":/images/thumbnails/noThumbnail");
    cv::Mat cv_no_thumbnail_image;
    if (no_thumbnail_image_file.open(QIODevice::ReadOnly))
    {
        qint64 no_thumbnail_image_file_size = no_thumbnail_image_file.size();
        std::vector<char> buffer(no_thumbnail_image_file_size);
        no_thumbnail_image_file.read(buffer.data(), no_thumbnail_image_file_size);
        cv_no_thumbnail_image = imdecode(buffer, cv::IMREAD_COLOR);
    }
    this->setNoThumbnailImage(cv_no_thumbnail_image);

    this->debug("Initialized");
}

bool ThumbnailGeneratorImpl::isEnabled() const
{
    return m_isEnabled;
}

int ThumbnailGeneratorImpl::thumbnailRows() const
{
    return m_thumbnailRows;
}

int ThumbnailGeneratorImpl::thumbnailColumns() const
{
    return m_thumbnailColumns;
}

int ThumbnailGeneratorImpl::thumbnailMaxWidth() const
{
    return m_thumbnailMaxWidth;
}

int ThumbnailGeneratorImpl::thumbnailMaxHeight() const
{
    return m_thumbnailMaxHeight;
}

Enums::State ThumbnailGeneratorImpl::state() const
{
    return m_state;
}

QString ThumbnailGeneratorImpl::stateDescription() const
{
    return m_stateDescription;
}

void ThumbnailGeneratorImpl::setPause(bool pause)
{
    if (m_pause == pause)
    {
        return;
    }

    m_pause = pause;

    this->debug("Pause changed: " + QString(m_pause ? "true" : "false"));

    emit this->pauseChanged(m_pause);
}

void ThumbnailGeneratorImpl::setStop(bool stop)
{
    if (m_stop == stop)
    {
        return;
    }

    m_stop = stop;

    this->debug("Stop changed: " + QString(m_stop ? "true" : "false"));

    emit this->stopChanged(m_stop);
}

float ThumbnailGeneratorImpl::progress() const
{
    return m_progress;
}

QString ThumbnailGeneratorImpl::progressString() const
{
    return m_progressString;
}

int ThumbnailGeneratorImpl::errors() const
{
    return m_errors;
}

int ThumbnailGeneratorImpl::warnings() const
{
    return m_warnings;
}

int ThumbnailGeneratorImpl::skipped() const
{
    return m_skipped;
}

int ThumbnailGeneratorImpl::overwritten() const
{
    return m_overwritten;
}

int ThumbnailGeneratorImpl::processed() const
{
    return m_processed;
}

QString ThumbnailGeneratorImpl::currentInputFile() const
{
    return m_currentInputFile;
}

QUrl ThumbnailGeneratorImpl::thumbnailUrl() const
{
    return m_thumbnailUrl;
}

QImage ThumbnailGeneratorImpl::thumbnail() const
{
    return m_thumbnail;
}

void ThumbnailGeneratorImpl::setIsEnabled(bool isEnabled)
{
    if (m_isEnabled == isEnabled)
    {
        return;
    }

    m_isEnabled = isEnabled;

    this->debug("Is enabled changed: " + QString(m_isEnabled ? "true" : "false"));

    emit this->isEnabledChanged(m_isEnabled);
}

void ThumbnailGeneratorImpl::setThumbnailRows(int thumbnailRows)
{
    if (m_thumbnailRows == thumbnailRows)
    {
        return;
    }

    m_thumbnailRows = thumbnailRows;

    this->debug("Thumbnail rows changed: " + QString::number(m_thumbnailRows));

    emit this->thumbnailRowsChanged(m_thumbnailRows);
}

void ThumbnailGeneratorImpl::setThumbnailColumns(int thumbnailColumns)
{
    if (m_thumbnailColumns == thumbnailColumns)
    {
        return;
    }

    m_thumbnailColumns = thumbnailColumns;

    this->debug("Thumbnail columns changed: " + QString::number(m_thumbnailColumns));

    emit this->thumbnailColumnsChanged(m_thumbnailColumns);
}

void ThumbnailGeneratorImpl::setThumbnailMaxWidth(int thumbnailMaxWidth)
{
    if (m_thumbnailMaxWidth == thumbnailMaxWidth)
    {
        return;
    }

    m_thumbnailMaxWidth = thumbnailMaxWidth;

    this->debug("Thumbnail max width changed: " + QString::number(m_thumbnailMaxWidth));

    emit this->thumbnailMaxWidthChanged(m_thumbnailMaxWidth);
}

void ThumbnailGeneratorImpl::setThumbnailMaxHeight(int thumbnailMaxHeight)
{
    if (m_thumbnailMaxHeight == thumbnailMaxHeight)
    {
        return;
    }

    m_thumbnailMaxHeight = thumbnailMaxHeight;

    this->debug("Thumbnail max height changed: " + QString::number(m_thumbnailMaxHeight));

    emit this->thumbnailMaxHeightChanged(m_thumbnailMaxHeight);
}

void ThumbnailGeneratorImpl::setNoThumbnailImage(const cv::Mat &noThumbnailImage)
{
    // Not doing the comparison because it's too costly.

    noThumbnailImage.copyTo(m_noThumbnailImage);

    this->debug("No thumbnail image changed: size " + QString::number(m_noThumbnailImage.cols) + "x" + QString::number(m_noThumbnailImage.rows) + " format " + QString::number(m_noThumbnailImage.type()));

    emit this->noThumbnailImageChanged(m_noThumbnailImage);
}

void ThumbnailGeneratorImpl::setState(Enums::State state)
{
    if (m_state == state)
    {
        return;
    }

    m_state = state;

    this->debug("State changed: " + QString::number(m_state));

    emit this->stateChanged(m_state);
}

void ThumbnailGeneratorImpl::setStateDescription(const QString &stateDescription)
{
    if (m_stateDescription == stateDescription)
    {
        return;
    }

    m_stateDescription = stateDescription;

    this->debug("State description changed: " + m_stateDescription);

    emit this->stateDescriptionChanged(m_stateDescription);
}

void ThumbnailGeneratorImpl::setProgress(float progress)
{
    if (m_progress == progress)
    {
        return;
    }

    m_progress = progress;

    this->debug("Progress changed: " + QString::number(m_progress));

    emit this->progressChanged(m_progress);
}

void ThumbnailGeneratorImpl::setProgressString(const QString &progressString)
{
    if (m_progressString == progressString)
    {
        return;
    }

    m_progressString = progressString;

    this->debug("Progress string changed: " + m_progressString);

    emit this->progressStringChanged(m_progressString);
}

void ThumbnailGeneratorImpl::setErrors(int errors)
{
    if (m_errors == errors)
    {
        return;
    }

    m_errors = errors;

    this->debug("Errors changed: " + QString::number(m_errors));

    emit this->errorsChanged(m_errors);
}

void ThumbnailGeneratorImpl::setWarnings(int warnings)
{
    if (m_warnings == warnings)
    {
        return;
    }

    m_warnings = warnings;

    this->debug("Warnings changed: " + QString::number(m_warnings));

    emit this->warningsChanged(m_warnings);
}

void ThumbnailGeneratorImpl::setSkipped(int skipped)
{
    if (m_skipped == skipped)
    {
        return;
    }

    m_skipped = skipped;

    this->debug("Skipped changed: " + QString::number(m_skipped));

    emit this->skippedChanged(m_skipped);
}

void ThumbnailGeneratorImpl::setOverwritten(int overwritten)
{
    if (m_overwritten == overwritten)
    {
        return;
    }

    m_overwritten = overwritten;

    this->debug("Overwritten changed: " + QString::number(m_overwritten));

    emit this->overwrittenChanged(m_overwritten);
}

void ThumbnailGeneratorImpl::setProcessed(int processed)
{
    if (m_processed == processed)
    {
        return;
    }

    m_processed = processed;

    this->debug("Processed changed: " + QString::number(m_processed));

    emit this->processedChanged(m_processed);
}

void ThumbnailGeneratorImpl::setCurrentInputFile(const QString &currentInputFile)
{
    if (m_currentInputFile == currentInputFile)
    {
        return;
    }

    m_currentInputFile = currentInputFile;

    this->debug("Current input file changed: " + m_currentInputFile);

    emit this->currentInputFileChanged(m_currentInputFile);
}

void ThumbnailGeneratorImpl::setThumbnailUrl(const QUrl &thumbnailUrl)
{
    if (m_thumbnailUrl == thumbnailUrl)
    {
        return;
    }

    m_thumbnailUrl = thumbnailUrl;

    this->debug("Thumbnail URL changed: " + m_thumbnailUrl.toString());

    emit this->thumbnailUrlChanged(m_thumbnailUrl);
}

void ThumbnailGeneratorImpl::setThumbnail(const QImage &thumbnail)
{
    if (m_thumbnail == thumbnail)
    {
        return;
    }

    m_thumbnail = thumbnail;

    this->debug("Thumbnail changed");

    emit this->thumbnailChanged(m_thumbnail);
}

bool ThumbnailGeneratorImpl::checkIfEnabled()
{
    bool is_source_url_path_valid = false;
    emit this->isSourcePathUrlValid(&is_source_url_path_valid);
    if (!is_source_url_path_valid)
    {
        return false;
    }

    bool is_destination_url_path_valid = false;
    emit this->isDestinationPathUrlValid(&is_destination_url_path_valid);
    if (!is_destination_url_path_valid)
    {
        return false;
    }

    if (m_thumbnailRows <= 0)
    {
        return false;
    }

    if (m_thumbnailColumns <= 0)
    {
        return false;
    }

    if (m_thumbnailMaxWidth <= 0)
    {
        return false;
    }

    if (m_thumbnailMaxHeight <= 0)
    {
        return false;
    }

    return true;
}

bool ThumbnailGeneratorImpl::processStateCheckpoint()
{
    QMutexLocker mutex_locker(m_mutex);
    Q_UNUSED(mutex_locker)

    // Check whether the process should be paused, resumed or stopped.
    if (m_pause)
    {
        // The process should be paused.
        this->setState(Enums::Paused);
        emit this->paused();
        m_waitCondition->wait(m_mutex);

        if (!m_stop)
        {
            // The process has been resumed.
            this->setState(Enums::Working);
            emit this->running();
        }
    }

    if (m_stop)
    {
        // The process should be stopped.
        this->setState(Enums::Stopped);
        emit this->stopped();
        this->setStop(false);

        this->setProgress(0);

        return false;
    }

    return true;
}

void ThumbnailGeneratorImpl::onUpdateThumbnailRows(int thumbnailRows)
{
    if (thumbnailRows <= 0)
    {
        this->setIsEnabled(false);

        return;
    }

    this->setThumbnailRows(thumbnailRows);

    this->setIsEnabled(this->checkIfEnabled());
}

void ThumbnailGeneratorImpl::onUpdateThumbnailColumns(int thumbnailColumns)
{
    if (thumbnailColumns <= 0)
    {
        this->setIsEnabled(false);

        return;
    }

    this->setThumbnailColumns(thumbnailColumns);

    this->setIsEnabled(this->checkIfEnabled());
}

void ThumbnailGeneratorImpl::onUpdateThumbnailMaxWidth(int thumbnailMaxWidth)
{
    if (thumbnailMaxWidth <= 0)
    {
        this->setIsEnabled(false);

        return;
    }

    this->setThumbnailMaxWidth(thumbnailMaxWidth);

    this->setIsEnabled(this->checkIfEnabled());
}

void ThumbnailGeneratorImpl::onUpdateThumbnailMaxHeight(int thumbnailMaxHeight)
{
    if (thumbnailMaxHeight <= 0)
    {
        this->setIsEnabled(false);

        return;
    }

    this->setThumbnailMaxHeight(thumbnailMaxHeight);

    this->setIsEnabled(this->checkIfEnabled());
}

void ThumbnailGeneratorImpl::onIsSourcePathUrlValidChanged(bool isSourcePathUrlValid)
{
    this->debug("Is source path URL vaild: " + QString(isSourcePathUrlValid ? "true" : "false"));

    if (!isSourcePathUrlValid)
    {
        this->setIsEnabled(false);

        return;
    }

    this->setIsEnabled(this->checkIfEnabled());
}

void ThumbnailGeneratorImpl::onIsDestinationPathUrlValidChanged(bool isDestinationPathUrlValid)
{
    this->debug("Is destination path URL vaild: " + QString(isDestinationPathUrlValid ? "true" : "false"));

    if (!isDestinationPathUrlValid)
    {
        this->setIsEnabled(false);

        return;
    }

    this->setIsEnabled(this->checkIfEnabled());
}

void ThumbnailGeneratorImpl::onGenerateThumbnails()
{
    this->debug("Generating thumbnails...");

    if (!m_isEnabled)
    {
        this->error("Thumbnail generation not enabled");

        return;
    }

    QString source_path;
    emit this->sourcePath(&source_path);
    this->debug("Source path: " + source_path);
    if (source_path.isEmpty())
    {
        this->error("Source path is empty");

        return;
    }
    QDir source_directory(source_path);
    if (!source_directory.exists())
    {
        this->error("The source directory does not exist: " + source_directory.path());

        return;
    }

    QString destination_path;
    emit this->destinationPath(&destination_path);
    this->debug("Destination path: " + destination_path);
    if (destination_path.isEmpty())
    {
        this->error("Destination path is empty");

        return;
    }
    QDir destination_directory(destination_path);
    if (!destination_directory.exists())
    {
        this->error("The destination directory does not exist: " + destination_directory.path());

        return;
    }

    QStringList video_files;
    emit this->videoFiles(&video_files);
    if (video_files.size() == 0)
    {
        this->error("The source path contains no video files");

        return;
    }

    bool overwrite_output_files = false;
    emit this->overwriteOutputFiles(&overwrite_output_files);
    this->debug("Overwrite output files: " + QString(overwrite_output_files ? "true" : "false"));

    this->debug("Generating " + QString::number(m_thumbnailRows) + "x" + QString::number(m_thumbnailColumns) + " thumbnails...");
    int thumbnail_number = m_thumbnailRows * m_thumbnailColumns;
    if (thumbnail_number == 0)
    {
        this->error("No elements set to compose the thumbnail");

        return;
    }

    // Generate the thumbnails.
    float total_progress = video_files.size() * thumbnail_number;
    int current_progress = 0;
    this->setProgress(0);
    this->setErrors(0);
    this->setWarnings(0);
    this->setSkipped(0);
    this->setOverwritten(0);
    this->setProcessed(0);
    this->setCurrentInputFile(m_CURRENT_INPUT_FILE_NONE);
    this->setThumbnailUrl(QUrl());
    this->setThumbnail(QImage());
    this->setState(Enums::Working);
    for (int i = 0; i < video_files.size(); i++)
    {
        // Check whether the process should be paused, resumed or stopped.
        if (!this->processStateCheckpoint())
        {
            return;
        }

        // Get the input file.
        QFileInfo input_file_info(source_directory.filePath(video_files.at(i)));
        QString input_file = input_file_info.absoluteFilePath();
        this->debug("Working on file " + input_file + " [" + QString::number(i + 1) + "]...");
        this->setCurrentInputFile(input_file_info.fileName());

        // Check whether the input file exists.
        if (!input_file_info.exists())
        {
            this->error("The input file \"" + input_file + "\" does not exist");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Check whether the output file can be overwritten.
        QFileInfo output_file_info(destination_directory.filePath(input_file_info.completeBaseName()).left(MAX_PATH - m_OUTPUT_FILE_EXTENSION.length() - 1) + m_OUTPUT_FILE_EXTENSION);
        bool output_file_exists = output_file_info.exists();
        if (!overwrite_output_files)
        {
            // The output file cannot be overwritten, check whether the thumbnail already exists.
            if (output_file_exists)
            {
                this->debug("The output thumbnail file \"" + output_file_info.fileName() + "\" already exists, skipping...");

                this->setProgress((current_progress += thumbnail_number) / total_progress);
                this->setSkipped(m_skipped + 1);

                continue;
            }
        }
        QString output_file = output_file_info.absoluteFilePath();

        // Check whether the process should be paused, resumed or stopped.
        if (!this->processStateCheckpoint())
        {
            return;
        }

        // Open the video file.
        AVFormatContext* av_format_context = nullptr;
        int av_ret_val = avformat_open_input(&av_format_context, input_file.toStdString().c_str(), nullptr, nullptr);
        if (av_ret_val < 0)
        {
            this->error("Cannot open file \"" + input_file + "\" " + AVUtils::avErrorToQString(av_ret_val));

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        if (!av_format_context)
        {
            this->error("Invalid format context for file \"" + input_file + "\"");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        std::unique_ptr<AVFormatContext, AVFormatContextDeleter> av_format_context_ptr(av_format_context);

        // Retrieve the stream information.
        av_ret_val = avformat_find_stream_info(av_format_context, nullptr);
        if (av_ret_val < 0)
        {
            this->error("Cannot find the stream information: " + AVUtils::avErrorToQString((av_ret_val)));

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Find the first video stream.
        int video_stream_index = -1;
        for (unsigned int i = 0; i < av_format_context->nb_streams; i++)
        {
            if (av_format_context->streams[i]
                    && av_format_context->streams[i]->codec
                    && av_format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                video_stream_index = i;
                break;
            }
        }
        if (video_stream_index == -1)
        {
            this->error("Cannot find a video stream for file \"" + input_file + "\"");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Get the video stream.
        AVStream* av_video_stream = av_format_context->streams[video_stream_index];
        if (!av_video_stream)
        {
            this->error("No video stream for file \"" + input_file + "\"");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Get the codec context for the video stream.
        AVCodecContext* av_codec_context_original = av_video_stream->codec;
        if (!av_codec_context_original)
        {
            this->error("No codec context for file \"" + input_file + "\"");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        std::unique_ptr<AVCodecContext, AVCodecContextDeleter> av_codec_context_original_ptr(av_codec_context_original);

        // Find the decoder for the video stream.
        AVCodec* av_codec = avcodec_find_decoder(av_codec_context_original->codec_id);
        if (!av_codec)
        {
            this->error("Unsupported codec for codec id " + av_codec_context_original->codec_id);

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Copy the codec context.
        AVCodecContext* av_codec_context = avcodec_alloc_context3(av_codec);
        if (!av_codec_context)
        {
            this->error("Cannot create the codec context");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        std::unique_ptr<AVCodecContext, AVCodecContextDeleter> av_codec_context_ptr(av_codec_context);
        av_ret_val = avcodec_copy_context(av_codec_context, av_codec_context_original);
        if (av_ret_val < 0)
        {
            this->error("Cannot copy the codec context: " + AVUtils::avErrorToQString(av_ret_val));

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Open the codec.
        av_ret_val = avcodec_open2(av_codec_context, av_codec, nullptr);
        if (av_ret_val < 0)
        {
            this->error("Cannot open the codec: " + AVUtils::avErrorToQString(av_ret_val));

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Allocate the video frame.
        AVFrame* av_frame = av_frame_alloc();
        if (!av_frame)
        {
            this->error("Cannot allocate the video frame");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        std::unique_ptr<AVFrame, AVFrameDeleter> av_frame_ptr(av_frame);

        // Allocate the RGB frame.
        AVFrame* av_frame_rgb = av_frame_alloc();
        if (!av_frame_rgb)
        {
            this->error("Cannot allocate the rgb frame");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        std::unique_ptr<AVFrame, AVFrameDeleter> av_frame_rgb_ptr(av_frame_rgb);

        // Determine the required buffer size and allocate the buffer.
        int num_bytes = avpicture_get_size(AV_PIX_FMT_RGB24, av_codec_context->width, av_codec_context->height);
        uint8_t* av_buffer = reinterpret_cast<uint8_t*>(av_malloc(num_bytes * sizeof(uint8_t)));
        if (!av_buffer)
        {
            this->error("Cannot allocate the buffer");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        std::unique_ptr<uint8_t, AVDeleter> av_buffer_ptr(av_buffer);

        // Assign the appropriate parts of the buffer to the image planes in av_frame_rgb.
        // Note that av_frame_rgb is an AVFrame, but AVFrame is a superset of AVPicture.
        av_ret_val = avpicture_fill(reinterpret_cast<AVPicture*>(av_frame_rgb), av_buffer, AV_PIX_FMT_RGB24, av_codec_context->width, av_codec_context->height);
        if (av_ret_val < 0)
        {
            this->error("Cannot fill RGB frame buffer: " + AVUtils::avErrorToQString(av_ret_val));

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Initialize the SWS context for software scaling.
        struct SwsContext* sws_context = sws_getContext(av_codec_context->width,
                                                        av_codec_context->height,
                                                        av_codec_context->pix_fmt,
                                                        av_codec_context->width,
                                                        av_codec_context->height,
                                                        AV_PIX_FMT_BGR24,
                                                        SWS_BILINEAR,
                                                        nullptr,
                                                        nullptr,
                                                        nullptr);
        if (!sws_context)
        {
            this->error("Cannot get SWS context");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        std::unique_ptr<SwsContext, SwsContextDeleter> sws_context_ptr(sws_context);

        // Get the width, height and duration of the input video file.
        int av_input_width = av_codec_context_original->width;
        int av_input_height = av_codec_context_original->height;
        this->debug("Input width: " + QString::number(av_input_width) + " pixels");
        this->debug("Input height: " + QString::number(av_input_height) + " pixels");

        int av_input_horizontal_aspect_ratio = av_video_stream->display_aspect_ratio.num;
        int av_input_vertical_aspect_ratio = av_video_stream->display_aspect_ratio.den;
        this->debug("Input horizontal aspect ratio: " + QString::number(av_input_horizontal_aspect_ratio));
        this->debug("Input vertical aspect ratio: " + QString::number(av_input_vertical_aspect_ratio));
        this->debug("Input display aspect ratio: " + QString::number(av_input_horizontal_aspect_ratio) + ":" + QString::number(av_input_vertical_aspect_ratio));

        double av_input_duration = static_cast<double>(av_video_stream->duration) * av_video_stream->time_base.num / av_video_stream->time_base.den;
        this->debug("Input duration: " + QString::number(av_input_duration) + " seconds");

        // Check whether the video format is consistent with its size.
        cv::Size cv_input_thumbnail_size(av_input_width, av_input_height);
        if (av_input_horizontal_aspect_ratio > 0
                && av_input_vertical_aspect_ratio > 0
                && av_input_vertical_aspect_ratio * cv_input_thumbnail_size.width != av_input_horizontal_aspect_ratio * cv_input_thumbnail_size.height)
        {
            // The display aspect ratio does not match the actual resolution of the video, adjust the input height accordingly.
            cv_input_thumbnail_size.height = static_cast<float>(cv_input_thumbnail_size.width) / av_input_horizontal_aspect_ratio * av_input_vertical_aspect_ratio;

            this->debug("Display aspect ratio/video resolution mismatch. Input thumbnail's aspect ratio adjusted: " + QString::number(cv_input_thumbnail_size.width) + "x" + QString::number(cv_input_thumbnail_size.height));
        }

        // Check whether the process should be paused, resumed or stopped.
        if (!this->processStateCheckpoint())
        {
            return;
        }

        // Calculate the size of the output thumbnail.
        cv::Size cv_output_thumbnail_size(m_thumbnailColumns * cv_input_thumbnail_size.width,
                                          m_thumbnailRows * cv_input_thumbnail_size.height);
        this->debug("Output thumbnail size: " + QString::number(cv_output_thumbnail_size.width) + "x" + QString::number(cv_output_thumbnail_size.height));

        // Resize the output thumbnail if necessary.
        if (cv_output_thumbnail_size.width > m_thumbnailMaxWidth
                || cv_output_thumbnail_size.height > m_thumbnailMaxHeight)
        {
            // Scale the output thumbnail.
            float scaling_factor = qMin(static_cast<float>(m_thumbnailMaxWidth) / cv_output_thumbnail_size.width,
                                        static_cast<float>(m_thumbnailMaxHeight) / cv_output_thumbnail_size.height);
            this->debug("Scaling factor: " + QString::number(scaling_factor));
            cv_output_thumbnail_size.width = qMin(m_thumbnailMaxWidth,
                                                  qCeil(cv_output_thumbnail_size.width * scaling_factor));
            cv_output_thumbnail_size.height = qMin(m_thumbnailMaxHeight,
                                                   qCeil(cv_output_thumbnail_size.height * scaling_factor));

            // Adjust the input thumbnail size accordingly.
            cv_input_thumbnail_size.width = cv_output_thumbnail_size.width / m_thumbnailColumns;
            cv_input_thumbnail_size.height = cv_output_thumbnail_size.height / m_thumbnailRows;
        }
        this->debug("Output thumbnail final size: " + QString::number(cv_output_thumbnail_size.width) + "x" + QString::number(cv_output_thumbnail_size.height));
        if (cv_output_thumbnail_size.width <= 0
                || cv_output_thumbnail_size.height <= 0)
        {
            this->error("Invalid output thumbnail size: " + QString::number(cv_output_thumbnail_size.width) + "x" + QString::number(cv_output_thumbnail_size.height));

            this->setErrors(m_errors + 1);

            continue;
        }
        this->debug("Input thumbnail final size: " + QString::number(cv_input_thumbnail_size.width) + "x" + QString::number(cv_input_thumbnail_size.height));
        if (cv_input_thumbnail_size.width <= 0
                || cv_input_thumbnail_size.height <= 0)
        {
            this->error("Invalid input thumbnail size: " + QString::number(cv_input_thumbnail_size.width) + "x" + QString::number(cv_input_thumbnail_size.height));

            this->setErrors(m_errors + 1);

            continue;
        }

        // Initialize the output thumbnail.
        cv::Mat cv_output_thumbnail = cv::Mat::zeros(cv_output_thumbnail_size, CV_8UC3);

        // Prepare the fallback thumbnail just in case.
        cv::Mat cv_fallback_thumbnail = cv::Mat::zeros(cv_input_thumbnail_size, CV_8UC3);
        float cv_no_thumbnail_scaling_factor = cv_input_thumbnail_size.height / 3.f / m_noThumbnailImage.rows;
        this->debug("No thumbnail scaling factor: " + QString::number(cv_no_thumbnail_scaling_factor));
        cv::Mat cv_no_thumbnail_resized;
        cv::resize(m_noThumbnailImage,
                   cv_no_thumbnail_resized,
                   cv::Size(),
                   cv_no_thumbnail_scaling_factor,
                   cv_no_thumbnail_scaling_factor,
                   cv::INTER_CUBIC);
        cv_no_thumbnail_resized.copyTo(cv_fallback_thumbnail(cv::Rect((cv_input_thumbnail_size.width - cv_no_thumbnail_resized.cols) / 2,
                                                                      (cv_input_thumbnail_size.height - cv_no_thumbnail_resized.rows) / 2,
                                                                      cv_no_thumbnail_resized.cols,
                                                                      cv_no_thumbnail_resized.rows)));

        // Compose the multi-screenshot thumbnail with the fallback thumbnail.
        for (int c = 0; c < m_thumbnailColumns; c++)
        {
            for (int r = 0; r < m_thumbnailRows; r++)
            {
                cv_fallback_thumbnail.copyTo(cv_output_thumbnail(cv::Rect(c * cv_input_thumbnail_size.width,
                                                                          r * cv_input_thumbnail_size.height,
                                                                          cv_input_thumbnail_size.width,
                                                                          cv_input_thumbnail_size.height)));
            }
        }

        // Calculate the interval of the thumbnails.
        double thumbnail_interval = av_input_duration / (thumbnail_number + 1);
        this->debug("Thumbnails interval: " + QString::number(thumbnail_interval) + " seconds");
        int generated_thumbnails = 0;
        int64_t av_seek_step = av_video_stream->duration / (thumbnail_number + 1);
        int64_t av_seek_timestamp = av_video_stream->start_time;
        this->debug("Generating " + QString::number(thumbnail_number) + " thumbnails...");
        for (int thumbnail_index = 0; thumbnail_index < thumbnail_number; thumbnail_index++)
        {
            // Check whether the process should be paused, resumed or stopped.
            if (!this->processStateCheckpoint())
            {
                this->setThumbnailUrl(QUrl());
                this->setThumbnail(QImage());

                return;
            }

            // Flush the codec buffer before every seek.
            avcodec_flush_buffers(av_codec_context);

            // Seek to next timestamp.
            av_seek_timestamp += av_seek_step;
            av_ret_val = av_seek_frame(av_format_context, video_stream_index, av_seek_timestamp, AVSEEK_FLAG_FRAME);
            if (av_ret_val < 0)
            {
                this->error("Cannot seek to timestamp " + QString::number(av_seek_timestamp) + ": " + AVUtils::avErrorToQString(av_ret_val));

                this->setProgress(++current_progress / total_progress);
                this->setThumbnailUrl(QUrl());
                this->setThumbnail(QImage());

                continue;
            }

            // Decode the next frame on the video stream.
            if (AVUtils::decodeFrame(av_format_context, video_stream_index, av_codec_context, av_frame) < 0)
            {
                this->error("Cannot decode a frame");

                this->setProgress(++current_progress / total_progress);
                this->setThumbnailUrl(QUrl());
                this->setThumbnail(QImage());

                continue;
            }

            // Convert the AV frame to an OpenCV Mat.
            cv::Mat cv_current_thumbnail;
            if (AVUtils::convertAvFrameToCvMat(sws_context, av_codec_context, av_frame, av_frame_rgb, cv_current_thumbnail) < 0)
            {
                this->error("Cannot convert the frame to a CV image");

                this->setProgress(++current_progress / total_progress);
                this->setThumbnailUrl(QUrl());
                this->setThumbnail(QImage());

                continue;
            }

            // Thumbnail generated.
            generated_thumbnails++;

            // Set the thumbnail image in the GUI.
            /*QImage current_thumbnail_image(cv_current_thumbnail.data,
                                           cv_current_thumbnail.cols,
                                           cv_current_thumbnail.rows,
                                           static_cast<int>(cv_current_thumbnail.step),
                                           QImage::Format_RGB888);
            this->setThumbnail(current_thumbnail_image);*/

            // Check whether the process should be paused, resumed or stopped.
            if (!this->processStateCheckpoint())
            {
                this->setThumbnailUrl(QUrl());
                this->setThumbnail(QImage());

                return;
            }

            // Add the thumbnail to the multi-screenshot thumbnail.
            int row = thumbnail_index / m_thumbnailColumns;
            int column = thumbnail_index % m_thumbnailColumns;
            cv::Mat cv_current_thumbnail_final;
            cv::resize(cv_current_thumbnail, cv_current_thumbnail_final, cv_input_thumbnail_size);
            cv_current_thumbnail_final.copyTo(cv_output_thumbnail(cv::Rect(column * cv_input_thumbnail_size.width,
                                                                           row * cv_input_thumbnail_size.height,
                                                                           cv_input_thumbnail_size.width,
                                                                           cv_input_thumbnail_size.height)));

            this->setProgress(++current_progress / total_progress);
        }
        if (generated_thumbnails == 0)
        {
            this->error("No thumbnails for input file: " + input_file);

            this->setErrors(m_errors + 1);
            this->setThumbnailUrl(QUrl());
            this->setThumbnail(QImage());

            continue;
        }
        if (generated_thumbnails < thumbnail_number)
        {
            this->warning("Partial thumbnails (" + QString::number(generated_thumbnails) + " out of " + QString::number(thumbnail_number) + ") for input file: " + input_file);

            this->setWarnings(m_warnings + 1);
        }
        this->setThumbnailUrl(QUrl());
        this->setThumbnail(QImage());
        this->debug("Thumbnails generated");

        // Convert the output thumbnail to QImage.
        QImage output_thumbnail_image(cv_output_thumbnail.data,
                                      cv_output_thumbnail.cols,
                                      cv_output_thumbnail.rows,
                                      static_cast<int>(cv_output_thumbnail.step),
                                      QImage::Format_RGB888);

        // Write the filename on the output thumbnail.
        QString text = input_file_info.fileName();
        int text_left_margin = 10;
        int text_bottom_margin = 10;
        QPainter painter(&output_thumbnail_image);
        QFont font = painter.font();
        font.setPixelSize(48);
        painter.setFont(font);
        painter.setPen(QPen(Qt::white));
        painter.drawText(text_left_margin, output_thumbnail_image.height() - text_bottom_margin, text);

        // Write the output thumbnail (mind the RBG swap).
        bool ret_val = output_thumbnail_image.rgbSwapped().save(output_file);
        if (!ret_val)
        {
            this->error("Cannot write output thumbnail: " + output_file);

            this->setErrors(m_errors + 1);

            continue;
        }
        this->debug("Output thumbnail written: " + output_file);
        if (output_file_exists)
        {
            this->debug("Output file overwritten");
            this->setOverwritten(m_overwritten + 1);
        }
        this->setProcessed(m_processed + 1);
    }
    this->setProgress(1);
    this->setState(Enums::Completed);
    this->setCurrentInputFile(m_CURRENT_INPUT_FILE_NONE);

    this->debug("Thumbnails generated");
}

void ThumbnailGeneratorImpl::onStateChanged(Enums::State state)
{
    this->setStateDescription(Utils::thumbnailGenerationStateToString(state));
}

void ThumbnailGeneratorImpl::onProgressChanged(float progress)
{
    this->setProgressString(Utils::progressToString(progress));
}
