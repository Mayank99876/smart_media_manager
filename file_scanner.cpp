#include "file_scanner.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <windows.h>

// Define static members (must appear exactly once in the program)
const QStringList FileScanner::mediaExtensions = {
    "mp3", "wav", "mp4", "mkv", "avi", "flv", "mov",
    "aac", "wmv", "webm"
};

const QStringList FileScanner::languages = {
    "haryanvi", "mewati", "punjabi", "hindi", "english"
};

const QStringList FileScanner::years = {
    "2023", "2024", "2025"
};

QString FileScanner::detectType(const QString &fileName) {
    QString lower = fileName.toLower();
    if (lower.endsWith(".mp3") || lower.endsWith(".wav") || lower.endsWith(".aac")) {
        return "Song";
    }
    else if (lower.endsWith(".mp4") || lower.endsWith(".mkv") || lower.endsWith(".avi")) {
        return "Movie";
    }
    return "Other";
}

QString FileScanner::detectTags(const QString &fileName) {
    QString lower = fileName.toLower();
    QStringList tags;

    for (const QString &lang : languages) {
        if (lower.contains(lang)) tags.append(lang.toUpper());
    }

    for (const QString &yr : years) {
        if (lower.contains(yr)) tags.append(yr);
    }

    return tags.join(", ");
}

QList<FileInfo> FileScanner::scanAllDrives() {
    QList<FileInfo> result;
    DWORD drives = GetLogicalDrives();

    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            QString drive = QString("%1:/").arg(QChar('A' + i));
            QDir rootDir(drive);
            if (!rootDir.exists()) continue;

            QDirIterator it(drive, QDir::Files | QDir::NoSymLinks,
                          QDirIterator::Subdirectories);
            while (it.hasNext()) {
                QString filePath = it.next();
                QFileInfo info(filePath);
                QString ext = info.suffix().toLower();

                if (mediaExtensions.contains(ext)) {
                    FileInfo f;
                    f.displayName = info.fileName() + " [" + detectTags(info.fileName()) + "]";
                    f.path = filePath;
                    f.type = detectType(info.fileName());
                    result.append(f);
                }
            }
        }
    }
    return result;
}
