#include "multisync.h"

gboolean
on_syncpairwin_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_localedit_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	msync_open_localplugin_window();
}


void
on_remoteedit_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	msync_open_remoteplugin_window();
}


void
on_calendarcheck_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_addressbookcheck_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_todocheck_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_item1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_filterdataentry_changed             (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_dirlocalremoteradio_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_dirremotelocalradio_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_syncpaircancel_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	msync_cancel_syncpairwindow();
}


void
on_syncpairok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	msync_ok_syncpairwindow();
}


gboolean
on_logwindow_delete_event              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_logclearbutton_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_logokbutton_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


gboolean
on_mainwindow_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  msync_main_quit();
  return TRUE;
}


void
on_mainwindow_destroy                  (GtkObject       *object,
                                        gpointer         user_data)
{

}


void
on_newpairmenu_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_hidewinmenu_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  msync_main_quit();
}


void
on_editpairitem_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_deletepairitem_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_item3_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_showtrayitem_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_starthiddenitem_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_showresyncitem_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_helpmenu_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_aboutmenu_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_editbutton_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	msync_edit_pair();
}


void
on_deletebutton_clicked                (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	msync_delete_pair();
}


void
on_logbutton_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{

}


void
on_syncnowbutton_clicked               (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	msync_synchronize();
}


void
on_resyncnowbutton_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{

}


void
on_syncpairlist_row_activated          (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{

}


gboolean
on_duplicatewin_delete                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_duplicate_usefirstbutton_clicked    (GtkButton       *button,
                                        gpointer         user_data)
{
	msync_solve_conflict_choose(0);
}


void
on_duplicate_usesecondbutton_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{
	msync_solve_conflict_choose(1);
}


void
on_duplicate_usebothbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
	msync_solve_conflict_duplicate();
}

