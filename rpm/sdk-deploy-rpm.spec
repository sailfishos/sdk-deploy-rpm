#define qtc_qmake

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Name:       sdk-deploy-rpm
Summary:    Tool to install rpm packages.
Version:    0.0.1
Release:    1
Group:      System/Other
License:    BSD
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(packagekit-qt5)

%description
Tool to make rpm installations in SailfishOS with new Packagekit.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

%qmake5_install

%files
%defattr(-,root,root,-)
%{_bindir}/sdk-deploy-rpm
