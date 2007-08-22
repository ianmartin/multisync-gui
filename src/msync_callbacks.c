#include "multisync.h"

void on_msync_exit(gpointer user_data)
{
	g_assert(user_data);
	MSyncEnv* env = (MSyncEnv *)user_data;
	
	msync_env_finalize(env);
	gtk_main_quit();	
}

void on_newgroupbuttonapply_clicked(GtkButton *button, gpointer user_data)
{
	g_assert(user_data);
	MSyncEnv* env = (MSyncEnv*)user_data;
	
	//-there is also a response event so we have not to hide the dialog */
	//+since we can not set require of gtk to 2.10 we close the window with the button
	//+the window frame close button is useless
	msync_env_newgroupdialog_add_group(env, (char *)gtk_entry_get_text(GTK_ENTRY(env->newgroupentry)));
	gtk_widget_hide(env->editgroupdialog);
}

void on_editgroupclosebutton_clicked(GtkButton* button, gpointer user_data)
{
	g_assert(user_data);
	MSyncEnv* env = (MSyncEnv*)user_data;
	
	msync_env_editgroupdialog_save_settings(env, env->curgroup);
	gtk_widget_hide(GTK_WIDGET(env->editgroupdialog));	
}

void on_editgroupaddmemberapplybutton_clicked(GtkButton* button, gpointer user_data)
{
	MSyncEnv* env;
	GtkTreeSelection* selection; 
	GtkTreeModel* model;
	GtkTreeIter iter;
	gpointer plugin;

	g_assert(user_data);
	env = (MSyncEnv *)user_data;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(env->editgroupaddmembertreeview));
	if(gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get(model, &iter, 2, &plugin, -1);		
		msync_env_editgroupaddmemberdialog_add_member(env, plugin);
	}
}

void on_editgroupdiscoverbutton_clicked(GtkButton* button, gpointer user_data)
{
	g_assert(user_data);
	MSyncEnv* env = (MSyncEnv*)user_data;
	OSyncError* error = NULL;
	
	//TODO: dont block gui thread!
	//g_thread_create((GThreadFunc)msync_group_syncronize2, group, FALSE, NULL);
	
	OSyncEngine *engine = osync_engine_new(env->curgroup->group, &error);
	if (!engine) {
		goto error;
	}
	
	/* Discover the objtypes for the members */
	if(!osync_engine_discover_and_block(engine,env->curmember, &error))
		goto error_free_engine;
	
	osync_engine_unref(engine);
	return;

error_free_engine:
	osync_engine_unref(engine);
error:
	msync_error_message(GTK_WINDOW(env->mainwindow), TRUE, "Error while discovering: %s\n", osync_error_print(&error));
	osync_error_unref(&error);
}

void on_editgroupdiscoverbutton_clicked2(gpointer user_data)
{

}

void on_editgrouptreeview_change(GtkTreeSelection *selection, gpointer user_data)
{
	MSyncEnv* env;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gpointer *member;
	
	g_assert(user_data);
	env = (MSyncEnv *)user_data;
	
	if(gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get(model, &iter, 2, &member, -1);
		msync_env_editgroupdialog_save_settings(env, env->curgroup);
		msync_env_editgroupdialog_show_extended(env, (OSyncMember *)member);
	}
}

void on_buttonedit_clicked(GtkButton *button, gpointer user_data)
{
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup*)user_data;
	
	msync_evn_editgroupdialog_show(group->msyncenv, group);
}
