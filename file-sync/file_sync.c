#include "multisync.h"
#include "interface.h"
#include "callbacks.h"

GtkWidget *file_wnd_options;
OSyncMember *member;

void msync_file_sync_options(MSyncEnv *env, OSyncMember *target)
{
	char *config;
	int size;
	member = target;
	OSyncError *error = NULL;
	if (!osync_member_get_config(member, &config, &size, &error)) {
		printf("Unable to get config: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	printf("showing options for plugin file-sync!\n");
	file_wnd_options = create_wnd_options();
	GtkEntry *entry = GTK_ENTRY(lookup_widget(file_wnd_options, "txt_path"));
	gtk_entry_set_text(entry, g_strstrip(g_strdup(config)));
	gtk_widget_show (file_wnd_options);
	g_free(config);
}

void msync_register_file_sync(MSyncEnv *env)
{
	msync_register_plugin(env, "file-sync", msync_file_sync_options);
}

void
on_wnd_options_destroy                 (GtkObject       *object,
                                        gpointer         user_data)
{
	printf("release controls now\n");
}

void
on_btn_cancel_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(file_wnd_options);
	file_wnd_options = NULL;
}

void
on_btn_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkEntry *entry = GTK_ENTRY(lookup_widget(file_wnd_options, "txt_path"));
	const char *config = gtk_entry_get_text(entry);
	osync_member_set_config(member, config, strlen(config) + 1);
	gtk_widget_destroy(file_wnd_options);
	file_wnd_options = NULL;
}
