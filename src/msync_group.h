#ifndef MSYNC_GROUP_H_
#define MSYNC_GROUP_H_

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
};

void msync_group_syncronize_show_conflict_dialog(OSyncEngine *engine, OSyncMapping *mapping, void *user_data);
void msync_group_syncronize_update_member_status(OSyncMemberUpdate *status, void *user_data);
void mapping_status(OSyncMappingUpdate *status, void *user_data);
void msync_group_syncronize_update_engine_status(OSyncEngine *engine, OSyncEngineUpdate *status, void *user_data);
void entry_status(OSyncEngine *engine, OSyncChangeUpdate *status, void *user_data);

void msync_group_free(MSyncGroup *group);
void msync_group_update_engine_status(MSyncGroup *group, gboolean gtkthreadsafe, const char* msg);
void msync_group_update_member_status(MSyncGroup *group, OSyncMember *member, const char* msg);

#endif /*MSYNC_GROUP_H_*/
