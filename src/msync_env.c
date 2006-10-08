#include "multisync.h"



void msync_env_finalize(MSyncEnv* env)
{
	g_list_foreach(env->groups, msync_group_free, NULL);
}

void msync_env_load_plugins(MSyncEnv* env)
{
	msync_plugin_register ( &(env->plugins),
							"default",
							msync_defaultplugin_create_widget(),
							msync_defaultplugin_get_config,
							msync_defaultplugin_set_config,
							NULL);
}

void msync_env_load_groups(MSyncEnv *env)
{
	int i;
	for (i = 0; i < osync_env_num_groups(env->osync); i++) {
		OSyncGroup* group = osync_env_nth_group(env->osync, i);
		msync_group_new(env, group);
	}
}

void msync_env_syncronize_group(MSyncEnv *env, MSyncGroup *group)
{
printf("%s\n", __func__);
	OSyncError *error = NULL;



osengine_synchronize(group->engine, &error);
if(error)
	printf("Error synchronizing: %s\n", osync_error_print(&error));

	//osengine_finalize(engine);
	//osengine_free(engine);
printf("%sENDE\n", __func__);	
}

void msync_env_remove_group(MSyncEnv *env, MSyncGroup *group)
{
	OSyncError *error = NULL;
	if (!osync_group_delete(group->group, &error)) {
		msync_error_message(GTK_WINDOW(group->msync), "Unable to delete group %s: %s\n", osync_group_get_name(group->group), osync_error_print(&error));
		osync_error_free(&error);
	}
	msync_group_free(group);
	
}

void msync_evn_newgroupdialog_show(MSyncEnv *env)
{
	gtk_entry_set_text(GTK_ENTRY(env->newgroupentry), "");
	gtk_widget_show(env->newgroupdialog);
}

void msync_env_newgroupdialog_add_group(MSyncEnv *env, char* groupname)
{
	if(strlen(groupname) < 1) {
		msync_error_message(GTK_WINDOW(env->mainwindow), "You have not entered a group name.");
		return;		
	}
	
	OSyncGroup *osyncgroup = osync_group_new(env->osync);
	osync_group_set_name(osyncgroup, groupname);

	OSyncError *error = NULL;
	if (!osync_group_save(osyncgroup, &error)) {
		msync_error_message(GTK_WINDOW(env->mainwindow), "Unable to save group: %s\n", osync_error_print(&error));
		osync_error_free(&error);
	}
	
	msync_group_new(env, osyncgroup);
}

void msync_evn_editgroupdialog_show(MSyncEnv *env, MSyncGroup* group)
{
printf("%s\n", __func__);
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
printf("%s\n", __func__);
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
printf("%s\n", __func__);
	OSyncError* error = NULL;
	
	if(!env->curmember) {
		char *tmp = (char *) gtk_entry_get_text(GTK_ENTRY(env->editgroupsettingsgroupnameentry));
		if(strcmp(osync_group_get_name(group->group), tmp) != 0)
		{
			osync_group_set_name(group->group, tmp);
		
			if (!osync_group_save(group->group, &error)) {
				msync_error_message(GTK_WINDOW(env->editgroupdialog), "Unable to save group: %s\n", osync_error_print(&error));
				osync_error_free(&error);
			}
			msync_env_editgroupdialog_update_treeview(env);
		}
	}else{
		OSyncError* error = NULL;
		
		int size;
		MSyncPlugin* plugin;
		
		plugin = msync_plugin_find(env->plugins, osync_member_get_pluginname(env->curmember));
		if(!plugin)
			plugin = msync_plugin_find(env->plugins, "default");
		
		const char* data = plugin->msync_plugin_get_config(plugin);
		osync_member_set_config(env->curmember, data, strlen(data));
		free(data);

		if (!osync_group_save(group->group, &error)) {
			msync_error_message(GTK_WINDOW(env->editgroupdialog), "Unable to save group: %s\n", osync_error_print(&error));
			osync_error_free(&error);
		}
	}
}

void msync_env_editgroupdialog_show_extended(MSyncEnv *env, OSyncMember* member)
{
printf("%s\n", __func__);
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
		plugin->msync_plugin_set_config(plugin, data);
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
	for (i = 0; i < osync_env_num_plugins(env->osync); i++) {
		plugin = osync_env_nth_plugin(env->osync, i);
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
	OSyncMember *member = osync_member_new(env->curgroup->group);
	if (!osync_member_instance_plugin(member, osync_plugin_get_name(plugin), &error)) {
		msync_error_message(GTK_WINDOW(env->editgroupdialog), "Unable to instance plugin with name %s: %s\n", osync_plugin_get_name(plugin), osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
		
	if (!osync_member_save(member, &error)) {
		msync_error_message(GTK_WINDOW(env->editgroupdialog), "Unable to save member: %s\n", osync_error_print(&error));
		osync_error_free(&error);
	}
	msync_env_editgroupdialog_update_treeview(env);
}

void msync_evn_aboutdialog_show(MSyncEnv *env)
{
	gtk_widget_show(env->aboutdialog);
}
