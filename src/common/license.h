/**
 * @file          /ks-scmc-gui/src/common/license.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef LICENSE_H
#define LICENSE_H

#include <QString>

struct License
{
    QString activation_code;
    QString machine_code;
    int activation_status;
    int activation_time;
    int activation_type;
    int customer_info;
    uint expired_time;
    int first_boot_time;
    int install_type;
    int service_status;
};


#endif // LICENSE_H
