%global pkg_name ks-scmc-gui
%global pkg_ver 1.1.1
%global pkg_rel 8

%global _is_grpc_plugins 1
%global _is_generate_to_run 0

%if (0%{kylin_major_version} == 3 && 0%{kylin_minor_version} == 4 && 0%{kylin_sub_minor_version} >= 4) || 
    (0%{kylin_major_version} == 3 && 0%{kylin_minor_version} >= 5) || 
    (0%{kylin_major_version} == 3 && 0%{kylin_minor_version} == 3 && 0%{kylin_sub_minor_version} == 6)
	%global enable_set_palette 1
%endif

Name:           %{pkg_name}
Version:        %{pkg_ver}
Release:        %{pkg_rel}
Summary:        KylinSec security container magic cube gui

License:        No license
URL:            http://gitlab.kylinsec.com.cn/ks-scmc-gui.git
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  libarchive
BuildRequires:  protobuf-devel
BuildRequires:  qt5-linguist
BuildRequires:  qt5-qtcharts
BuildRequires:  qt5-qtcharts-devel
BuildRequires:  qt5-qtbase-devel
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  gcc-c++
BuildRequires:  gcc
BuildRequires:  xsd
BuildRequires:  grpc-devel
BuildRequires:  ks-license-devel >= 2.7
BuildRequires:  qrencode-devel
BuildRequires:  c-ares-devel
BuildRequires:  libnotify-devel
BuildRequires:  libarchive-devel
BuildRequires:  pkgconfig(klog-qt5)
%if "%{kylin_version}" == "3.3"
BuildRequires:  pkgconfig(kiranwidgets-qt5) >= 2.1.1
%else
BuildRequires:  kiranwidgets-qt5-devel == 2.1.1
%endif

%if 0%{?enable_set_palette}
BuildRequires:  kiran-qt5-integration-devel
%endif

%if 0%{?_is_grpc_plugins}
BuildRequires:  grpc-plugins
%endif

BuildRequires:  cryptopp-devel

%if "%{kylin_version}" == "3.3"
%global _is_generate_to_run 1
BuildRequires: qt5-qtsvg
BuildRequires: fcitx-qt5
%endif

Requires:       ks-license-client

%if %{_is_generate_to_run} == 0
%if "%{kylin_version}" == "3.3"
Requires:       kiran-widgets-qt5 >= 2.1.1
%else
Requires:       pkgconfig(kiranwidgets-qt5) >= 2.1.1
%endif
Requires:       kiran-log-qt5
Requires:       qt5-qtbase
Requires:       qt5-qtsvg
Requires:       protobuf
Requires:       libnotify
Requires:       qt5-qtcharts
Requires:       c-ares
Requires:       fcitx-qt5
Requires:       libarchive

%if 0%{?enable_set_palette}
Requires:	kiran-qt5-integration >= 2.4
%endif
%endif

%description
KylinSec security container magic cube gui

%prep
%autosetup -p1

%build
mkdir build; cd build
%if %{_is_generate_to_run}
    %cmake ..
%else
    %cmake .. -DGENERATE_TO_RUN:BOOL=OFF \
%endif

make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
cd build
%make_install
cd -
%if %{_is_generate_to_run}
deps_dir=%{buildroot}/usr/local/ks-scmc-gui
extract_file=./extract-libs/extract-libs.py
python $extract_file $deps_dir --need_qt_plugins
%endif

%post
echo %{version}-%{release} > %{_datadir}/ks-scmc-gui/ks-scmc-gui.version
chmod 0744 %{_datadir}/ks-scmc-gui/ks-scmc-gui.version > /dev/null || :

gtk-update-icon-cache -f /usr/share/icons/hicolor/
xdg-desktop-icon install --novendor %{_datadir}/applications/ks-scmc-gui.desktop

%postun
xdg-desktop-icon uninstall --novendor %{_datadir}/applications/ks-scmc-gui.desktop
killall ks-scmc-gui > /dev/null 2>&1 || true

%files
%{_datadir}/ks-scmc-gui/*
%{_datadir}/applications/ks-scmc-gui.desktop
%{_bindir}/ks-scmc-gui
%if %{_is_generate_to_run}
	%{_libexecdir}/ks-scmc-gui
    /usr/local/ks-scmc-gui/*
%endif
%{_datadir}/icons/hicolor/*/apps/*.*

%changelog
* Mon May 06 2024 zhenggongping <zhenggongping@kylinos.com.cn> -1.1.1-7
- KYOS-F: Remove sudo from the spec file. (#36753)

* Thu Aug 17 2023 yuanxing <yuanxing@kylinos.com.cn> -1.1.1-3
- KYOS-F: fix bugs of ks-scmc-gui-1.1

* Thu Aug 17 2023 yuanxing <yuanxing@kylinos.com.cn> -1.1.1-2
- KYOS-F: add build to run option and set theme don't change with system kiran theme

* Wed Feb 22 2023 yuanxing <yuanxing@kylinos.com.cn> -1.1.1-1
- KYOS-F: add ScatterSeries in chart(#65359)
- KYOS-F: improve image info worker and don't popup message dialog when user cancel image tranmission(#65466)
- KYOS-F: adjust quick action item width to show compelte when window in small(#65291)
- KYOS-F: update page num when user read last page info(65420)
- KYOS-F: don't set network ip when user generate template from container(#65335)
- KYOS-F: add help manual function(#65591)
- KYOS-F: update container of it's node id when operate container (#65288)
- KYOS-F: show button in mainwindow according to user authority(#65515)
- KYOS-F: use sys::dashboard to get information in outline (#65287)
- KYOS-F: add dialog of input reason when user reject image(#65243)
- KYOS-F: exit client when session expired second(#65528)
- KYOS-F: update error prompt of cpu and memory in container setting(##65519 #65523)
- KYOS-F: fix the error of check status of network access in container setting(#65531)
- KYOS-F: don't popup confirm dialog when user cancle image transmission(#65469) 
- KYOS-F: limit the lenth of virt network card name to 200 when create virt network (#65280)
- KYOS-F: fix the error of operate result in audit log(#65276)
- KYOS-F: update some translate(#65273)

* Fri Jan 06 2023 yuanxing <yuanxing@kylinos.com.cn> -1.1.0-1
- KYOS-F: generate image from container backup (#61181)
- KYOS-F: export container template from container (#61179)
- KYOS-F: add global security switch (#61180)
- KYOS-F: add configuration of network card of node (#61183)
- KYOS-F: improve the session expire prompt (#58484)
- KYOS-F: remove mac config in container setting (#61184)
- KYOS-F: set node warning threshold (#61186)
- KYOS-F: reclassify tab menu (#61169)

* Wed Aug 10 2022 yuanxing <yuanxing@kylinos.com.cn> -1.0.7-1
- KYOS-F: show monitor data when user click quick operation in container list page,clear point of charts when user change container and show 0 if value <0 when hover in QSplineSeries. (#58494)
- KYOS-F: add check and improve tips when image download if image is broken in database. (#59395)
- KYOS-F: Optimize the audit log, and fix the inconsistency of the number to be audited in the toolbar. (#59536,#57412)

* Tue Jul 12 2022 chendingjian <chendingjian@kylinos.com.cn> -1.0.6-1
- KYOS-F: add deadline of request , popup reaponse time out tip when don't get response ,and set loading in every page.(#57436)
- KYOS-F: set search col of table.(#57412)
- KYOS-F: Optimize paging input box.(#57412)
- KYOS-F: fix the data setting of host path and container path.(#57384)
- KYOS-F: change the node edit window title.(#56642)

* Fri Jun 24 2022 chendingjian <chendingjian@kylinos.com.cn> -1.0.5-1
- KYOS-F: Fix some bugs.

* Wed Jun 08 2022 chendingjian <chendingjian@kylinos.com.cn> -1.0.4-1
- KYOS-F: Fix some bugs(#55961,#55960,#55959,#55958,#55882,#55957,#55956,#55955,#55954,#55827,#55902,#55776,#55824,#55766,#55764,#55952,#55953,#55828,#55903,#55825,#55826,#55887,#55876,#55830,#55817,#55626,#55631,#55796,#55742,#55743,#55741,#55740,#55790).

* Fri May 20 2022 chendingjian <chendingjian@kylinos.com.cn> -1.0.3-1
- KYOS-F: Fix some bugs.

* Wed Apr 27 2022 chendingjian <chendingjian@kylinos.com.cn> -1.0.2-1
- KYOS-F: Fix the bug of abnormal jump of overview page.
- KYOS-F: Add audit list.

* Tue Apr 26 2022 yuanxing <yuanxing@kylinos.com.cn> -1.0.1-1
- KYOS-F:fix some bug in monitor and image, improve ui

* Thu Apr 21 2022 chendingjian <chendingjian@kylinos.com.cn> -1.0.0-2
- rebuild for KY3.4-4

* Wed Apr 20 2022 chendingjian <chendingjian@kylinos.com.cn> -1.0.0-1
- KYOS-F: First commit.


