#ifndef PROJECTITEM_H
#define PROJECTITEM_H

#include <QVariant>
#include <QDir>
#include <QFileIconProvider>

#include <vector>
#include <memory>

/**
 * @brief The ProjectItem class
 * Элемент дерева проекта
 */
class ProjectItem
{
    const qulonglong    id{0};
    const QDir          m_path;
    const QFileInfo     m_info;
    QFileIconProvider   m_iconProvider;
    double              m_orderIndex{1.0};

    ProjectItem                                 *m_parentItem;
    std::vector<std::unique_ptr<ProjectItem>>   m_childItems;

public:
    enum Roles
    {
        ID = Qt::UserRole,
        STATUS
    };

    explicit ProjectItem(const qulonglong id, const QString &path, ProjectItem *parentItem = nullptr);

    void appendChild(std::unique_ptr<ProjectItem> &&child);

    ProjectItem *child(const int row);
    int childCount() const;
    int row() const;
    ProjectItem *parentItem() const;

    qulonglong getId() const;
    const QDir &getPath() const;
    double getOrderIndex() const;

    void setOrderIndex(const double index);

    bool exists() const;
    bool isDir() const;
    void sortChildren(const Qt::SortOrder order = Qt::AscendingOrder);
};

#endif // PROJECTITEM_H


