%global cmake_build_dir build-cmake

Name:          DNSCryptClient
Version:       1.0
Release:       1%{?dist}
Summary:       Qt/KF5 GUI wrapped over dnscrypt-proxy
License:       GPLv2+
Source0:       https://github.com/F1ash/%{name}/archive/%{version}.tar.gz
URL:           https://github.com/F1ash/%{name}

Requires:      qt5-qtbase
Requires:      kf5-kauth
Requires:      kf5-knotifications
Requires:      dbus
Requires:      systemd
Requires:      polkit
Requires:      dnscrypt-proxy
Requires:      hicolor-icon-theme

BuildRequires: desktop-file-utils
BuildRequires: qt5-qtbase-devel
BuildRequires: kf5-kauth-devel
BuildRequires: kf5-knotifications-devel
BuildRequires: extra-cmake-modules
BuildRequires: systemd

%description
DNSCryptClient
The Qt GUI wrapped over dnscrypt-proxy
for encrypting all DNS traffic between the user and DNS resolvers,
preventing any spying, spoofing or man-in-the-middle attacks.

%prep
%setup -q

%build
mkdir %{cmake_build_dir}
pushd %{cmake_build_dir}
      %cmake ..
      %{make_build}
popd

%install
pushd %{cmake_build_dir}
      %{make_install}
popd

%check
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop

%files
%doc README.md
%{_bindir}/%{name}
%{_libexecdir}/kf5/kauth/dnscrypt_client_helper
%{_datadir}/applications/%{name}.desktop
%{_datadir}/dbus-1/system-services/pro.russianfedora.dnscrypt_client.service
%{_datadir}/polkit-1/actions/pro.russianfedora.dnscrypt_client.policy
%{_sysconfdir}/dbus-1/system.d/pro.russianfedora.dnscrypt_client.conf
%{_datadir}/knotifications5/%{name}.notifyrc
%{_unitdir}/%{name}.service
%{_datadir}/icons/hicolor/64x64/apps/%{name}.png

%changelog
* Fri Nov 11 2016 Fl@sh <kaperang07@gmail.com> - 1.0-1
- Initial build
