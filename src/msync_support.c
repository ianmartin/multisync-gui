#include "multisync.h"

void msync_error_message(GtkWindow* window, char *format, ...)
{
	GtkWidget *dialog;
	GtkWidget *label;
	gchar* msg;
	
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
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE); 
	label = gtk_label_new(msg);
	g_free(msg);
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), label);
	gtk_widget_show_all (dialog);
}
