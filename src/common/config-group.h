/**
 * @file          /ks-scmc-gui/src/common/config-group.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

enum HeaderInfoTable
{
    FIRST_COLUMN_HEADER = 0,
    SECOND_COLUMN_HEADER,
    THIRD_COLUMN_HEADER,
    OPERATE_HEADER
};

class ModelItem : public QObject
{
    Q_OBJECT
public:
    ModelItem(QObject *parent = nullptr);

public:
    QString m_firstColVal{};
    QString m_secondColVal{};
    int m_thirdColVal;
};

#endif  // CONFIG_H
