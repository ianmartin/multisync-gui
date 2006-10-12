#ifndef MSYNC_EVO2SYNC_H_
#define MSYNC_EVO2SYNC_H_

typedef struct MSyncEvo2Sync {
	GtkWidget* addressbookcombobox;
	GtkWidget* calendarcombobox;
	GtkWidget* todocombobox;
	GList* curaddressbooklist;
	GList* curcalendarlist;
	GList* curtodolist;
} MSyncEvo2Sync;

GtkWidget* msync_evo2sync_create_widget(MSyncEvo2Sync* evo2sync);
void msync_evo2sync_set_config(MSyncPlugin* plugin, OSyncMember *member, const char *config);
const char* msync_evo2sync_get_config(MSyncPlugin* plugin);

#endif /*MSYNC_EVO2SYNC_H_*/
