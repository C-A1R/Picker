#ifndef FILESYSTEMLISTVIEW_H
#define FILESYSTEMLISTVIEW_H

#include <QListView>
#include <QSet>

class FileSystemListView : public QListView
{
    Q_OBJECT

    enum class SelectInstructions
    {
        do_nothing,
        do_select,
        do_unselect
    };

    QSet<qintptr> selected;
    SelectInstructions selectInstruction = SelectInstructions::do_nothing;

public:
    FileSystemListView(QWidget *parent = nullptr);
    const QSet<qintptr> &getSelected() const;

private:
    void selectItem(const QModelIndex &index);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void slot_selectItem();
};

#endif // FILESYSTEMLISTVIEW_H
