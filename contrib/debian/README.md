
Debian
====================
This directory contains files used to package eunod/euno-qt
for Debian-based Linux systems. If you compile eunod/euno-qt yourself, there are some useful files here.

## euno: URI support ##


euno-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install euno-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your euno-qt binary to `/usr/bin`
and the `../../share/pixmaps/euno128.png` to `/usr/share/pixmaps`

euno-qt.protocol (KDE)

