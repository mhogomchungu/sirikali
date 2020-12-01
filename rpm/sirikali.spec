#
# Spec file for package sirikali
#
# Copyright © 2016 Francis Banyikwa <mhogomchungu@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

Name:           SiriKali
Version:        1.4.8
Release:        0
Summary:        SiriKali is a Qt/C++ GUI front end to fscrypt,encfs,cryfs,gocryptfs,ecryptfs and securefs.
License:        GPL-2.0+
Group:          Productivity/Security
Source:         %{name}-%{version}.tar.xz
Source100:      sirikali-rpmlint
URL:            http://mhogomchungu.github.io/sirikali

BuildRequires: cmake
BuildRequires: gcc-c++
BuildRequires: glibc-devel
BuildRequires: libgcrypt-devel
BuildRequires: libsecret-devel

%if 0%{?fedora}
BuildRequires: qt5-qtbase-devel
%else
#This package maybe named differently in your distribution.
#What is required here is a package or packages that provides development packages for Qt5Core,Qt5GUI and Qt5Network
#BuildRequires: libqt5-qtbase-devel
%endif

%description
SiriKali is a Qt/C++ GUI front end to encfs,cryfs,gocryptfs,ecryptfs and securefs.

%undefine _debugsource_packages

%prep
%setup -q

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DQT5=true -DNOKDESUPPORT=true -DNOSECRETSUPPORT=false -DCMAKE_BUILD_TYPE=RELEASE ..

%install
cd build
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf %{buildroot}
rm -rf $RPM_BUILD_DIR/sirikali

%files
%defattr(0755,root,root)
%{_bindir}/sirikali
%{_bindir}/sirikali.pkexec
%{_datadir}/applications/io.github.mhogomchungu.sirikali.desktop

%dir %{_datadir}/sirikali/translations
%dir %{_datadir}/sirikali

%defattr(0644,root,root)
%{_datadir}/icons/hicolor/48x48/apps/sirikali.png
%{_datadir}/icons/hicolor/256x256/apps/sirikali.png
%{_datadir}/pixmaps/sirikali.png
%{_datadir}/icons/sirikali.png
%{_datadir}/metainfo/sirikali.appdata.xml
%{_datadir}/sirikali/translations/en_US.qm
%{_datadir}/sirikali/translations/fr_FR.qm
%{_datadir}/sirikali/translations/es_MX.qm
%{_datadir}/sirikali/translations/de_DE.qm
%{_datadir}/sirikali/translations/ru_RU.qm

%{_datadir}/polkit-1/actions/org.sirikali.pkexec.policy
%{_datadir}/man/man1/sirikali.1.bz2
%{_datadir}/man/man1/sirikali.pkexec.1.bz2
%defattr(0644,root,root)

%changelog
