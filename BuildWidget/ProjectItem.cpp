#include "ProjectItem.h"


ProjectItem::ProjectItem(const uint64_t id, const QString &path, ProjectItem *parent)
    : id{id}
    , m_path{path}
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

ProjectItem *ProjectItem::parentItem()
{
    return m_parentItem;
}

uint64_t ProjectItem::getId() const
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
              [order](const std::unique_ptr<ProjectItem> &a, const std::unique_ptr<ProjectItem> &b) -> bool
              {
                  return (order == Qt::AscendingOrder) ? a->getOrderIndex() < b->getOrderIndex()
                                                       : a->getOrderIndex() > b->getOrderIndex();
              });
}

