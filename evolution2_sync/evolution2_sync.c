#include "multisync.h"
#include "interface.h"
#include "callbacks.h"
#include "opensync/evo2_sync.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

GtkWidget *evo2_wnd_options;
OSyncMember *member;

typedef struct evo2_options {
	char *addressbook_path;
	char *calendar_path;
	char *tasks_path;
} evo2_options;

evo2_options *options;

void fill_calendar_menu(OSyncMember *member, char *selected)
{
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
			
	gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(evo2_wnd_options, "calendarmenu")), calendarmenu);
	gtk_widget_show_all(GTK_WIDGET(calendarmenu));
}

void fill_tasks_menu(OSyncMember *member, char *selected)
{
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
			
	gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(evo2_wnd_options, "todomenu")), tasksmenu);
	gtk_widget_show_all(GTK_WIDGET(tasksmenu));
}

void fill_addressbook_menu(OSyncMember *member, char *selected)
{
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
			
	gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(evo2_wnd_options, "addressbookmenu")), addressbookmenu);
	gtk_widget_show_all(GTK_WIDGET(addressbookmenu));
}

void msync_evo2_make_config(evo2_options *options, char **data, int *size)
{
	xmlDocPtr doc;
	
	doc = xmlNewDoc("1.0");
	doc->children = xmlNewDocNode(doc, NULL, "config", NULL);
	
	xmlNewChild(doc->children, NULL, "adress_path", options->addressbook_path);
	xmlNewChild(doc->children, NULL, "calendar_path", options->calendar_path);
	xmlNewChild(doc->children, NULL, "tasks_path", options->tasks_path);
	
	xmlDocDumpMemory(doc, (xmlChar **)data, size);
	*size++;
}


osync_bool msync_evo2_parse_config(evo2_options *options, char *data, int size)
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

	if (xmlStrcmp(cur->name, "config")) {
		printf("EVO2-SYNC data seems not to be a valid configdata.\n");
		xmlFreeDoc(doc);
		return FALSE;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		char *str = xmlNodeGetContent(cur);
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

void msync_evo2_sync_options(MSyncEnv *env, OSyncMember *target)
{
	char *config;
	int size;
	
	member = target;
	printf("showing options for plugin evo2-sync!\n");
	evo2_wnd_options = create_optwin();
	OSyncError *error = NULL;
	if (!osync_member_get_config(member, &config, &size, &error)) {
		printf("Unable to get config: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
	//Parse config
	options = g_malloc0(sizeof(evo2_options));
	if (!msync_evo2_parse_config(options, config, size))
		return;
	
	fill_calendar_menu(target, options->calendar_path);
	fill_tasks_menu(target, options->tasks_path);
	fill_addressbook_menu(target, options->addressbook_path);
	
	gtk_widget_show (evo2_wnd_options);
	g_free(config);
}

void msync_register_evo2_sync(MSyncEnv *env)
{
	msync_register_plugin(env, "evo2-sync", msync_evo2_sync_options);
}

gboolean
evo2_optwin_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	g_free(options);
	return FALSE;
}


void
evo2_cancelbutton_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(evo2_wnd_options);
	evo2_wnd_options = NULL;
}


void
evo2_okbutton_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *item;
	item = gtk_option_menu_get_menu (GTK_OPTION_MENU (lookup_widget(evo2_wnd_options, "addressbookmenu")));
	item = gtk_menu_get_active (GTK_MENU (item));
	options->addressbook_path = gtk_object_get_data (GTK_OBJECT (item), "uri");
	
	item = gtk_option_menu_get_menu (GTK_OPTION_MENU (lookup_widget(evo2_wnd_options, "calendarmenu")));
	item = gtk_menu_get_active (GTK_MENU (item));
	options->calendar_path = gtk_object_get_data (GTK_OBJECT (item), "uri");
	
	item = gtk_option_menu_get_menu (GTK_OPTION_MENU (lookup_widget(evo2_wnd_options, "todomenu")));
	item = gtk_menu_get_active (GTK_MENU (item));
	options->tasks_path = gtk_object_get_data (GTK_OBJECT (item), "uri");
	
	char *config = NULL;
	int size = 0;
	msync_evo2_make_config(options, &config, &size);
	osync_member_set_config(member, config, size);
	gtk_widget_destroy(evo2_wnd_options);
	evo2_wnd_options = NULL;
}
