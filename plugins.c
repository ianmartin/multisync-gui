#include "multisync.h"

void msync_register_plugin(MSyncEnv *env, const char *name, MSyncOptionFunction function)
{
	MSyncPlugin *plugin = g_malloc0(sizeof(MSyncPlugin));
	plugin->name = g_strdup(name);
	plugin->options = function;
	printf("env %p\n", env);
	printf("name %s\n", plugin->name);
	printf("option %p\n", env->optionplugins);
	env->optionplugins = g_list_append(env->optionplugins, plugin);
}

MSyncPlugin *msync_find_plugin(MSyncEnv *env, const char *name)
{
	GList *p;
	for (p = env->optionplugins; p; p = p->next) {
		MSyncPlugin *plugin = p->data;
		if (!strcmp(plugin->name, name))
			return plugin;
	}
	return NULL;
}

void msync_register_plugins(MSyncEnv *env)
{
	printf("register plugins %p\n", env);
	msync_register_file_sync(env);
	msync_register_evo2_sync(env);
	msync_register_evo_sync(env);
}
