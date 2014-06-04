%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Name:       sdk-deploy-rpm
Summary:    Tool to install RPM packages.
Version:    1.0
Release:    1
Group:      System/Other
License:    BSD
Source0:    %{name}-%{version}.tar.bz2
URL:        https://github.com/sailfish-sdk/sdk-deploy-rpm
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(packagekit-qt5)

%description
Tool to install local RPM packages in Sailfish OS.

%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5

make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

%files
%defattr(-,root,root,-)
%{_bindir}/sdk-deploy-rpm
