#ifndef GUI_H
#define GUI_H
#include <glib.h>
#include <gtk/gtkmain.h>

gboolean msync_open_mainwindow(gpointer data);
gboolean msync_okcanceldialog(char *text, gboolean okcancel);
gboolean msync_open_syncpairwindow(gpointer data);

void msync_open_localplugin_window(void);
void msync_open_remoteplugin_window(void);
void msync_cancel_syncpairwindow(void);
void msync_ok_syncpairwindow(void);
void msync_edit_pair(void);
void msync_main_quit(void);
void msync_delete_pair(void);
void msync_synchronize(void);

#endif
