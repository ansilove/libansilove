Name:           libansilove
Version:        1.4.2
Release:        1%{?dist}
Summary:        Library for converting ANSI, ASCII, and other formats to PNG

License:        BSD-2-Clause
URL:            https://github.com/ansilove/libansilove
Source0:        %{url}/releases/download/%{version}/%{name}-%{version}.tar.gz

BuildRequires:  gd-devel
BuildRequires:  cmake
BuildRequires:  gcc

%description
Library for converting ANSI, ASCII, and other formats to PNG, allowing
to render ANSI and artscene-related file formats into PNG images, and
supporting ANSI (.ANS), PCBoard (.PCB), Binary (.BIN), Artworx (.ADF),
iCE Draw (.IDF), Tundra (.TND) and XBin (.XB) formats.

It creates size optimized 4-bit PNG files and supports SAUCE (Standard
Architecture for Universal Comment Extentions), 80x25 and 80x50 PC fonts
(including all the 14 MS-DOS charsets), Amiga fonts, and iCE colors.

%package devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.

%prep
%autosetup

%build
%cmake .
%cmake_build

%install
%cmake_install

%check

%files
%license LICENSE
%doc README.md
%{_libdir}/%{name}.so.1*

%files devel
%{_libdir}/%{name}.so
%{_libdir}/%{name}-static.a
%{_includedir}/ansilove.h
%{_mandir}/man3/%{name}.3*
%{_mandir}/man3/ansilove_*.3*
