#include "multisync.h"

void msync_plugin_register( GList** plugins,
							const char* name,
							GtkWidget *widget,
							MSyncPluginGetConfig get_config,
							MSyncPluginSetConfig set_config,
							void* private)
{
	g_assert(name);
	g_assert(widget);
	g_assert(get_config);
	g_assert(set_config);
		
	MSyncPlugin* plugin = g_malloc0(sizeof(MSyncPlugin));
	plugin->_private = private;
	plugin->name = name;
	plugin->widget = widget;
	plugin->msync_plugin_get_config = get_config;
	plugin->msync_plugin_set_config = set_config;
	*plugins = g_list_append(*plugins, plugin);
}

MSyncPlugin* msync_plugin_find(GList* plugins, const char* name)
{
	MSyncPlugin* plugin;
	GList* tmp;
	
	plugin = NULL;
	for(tmp = g_list_first(plugins); tmp != NULL; tmp = tmp->next) {
		if(strcmp(((MSyncPlugin *)tmp->data)->name, name) == 0) {
			plugin = tmp->data;
			break;
		}
	}
	return plugin;
}
