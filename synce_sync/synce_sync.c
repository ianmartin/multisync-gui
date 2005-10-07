/*
 * Stolen from evolution2
 */
#include "multisync.h"
#include "interface.h"
#include "callbacks.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

GtkWidget *synce_wnd_options;

typedef struct synce_options {
	char		*addressbook_path;
	char		*calendar_path;
	char		*tasks_path;
	char		*path;
	osync_bool	recursive;
} synce_options;

synce_options *options;

GtkWidget *synce_wnd_options;
OSyncMember *member;

void synce_fill_calendar_menu(OSyncMember *member, char *selected)
{
#if 0
	GtkWidget *menuitem;
	GtkWidget *calendarmenu;
	calendarmenu = gtk_menu_new ();
	
	menuitem = gtk_menu_item_new_with_label ("No Calendar");
	gtk_menu_append (GTK_MENU (calendarmenu), menuitem);
	gtk_object_set_data (GTK_OBJECT (menuitem), "uri", NULL);
	gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
	
	OSyncError *error = NULL;
	GList *calendars = osync_member_call_plugin(member, "evo2_list_calendars", NULL, &error);
	if (osync_error_is_set(&error)) {
		printf("Unable to call plugin: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
	GList *c;
	int n = 1;
	for (c = calendars; c; c = c->next) {
		evo2_location *path = c->data;
		menuitem = gtk_menu_item_new_with_label(path->name);
		gtk_menu_append (GTK_MENU (calendarmenu), menuitem);
		gtk_object_set_data (GTK_OBJECT (menuitem), "uri", path->uri);
		//FIXME free the evo2_location somewhere!
		if (selected && !strcmp(path->uri, selected)) {
			gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
			gtk_menu_set_active (GTK_MENU(calendarmenu), n);
		}
		n++;
	}
			
	gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(synce_wnd_options, "calendarmenu")), calendarmenu);
	gtk_widget_show_all(GTK_WIDGET(calendarmenu));
#endif
}

void synce_fill_tasks_menu(OSyncMember *member, char *selected)
{
#if 0
	GtkWidget *menuitem;
	GtkWidget *tasksmenu;
	tasksmenu = gtk_menu_new ();
	
	menuitem = gtk_menu_item_new_with_label ("No Tasks");
	gtk_menu_append (GTK_MENU (tasksmenu), menuitem);
	gtk_object_set_data (GTK_OBJECT (menuitem), "uri", NULL);
	gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
	
	OSyncError *error = NULL;
	GList *tasks = osync_member_call_plugin(member, "evo2_list_tasks", NULL, &error);
	if (osync_error_is_set(&error)) {
		printf("Unable to call plugin: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
	GList *c;
	int n = 1;
	for (c = tasks; c; c = c->next) {
		evo2_location *path = c->data;
		menuitem = gtk_menu_item_new_with_label(path->name);
		gtk_menu_append (GTK_MENU (tasksmenu), menuitem);
		gtk_object_set_data (GTK_OBJECT (menuitem), "uri", path->uri);
		//FIXME free the evo2_location somewhere!
		if (selected && !strcmp(path->uri, selected)) {
			gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
			gtk_menu_set_active (GTK_MENU(tasksmenu), n);
		}
		n++;
	}
			
	gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(synce_wnd_options, "todomenu")), tasksmenu);
	gtk_widget_show_all(GTK_WIDGET(tasksmenu));
#endif
}

void synce_fill_addressbook_menu(OSyncMember *member, char *selected)
{
#if 0
	GtkWidget *menuitem;
	GtkWidget *addressbookmenu;
	addressbookmenu = gtk_menu_new ();
	
	menuitem = gtk_menu_item_new_with_label ("No Addressbook");
	gtk_menu_append (GTK_MENU (addressbookmenu), menuitem);
	gtk_object_set_data (GTK_OBJECT (menuitem), "uri", NULL);
	gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
	
	OSyncError *error = NULL;
	GList *tasks = osync_member_call_plugin(member, "evo2_list_addressbooks", NULL, &error);
	if (osync_error_is_set(&error)) {
		printf("Unable to call plugin: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
	GList *c;
	int n = 1;
	for (c = tasks; c; c = c->next) {
		evo2_location *path = c->data;
		menuitem = gtk_menu_item_new_with_label(path->name);
		gtk_menu_append (GTK_MENU (addressbookmenu), menuitem);
		gtk_object_set_data (GTK_OBJECT (menuitem), "uri", path->uri);
		//FIXME free the evo2_location somewhere!
		if (selected && !strcmp(path->uri, selected)) {
			gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
			gtk_menu_set_active (GTK_MENU(addressbookmenu), n);
		}
		n++;
	}
			
	gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(synce_wnd_options, "addressbookmenu")), addressbookmenu);
	gtk_widget_show_all(GTK_WIDGET(addressbookmenu));
#endif
}

void msync_synce_make_config(synce_options *options, char **data, int *size)
{
	xmlDocPtr doc;
	
	doc = xmlNewDoc((xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, (xmlChar*)"config", NULL);
	
	xmlNewChild(doc->children, NULL, (xmlChar*)"adress_path", (xmlChar*)options->addressbook_path);
	xmlNewChild(doc->children, NULL, (xmlChar*)"calendar_path", (xmlChar*)options->calendar_path);
	xmlNewChild(doc->children, NULL, (xmlChar*)"tasks_path", (xmlChar*)options->tasks_path);
	
	xmlDocDumpMemory(doc, (xmlChar **)data, size);
	*size++;
}


static osync_bool fs_parse_settings(synce_options *env, char *data, int size, OSyncError **error)
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

osync_bool msync_synce_parse_config(synce_options *options, char *data, int size)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	printf("EVO2-SYNC start: %s\n", __func__);

	//set defaults
	options->addressbook_path = NULL;
	options->calendar_path = NULL;
	options->tasks_path = NULL;

	printf("Config data: %s\n", data);

	doc = xmlParseMemory(data, size);

	if (!doc) {
		printf("EVO2-SYNC Could not parse data!\n");
		msync_show_msg_warn("Unable to open options.\nCould not parse configuration data");
		return FALSE;
	}

	cur = xmlDocGetRootElement(doc);

	if (!cur) {
		printf("EVO2-SYNC data seems to be empty\n");
		xmlFreeDoc(doc);
		return FALSE;
	}

	if (xmlStrcmp(cur->name, (xmlChar*)"config")) {
		printf("EVO2-SYNC data seems not to be a valid configdata.\n");
		xmlFreeDoc(doc);
		return FALSE;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		char *str = (char*)xmlNodeGetContent(cur);
		if (str) {
			if (!xmlStrcmp(cur->name, (const xmlChar *)"adress_path")) {
				options->addressbook_path = g_strdup(str);
			}
			if (!xmlStrcmp(cur->name, (const xmlChar *)"calendar_path")) {
				options->calendar_path = g_strdup(str);
			}
			if (!xmlStrcmp(cur->name, (const xmlChar *)"tasks_path")) {
				options->tasks_path = g_strdup(str);	
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
	
	synce_fill_calendar_menu(target, options->calendar_path);
	synce_fill_tasks_menu(target, options->tasks_path);
	synce_fill_addressbook_menu(target, options->addressbook_path);
	
	gtk_widget_show (synce_wnd_options);
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
synce_on_btn_ok_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *item;
	item = gtk_option_menu_get_menu (GTK_OPTION_MENU (lookup_widget(synce_wnd_options, "addressbookmenu")));
	item = gtk_menu_get_active (GTK_MENU (item));
	options->addressbook_path = gtk_object_get_data (GTK_OBJECT (item), "uri");
	
	item = gtk_option_menu_get_menu (GTK_OPTION_MENU (lookup_widget(synce_wnd_options, "calendarmenu")));
	item = gtk_menu_get_active (GTK_MENU (item));
	options->calendar_path = gtk_object_get_data (GTK_OBJECT (item), "uri");
	
	item = gtk_option_menu_get_menu (GTK_OPTION_MENU (lookup_widget(synce_wnd_options, "todomenu")));
	item = gtk_menu_get_active (GTK_MENU (item));
	options->tasks_path = gtk_object_get_data (GTK_OBJECT (item), "uri");
	
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
