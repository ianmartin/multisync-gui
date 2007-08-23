#include "multisync.h"

int msync_env_init(MSyncEnv* env)
{
	OSyncError *error = NULL;
	GtkWidget *widget;
	//char *configdir = NULL;
	//const char *load_groups = "TRUE";
	//const char *load_plugins = "TRUE";
	//const char *load_formats = "TRUE";
	
	env->osync_group_env = osync_group_env_new(&error);
	if (!env->osync_group_env)
		goto error;
		
	env->osync_format_env = osync_format_env_new(&error);
	if (!env->osync_format_env)
		goto error_free_group_env;
		
	env->osync_plugin_env = osync_plugin_env_new(&error);
	if (!env->osync_plugin_env)
		goto error_free_format_env;
	
	if (!osync_group_env_load_groups(env->osync_group_env, NULL, &error))
		goto error_free_plugin_env;
				
	if (!osync_format_env_load_plugins(env->osync_format_env, NULL, &error))
		goto error_free_plugin_env;
				
	if (!osync_plugin_env_load(env->osync_plugin_env, NULL, &error))
		goto error_free_plugin_env;
	
	//env->osyncenv = osync_env_new();
		
	//if (!osync_env_initialize(env->osyncenv, &error)) {
	//	osync_error_update(&error, "Unable to initialize environment: %s\n", osync_error_print(&error));
	//	goto error_free_env;
	//}

	//osync_env_set_option(env->osyncenv, "GROUPS_DIRECTORY", configdir);
	//osync_env_set_option(env->osyncenv, "LOAD_GROUPS", load_groups);
	//osync_env_set_option(env->osyncenv, "LOAD_PLUGINS", load_plugins);
	//osync_env_set_option(env->osyncenv, "LOAD_FORMATS", load_formats);

	env->plugins = NULL;
	env->gladexml = glade_xml_new(MULTISYNC_GLADE, NULL, NULL);
	env->mainwindow = glade_xml_get_widget(env->gladexml, "mainwindow");
	env->groupcontainer = glade_xml_get_widget(env->gladexml, "groupcontainer1");
	env->newgroupdialog = glade_xml_get_widget(env->gladexml, "newgroupdialog");
	env->newgroupentry = glade_xml_get_widget(env->gladexml, "newgroupentry");
	env->editgroupdialog = glade_xml_get_widget(env->gladexml, "editgroupdialog");
	env->editgrouptreeview = glade_xml_get_widget(env->gladexml, "editgrouptreeview");
	env->editgrouplabel = glade_xml_get_widget(env->gladexml, "editgrouplabel");
	env->editgroupdiscoverhbox = glade_xml_get_widget(env->gladexml, "editgroupdiscoverhbox");
	env->editgroupplugincontainer = glade_xml_get_widget(env->gladexml, "editgroupplugincontainer");
	env->editgroupsettings = glade_xml_get_widget(env->gladexml, "editgroupsettings");
	env->editgroupsettingsconvertercheckbutton = glade_xml_get_widget(env->gladexml, "editgroupsettingsconvertercheckbutton");
	env->editgroupsettingsmergercheckbutton = glade_xml_get_widget(env->gladexml, "editgroupsettingsmergercheckbutton");
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

	widget = glade_xml_get_widget(env->gladexml, "editgroupdiscoverbutton");	
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_editgroupdiscoverbutton_clicked), env);

	GtkTreeSelection* treeselection = gtk_tree_view_get_selection(GTK_TREE_VIEW(env->editgrouptreeview));
	g_signal_connect(G_OBJECT(treeselection), "changed", G_CALLBACK(on_editgrouptreeview_change), env);

//	#ifdef MULTISYNC_LEGACY
//	int i;
//	OSyncFormatEnv *fenv = osync_conv_env_new(env->osyncenv);
//	if (!fenv) {
//		fprintf(stderr, "Unable to load format environment: %s\n", osync_error_print(&error));
//		exit(0);
//	}
//	
//	//GtkWidget* label = gtk_label_new ("<span weight=\"bold\">Disable syncing of objtype:</span>");
//	GtkWidget* label = gtk_label_new ("<span>Disable syncing of objtype:</span>");
//  	gtk_widget_show (label);
//	gtk_box_pack_start (GTK_BOX (env->editgroupsettings), label, FALSE, FALSE, 10);
//	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
//	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
//		
//	for (i = 0; i < osync_conv_num_objtypes(fenv); i++) {
//		OSyncObjType *type = osync_conv_nth_objtype(fenv, i);
//		GtkWidget* checkbutton = gtk_check_button_new_with_label((gchar*)osync_objtype_get_name(type));
//		gtk_widget_show (checkbutton);
//  		gtk_box_pack_start (GTK_BOX (env->editgroupsettings), checkbutton, FALSE, FALSE, 0);
//  		GTK_WIDGET_UNSET_FLAGS (checkbutton, GTK_CAN_FOCUS);
//  		env->editgroupsettingsfilter = g_list_prepend(env->editgroupsettingsfilter, checkbutton);
//	}
//	#endif
	
	
	//osync_group_env_free(env->osync_group_env);
	//osync_format_env_free(env->osync_format_env);
	//osync_plugin_env_free(env->osync_plugin_env);
	
	msync_env_load_groups(env);
	msync_env_load_plugins(env);
	return TRUE;

error_free_plugin_env:
	osync_plugin_env_free(env->osync_plugin_env);
error_free_format_env:
	osync_format_env_free(env->osync_format_env);
error_free_group_env:
	osync_group_env_free(env->osync_group_env);
error:
	fprintf(stderr, "ERROR: %s\n", osync_error_print(&error));
	osync_trace(TRACE_EXIT, "%s: %s", __func__, osync_error_print(&error));
	osync_error_unref(&error);
	return FALSE;
}

void msync_env_finalize(MSyncEnv* env)
{
	//OSyncError* error;
	
	g_list_foreach(env->groups, (GFunc)msync_group_free, NULL);
	
	//if (!osync_env_finalize(env->osyncenv, &error)) {
	//	osync_error_update(&error, "Unable to finalize environment: %s\n", osync_error_print(&error));
	//	goto error_free_env;
	//}
	//osync_env_free(env->osyncenv);
	osync_group_env_free(env->osync_group_env);
	osync_format_env_free(env->osync_format_env);
	osync_plugin_env_free(env->osync_plugin_env);
	return;
	
//error_free_env:
//	osync_env_free(env->osyncenv);
//	fprintf(stderr, "%s\n", osync_error_print(&error));
//	osync_error_free(&error);
//	return;
}

void msync_env_load_plugins(MSyncEnv* env)
{
	msync_plugin_register ( &(env->plugins),
							"default",
							msync_defaultplugin_create_widget(),
							msync_defaultplugin_get_config,
							msync_defaultplugin_set_config,
							NULL);
	
//	MSyncEvo2Sync* evo2sync = g_malloc0(sizeof(MSyncEvo2Sync));
//	GtkWidget* widget = msync_evo2sync_create_widget(evo2sync);
//	msync_plugin_register ( &(env->plugins),
//							"evo2-sync",
//							widget,
//							msync_evo2sync_get_config,
//							msync_evo2sync_set_config,
//							evo2sync);
}

void msync_env_load_groups(MSyncEnv *env)
{
	int i;
	//for (i = 0; i < osync_env_num_groups(env->osyncenv); i++) {
	//	OSyncGroup* group = osync_env_nth_group(env->osyncenv, i);
	//	msync_group_new(env, group);
	//}
	for (i = 0; i < osync_group_env_num_groups(env->osync_group_env); i++) {
		OSyncGroup* group = osync_group_env_nth_group(env->osync_group_env, i);
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
	
	//group = osync_group_new(env->osyncenv);
	//osync_group_set_name(group, groupname);
	//if (!osync_group_save(group, &error)) {
	//	msync_error_message(GTK_WINDOW(env->mainwindow), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
	//	osync_error_free(&error);
	//}
	//msync_group_new(env, group);
	
	group = osync_group_new(&error);
	if (!group)
		goto error;
	
	osync_group_set_name(group, groupname);
	if (!osync_group_env_add_group(env->osync_group_env, group, &error))
		goto error_and_free;
	
	if (!osync_group_save(group, &error))
		goto error_and_free;
	
	msync_group_new(env, group);
	osync_group_unref(group);
	return;

	error_and_free:
		osync_group_unref(group);
	error:
		msync_error_message(GTK_WINDOW(env->mainwindow), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
		osync_error_unref(&error);
	return;
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
	
		#ifdef MULTISYNC_LEGACY
		xmlDocPtr doc;
		char* filename;
		char* buffer;
		int size;
		GList* iter;
		doc = xmlNewDoc((xmlChar*)"1.0");
		doc->children = xmlNewDocNode(doc, NULL, (xmlChar*)"filter", NULL);
		
		for(iter = env->editgroupsettingsfilter; iter != NULL; iter = g_list_next(iter))
		{
			if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(iter->data)))
				xmlNewTextChild(doc->children, NULL, (xmlChar*)gtk_button_get_label(GTK_BUTTON(iter->data)), NULL);
		}
		xmlDocDumpFormatMemory(doc, (xmlChar**)&buffer, &size, 1);
		xmlFree(doc);
		filename = g_strdup_printf("%s/%s", osync_group_get_configdir(group->group), MULTISYNC_LEGACY_FILTERFILE);
		if (!osync_file_write(filename, buffer, size, 0600, &error)) {
			msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
			osync_error_unref(&error);
		}
		g_free(filename);
		#endif
		
		
		osync_group_set_name(group->group, gtk_entry_get_text(GTK_ENTRY(env->editgroupsettingsgroupnameentry)));
		osync_group_set_converter_enabled(group->group, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(env->editgroupsettingsconvertercheckbutton)));
		osync_group_set_merger_enabled(group->group, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(env->editgroupsettingsmergercheckbutton)));
		
		if (!osync_group_save(group->group, &error)) {
			msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
			osync_error_unref(&error);
		}
		msync_env_editgroupdialog_update_treeview(env);

	/* save member settings */
	}else{
		plugin = msync_plugin_find(env->plugins, osync_member_get_pluginname(env->curmember));
		if(!plugin)
			plugin = msync_plugin_find(env->plugins, "default");
		
		const char* data = plugin->msync_plugin_get_config(plugin);
		osync_member_set_config(env->curmember, data);
		free((void *)data);

		if (!osync_group_save(group->group, &error)) {
			msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
			osync_error_unref(&error);
		}
	}
}

void msync_env_editgroupdialog_show_extended(MSyncEnv *env, OSyncMember* member)
{
	OSyncError* error = NULL;
	env->curmember = member;
	
	gchar* tmp;
	if(!member) {
		tmp = g_strdup_printf("<span weight=\"bold\" size=\"larger\">Settings for Group %s</span>", osync_group_get_name(env->curgroup->group));
		gtk_widget_hide(env->editgroupdiscoverhbox);	
	} else {
		tmp = g_strdup_printf("<span weight=\"bold\" size=\"larger\">Settings for Member %s</span>", osync_member_get_pluginname(member));
		gtk_widget_show(env->editgroupdiscoverhbox);
	}
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
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(env->editgroupsettingsconvertercheckbutton), osync_group_get_converter_enabled(env->curgroup->group));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(env->editgroupsettingsmergercheckbutton), osync_group_get_merger_enabled(env->curgroup->group));
		
		#ifdef MULTISYNC_LEGACY
		char* filename;
		xmlDocPtr doc;
		GList* iter;
		for(iter = env->editgroupsettingsfilter; iter != NULL; iter = g_list_next(iter))
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(iter->data), FALSE);
			
		filename = g_strdup_printf("%s/%s", osync_group_get_configdir(env->curgroup->group), MULTISYNC_LEGACY_FILTERFILE);
		doc = xmlReadFile(filename, NULL, 0);
	    if (doc == NULL) {
    	    //msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Could not parse file %s.\n", filename);
			g_free(filename);
			return;
    	}
    	g_free(filename);
		
		xmlNodePtr root = xmlDocGetRootElement(doc);
		xmlNode *cur = root->children;
		
		for (cur = root->children; cur; cur = cur->next) {
        	if (cur->type == XML_ELEMENT_NODE) {
            	for(iter = env->editgroupsettingsfilter; iter != NULL; iter = g_list_next(iter)) {
            		if(!strcmp((char*)cur->name, (char*)gtk_button_get_label(GTK_BUTTON(iter->data))))
            			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(iter->data), TRUE);		
            	}
        	}
    	}
	
    	xmlFreeDoc(doc);
		#endif
		
	}else{
		const char *config = NULL;
		MSyncPlugin* plugin;

		plugin = msync_plugin_find(env->plugins, osync_member_get_pluginname(member));
		if(!plugin)
			plugin = msync_plugin_find(env->plugins, "default");
	
		gtk_container_add(GTK_CONTAINER(env->editgroupplugincontainer), plugin->widget);
		gtk_widget_show(plugin->widget);
		config = osync_member_get_config_or_default(member, &error);
		if (!config)
			goto error;
		plugin->msync_plugin_set_config(plugin, member, config);
	}
	return;
error:
	msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Error: %s\n", osync_error_print(&error));
	osync_error_unref(&error);
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
	//for (i = 0; i < osync_env_num_plugins(env->osyncenv); i++) {
	for (i = 0; i < osync_plugin_env_num_plugins(env->osync_plugin_env); i++) {
		//plugin = osync_env_nth_plugin(env->osync_plugin_env, i);
		plugin = osync_plugin_env_nth_plugin(env->osync_plugin_env, i);
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
	
	//member = osync_member_new(env->curgroup->group);
	member = osync_member_new(&error);
	if (!member) {
		msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to create member for plugin %s: %s\n", osync_plugin_get_name(plugin), osync_error_print(&error));
		osync_error_unref(&error);
		return;
	}
	
	osync_group_add_member(env->curgroup->group, member);
	osync_member_set_pluginname(member, osync_plugin_get_name(plugin));
	
	//if (!osync_member_instance_plugin(member, osync_plugin_get_name(plugin), &error)) {
	//	msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to instance plugin with name %s: %s\n", osync_plugin_get_name(plugin), osync_error_print(&error));
	//	osync_error_free(&error);
	//	return;
	//}
		
	if (!osync_member_save(member, &error)) {
		msync_error_message(GTK_WINDOW(env->editgroupdialog), FALSE, "Unable to save member: %s\n", osync_error_print(&error));
		osync_error_unref(&error);
	}
	msync_env_editgroupdialog_update_treeview(env);
	osync_member_unref(member);
}

void msync_evn_aboutdialog_show(MSyncEnv *env)
{
	gtk_widget_show(env->aboutdialog);
}
