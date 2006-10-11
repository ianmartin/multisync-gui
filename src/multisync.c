#include "multisync.h"

#ifndef MULTISYNC_GLADE
#define MULTISYNC_GLADE "multisync.glade"
#endif

int main (int argc, char *argv[])
{
	MSyncEnv *env;
	OSyncError *error = NULL;
	GtkWidget *widget;
	char *configdir = NULL;
	const char *load_groups = "TRUE";
	const char *load_plugins = "TRUE";
	const char *load_formats = "TRUE";

	env = g_malloc0(sizeof(MSyncEnv));
	env->osync = osync_env_new();

	if (!osync_env_initialize(env->osync, &error)) {
		osync_error_update(&error, "Unable to initialize environment: %s\n", osync_error_print(&error));
		goto error_free_env;
	}

	osync_env_set_option(env->osync, "GROUPS_DIRECTORY", configdir);
	osync_env_set_option(env->osync, "LOAD_GROUPS", load_groups);
	osync_env_set_option(env->osync, "LOAD_PLUGINS", load_plugins);
	osync_env_set_option(env->osync, "LOAD_FORMATS", load_formats);

	g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);
	gdk_threads_enter();

	env->plugins = NULL;
	env->window = glade_xml_new(MULTISYNC_GLADE, NULL, NULL);
	env->mainwindow = glade_xml_get_widget(env->window, "mainwindow");
	env->groupcontainer = glade_xml_get_widget(env->window, "groupcontainer1");
	env->newgroupdialog = glade_xml_get_widget(env->window, "newgroupdialog");
	env->newgroupentry = glade_xml_get_widget(env->window, "newgroupentry");
	env->editgroupdialog = glade_xml_get_widget(env->window, "editgroupdialog");
	env->editgrouptreeview = glade_xml_get_widget(env->window, "editgrouptreeview");
	env->editgrouplabel = glade_xml_get_widget(env->window, "editgrouplabel");
	env->editgroupplugincontainer = glade_xml_get_widget(env->window, "editgroupplugincontainer");
	env->editgroupsettings = glade_xml_get_widget(env->window, "editgroupsettings");
	env->editgroupsettingsgroupnameentry = glade_xml_get_widget(env->window, "editgroupsettingsgroupnameentry");
	env->editgroupaddmemberdialog = glade_xml_get_widget(env->window, "editgroupaddmemberdialog");
	env->editgroupaddmembertreeview = glade_xml_get_widget(env->window, "editgroupaddmembertreeview");
	env->syncronizegroupconflictdialog = glade_xml_get_widget(env->window, "syncronizegroupconflictdialog");
	env->syncronizegroupconflictcontainer = glade_xml_get_widget(env->window, "syncronizegroupconflictcontainer");
	env->syncronizegroupconflictbuttons = glade_xml_get_widget(env->window, "syncronizegroupconflictbuttons");
	env->syncronizegroupcheckbuttonremember = glade_xml_get_widget(env->window, "syncronizegroupcheckbuttonremember");
	env->aboutdialog = glade_xml_get_widget(env->window, "aboutdialog");

	g_signal_connect_swapped(G_OBJECT(env->mainwindow), "delete_event", G_CALLBACK(on_msync_exit), env);
	g_signal_connect(G_OBJECT(env->newgroupdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->newgroupdialog), "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(G_OBJECT(env->editgroupdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->editgroupdialog), "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(G_OBJECT(env->editgroupaddmemberdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->editgroupaddmemberdialog), "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(G_OBJECT(env->syncronizegroupconflictdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->syncronizegroupconflictdialog), "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(G_OBJECT(env->aboutdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
	g_signal_connect(G_OBJECT(env->aboutdialog), "response", G_CALLBACK(gtk_widget_hide), NULL);

	widget = glade_xml_get_widget (env->window, "toolbutton1");
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_toolbutton_newgroup_clicked), env);

	//widget = glade_xml_get_widget (window, "toolbutton2");
	//g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(), NULL);

	widget = glade_xml_get_widget(env->window, "toolbutton3");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(on_msync_exit), env);
                   
	widget = glade_xml_get_widget(env->window, "toolbutton4");
	g_signal_connect_swapped(G_OBJECT(widget), "clicked", G_CALLBACK(on_toolbutton_about_clicked), env);

	widget = glade_xml_get_widget (env->window, "newgroupbuttonapply");
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_newgroupbuttonapply_clicked), env);	
	
	widget = glade_xml_get_widget(env->window, "editgroupclosebutton");	
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_editgroupclosebutton_clicked), env);
	
	widget = glade_xml_get_widget(env->window, "editgroupaddmemberbutton");	
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_editgroupaddmemberbutton_clicked), env);
	
	widget = glade_xml_get_widget(env->window, "editgroupaddmemberapplybutton");	
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(on_editgroupaddmemberapplybutton_clicked), env);		

	GtkTreeSelection* treeselection = gtk_tree_view_get_selection(GTK_TREE_VIEW(env->editgrouptreeview));
	g_signal_connect(G_OBJECT(treeselection), "changed", G_CALLBACK(on_editgrouptreeview_change), env);

	msync_env_load_groups(env);
	msync_env_load_plugins(env);

	gtk_main ();
	gdk_threads_leave();

	return 0;

error_free_env:
	osync_env_free(env->osync);
	fprintf(stderr, "%s\n", osync_error_print(&error));
	osync_error_free(&error);
	return -1;
}
