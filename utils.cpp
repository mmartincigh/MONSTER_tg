#include "utils.h"

const QUrl::FormattingOptions Utils::m_URL_CONVERSION_FLAGS(QUrl::PrettyDecoded | QUrl::PreferLocalFile);
const QRegExp Utils::m_FILE_SCHEME_REG_EXP("^file://");
#if defined(Q_OS_WIN)
const QRegExp Utils::m_LEADING_SLASHES_REG_EXP("^/*");
#endif

QString Utils::urlToString(const QUrl &url)
{
    QString string = url.toString(m_URL_CONVERSION_FLAGS).remove(m_FILE_SCHEME_REG_EXP);
#if defined(Q_OS_WIN)
    string.remove(m_LEADING_SLASHES_REG_EXP);
#endif
    return string;
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

#include <vector>

#include "utils.h"

unsigned int Utils::levenshteinDistance(const std::string &fromString, const std::string &toString)
{
    const size_t from_string_size = fromString.size();
    const size_t to_string_size = toString.size();

    if (from_string_size == 0)
    {
        return to_string_size;
    }
    if (to_string_size == 0)
    {
        return from_string_size;
    }

    std::vector<size_t> costs(to_string_size + 1);
    for (size_t i = 0; i <= to_string_size; i++)
    {
        costs[i] = i;
    }

    size_t i = 0;
    for (std::string::const_iterator from_iterator = fromString.begin();
         from_iterator != fromString.end();
         from_iterator++, i++)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (std::string::const_iterator to_iterator = toString.begin();
             to_iterator != toString.end();
             to_iterator++, j++)
        {
            size_t upper = costs[j + 1];
            if (*from_iterator == *to_iterator)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t = upper < corner ? upper : corner;
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    return costs[to_string_size];
}
