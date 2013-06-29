project("installer" C)

cmake_minimum_required(VERSION 2.6)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/ )

find_package(PkgConfig)

Include(Wayland)
Include(GObjectIntrospection)

SET(CMAKE_BUILD_TYPE "Release")

pkg_check_modules (GTK REQUIRED gtk+-3.0)
add_definitions (${GTK_CFLAGS})
link_libraries (${GTK_LIBRARIES})
link_directories (${GTK_LIBRARY_DIRS})

pkg_check_modules (GOBJECT REQUIRED gobject-2.0)
add_definitions (${GOBJECT_CFLAGS})
link_libraries (${GOBJECT_LIBRARIES})
link_directories (${GOBJECT_LIBRARY_DIRS})

pkg_check_modules (GOBJECT_INTROSPECTION REQUIRED gobject-introspection-1.0)
add_definitions (${GOBJECT_INTROSPECTION_CFLAGS})
link_libraries (${GOBJECT_INTROSPECTION_LIBRARIES})
link_directories (${GOBJECT_INTROSPECTION_LIBRARY_DIRS})


pkg_check_modules (WAYLAND_CLIENT REQUIRED wayland-client)
add_definitions (${WAYLAND_CLIENT_CFLAGS})
link_libraries (${WAYLAND_CLIENT_LIBRARIES})
link_directories (${WAYLAND_CLIENT_LIBRARY_DIRS})

pkg_check_modules (LIBPEAS REQUIRED libpeas-1.0>=1.8.0)
add_definitions (${LIBPEAS_CFLAGS})
link_directories (${LIBPEAS_LIBRARY_DIRS})

pkg_check_modules (LIBPEASGTK REQUIRED libpeas-gtk-1.0>=1.8.0)
add_definitions (${LIBPEASGTK_CFLAGS})
link_directories (${LIBPEASGTK_LIBRARY_DIRS})

include_directories (${CMAKE_SOURCE_DIR})

SET (GTK_SHELL_SOURCES
    shell/gtk-shell.c
    shell/gtk-shell.h)

wayland_add_protocol_client(GTK_SHELL_SOURCES
    ${CMAKE_SOURCE_DIR}/protocol/desktop-shell.xml
    desktop-shell
)

SET (CONSORT_SHELL_SOURCES
    shell/consort-shell.c
    shell/consort-shell.h
    shell/consort-shell-private.h)
    
wayland_add_protocol_client(CONSORT_SHELL_SOURCES
    ${CMAKE_SOURCE_DIR}/protocol/desktop-shell.xml
    consort-shell
)

add_executable (wayland-desktop-shell ${GTK_SHELL_SOURCES})
set_target_properties(wayland-desktop-shell PROPERTIES PREFIX "")

add_executable (consort-shell ${CONSORT_SHELL_SOURCES})
target_link_libraries (consort-shell ${LIBPEAS_LIBRARIES})
target_link_libraries (consort-shell ${LIBPEASGTK_LIBRARIES})

gobject_introspection(
  FILENAME ConsortShell-0.1.gir
  NSVERSION 0.1
  NAMESPACE ConsortShell
  INCLUDE GObject-2.0 GLib-2.0 Gio-2.0 Gtk-3.0 Peas-1.0
  PROGRAM ${CMAKE_BINARY_DIR}/consort-shell
  SCANNER_ARGS --warn-all --add-include-path=${CMAKE_CURRENT_SOURCE_DIR} -I/usr/include/gobject-introspection-1.0
  COMPILER_ARGS "--includedir=${CMAKE_CURRENT_SOURCE_DIR}"
  SOURCES ${CONSORT_SHELL_SOURCES}
  QUIET
)

INSTALL(PROGRAMS wayland-desktop-shell DESTINATION libexec)
INSTALL(PROGRAMS consort-shell DESTINATION libexec)

#INSTALL(FILES ${CMAKE_BINARY_DIR}/ConsortShell-0.1.gir DESTINATION ${GOBJECT_INTROSPECTION_GIRDIR})
#INSTALL(FILES ${CMAKE_BINARY_DIR}/ConsortShell-0.1.typelib DESTINATION ${GOBJECT_INTROSPECTION_TYPELIBDIR})
  
INSTALL(FILES ${CMAKE_BINARY_DIR}/ConsortShell-0.1.gir DESTINATION share/gir-1.0/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/ConsortShell-0.1.typelib DESTINATION lib/girepository-1.0/)
