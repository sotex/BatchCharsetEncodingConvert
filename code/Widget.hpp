#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QTableView;
class QStandardItemModel;
class QPushButton;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = 0);
    ~Widget();

public slots:
    void fileSelectSlot();
    void dirSelectSlot();
    void convertToSlot();

private:
    void addToFileModel(QStringList filenames);
    void initFileTable();

private:
    QTableView*         ui_fileTable;
    QStandardItemModel* ui_fileModel;
    QPushButton*        ui_fileSelect;
    QPushButton*        ui_dirSelect;
    QPushButton*        ui_ConvertTo;
    QPushButton*        ui_Exit;
};

#endif   // WIDGET_H
