#include "ComboxDelegate.hpp"

#include <QComboBox>

QWidget *ComboxDelegate::createEditor(QWidget *parent,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{

    QComboBox* ui_encoding = new QComboBox(parent);
    // International (Unicode)
    ui_encoding->addItem("UTF-8");
    ui_encoding->addItem("UTF-16BE");
    ui_encoding->addItem("UTF-16LE");
    ui_encoding->addItem("UTF-32BE");
    ui_encoding->addItem("UTF-32LE");
    // Chinese
    ui_encoding->addItem("ISO-2022-CN");
    ui_encoding->addItem("BIG5");
    ui_encoding->addItem("EUC-TW");
    ui_encoding->addItem("GB18030");
    ui_encoding->addItem("HZ-GB-2312");
    // English
    ui_encoding->addItem("ASCII");
    // Esperanto
    ui_encoding->addItem("ISO-8859-3");
    // Japanese
    ui_encoding->addItem("ISO-2022-JP");
    ui_encoding->addItem("SHIFT_JIS");
    ui_encoding->addItem("EUC-JP");
    // Korean
    ui_encoding->addItem("ISO-2022-KR");
    ui_encoding->addItem("EUC-KR");
    // Others
    ui_encoding->addItem("WINDOWS-1252");

    return ui_encoding;
}

void ComboxDelegate::setEditorData(QWidget *editor,
                                   const QModelIndex &index) const
{
    const QString encoding = index.model()->data(index,Qt::EditRole).toString();

    QComboBox* ui_encoding = dynamic_cast<QComboBox*>(editor);
    if(ui_encoding->findText(encoding) < 0){
        ui_encoding->addItem(encoding);
    }
    ui_encoding->setCurrentText(encoding);
}

void ComboxDelegate::setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    QComboBox *ui_encoding = dynamic_cast<QComboBox*>(editor);
    model->setData(index, ui_encoding->currentText(), Qt::EditRole);
}

void ComboxDelegate::updateEditorGeometry(QWidget *editor,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index)
{
    editor->setGeometry(option.rect);
}
