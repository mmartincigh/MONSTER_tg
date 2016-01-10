#if defined(Q_OS_WIN)
#include <Windows.h>
#else
#define MAX_PATH 260
#endif

#include <QDir>
#include <QTemporaryDir>
#include <QThread>
#include <QProcess>
#include <QTime>
#include <QImageReader>
#include <QtMath>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "thumbnailgeneratorimpl.h"
#include "utils.h"

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

    QImage no_thumbnail_image(":/images/thumbnails/noThumbnail");
    this->setNoThumbnailImage(no_thumbnail_image.convertToFormat(QImage::Format_RGB888));

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

void ThumbnailGeneratorImpl::setNoThumbnailImage(const QImage &noThumbnailImage)
{
    if (m_noThumbnailImage == noThumbnailImage)
    {
        return;
    }

    m_noThumbnailImage = noThumbnailImage;

    this->debug("No thumbnail image changed: size " + QString::number(m_noThumbnailImage.width()) + "x" + QString::number(m_noThumbnailImage.height()) + " format " + QString::number(m_noThumbnailImage.format()));

    emit this->noThumbnailImageChanged(m_noThumbnailImage);
}

void ThumbnailGeneratorImpl::setState(Enums::State state)
{
    if (m_state == state)
    {
        return;
    }

    m_state = state;

    this->debug("State is changed: " + QString::number(m_state));

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

    this->debug("Errors changed: " + m_errors);

    emit this->errorsChanged(m_errors);
}

void ThumbnailGeneratorImpl::setWarnings(int warnings)
{
    if (m_warnings == warnings)
    {
        return;
    }

    m_warnings = warnings;

    this->debug("Warnings changed: " + m_warnings);

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

    // Prepare the fallback thumbnail beforehand.
    cv::Mat no_thumbnail(m_noThumbnailImage.height(), m_noThumbnailImage.width(), CV_8UC3, const_cast<uchar *>(m_noThumbnailImage.bits()), m_noThumbnailImage.bytesPerLine());

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

        // Create the temporary directory for the thumbnails.
        QTemporaryDir temporary_directory;
        if (!temporary_directory.isValid())
        {
            this->error("Cannot create a temporary directory");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        QString temporary_path = temporary_directory.path();
        this->debug("Temporary directory: " + temporary_path);

        // Check whether the process should be paused, resumed or stopped.
        if (!this->processStateCheckpoint())
        {
            return;
        }

        // Get the width, height and duration of the input video file.
        QString ffprobe_info_command(QString("ffprobe -v error -show_entries stream=width -show_entries stream=height -show_entries stream=display_aspect_ratio -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"%1\"").arg(input_file));
        this->debug("FFprobe info command: " + ffprobe_info_command);
        QProcess ffprobe_info_process;
        ffprobe_info_process.start(ffprobe_info_command);
        if (!ffprobe_info_process.waitForStarted())
        {
            this->error("An error has occurred starting the ffprobe process");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        if (!ffprobe_info_process.waitForFinished())
        {
            this->error("An error has occurred during the execution of the ffprobe process");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        QString ffprobe_info_output = ffprobe_info_process.readAll().simplified();
        this->debug("FFprobe info output: " + ffprobe_info_output);
        QStringList ffprobe_info = ffprobe_info_output.split(' ').filter(QRegExp("^((?!N/A).)*$"));
        this->debug("FFprobe info: " + ffprobe_info.join(' '));
        if (ffprobe_info.size() != 4)
        {
            this->error("FFprobe did not collect the corrent number of info. Expected 4 elements: width, height, display aspect ratio and duration");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }

        // Prepare the locale for the conversions.
        QLocale english_locale(QLocale::English);

        // Convert the input width to a number.
        bool ret_val = false;
        int input_width = english_locale.toInt(ffprobe_info.at(0), &ret_val);
        if (!ret_val)
        {
            this->error("Cannot convert the input width to int");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        this->debug("Input width: " + QString::number(input_width) + " pixels");

        // Convert the input height to a number.
        ret_val = false;
        int input_height = english_locale.toInt(ffprobe_info.at(1), &ret_val);
        if (!ret_val)
        {
            this->error("Cannot convert the input height to int");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        this->debug("Input height: " + QString::number(input_height) + " pixels");

        // Convert the display aspect ratio to numbers.
        QStringList input_display_aspect_ratio_split = ffprobe_info.at(2).split(':');
        if (input_display_aspect_ratio_split.size() != 2)
        {
            this->error("Cannot convert the input display ratio to numbers");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        ret_val = false;
        int input_horizontal_aspect_ratio = english_locale.toInt(input_display_aspect_ratio_split.at(0), &ret_val);
        if (!ret_val)
        {
            this->error("Cannot convert the input horizontal aspect ratio to int");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        if (input_horizontal_aspect_ratio < 0)
        {
            this->error("Invalid input horizontal aspect ratio: " + QString::number(input_horizontal_aspect_ratio));

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        this->debug("Input horizontal aspect ratio: " + QString::number(input_horizontal_aspect_ratio));
        ret_val = false;
        int input_vertical_aspect_ratio = english_locale.toInt(input_display_aspect_ratio_split.at(1), &ret_val);
        if (!ret_val)
        {
            this->error("Cannot convert the input vertical aspect ratio to int");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        if (input_vertical_aspect_ratio < 0)
        {
            this->error("Invalid input vertical aspect ratio: " + QString::number(input_vertical_aspect_ratio));

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        this->debug("Input vertical aspect ratio: " + QString::number(input_vertical_aspect_ratio));
        this->debug("Input display aspect ratio: " + QString::number(input_horizontal_aspect_ratio) + ":" + QString::number(input_vertical_aspect_ratio));

        // Convert the input duration to a number.
        ret_val = false;
        double input_duration = english_locale.toDouble(ffprobe_info.at(3), &ret_val);
        if (!ret_val)
        {
            this->error("Cannot convert the input duration to double");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        this->debug("Input duration: " + QString::number(input_duration) + " seconds");

        // Check whether the video format is consistent with its size.
        cv::Size input_thumbnail_size(input_width, input_height);
        if (input_horizontal_aspect_ratio > 0
                && input_vertical_aspect_ratio > 0
                && input_vertical_aspect_ratio * input_thumbnail_size.width != input_horizontal_aspect_ratio * input_thumbnail_size.height)
        {
            // The display aspect ratio does not match the actual resolution of the video, adjust the input height accordingly.
            input_thumbnail_size.height = static_cast<float>(input_thumbnail_size.width) / input_horizontal_aspect_ratio * input_vertical_aspect_ratio;

            this->debug("Display aspect ratio/video resolution mismatch. Input thumbnail's aspect ratio adjusted: " + QString::number(input_thumbnail_size.width) + "x" + QString::number(input_thumbnail_size.height));
        }

        // Calculate the interval of the thumbnails.
        double thumbnail_interval = input_duration / (thumbnail_number + 1);
        this->debug("Thumbnails interval: " + QString::number(thumbnail_interval) + " seconds");

        // Calculate the offset of the thumbnails.
        QVector<double> thumbnail_offsets(thumbnail_number);
        for (int j = 0; j < thumbnail_number; j++)
        {
            thumbnail_offsets[j] = (j + 1) * thumbnail_interval;
        }

        // Prepare the fallback thumbnail just in case.
        cv::Mat fallback_thumbnail = cv::Mat::zeros(input_thumbnail_size, CV_8UC3);
        no_thumbnail.copyTo(fallback_thumbnail(cv::Rect((input_thumbnail_size.width - no_thumbnail.cols) / 2, (input_thumbnail_size.height - no_thumbnail.rows) / 2, no_thumbnail.cols, no_thumbnail.rows)));

        // Check whether the process should be paused, resumed or stopped.
        if (!this->processStateCheckpoint())
        {
            return;
        }

        // Generate the thumbnails and compose the multi-screenshot thumbnail.
        int output_thumbnail_width = m_thumbnailColumns * input_thumbnail_size.width;
        int output_thumbnail_height = m_thumbnailRows * input_thumbnail_size.height;
        cv::Size output_thumbnail_size(output_thumbnail_width, output_thumbnail_height);
        this->debug("Output thumbnail size: " + QString::number(output_thumbnail_size.width) + "x" + QString::number(output_thumbnail_size.height));
        cv::Mat output_thumbnail = cv::Mat::zeros(output_thumbnail_size, CV_8UC3);
        this->debug("Generating " + QString::number(thumbnail_offsets.size()) + " thumbnails...");
        QDir temp_directory(temporary_path);
        if (!temp_directory.exists())
        {
            this->error("Cannot access the contents of the temporary directory");

            this->setProgress((current_progress += thumbnail_number) / total_progress);
            this->setErrors(m_errors + 1);

            continue;
        }
        int generated_thumbnails = 0;
        for (int thumbnail_index = 0; thumbnail_index < thumbnail_number; thumbnail_index++)
        {
            // Check whether the process should be paused, resumed or stopped.
            if (!this->processStateCheckpoint())
            {
                this->setThumbnailUrl(QUrl());

                return;
            }

            // Generate the thumbnail.
            int current_thumbnail_number = thumbnail_index + 1;
            double thumbnail_offset = thumbnail_offsets.at(thumbnail_index);
            QTime offset_time = QTime::fromMSecsSinceStartOfDay(thumbnail_offset * 1000);
            QString offset_time_string = offset_time.toString("hh_mm_ss");
            QString thumbnail_path = temp_directory.filePath(QString("thumbnail_%4_%5%6").arg(current_thumbnail_number, 3, 10, QChar('0')).arg(offset_time_string).arg(m_OUTPUT_FILE_EXTENSION));
            QString ffmpeg_thumbnail_command(QString("ffmpeg -ss %1 -i \"%2\" -vframes 1 -y \"%3\"").arg(thumbnail_offset).arg(input_file).arg(thumbnail_path));
            QProcess ffmpeg_process;
            ffmpeg_process.start(ffmpeg_thumbnail_command);
            if (!ffmpeg_process.waitForStarted())
            {
                this->error("An error has occurred starting the ffmpeg process");

                this->setProgress(++current_progress / total_progress);
                this->setThumbnailUrl(QUrl());

                continue;
            }
            if (!ffmpeg_process.waitForFinished())
            {
                this->error("An error has occurred during the execution of the ffmpeg process");

                this->setProgress(++current_progress / total_progress);
                this->setThumbnailUrl(QUrl());

                continue;
            }

            // Check whether the thumbnail has been generated.
            QFileInfo thumbnail_file_info(thumbnail_path);
            bool thumbnail_generated = thumbnail_file_info.exists();
            if (thumbnail_generated)
            {
                generated_thumbnails++;

                this->debug("Thumbnail " + QString::number(current_thumbnail_number) + " generated: offset: " + QString::number(thumbnail_offset) + " seconds");
                this->setThumbnailUrl(QUrl::fromLocalFile(thumbnail_path));
            }
            else
            {
                this->warning("The thumbnail number " + QString::number(current_thumbnail_number) + " has not been generated");

                this->setWarnings(m_warnings + 1);
                this->setThumbnailUrl(QUrl());
            }

            // Check whether the process should be paused, resumed or stopped.
            if (!this->processStateCheckpoint())
            {
                this->setThumbnailUrl(QUrl());

                return;
            }

            // Add the thumbnail to the the multi-screenshot thumbnail.
            int row = thumbnail_index / m_thumbnailColumns;
            int column = thumbnail_index % m_thumbnailColumns;
            cv::Mat current_thumbnail = thumbnail_generated ? cv::imread(thumbnail_path.toStdString(), CV_LOAD_IMAGE_ANYCOLOR) : fallback_thumbnail;
            cv::Mat current_thumbnail_final;
            cv::resize(current_thumbnail, current_thumbnail_final, input_thumbnail_size);
            current_thumbnail_final.copyTo(output_thumbnail(cv::Rect(column * input_thumbnail_size.width, row * input_thumbnail_size.height, input_thumbnail_size.width, input_thumbnail_size.height)));

            this->setProgress(++current_progress / total_progress);
        }
        if (generated_thumbnails == 0)
        {
            this->error("No thumbnails for input file: " + input_file);

            this->setErrors(m_errors + 1);
            this->setThumbnailUrl(QUrl());

            continue;
        }
        if (generated_thumbnails < thumbnail_number)
        {
            this->warning("Partial thumbnails (" + QString::number(generated_thumbnails) + " out of " + QString::number(thumbnail_number) + ") for input file: " + input_file);

            this->setWarnings(m_warnings + 1);
        }
        this->setThumbnailUrl(QUrl());
        this->debug("Thumbnails generated");

        // Resize the output thumbnail if necessary.
        cv::Size output_thumbnail_final_size(output_thumbnail_width, output_thumbnail_height);
        if (output_thumbnail_width > m_thumbnailMaxWidth
                || output_thumbnail_height > m_thumbnailMaxHeight)
        {
            float scale = qMin(static_cast<float>(m_thumbnailMaxWidth) / output_thumbnail_width, static_cast<float>(m_thumbnailMaxHeight) / output_thumbnail_height);
            this->debug("Scale: " + QString::number(scale));
            output_thumbnail_final_size.width = qMin(m_thumbnailMaxWidth, qCeil(output_thumbnail_width * scale));
            output_thumbnail_final_size.height = qMin(m_thumbnailMaxHeight, qCeil(output_thumbnail_height * scale));
        }
        this->debug("Output thumbnail final size: " + QString::number(output_thumbnail_final_size.width) + "x" + QString::number(output_thumbnail_final_size.height));
        if (output_thumbnail_final_size.width <= 0
                || output_thumbnail_final_size.height <= 0)
        {
            this->error("Invalid output thumbnail size: " + QString::number(output_thumbnail_final_size.width) + "x" + QString::number(output_thumbnail_final_size.height));

            this->setErrors(m_errors + 1);

            continue;
        }

        // Write the output thumbnail.
        cv::Mat output_thumbnail_final;
        cv::resize(output_thumbnail, output_thumbnail_final, output_thumbnail_final_size);
        ret_val = imwrite(output_file.toStdString(), output_thumbnail_final);
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
