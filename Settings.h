#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

//General
#define SETTINGS_FILE_EXP_PATH "file_explorer_path"
#define SETTINGS_PROJECT_PATH "project_path"
#define SETTINGS_SAVE_OPTIONS "save_options"
#define SETTINGS_DEFENIT_PATH "defenit_path"
#define SETTINGS_FILE_EXP_HIDDEN "file_explorer_hidden"
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
