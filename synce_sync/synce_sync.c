#include "multisync.h"
#include "interface.h"
#include "callbacks.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <gtk/gtk.h>

GtkWidget *synce_wnd_options;

typedef struct synce_options {
	char		*file_path;
	osync_bool	recursive;
} synce_options;

synce_options *options;

GtkWidget *synce_wnd_options;
OSyncMember *member;

void synce_fill_directory_menu(OSyncMember *member, char *selected)
{
	GList		*d, *dirs;
	OSyncError	*error = NULL;
	GtkWidget	*menu = gtk_menu_new();
	GtkWidget	*menuitem;

	fprintf(stderr, "%s\n", __func__);

	dirs = osync_member_call_plugin(member, "synce_list_directories", NULL, &error);

	for (d = dirs; d; d = d->next) {
		char	*s = d->data;
#if 0
		fprintf(stderr, "%s -> %s\n", __func__, s);
#endif
		menuitem = gtk_menu_item_new_with_label(s);
		gtk_menu_append(GTK_MENU(menu), menuitem);
		gtk_object_set_data(GTK_OBJECT(menuitem), "uri", s);
		gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
	}

	gtk_option_menu_set_menu(GTK_OPTION_MENU(lookup_widget(synce_wnd_options,
					"optionmenu1")), menu);
	gtk_widget_show_all(GTK_WIDGET(menu));
}

void msync_synce_make_config(synce_options *options, char **data, int *size)
{
	xmlDocPtr doc;
	
	doc = xmlNewDoc((xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, (xmlChar*)"config", NULL);
	
	xmlNewChild(doc->children, NULL, (xmlChar*)"files_path", (xmlChar*)options->file_path);

	xmlDocDumpMemory(doc, (xmlChar **)data, size);
	*size++;
}


static osync_bool fs_parse_settings(synce_options *env, char *data, int size, OSyncError **error)
{
	osync_trace(TRACE_ENTRY, "%s(%p, %p, %i)", __func__, env, data, size);
	xmlDocPtr doc;
	xmlNodePtr cur;

	//set defaults
	env->file_path = "";
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

	if (xmlStrcmp(cur->name, (xmlChar*)"config")) {
		xmlFreeDoc(doc);
		osync_error_set(error, OSYNC_ERROR_GENERIC, "Config valid is not valid");
		osync_trace(TRACE_EXIT_ERROR, "%s: %s", __func__, osync_error_print(error));
		return FALSE;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		char *str = (char*)xmlNodeGetContent(cur);
		if (str) {
			if (!xmlStrcmp(cur->name, (const xmlChar *)"files_path")) {
				env->file_path = g_strdup(str);
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

osync_bool msync_synce_parse_config(synce_options *options, char *data, int size)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	printf("SynCE-SYNC start: %s\n", __func__);

	//set defaults
	options->file_path = NULL;

	printf("Config data: %s\n", data);

	doc = xmlParseMemory(data, size);

	if (!doc) {
		printf("SynCE-SYNC Could not parse data!\n");
		msync_show_msg_warn("Unable to open options.\nCould not parse configuration data");
		return FALSE;
	}

	cur = xmlDocGetRootElement(doc);

	if (!cur) {
		printf("SynCE-SYNC data seems to be empty\n");
		xmlFreeDoc(doc);
		return FALSE;
	}

	if (xmlStrcmp(cur->name, (xmlChar*)"config")) {
		printf("SynCE-SYNC data seems not to be a valid configdata.\n");
		xmlFreeDoc(doc);
		return FALSE;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		char *str = (char*)xmlNodeGetContent(cur);
		if (str) {
			if (!xmlStrcmp(cur->name, (const xmlChar *)"files_path")) {
				options->file_path = g_strdup(str);
			}
			xmlFree(str);
		}
		cur = cur->next;
	}

	xmlFreeDoc(doc);
	return TRUE;
}

void msync_synce_sync_options(MSyncEnv *env, OSyncMember *target)
{
	char *config;
	int size;
	
	member = target;

	printf("showing options for plugin SynCE-sync!\n");

	synce_wnd_options = create_synce_wnd_options();

	OSyncError *error = NULL;

	if (!osync_member_get_config_or_default(member, &config, &size, &error)) {
		printf("Unable to get config: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
	//Parse config
	options = g_malloc0(sizeof(synce_options));
	if (!msync_synce_parse_config(options, config, size))
		return;
	if (!fs_parse_settings(options, config, size, &error))
		return;
	
	synce_fill_directory_menu(target, options->file_path);
	
	gtk_widget_show(synce_wnd_options);
	g_free(config);
#if 0
	GtkEntry *entry = GTK_ENTRY(lookup_widget(synce_wnd_options, "txt_path"));
	gtk_entry_set_text(entry, options->path);
#endif
}


void
synce_on_wnd_options_destroy           (GtkObject       *object,
                                        gpointer         user_data)
{
	g_free(options);
	return;
}


void
synce_on_btn_cancel_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(synce_wnd_options);
	synce_wnd_options = NULL;
}


void
synce_on_btn_ok_clicked(GtkButton *button, gpointer user_data)
{
	GtkWidget *item;

	item = gtk_option_menu_get_menu(GTK_OPTION_MENU
			(lookup_widget(synce_wnd_options, "optionmenu1")));
	item = gtk_menu_get_active(GTK_MENU(item));
	options->file_path = gtk_object_get_data(GTK_OBJECT (item), "uri");
	fprintf(stderr, "Yow %s(%s)\n", __func__, options->file_path);

	char *config = NULL;
	int size = 0;
	msync_synce_make_config(options, &config, &size);
	osync_member_set_config(member, config, size);
	gtk_widget_destroy(synce_wnd_options);
	synce_wnd_options = NULL;
}

void msync_register_synce_sync(MSyncEnv *env)
{
	msync_register_plugin(env, "synce-plugin", msync_synce_sync_options);
}

#if 0
static void msync_fs_make_config(synce_options *options, char **data, int *size)
{
	xmlDocPtr doc;
	
	doc = xmlNewDoc((xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, (xmlChar*)"config", NULL);
	
	xmlNewChild(doc->children, NULL, (xmlChar*)"path", (xmlChar*)options->path);
	xmlNewChild(doc->children, NULL, (xmlChar*)"recursive", (xmlChar*)(options->recursive ? "TRUE" : "FALSE"));
	
	xmlDocDumpMemory(doc, (xmlChar **)data, size);
	*size++;
}

void
synce_on_btn_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkEntry *item;
	item = GTK_ENTRY(lookup_widget(synce_wnd_options, "txt_path"));
	options->path = g_strdup(gtk_entry_get_text(item));
	options->recursive = FALSE;
	
	char *config = NULL;
	int size = 0;
	msync_fs_make_config(options, &config, &size);
	osync_member_set_config(member, config, size);
	gtk_widget_destroy(synce_wnd_options);
	synce_wnd_options = NULL;
}
#endif
