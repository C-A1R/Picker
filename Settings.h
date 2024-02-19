#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

//General
#define SETTINGS_FILESYSTEM_PATH "filesystem_path"
#define SETTINGS_BUILD_PATH "build_path"
#define SETTINGS_SAVE_OPTIONS "save_options"
#define SETTINGS_DEFENIT_PATH "defenit_path"
//Size
#define SETTINGS_MAXIMAZED "Size/maximized"
#define SETTINGS_WIDTH "Size/width"
#define SETTINGS_HEIGHT "Size/height"
#define SETTINGS_SPLIT_SIZES "Size/split_sizes"

class Settings
{
    const QString               settingsFilename = QStringLiteral("settings.ini");
    QScopedPointer<QSettings>   settings {new QSettings(settingsFilename, QSettings::IniFormat)};
    Settings() = default;
public:
    static Settings *instance();
    void setValue(QAnyStringView key, const QVariant &value);
    QVariant value(QAnyStringView key, const QVariant &defaultValue) const;
    QVariant value(QAnyStringView key) const;
};

#endif // SETTINGS_H
