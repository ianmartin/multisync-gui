#include "multisync.h"
#include "interface.h"
#include "callbacks.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

GtkWidget *file_wnd_options;
OSyncMember *member;

typedef struct fs_options {
	char *path;
	osync_bool recursive;
} fs_options;

fs_options *options;

static osync_bool fs_parse_settings(fs_options *env, char *data, int size, OSyncError **error)
{
	osync_trace(TRACE_ENTRY, "%s(%p, %p, %i)", __func__, env, data, size);
	xmlDocPtr doc;
	xmlNodePtr cur;

	//set defaults
	env->path = "";
	env->recursive = TRUE;

	doc = xmlParseMemory(data, size);

	if (!doc) {
		osync_error_set(error, OSYNC_ERROR_GENERIC, "Unable to parse settings");
		osync_trace(TRACE_EXIT_ERROR, "%s: %s", __func__, osync_error_print(error));
		return FALSE;
	}

	cur = xmlDocGetRootElement(doc);

	if (!cur) {
		xmlFreeDoc(doc);
		osync_error_set(error, OSYNC_ERROR_GENERIC, "Unable to get root element of the settings");
		osync_trace(TRACE_EXIT_ERROR, "%s: %s", __func__, osync_error_print(error));
		return FALSE;
	}

	if (xmlStrcmp(cur->name, "config")) {
		xmlFreeDoc(doc);
		osync_error_set(error, OSYNC_ERROR_GENERIC, "Config valid is not valid");
		osync_trace(TRACE_EXIT_ERROR, "%s: %s", __func__, osync_error_print(error));
		return FALSE;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		char *str = xmlNodeGetContent(cur);
		if (str) {
			if (!xmlStrcmp(cur->name, (const xmlChar *)"path")) {
				env->path = g_strdup(str);
			}
			if (!xmlStrcmp(cur->name, (const xmlChar *)"recursive")) {
				env->recursive = (g_ascii_strcasecmp(str, "TRUE") == 0);
			}
			xmlFree(str);
		}
		cur = cur->next;
	}

	xmlFreeDoc(doc);
	osync_trace(TRACE_EXIT, "%s", __func__);
	return TRUE;
}

void msync_file_sync_options(MSyncEnv *env, OSyncMember *target)
{
	char *config;
	int size;
	member = target;
	OSyncError *error = NULL;
	if (!osync_member_get_config(member, &config, &size, &error)) {
		printf("Unable to get config: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	printf("showing options for plugin file-sync!\n");
	file_wnd_options = create_wnd_options();
	
	
	options = g_malloc0(sizeof(fs_options));
	if (!fs_parse_settings(options, config, size, &error))
		return;
	
	GtkEntry *entry = GTK_ENTRY(lookup_widget(file_wnd_options, "txt_path"));
	gtk_entry_set_text(entry, options->path);
	gtk_widget_show (file_wnd_options);
	g_free(config);
}

void msync_register_file_sync(MSyncEnv *env)
{
	msync_register_plugin(env, "file-sync", msync_file_sync_options);
}

void
on_wnd_options_destroy                 (GtkObject       *object,
                                        gpointer         user_data)
{
	printf("release controls now\n");
}

void
on_btn_cancel_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(file_wnd_options);
	file_wnd_options = NULL;
}

static void msync_fs_make_config(fs_options *options, char **data, int *size)
{
	xmlDocPtr doc;
	
	doc = xmlNewDoc("1.0");
	doc->children = xmlNewDocNode(doc, NULL, "config", NULL);
	
	xmlNewChild(doc->children, NULL, "path", options->path);
	xmlNewChild(doc->children, NULL, "recursive", options->recursive ? "TRUE" : "FALSE");
	
	xmlDocDumpMemory(doc, (xmlChar **)data, size);
	*size++;
}

void
on_btn_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkEntry *item;
	item = GTK_ENTRY(lookup_widget(file_wnd_options, "txt_path"));
	options->path = g_strdup(gtk_entry_get_text(item));
	options->recursive = FALSE;
	
	char *config = NULL;
	int size = 0;
	msync_fs_make_config(options, &config, &size);
	osync_member_set_config(member, config, size);
	gtk_widget_destroy(file_wnd_options);
	file_wnd_options = NULL;
}
