#ifndef MSYNC_GROUP_H_
#define MSYNC_GROUP_H_

typedef enum MSyncResolution {
	MSYNC_RESOLUTION_UNKNOWN,
	MSYNC_RESOLUTION_DUPLICATE,
	MSYNC_RESOLUTION_IGNORE,
	MSYNC_RESOLUTION_NEWER,
	MSYNC_RESOLUTION_SELECT
} MSyncResolution;

struct MSyncGroup {
	MSyncEnv* msyncenv;
	OSyncGroup *group;
	GtkWidget* widget;
	GtkWidget* buttondelete;
	GtkWidget* buttonedit;
	GtkWidget* buttonsyncronize;
	GtkWidget* label;
	GtkWidget* vbox;
	GtkWidget* enginelabel;
	GList* memberstatuslabel;
	GtkWidget* conflictdialog;
	GtkWidget* conflictcontainer;
	GtkWidget* conflictbuttons;
	GtkWidget* conflictcheckbuttonremember;
	OSyncEngine* engine;
	GCond* cond;
	GMutex* mutex;
	gboolean go;
	MSyncResolution resolution;
	int winningside;
};

void msync_group_new(MSyncEnv *env, OSyncGroup *osyncgroup);
void msync_group_free(MSyncGroup *group);
void msync_group_remove(MSyncGroup *group);
void msync_group_syncronize(MSyncGroup *group);
void msync_group_syncronize2(MSyncGroup *group);
void msync_group_syncronize_enginestatus(OSyncEngine *engine, OSyncEngineUpdate *status, void *user_data);
void msync_group_syncronize_memberstatus(OSyncMemberUpdate *status, void *user_data);
void msync_group_syncronize_mappingstatus(OSyncMappingUpdate *status, void *user_data);
void msync_group_syncronize_entrystatus(OSyncEngine *engine, OSyncChangeUpdate *status, void *user_data);
void msync_group_syncronize_conflict(OSyncEngine *engine, OSyncMapping *mapping, void *user_data);
void msync_group_syncronize_conflictdialog_show(MSyncGroup *group, gboolean threadsafe);
void msync_group_syncronize_unlock_conflict_dialog(MSyncGroup* group);
void msync_group_syncronize_mapping_duplicate(MSyncGroup* group);
void msync_group_syncronize_mapping_ignore(MSyncGroup* group);
void msync_group_syncronize_mapping_newer(MSyncGroup* group);
void msync_group_syncronize_mapping_select(GtkButton* button, MSyncGroup* group);
void msync_group_update_engine_status(MSyncGroup *group, gboolean gtkthreadsafe, const char* msg);
void msync_group_update_member_status(MSyncGroup *group, OSyncMember *member, const char* msg);
void msync_group_set_sensitive(MSyncGroup *group, gboolean gtkthreadsafe, gboolean sensitive);
void msync_group_update_widget(MSyncGroup *group);
GtkWidget *msync_group_create_widget(MSyncGroup *group);
void msync_group_create_syncronizegroupconflictdialog(MSyncGroup *group);

#endif /*MSYNC_GROUP_H_*/
