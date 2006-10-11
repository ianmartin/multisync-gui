#ifndef MULTISYNC_H_
#define MULTISYNC_H_

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <glade/glade.h>
#include <opensync/opensync.h>
#include <osengine/engine.h>

#ifndef MULTISYNC_GLADE
#define MULTISYNC_GLADE "multisync.glade"
#endif

typedef struct MSyncEnv MSyncEnv;
typedef struct MSyncGroup MSyncGroup;
typedef struct MSyncPlugin MSyncPlugin;
typedef void (* MSyncPluginSetConfig)(MSyncPlugin*, const char *);
typedef const char* (* MSyncPluginGetConfig)(MSyncPlugin*);

#include "msync_env.h"
#include "msync_group.h"
#include "msync_plugin.h"
#include "msync_support.h"
#include "msync_callbacks.h"
#include "plugins/msync_default.h"

#endif /*MULTISYNC_H_*/
