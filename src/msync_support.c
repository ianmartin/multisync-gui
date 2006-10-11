#include "multisync.h"

void msync_error_message(GtkWindow* window, gboolean threadsafe, char *format, ...)
{
	GtkWidget *dialog;
	GtkWidget *label;
	gchar* msg;
	
	if(threadsafe) {
		gdk_threads_enter();
	}
	
	va_list args;
	va_start(args, format);
	msg = g_strdup_vprintf((gchar *)format, args);
	va_end(args);
	
	dialog = gtk_dialog_new_with_buttons ("Error Message",
                                         window,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_NONE,
                                         NULL);
	label = gtk_label_new(msg);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), label);
	g_free(msg);
	
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
	gtk_widget_show_all (dialog);
	
	if(threadsafe) {
		gdk_flush();	
		gdk_threads_leave ();	
	}
}
