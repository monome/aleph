### beekeep OSX build

GTK is a little painful to setup. in theory, following this page should get it started ( you are aiming for gtk3 ):

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
	
you should then be able to simply:
`cd aleph/utils/beekeep`
`make`

TODO: bundling