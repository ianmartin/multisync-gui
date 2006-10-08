#ifndef MSYNC_PLUGIN_H_
#define MSYNC_PLUGIN_H_

struct MSyncPlugin {
	void* _private;
	const char* name;
	GtkWidget *widget;
	MSyncPluginGetConfig msync_plugin_get_config;
	MSyncPluginSetConfig msync_plugin_set_config;
};


void msync_plugin_register( GList** plugins,
							const char* name,
							GtkWidget *widget,
							MSyncPluginGetConfig get_config,
							MSyncPluginSetConfig set_config,
							void* private);

MSyncPlugin* msync_plugin_find(GList* plugins, const char* name);

#endif /*MSYNC_PLUGIN_H_*/
