#include "multisync.h"

MSyncEnv *env;

void msync_dupe_window_close(void)
{
	gtk_widget_destroy(env->dupwin);
	env->dupwin = NULL;
}

void msync_solve_conflict_duplicate(void)
{
	MSyncPair *pair = (MSyncPair *)gtk_object_get_data(GTK_OBJECT(env->dupwin), "pair");
	OSyncMapping *mapping = (OSyncMapping *)gtk_object_get_data(GTK_OBJECT(env->dupwin), "mapping");
	osengine_mapping_duplicate(pair->engine, mapping);
	msync_dupe_window_close();
}

void msync_solve_conflict_choose(int nth)
{
	MSyncPair *pair = (MSyncPair *)gtk_object_get_data(GTK_OBJECT(env->dupwin), "pair");
	OSyncMapping *mapping = (OSyncMapping *)gtk_object_get_data(GTK_OBJECT(env->dupwin), "mapping");
	OSyncChange *change = osengine_mapping_nth_change(mapping, nth);
	g_assert(change);
	osengine_mapping_solve(pair->engine, mapping, change);
	//osync_mapping_multiply_master(pair->engine, mapping);
	msync_dupe_window_close();
}

void conflict_handler(OSyncEngine *engine, OSyncMapping *mapping, void *user_data)
{
	MSyncPair *pair = (MSyncPair *)user_data;
	GtkTextBuffer *buffer;
	char *tmp;
	OSyncChange *change;
	
	printf("Handling conflict\n");

	env->dupwin = create_duplicatewin();
	gtk_object_set_data(GTK_OBJECT(env->dupwin), "pair", pair);
	gtk_object_set_data(GTK_OBJECT(env->dupwin), "mapping", mapping);

	gtk_label_set_label(GTK_LABEL(lookup_widget(env->dupwin, "reasonlabel")),
			"A conflict occured for these entries. Either the modifications you made are not the same or you modified in on one side and deleted the other\n"
			"How should MultiSync proceed?");

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(lookup_widget(env->dupwin, "firsttextview")));
	change = osengine_mapping_nth_change(mapping, 0);
	gtk_text_buffer_set_text(buffer, osync_change_get_data(change), osync_change_get_datasize(change));
	tmp = g_strdup_printf("Entry from %s (first plugin):", osync_member_get_pluginname(osync_change_get_member(change)));
	gtk_label_set_label(GTK_LABEL(lookup_widget(env->dupwin, "firstlabel")), tmp);
    g_free(tmp);

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(lookup_widget(env->dupwin, "secondtextview")));
	change = osengine_mapping_nth_change(mapping, 1);
	gtk_text_buffer_set_text(buffer, osync_change_get_data(change), osync_change_get_datasize(change));
	tmp = g_strdup_printf("Entry from %s (second plugin):", osync_member_get_pluginname(osync_change_get_member(change)));
	gtk_label_set_label(GTK_LABEL(lookup_widget(env->dupwin, "secondlabel")), tmp);
    g_free(tmp);

	tmp = g_strdup_printf("<span weight='bold'>%s</span>", osync_group_get_name(pair->group));
	gtk_label_set_label(GTK_LABEL(lookup_widget(env->dupwin, "syncpairlabel")), tmp);
	g_free(tmp);

	gtk_widget_show(env->dupwin);
}

void entry_status(OSyncEngine *engine, OSyncChangeUpdate *status, void *user_data)
{
	MSyncPair *pair = (MSyncPair *)user_data;
	switch (status->type) {
		case CHANGE_RECEIVED:
			if (osync_change_has_data(status->change))
				pair->read++;
			break;
		case CHANGE_SENT:
			pair->written++;
			break;
		default:
			printf("Not handling entry_status %i\n", status->type);
	}
	msync_set_pairlist_status(pair, "Syncing. Read %i entries, Wrote %i entries", pair->read, pair->written);
}

void engine_status(OSyncEngine *engine, OSyncEngineUpdate *status, void *user_data)
{
	MSyncPair *pair = (MSyncPair *)user_data;
	if (status->type == ENG_ENDPHASE_DISCON)
		msync_set_pairlist_status(pair, "Synced Successfully. Read %i entries, Wrote %i entries", pair->read, pair->written);
	if (status->type == ENG_PREV_UNCLEAN)
		msync_set_pairlist_status(pair, "Unclean shutdown detected. Slow-syncing now");
}

void msync_start_groups(void)
{
	GList *p;
	for (p = env->syncpairs; p; p = p->next) {
		MSyncPair *pair = p->data;
		OSyncError *error = NULL;
		if (pair->engine)
			continue;
		pair->engine = osengine_new(pair->group, &error);
		if (!pair->engine) {
			printf("Error while creating syncengine: %s\n", osync_error_print(&error));
			msync_set_pairlist_status(pair, "Error initializing");
			osync_error_free(&error);
			continue;
		}
		msync_set_pairlist_status(pair, "Ready");
		//osengine_set_memberstatus_callback(engine, member_status);
		osengine_set_changestatus_callback(pair->engine, entry_status, (void *)pair);
		osengine_set_enginestatus_callback(pair->engine, engine_status, (void *)pair);
		//osengine_set_mappingstatus_callback(engine, mapping_status);
		osengine_set_conflict_callback(pair->engine, conflict_handler, (void *)pair);
		//osengine_set_message_callback(engine, plgmsg_function, "palm_uid_mismatch", NULL);
		//osengine_flag_only_info(engine);
		
		if (!osengine_init(pair->engine, &error)) {
			char *message = g_strdup_printf("Error initializing: %s", osync_error_print(&error));
			msync_set_pairlist_status(pair, message);
			g_free(message);
			pair->error = osync_error_get_type(&error);
			osync_error_free(&error);
			continue;
		}
		
		
	}
}

int main (int argc, char *argv[])
{
	OSyncError *error = NULL;
  gnome_program_init (PACKAGE, VERSION, LIBGNOMEUI_MODULE, argc, argv, GNOME_PARAM_APP_DATADIR, PACKAGE_DATA_DIR, NULL);

	printf("Starting up!\n");
	env = g_malloc0(sizeof(MSyncEnv));
	env->osync = osync_env_new();
	
	msync_register_plugins(env);
	
	if (!osync_env_initialize(env->osync, &error)) {
		printf("Unable to initialize: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return 1;
	}

	msync_pairs_load(env);
	env->mainwindow = create_mainwindow();
	msync_open_mainwindow(env);
	
	msync_start_groups();
  /*syncpairwin = create_syncpairwin ();
  gtk_widget_show (syncpairwin);
  aboutdialog = create_aboutdialog ();
  gtk_widget_show (aboutdialog);
  logwindow = create_logwindow ();
  gtk_widget_show (logwindow);*/
  /*duplicatewin = create_duplicatewin ();
  gtk_widget_show (duplicatewin);*/

	printf("About to run gtk_main\n");
  gtk_main ();
  return 0;
}

