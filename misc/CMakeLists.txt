CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/multisyncgui.desktop" "${CMAKE_CURRENT_BINARY_DIR}/multisyncgui.desktop" )
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/multisyncgui.glade" "${CMAKE_CURRENT_BINARY_DIR}/multisyncgui.glade")

###### INSTALL ################### 

INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/multisyncgui.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/applications/" )
INSTALL( FILES "${CMAKE_CURRENT_BINARY_DIR}/multisyncgui.glade" DESTINATION "${MULTISYNCGUI_GLADE_DIR}" )
INSTALL( FILES multisync.png DESTINATION ${MULTISYNCGUI_ICON_DIR} ) 
