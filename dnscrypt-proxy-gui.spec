%global cmake_build_dir build-cmake
%global app_name DNSCryptClient

Name:          dnscrypt-proxy-gui
Version:       1.11.15
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
%systemd_post %{app_name}_test@.service

%preun
%systemd_preun %{app_name}@.service
%systemd_preun %{app_name}_test@.service

%postun
if [ $1 -eq 0 ] ; then
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
fi
%systemd_postun %{app_name}@.service
%systemd_postun %{app_name}_test@.service

%posttrans
/usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :

%files
%license LICENSE
%doc README.md
%{_bindir}/%{app_name}
%{_datadir}/applications/%{app_name}.desktop
%{_libexecdir}/kf5/kauth/dnscrypt_client_helper
%{_datadir}/dbus-1/system-services/pro.russianfedora.dnscryptclient.service
%{_datadir}/polkit-1/actions/pro.russianfedora.dnscryptclient.policy
%{_sysconfdir}/dbus-1/system.d/pro.russianfedora.dnscryptclient.conf
%{_libexecdir}/kf5/kauth/dnscrypt_client_test_helper
%{_datadir}/dbus-1/system-services/pro.russianfedora.dnscryptclienttest.service
%{_datadir}/polkit-1/actions/pro.russianfedora.dnscryptclienttest.policy
%{_sysconfdir}/dbus-1/system.d/pro.russianfedora.dnscryptclienttest.conf
%{_libexecdir}/kf5/kauth/dnscrypt_client_reload_helper
%{_datadir}/dbus-1/system-services/pro.russianfedora.dnscryptclientreload.service
%{_datadir}/polkit-1/actions/pro.russianfedora.dnscryptclientreload.policy
%{_sysconfdir}/dbus-1/system.d/pro.russianfedora.dnscryptclientreload.conf
%{_datadir}/knotifications5/%{app_name}.notifyrc
%{_unitdir}/%{app_name}@.service
%{_unitdir}/%{app_name}_test@.service
%{_datadir}/icons/hicolor/64x64/apps/%{app_name}.png

%changelog
* Mon Mar  5 2018 Fl@sh <kaperang07@gmail.com> - 1.11.15-1
- version updated;

* Wed Feb 21 2018 Fl@sh <kaperang07@gmail.com> - 1.11.14-1
- version updated;

* Mon Jan 15 2018 Fl@sh <kaperang07@gmail.com> - 1.11.11-1
- version updated;

* Wed Jun 28 2017 Fl@sh <kaperang07@gmail.com> - 1.11.10-1
- version updated;

* Tue Jun 27 2017 Fl@sh <kaperang07@gmail.com> - 1.10.10-1
- version updated;

* Thu Jun 22 2017 Fl@sh <kaperang07@gmail.com> - 1.10.9-1
- version updated;

* Sat Jun  3 2017 Fl@sh <kaperang07@gmail.com> - 1.10.8-1
- changed %%files for reload_helper;
- version updated;

* Mon May 22 2017 Fl@sh <kaperang07@gmail.com> - 1.9.8-1
- changed %%files for test_helper;
- version updated;

* Mon Mar 27 2017 Fl@sh <kaperang07@gmail.com> - 1.6.8-2
- release updated;

* Mon Mar 27 2017 Fl@sh <kaperang07@gmail.com> - 1.6.8-1
- version updated;
- changed %%post, %%preun, %%postun, %%files for new systemd unit;

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
