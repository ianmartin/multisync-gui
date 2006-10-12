#include "multisync.h"

void msync_group_new(MSyncEnv *env, OSyncGroup *osyncgroup)
{
	MSyncGroup *group = g_malloc0(sizeof(MSyncGroup));
	env->groups = g_list_append(env->groups, group);
	group->msyncenv = env;
	group->group = osyncgroup;
	group->go = FALSE;
	group->cond = g_cond_new ();
	group->mutex = g_mutex_new ();
	group->widget = msync_group_create_widget(group);
	gtk_box_pack_start (GTK_BOX (env->groupcontainer), group->widget, FALSE, FALSE, 0);
	
	msync_group_create_syncronizegroupconflictdialog(group);
	msync_group_update_last_synchronization_status(group, FALSE);
}

void msync_group_free(MSyncGroup *group)
{
//  /* we need the xmpm stop patch */
//	if(group->engine) {
//		OSyncError* error;
//		osengine_abort(group->engine);
//		osengine_finalize(group->engine);
//		osengine_free(group->engine);
//	}
	
	gtk_widget_destroy(group->conflictdialog);
	
	g_list_free(group->memberstatuslabel);
	gtk_widget_destroy(group->widget);
	group->msyncenv->groups = g_list_remove(group->msyncenv->groups, group);
	g_free(group);
}

void msync_group_remove(MSyncGroup *group)
{
	OSyncError *error = NULL;
	if (!osync_group_delete(group->group, &error)) {
		msync_error_message(GTK_WINDOW(group->msyncenv), FALSE, "Unable to delete group %s: %s\n", osync_group_get_name(group->group), osync_error_print(&error));
		osync_error_free(&error);
	}
	msync_group_free(group);
}

void msync_group_syncronize(MSyncGroup *group)
{
	g_thread_create((GThreadFunc)msync_group_syncronize2, group, FALSE, NULL);
}

void msync_group_syncronize2(MSyncGroup *group)
{
	OSyncError *error = NULL;
	OSyncMember *member = NULL;
	gboolean wait = FALSE;
	int i, num;
	
	msync_group_set_sensitive(group, TRUE, FALSE);
	group->resolution = MSYNC_RESOLUTION_UNKNOWN;
	group->winningside = 0;
	group->entries_reveived = 0;
	group->entries_sended = 0;
	
	group->engine = osengine_new(group->group, &error);
	if (!group->engine) {
		msync_error_message(GTK_WINDOW(group->msyncenv->mainwindow), TRUE, "Error while creating syncengine: %s\n", osync_error_print(&error));
		goto error;
	}
	
	osengine_set_enginestatus_callback(group->engine, msync_group_syncronize_enginestatus, group);
	osengine_set_memberstatus_callback(group->engine, msync_group_syncronize_memberstatus, group);
	osengine_set_changestatus_callback(group->engine, msync_group_syncronize_entrystatus, group);
	osengine_set_mappingstatus_callback(group->engine, msync_group_syncronize_mappingstatus, NULL);
	osengine_set_conflict_callback(group->engine, msync_group_syncronize_conflict, group);
	
	if (!osengine_init(group->engine, &error)) {
		msync_error_message(GTK_WINDOW(group->msyncenv->mainwindow), TRUE, "Error while initializing syncengine: %s\n", osync_error_print(&error));
		goto error_free_engine;
	}

	num = osync_group_num_members(group->group);
	for(i=0; i<num; i++)
	{
		member = osync_group_nth_member(group->group, i);
		const char* name = osync_member_get_pluginname(member);
		if (strcmp(name, "syncml-http-server") == 0 ||
			strcmp(name, "palm-sync") == 0) {
				wait = TRUE;
				break;
		}
	}
	
	if(!wait)
		osengine_sync_and_block(group->engine, &error);
	else
		osengine_wait_sync_end(group->engine, &error);
		
	if(error) {
		msync_error_message(GTK_WINDOW(group->msyncenv->mainwindow), TRUE, "Error synchronizing: %s\n", osync_error_print(&error));
		goto error_finalize;
	}
	osync_group_set_last_synchronization(group->group, time(NULL));
	if (!osync_group_save(group->group, &error)) {
		msync_error_message(GTK_WINDOW(group->msyncenv->mainwindow), FALSE, "Unable to save group: %s\n", osync_error_print(&error));
		goto error_finalize;
	}
	osengine_finalize(group->engine);
	osengine_free(group->engine);
	group->engine = NULL;	
	msync_group_set_sensitive(group, TRUE, TRUE);
	return;

error_finalize:
	osengine_finalize(group->engine);
error_free_engine:
	osengine_free(group->engine);
	group->engine = NULL;
error:
	osync_error_free(&error);
	msync_group_set_sensitive(group, TRUE, TRUE);
}

void msync_group_syncronize_enginestatus(OSyncEngine *engine, OSyncEngineUpdate *status, void *user_data)
{
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup *)user_data;

	switch (status->type) {
		case ENG_PREV_UNCLEAN:
			printf("The previous synchronization was unclean. Slow-syncing\n");
			msync_group_update_engine_status(group, TRUE,"The previous synchronization was unclean. Slow-syncing");
			break;
		case ENG_ENDPHASE_CON:
			printf("All clients connected or error\n");
			msync_group_update_engine_status(group, TRUE, "All clients connected or error");
			break;
		case ENG_END_CONFLICTS:
			printf("All conflicts have been reported\n");
			msync_group_update_engine_status(group, TRUE, "All conflicts have been reported");
			break;
		case ENG_ENDPHASE_READ:
			printf("All clients sent changes or error\n");
			msync_group_update_engine_status(group, TRUE, "All clients sent changes or error");
			break;
		case ENG_ENDPHASE_WRITE:
			printf("All clients have written\n");
			msync_group_update_engine_status(group, TRUE, "All clients have written");
			break;
		case ENG_ENDPHASE_DISCON:
			printf("All clients have disconnected\n");
			msync_group_update_engine_status(group, TRUE, "All clients have disconnected");
			break;
		case ENG_SYNC_SUCCESSFULL:
			printf("The sync was successful\n");
			msync_group_update_engine_status(group, TRUE, "The sync was successful");
			msync_group_update_entry_status(group, TRUE, "");
			break;
		case ENG_ERROR:
			printf("The sync failed: %s\n", osync_error_print(&(status->error)));
			msync_group_update_engine_status(group, TRUE, "The sync failed: %s", osync_error_print(&(status->error)));
			msync_group_update_entry_status(group, TRUE, "");
			break;
	}
}

void msync_group_syncronize_memberstatus(OSyncMemberUpdate *status, void *user_data)
{
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup *)user_data;

	switch (status->type) {
		case MEMBER_CONNECTED:
			msync_group_update_member_status(group, status->member, "Connected");
			printf("Member %lli of type %s just connected\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member));
			break;
		case MEMBER_DISCONNECTED:
			msync_group_update_member_status(group, status->member, "Disconnected");
			printf("Member %lli of type %s just disconnected\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member));
			break;
		case MEMBER_SENT_CHANGES:
			msync_group_update_member_status(group, status->member, "Sent all changes");
			printf("Member %lli of type %s just sent all changes\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member));
			break;
		case MEMBER_COMMITTED_ALL:
			msync_group_update_member_status(group, status->member, "Committed all changes");
			printf("Member %lli of type %s committed all changes.\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member));
			break;
		case MEMBER_CONNECT_ERROR:
			msync_group_update_member_status(group, status->member, "Error while connecting");
			printf("Member %lli of type %s had an error while connecting: %s\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member), osync_error_print(&(status->error)));
			break;
		case MEMBER_GET_CHANGES_ERROR:
			msync_group_update_member_status(group, status->member, "Error while getting changes");
			printf("Member %lli of type %s had an error while getting changes: %s\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member), osync_error_print(&(status->error)));
			break;
		case MEMBER_SYNC_DONE_ERROR:
			msync_group_update_member_status(group, status->member, "Error while calling sync done");
			printf("Member %lli of type %s had an error while calling sync done: %s\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member), osync_error_print(&(status->error)));
			break;
		case MEMBER_DISCONNECT_ERROR:
			msync_group_update_member_status(group, status->member, "Error while disconnecting");
			printf("Member %lli of type %s had an error while disconnecting: %s\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member), osync_error_print(&(status->error)));
			break;
		case MEMBER_COMMITTED_ALL_ERROR:
			msync_group_update_member_status(group, status->member, "Error while commiting changes");
			printf("Member %lli of type %s had an error while commiting changes: %s\n", osync_member_get_id(status->member), osync_member_get_pluginname(status->member), osync_error_print(&(status->error)));
			break;
	}
}

void msync_group_syncronize_mappingstatus(OSyncMappingUpdate *status, void *user_data)
{
	switch (status->type) {
		case MAPPING_SOLVED:
			printf("Mapping solved\n");
			break;
		case MAPPING_SYNCED:
			printf("Mapping Synced\n");
			break;
		case MAPPING_WRITE_ERROR:
			printf("Mapping Write Error: %s\n", osync_error_print(&(status->error)));
			break;
	}
}

void msync_group_syncronize_entrystatus(OSyncEngine *engine, OSyncChangeUpdate *status, void *user_data)
{
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup *)user_data;
	
	switch (status->type) {
		case CHANGE_RECEIVED_INFO:
			printf("Received an entry %s without data from member %i. Changetype %s\n",
					osync_change_get_uid(status->change), status->member_id,
					OSyncChangeType2String(osync_change_get_changetype(status->change)));
			break;
		case CHANGE_RECEIVED:
			group->entries_reveived++;
			msync_group_update_entry_status(group, TRUE, "%i entries received", group->entries_reveived);
			printf("Received an entry %s with data of size %i from member %i. Changetype %s\n",
					osync_change_get_uid(status->change),
					osync_change_get_datasize(status->change),
					status->member_id,
					OSyncChangeType2String(osync_change_get_changetype(status->change)));
			break;
		case CHANGE_SENT:
			group->entries_sended++;
			msync_group_update_entry_status(group, TRUE, "%i entries sent", group->entries_sended);
			printf("Sent an entry %s of size %i to member %i. Changetype %s\n",
					osync_change_get_uid(status->change),
					osync_change_get_datasize(status->change),
					status->member_id,
					OSyncChangeType2String(osync_change_get_changetype(status->change)));
			break;
		case CHANGE_WRITE_ERROR:
			printf("Error writing entry %s to member %i: %s\n",
					osync_change_get_uid(status->change),
					status->member_id,
					osync_error_print(&(status->error)));
			break;
		case CHANGE_RECV_ERROR:
			printf("Error reading entry %s from member %i: %s\n", osync_change_get_uid(status->change), status->member_id, osync_error_print(&(status->error)));
			break;
	}
}


void msync_group_syncronize_conflict(OSyncEngine *engine, OSyncMapping *mapping, void *user_data)
{
	OSyncError	*error = NULL;
	OSyncChange *change = NULL;
	
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup *)user_data;

	gdk_threads_enter();
		
	GList* childs = gtk_container_get_children(GTK_CONTAINER(group->conflictcontainer));
	if(!childs)
		printf("no childs1\n");
	g_list_foreach(childs, (GFunc)gtk_widget_destroy, NULL);
	g_list_free(childs);

	childs = gtk_container_get_children(GTK_CONTAINER(group->conflictbuttons));
	if(!childs)
		printf("no childs\n");
	g_list_foreach(childs, (GFunc)gtk_widget_destroy, NULL);
	g_list_free(childs);

	if (osengine_mapping_ignore_supported(engine, mapping)) {
		GtkWidget* button3 = gtk_button_new_with_label ("Ignore");
		gtk_widget_show (button3);
		gtk_box_pack_start (GTK_BOX (group->conflictbuttons), button3, TRUE, TRUE, 0);
		GTK_WIDGET_UNSET_FLAGS (button3, GTK_CAN_FOCUS);
		g_signal_connect_swapped(G_OBJECT(button3), "clicked", G_CALLBACK(msync_group_syncronize_mapping_ignore), group);
	}
//	GtkWidget* button4 = gtk_button_new_with_label ("Duplicate");
//	gtk_widget_show (button4);
//	gtk_dialog_add_action_widget (GTK_DIALOG (group->conflictdialog), button4, 0);
//	GTK_WIDGET_UNSET_FLAGS (button4, GTK_CAN_FOCUS);
//	g_signal_connect_swapped(G_OBJECT(button4), "clicked", G_CALLBACK(msync_group_syncronize_mapping_duplicate), group);

	GtkWidget* button6 = gtk_button_new_with_label ("Newer");
	gtk_widget_show (button6);
	gtk_box_pack_start (GTK_BOX(group->conflictbuttons), button6, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS (button6, GTK_CAN_FOCUS);
	g_signal_connect_swapped(G_OBJECT(button6), "clicked", G_CALLBACK(msync_group_syncronize_mapping_newer), group);

	int i = 0;
	for (i = 0; i < osengine_mapping_num_changes(mapping); i++) {
		OSyncChange *change = osengine_mapping_nth_change(mapping, i);
		if (osync_change_get_changetype(change) != CHANGE_UNKNOWN)
		{
		  GtkWidget* vbox7 = gtk_vbox_new (FALSE, 10);
		  gtk_widget_show (vbox7);
		  gtk_box_pack_start (GTK_BOX (group->conflictcontainer), vbox7, TRUE, TRUE, 0);
		
		  GtkWidget* hbox8 = gtk_hbox_new (FALSE, 10);
		  gtk_widget_show (hbox8);
		  gtk_box_pack_start (GTK_BOX (vbox7), hbox8, FALSE, FALSE, 0);
		
	gchar* text = g_strdup_printf("Member: %s\nUID: %s", osync_member_get_pluginname(osync_change_get_member(change)), osync_change_get_uid(change));
	GtkWidget* label11 = gtk_label_new (text);
	g_free(text);
		  gtk_widget_show (label11);
		  gtk_box_pack_start (GTK_BOX (hbox8), label11, FALSE, FALSE, 0);
		
		  GtkWidget* button9 = gtk_button_new_from_stock ("gtk-apply");
		  gtk_widget_show (button9);
		  gtk_box_pack_start (GTK_BOX (hbox8), button9, FALSE, FALSE, 0);
		  GTK_WIDGET_UNSET_FLAGS (button9, GTK_CAN_FOCUS);
	g_signal_connect(G_OBJECT(button9), "clicked", G_CALLBACK(msync_group_syncronize_mapping_select), group);
	g_object_set_data(G_OBJECT(button9), "msync_change", (void *)i);
		
		  GtkWidget* scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
		  gtk_widget_show (scrolledwindow4);
		  gtk_box_pack_start (GTK_BOX (vbox7), scrolledwindow4, TRUE, TRUE, 0);
		  gtk_widget_set_size_request (scrolledwindow4, 300, 300);
		  GTK_WIDGET_UNSET_FLAGS (scrolledwindow4, GTK_CAN_FOCUS);
		  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		
		  GtkWidget* viewport2 = gtk_viewport_new (NULL, NULL);
		  gtk_widget_show (viewport2);
		  gtk_container_add (GTK_CONTAINER (scrolledwindow4), viewport2);
		
		  GtkWidget* textview1 = gtk_text_view_new ();
		  gtk_widget_show (textview1);
		  gtk_container_add (GTK_CONTAINER (viewport2), textview1);
		  GTK_WIDGET_UNSET_FLAGS (textview1, GTK_CAN_FOCUS);
	char* tmp = osync_change_get_printable(change);
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1)), (gchar *)tmp, -1);
	free(tmp);
		}
	}
	gdk_flush();	
	gdk_threads_leave ();
	

	if(group->resolution == MSYNC_RESOLUTION_UNKNOWN)
	{
		gdk_threads_enter();
		gtk_widget_show(group->conflictdialog);
		gdk_flush();	
		gdk_threads_leave ();
		
waitforuser:	
		g_mutex_lock (group->mutex);
		while(group->go != TRUE)
			g_cond_wait(group->cond, group->mutex);
		group->go = FALSE;
		g_mutex_unlock (group->mutex);
	}	

	

	switch(group->resolution) {
		case MSYNC_RESOLUTION_DUPLICATE:
			//osengine_mapping_duplicate(group->engine, mapping);
			break;
		case MSYNC_RESOLUTION_IGNORE:
			if (!osengine_mapping_ignore_conflict(group->engine, mapping, &error)) {
				msync_group_syncronize_conflictdialog_show(group, TRUE);
				msync_error_message(GTK_WINDOW(group->conflictdialog), TRUE, "Conflict not ignored: %s\n", osync_error_print(&error));
				osync_error_free(&error);
				goto waitforuser;
			}
			break;
		case MSYNC_RESOLUTION_NEWER:
			if (!osengine_mapping_solve_latest(group->engine, mapping, &error)) {
				msync_group_syncronize_conflictdialog_show(group, TRUE);
				msync_error_message(GTK_WINDOW(group->conflictdialog), TRUE, "Conflict not resolved: %s\n", osync_error_print(&error));
				osync_error_free(&error);
				goto waitforuser;
			}
			break;
		case MSYNC_RESOLUTION_SELECT:
			change = osengine_mapping_nth_change(mapping, group->winningside);
			g_assert(change);
			osengine_mapping_solve(group->engine, mapping, change);
			break;
		case MSYNC_RESOLUTION_UNKNOWN:
			g_assert_not_reached();
	}

	gdk_threads_enter();
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(group->conflictcheckbuttonremember)) != TRUE)
	{
		group->resolution = MSYNC_RESOLUTION_UNKNOWN;
		group->winningside = 0;		
	}
	gtk_widget_hide(group->conflictdialog);
	
	gdk_flush();	
	gdk_threads_leave ();
}

void msync_group_syncronize_conflictdialog_show(MSyncGroup *group, gboolean threadsafe)
{
	if(threadsafe) {
		gdk_threads_enter();
	}
	
	gtk_widget_show(group->conflictdialog);
	
	if(threadsafe) {
		gdk_flush();	
		gdk_threads_leave ();
	}
}

void msync_group_syncronize_unlock_conflict_dialog(MSyncGroup* group)
{
	g_mutex_lock (group->mutex);
	group->go = TRUE;
	g_cond_signal(group->cond);
	g_mutex_unlock (group->mutex);	
}

void msync_group_syncronize_mapping_duplicate(MSyncGroup* group)
{
	group->resolution = MSYNC_RESOLUTION_DUPLICATE;	
	msync_group_syncronize_unlock_conflict_dialog(group);
}

void msync_group_syncronize_mapping_ignore(MSyncGroup* group)
{
	group->resolution = MSYNC_RESOLUTION_IGNORE;
	msync_group_syncronize_unlock_conflict_dialog(group);
}

void msync_group_syncronize_mapping_newer(MSyncGroup* group)
{
	group->resolution = MSYNC_RESOLUTION_NEWER;
	msync_group_syncronize_unlock_conflict_dialog(group);
}

void msync_group_syncronize_mapping_select(GtkButton* button, MSyncGroup* group)
{
	group->resolution = MSYNC_RESOLUTION_SELECT;
	group->winningside = (int)g_object_get_data(G_OBJECT(button), "msync_change");
	msync_group_syncronize_unlock_conflict_dialog(group);
}

void msync_group_update_engine_status(MSyncGroup *group, gboolean gtkthreadsafe, char *format, ...)
{
	if(gtkthreadsafe) {
		gdk_threads_enter();
	}
	
	va_list args;
	va_start(args, format);
	gchar *msg = g_strdup_vprintf((gchar *)format, args);
	va_end(args);
	gtk_label_set_text(GTK_LABEL(group->enginelabel), (const gchar *)msg);
	g_free(msg);
	
	if(gtkthreadsafe) {
		gdk_flush();	
		gdk_threads_leave ();
	}
}

void msync_group_update_member_status(MSyncGroup *group, OSyncMember *member, const char* msg)
{
	gdk_threads_enter ();
	
	gchar* str = g_strdup_printf("%lli", osync_member_get_id(member));
	int i = atoi((char *)str);
	g_free(str);
	GList* tmp = g_list_nth(group->memberstatuslabel, i-1);
	GtkLabel* label = GTK_LABEL(tmp->data);	
	gtk_label_set_text(label, msg);
	
	gdk_flush();	
	gdk_threads_leave ();
}

void msync_group_update_entry_status(MSyncGroup *group, gboolean gtkthreadsafe, char *format, ...)
{
	if(gtkthreadsafe) {
		gdk_threads_enter();
	}

	va_list args;
	va_start(args, format);
	gchar *msg = g_strdup_vprintf((gchar *)format, args);
	va_end(args);
	gtk_label_set_text(GTK_LABEL(group->entrylabel), (const gchar *)msg);
	gtk_label_set_use_markup(GTK_LABEL(group->entrylabel), TRUE);
	g_free(msg);
	
	if(gtkthreadsafe) {
		gdk_flush();	
		gdk_threads_leave ();
	}
}

void msync_group_update_last_synchronization_status(MSyncGroup *group, gboolean gtkthreadsafe)
{
	char data[200];
	int slen;
	
	time_t timep = osync_group_get_last_synchronization(group->group);
	if(timep)
	{
		struct tm* result = localtime(&timep);
		strftime(data, slen, "Last syncronized on: %d/%m/%Y %H:%M", result);
		msync_group_update_engine_status(group, gtkthreadsafe, data);
	}else{
		msync_group_update_engine_status(group, gtkthreadsafe, "Not syncronized yet.");
	}
}

void msync_group_set_sensitive(MSyncGroup *group, gboolean gtkthreadsafe, gboolean sensitive)
{
	if(gtkthreadsafe) {
		gdk_threads_enter();
	}
	
	gtk_widget_set_sensitive(group->buttondelete, sensitive);
	gtk_widget_set_sensitive(group->buttonedit, sensitive);
	gtk_widget_set_sensitive(group->buttonsyncronize, sensitive);

	if(gtkthreadsafe) {
		gdk_flush();	
		gdk_threads_leave ();
	}
}

void msync_group_update_widget(MSyncGroup *group)
{
	GList* list = gtk_container_get_children(GTK_CONTAINER(group->vbox));
	g_list_foreach(list, (GFunc)gtk_widget_destroy, NULL);
	g_list_free(list);
	
	if(group->memberstatuslabel) {
		g_list_free(group->memberstatuslabel);
		group->memberstatuslabel = NULL;
	}
  	gchar* tmp = g_strdup_printf("<span weight=\"bold\" size=\"larger\">Group: %s</span>", osync_group_get_name(group->group));
  	gtk_label_set_markup(GTK_LABEL(group->grouplabel), tmp);
  	g_free(tmp);
	
	int i;
	for (i = 0; i < osync_group_num_members(group->group); i++) {
		OSyncMember* member = osync_group_nth_member(group->group, i);
		
		GtkWidget* hbox6 = gtk_hbox_new (FALSE, 10);
		gtk_widget_show (hbox6);
		gtk_box_pack_start (GTK_BOX (group->vbox), hbox6, FALSE, FALSE, 0);

		GtkWidget* label8 = gtk_label_new ("");
		gtk_widget_show (label8);
		gtk_box_pack_start (GTK_BOX (hbox6), label8, FALSE, FALSE, 0);
		gtk_widget_set_size_request (label8, 30, -1);

		GtkWidget* image5 = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_MENU);
		gtk_widget_show (image5);
		gtk_box_pack_start (GTK_BOX (hbox6), image5, FALSE, FALSE, 0);
		
		GtkWidget* label9 = gtk_label_new (osync_member_get_pluginname(member));
		gtk_widget_show (label9);
		gtk_box_pack_start (GTK_BOX (hbox6), label9, FALSE, FALSE, 0);
		gtk_widget_set_size_request (label9, 175, -1);
		gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);
		
		GtkWidget* label15 = gtk_label_new ("Ready");
		group->memberstatuslabel = g_list_append(group->memberstatuslabel, label15);
		gtk_widget_show (label15);
		gtk_box_pack_start (GTK_BOX (hbox6), label15, TRUE, TRUE, 0);
		gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);
	}
}

GtkWidget *msync_group_create_widget(MSyncGroup *group)
{
  g_assert(group->group);
  
  GtkWidget *vbox4;
  GtkWidget *hbox5;
  GtkWidget *image4;
  GtkWidget *label7;
  GtkWidget *hbox;
  GtkWidget *hbox8;
  GtkWidget *hbuttonbox2;
  GtkWidget *button3;
  GtkWidget *button4;
  GtkWidget *button6;
  
  vbox4 = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox4);

  hbox5 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox5, TRUE, TRUE, 0);

  image4 = gtk_image_new_from_stock ("gtk-dialog-info", GTK_ICON_SIZE_DND);
  gtk_widget_show (image4);
  gtk_box_pack_start (GTK_BOX (hbox5), image4, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (image4), 0, 0.5);

/*****/  
  label7 = gtk_label_new ("<span weight=\"bold\" size=\"larger\">Group: Mockup</span>");
  group->grouplabel = label7;
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (hbox5), label7, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label7), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  group->vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (group->vbox);
  gtk_box_pack_start (GTK_BOX (vbox4), group->vbox, TRUE, TRUE, 0);

/****/
	msync_group_update_widget(group);
/****/

  hbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox, TRUE, TRUE, 0);

  group->entrylabel = gtk_label_new("");
  gtk_widget_show(group->entrylabel);
  gtk_box_pack_start (GTK_BOX (hbox), group->entrylabel, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (group->entrylabel), 0, 0.5);

  group->enginelabel = gtk_label_new("");
  gtk_widget_show(group->enginelabel);
  gtk_box_pack_start (GTK_BOX (hbox), group->enginelabel, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (group->enginelabel), 0, 0.5);
  
  hbox8 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox8);
  gtk_box_pack_end (GTK_BOX (vbox4), hbox8, TRUE, TRUE, 0);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (hbox8), hbuttonbox2, TRUE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_START);
  gtk_box_set_spacing (GTK_BOX (hbuttonbox2), 5);

  button3 = gtk_button_new_from_stock ("gtk-remove");
  gtk_widget_show (button3);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button3);
  GTK_WIDGET_UNSET_FLAGS (button3, GTK_CAN_FOCUS);

  button4 = gtk_button_new_from_stock ("gtk-edit");
  gtk_widget_show (button4);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button4);
  GTK_WIDGET_UNSET_FLAGS (button4, GTK_CAN_FOCUS);

  button6 = gtk_button_new_from_stock ("gtk-refresh");
  gtk_widget_show (button6);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button6);
  GTK_WIDGET_UNSET_FLAGS (button6, GTK_CAN_FOCUS);

g_signal_connect_swapped(G_OBJECT(button3), "clicked", G_CALLBACK(msync_group_remove), group);
g_signal_connect(G_OBJECT(button4), "clicked", G_CALLBACK(on_buttonedit_clicked), group);
g_signal_connect_swapped(G_OBJECT(button6), "clicked", G_CALLBACK(msync_group_syncronize), group);
  
  group->buttondelete = button3;
  group->buttonedit = button4;
  group->buttonsyncronize = button6;
  
  return GTK_WIDGET(vbox4);
}

void msync_group_create_syncronizegroupconflictdialog(MSyncGroup *group)
{
  GtkWidget *syncronizegroupconflictdialog;
  GtkWidget *dialog_vbox5;
  GtkWidget *vbox7;
  GtkWidget *hbox9;
  GtkWidget *image5;
  GtkWidget *label12;
  GtkWidget *syncronizegroupcheckbuttonremember;
  GtkWidget *syncronizegroupconflictcontainer;
  GtkWidget *hbox8;
  GtkWidget *label11;
  GtkWidget *button9;
  GtkWidget *scrolledwindow4;
  GtkWidget *viewport2;
  GtkWidget *textview1;
  GtkWidget *vbox8;
  GtkWidget *hbox10;
  GtkWidget *label15;
  GtkWidget *button10;
  GtkWidget *scrolledwindow5;
  GtkWidget *viewport3;
  GtkWidget *textview2;
  GtkWidget *syncronizegroupconflictbuttons;
  GtkWidget *button8;
  GtkWidget *button6;
  GtkWidget *button3;

  syncronizegroupconflictdialog = gtk_dialog_new ();
  gtk_container_set_border_width (GTK_CONTAINER (syncronizegroupconflictdialog), 5);
  gtk_window_set_type_hint (GTK_WINDOW (syncronizegroupconflictdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox5 = GTK_DIALOG (syncronizegroupconflictdialog)->vbox;
  gtk_widget_show (dialog_vbox5);

  vbox7 = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox7);
  gtk_box_pack_start (GTK_BOX (dialog_vbox5), vbox7, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox7), 10);

  hbox9 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox9);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox9, FALSE, FALSE, 0);

  image5 = gtk_image_new_from_stock ("gtk-dialog-question", GTK_ICON_SIZE_DND);
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox9), image5, FALSE, FALSE, 0);

  label12 = gtk_label_new ("Which entry do you want to use?");
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox9), label12, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label12), TRUE);

  syncronizegroupcheckbuttonremember = gtk_check_button_new_with_mnemonic ("Remember my decision.");
  gtk_widget_show (syncronizegroupcheckbuttonremember);
  gtk_box_pack_start (GTK_BOX (vbox7), syncronizegroupcheckbuttonremember, FALSE, FALSE, 0);
  GTK_WIDGET_UNSET_FLAGS (syncronizegroupcheckbuttonremember, GTK_CAN_FOCUS);

  syncronizegroupconflictcontainer = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (syncronizegroupconflictcontainer);
  gtk_box_pack_start (GTK_BOX (vbox7), syncronizegroupconflictcontainer, TRUE, TRUE, 0);

  vbox7 = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox7);
  gtk_box_pack_start (GTK_BOX (syncronizegroupconflictcontainer), vbox7, TRUE, TRUE, 0);

  hbox8 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox8, FALSE, FALSE, 0);

  label11 = gtk_label_new ("Member1");
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox8), label11, FALSE, FALSE, 0);

  button9 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (button9);
  gtk_box_pack_start (GTK_BOX (hbox8), button9, FALSE, FALSE, 0);
  GTK_WIDGET_UNSET_FLAGS (button9, GTK_CAN_FOCUS);

  scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow4);
  gtk_box_pack_start (GTK_BOX (vbox7), scrolledwindow4, TRUE, TRUE, 0);
  GTK_WIDGET_UNSET_FLAGS (scrolledwindow4, GTK_CAN_FOCUS);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport2 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport2);
  gtk_container_add (GTK_CONTAINER (scrolledwindow4), viewport2);

  textview1 = gtk_text_view_new ();
  gtk_widget_show (textview1);
  gtk_container_add (GTK_CONTAINER (viewport2), textview1);
  GTK_WIDGET_UNSET_FLAGS (textview1, GTK_CAN_FOCUS);

  vbox8 = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox8);
  gtk_box_pack_start (GTK_BOX (syncronizegroupconflictcontainer), vbox8, TRUE, TRUE, 0);

  hbox10 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox10);
  gtk_box_pack_start (GTK_BOX (vbox8), hbox10, FALSE, FALSE, 0);

  label15 = gtk_label_new ("Member2");
  gtk_widget_show (label15);
  gtk_box_pack_start (GTK_BOX (hbox10), label15, FALSE, FALSE, 0);

  button10 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (button10);
  gtk_box_pack_start (GTK_BOX (hbox10), button10, FALSE, FALSE, 0);
  GTK_WIDGET_UNSET_FLAGS (button10, GTK_CAN_FOCUS);

  scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow5);
  gtk_box_pack_start (GTK_BOX (vbox8), scrolledwindow5, TRUE, TRUE, 0);
  GTK_WIDGET_UNSET_FLAGS (scrolledwindow5, GTK_CAN_FOCUS);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport3 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport3);
  gtk_container_add (GTK_CONTAINER (scrolledwindow5), viewport3);

  textview2 = gtk_text_view_new ();
  gtk_widget_show (textview2);
  gtk_container_add (GTK_CONTAINER (viewport3), textview2);
  GTK_WIDGET_UNSET_FLAGS (textview2, GTK_CAN_FOCUS);

  syncronizegroupconflictbuttons = GTK_DIALOG (syncronizegroupconflictdialog)->action_area;
  gtk_widget_show (syncronizegroupconflictbuttons);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (syncronizegroupconflictbuttons), GTK_BUTTONBOX_END);

  button8 = gtk_button_new_with_mnemonic ("Ignore");
  gtk_widget_show (button8);
  gtk_dialog_add_action_widget (GTK_DIALOG (syncronizegroupconflictdialog), button8, 0);
  GTK_WIDGET_UNSET_FLAGS (button8, GTK_CAN_FOCUS);

  button6 = gtk_button_new_with_mnemonic ("Duplicate");
  gtk_widget_show (button6);
  gtk_dialog_add_action_widget (GTK_DIALOG (syncronizegroupconflictdialog), button6, 0);
  GTK_WIDGET_UNSET_FLAGS (button6, GTK_CAN_FOCUS);

  button3 = gtk_button_new_with_mnemonic ("Newer");
  gtk_widget_show (button3);
  gtk_dialog_add_action_widget (GTK_DIALOG (syncronizegroupconflictdialog), button3, 0);
  GTK_WIDGET_UNSET_FLAGS (button3, GTK_CAN_FOCUS);

  group->conflictdialog = syncronizegroupconflictdialog;
  group->conflictcheckbuttonremember = syncronizegroupcheckbuttonremember; 
  group->conflictbuttons = syncronizegroupconflictbuttons;
  group->conflictcontainer = syncronizegroupconflictcontainer;
  //since gtk 2.10
  //gtk_window_set_deletable(GTK_WINDOW(syncronizegroupconflictdialog), FALSE);
  g_signal_connect(G_OBJECT(group->conflictdialog), "delete_event", G_CALLBACK (gtk_true), NULL);
  g_signal_connect(G_OBJECT(group->conflictdialog), "response", G_CALLBACK(gtk_true), NULL);
}
