vala_precompile(PANEL_PLUGIN
"panel_plugin.vala"
PACKAGES
gtk+-3.0
cairo
gmodule-2.0
libpeas-1.0
PeasGtk-1.0
Consort-0.1
DIRECTORY
.
OPTIONS
--disable-warnings
--thread
--girdir=${CMAKE_BINARY_DIR}/.
)

add_library ("panel" SHARED ${PANEL_PLUGIN})
target_link_libraries ("panel" ${LIBPEAS_LIBRARIES})
target_link_libraries ("panel" ${LIBPEASGTK_LIBRARIES})
target_link_libraries ("panel" consort-shell)

INSTALL(PROGRAMS libpanel.so DESTINATION share/consort2/plugins)
INSTALL(FILES panel.plugin DESTINATION share/consort2/plugins)
