#ifndef FILE_SCANNER_H
#define FILE_SCANNER_H

#include <QList>
#include <QString>
#include <QStringList>

struct FileInfo {
    QString displayName;
    QString path;
    QString type;
};

class FileScanner {
public:
    FileScanner() = default;
    static QList<FileInfo> scanAllDrives();

private:
    static QString detectType(const QString &fileName);
    static QString detectTags(const QString &fileName);

    // Declare static members
    static const QStringList mediaExtensions;
    static const QStringList languages;
    static const QStringList years;
};

#endif // FILE_SCANNER_H
