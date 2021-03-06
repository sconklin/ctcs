Summary: VA Cerberus Test Control System -- DO NOT INSTALL ON LIVE SYSTEMS
Name: ctcs
Version: 1.3.0pre4
Release: 1
Copyright: GPL
Group: Applications/System
Source0: %{name}-%{version}.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}

%description
This is the VA Linux burnin test suite, known to mere mortals
as Cerberus.  It's used to make sure that new systems are ready to
go out and face the perils of the cold, hard world.  It's made up
of a suite of programs that literally pound the system 

*NOTE* It's very easy to destroy your system with this software.

DO NOT INSTALL AND RUN ON A PRODUCTION SYSTEM.  Please, for
the love of Pete, I'm dead serious about this folks.  The
tests are meant for hardware with nothing on it yet... you
will lose data.  Not might.  Will.

Please be sure to read the README's that in the top level of
the install directory.  Please also visit
http://sourceforge.net/projects/va-ctcs for more information.

%prep
%setup -qn ctcs-%{version}

%build
make

%install
make install
mkdir -p $RPM_BUILD_ROOT/root/ctcs
rm -rf $RPM_BUILD_ROOT/root/ctcs/*
cp -Rap . $RPM_BUILD_ROOT/root/ctcs

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/root/ctcs/*

%doc CHANGELOG FAQ README.FIRST COPYING README README.TCF runin/README.runtest runin/README.tests

%changelog
* Thu Aug 2 2001 Jason Collins <jcollins@valinux.com>
- Fixed the RPM dependency generation so it is dynamic.  This means that you
can no longer rpm -ta the tarball, but it should generate correct dependencies
if you make the specfile yourself on SuSE, RH, Mandrake, etc, even if package
names are different.
* Mon Sep 25 2000 Jason Collins <jcollins@valinux.com>
- Ready for version 1.2.12.
* Mon Sep 04 2000 Jason Collins <jcollins@valinux.com>
- Some changes to put this into the mainline distribution.  rackspace
  may still want to keep the README.RACK file they wrote, but I considered
  it to be too specific to them to include here.
- I also deleted the post install script as this seems to be something
  specific to rackspace's needs.
- Took out the file list generation, /root/ctcs/* is good enough.
- Added two READMEs to the doc list.  The doc list works now -- it didn't with the file list.
  (?)
* Fri May 26 2000 Kelley Spoon <kspoon@rackspace.com>
- built RPM for rackspace to use in our burnin distro
