#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <gdk/gdk.h>
#include <gnome.h>
#include <libgnomeui/gnome-window-icon.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <config.h>

#include "interface.h"
#include "support.h"
#include "gui.h"
#include "opensync/opensync.h"
#include "opensync/engine.h"


typedef struct MSyncEnv {
	OSyncEnv *osync;
	GtkWidget* mainwindow;
	GtkWidget* optionwindow;
	GtkWidget* pluginwindow;
	GtkWidget* logwindow;
	GtkWidget* dupwin;
	GList *syncpairs;
	GList *optionplugins;
} MSyncEnv;

typedef struct MSyncPair {
	OSyncGroup *group;
	OSyncEngine *engine;
	int read;
	int written;
	osync_bool error;
} MSyncPair;

typedef void (* MSyncOptionFunction) (MSyncEnv *, OSyncMember *);

typedef struct MSyncPlugin {
	char *name;
	MSyncOptionFunction options;
} MSyncPlugin;

MSyncPair *msync_pair_new(void);
MSyncPlugin *msync_find_plugin(MSyncEnv *env, const char *);
void msync_register_plugin(MSyncEnv *env, const char *name, MSyncOptionFunction function);
void msync_show_msg_info(const char *msg);
void msync_show_msg_warn(const char *msg);
void msync_set_pairlist_status(MSyncPair *pair, const char *message, ...);
void msync_register_plugins(MSyncEnv *env);
void msync_start_groups(void);
void msync_solve_conflict_duplicate(void);
void msync_solve_conflict_choose(int nth);

void msync_pairs_load(MSyncEnv *env);
MSyncPair *msync_pair_new(void);
void msync_pair_free(MSyncPair *pair);

void msync_register_evo2_sync(MSyncEnv *env);
void msync_register_evo_sync(MSyncEnv *env);
void msync_register_file_sync(MSyncEnv *env);
