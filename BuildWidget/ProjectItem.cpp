#include "ProjectItem.h"


ProjectItem::ProjectItem(const qulonglong id, const QString &path, std::shared_ptr<ProjectItem> parent)
    : id{id}
    , m_path{path}
    , m_info{m_path.absolutePath()}
    , m_parentItem{parent}
{
}

void ProjectItem::appendChild(const std::shared_ptr<ProjectItem> &child)
{
    m_childItems.emplace_back(child);
}

std::shared_ptr<ProjectItem> ProjectItem::child(const int row) const
{
    return row >= 0 && row < childCount() ? m_childItems.at(row) : nullptr;
}

int ProjectItem::childCount() const
{
    return static_cast<int>(m_childItems.size());
}

int ProjectItem::row() const
{
    auto parent = m_parentItem.lock();
    if (!parent)
        return 0;

    const auto it = std::find_if(parent->m_childItems.cbegin(), parent->m_childItems.cend(),
                                 [this](const std::shared_ptr<ProjectItem> &treeItem) {
                                     return treeItem.get() == this;
                                 });

    if (it != parent->m_childItems.cend())
        return std::distance(parent->m_childItems.cbegin(), it);

    Q_ASSERT(false); // should not happen
    return -1;
}

std::shared_ptr<ProjectItem> ProjectItem::parentItem() const
{
    return m_parentItem.lock();
}

qulonglong ProjectItem::getId() const
{
    return id;
}

const QDir &ProjectItem::getPath() const
{
    return m_path;
}

double ProjectItem::getOrderIndex() const
{
    return m_orderIndex;
}

void ProjectItem::setOrderIndex(const double index)
{
    m_orderIndex = index;
}

bool ProjectItem::exists() const
{
    return m_info.exists();
}

bool ProjectItem::isDir() const
{
    return m_info.isDir();
}

void ProjectItem::sortChildren(const Qt::SortOrder order)
{
    std::sort(m_childItems.begin(), m_childItems.end(),
              [order](const std::shared_ptr<ProjectItem> &a, const std::shared_ptr<ProjectItem> &b) -> bool
              {
                  return (order == Qt::AscendingOrder) ? a->getOrderIndex() < b->getOrderIndex()
                                                       : a->getOrderIndex() > b->getOrderIndex();
              });
}

