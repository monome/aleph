## beekeep OSX build

GTK is a little painful to setup. using some kind of build system is pretty much mandatory. i tried two, `jhbuild` (python-based, recommended by the GTK group) and `homebrew` (based on ruby scripts, and a more generally useful package manager.) both produced the same imprefect results (some broken pixbuf links); i recommend homebrew since it's less finicky and more generally useful as a package manager for other things.


### using `jhbuild`

in theory, you should be able to just follow the instructions here for gtk3:

https://wiki.gnome.org/action/show//Projects/GTK+/OSX/Building

in practice, i needed to tweak the steps a little on 10.7. YMMV.

- download and run: https://git.gnome.org/browse/gtk-osx/plain/gtk-osx-build-setup.sh

- add to your PATH (and .bash_profile_)

	`PATH=$PATH/.local/bin``
	
- alias jhbuild with the correct prefic for gtk binaries:

	`alias jhbuild="PATH=/Users/[username]/gtk/inst/bin:$PATH jhbuild"`

- run jhbuild modules in this rather specific order:
	`jhbuild build python`
	
	`jhbuild build libpng`
	
	`jhbuild build gobject-introspection`
	
	`jhbuild update meta-gtk-osx-bootstrap`
	
	`jhbuild update meta-gtk-osx-gtk3`
	
- finally, you must for some reason add some pkgconfig locations manually:
	`PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/opt/X11/lib/pkgconfig/`
	
	and also, possibly:
	
	`PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/Users/[username]/gtk/inst/lib/pkgconfig/`

### using `homebrew`

##### install homebrew

`ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`


##### install xquartz: http://xquartz.macosforge.org

##### fix permissions
`sudo chown -R $USER:admin /usr/local`

`sudo chown -R $USER:admin $(brew --cache)`


##### install packages
`brew install ccache d-bus fontconfig freetype gettext glib gmp icu4c libffi libpng libtasn1 libtiff pkg-config xz hicolor-icon-theme gsettings-desktop-schemas gnutls atk pixman`

##### (not sure this is necessary)
`brew link --overwrite gsettings-desktop-schemas`

##### install gtk and dependencies
`brew install --build-from-source at-spi2-core at-spi2-atk cairo harfbuzz pango gtk+3 gtk+ librsvg gnome-icon-theme --without-x --without-x11 --with-gtk+3`

### building

at this point, regardless of the build system you used, beekeep should now compile and run:

`cd aleph/utils/beekeep`

`make`

`./beekeep`

however, i get missing images for spinbox and popup menu widgets. more help troubleshooting this is appreciated.
