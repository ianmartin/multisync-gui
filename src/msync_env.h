#ifndef MSYNC_ENV_H_
#define MSYNC_ENV_H_

struct MSyncEnv {
	OSyncEnv*  osync;
	GladeXML*  window;
	GtkWidget* mainwindow;
	GtkWidget* groupcontainer;
	GtkWidget* newgroupdialog;
	GtkWidget* newgroupentry;
	GtkWidget* editgroupdialog;
	GtkWidget* editgrouptreeview;
	GtkWidget* editgrouplabel;
	GtkWidget* editgroupplugincontainer;
	GtkWidget* editgroupsettings;
	GtkWidget* editgroupsettingsgroupnameentry;
	GtkWidget* editgroupaddmemberdialog;
	GtkWidget* editgroupaddmemberbutton;
	GtkWidget* editgroupaddmembertreeview;
	GtkWidget* syncronizegroupconflictdialog;
	GtkWidget* syncronizegroupconflictcontainer;
	GtkWidget* syncronizegroupconflictbuttons;
	GtkWidget* syncronizegroupcheckbuttonremember;
	GtkWidget* aboutdialog;
	GList *groups;
	GList *plugins;
	MSyncGroup* curgroup;
	OSyncMember* curmember;
};

void msync_env_finalize(MSyncEnv* env);
void msync_env_load_plugins(MSyncEnv* env);
void msync_env_load_groups(MSyncEnv *env);
void msync_env_syncronize_group(MSyncEnv *env, MSyncGroup *group);
void msync_env_remove_group(MSyncEnv *env, MSyncGroup *group);
void msync_evn_newgroupdialog_show(MSyncEnv *env);
void msync_env_newgroupdialog_add_group(MSyncEnv *env, char* groupname);
void msync_evn_editgroupdialog_show(MSyncEnv *env, MSyncGroup* group);
void msync_env_editgroupdialog_update_treeview(MSyncEnv *env);
void msync_env_editgroupdialog_save_settings(MSyncEnv *env, MSyncGroup* group);
void msync_env_editgroupdialog_show_extended(MSyncEnv *env, OSyncMember* member);
void msync_env_editgroupaddmemberdialog_show(MSyncEnv *env);
void msync_env_editgroupaddmemberdialog_add_member(MSyncEnv *env, OSyncPlugin* plugin);
void msync_evn_syncronizegroupconflictdialog_show(MSyncEnv *env, gboolean threadsafe);
void msync_evn_aboutdialog_show(MSyncEnv *env);

#endif /*MSYNC_ENV_H_*/
