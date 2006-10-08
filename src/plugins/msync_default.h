#ifndef MSYNC_DEFAULT_H_
#define MSYNC_DEFAULT_H_

GtkWidget* msync_defaultplugin_create_widget(void);
void msync_defaultplugin_set_config(MSyncPlugin* plugin, const char *config);
const char* msync_defaultplugin_get_config(MSyncPlugin* plugin);

#endif /*MSYNC_DEFAULT_H_*/
