Name:           thedesk
Version:        beta2
Release:        1%{?dist}
Summary:        Desktop Environment built on Qt

License:        GPLv3+
URL:            https://github.com/vicr123/thedesk
Source0:        https://github.com/vicr123/thedesk/archive/%{version}.tar.gz


%if 0%{?fedora} == 32
BuildRequires:  make qt5-devel the-libs-devel libtdesktopenvironment-devel libX11-devel libXi-devel xorg-x11-drv-libinput-devel kf5-networkmanager-qt-devel kf5-modemmanager-qt-devel pulseaudio-qt-devel polkit-qt5-devel libtwebservices-devel qt5-qtmultimedia-devel qt5-qtquickcontrols2-devel qt5-qtx11extras-devel qt5-qtlocation-devel qt5-qtbase-private-devel
Requires:       qt5 kwin the-libs libtdesktopenvironment libX11 libXi kf5-networkmanager-qt kf5-modemmanager-qt pulseaudio-qt accountsservice contemporary-widgets contemporary-icons ttf-contemporary polkit-qt5 libtwebservices qt5-qtmultimedia qt5-qtquickcontrols2 qt5-qtx11extras qt5-qtlocation
%endif

%if 0%{?fedora} >= 33
BuildRequires:  make qt5-qtbase-devel the-libs-devel libtdesktopenvironment-devel libX11-devel libXi-devel xorg-x11-drv-libinput-devel kf5-networkmanager-qt-devel kf5-modemmanager-qt-devel pulseaudio-qt-devel polkit-qt5-devel libtwebservices-devel qt5-qtmultimedia-devel qt5-qtquickcontrols2-devel qt5-qtx11extras-devel qt5-qtlocation-devel qt5-qtbase-private-devel qt5-linguist
Requires:       qt5-qtbase kwin the-libs libtdesktopenvironment libX11 libXi kf5-networkmanager-qt kf5-modemmanager-qt pulseaudio-qt accountsservice contemporary-widgets contemporary-icons ttf-contemporary polkit-qt5 libtwebservices qt5-qtmultimedia qt5-qtquickcontrols2 qt5-qtx11extras qt5-qtlocation
%endif

%define debug_package %{nil}
%define _unpackaged_files_terminate_build 0

%description
Desktop Environment built on Qt

%prep
%setup

%build
mkdir build
cd build
qmake-qt5 ..
make

%install
cd build
rm -rf $RPM_BUILD_ROOT
make install INSTALL_ROOT=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%{_bindir}/*
%{_datarootdir}/*
%{_libdir}/*.so*
%{_libdir}/td-polkitagent
%{_libdir}/thedesk/plugins/*.so
%{_libdir}/qt5/plugins/platformthemes/*.so
%{_includedir}/*
%{_sysconfdir}/*

%changelog
* Sun May  3 2020 Victor Tran
- 
