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
    QString selectUnknownEncodingDialog(const QString& fileName, bool& applyToAll);
    bool convertFile(const QString& filePath, const QString& fromEncoding, const QString& toEncoding, bool createBackup);
    bool backupFile(const QString& filePath);

private:
    QTableView*         ui_fileTable;
    QStandardItemModel* ui_fileModel;
    QPushButton*        ui_fileSelect;
    QPushButton*        ui_dirSelect;
    QPushButton*        ui_ConvertTo;
    QPushButton*        ui_Exit;
    QMenu*              m_contextMenu;
    QString             m_unknownEncoding;  // 全局未知编码设置
    bool                m_applyToAllUnknown;  // 是否应用到全部未知编码文件
};

#endif   // WIDGET_H
