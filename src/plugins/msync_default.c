#include "../multisync.h"

GtkWidget* msync_defaultplugin_create_widget(void) {
  GtkWidget* textview = gtk_text_view_new();
  return textview;
}

void msync_defaultplugin_set_config(MSyncPlugin* plugin, const char *config)
{
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(plugin->widget)), (const gchar *)config, -1);
}

const char* msync_defaultplugin_get_config(MSyncPlugin* plugin)
{
	GtkTextIter start;
	GtkTextIter end;
	GtkTextBuffer* buffer= gtk_text_view_get_buffer(GTK_TEXT_VIEW(plugin->widget));
	gtk_text_buffer_get_bounds(buffer, &start, &end);	
	return (char *)gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}
