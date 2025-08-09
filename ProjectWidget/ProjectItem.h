#ifndef PROJECTITEM_H
#define PROJECTITEM_H

#include "ProjectEnums.h"

#include <QVariant>
#include <QDir>

#include <memory>

/**
 * @brief The ProjectItem class
 * Элемент дерева проекта
 */
class ProjectItem
{
    const qulonglong    m_id{0};
    const QDir          m_path;
    const QFileInfo     m_info;
    double              m_orderIndex{0.0};
    Project::ExStatus   m_exStatus{Project::ExStatus::DEFAULT};

    std::weak_ptr<ProjectItem>              m_parentItem;
    QList<std::shared_ptr<ProjectItem>>     m_childItems;

public:
    explicit ProjectItem(const qulonglong id, const QString &path, std::shared_ptr<ProjectItem> parentItem = nullptr);

    void appendChild(const std::shared_ptr<ProjectItem> &child);
    void removeChild(const qulonglong id);

    std::shared_ptr<ProjectItem> child(const int row) const;
    int childCount() const;
    int row() const;
    std::shared_ptr<ProjectItem> parentItem() const;

    qulonglong id() const;
    const QDir &path() const;
    double orderIndex() const;
    Project::ExStatus exStatus() const;

    void setOrderIndex(const double index);
    void setParent(const std::shared_ptr<ProjectItem> &parent);
    void setExStatus(const Project::ExStatus newExStatus);

    bool exists() const;
    bool isDir() const;
    QString lastModified() const;
    void sortChildren(const Qt::SortOrder order = Qt::AscendingOrder);
};

#endif // PROJECTITEM_H


