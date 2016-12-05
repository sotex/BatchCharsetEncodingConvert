#ifndef COMBOXDELEGATE_HPP
#define COMBOXDELEGATE_HPP

#include <QStyledItemDelegate>

class ComboxDelegate:public QStyledItemDelegate{
public:
    // 创建你编辑时候的控件
    // 返回一个组件。该组件会被作为用户编辑数据时所使用的编辑器，从模型中接受数据，返回用户修改的数据。
    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    // 编辑的时候设置数据到上面创建的editor中。
    // 提供上述组件在显示时所需要的默认值。
    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    // 编辑完成，保存数据到data中
    // 确保上述组件作为编辑器时能够完整地显示出来
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;

    // 设置编辑控件的位置和大小、样式等
    // 返回给模型用户修改过的数据
    void updateEditorGeometry(QWidget * editor,
                              const QStyleOptionViewItem & option,
                              const QModelIndex & index );
};

#endif // COMBOXDELEGATE_HPP
