#include "multisync.h"

extern MSyncEnv *env;

MSyncPair *msync_get_selected_pair(MSyncEnv *env) {
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	MSyncPair *pair = NULL;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW(lookup_widget(env->mainwindow, "syncpairlist")));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &pair, -1);
	}

	return(pair);
}

void msync_show_pairlist(MSyncEnv *env) {
  printf("show_pairlist\n");
  GtkListStore *pairlist = NULL;
  GtkTreeSelection *select;
  const char *columns[] = {NULL, NULL, NULL, NULL};
  int n;
  gboolean nonempty = (g_list_length(env->syncpairs) > 0);
  pairlist = g_object_get_data(G_OBJECT(env->mainwindow), "syncpairstore");
  gtk_widget_set_sensitive(lookup_widget(env->mainwindow, "editbutton"), nonempty);
  gtk_widget_set_sensitive(lookup_widget(env->mainwindow, "deletebutton"), nonempty);
  gtk_widget_set_sensitive(lookup_widget(env->mainwindow, "logbutton"), nonempty);
  gtk_widget_set_sensitive(lookup_widget(env->mainwindow, "syncnowbutton"), nonempty);
  gtk_list_store_clear(pairlist);
  
  select = gtk_tree_view_get_selection (GTK_TREE_VIEW(lookup_widget(env->mainwindow, "syncpairlist"))); 
  for (n = 0; n < g_list_length(env->syncpairs); n++) { 
    GtkTreeIter iter;
    MSyncPair *pair = g_list_nth_data(env->syncpairs, n);
    printf("Showing group %s\n", osync_group_get_name(pair->group));
    columns[0] = osync_group_get_name(pair->group);
    columns[1] = "Ready";
    gtk_list_store_append (pairlist, &iter);
    gtk_list_store_set(pairlist, &iter, 0, columns[0], 1, columns[1],
		       2, pair, 3, FALSE, -1);
    if (n == 0)
      gtk_tree_selection_select_iter(select, &iter);	 
  }
}
	
void msync_set_pairlist_status(MSyncPair *pair, const char *message, ...)
{
	int n;
	GtkListStore* pairlist;
  
	if (!env->mainwindow)
		return;
  
	va_list arglist;
	char status[4096];
	
	va_start(arglist, message);
	vsprintf(status, message, arglist);
	printf("setting status %s\n", status);

	if ((pairlist = g_object_get_data(G_OBJECT(env->mainwindow), "syncpairstore"))) {
		for (n = 0; n < g_list_length(env->syncpairs); n++) {
			if (g_list_nth_data(env->syncpairs,n) == pair) {
				GtkTreeIter iter;
				gtk_tree_model_iter_nth_child (GTK_TREE_MODEL(pairlist), &iter, NULL, n);
				gtk_list_store_set(pairlist, &iter, 1,  status, -1);
			}
		}
	}
	va_end(arglist);
}

/*void async_set_pairlist_status(sync_pair *pair, char *status, 
			       gboolean realaction) {
  if (pair->status)
    g_free(pair->status);
  pair->status = g_strdup(status);
  if (mainwindow) {
    set_pairlist_status_args *arg = g_malloc(sizeof(set_pairlist_status_args));
    g_assert(arg);
    arg->pair = pair;
    arg->realaction = realaction;
    g_idle_add(do_set_pairlist_status, arg);
  }
}*/

// Public plugin API function
/*void sync_set_pair_status(sync_pair *pair, char *status) {
  if (pair)
    async_set_pairlist_status(pair, status, FALSE);
}

typedef struct {
  sync_pair *pair;
  char *logstring;
  sync_log_type type;
} add_pairlist_log_args;

gboolean do_add_pairlist_log(gpointer data) {
  add_pairlist_log_args *arg = data;
  GList *last = g_list_last(arg->pair->log);
  gboolean done = FALSE;
  if (last) {
    sync_pair_log *log = last->data;
    if (log && log->logstring && !strcmp(log->logstring, arg->logstring)) {
      // Same as last time, replace
      g_free(log->logstring);
      log->logstring = arg->logstring;
      time(&log->timestamp);
      log->type = arg->type;
      log->repeat++;
      done = TRUE;
    }
  }
  if (!done) {
    // Append new string
    sync_pair_log *log = g_malloc0(sizeof(sync_pair_log));
    log->logstring = arg->logstring;
    time(&log->timestamp);
    log->type = arg->type;
    arg->pair->log = g_list_append(arg->pair->log, log);
    

    if (g_list_length(arg->pair->log) > 50) {
      // Shorten the list if too long.
      GList *first = g_list_first(arg->pair->log);
      sync_pair_log *log = first->data;
      if (log->logstring)
	g_free(log->logstring);
      g_free(log);
      arg->pair->log = g_list_remove(arg->pair->log, log);
    }
  }
  logwindow_show_log();
  g_free(arg);
  return(FALSE);
}

void async_add_pairlist_log(sync_pair *pair, char* logstring, 
			    sync_log_type type) {
  add_pairlist_log_args *arg = g_malloc(sizeof(add_pairlist_log_args));
  g_assert(arg);
  arg->pair = pair;
  arg->logstring = g_strdup(logstring);
  arg->type = type;
  g_idle_add(do_add_pairlist_log, arg);
}


void clear_pairlist_log(void) {
  sync_pair *pair = NULL;

  if (!logwindow || !get_selected_pair() )
    return;
  pair = get_selected_pair();
  
  while (pair->log) {
    GList *first = g_list_first(pair->log);
    sync_pair_log *log = first->data;
    if (log->logstring)
      g_free(log->logstring);
    g_free(log);
    pair->log = g_list_remove(pair->log, log);
  }
  logwindow_show_log();
}

void logwindow_show_log(void) {
  GtkListStore *loglist;
  sync_pair *pair = NULL;
  int len;
  int n;
  char *wintitle;
  GdkVisual *visual;
  GdkColormap *colormap;
  GdkColor green, red;

  if (!logwindow || !get_selected_pair())
    return;
  pair = get_selected_pair();
  wintitle = g_strdup_printf("%s - %s log", 
			     pair->localclient->longname?
			     pair->localclient->longname:
			     pair->localclient->shortname,
			     pair->remoteclient->longname?
			     pair->remoteclient->longname:
			     pair->remoteclient->shortname);
  gtk_window_set_title(GTK_WINDOW(logwindow), wintitle);
  g_free(wintitle);

  len = g_list_length(pair->log);
  loglist = g_object_get_data(G_OBJECT(logwindow), "logstore");
  gtk_list_store_clear(loglist);
  visual = gdk_visual_get_system();
  colormap = gdk_colormap_new(visual, FALSE);
  gdk_color_parse("green", &green);
  gdk_color_parse("red", &red);
  gdk_colormap_alloc_color (colormap, &green, FALSE, TRUE);
  gdk_colormap_alloc_color (colormap, &red, FALSE, TRUE);
  for (n = 0; n < len; n++) {
    GtkTreeIter iter;
    char *columns[3] = { NULL, NULL, NULL };
    char *d, *l;
    sync_pair_log *log = g_list_nth_data(pair->log, n);

    d = g_strdup(ctime(&log->timestamp));
    if (strlen(d) > 0 && d[strlen(d)-1] == '\n')
      d[strlen(d)-1] = 0; // Remove stupid newline
    columns[0] = d;
    if (log->repeat > 0)
      l = g_strdup_printf("%s (repeated %d times)", log->logstring, 
			   log->repeat+1);
    else
      l = g_strdup(log->logstring);
    columns[1] = l;
    gtk_list_store_append (loglist, &iter);
    gtk_list_store_set(loglist, &iter, 0, columns[0], 1, columns[1], -1);
    g_free(d);
    g_free(l);
  }
  gdk_colormap_free_colors(colormap, &green, 1);
  gdk_colormap_free_colors(colormap, &red, 1);
}*/

/*
void open_logwindow(void) {
  if (!logwindow) {
    GtkListStore *loglist = gtk_list_store_new(2,G_TYPE_STRING, G_TYPE_STRING);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    GtkTreeViewColumn *column;
    GtkTreeView *logtreeview;

    logwindow = create_logwindow();
    logtreeview = GTK_TREE_VIEW(lookup_widget(logwindow, "logtreeview"));  
    gtk_tree_view_set_model(logtreeview, GTK_TREE_MODEL(loglist));
    g_object_set_data(G_OBJECT(logwindow), "logstore", loglist);
    column = gtk_tree_view_column_new_with_attributes("Time",
						      renderer, 
						      "text", 0, 
						      NULL);
    gtk_tree_view_column_set_min_width(column, 100);
    gtk_tree_view_column_set_reorderable (column, TRUE);
    gtk_tree_view_column_set_clickable (column, TRUE);
    gtk_tree_view_append_column (logtreeview, column);
    
    column = gtk_tree_view_column_new_with_attributes("Log",
						      renderer, 
						      "text", 1, 
						      NULL);
    gtk_tree_view_column_set_min_width(column, 300);
    gtk_tree_view_column_set_clickable (column, TRUE);
    gtk_tree_view_column_set_reorderable (column, TRUE);
    gtk_tree_view_append_column (logtreeview, column);

    gtk_widget_show(logwindow);
  }
  logwindow_show_log();
}

void close_logwindow(void) {
  if (logwindow) {
    gtk_widget_destroy(logwindow);
    logwindow = NULL;
  }
}

gboolean sync_set_multisync_status(gpointer data) {
  if (mainwindow) {
    gnome_appbar_push(GNOME_APPBAR(lookup_widget(mainwindow, "appbar")),
		      statusstring);
  }
  return(FALSE);
}

void async_set_multisync_status(char *status) {
  if (status) {
    if (statusstring)
      g_free(statusstring);
    statusstring = g_strdup(status);
  }
  gtk_idle_add(sync_set_multisync_status, NULL);
}


void mainwindow_save_size(void) {
  if (mainwindow) {
    GConfClient *gconf = gconf_client_get_default ();
    int w, h;
    gtk_window_get_size(GTK_WINDOW(mainwindow), &w, &h);
    gconf_client_set_int(gconf, SYNC_GCONF_PATH"/main_width", w, NULL);
    gconf_client_set_int(gconf, SYNC_GCONF_PATH"/main_height", h, NULL);
    g_object_unref(gconf);
  }
}

void sync_pair_auto_toggled(GtkCellRendererToggle *cellrenderertoggle,
			    gchar *path,
			    gpointer user_data) {
  GtkTreeSelection *select;
  GtkTreeIter iter;
  GtkTreeModel *model;
  sync_pair *pair = NULL;
  gboolean autosync = FALSE;
  GtkListStore *pairlist = user_data;

  if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(pairlist), &iter,
					  path)) {
    gtk_tree_model_get (GTK_TREE_MODEL(pairlist), 
			&iter, 2, &pair, 3, &autosync, -1);
    autosync = !autosync;
    pair->manualonly = !autosync;
    gtk_list_store_set(pairlist, &iter, 3, autosync, -1);
    save_syncpair(pair);
    sync_settings_changed(pair); // Tell the thread that we have changed
  }
}

static void sync_resync_gconf_notify (GConfClient *gconf,
				      guint        cnxn_id,
				      GConfEntry  *entry,
				      gpointer     user_data)
{
  GConfValue  *value;
  gboolean     checked;
  
  if (entry && !strcmp(gconf_entry_get_key(entry), 
		       SYNC_GCONF_PATH"/showresync")) {
    value = gconf_entry_get_value (entry);
    checked = gconf_value_get_bool (value);
    if (checked)
      gtk_widget_show(lookup_widget(mainwindow, "resyncnowbutton"));
    else
      gtk_widget_hide(lookup_widget(mainwindow, "resyncnowbutton"));
  }
}*/

gboolean msync_open_mainwindow(gpointer data) {
	printf("msync_open_mainwindow()\n");
	MSyncEnv *env = (MSyncEnv *)data;
  GtkCheckMenuItem *menuitem;
  gboolean notray = FALSE, nogui = FALSE, showresync = FALSE;
  GtkListStore *pairlist = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER,G_TYPE_BOOLEAN);
  GtkTreeView *syncpairlist;
  GtkTreeSelection *select;
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
  GtkCellRenderer *boolrenderer = gtk_cell_renderer_toggle_new();
  GtkTreeViewColumn *column;
  gnome_app_enable_layout_config(GNOME_APP(env->mainwindow), TRUE);

  syncpairlist = GTK_TREE_VIEW(lookup_widget(env->mainwindow, "syncpairlist"));  
  gtk_tree_view_set_model(syncpairlist, GTK_TREE_MODEL(pairlist));
  g_object_set_data(G_OBJECT(env->mainwindow), "syncpairstore", pairlist);
  /*g_signal_connect (G_OBJECT (boolrenderer), "toggled",
		    G_CALLBACK (sync_pair_auto_toggled),
		    pairlist);*/
  column = gtk_tree_view_column_new_with_attributes("Auto",
						    boolrenderer, 
						    "active", 3, 
						    NULL);
  gtk_tree_view_column_set_min_width(column, 20);
  gtk_tree_view_append_column (syncpairlist, column);

  column = gtk_tree_view_column_new_with_attributes("Synchronization pair",
						    renderer, 
						    "text", 0, 
						    NULL);
  gtk_tree_view_column_set_min_width(column, 200);
  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_append_column (syncpairlist, column);

  column = gtk_tree_view_column_new_with_attributes("Status",
						    renderer, 
						    "text", 1, 
						    NULL);
  gtk_tree_view_column_set_min_width(column, 150);
  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_append_column (syncpairlist, column);


  select = gtk_tree_view_get_selection (GTK_TREE_VIEW (syncpairlist));
  gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
  /*g_signal_connect (G_OBJECT (select), "changed",
		    G_CALLBACK (msync_pair_selected),
		    env);*/
  
  //gtk_window_resize(GTK_WINDOW(env->mainwindow), w, h);
  
  gtk_widget_show (env->mainwindow);

  msync_show_pairlist(env);

  menuitem =  GTK_CHECK_MENU_ITEM(lookup_widget(env->mainwindow, "showtrayitem"));

  gtk_check_menu_item_set_active(menuitem, !notray);
  menuitem =  GTK_CHECK_MENU_ITEM(lookup_widget(env->mainwindow, 
						"starthiddenitem"));

  gtk_check_menu_item_set_active(menuitem, nogui);
  menuitem =  GTK_CHECK_MENU_ITEM(lookup_widget(env->mainwindow, 
						"showresyncitem"));

  gtk_check_menu_item_set_active(menuitem, showresync);
  if (showresync)
    gtk_widget_show(lookup_widget(env->mainwindow, "resyncnowbutton"));
  else
    gtk_widget_hide(lookup_widget(env->mainwindow, "resyncnowbutton"));
  
  return(FALSE);
}

/*
void show_common_object_types(sync_plugin *loc, sync_plugin *rem) {
  sync_object_type objtype;

  objtype = SYNC_OBJECT_TYPE_ANY;
  if (loc)
    objtype &= (sync_object_type) CALL_PLUGIN(loc, "object_types", ());
  if (rem)
    objtype &= (sync_object_type) CALL_PLUGIN(rem, "object_types", ());
  optionpair->object_types &= objtype;
  if (objtype & SYNC_OBJECT_TYPE_CALENDAR) {
    gtk_widget_show(lookup_widget(optionwindow, "calendarcheck"));
    if (optionpair->object_types & SYNC_OBJECT_TYPE_CALENDAR)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow,"calendarcheck")), 1);
  } else
    gtk_widget_hide(lookup_widget(optionwindow, "calendarcheck"));
    
  if (objtype & SYNC_OBJECT_TYPE_PHONEBOOK) {
    gtk_widget_show(lookup_widget(optionwindow, "addressbookcheck"));
    if (optionpair->object_types & SYNC_OBJECT_TYPE_PHONEBOOK)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow,"addressbookcheck")), 1);
  } else
    gtk_widget_hide(lookup_widget(optionwindow, "addressbookcheck"));

  if (objtype & SYNC_OBJECT_TYPE_TODO) {
    gtk_widget_show(lookup_widget(optionwindow, "todocheck"));
    if (optionpair->object_types & SYNC_OBJECT_TYPE_TODO)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow,"todocheck")), 1);
  } else
    gtk_widget_hide(lookup_widget(optionwindow, "todocheck"));

  if (!objtype)
    gtk_widget_show(lookup_widget(optionwindow, "nocommontypelabel"));
  else
    gtk_widget_hide(lookup_widget(optionwindow, "nocommontypelabel"));
}
*/

void localoption_selected(GtkMenuItem *menuitem, gpointer user_data) {
  
  /*sync_plugin *plugin;
  sync_pair *pair;
  GtkButton *localedit;
  char *info;
  pair = optionpair;*/
  OSyncPlugin *plugin = (OSyncPlugin*) gtk_object_get_data (GTK_OBJECT (menuitem), "plugin");
  printf("plugin %s selected\n", osync_plugin_get_name(plugin));
  
	MSyncPair *pair = gtk_object_get_data(GTK_OBJECT(env->optionwindow), "pair");
	OSyncMember *member = osync_group_nth_member(pair->group, 0);
	
	OSyncError *error = NULL;
  	if (!osync_member_instance_plugin(member, osync_plugin_get_name(plugin), &error)) {
		printf("Unable to instance plugin with name %s: %s\n", osync_plugin_get_name(plugin), osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	  
	if (osync_member_has_configuration(member))
		gtk_widget_set_sensitive(lookup_widget(env->optionwindow, "localedit"), TRUE);
	else
		gtk_widget_set_sensitive(lookup_widget(env->optionwindow, "localedit"), FALSE);
  
  /*localedit = GTK_BUTTON();
  pair->localclient = plugin;
  if (pair->localname)
    g_free(pair->localname);
  pair->localname = g_strdup(plugin->shortname);
  if (CHECK_PLUGIN(plugin,"open_option_window"))
    gtk_widget_set_sensitive(GTK_WIDGET(localedit), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(localedit), FALSE);
  show_common_object_types(pair->localclient, pair->remoteclient);
  info = CALL_PLUGIN(plugin, "plugin_info", ());
  gtk_label_set_text(GTK_LABEL(lookup_widget(optionwindow, "localinfolabel")),
		     info?info:"No information on the chosen plugin.");*/
}

void remoteoption_selected(GtkMenuItem *menuitem, gpointer user_data) {
	OSyncPlugin *plugin = (OSyncPlugin*) gtk_object_get_data (GTK_OBJECT (menuitem), "plugin");
	printf("plugin2 %s selected\n", osync_plugin_get_name(plugin));

	MSyncPair *pair = gtk_object_get_data(GTK_OBJECT(env->optionwindow), "pair");
	OSyncMember *member = osync_group_nth_member(pair->group, 1);
	
	OSyncError *error = NULL;
  	if (!osync_member_instance_plugin(member, osync_plugin_get_name(plugin), &error)) {
		printf("Unable to instance plugin with name %s: %s\n", osync_plugin_get_name(plugin), osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
	printf("remote\n");
	if (osync_member_has_configuration(member)) {
		printf("true\n");
		gtk_widget_set_sensitive(lookup_widget(env->optionwindow, "remoteedit"), TRUE);
	} else
		gtk_widget_set_sensitive(lookup_widget(env->optionwindow, "remoteedit"), FALSE);
	
  /*
  sync_plugin *plugin;
  sync_pair *pair;
  char *info;
  GtkButton *remoteedit;
  pair = optionpair;
  plugin = (sync_plugin*) gtk_object_get_data (GTK_OBJECT (menuitem), "plugin");
  remoteedit = GTK_BUTTON();
  pair->remoteclient = plugin;
  if (pair->remotename)
    g_free(pair->remotename);
  pair->remotename = g_strdup(plugin->shortname);
  if (CHECK_PLUGIN(plugin,"open_option_window"))
    gtk_widget_set_sensitive(GTK_WIDGET(remoteedit), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(remoteedit), FALSE);
  show_common_object_types(pair->localclient, pair->remoteclient);
  info = CALL_PLUGIN(plugin, "plugin_info", ());
  gtk_label_set_text(GTK_LABEL(lookup_widget(optionwindow, "remoteinfolabel")),
		     info?info:"No information on the chosen plugin.");*/
}

/*
int syncintervals[] = { 10, 30, 60, 2*60, 5*60, 10*60, 30*60, 3600,
			2*3600, 4*3600, 8*3600, 12*3600, 24*3600, 0, -1 };
int dwelltimes[] = { 0, 1, 2, 5, 10, 20, 30, 60, 2*60, -1 };
int reconnintervals[] = { 1, 2, 5, 10, 20, 30, 60, 2*60, 5*60, 10*60, 
			  30*60, 3600, -1 };

char *interval_to_string(int interval) {
  char *ret = NULL;
  if (interval <= 60)
    ret = g_strdup_printf("%d secs", interval);
  else if (interval <= 3600)
    ret = g_strdup_printf("%d mins", interval/60);
  else if (interval <= 24*3600)
    ret = g_strdup_printf("%d hours", interval/3600);
  else
    ret = g_strdup_printf("%d days", interval/3600/24);
  return(ret);
}

int index_to_time(int* intervals, int index) {
  int t = 0;
  while (intervals[t]>=0 && t < index)
    t++;
  if (intervals[t]>=0)
    return(intervals[t]);
  return(intervals[t-1]);
}

int time_to_index(int* intervals, int secs) {
  int t = 0;
  while (intervals[t]>=0 && ((secs > 0 && intervals[t] < secs) ||
			     (secs == 0 && intervals[t] != secs)))
    t++;
  if (intervals[t]>=0)
    return(t);
  return(t-1);
}

void syncinterval_changed(GtkAdjustment *adjustment) {
  int val = (int) adjustment->value;
  GtkLabel *label;
  char *text;

  label = GTK_LABEL(gtk_object_get_data(GTK_OBJECT(optionwindow), 
					"syncintervallabel"));
  optionpair->syncinterval = index_to_time(syncintervals, val);
  if (optionpair->syncinterval)
    text = interval_to_string(optionpair->syncinterval);
  else
    text = g_strdup("On change");
  gtk_label_set_text(label, text);
  g_free(text);
}

void dwell_changed(GtkAdjustment *adjustment) {
  int val = (int) adjustment->value;
  GtkLabel *label;
  char *text;

  label = GTK_LABEL(gtk_object_get_data(GTK_OBJECT(optionwindow), 
					"dwelllabel"));
  optionpair->dwelltime = index_to_time(dwelltimes, val);
  text = interval_to_string(optionpair->dwelltime);
  gtk_label_set_text(label, text);
  g_free(text);
}

void reconninterval_changed(GtkAdjustment *adjustment) {
  int val = (int) adjustment->value;
  GtkLabel *label;
  char *text;

  label = GTK_LABEL(gtk_object_get_data(GTK_OBJECT(optionwindow), 
					"reconnintervallabel"));
  optionpair->reconninterval = index_to_time(reconnintervals, val);
  text = interval_to_string(optionpair->reconninterval);
  gtk_label_set_text(label, text);
  g_free(text);
}*/

void msync_ok_syncpairwindow(void)
{
  GtkWidget *button;
  /*if (pluginwindow)
    return;*/
  
  MSyncPair *pair = gtk_object_get_data (GTK_OBJECT(env->optionwindow), "pair");
  
  // Fetch data from the window
 /*if (optionpair->displayname)
    g_free(optionpair->displayname);*/
  osync_group_set_name(pair->group, gtk_entry_get_text(GTK_ENTRY(lookup_widget(env->optionwindow, "displaynameentry"))));
  /*if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow, "duplicatemanualradio"))))
    optionpair->duplicate_mode = SYNC_DUPLICATE_ACTION_NONE;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow, "duplicatefirstradio"))))
    optionpair->duplicate_mode = SYNC_DUPLICATE_ACTION_KEEPFIRST;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow, "duplicatesecondradio"))))
    optionpair->duplicate_mode = SYNC_DUPLICATE_ACTION_KEEPSECOND;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow, "duplicatebothradio"))))
    optionpair->duplicate_mode = SYNC_DUPLICATE_ACTION_KEEPBOTH;*/
  /*
  if (optionpair->original) {
    sync_pair *original = (sync_pair*) optionpair->original;
    apply_syncpair(original, optionpair);
    pthread_create(&original->syncthread, NULL, sync_main, original);
  } else {
    // New pair
    syncpairlist = g_list_append(syncpairlist, optionpair);
    pthread_create(&optionpair->syncthread, NULL, sync_main, optionpair);
    save_syncpair(optionpair);
  }*/
  
  OSyncError *error = NULL;
  if (!osync_group_save(pair->group, &error)) {
  	printf("Unable to save group: %s\n", osync_error_print(&error));
  } else {
	  if (!g_list_find(env->syncpairs, pair)) {
	  	osync_env_append_group(env->osync, pair->group);
	  	env->syncpairs = g_list_append(env->syncpairs, pair);
	  }
  }
  
  gtk_widget_destroy(env->optionwindow);
  env->optionwindow = NULL;
  button = lookup_widget(env->mainwindow,"editbutton");
  gtk_widget_set_sensitive(button, TRUE);
  button = lookup_widget(env->mainwindow,"newbutton");
  gtk_widget_set_sensitive(button, TRUE);
  button = lookup_widget(env->mainwindow,"deletebutton");
  gtk_widget_set_sensitive(button, TRUE);
  button = lookup_widget(env->mainwindow,"logbutton");
  gtk_widget_set_sensitive(button, TRUE);
  button = lookup_widget(env->mainwindow,"syncnowbutton");
  gtk_widget_set_sensitive(button, TRUE);
  msync_show_pairlist(env);
  msync_start_groups();
}

void msync_cancel_syncpairwindow(void)
{
	GtkWidget *button;
	MSyncPair *pair = gtk_object_get_data (GTK_OBJECT(env->optionwindow), "pair");
	
	if (!g_list_find(env->syncpairs, pair)) {
		//New pair
		osync_group_free(pair->group);
		msync_pair_free(pair);
	}

	gtk_widget_destroy(env->optionwindow);
	env->optionwindow = NULL;
	button = lookup_widget(env->mainwindow,"editbutton");
	gtk_widget_set_sensitive(button, TRUE);
	button = lookup_widget(env->mainwindow,"newbutton");
	gtk_widget_set_sensitive(button, TRUE);
	button = lookup_widget(env->mainwindow,"deletebutton");
	gtk_widget_set_sensitive(button, TRUE);
	button = lookup_widget(env->mainwindow,"logbutton");
	gtk_widget_set_sensitive(button, TRUE);
	button = lookup_widget(env->mainwindow,"syncnowbutton");
	gtk_widget_set_sensitive(button, TRUE);
}

void msync_synchronize(void)
{
	MSyncPair *pair = msync_get_selected_pair(env);
	if (pair) {
		if (pair->error) {
			msync_show_msg_warn("Unable to synchronize: There was a error during initialization");
			return;
		}
		
		pair->read = 0;
		pair->written = 0;
		OSyncError *error = NULL;
		if (!osengine_synchronize(pair->engine, &error)) {
			printf("Error while starting synchronization: %s\n", osync_error_print(&error));
			msync_set_pairlist_status(pair, "Error starting sync");
			osync_error_free(&error);
			return;
		}
		msync_set_pairlist_status(pair, "Syncing");
	}
}

/*
void resync_now() {
  GtkListStore* pairlist;
  int row;
  sync_pair *pair;

  pairlist = g_object_get_data(G_OBJECT(mainwindow), "syncpairstore");
  pair = get_selected_pair();
  if (pair) {
    if (sync_okcanceldialog("Are you sure you want to re-synchornize all entries?\nThis may take a long time and may potentially duplicate ALL of your entries.", TRUE))
      sync_force_resync(pair);
  }
}*/

void msync_delete_pair(void)
{
  GtkListStore* pairlist;
  MSyncPair *pair;

  if (!msync_okcanceldialog("Are you sure you want to delete the selected pair?", TRUE))
    return;

  pairlist = g_object_get_data(G_OBJECT(env->mainwindow), "syncpairstore");
  pair = msync_get_selected_pair(env);
  if (pair) {
  	OSyncError *error = NULL;
    if (!osync_group_delete(pair->group, &error)) {
		printf("Unable to delete pair: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
    env->syncpairs = g_list_remove(env->syncpairs, pair);
    msync_pair_free(pair);
    msync_show_pairlist(env);
  }
}

void msync_edit_pair(void) {
	//GtkListStore *pairlist = NULL;

	//pairlist = g_object_get_data(G_OBJECT(env->mainwindow), "syncpairstore");
	MSyncPair *pair = msync_get_selected_pair(env);
	if (!pair)
		return;
	if (pair->error && pair->error != OSYNC_ERROR_MISCONFIGURATION) {
		msync_show_msg_warn("Unable to edit. There was a error during initialization");
		return;
	}
  // Turn off sync thread before editing
  /*if (pair->original && ((sync_pair *) pair->original)->thread_running) {
    async_set_pairlist_status((sync_pair *) pair->original, "Exiting...", 0);
    sync_quit((sync_pair *) pair->original, do_open_syncpairwindow, pair);
    ((sync_pair *) pair->original)->thread_running = FALSE;
  } else*/
	msync_open_syncpairwindow(pair);
}  

/*
sync_filter *filter_find(sync_object_type type, sync_direction dir) {
  sync_filter *filter = NULL;
  int n;
  for (n = 0; !filter && n < g_list_length(optionpair->filters); n++) {
    sync_filter *f = g_list_nth_data(optionpair->filters, n);
    if (f->type == type && (f->dir&dir))
      filter = f;
  }
  return(filter);
}*/

void msync_new_pair()
{
	OSyncError *error = NULL;
	GtkWidget *selected = NULL;
	
	if (osync_env_num_plugins(env->osync) == 0) {
		msync_show_msg_warn("No plugins found! You have to install some plugins.");
		return;
	}
	
	MSyncPair *pair = msync_pair_new();
	pair->group = osync_group_new(env->osync);
	OSyncMember *leftmember = osync_member_new(pair->group);
	OSyncMember *rightmember = osync_member_new(pair->group);
	
	msync_open_syncpairwindow(pair);
	
	GtkOptionMenu *localoption = GTK_OPTION_MENU(lookup_widget(env->optionwindow, "localoption"));
	GtkMenu *localmenu = (GtkMenu *)gtk_option_menu_get_menu(localoption);
	selected = gtk_menu_get_active(localmenu);
	OSyncPlugin *leftplugin = gtk_object_get_data(GTK_OBJECT(selected), "plugin");
	if (!osync_member_instance_plugin(leftmember, osync_plugin_get_name(leftplugin), &error)) {
		printf("Unable to instance plugin with name %s: %s\n", osync_plugin_get_name(leftplugin), osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
	GtkOptionMenu *remoteoption = GTK_OPTION_MENU(lookup_widget(env->optionwindow, "remoteoption"));
	GtkMenu *remotemenu = (GtkMenu *)gtk_option_menu_get_menu(remoteoption);
	selected = gtk_menu_get_active(remotemenu);
	OSyncPlugin *rightplugin = gtk_object_get_data(GTK_OBJECT(selected), "plugin");
	if (!osync_member_instance_plugin(rightmember, osync_plugin_get_name(rightplugin), &error)) {
		printf("Unable to instance plugin with name %s: %s\n", osync_plugin_get_name(rightplugin), osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
}

void
on_newbutton_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	msync_new_pair();
}

/*
void filter_type_selected(GtkMenuItem *menuitem, gpointer user_data) {
  sync_object_type type = (sync_object_type) user_data;
  sync_filter *filter = NULL;
  activefiltertype = type;
  filter = filter_find(activefiltertype, activefilterdir);
  filter_show(filter);
}

void filter_rule_selected(GtkMenuItem *menuitem, gpointer user_data) {
  sync_rule rule = (sync_rule) user_data;
  sync_filter *filter = NULL;
  filter = filter_find(activefiltertype, activefilterdir);
  if (!filter)
    return;
  filter->rule = rule;
  gtk_widget_set_sensitive(lookup_widget(optionwindow, "filterfieldmenu"),
			   rule!=SYNC_RULE_ALL && rule!=SYNC_RULE_NONE);
  gtk_widget_set_sensitive(lookup_widget(optionwindow, "filterdataentry"),
			   rule!=SYNC_RULE_ALL && rule!=SYNC_RULE_NONE);
}

void filter_dir_selected(sync_direction dir) {
  sync_filter *filter = NULL;
  activefilterdir = dir;
  filter = filter_find(activefiltertype, activefilterdir);
  filter_show(filter);
}

void filter_show(sync_filter *filter) {
  GtkOptionMenu *om = GTK_OPTION_MENU(lookup_widget(optionwindow, "filterrulemenu"));
  if (!filter)
    return;
  switch (filter->rule) {
  case SYNC_RULE_ALL:
    gtk_option_menu_set_history(om, 0);
    break;
  case SYNC_RULE_ONLYIF:
    gtk_option_menu_set_history(om, 1);
    break;
  case SYNC_RULE_ALLBUT:
    gtk_option_menu_set_history(om, 2);
    break;
  case SYNC_RULE_NONE:
    gtk_option_menu_set_history(om, 3);
    break;
  }
  filter_rule_selected(NULL, (gpointer) filter->rule);
  gtk_entry_set_text(GTK_ENTRY(lookup_widget(optionwindow, "filterdataentry")),
		     filter->data?filter->data:"");
}

void filter_data_changed(char *data) {
  sync_filter *filter = NULL;
  int n;
  for (n = 0; !filter && n < g_list_length(optionpair->filters); n++) {
    sync_filter *f = g_list_nth_data(optionpair->filters, n);
    if (f->type == activefiltertype && (f->dir&activefilterdir))
      filter = f;
  }
  if (!filter)
    return;
  if (filter->data)
    g_free(filter->data);
  filter->data = g_strdup(data);
}*/

gboolean msync_open_syncpairwindow(gpointer data) {
  GtkWidget *button;
  MSyncPair *pair = data;
  env->optionwindow = create_syncpairwin();
  gtk_object_set_data (GTK_OBJECT(env->optionwindow), "pair", pair);
  
  /*if (pair->original)
    async_set_pairlist_status((sync_pair *) pair->original, 
			      "Editing options...", 0);*/
  button = lookup_widget(env->mainwindow,"editbutton");
  gtk_widget_set_sensitive(button, FALSE);
  button = lookup_widget(env->mainwindow,"newbutton");
  gtk_widget_set_sensitive(button, FALSE);
  button = lookup_widget(env->mainwindow,"deletebutton");
  gtk_widget_set_sensitive(button, FALSE);
  button = lookup_widget(env->mainwindow,"logbutton");
  gtk_widget_set_sensitive(button, FALSE);
  button = lookup_widget(env->mainwindow,"syncnowbutton");
  gtk_widget_set_sensitive(button, FALSE);
  
	if (osync_group_get_name(pair->group)) {
		GtkEntry *displayname = GTK_ENTRY(lookup_widget(env->optionwindow, "displaynameentry"));
		gtk_entry_set_text(displayname, osync_group_get_name(pair->group));
	}
  
 /* if (pair) {
    if (!optionwindow) {*/
      //char *filename;
      int n;
      GtkMenu *localmenu, *remotemenu;
      GtkOptionMenu *localoption, *remoteoption;

     // optionpair = pair;
      //optionwindow = create_syncpairwin();
      

     // filename = g_strdup_printf("%s/%s", PACKAGE_DATA_DIR, 
	//			 "pixmaps/multisync.png");
      //gnome_window_icon_set_from_file(GTK_WINDOW(optionwindow), filename);
      //g_free(filename);

      localmenu = GTK_MENU(gtk_menu_new());
      remotemenu = GTK_MENU(gtk_menu_new());
      printf("about to show plugins\n");
      GtkMenuItem *localseli = NULL, *remoteseli = NULL;
      int localseln = 0, remoteseln = 0;
      for (n = 0; n < osync_env_num_plugins(env->osync); n++) {
	OSyncPlugin* plugin = osync_env_nth_plugin(env->osync, n);
	 printf("adding plugin %s\n", osync_plugin_get_name(plugin));
	GtkMenuItem *localmi, *remotemi;
	localmi = 
	  GTK_MENU_ITEM(gtk_menu_item_new_with_label(osync_plugin_get_name(plugin)));
	remotemi = 
	  GTK_MENU_ITEM(gtk_menu_item_new_with_label(osync_plugin_get_name(plugin)));
	gtk_signal_connect (GTK_OBJECT (localmi), "activate", GTK_SIGNAL_FUNC (localoption_selected), NULL);
	gtk_signal_connect (GTK_OBJECT (remotemi), "activate", GTK_SIGNAL_FUNC (remoteoption_selected), NULL);
	gtk_object_set_data (GTK_OBJECT (localmi), "plugin", plugin);
	gtk_object_set_data (GTK_OBJECT (remotemi), "plugin", plugin);
	gtk_menu_append(localmenu, GTK_WIDGET(localmi));
	gtk_menu_append(remotemenu, GTK_WIDGET(remotemi));
	OSyncMember *leftmember = osync_group_nth_member(pair->group, 0);
	OSyncMember *rightmember = osync_group_nth_member(pair->group, 1);
	if (osync_member_get_plugin(leftmember) && !strcmp(osync_plugin_get_name(plugin), osync_member_get_pluginname(leftmember))) {
	  //gtk_menu_set_active (localmenu, n);
	  localseln = n;
	  localseli = localmi;
	}
	if (osync_member_get_plugin(rightmember) && !strcmp(osync_plugin_get_name(plugin), osync_member_get_pluginname(rightmember))) {
	  //gtk_menu_set_active (remotemenu, n);
	  remoteseli = remotemi;
	  remoteseln = n;
	}
	
		if (n == 0) {
			localseli = localmi;
			remoteseli = remotemi;
	  		remoteseln = n;
	  		localseln = n;
		}
	
      }
	
	if (localseli) {
		localoption_selected(localseli, NULL);
		gtk_menu_item_activate(localseli);
		gtk_menu_set_active (localmenu, localseln);
	}
	if (remoteseli) {
		remoteoption_selected(remoteseli, NULL);
		gtk_menu_item_activate(remoteseli);
		gtk_menu_set_active (remotemenu, remoteseln);
	}
	
      localoption = 
	GTK_OPTION_MENU(gtk_object_get_data(GTK_OBJECT(env->optionwindow), 
				      "localoption"));
      remoteoption = 
	GTK_OPTION_MENU(gtk_object_get_data(GTK_OBJECT(env->optionwindow), 
				      "remoteoption"));
      g_assert(localoption && remoteoption);
      gtk_widget_show_all(GTK_WIDGET(localmenu));
      gtk_widget_show_all(GTK_WIDGET(remotemenu));
      gtk_option_menu_set_menu(localoption, GTK_WIDGET(localmenu));
      gtk_option_menu_set_menu(remoteoption, GTK_WIDGET(remotemenu));
/*
      {
	// Get the sync interval scale to work
	GtkAdjustment *syncintervaladj;
	GtkHScale *syncintervalscale;
	syncintervaladj = 
	  GTK_ADJUSTMENT(gtk_adjustment_new((float) time_to_index(syncintervals,pair->syncinterval), 0.0, 14.0, 1.0, 1.0, 1.0));
	syncintervalscale = 
	  GTK_HSCALE(gtk_object_get_data(GTK_OBJECT(optionwindow), 
					  "syncintervalscale"));
	gtk_object_set_data(GTK_OBJECT(optionwindow), "syncintervaladj",
			    syncintervaladj);
	gtk_object_set(GTK_OBJECT(syncintervalscale), "adjustment",
		       syncintervaladj, NULL);
	gtk_signal_connect (GTK_OBJECT (syncintervaladj), "value-changed",
			    GTK_SIGNAL_FUNC (syncinterval_changed),
			    NULL);
	gtk_adjustment_value_changed(syncintervaladj);
      }
	
      {
	// Get the dwell time scale to work
	GtkAdjustment *dwelladj;
	GtkHScale *dwellscale;
	dwelladj = 
	  GTK_ADJUSTMENT(gtk_adjustment_new((float) time_to_index(dwelltimes,pair->dwelltime), 0.0, 9.0, 1.0, 1.0, 1.0));
	dwellscale = 
	  GTK_HSCALE(gtk_object_get_data(GTK_OBJECT(optionwindow), 
					  "dwellscale"));
	gtk_object_set_data(GTK_OBJECT(optionwindow), "dwelladj",
			    dwelladj);
	gtk_object_set(GTK_OBJECT(dwellscale), "adjustment",
		       dwelladj, NULL);
	gtk_signal_connect (GTK_OBJECT (dwelladj), "value-changed",
			    GTK_SIGNAL_FUNC (dwell_changed),
			    NULL);
	gtk_adjustment_value_changed(dwelladj);
      }

      {
	// Get the reconn interval scale to work
	GtkAdjustment *reconnintervaladj;
	GtkHScale *reconnintervalscale;
	reconnintervaladj = 
	  GTK_ADJUSTMENT(gtk_adjustment_new((float) time_to_index(reconnintervals,pair->reconninterval), 0.0, 12.0, 1.0, 1.0, 1.0));
	reconnintervalscale = 
	  GTK_HSCALE(gtk_object_get_data(GTK_OBJECT(optionwindow), 
					  "reconnintervalscale"));
	gtk_object_set_data(GTK_OBJECT(optionwindow), "reconnintervaladj",
			    reconnintervaladj);
	gtk_object_set(GTK_OBJECT(reconnintervalscale), "adjustment",
		       reconnintervaladj, NULL);
	gtk_signal_connect (GTK_OBJECT (reconnintervaladj), "value-changed",
			    GTK_SIGNAL_FUNC (reconninterval_changed),
			    NULL);
	gtk_adjustment_value_changed(reconnintervaladj);
      }

      // Set the sound options
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow,"syncsoundcheck")), optionpair->playsyncsound);
      if (optionpair->syncsound)
	gtk_entry_set_text(GTK_ENTRY(lookup_widget(optionwindow, "syncsoundentry")), optionpair->syncsound);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow,"welcomesoundcheck")), optionpair->playwelcomesound);
      if (optionpair->welcomesound)
	gtk_entry_set_text(GTK_ENTRY(lookup_widget(optionwindow, "welcomesoundentry")), optionpair->welcomesound);
      if (optionpair->displayname)
	gtk_entry_set_text(GTK_ENTRY(lookup_widget(optionwindow, "displaynameentry")), optionpair->displayname);
      if (optionpair->duplicate_mode == SYNC_DUPLICATE_ACTION_NONE)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow, "duplicatemanualradio")), TRUE);
      if (optionpair->duplicate_mode == SYNC_DUPLICATE_ACTION_KEEPFIRST)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow, "duplicatefirstradio")), TRUE);
      if (optionpair->duplicate_mode == SYNC_DUPLICATE_ACTION_KEEPSECOND)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow, "duplicatesecondradio")), TRUE);
      if (optionpair->duplicate_mode == SYNC_DUPLICATE_ACTION_KEEPBOTH)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(optionwindow, "duplicatebothradio")), TRUE);


      // Set the filter options
      {
	int i;
	GtkMenu *menu;
	GtkMenuItem *mi;
	// Create one filter per type if they don't exist
	menu = GTK_MENU(gtk_menu_new());
	for (i = 0; i < 3; i++) {
	  int k = 0;
	  sync_object_type type = (i==0?SYNC_OBJECT_TYPE_CALENDAR:
				   (i==1?SYNC_OBJECT_TYPE_PHONEBOOK:
				    (i==2?SYNC_OBJECT_TYPE_TODO:0)));
	  for (k = 0; k < 2; k++) {
	    sync_direction dir = (k?SYNC_DIR_REMOTETOLOCAL:
				  SYNC_DIR_LOCALTOREMOTE);
	    
	    gboolean found = FALSE;
	    for (n = 0; !found && n<g_list_length(optionpair->filters); n++) {
	      sync_filter *filter = g_list_nth_data(optionpair->filters, n);
	      if (filter->type == type && (filter->dir & dir)) {
		found = TRUE;
		if (filter->dir != dir) {
		  // If two-way filter, split into two filters
		  sync_filter *filter2 = g_malloc0(sizeof(sync_filter));
		  filter->dir = SYNC_DIR_LOCALTOREMOTE;
		  filter2->dir = SYNC_DIR_REMOTETOLOCAL;
		  filter2->type = filter->type;
		  filter2->rule = filter->rule;
		  if (filter->field)
		    filter2->field = g_strdup(filter->field);
		  filter2->data = g_strdup(filter->data);
		  optionpair->filters = g_list_append(optionpair->filters,
						      filter2);
		}
	      }
	    }
	    if (!found) {
	      sync_filter *filter = g_malloc0(sizeof(sync_filter));
	      filter->dir = dir;
	      filter->type = type;
	      filter->rule = SYNC_RULE_ALL;
	      filter->field = g_strdup("CATEGORIES");
	      filter->data = g_strdup("");
	      optionpair->filters = g_list_append(optionpair->filters, filter);
	    }
	  }
	  mi = GTK_MENU_ITEM(gtk_menu_item_new_with_label(sync_objtype_as_string(type)));
	  gtk_signal_connect (GTK_OBJECT (mi), "activate",
			      GTK_SIGNAL_FUNC (filter_type_selected),
			      (gpointer) type);
	  gtk_menu_append(menu, GTK_WIDGET(mi));
	  if (i == 0) {
	    gtk_menu_set_active (localmenu, i);
	    gtk_menu_item_activate(mi);
	  }
	}
	activefilterdir = SYNC_DIR_LOCALTOREMOTE;
	activefiltertype = SYNC_OBJECT_TYPE_CALENDAR;
	gtk_widget_show_all(GTK_WIDGET(menu));
	gtk_option_menu_set_menu(GTK_OPTION_MENU(lookup_widget(optionwindow, "filtertypemenu")), GTK_WIDGET(menu));

	menu = GTK_MENU(gtk_menu_new());
	mi = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Always synchronize"));
	gtk_signal_connect (GTK_OBJECT (mi), "activate",
			    GTK_SIGNAL_FUNC (filter_rule_selected),
			    (gpointer) SYNC_RULE_ALL);
	gtk_menu_append(menu, GTK_WIDGET(mi));
	mi = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Synchronize only if"));
	gtk_signal_connect (GTK_OBJECT (mi), "activate",
			    GTK_SIGNAL_FUNC (filter_rule_selected),
			    (gpointer) SYNC_RULE_ONLYIF);
	gtk_menu_append(menu, GTK_WIDGET(mi));
	mi = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Do not synchronize if"));
	gtk_signal_connect (GTK_OBJECT (mi), "activate",
			    GTK_SIGNAL_FUNC (filter_rule_selected),
			    (gpointer) SYNC_RULE_ALLBUT);
	gtk_menu_append(menu, GTK_WIDGET(mi));
	mi = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Never synchronize"));
	gtk_signal_connect (GTK_OBJECT (mi), "activate",
			    GTK_SIGNAL_FUNC (filter_rule_selected),
			    (gpointer) SYNC_RULE_NONE);
	gtk_menu_append(menu, GTK_WIDGET(mi));
	gtk_widget_show_all(GTK_WIDGET(menu));
	gtk_option_menu_set_menu(GTK_OPTION_MENU(lookup_widget(optionwindow, "filterrulemenu")), GTK_WIDGET(menu));

	filter_show(filter_find(activefiltertype, activefilterdir));
      }
    }
  }*/
  gtk_widget_show (env->optionwindow);
  
  return FALSE;
}

void msync_open_localplugin_window(void) {
	GtkOptionMenu *localoption = GTK_OPTION_MENU(lookup_widget(env->optionwindow, "localoption"));
	GtkMenu *localmenu = (GtkMenu *)gtk_option_menu_get_menu(localoption);
	GtkWidget *selected = gtk_menu_get_active(localmenu);
	OSyncPlugin *osplugin = gtk_object_get_data(GTK_OBJECT(selected), "plugin");
	printf("opening %s plugin\n", osync_plugin_get_name(osplugin));
	
	MSyncPair *pair = gtk_object_get_data(GTK_OBJECT(env->optionwindow), "pair");
	OSyncMember *member = osync_group_nth_member(pair->group, 0);
	
	MSyncPlugin *plugin = msync_find_plugin(env, osync_plugin_get_name(osplugin));
	if (!plugin) {
		printf("Unable to find MSyncPlugin!!!\n");
		return;
	}
	plugin->options(env, member);
}

void msync_open_remoteplugin_window(void) {
	GtkOptionMenu *remoteoption = GTK_OPTION_MENU(lookup_widget(env->optionwindow, "remoteoption"));
	GtkMenu *remotemenu = (GtkMenu *)gtk_option_menu_get_menu(remoteoption);
	GtkWidget *selected = gtk_menu_get_active(remotemenu);
	OSyncPlugin *osplugin = gtk_object_get_data(GTK_OBJECT(selected), "plugin");
	
	MSyncPair *pair = gtk_object_get_data(GTK_OBJECT(env->optionwindow), "pair");
	OSyncMember *member = osync_group_nth_member(pair->group, 1);
	
	MSyncPlugin *plugin = msync_find_plugin(env, osync_plugin_get_name(osplugin));
	if (!plugin) {
		printf("Unable to find MSyncPlugin!!!\n");
		return;
	}
	plugin->options(env, member);
}

/*
void sync_plugin_window_closed() {
  pluginwindow = NULL;
}*/

void msync_main_quit(void) {
	GList *p;
	for (p = env->syncpairs; p; p = p->next) {
		MSyncPair *pair = p->data;
		if (pair->engine) {
			osengine_finalize(pair->engine);
			osengine_free(pair->engine);
		}
	}
	gtk_main_quit();
}


gboolean msync_okcanceldialog(char *text, gboolean okcancel) {
  GtkWidget *sync_message;
  int ret=0;
  if (okcancel) {
    sync_message = gnome_message_box_new(text,
					 GNOME_MESSAGE_BOX_QUESTION,
					 GNOME_STOCK_BUTTON_CANCEL,
					 GNOME_STOCK_BUTTON_OK, NULL);
    gtk_window_set_title (GTK_WINDOW (sync_message), "MultiSync Question");
  } else {
    sync_message = gnome_message_box_new(text,
					 GNOME_MESSAGE_BOX_INFO,
					 GNOME_STOCK_BUTTON_OK, NULL);
    gtk_window_set_title (GTK_WINDOW (sync_message), "MultiSync Information");
  }
  gtk_window_set_modal(GTK_WINDOW(sync_message), TRUE);
  gtk_widget_show(sync_message);
  ret=gnome_dialog_run_and_close(GNOME_DIALOG(sync_message));
  return(ret == 1);
}

/*
void sync_pair_selected(GtkTreeSelection *selection, gpointer data) {
  gtk_widget_set_sensitive(lookup_widget(mainwindow, "editbutton"), TRUE);
  gtk_widget_set_sensitive(lookup_widget(mainwindow, "deletebutton"), TRUE);
  gtk_widget_set_sensitive(lookup_widget(mainwindow, "syncnowbutton"), TRUE);
  logwindow_show_log();
}

typedef struct  {
  sync_pair *pair;
  changed_object *change;
} sync_ask_dbfull_arg;

gboolean sync_do_ask_dbfull(gpointer data) {
  char *msg;
  GtkWidget *sync_message;
  sync_ask_dbfull_arg *arg = data;
  int ret;

  msg = g_strdup_printf("The database for %ss is full.", 
			sync_objtype_as_string(arg->change->object_type));
  sync_message = gnome_message_box_new(msg,
				       GNOME_MESSAGE_BOX_QUESTION,
				       "Try again next time",
				       "Ignore", NULL);
  
  gtk_window_set_title (GTK_WINDOW (sync_message), "MultiSync Question");
  gtk_window_set_modal(GTK_WINDOW(sync_message), TRUE);
  gtk_widget_show(sync_message);
  ret=gnome_dialog_run(GNOME_DIALOG(sync_message));
  sync_set_requestmsg(ret==0?SYNC_MSG_FALSE:SYNC_MSG_TRUE, arg->pair);
  return(FALSE);
}

void sync_ask_dbfull(changed_object *change, sync_pair *pair) {
  sync_ask_dbfull_arg *arg = 
    g_malloc0(sizeof(sync_ask_dbfull_arg));
  arg->change = change;
  arg->pair = pair;
  g_idle_add(sync_do_ask_dbfull, arg);
}*/

/*
void sync_process_socket_stream(gpointer data, int fd, 
				GdkInputCondition cond) {
  int ret;
  char *eol;
  gboolean closeit = FALSE;

  ret = read(fd, procsockbuf+procsockpos, 256-procsockpos);
  if (ret <= 0)
    closeit = TRUE;
  else
    procsockpos += ret;
  // Find end of line
  for (eol = procsockbuf; eol < procsockbuf+procsockpos &&
	 *eol != '\r' && *eol != 0 && *eol != '\n'; eol++);
  if (eol < procsockbuf+procsockpos) {
    *eol = 0;
    closeit = TRUE;
    if (!strcmp(procsockbuf, "SHOWGUI")) {
      sync_show_gui();
    }
  }
  if (closeit) {
    close(fd);
    gdk_input_remove(procsockstreamtag);
    procsockpos = 0;
  }
}

void sync_process_socket_connection(gpointer data, int fd, 
				    GdkInputCondition cond) {
  // Someone connected to our process socket.
  int streamfd;
  struct sockaddr_un mysock;
  int socksize = sizeof(struct sockaddr_un);

  streamfd = accept(fd, (struct sockaddr*) &mysock, 
		    &socksize);
  if (streamfd >= 0) {
    fcntl(streamfd, F_SETFL, O_NONBLOCK);
    procsockstreamtag = gdk_input_add(streamfd, GDK_INPUT_READ, 
				      sync_process_socket_stream, 
				      NULL);
  }
}

// Returns true if we should continue executing (false if it found
// another process)
gboolean sync_open_process_socket() {
  struct sockaddr_un mysock;
  char *sockname = NULL;

  mysock.sun_family = AF_UNIX;
  sockname = g_strdup_printf("/tmp/multisync-%s", g_get_user_name());
  strncpy(mysock.sun_path, sockname, 108);
  g_free(sockname);

  procsockfd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (procsockfd < 0)
    return(TRUE);
  if (!connect(procsockfd, (struct sockaddr*) &mysock, 
	       sizeof(struct sockaddr_un))) {
    char cmd[] = "SHOWGUI";
    if (write(procsockfd, cmd, strlen(cmd)+1) == strlen(cmd)+1) {
      // Successfully told the original MultiSync instance to show GUI.
      close(procsockfd);
      procsockfd = -1;
      return(FALSE);
    }
  }
  close(procsockfd);
  procsockfd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (procsockfd < 0)
    return(TRUE);
  unlink(mysock.sun_path);
  if (!bind(procsockfd, (struct sockaddr*) &mysock, 
	    sizeof(struct sockaddr_un))) {
    listen(procsockfd, 0);
    gdk_input_add(procsockfd, GDK_INPUT_READ, sync_process_socket_connection, 
		  NULL);  
    return(TRUE);
  } else {
    printf("Failed to bind process socket.\n");
  }
  close(procsockfd);
  procsockfd = -1;
  return(TRUE);
}*/

void msync_show_msg_info(const char *msg) {
  GtkWidget *sync_message;
  sync_message = gnome_message_box_new(msg,
					 GNOME_MESSAGE_BOX_INFO,
					 GNOME_STOCK_BUTTON_OK, NULL);
  gtk_window_set_title (GTK_WINDOW (sync_message), "MultiSync");
  gtk_window_set_modal(GTK_WINDOW(sync_message), TRUE);
  gtk_widget_show(sync_message);
}

void msync_show_msg_warn(const char *msg) {
  GtkWidget *sync_message;
  sync_message = gnome_message_box_new(msg,
					 GNOME_MESSAGE_BOX_ERROR,
					 GNOME_STOCK_BUTTON_OK, NULL);
  gtk_window_set_title (GTK_WINDOW (sync_message), "MultiSync");
  gtk_window_set_modal(GTK_WINDOW(sync_message), TRUE);
  gtk_widget_show(sync_message);
}

/*
gboolean sync_do_show_msg(gpointer msg) {
  sync_show_msg((char*) msg);
  g_free(msg);
  return(FALSE);
}

void sync_async_msg(char *msg) {
  g_idle_add(sync_do_show_msg, g_strdup(msg));
}

typedef struct  {
  sync_pair *pair;
  changed_object *firstobj;
  changed_object *secondobj;
  gboolean sameuid;
} sync_ask_duplicate_arg;
*/

/*
char *msync_pair_to_string(MSyncPair *pair) {
  char *name NULL;
  
  if (pair->displayname && strlen(pair->displayname) > 0)
    name = g_strdup(pair->displayname);
  else {
    char *l = NULL, *r = NULL;
    if (pair->localclient)
      l = pair->localclient->longname;
    else
      l = pair->localname;
    if (pair->remoteclient)
      r = pair->remoteclient->longname;
    else
      r = pair->remotename;
    name = g_strdup_printf("%s - %s", l, r);
  }
  return(name);
}

void sync_duplicate_actionselected(GtkWidget *window,
				   sync_duplicate_action action) {
  sync_pair *pair; 
  pair = gtk_object_get_data(GTK_OBJECT(window), "syncpair");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(window, "remembercheckbutton")))) 
    pair->default_duplicate_action = action;

  gtk_widget_destroy(window);
  sync_set_requestdata((gpointer) action, pair);
}

void sync_ask_duplicate(changed_object *firstobj,
			changed_object *secondobj, gboolean sameuid,
			sync_pair *pair) {
  sync_ask_duplicate_arg *arg = g_malloc0(sizeof(sync_ask_duplicate_arg));
  arg->firstobj = firstobj;
  arg->secondobj = secondobj;
  arg->pair = pair;
  arg->sameuid = sameuid;
  g_idle_add(sync_do_ask_duplicate, arg);
}*/
