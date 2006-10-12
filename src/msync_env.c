#include "multisync.h"

int msync_env_init(MSyncEnv* env)
{
	OSyncError *error = NULL;
	GtkWidget *widget;
	char *configdir = NULL;
	const char *load_groups = "TRUE";
	const char *load_plugins = "TRUE";
	const char *load_formats = "TRUE";
	
	env->osyncenv = osync_env_new();
	
	if (!osync_env_initialize(env->osyncenv, &error)) {
		osync_error_update(&error, "Unable to initialize environment: %s\n", osync_error_print(&error));
		goto error_free_env;
	}

	osync_env_set_option(env->osyncenv, "GROUPS_DIRECTORY", configdir);
	osync_env_set_option(env->osyncenv, "LOAD_GROUPS", load_groups);
	osync_env_set_option(env->osyncenv, "LOAD_PLUGINS", load_plugins);
	osync_env_set_option(env->osyncenv, "LOAD_FORMATS", load_formats);

	env->plugins = NULL;
	env->gladexml = glade_xml_new(MULTISYNC_GLADE, NULL, NULL);
	env->mainwindow = glade_xml_get_widget(env->gladexml, "mainwindow");
	env->groupcontainer = glade_xml_get_widget(env->gladexml, "groupcontainer1");
	env->newgroupdialog = glade_xml_get_widget(env->gladexml, "newgroupdialog");
	env->newgroupentry = glade_xml_get_widget(env->gladexml, "newgroupentry");
	env->editgroupdialog = glade_xml_get_widget(env->gladexml, "editgroupdialog");
	env->editgrouptreeview = glade_xml_get_widget(env->gladexml, "editgrouptreeview");
	env->editgrouplabel = glade_xml_get_widget(env->gladexml, "editgrouplabel");
	env->editgroupplugincontainer = glade_xml_get_widget(env->gladexml, "editgroupplugincontainer");
	env->editgroupsettings = glade_xml_get_widget(env->gladexml, "editgroupsettings");
	env->editgroupsettingsgroupnameentry = glade_xml_get_widget(env->gladexml, "editgroupsettingsgroupnameentry");
	env->editgroupaddmemberdialog = glade_xml_get_widget(env->gladexml, "editgroupaddmemberdialog");
	env->editgroupaddmembertreeview = glade_xml_get_widget(env->gladexml, "editgroupaddmembertreeview");
	env->aboutdialog = glade_xml_get_widget(env->gladexml, "aboutdialog");

	g_signal_connect_swapped(G_OBJECT(env->mainwindow), "delete_event", G_CALLBACK(on_msync_exit), env);
	g_signal_connect(G_OBJECT(env->newgroupdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->newgroupdialog), "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(G_OBJECT(env->editgroupdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->editgroupdialog), "response", G_CALLBACK(gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->editgroupaddmemberdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->editgroupaddmemberdialog), "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(G_OBJECT(env->aboutdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->aboutdialog), "response", G_CALLBACK(gtk_widget_hide), NULL);

	widget = glade_xml_get_widget (env->gladexml, "toolbutton1");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(msync_evn_newgroupdialog_show), env);

	//widget = glade_xml_get_widget (gladexml, "toolbutton2");
	//g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(), NULL);

	widget = glade_xml_get_widget(env->gladexml, "toolbutton3");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(on_msync_exit), env);
                   
	widget = glade_xml_get_widget(env->gladexml, "toolbutton4");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(gtk_widget_show), env->aboutdialog);

	widget = glade_xml_get_widget (env->gladexml, "newgroupbuttonapply");
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_newgroupbuttonapply_clicked), env);	
	
	widget = glade_xml_get_widget(env->gladexml, "editgroupclosebutton");	
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_editgroupclosebutton_clicked), env);
	
	widget = glade_xml_get_widget(env->gladexml, "editgroupaddmemberbutton");	
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(msync_env_editgroupaddmemberdialog_show), env);
	
	widget = glade_xml_get_widget(env->gladexml, "editgroupaddmemberapplybutton");	
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_editgroupaddmemberapplybutton_clicked), env);		

	GtkTreeSelection* treeselection = gtk_tree_view_get_selection(GTK_TREE_VIEW(env->editgrouptreeview));
	g_signal_connect(G_OBJECT(treeselection), "changed", G_CALLBACK(on_editgrouptreeview_change), env);

	msync_env_load_groups(env);
	msync_env_load_plugins(env);
	return TRUE;

error_free_env:
	osync_env_free(env->osyncenv);
	fprintf(stderr, "%s\n", osync_error_print(&error));
	osync_error_free(&error);
	return FALSE;
}

void msync_env_finalize(MSyncEnv* env)
{
	OSyncError* error;
	
	g_list_foreach(env->groups, (GFunc)msync_group_free, NULL);
	
	if (!osync_env_finalize(env->osyncenv, &error)) {
		osync_error_update(&error, "Unable to finalize environment: %s\n", osync_error_print(&error));
		goto error_free_env;
	}
	osync_env_free(env->osyncenv);
	return;
	
error_free_env:
	osync_env_free(env->osyncenv);
	fprintf(stderr, "%s\n", osync_error_print(&error));
	osync_error_free(&error);
	return;
}

void msync_env_load_plugins(MSyncEnv* env)
{
	msync_plugin_register ( &(env->plugins),
							"default",
							msync_defaultplugin_create_widget(),
							msync_defaultplugin_get_config,
							msync_defaultplugin_set_config,
							NULL);
	
	MSyncEvo2Sync* evo2sync = g_malloc0(sizeof(MSyncEvo2Sync));
	GtkWidget* widget = msync_evo2sync_create_widget(evo2sync);
	msync_plugin_register ( &(env->plugins),
							"evo2-sync",
							widget,
							msync_evo2sync_get_config,
							msync_evo2sync_set_config,
							evo2sync);
}

void msync_env_load_groups(MSyncEnv *env)
{
	int i;
	for (i = 0; i < osync_env_num_groups(env->osyncenv); i++) {
		OSyncGroup* group = osync_env_nth_group(env->osyncenv, i);
		msync_group_new(env, group);
	}
}

void msync_evn_newgroupdialog_show(MSyncEnv *env)
{
	gtk_entry_set_text(GTK_ENTRY(env->newgroupentry), "");
	gtk_widget_show(env->newgroupdialog);
}

void msync_env_newgroupdialog_add_group(MSyncEnv *env, char* groupname)
{
	OSyncError *error = NULL;
	OSyncGroup *group;
	
	if(strlen(groupname) < 1) {
		msync_error_message(GTK_WINDOW(env->mainwindow), FALSE, "You have not entered a group name.");
		return;		
	}
	
	group = osync_group_new(env->osyncenv);
	osync_group_set_name(group, groupname);

	if (!osync_group_save(group, &error)) {
		msync_error_message(GTK_WINDOW(env->mainwindow), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
		osync_error_free(&error);
	}
	
	msync_group_new(env, group);
}

void msync_evn_editgroupdialog_show(MSyncEnv *env, MSyncGroup* group)
{
	env->curgroup = group;
	gtk_widget_show(env->editgroupdialog);
	
	GtkTreeViewColumn *col;
	GtkCellRenderer   *renderer;
	GtkTreeIter iter;

	if(!gtk_tree_view_get_column(GTK_TREE_VIEW(env->editgrouptreeview), 0))
	{
		col = gtk_tree_view_column_new();
		
		renderer = gtk_cell_renderer_pixbuf_new();
		gtk_tree_view_column_pack_start(col, renderer, FALSE);
		gtk_tree_view_column_add_attribute(col, renderer, "pixbuf", 0);
	
		renderer = gtk_cell_renderer_text_new();
		gtk_tree_view_column_pack_start(col, renderer, FALSE);
		gtk_tree_view_column_add_attribute(col, renderer, "text", 1);

		gtk_tree_view_append_column(GTK_TREE_VIEW(env->editgrouptreeview), col);
	}

	/* must be called before the first changed event */ 
	msync_env_editgroupdialog_show_extended(env, NULL);
	
	msync_env_editgroupdialog_update_treeview(env);
	
	GtkTreeSelection* treeselection = gtk_tree_view_get_selection(GTK_TREE_VIEW(env->editgrouptreeview));
	gtk_tree_selection_set_mode(treeselection, GTK_SELECTION_BROWSE);
	gtk_tree_model_get_iter_first (gtk_tree_view_get_model(GTK_TREE_VIEW(env->editgrouptreeview)), &iter);
	gtk_tree_selection_select_iter(treeselection, &iter);
	
}

void msync_env_editgroupdialog_update_treeview(MSyncEnv *env)
{
	int i;
	GtkTreeModel *model;
	GtkTreeStore *treestore;
	GtkTreeIter  toplevel, child;
	
	treestore = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER);
	GdkPixbuf* pixbuf = gtk_widget_render_icon(env->editgroupdialog, "gtk-dialog-info", GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set (treestore, &toplevel,
						0, pixbuf,
						1, (char *)osync_group_get_name(env->curgroup->group),
						2, NULL,
						-1);
	g_object_unref(pixbuf);
	
	pixbuf = gtk_widget_render_icon(env->editgroupdialog, "gtk-go-forward", GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	for (i = 0; i < osync_group_num_members(env->curgroup->group); i++) {
		OSyncMember* member = osync_group_nth_member(env->curgroup->group, i);
		gtk_tree_store_append(treestore, &child, &toplevel);
		gtk_tree_store_set (treestore, &child,
							0, pixbuf,
							1, (char *)osync_member_get_pluginname(member),
							2, member,
							-1);
	}
	g_object_unref(pixbuf);
	
	model = GTK_TREE_MODEL(treestore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(env->editgrouptreeview), model);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(env->editgrouptreeview));
	g_object_unref(model);

	msync_group_update_widget(env->curgroup);
}

void msync_env_editgroupdialog_save_settings(MSyncEnv *env, MSyncGroup* group)
{
	OSyncError* error = NULL;
	MSyncPlugin* plugin = NULL;
	
	/* save group settings */
	if(!env->curmember) {
		char *tmp = (char *) gtk_entry_get_text(GTK_ENTRY(env->editgroupsettingsgroupnameentry));
		if(strcmp(osync_group_get_name(group->group), tmp) != 0)
		{
			osync_group_set_name(group->group, tmp);
		
			if (!osync_group_save(group->group, &error)) {
				msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
				osync_error_free(&error);
			}
			msync_env_editgroupdialog_update_treeview(env);
		}
	/* save member settings */
	}else{
		plugin = msync_plugin_find(env->plugins, osync_member_get_pluginname(env->curmember));
		if(!plugin)
			plugin = msync_plugin_find(env->plugins, "default");
		
		const char* data = plugin->msync_plugin_get_config(plugin);
		osync_member_set_config(env->curmember, data, strlen(data));
		free((void *)data);

		if (!osync_group_save(group->group, &error)) {
			msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
			osync_error_free(&error);
		}
	}
}

void msync_env_editgroupdialog_show_extended(MSyncEnv *env, OSyncMember* member)
{
	env->curmember = member;
	
	gchar* tmp;
	if(!member)
		tmp = g_strdup_printf("<span weight=\"bold\" size=\"larger\">Settings for Group %s</span>", osync_group_get_name(env->curgroup->group));
	else
		tmp = g_strdup_printf("<span weight=\"bold\" size=\"larger\">Settings for Member %s</span>", osync_member_get_pluginname(member));
	
	gtk_label_set_text(GTK_LABEL(env->editgrouplabel), tmp);
	gtk_label_set_use_markup(GTK_LABEL(env->editgrouplabel), TRUE);
	g_free(tmp);

	GList* childs = gtk_container_get_children(GTK_CONTAINER(env->editgroupplugincontainer));
	g_object_ref(G_OBJECT(childs->data));
	gtk_container_remove(GTK_CONTAINER(env->editgroupplugincontainer), childs->data);
	g_list_free(childs);

	if(!member) {
		gtk_container_add(GTK_CONTAINER(env->editgroupplugincontainer), env->editgroupsettings);
		gtk_widget_show(env->editgroupsettings);
		gtk_entry_set_text(GTK_ENTRY(env->editgroupsettingsgroupnameentry), (const gchar *)osync_group_get_name(env->curgroup->group));
	}else{
		OSyncError* error = NULL;
		char *data;
		int size;
		MSyncPlugin* plugin;
		plugin = msync_plugin_find(env->plugins, osync_member_get_pluginname(member));
		if(!plugin)
			plugin = msync_plugin_find(env->plugins, "default");
	
		gtk_container_add(GTK_CONTAINER(env->editgroupplugincontainer), plugin->widget);
		gtk_widget_show(plugin->widget);
		osync_member_get_config_or_default(member, &data, &size, &error);
		plugin->msync_plugin_set_config(plugin, member, data);
	}	
}

void msync_env_editgroupaddmemberdialog_show(MSyncEnv *env)
{
	gtk_widget_show(env->editgroupaddmemberdialog);
	
	int i;
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;
	GtkTreeModel *model;
	OSyncPlugin *plugin;

	if(!gtk_tree_view_get_column(GTK_TREE_VIEW(env->editgroupaddmembertreeview), 0))
	{
		col = gtk_tree_view_column_new();
		
		renderer = gtk_cell_renderer_pixbuf_new();
		gtk_tree_view_column_pack_start(col, renderer, FALSE);
		gtk_tree_view_column_add_attribute(col, renderer, "pixbuf", 0);
	
		renderer = gtk_cell_renderer_text_new();
		gtk_tree_view_column_pack_start(col, renderer, FALSE);
		gtk_tree_view_column_add_attribute(col, renderer, "text", 1);

		gtk_tree_view_append_column(GTK_TREE_VIEW(env->editgroupaddmembertreeview), col);
	}

	GtkTreeStore *treestore;
	GtkTreeIter  toplevel;
	
	treestore = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER);
	GdkPixbuf* pixbuf = gtk_widget_render_icon(env->editgroupaddmemberdialog, "gtk-go-forward", GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	for (i = 0; i < osync_env_num_plugins(env->osyncenv); i++) {
		plugin = osync_env_nth_plugin(env->osyncenv, i);
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set (treestore, &toplevel,
							0, pixbuf,
							1, (char *)osync_plugin_get_longname(plugin),
							2, plugin,
							-1);
	}
	g_object_unref(pixbuf);
		
	model = GTK_TREE_MODEL(treestore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(env->editgroupaddmembertreeview), model);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(env->editgroupaddmembertreeview));
	g_object_unref(model);

	GtkTreeSelection* treeselection = gtk_tree_view_get_selection(GTK_TREE_VIEW(env->editgroupaddmembertreeview));
	gtk_tree_selection_set_mode(treeselection, GTK_SELECTION_BROWSE);
	gtk_tree_model_get_iter_first (gtk_tree_view_get_model(GTK_TREE_VIEW(env->editgroupaddmembertreeview)), &toplevel);
	gtk_tree_selection_select_iter(treeselection, &toplevel);
}

void msync_env_editgroupaddmemberdialog_add_member(MSyncEnv *env, OSyncPlugin* plugin)
{
	OSyncError* error = NULL;
	OSyncMember *member = NULL;
	
	member = osync_member_new(env->curgroup->group);
	if (!osync_member_instance_plugin(member, osync_plugin_get_name(plugin), &error)) {
		msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to instance plugin with name %s: %s\n", osync_plugin_get_name(plugin), osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
		
	if (!osync_member_save(member, &error)) {
		msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to save member: %s\n", osync_error_print(&error));
		osync_error_free(&error);
	}
	msync_env_editgroupdialog_update_treeview(env);
}

void msync_evn_aboutdialog_show(MSyncEnv *env)
{
	gtk_widget_show(env->aboutdialog);
}
