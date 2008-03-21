SET( MULTISYNC_ICON_DIR "${CMAKE_INSTALL_PREFIX}/share/pixmaps/multisync-gui/")

CONFIGURE_FILE( "multisync-gui.desktop.in" "multisync-gui.desktop" )

###### INSTALL ################### 

INSTALL( FILES multisync.png DESTINATION ${MULTISYNC_ICON_DIR} )
INSTALL( FILES multisync-gui.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications/" ) 
