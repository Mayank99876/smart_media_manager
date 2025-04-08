#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "file_scanner.h"
#include <QListView>
#include <QComboBox>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void filterModels();
    void filterModels(const QString &searchText, const QString &filterTag);
    void onFileDoubleClicked(const QModelIndex &index);
    void onCustomContextMenuRequested(const QPoint &pos);
    void copySelectedFiles(QListView *view);


private:
    void setupModels();
    void loadFiles();



    Ui::MainWindow *ui;
    QStandardItemModel *movieModel;
    QStandardItemModel *songModel;
    QStandardItemModel *seriesModel;
    QList<FileInfo> allFiles;
};

#endif // MAINWINDOW_H
