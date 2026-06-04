#ifndef WIDGET_H
#define WIDGET_H

#include <QDir>
#include <QWidget>

class QTableView;
class QStandardItemModel;
class QPushButton;
class QMenu;

class Widget : public QWidget
{
    Q_OBJECT

public:
    static const int COL_FULLPATH = 4;  // 隐藏的完整路径列

public:
    Widget(QWidget* parent = 0);
    ~Widget();

public slots:
    void fileSelectSlot();
    void dirSelectSlot();
    void convertToSlot();

private slots:
    void onHeaderContextMenu(const QPoint& pos);
    void onTableContextMenu(const QPoint& pos);
    void onSetUniformOutEncode();
    void onOpenFileLocation();
    void onOpenWithNotepad();
    void onRemoveSelected();

private:
    void addToFileModel(QStringList filenames);
    void initFileTable();
    void scanFilesRecursively(QDir dir, const QStringList& extensions, QStringList& fileList);

private:
    QTableView*         ui_fileTable;
    QStandardItemModel* ui_fileModel;
    QPushButton*        ui_fileSelect;
    QPushButton*        ui_dirSelect;
    QPushButton*        ui_ConvertTo;
    QPushButton*        ui_Exit;
    QMenu*              m_contextMenu;
};

#endif   // WIDGET_H
