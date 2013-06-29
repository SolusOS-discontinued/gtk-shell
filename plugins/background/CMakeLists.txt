vala_precompile(BACKGROUND_PLUGIN
"background_plugin.vala"
PACKAGES
gtk+-3.0
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

add_library ("background" SHARED ${BACKGROUND_PLUGIN})
target_link_libraries ("background" ${LIBPEAS_LIBRARIES})
target_link_libraries ("background" ${LIBPEASGTK_LIBRARIES})
target_link_libraries ("background" consort-shell)

INSTALL(PROGRAMS libbackground.so DESTINATION share/consort2/plugins)
INSTALL(FILES background.plugin DESTINATION share/consort2/plugins)
