#include "utils.h"

const QUrl::FormattingOptions Utils::m_URL_CONVERSION_FLAGS(QUrl::PrettyDecoded | QUrl::PreferLocalFile);
const QRegExp Utils::m_FILE_SCHEME_REG_EXP("^[file://]");
const QRegExp Utils::m_LEADING_SLASHES_REG_EXP("^[/]*");

QString Utils::urlToString(const QUrl &url)
{
    return url.toString(m_URL_CONVERSION_FLAGS).remove(m_FILE_SCHEME_REG_EXP).remove(m_LEADING_SLASHES_REG_EXP);
}

QString Utils::thumbnailGenerationStateToString(Enums::State thumbnailGenerationState)
{
    switch (thumbnailGenerationState)
    {
    case Enums::Idle:
        return "idle";
    case Enums::Working:
        return "working";
    case Enums::Paused:
        return "paused";
    case Enums::Stopped:
        return "stopped";
    case Enums::Completed:
        return "completed";
    default:
        return "unknown";
    }
}

QString Utils::progressToString(float progress)
{
    return QString("%1 %").arg(progress * 100, 0, 'f', 1);
}