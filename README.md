# Consort Shell

This is a fork of Tiago Vignatti's gtk-shell (https://github.com/tiagovignatti)

The aim here is to simplify the construction of a basic desktop system that ties in with Weston's desktop shell.
ConsortShell is actually a client and not a plugin. We provide a very basic system that is merely enough to allow
non-C plugins to be created quickly and easily.

All non-static consort_shell functions are exported via GObject Introspection. Plugins may be implemented in any
language supported by libpeas. You may also offer GTK configuration dialogs through the native procedures available.

*Building*

    cmake -DCMAKE_INSTALL_PREFIX=/usr
    make
    make install
    
*Running*

    export GDK_BACKEND=wayland
    weston
    
