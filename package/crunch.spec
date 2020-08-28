Name: crunch
Version: 1.1.1
Release: 1%{?dist}
URL: https://github.com/DX-MON/crunch
Summary: A unit testing framework written for C and C++
License: LGPLv3+

Source0: https://github.com/DX-MON/crunch/releases/v%{version}/crunch-%{version}.tar.xz
Source1: https://github.com/DX-MON/crunch/releases/v%{version}/%{name}-%{version}.tar.xz.asc

%description
crunch your code. Crunch is a unit testing framework written for C and C++.

BuildRequires: meson
BuildRequires: ninja-build >= 1.8.2
BuildRequires: gnupg2
Requires: pkgconfig
Requires: gcc >= 7
Requires: gcc-c++ >= 7
Requires: glibc-devel

%prep
gpg2 --no-default-keyring --keyring ./gpg-keyring.gpg --import-options import-minimal \
	--keyserver keys.gnupg.net --recv-keys 5E30DFF73626CA96F83DDF5E0137BA904EBDB4CD
gpg2 --keyring ./gpg-keyring.gpg --verify %{SOURCE1} %{SOURCE0}
rm ./gpg-keyring.gpg
%setup -q

%build
meson build --prefix=%{_prefix} --libdir=%{_libdir} --debug -Dstrip=false
ninja -C build

%check
ninja -C build test

%install
DESTDIR=$RPM_BUILD_ROOT ninja -C build install
export QA_RPATHS=0x0001

%clean
rm -r build

%files
%license COPYING COPYING.LESSER
%{_bindir}/crunch
%{_bindir}/crunch++
%{_bindir}/crunchMake
%{_libdir}/libcrunch.so*
%{_libdir}/libcrunch++.so*
%{_libdir}/pkgconfig/crunch.pc
%{_libdir}/pkgconfig/crunch++.pc
%{_includedir}/crunch.h
%{_includedir}/crunch++.h
%{_mandir}/man1/crunch.1.gz
%{_mandir}/man1/crunch++.1.gz
%{_mandir}/man1/crunchMake.1.gz

%changelog
* Fri Aug 28 2020 Rachel Mant <dx-mon@users.sourceforge.net> - 1.1.1-1
- Solved the previous lack of support on windows for crunchMake's `--debug` flag
- Fixed the crunch assertion messages as the expected value and result were backwards
- Fixed bugs revealed by the args parser test suite alloc tests

* Mon Aug 10 2020 Rachel Mant <dx-mon@users.sourceforge.net> - 1.1.0-1
- Successfully fixed the windows build
- Improved the unit tests correctness
- Improved threadShim
- Fixed the Clang 5 build which 1.0.1 broke
- Fixed a security bug in the `formatString()` function in both crunch and crunch++
- Improved handling of -std in crunchMake so it works with both the MSVC and GCC forms of the argument

* Wed Jun 17 2020 Rachel Mant <dx-mon@users.sourceforge.net> - 1.0.1-1
- Given crunchMake support for -flto
- Fixed a number of clang-tidy warnings

* Wed May 13 2020 Rachel Mant <dx-mon@users.sourceforge.net> - 1.0.0-1
- Initial RPM packaging and release of crunch
