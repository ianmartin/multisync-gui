#ifndef MSYNC_GROUP_H_
#define MSYNC_GROUP_H_

struct MSyncGroup {
	MSyncEnv* msync;
	OSyncGroup *group;
	GtkWidget* widget;
	GtkWidget* label;
	GtkWidget* vbox;
	GtkWidget* enginelabel;
	GList* memberstatuslabel;
	OSyncEngine* engine;
	OSyncMapping* curmapping;
};

void msync_group_free(MSyncGroup *group);
void msync_group_update_engine_status(MSyncGroup *group, gboolean gtkthreadsafe, const char* msg);
void msync_group_update_member_status(MSyncGroup *group, OSyncMember *member, const char* msg);

#endif /*MSYNC_GROUP_H_*/
