#include "ProjectItem.h"


ProjectItem::ProjectItem(const QString &path, ProjectItem *parent)
    : m_path{path}
    , m_info{m_path.absolutePath()}
    , m_parentItem{parent}
{
}

void ProjectItem::appendChild(std::unique_ptr<ProjectItem> &&child)
{
    m_childItems.emplace_back(std::move(child));
}

ProjectItem *ProjectItem::child(const int row)
{
    return row >= 0 && row < childCount() ? m_childItems.at(row).get() : nullptr;
}

int ProjectItem::childCount() const
{
    return static_cast<int>(m_childItems.size());
}

int ProjectItem::row() const
{
    if (m_parentItem == nullptr)
        return 0;

    const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(),
                                 [this](const std::unique_ptr<ProjectItem> &treeItem) {
                                     return treeItem.get() == this;
                                 });

    if (it != m_parentItem->m_childItems.cend())
        return std::distance(m_parentItem->m_childItems.cbegin(), it);

    Q_ASSERT(false); // should not happen
    return -1;
}

QVariant ProjectItem::data(const int column, const int role) const
{
    if (column < 0)
        return {};

    if (column == 0)
    {
        switch (role) {
        case Qt::DecorationRole:
        {
            if (m_info.isDir())
                return m_iconProvider.icon(QFileIconProvider::Folder);
            else
                return m_iconProvider.icon(QFileIconProvider::File);
            break;
        }
        case Qt::DisplayRole:
            return m_path.dirName();
        default:
            break;
        }
    }

    return {};
}

ProjectItem *ProjectItem::parentItem()
{
    return m_parentItem;
}

const QDir &ProjectItem::getPath() const
{
    return m_path;
}

const QFileInfo &ProjectItem::getInfo() const
{
    return m_info;
}
