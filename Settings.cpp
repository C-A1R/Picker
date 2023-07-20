#include "Settings.h"

Settings *Settings::instance()
{
    static Settings sett;
    return &sett;
}

void Settings::setValue(QAnyStringView key, const QVariant &value)
{
    settings->setValue(key, value);
}

QVariant Settings::value(QAnyStringView key, const QVariant &defaultValue) const
{
    return settings->value(key, defaultValue);
}

QVariant Settings::value(QAnyStringView key) const
{
    return settings->value(key);
}
