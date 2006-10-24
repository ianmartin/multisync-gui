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
#include <osengine/engine.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /*HAVE_CONFIG_H*/

#ifndef MULTISYNC_GLADE
#define MULTISYNC_GLADE "multisync.glade"
#endif /*MULTISYNC_GLADE*/

#ifndef MULTISYNC_ICON
#define MULTISYNC_ICON "../misc/multisync.png"
#endif /*MULTISYNC_ICON*/


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
