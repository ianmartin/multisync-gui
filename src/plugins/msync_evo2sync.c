#include "../multisync.h"

typedef struct evo2_location {
	char *name;
	char *uri;
} evo2_location;


void combo_box_fill(OSyncMember *member, char * membercall, char *none_selected_str, char *selected, GtkWidget* combobox, GList **list, OSyncError **error)
{
	gtk_combo_box_append_text (GTK_COMBO_BOX(combobox), none_selected_str);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0);

	*list = osync_member_call_plugin(member, membercall, NULL, error);
	if (osync_error_is_set(error)) {
		return;
	}

	GList *iter;
	int n = 1;
	for (iter = *list; iter; iter = iter->next) {
		evo2_location *path = iter->data;
		gtk_combo_box_append_text (GTK_COMBO_BOX(combobox), path->name);
		if (selected && !strcmp(path->uri, selected)) {
			gtk_combo_box_set_active (GTK_COMBO_BOX(combobox), n);
		}
		n++;
	}
}

void combo_box_empty(GtkWidget* combobox, GList** list)
{ 
	gtk_combo_box_remove_text(GTK_COMBO_BOX(combobox), 0);
	GList *iter;
	for (iter = *list; iter; iter = iter->next) {
		gtk_combo_box_remove_text(GTK_COMBO_BOX(combobox), 0);
		evo2_location *path = iter->data;
		free(path->name);
		free(path->uri);
	}
	g_list_free(*list);
	*list = NULL;
}

char *combo_box_get_uri(GtkWidget* combobox, GList* list)
{	
	char* uri = NULL;
	gchar* activestr = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combobox));
	
	GList *iter;
	for (iter = list; iter; iter = iter->next) {
		evo2_location *path = iter->data;
		if (!strcmp(path->name, (char *)activestr)) {
			uri = path->uri;
			break;
		}
	}
	g_free(activestr);
	return uri; 
}

GtkWidget* msync_evo2sync_create_widget(MSyncEvo2Sync* evo2sync) {
  GtkWidget* label;
  GtkWidget* addressbookcombobox;
  GtkWidget* calendarcombobox;
  GtkWidget* todocombobox;
  
  GtkWidget* vbox = gtk_vbox_new(FALSE, 10);
  gtk_widget_show (vbox);

  GtkWidget* textview = gtk_text_view_new();
  gtk_box_pack_start(GTK_BOX(vbox), textview, FALSE, FALSE, 0);

  label = gtk_label_new("<span weight='bold'>Addressbook:</span>");
  gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

  addressbookcombobox = gtk_combo_box_new_text ();
  gtk_widget_show(addressbookcombobox);
  gtk_box_pack_start (GTK_BOX (vbox), addressbookcombobox, FALSE, FALSE, 0);
  
  
  label = gtk_label_new("<span weight='bold'>Calendar:</span>");
  gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

  calendarcombobox = gtk_combo_box_new_text ();
  gtk_widget_show(calendarcombobox);
  gtk_box_pack_start (GTK_BOX (vbox), calendarcombobox, FALSE, FALSE, 0);
  
  
  label = gtk_label_new("<span weight='bold'>Tasks:</span>");
  gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

  todocombobox = gtk_combo_box_new_text ();
  gtk_widget_show(todocombobox);
  gtk_box_pack_start (GTK_BOX (vbox), todocombobox, FALSE, FALSE, 0);
  
  evo2sync->addressbookcombobox = addressbookcombobox;
  evo2sync->calendarcombobox = calendarcombobox;
  evo2sync->todocombobox = todocombobox;
  evo2sync->curaddressbooklist = NULL;
  evo2sync->curcalendarlist = NULL;
  evo2sync->curtodolist = NULL;
  
  return vbox;
}

void msync_evo2sync_set_config(MSyncPlugin* plugin, OSyncMember *member, const char *config)
{
	OSyncError* error = NULL;
	xmlDocPtr doc;
	xmlNodePtr cur;
	char *curaddressbookuri = NULL;
	char *curcalendaruri = NULL;
	char *curtodouri = NULL;
	
	doc = xmlParseMemory(config, strlen(config));
	if (!doc) {
		return;
	}
	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		xmlFreeDoc(doc);
		return;
	}
	if (xmlStrcmp(cur->name, (xmlChar*)"config")) {
		xmlFreeDoc(doc);
		return;
	}
	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		char *str = (char*)xmlNodeGetContent(cur);
		if (str) {
			if (!xmlStrcmp(cur->name, (const xmlChar *)"address_path"))
				curaddressbookuri = g_strdup(str);
			if (!xmlStrcmp(cur->name, (const xmlChar *)"calendar_path"))
				curcalendaruri = g_strdup(str);
			if (!xmlStrcmp(cur->name, (const xmlChar *)"tasks_path")) {
				curtodouri = g_strdup(str);	
			}
			xmlFree(str);
		}
		cur = cur->next;
	}
	xmlFreeDoc(doc);	

	combo_box_fill (member,
					"evo2_list_addressbooks",
					"No Addressbook",
					curaddressbookuri,
					((MSyncEvo2Sync *)plugin->_private)->addressbookcombobox,
					&(((MSyncEvo2Sync *)plugin->_private)->curaddressbooklist),
					&error);

	combo_box_fill (member,
					"evo2_list_calendars",
					"No Calendar",
					curcalendaruri,
					((MSyncEvo2Sync *)plugin->_private)->calendarcombobox,
					&(((MSyncEvo2Sync *)plugin->_private)->curcalendarlist),
					&error);

	combo_box_fill (member,
					"evo2_list_tasks",
					"No Todo",
					curtodouri,
					((MSyncEvo2Sync *)plugin->_private)->todocombobox,
					&(((MSyncEvo2Sync *)plugin->_private)->curtodolist),
					&error);

  	if(curaddressbookuri)
  		free(curaddressbookuri);
  	if(curcalendaruri)
  		free(curcalendaruri);
  	if(curtodouri)
  		free(curtodouri);
}

const char* msync_evo2sync_get_config(MSyncPlugin* plugin)
{
	xmlDocPtr doc;
	char *config;
	int size;
	doc = xmlNewDoc((xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, (xmlChar*)"config", NULL);
		
	xmlNewChild(doc->children, NULL, (xmlChar*)"address_path",
		(xmlChar *)combo_box_get_uri(((MSyncEvo2Sync *)plugin->_private)->addressbookcombobox, ((MSyncEvo2Sync *)plugin->_private)->curaddressbooklist));
	xmlNewChild(doc->children, NULL, (xmlChar*)"calendar_path",
		(xmlChar *)combo_box_get_uri(((MSyncEvo2Sync *)plugin->_private)->calendarcombobox, ((MSyncEvo2Sync *)plugin->_private)->curcalendarlist));
	xmlNewChild(doc->children, NULL, (xmlChar*)"tasks_path", 
		(xmlChar *)combo_box_get_uri(((MSyncEvo2Sync *)plugin->_private)->todocombobox, ((MSyncEvo2Sync *)plugin->_private)->curtodolist));

	combo_box_empty(((MSyncEvo2Sync *)plugin->_private)->addressbookcombobox, &(((MSyncEvo2Sync *)plugin->_private)->curaddressbooklist));
	combo_box_empty(((MSyncEvo2Sync *)plugin->_private)->calendarcombobox, &(((MSyncEvo2Sync *)plugin->_private)->curcalendarlist));
	combo_box_empty(((MSyncEvo2Sync *)plugin->_private)->todocombobox, &(((MSyncEvo2Sync *)plugin->_private)->curtodolist));
	
	xmlDocDumpFormatMemoryEnc(doc, (xmlChar **)&config, &size, NULL, 1);
	return config;
}
