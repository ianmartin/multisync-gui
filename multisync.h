#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <glib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <gdk/gdk.h>
#include <gnome.h>
#include <libgnomeui/gnome-window-icon.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

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
	GList *syncpairs;
	GList *optionplugins;
} MSyncEnv;

typedef struct MSyncPair {
	OSyncGroup *group;
} MSyncPair;

typedef void (* MSyncOptionFunction) (MSyncEnv *, OSyncMember *);

typedef struct MSyncPlugin {
	char *name;
	MSyncOptionFunction options;
} MSyncPlugin;

MSyncPair *msync_pair_new(void);
MSyncPlugin *msync_find_plugin(MSyncEnv *env, const char *);
