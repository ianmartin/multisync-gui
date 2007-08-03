#ifndef MULTISYNC_H_
#define MULTISYNC_H_

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <glade/glade.h>
#include <libxml/parser.h>
#include <opensync/opensync.h>
#include <opensync/opensync-group.h>
#include <opensync/opensync-format.h>
#include <opensync/opensync-plugin.h>
#include <opensync/opensync-engine.h>
#include <opensync/opensync-data.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /*HAVE_CONFIG_H*/

#ifndef MULTISYNC_GLADE
#define MULTISYNC_GLADE "multisync.glade"
#endif /*MULTISYNC_GLADE*/

#ifndef MULTISYNC_ICON
#define MULTISYNC_ICON "../misc/multisync.png"
#endif /*MULTISYNC_ICON*/

//#ifndef MULTISYNC_LEGACY
//#define MULTISYNC_LEGACY
//#endif /*MULTISYNC_LEGACY*/

#ifndef MULTISYNC_LEGACY_FILTERFILE
#define MULTISYNC_LEGACY_FILTERFILE "filter.conf"
#endif /*MULTISYNC_LEGACY_FILTERFILE*/

typedef struct MSyncEnv MSyncEnv;
typedef struct MSyncGroup MSyncGroup;
typedef struct MSyncPlugin MSyncPlugin;
typedef void (* MSyncPluginSetConfig)(MSyncPlugin*, OSyncMember *member, const char *);
typedef const char* (* MSyncPluginGetConfig)(MSyncPlugin*);

#include "msync_env.h"
#include "msync_group.h"
#include "msync_plugin.h"
#include "msync_support.h"
#include "msync_callbacks.h"
#include "plugins/msync_default.h"
#include "plugins/msync_evo2sync.h"

#endif /*MULTISYNC_H_*/
