%global cmake_build_dir build-cmake
%global app_name DNSCryptClient

Name:          dnscrypt-proxy-gui-qt
Version:       1.2.2
Release:       2%{?dist}
Summary:       The GUI wrapped over dnscrypt-proxy
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
The Qt/KF5 GUI wrapped over dnscrypt-proxy
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
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{app_name}.desktop

%files
%doc README.md
%{_bindir}/%{app_name}
%{_libexecdir}/kf5/kauth/dnscrypt_client_helper
%{_datadir}/applications/%{app_name}.desktop
%{_datadir}/dbus-1/system-services/pro.russianfedora.dnscryptclient.service
%{_datadir}/polkit-1/actions/pro.russianfedora.dnscryptclient.policy
%{_sysconfdir}/dbus-1/system.d/pro.russianfedora.dnscryptclient.conf
%{_datadir}/knotifications5/%{app_name}.notifyrc
%{_unitdir}/%{app_name}@.service
%{_unitdir}/%{app_name}.socket
%{_datadir}/icons/hicolor/64x64/apps/%{app_name}.png

%changelog
* Fri Nov 25 2016 Fl@sh <kaperang07@gmail.com> - 1.2.2-2
- changed package name to comply with the NamingGuidelines;
- release updated;

* Fri Nov 25 2016 Fl@sh <kaperang07@gmail.com> - 1.2.2-1
- version updated;

* Tue Nov 22 2016 Fl@sh <kaperang07@gmail.com> - 1.0.0-2
- enhanced Summary and %%description;
- release updated;

* Mon Nov 21 2016 Fl@sh <kaperang07@gmail.com> - 1.0.0-1
- Initial build
