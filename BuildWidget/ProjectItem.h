#ifndef PROJECTITEM_H
#define PROJECTITEM_H

#include <QVariant>
#include <QDir>
#include <QFileIconProvider>

#include <vector>
#include <memory>

class ProjectItem
{
    const QDir          m_path;
    const QFileInfo     m_info;
    QFileIconProvider   m_iconProvider;

    ProjectItem                                 *m_parentItem;
    std::vector<std::unique_ptr<ProjectItem>>   m_childItems;

public:
    explicit ProjectItem(const QString &path, ProjectItem *parentItem = nullptr);

    void appendChild(std::unique_ptr<ProjectItem> &&child);

    ProjectItem *child(const int row);
    int childCount() const;
    QVariant data(const int column, const int role = Qt::DisplayRole) const;
    int row() const;
    ProjectItem *parentItem();

    const QDir &getPath() const;
    const QFileInfo &getInfo() const;
};

#endif // PROJECTITEM_H


