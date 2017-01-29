%global cmake_build_dir build-cmake
%global app_name DNSCryptClient

Name:          dnscrypt-proxy-gui
Version:       1.5.7
Release:       1%{?dist}
Summary:       GUI wrapper for dnscrypt-proxy
License:       GPLv2+
Source0:       https://github.com/F1ash/%{name}/archive/%{version}.tar.gz
URL:           https://github.com/F1ash/%{name}

Requires:      qt5-qtbase
Requires:      kf5-kauth
Requires:      kf5-knotifications
Requires:      systemd
Requires:      polkit
Requires:      dnscrypt-proxy
Requires:      hicolor-icon-theme

BuildRequires: gcc-c++
BuildRequires: cmake
BuildRequires: glibc-headers
BuildRequires: desktop-file-utils
BuildRequires: qt5-qtbase-devel
BuildRequires: kf5-kauth-devel
BuildRequires: kf5-knotifications-devel
BuildRequires: extra-cmake-modules
%{?systemd_requires}
BuildRequires: systemd

%description
The Qt/KF5 GUI wrapper over dnscrypt-proxy
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

%post
/bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :
%systemd_post %{app_name}@.service

%preun
%systemd_preun %{app_name}@.service

%postun
if [ $1 -eq 0 ] ; then
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
fi
%systemd_postun %{app_name}@.service

%posttrans
/usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :

%files
%license LICENSE
%doc README.md
%{_bindir}/%{app_name}
%{_libexecdir}/kf5/kauth/dnscrypt_client_helper
%{_datadir}/applications/%{app_name}.desktop
%{_datadir}/dbus-1/system-services/pro.russianfedora.dnscryptclient.service
%{_datadir}/polkit-1/actions/pro.russianfedora.dnscryptclient.policy
%{_sysconfdir}/dbus-1/system.d/pro.russianfedora.dnscryptclient.conf
%{_datadir}/knotifications5/%{app_name}.notifyrc
%{_unitdir}/%{app_name}@.service
%{_datadir}/icons/hicolor/64x64/apps/%{app_name}.png

%changelog
* Sun Jan 29 2017 Fl@sh <kaperang07@gmail.com> - 1.5.7-1
- version updated;

* Tue Jan 10 2017 Fl@sh <kaperang07@gmail.com> - 1.3.7-1
- version updated;

* Fri Dec 16 2016 Fl@sh <kaperang07@gmail.com> - 1.2.3-4
- removed dbus-1 R;
- release updated;

* Wed Dec  7 2016 Fl@sh <kaperang07@gmail.com> - 1.2.3-3
- returned gcc-c++ BR;
- release updated;

* Wed Dec  7 2016 Fl@sh <kaperang07@gmail.com> - 1.2.3-2
- removed gcc-c++ BR, fixed dbus-1 R;
- added scriptlets for update Icon_Cache;
- added %%license in %%files;
- release updated;

* Wed Dec  7 2016 Fl@sh <kaperang07@gmail.com> - 1.2.3-1
- enhanced Summary and %%description;
- removed useless socket unit from scriplets and %%files;
- version updated;

* Mon Nov 28 2016 Fl@sh <kaperang07@gmail.com> - 1.2.2-4
- added cmake, gcc-c++ BR;
- added systemd scriptlets;
- release updated;

* Sun Nov 27 2016 Fl@sh <kaperang07@gmail.com> - 1.2.2-3
- changed package name to comply with the NamingGuidelines;
- release updated;

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
