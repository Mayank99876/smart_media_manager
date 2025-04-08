#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QStandardItem>
#include <QDesktopServices>
#include <QFileDialog>
#include <QDebug>
#include <QUrl>
#include <QMenu>
#include <QClipboard>
#include <QFile>
#include <QComboBox>
#include <QMessageBox>
#include <QListView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Make the combobox editable
    ui->categoryFilter->setEditable(true);

//    // Set placeholder text safely
//        ui->categoryFilter->setEditable(true);
//        QMetaObject::invokeMethod(this, [this]() {
//            if (auto le = ui->categoryFilter->lineEdit()) {
//                le->setPlaceholderText("Filter by tag...");
//            }
//        }, Qt::QueuedConnection);

    setupModels();
    loadFiles();

    // Set search bar placeholder
//    ui->searchBar->setPlaceholderText("Search files...");

    // Populate tags
    ui->categoryFilter->addItems({"All", "HARYANVI", "MEWATI", "2025"});

    // Modern Qt5 connection style (preferred)
    connect(ui->searchBar, &QLineEdit::textChanged,
            this, [this](const QString &text) {
                filterModels(text, ui->categoryFilter->currentText());
            });

    connect(ui->categoryFilter, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, [this](const QString &text) {
                filterModels(ui->searchBar->text(), text);
            });

    // Context menu setup
    ui->moviesView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->songsView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->seriesView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Double-click handlers
    connect(ui->moviesView, &QListView::doubleClicked, this, &MainWindow::onFileDoubleClicked);
    connect(ui->songsView, &QListView::doubleClicked, this, &MainWindow::onFileDoubleClicked);
    connect(ui->seriesView, &QListView::doubleClicked, this, &MainWindow::onFileDoubleClicked);

    // Right-click context menus
    connect(ui->moviesView, &QListView::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);
    connect(ui->songsView, &QListView::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);
    connect(ui->seriesView, &QListView::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModels()
{
    movieModel = new QStandardItemModel(this);
    songModel = new QStandardItemModel(this);
    seriesModel = new QStandardItemModel(this);

    if (!movieModel || !songModel || !seriesModel) {
            qFatal("Failed to create models!");
            return;
        }
    ui->moviesView->setModel(movieModel);
    ui->songsView->setModel(songModel);
    ui->seriesView->setModel(seriesModel);

    ui->moviesView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->songsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->seriesView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void MainWindow::loadFiles()
{
    qDebug() << "Starting file scan...";

    allFiles = FileScanner::scanAllDrives();
    if (allFiles.isEmpty()) {
        qWarning() << "No files found during scanning";
        return;
    }

    qDebug() << "Found" << allFiles.size() << "files";

    for (const FileInfo &file : allFiles) {
        if (file.path.isEmpty() || file.displayName.isEmpty()) {
            qWarning() << "Skipping invalid file entry";
            continue;
        }

        // Load icons more safely
        QPixmap pixmap;
        if (!pixmap.load(":/icons/file.png")) {
            qWarning() << "Failed to load icon, using default";
            pixmap = QPixmap(32, 32);  // Create blank icon as fallback
            pixmap.fill(Qt::transparent);
        }
        QIcon icon(pixmap);
        QStandardItem *item = new QStandardItem(icon, file.displayName);
        item->setData(file.path, Qt::UserRole + 1);

        if (file.type == "Movie") {
            if (!movieModel) {
                qCritical() << "Movie model is null!";
                continue;
            }
            movieModel->appendRow(item);
        }
        if (file.type == "Webseries") {
            if (!seriesModel) {
                qCritical() << "Webseries model is null!";
                continue;
            }
            movieModel->appendRow(item);
        }
        if (file.type == "Song") {
            if (!songModel) {
                qCritical() << "Song model is null!";
                continue;
            }
            movieModel->appendRow(item);
        }
        // Similar checks for other models...
    }
}

void MainWindow::onFileDoubleClicked(const QModelIndex &index)
{
    QString path = index.data(Qt::UserRole + 1).toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

// ✅ No-arg version used in slots
void MainWindow::filterModels()
{
    filterModels(ui->searchBar->text(), ui->categoryFilter->currentText());
}

// ✅ Filtering logic
void MainWindow::filterModels(const QString &searchText, const QString &filterTag)
{
    movieModel->clear();
    songModel->clear();
    seriesModel->clear();

    for (const FileInfo &file : allFiles) {
        if (!file.displayName.contains(searchText, Qt::CaseInsensitive))
            continue;

        if (filterTag != "All" && !file.displayName.contains(filterTag, Qt::CaseInsensitive))
            continue;

        QIcon icon(":/icons/file.png");
        QStandardItem *item = new QStandardItem(icon, file.displayName);
        item->setData(file.path, Qt::UserRole + 1);

        if (file.type == "Movie")
            movieModel->appendRow(item);
        else if (file.type == "Song")
            songModel->appendRow(item);
        else if (file.type == "Webseries")
            seriesModel->appendRow(item);
    }
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    QListView *senderView = qobject_cast<QListView *>(sender());
    QMenu menu(this);
    QAction *copyAct = menu.addAction("Copy Selected to Folder...");
    connect(copyAct, &QAction::triggered, [=]() {
        copySelectedFiles(senderView);
    });
    menu.exec(senderView->viewport()->mapToGlobal(pos));
}

void MainWindow::copySelectedFiles(QListView *view)
{
    QModelIndexList selected = view->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    QString dest = QFileDialog::getExistingDirectory(this, "Select Destination Folder");
    if (dest.isEmpty()) return;

    for (const QModelIndex &index : selected) {
        QString srcPath = index.data(Qt::UserRole + 1).toString();
        QFileInfo fileInfo(srcPath);
        QString destPath = dest + "/" + fileInfo.fileName();
        QFile::copy(srcPath, destPath);
    }

    QMessageBox::information(this, "Copy", "Files copied successfully.");
}
