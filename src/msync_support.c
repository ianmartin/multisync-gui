#include "multisync.h"

void msync_error_message(GtkWindow* window, gboolean threadsafe, char *format, ...)
{
	GtkWidget *dialog;
	GtkWidget *label;
	GtkWidget* hbox;
	GtkWidget* image;
	gchar* msg;
	
	if(threadsafe) {
		gdk_threads_enter();
	}
	
	dialog = gtk_dialog_new_with_buttons ("Error Message",
                                         window,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_NONE,
                                         NULL);
	
	GError **gerror = NULL;
	GdkPixbuf *pixbuf = NULL;
	pixbuf = gdk_pixbuf_new_from_file(MULTISYNCGUI_ICON, gerror);
  	if(pixbuf) {
		gtk_window_set_icon (GTK_WINDOW (dialog), pixbuf);
		gdk_pixbuf_unref (pixbuf);
    }
	
	hbox = gtk_hbox_new (FALSE, 10);
	gtk_widget_show (hbox);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 10);
	
	image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);	
	gtk_widget_show(image);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

	va_list args;
	va_start(args, format);
	msg = g_strdup_vprintf((gchar *)format, args);
	va_end(args);
	label = gtk_label_new(msg);
	g_free(msg);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
	gtk_widget_show_all(dialog);
	
	if(threadsafe) {
		gdk_flush();	
		gdk_threads_leave ();	
	}
}

const char *OSyncChangeType2String(OSyncChangeType changetype)
{
	switch (changetype) {
		case OSYNC_CHANGE_TYPE_ADDED: return "ADDED";
		case OSYNC_CHANGE_TYPE_UNMODIFIED: return "UNMODIFIED";
		case OSYNC_CHANGE_TYPE_DELETED: return "DELETED";
		case OSYNC_CHANGE_TYPE_MODIFIED: return "MODIFIED";
		case OSYNC_CHANGE_TYPE_UNKNOWN: return "?";
	}
	return NULL;
}
