
1 config repo

# Kylin-Base.repo
#

[base]
name=KylinSec OS-$releasever - Base
gpgcheck=0
baseurl=http://192.168.120.17/kojifiles/repos/KY3.3-6-PG-ks-scmc-1.1-build/latest/x86_64/
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-kylin-release

[epel]
name=KylinSec OS-$releasever - Epel
gpgcheck=0
baseurl=http://192.168.120.17/kojifiles/repos/KY3.3-EPEL-build/latest/x86_64/
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-kylin-release

2 change /etc/yum.conf make keepcache=1

3 get depend packages
yum install install cryptopp gflags grpc kiran-widgets-qt5 ks-scmc-gui protobuf protobuf-compiler qt5-qtcharts qt5-qtdeclarative qt5-qtxmlpatterns re2 ks-scmc-gui

4 build run 
