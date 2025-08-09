#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QTableView>
#include <QSet>

class FileSystemView : public QTableView
{
    Q_OBJECT

    enum class SelectInstructions
    {
        do_nothing,
        do_select,
        do_unselect
    };

    QSet<QModelIndex>   selected;
    SelectInstructions  selectInstruction = SelectInstructions::do_nothing;

public:
    FileSystemView(QWidget *parent = nullptr);
    const QSet<QModelIndex> &getSelected() const;
    void clearSelected();

private:
    void selectItem(const QModelIndex &index);
    void selectItemRow(const QModelIndex &index);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void slot_selectItem();
};

#endif // FILESYSTEMVIEW_H
