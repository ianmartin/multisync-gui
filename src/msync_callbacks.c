#include "multisync.h"

void on_exit(gpointer user_data)
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
	
	//there is also a response event so we have not to hide the dialog */
	msync_env_newgroupdialog_add_group(env, (char *)gtk_entry_get_text(GTK_ENTRY(env->newgroupentry)));
}

void on_editgroupclosebutton_clicked(GtkButton* button, gpointer user_data)
{
	g_assert(user_data);
	MSyncEnv* env = (MSyncEnv*)user_data;
	
	msync_env_editgroupdialog_save_settings(env, env->curgroup);
	gtk_widget_hide(GTK_WIDGET(env->editgroupdialog));	
}

void on_editgroupaddmemberbutton_clicked(GtkButton* button, gpointer user_data)
{
	g_assert(user_data);
	MSyncEnv* env = (MSyncEnv *)user_data;
	
	msync_env_editgroupaddmemberdialog_show(env);
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

void on_buttondelete_clicked(GtkButton *button, gpointer user_data)
{
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup*)user_data;
	
	msync_env_remove_group(group->msync, group);
}

void on_buttonedit_clicked(GtkButton *button, gpointer user_data)
{
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup*)user_data;
	
	msync_evn_editgroupdialog_show(group->msync, group);
}

void on_buttonsyncronize_clicked(GtkButton *button, gpointer user_data)
{
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup*)user_data;
	
	msync_env_syncronize_group(group->msync, group);
}

void on_toolbutton_newgroup_clicked(GtkMenuItem *menuitem, gpointer user_data)
{
	g_assert(user_data);
	MSyncEnv* env = (MSyncEnv*)user_data;
	
	msync_evn_newgroupdialog_show(env);
}

void on_toolbutton_about_clicked(GtkMenuItem *menuitem, gpointer user_data)
{
	g_assert(user_data);
	MSyncEnv* env = (MSyncEnv*)user_data;

	msync_evn_aboutdialog_show(env);
}
