Name:           thedesk
Version:        beta4
Release:        12%{?dist}
Summary:        Desktop Environment built on Qt

License:        GPLv3+
URL:            https://github.com/vicr123/thedesk
Source0:        https://github.com/vicr123/thedesk/archive/%{version}.tar.gz


%if 0%{?fedora} >= 33
BuildRequires:  make qt5-qtbase-devel the-libs-devel libtdesktopenvironment-devel libX11-devel libXi-devel xorg-x11-drv-libinput-devel kf5-networkmanager-qt-devel kf5-modemmanager-qt-devel pulseaudio-qt-devel polkit-qt5-devel libtwebservices-devel qt5-qtmultimedia-devel qt5-qtquickcontrols2-devel qt5-qtx11extras-devel qt5-qtlocation-devel qt5-qtbase-private-devel qt5-qtsvg-devel qt5-linguist qrencode-devel kf5-bluez-qt-devel
Requires:       qt5-qtbase kwin the-libs libtdesktopenvironment libX11 libXi kf5-networkmanager-qt kf5-modemmanager-qt pulseaudio-qt accountsservice contemporary-widgets contemporary-icons ttf-contemporary polkit-qt5 libtwebservices qt5-qtmultimedia qt5-qtquickcontrols2 qt5-qtx11extras qt5-qtlocation qt5-qtsvg libthedesk qrencode kf5-bluez-qt thedesk-xdg-utils
%endif

%define debug_package %{nil}
%define _unpackaged_files_terminate_build 0

%description
Desktop Environment built on Qt

%package -n libthedesk
Summary:        Libraries for %{name} plugins
Conflicts:      libthedesk
Provides:       libthedesk

%description -n libthedesk
Libraries for theDesk plugins

%package -n libthedesk-devel
Summary:        Development files for libthedesk
Requires:       libthedesk%{?_isa} = %{version}-%{release}
Conflicts:      libthedesk-devel
Provides:       libthedesk-devel


%description -n libthedesk-devel
The libthedesk-devel package contains libraries and header files for
developing applications that use libthedesk.

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
%{_libdir}/td-polkitagent
%{_libdir}/thedesk/plugins/*.so
%{_libdir}/qt5/plugins/platformthemes/*.so
%{_sysconfdir}/*

%files -n libthedesk
%{_libdir}/libthedesk.so*

%files -n libthedesk-devel
%{_includedir}/*

%changelog
* Sun May  3 2020 Victor Tran
-
