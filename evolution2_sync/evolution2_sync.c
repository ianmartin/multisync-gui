#include "multisync.h"
#include "interface.h"
#include "callbacks.h"

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
		printf("Unable to call plugin: %s\n", error->message);
		osync_error_free(&error);
		return;
	}
	
	
	GList *c;
	int n = 1;
	for (c = calendars; c; c = c->next) {
		char *uri = NULL;
		char *name = NULL;
		sscanf(c->data, "%[^:]%s", uri, name);
		menuitem = gtk_menu_item_new_with_label(name);
		gtk_menu_append (GTK_MENU (calendarmenu), menuitem);
		gtk_object_set_data (GTK_OBJECT (menuitem), "uri", uri);
		if (selected && !strcmp(uri, selected)) {
			gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
			gtk_menu_set_active (GTK_MENU(calendarmenu), n);
		}
		n++;
	}
			
	gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(evo2_wnd_options, "calendarmenu")), calendarmenu);
	gtk_widget_show_all(GTK_WIDGET(calendarmenu));
}

/*
void fill_tasks_menu(char *selected)
{
        GtkWidget *menuitem;
        GtkWidget *tasksmenu;
        tasksmenu = gtk_menu_new ();
        ESourceList *sources;
        ESource *source;

        menuitem = gtk_menu_item_new_with_label ("No Tasks");
        gtk_menu_append (GTK_MENU (tasksmenu), menuitem);
        gtk_object_set_data (GTK_OBJECT (menuitem), "uri", NULL);
        gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));

        if (!e_cal_get_sources(&sources, E_CAL_SOURCE_TYPE_TODO, NULL)) {
                return;
        }

        int n = 1;
        GSList *g;
        for (g = e_source_list_peek_groups (sources); g; g = g->next) {
                ESourceGroup *group = E_SOURCE_GROUP (g->data);
                GSList *s;
                for (s = e_source_group_peek_sources (group); s; s = s->next) {
                        source = E_SOURCE (s->data);
                        menuitem = gtk_menu_item_new_with_label (e_source_peek_name(source));
                        gtk_menu_append (GTK_MENU (tasksmenu), menuitem);
                        gtk_object_set_data (GTK_OBJECT (menuitem), "uri", e_source_get_uri(source));
                        if (selected && !strcmp(e_source_get_uri(source), selected)) {
                                gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
                                gtk_menu_set_active (GTK_MENU(tasksmenu), n);
                        }
                        n++;
                }
        }
        gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(evowindow, "todomenu")), tasksmenu);
        gtk_widget_show_all(GTK_WIDGET(tasksmenu));
}

void fill_addressbook_menu(char *selected)
{
        GtkWidget *menuitem;
        GtkWidget *addressbookmenu;
        addressbookmenu = gtk_menu_new ();
        ESourceList *sources;
        ESource *source;

        menuitem = gtk_menu_item_new_with_label ("No Addressbook");
        gtk_menu_append (GTK_MENU (addressbookmenu), menuitem);
        gtk_object_set_data (GTK_OBJECT (menuitem), "uri", NULL);
        gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));

        if (!e_book_get_addressbooks(&sources, NULL)) {
                return;
        }

        GSList *g;
        int n = 1;
        for (g = e_source_list_peek_groups (sources); g; g = g->next) {
                ESourceGroup *group = E_SOURCE_GROUP (g->data);
                GSList *s;
                for (s = e_source_group_peek_sources (group); s; s = s->next) {
                        source = E_SOURCE (s->data);
                        menuitem = gtk_menu_item_new_with_label (e_source_peek_name(source));
                        gtk_menu_append (GTK_MENU (addressbookmenu), menuitem);
                        gtk_object_set_data (GTK_OBJECT (menuitem), "uri", e_source_get_uri(source));
                        if (selected && !strcmp(e_source_get_uri(source), selected)) {
                                gtk_menu_item_activate(GTK_MENU_ITEM(menuitem));
                                gtk_menu_set_active (GTK_MENU(addressbookmenu), n);
                        }
                        n++;
                }
        }
        gtk_option_menu_set_menu (GTK_OPTION_MENU(lookup_widget(evowindow, "addressbookmenu")), addressbookmenu);
        gtk_widget_show_all(GTK_WIDGET(addressbookmenu));
}*/

osync_bool msync_evo2_parse_config(evo2_options *options, char *data, int size)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	printf("EVO2-SYNC start: %s\n", __func__);

	//set defaults
	options->addressbook_path = NULL;
	options->calendar_path = NULL;
	options->tasks_path = NULL;

	doc = xmlParseMemory(data, size);

	if (!doc) {
		printf("EVO2-SYNC Could not parse data!\n");
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
		printf("Unable to get config: %s\n", error->message);
		osync_error_free(&error);
		return;
	}
	
	//Parse config
	options = g_malloc0(sizeof(evo2_options));
	if (!msync_evo2_parse_config(options, config, size))
		return;
	
	fill_calendar_menu(target, options->calendar_path);
	//fill_tasks_menu(options->tasks_path);
	//fill_addressbook_menu(options->addressbook_path);
	
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
	/*GtkEntry *entry = GTK_ENTRY(lookup_widget(file_wnd_options, "txt_path"));
	const char *config = gtk_entry_get_text(entry);
	osync_member_set_config(member, config, strlen(config) + 1);*/
	gtk_widget_destroy(evo2_wnd_options);
	evo2_wnd_options = NULL;
}
