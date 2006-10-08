#include "multisync.h"

GtkWidget *msync_group_create_widget(MSyncGroup *group);

void msync_group_syncronize_update_member_status(OSyncMemberUpdate *status, void *user_data)
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

void mapping_status(OSyncMappingUpdate *status, void *user_data)
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

void msync_group_syncronize_update_engine_status(OSyncEngine *engine, OSyncEngineUpdate *status, void *user_data)
{
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup *)user_data;

	switch (status->type) {
		case ENG_PREV_UNCLEAN:
			printf("The previous synchronization was unclean. Slow-syncing\n");
			msync_group_update_engine_status(group, FALSE,"The previous synchronization was unclean. Slow-syncing");
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
			break;
		case ENG_ERROR:
			printf("The sync failed: %s\n", osync_error_print(&(status->error)));
			msync_group_update_engine_status(group, TRUE, "The sync failed");
			break;
	}
}

static const char *OSyncChangeType2String(OSyncChangeType c)
{
	switch (c) {
		case CHANGE_ADDED: return "ADDED";
		case CHANGE_UNMODIFIED: return "UNMODIFIED";
		case CHANGE_DELETED: return "DELETED";
		case CHANGE_MODIFIED: return "MODIFIED";
		default:
		case CHANGE_UNKNOWN: return "?";
	}
}

void entry_status(OSyncEngine *engine, OSyncChangeUpdate *status, void *user_data)
{
	switch (status->type) {
		case CHANGE_RECEIVED_INFO:
			printf("Received an entry %s without data from member %i. Changetype %s\n",
					osync_change_get_uid(status->change), status->member_id,
					OSyncChangeType2String(osync_change_get_changetype(status->change)));
			break;
		case CHANGE_RECEIVED:
			printf("Received an entry %s with data of size %i from member %i. Changetype %s\n",
					osync_change_get_uid(status->change),
					osync_change_get_datasize(status->change),
					status->member_id,
					OSyncChangeType2String(osync_change_get_changetype(status->change)));
			break;
		case CHANGE_SENT:
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

msync_group_syncronize_mapping_duplicate(MSyncGroup* group)
{
	printf("Mapping duplicated\n");
	osengine_mapping_duplicate(group->engine, group->curmapping);
	gtk_widget_hide(group->msync->syncronizegroupconflictdialog);
}

msync_group_syncronize_mapping_ignore(MSyncGroup* group)
{
	printf("Conflict ignored\n");
	OSyncError* error = NULL;
	if (!osengine_mapping_ignore_conflict(group->engine, group->curmapping, &error)) {
		msync_error_message(GTK_WINDOW(group->msync->syncronizegroupconflictdialog), "Conflict not ignored: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	gtk_widget_hide(group->msync->syncronizegroupconflictdialog);
}

msync_group_syncronize_mapping_newer(MSyncGroup* group)
{
	printf("Newest entry used\n");
	OSyncError* error = NULL;
	if (!osengine_mapping_solve_latest(group->engine, group->curmapping, &error)) {
		msync_error_message(GTK_WINDOW(group->msync->syncronizegroupconflictdialog), "Conflict not resolved: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	gtk_widget_hide(group->msync->syncronizegroupconflictdialog);
}

msync_group_syncronize_mapping_select(GtkButton* button, MSyncGroup* group)
{
	printf("Overwriting conflict\n");
	OSyncChange* change =g_object_get_data(G_OBJECT(button), "msync_change");
	osengine_mapping_solve(group->engine, group->curmapping, change);
	gtk_widget_hide(group->msync->syncronizegroupconflictdialog);
}

void msync_group_syncronize_show_conflict_dialog(OSyncEngine *engine, OSyncMapping *mapping, void *user_data)
{
	OSyncError	*error = NULL;
	OSyncChange *change = NULL;
	
	g_assert(user_data);
	MSyncGroup* group = (MSyncGroup *)user_data;
	group->curmapping = mapping;
	
	gdk_threads_enter();
	
	gtk_widget_show(group->msync->syncronizegroupconflictdialog);

	GList* childs = gtk_container_get_children(GTK_CONTAINER(group->msync->syncronizegroupconflictcontainer));
	if(!childs)
		printf("no childs1\n");
	g_list_foreach(childs, (GFunc)gtk_widget_destroy, NULL);
	g_list_free(childs);

	childs = gtk_container_get_children(GTK_CONTAINER(group->msync->syncronizegroupconflictbuttons));
	if(!childs)
		printf("no childs\n");
	g_list_foreach(childs, (GFunc)gtk_widget_destroy, NULL);
	g_list_free(childs);

	if (osengine_mapping_ignore_supported(engine, mapping)) {
		GtkWidget* button3 = gtk_button_new_with_label ("Ignore");
		gtk_widget_show (button3);
		gtk_box_pack_start (GTK_BOX (group->msync->syncronizegroupconflictbuttons), button3, TRUE, TRUE, 0);
		GTK_WIDGET_UNSET_FLAGS (button3, GTK_CAN_FOCUS);
		g_signal_connect_swapped(G_OBJECT(button3), "clicked", G_CALLBACK(msync_group_syncronize_mapping_ignore), group);
	}
	GtkWidget* button4 = gtk_button_new_with_label ("Duplicate");
	gtk_widget_show (button4);
	gtk_dialog_add_action_widget (GTK_DIALOG (group->msync->syncronizegroupconflictdialog), button4, 0);
	GTK_WIDGET_UNSET_FLAGS (button4, GTK_CAN_FOCUS);
	g_signal_connect_swapped(G_OBJECT(button4), "clicked", G_CALLBACK(msync_group_syncronize_mapping_duplicate), group);

	GtkWidget* button6 = gtk_button_new_with_label ("Newer");
	gtk_widget_show (button6);
	gtk_box_pack_start (GTK_BOX(group->msync->syncronizegroupconflictbuttons), button6, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS (button6, GTK_CAN_FOCUS);
	g_signal_connect_swapped(G_OBJECT(button6), "clicked", G_CALLBACK(msync_group_syncronize_mapping_newer), group);

	int i = 0;
	for (i = 0; i < osengine_mapping_num_changes(mapping); i++) {
		OSyncChange *change = osengine_mapping_nth_change(mapping, i);
		if (osync_change_get_changetype(change) != CHANGE_UNKNOWN)
		{
  GtkWidget* vbox7 = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox7);
  gtk_box_pack_start (GTK_BOX (group->msync->syncronizegroupconflictcontainer), vbox7, TRUE, TRUE, 0);

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
g_object_set_data(G_OBJECT(button9), "msync_change", change);

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
  //gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview1), GTK_WRAP_WORD);
char* tmp = osync_change_get_printable(change);
//gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1)), "muh", -1);
gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1)), (gchar *)tmp, -1);
free(tmp);
		}
	}
	
	gdk_flush();	
	gdk_threads_leave ();
}

void msync_group_new(MSyncEnv *env, OSyncGroup *osyncgroup)
{
	MSyncGroup *group = g_malloc0(sizeof(MSyncGroup));
	env->groups = g_list_append(env->groups, group);
	group->msync = env;
	group->group = osyncgroup;
	group->widget = msync_group_create_widget(group);
	gtk_box_pack_start (GTK_BOX (env->groupcontainer), group->widget, FALSE, FALSE, 0);
	
	OSyncError *error = NULL;
	
	//printf("Synchronizing group %s\n", osync_group_get_name(group->group));
	group->engine = osengine_new(group->group, &error);
	if (!group->engine) {
		msync_error_message(GTK_WINDOW(env->mainwindow), "Error while creating syncengine: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
	osengine_set_memberstatus_callback(group->engine, msync_group_syncronize_update_member_status, group);
	osengine_set_enginestatus_callback(group->engine, msync_group_syncronize_update_engine_status, group);
	osengine_set_conflict_callback(group->engine, msync_group_syncronize_show_conflict_dialog, group);
	osengine_set_changestatus_callback(group->engine, entry_status, NULL);
	osengine_set_mappingstatus_callback(group->engine, mapping_status, NULL);

	if (!osengine_init(group->engine, &error)) {
		msync_error_message(GTK_WINDOW(env->mainwindow), "Error while initializing syncengine: %s\n", osync_error_print(&error));
		osync_error_free(&error);
		return;
	}
	
}

void msync_group_free(MSyncGroup *group)
{
	osengine_finalize(group->engine);
	osengine_free(group->engine);
	
	g_list_free(group->memberstatuslabel);
	gtk_widget_destroy(group->widget);
	group->msync->groups = g_list_remove(group->msync->groups, group);
	g_free(group);
}

void msync_group_update_engine_status(MSyncGroup *group, gboolean gtkthreadsafe, const char* msg)
{
	if(gtkthreadsafe) {
		gdk_threads_enter();
	}
		gtk_label_set_text(GTK_LABEL(group->enginelabel), (const gchar *)msg);
	if(gtkthreadsafe) {
		gdk_flush();	
		gdk_threads_leave ();
	}
}

void msync_group_update_member_status(MSyncGroup *group, OSyncMember *member, const char* msg)
{
	//g_mutex_lock(group->mutex);
	gdk_threads_enter ();
	
	gchar* str = g_strdup_printf("%lli", osync_member_get_id(member));
	int i = atoi((char *)str);
	g_free(str);
	GList* tmp = g_list_nth(group->memberstatuslabel, i-1);
	GtkLabel* label = GTK_LABEL(tmp->data);	
	gtk_label_set_text(label, msg);
	
	//while( gtk_events_pending() )
      // gtk_main_iteration();
	//GdkDisplay *display = gdk_display_get_default ();
  	//XFlush (GDK_DISPLAY_XDISPLAY (display));
  	
	gdk_flush();	
	gdk_threads_leave ();
	//g_mutex_unlock(group->mutex);
}

GtkWidget *msync_group_update_widget(MSyncGroup *group)
{
	GList* list = gtk_container_get_children(GTK_CONTAINER(group->vbox));
	g_list_foreach(list, (GFunc)gtk_widget_destroy, NULL);
	g_list_free(list);
	
	if(group->memberstatuslabel) {
		g_list_free(group->memberstatuslabel);
		group->memberstatuslabel = NULL;
	}
  	gchar* tmp = g_strdup_printf("<span weight=\"bold\" size=\"larger\">Group: %s</span>", osync_group_get_name(group->group));
  	gtk_label_set_markup(GTK_LABEL(group->label), tmp);
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
		gtk_widget_set_size_request (label9, 200, -1);
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
  GtkWidget *vbox5;
  GtkWidget *hbox6;
  GtkWidget *label8;
  GtkWidget *image5;
  GtkWidget *label9;
  GtkWidget *label15;
  GtkWidget *hbox7;
  GtkWidget *label10;
  GtkWidget *image6;
  GtkWidget *label11;
  GtkWidget *label16;
  GtkWidget *hbox8;
  GtkWidget *hbuttonbox2;
  GtkWidget *button3;
  GtkWidget *button4;
  GtkWidget *button6;
  gchar* tmp;
  
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
  group->label = label7;
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (hbox5), label7, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label7), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  vbox5 = gtk_vbox_new (FALSE, 0);
  group->vbox = vbox5;
  gtk_widget_show (vbox5);
  gtk_box_pack_start (GTK_BOX (vbox4), vbox5, TRUE, TRUE, 0);

/****/
	msync_group_update_widget(group);
/****/

  group->enginelabel = (GtkWidget *)gtk_label_new("");
  gtk_widget_show (group->enginelabel);
  gtk_box_pack_start (GTK_BOX (vbox4), group->enginelabel, TRUE, TRUE, 0);
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

g_signal_connect(G_OBJECT(button3), "clicked", G_CALLBACK(on_buttondelete_clicked), group);
g_signal_connect(G_OBJECT(button4), "clicked", G_CALLBACK(on_buttonedit_clicked), group);
g_signal_connect(G_OBJECT(button6), "clicked", G_CALLBACK(on_buttonsyncronize_clicked), group);
  
  return GTK_WIDGET(vbox4);
}
