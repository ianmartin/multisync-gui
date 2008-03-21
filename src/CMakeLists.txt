CONFIGURE_FILE( "multisync-gui.glade.in" "multisync-gui.glade" )

INCLUDE_DIRECTORIES( ${OPENSYNC_INCLUDE_DIRS} ${GTK2_INCLUDE_DIRS} ${LIBGLADE_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${OPENSYNC_LIBRARY_DIRS} ${GTK2_LIBRARY_DIRS} ${LIBGLADE_LIBRARY_DIRS} ${LIBXML2_LIBRARY_DIRS} )

SET( multisyncgui_SRCS
	plugins/msync_default.c
	#plugins/msync_evo2sync.c
	msync_callbacks.c
	msync_env.c
	msync_group.c
	msync_plugin.c
	msync_support.c
	multisync.c
)

ADD_EXECUTABLE( multisyncgui ${multisyncgui_SRCS} )
TARGET_LINK_LIBRARIES( multisyncgui ${OPENSYNC_LIBRARIES} ${GTK2_LIBRARIES} ${LIBGLADE_LIBRARIES} ${GTK_LIBRARIES} )

###### INSTALL ################### 

INSTALL( TARGETS multisyncgui DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
INSTALL( FILES multisync-gui.glade DESTINATION "${CMAKE_INSTALL_PREFIX}/share/multisync-gui/" )
  
