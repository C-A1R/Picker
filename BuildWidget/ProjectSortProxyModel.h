#ifndef PROJECTSORTPROXYMODEL_H
#define PROJECTSORTPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QVector>

/**
 * @brief The ProjectSortProxyModel class
 * Прокси модель для сортировки элементов в ProjectModel
 */
class ProjectSortProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

    typedef QHash<QPersistentModelIndex, QVector<int>> IndexHashType;
    mutable IndexHashType   m_sortedRows; ///< Для каждого parent sourceIndex храним отсортированный список индексов
    int                     m_sortColumn = 0;
    Qt::SortOrder           m_sortOrder = Qt::AscendingOrder;

public:
    explicit ProjectSortProxyModel(QObject *parent = nullptr);

    // Переопределяем эти методы для сортировки
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(const int row, const int column, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;

    // Настройка сортировки
    void sort(const int column, const Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    int compareRows(const QModelIndex &a, const QModelIndex &b) const;
    void resort(const QModelIndex &parent = QModelIndex()) const;
};


// class ProjectProxyModel : public QSortFilterProxyModel
// {
//     Q_OBJECT

// public:
//     ProjectProxyModel(QObject *parent = nullptr);

//     bool hasChildren(const QModelIndex &parent) const override;

//     int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//     QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
//     QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

// protected:
//     bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
//     bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
//     // QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
//     // QVariant data(const QModelIndex &index, const int role) const override;

// signals:
// //     void signal_dropped(const quintptr, const QList<quintptr> &);
// //     void signal_added(const quintptr, const QString &);
//     // void signal_setChecked(const QModelIndexList &, const Qt::CheckState);
//     // void signal_expand(const QModelIndexList &);

// public slots:
// //     void slot_dropped(const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices);
// //     void slot_added(const QModelIndex &droppedIndex, const QString &fullPaths);
//     // void slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState);
//     // void slot_expand(const QModelIndexList &expanded);
// };


#endif // PROJECTSORTPROXYMODEL_H
