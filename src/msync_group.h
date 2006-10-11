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
	MSyncEnv* msync;
	OSyncGroup *group;
	GtkWidget* widget;
	GtkWidget* buttondelete;
	GtkWidget* buttonedit;
	GtkWidget* buttonsyncronize;
	GtkWidget* label;
	GtkWidget* vbox;
	GtkWidget* enginelabel;
	GList* memberstatuslabel;
	OSyncEngine* engine;
	GCond* cond;
	GMutex* mutex;
	gboolean go;
	gboolean remember;
	MSyncResolution resolution;
	int winningside;
};

void msync_group_syncronize_show_conflict_dialog(OSyncEngine *engine, OSyncMapping *mapping, void *user_data);
void msync_group_syncronize_update_member_status(OSyncMemberUpdate *status, void *user_data);
void mapping_status(OSyncMappingUpdate *status, void *user_data);
void msync_group_syncronize_update_engine_status(OSyncEngine *engine, OSyncEngineUpdate *status, void *user_data);
void entry_status(OSyncEngine *engine, OSyncChangeUpdate *status, void *user_data);

void msync_group_new(MSyncEnv *env, OSyncGroup *osyncgroup);
void msync_group_free(MSyncGroup *group);
void msync_group_set_sensitive(MSyncGroup *group, gboolean gtkthreadsafe, gboolean sensitive);
void msync_group_update_widget(MSyncGroup *group);
void msync_group_update_engine_status(MSyncGroup *group, gboolean gtkthreadsafe, const char* msg);
void msync_group_update_member_status(MSyncGroup *group, OSyncMember *member, const char* msg);

#endif /*MSYNC_GROUP_H_*/
