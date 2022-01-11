Name:       sdk-deploy-rpm
Summary:    Tool to install RPM packages.
Version:    1.0
Release:    1
License:    BSD
Source0:    %{name}-%{version}.tar.bz2
URL:        https://github.com/sailfishos/sdk-deploy-rpm
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)

%description
Tool to install local RPM packages in Sailfish OS.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5

make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

%files
%defattr(-,root,root,-)
%license LICENSE.BSD
%{_bindir}/sdk-deploy-rpm
