/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <bonobo.h>
#include <gnome.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

static GnomeUIInfo e2convertwidget1_uiinfo[] =
{
  GNOMEUIINFO_END
};

static GnomeUIInfo e2menu1_uiinfo[] =
{
  GNOMEUIINFO_END
};

static GnomeUIInfo e2convertwidget2_uiinfo[] =
{
  GNOMEUIINFO_END
};

GtkWidget*
create_synce_wnd_options (void)
{
  GtkWidget *synce_wnd_options;
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *label15;
  GtkWidget *hbox1;
  GtkWidget *label12;
  GtkWidget *table1;
  GtkWidget *label13;
  GtkWidget *button1;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label14;
  GtkWidget *txt_path;
  GtkWidget *label16;
  GtkWidget *hbox3;
  GtkWidget *label17;
  GtkWidget *table2;
  GtkWidget *lbl_passwd;
  GtkWidget *chk_mode;
  GtkWidget *txt_mode;
  GtkWidget *e2label4;
  GtkWidget *e2hbox1;
  GtkWidget *e2label5;
  GtkWidget *e2calendarmenu;
  AtkObject *atko;
  GtkWidget *e2convertwidget1;
  GtkWidget *e2label6;
  GtkWidget *e2hbox2;
  GtkWidget *e2label7;
  GtkWidget *e2addressbookmenu;
  GtkWidget *e2menu1;
  GtkWidget *e2label8;
  GtkWidget *e2hbox3;
  GtkWidget *e2label9;
  GtkWidget *e2todomenu;
  GtkWidget *e2convertwidget2;
  GtkWidget *hseparator10;
  GtkWidget *hbuttonbox2;
  GtkWidget *btn_cancel;
  GtkWidget *btn_ok;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  synce_wnd_options = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (synce_wnd_options, "synce_wnd_options");
  gtk_window_set_title (GTK_WINDOW (synce_wnd_options), "Preferences");
  gtk_window_set_destroy_with_parent (GTK_WINDOW (synce_wnd_options), TRUE);

  vbox1 = gtk_vbox_new (FALSE, 6);
  gtk_widget_set_name (vbox1, "vbox1");
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (synce_wnd_options), vbox1);
  gtk_container_set_border_width (GTK_CONTAINER (vbox1), 12);

  vbox2 = gtk_vbox_new (FALSE, 6);
  gtk_widget_set_name (vbox2, "vbox2");
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 6);

  label15 = gtk_label_new ("<span weight='bold'>Options</span>");
  gtk_widget_set_name (label15, "label15");
  gtk_widget_show (label15);
  gtk_box_pack_start (GTK_BOX (vbox2), label15, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label15), TRUE);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_CENTER);
  gtk_label_set_line_wrap (GTK_LABEL (label15), TRUE);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox1, "hbox1");
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);

  label12 = gtk_label_new ("    ");
  gtk_widget_set_name (label12, "label12");
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox1), label12, FALSE, FALSE, 0);

  table1 = gtk_table_new (1, 4, FALSE);
  gtk_widget_set_name (table1, "table1");
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (hbox1), table1, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 12);

  label13 = gtk_label_new_with_mnemonic ("Directory");
  gtk_widget_set_name (label13, "label13");
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table1), label13, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label13), 0, 0.5);

  button1 = gtk_button_new ();
  gtk_widget_set_name (button1, "button1");
  gtk_widget_show (button1);
  gtk_table_attach (GTK_TABLE (table1), button1, 3, 4, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_set_name (alignment1, "alignment1");
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button1), alignment1);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_set_name (hbox2, "hbox2");
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox2);

  image1 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_set_name (image1, "image1");
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

  label14 = gtk_label_new_with_mnemonic ("Browse");
  gtk_widget_set_name (label14, "label14");
  gtk_widget_show (label14);
  gtk_box_pack_start (GTK_BOX (hbox2), label14, FALSE, FALSE, 0);

  txt_path = gtk_entry_new ();
  gtk_widget_set_name (txt_path, "txt_path");
  gtk_widget_show (txt_path);
  gtk_table_attach (GTK_TABLE (table1), txt_path, 1, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (txt_path, 40, 23);

  label16 = gtk_label_new ("<span weight='bold'>Default Mode:</span>");
  gtk_widget_set_name (label16, "label16");
  gtk_widget_show (label16);
  gtk_box_pack_start (GTK_BOX (vbox2), label16, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label16), TRUE);
  gtk_label_set_justify (GTK_LABEL (label16), GTK_JUSTIFY_CENTER);
  gtk_label_set_line_wrap (GTK_LABEL (label16), TRUE);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox3, "hbox3");
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox3, TRUE, TRUE, 0);

  label17 = gtk_label_new ("    ");
  gtk_widget_set_name (label17, "label17");
  gtk_widget_show (label17);
  gtk_box_pack_start (GTK_BOX (hbox3), label17, FALSE, FALSE, 0);

  table2 = gtk_table_new (1, 3, FALSE);
  gtk_widget_set_name (table2, "table2");
  gtk_widget_show (table2);
  gtk_box_pack_start (GTK_BOX (hbox3), table2, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table2), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table2), 12);

  lbl_passwd = gtk_label_new ("Default Mode");
  gtk_widget_set_name (lbl_passwd, "lbl_passwd");
  gtk_widget_show (lbl_passwd);
  gtk_table_attach (GTK_TABLE (table2), lbl_passwd, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_line_wrap (GTK_LABEL (lbl_passwd), TRUE);
  gtk_misc_set_alignment (GTK_MISC (lbl_passwd), 0, 0.5);

  chk_mode = gtk_check_button_new_with_mnemonic ("Overwrite file mode");
  gtk_widget_set_name (chk_mode, "chk_mode");
  gtk_widget_show (chk_mode);
  gtk_table_attach (GTK_TABLE (table2), chk_mode, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  txt_mode = gtk_entry_new ();
  gtk_widget_set_name (txt_mode, "txt_mode");
  gtk_widget_show (txt_mode);
  gtk_table_attach (GTK_TABLE (table2), txt_mode, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (txt_mode, 38, -1);
  gtk_entry_set_text (GTK_ENTRY (txt_mode), "664");

  e2label4 = gtk_label_new ("<span weight='bold'>Calendar:</span>");
  gtk_widget_set_name (e2label4, "e2label4");
  gtk_widget_show (e2label4);
  gtk_box_pack_start (GTK_BOX (vbox1), e2label4, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (e2label4), TRUE);
  gtk_misc_set_alignment (GTK_MISC (e2label4), 0, 0.5);

  e2hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (e2hbox1, "e2hbox1");
  gtk_widget_show (e2hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), e2hbox1, FALSE, TRUE, 0);

  e2label5 = gtk_label_new ("    ");
  gtk_widget_set_name (e2label5, "e2label5");
  gtk_widget_show (e2label5);
  gtk_box_pack_start (GTK_BOX (e2hbox1), e2label5, FALSE, FALSE, 0);

  e2calendarmenu = gtk_option_menu_new ();
  gtk_widget_set_name (e2calendarmenu, "e2calendarmenu");
  gtk_widget_show (e2calendarmenu);
  gtk_box_pack_start (GTK_BOX (e2hbox1), e2calendarmenu, TRUE, TRUE, 0);

  e2convertwidget1 = gtk_menu_new ();
  gtk_widget_set_name (e2convertwidget1, "e2convertwidget1");
  gnome_app_fill_menu (GTK_MENU_SHELL (e2convertwidget1), e2convertwidget1_uiinfo,
                       accel_group, FALSE, 0);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (e2calendarmenu), e2convertwidget1);

  e2label6 = gtk_label_new ("<span weight='bold'>Addressbook:</span>");
  gtk_widget_set_name (e2label6, "e2label6");
  gtk_widget_show (e2label6);
  gtk_box_pack_start (GTK_BOX (vbox1), e2label6, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (e2label6), TRUE);
  gtk_misc_set_alignment (GTK_MISC (e2label6), 0, 0.5);

  e2hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (e2hbox2, "e2hbox2");
  gtk_widget_show (e2hbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), e2hbox2, FALSE, TRUE, 0);

  e2label7 = gtk_label_new ("    ");
  gtk_widget_set_name (e2label7, "e2label7");
  gtk_widget_show (e2label7);
  gtk_box_pack_start (GTK_BOX (e2hbox2), e2label7, FALSE, FALSE, 0);

  e2addressbookmenu = gtk_option_menu_new ();
  gtk_widget_set_name (e2addressbookmenu, "e2addressbookmenu");
  gtk_widget_show (e2addressbookmenu);
  gtk_box_pack_start (GTK_BOX (e2hbox2), e2addressbookmenu, TRUE, TRUE, 0);

  e2menu1 = gtk_menu_new ();
  gtk_widget_set_name (e2menu1, "e2menu1");
  gnome_app_fill_menu (GTK_MENU_SHELL (e2menu1), e2menu1_uiinfo,
                       accel_group, FALSE, 0);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (e2addressbookmenu), e2menu1);

  e2label8 = gtk_label_new ("<span weight='bold'>Tasks:</span>");
  gtk_widget_set_name (e2label8, "e2label8");
  gtk_widget_show (e2label8);
  gtk_box_pack_start (GTK_BOX (vbox1), e2label8, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (e2label8), TRUE);
  gtk_misc_set_alignment (GTK_MISC (e2label8), 0, 0.5);

  e2hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (e2hbox3, "e2hbox3");
  gtk_widget_show (e2hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), e2hbox3, FALSE, TRUE, 0);

  e2label9 = gtk_label_new ("    ");
  gtk_widget_set_name (e2label9, "e2label9");
  gtk_widget_show (e2label9);
  gtk_box_pack_start (GTK_BOX (e2hbox3), e2label9, FALSE, FALSE, 0);

  e2todomenu = gtk_option_menu_new ();
  gtk_widget_set_name (e2todomenu, "e2todomenu");
  gtk_widget_show (e2todomenu);
  gtk_box_pack_start (GTK_BOX (e2hbox3), e2todomenu, TRUE, TRUE, 0);

  e2convertwidget2 = gtk_menu_new ();
  gtk_widget_set_name (e2convertwidget2, "e2convertwidget2");
  gnome_app_fill_menu (GTK_MENU_SHELL (e2convertwidget2), e2convertwidget2_uiinfo,
                       accel_group, FALSE, 0);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (e2todomenu), e2convertwidget2);

  hseparator10 = gtk_hseparator_new ();
  gtk_widget_set_name (hseparator10, "hseparator10");
  gtk_widget_show (hseparator10);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator10, FALSE, TRUE, 0);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_set_name (hbuttonbox2, "hbuttonbox2");
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox2, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (hbuttonbox2), 30);

  btn_cancel = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_set_name (btn_cancel, "btn_cancel");
  gtk_widget_show (btn_cancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), btn_cancel);
  gtk_widget_set_size_request (btn_cancel, 81, 32);

  btn_ok = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_set_name (btn_ok, "btn_ok");
  gtk_widget_show (btn_ok);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), btn_ok);
  gtk_widget_set_size_request (btn_ok, 81, 32);

  g_signal_connect ((gpointer) synce_wnd_options, "destroy",
                    G_CALLBACK (synce_on_wnd_options_destroy),
                    NULL);
  g_signal_connect ((gpointer) btn_cancel, "clicked",
                    G_CALLBACK (synce_on_btn_cancel_clicked),
                    NULL);
  g_signal_connect ((gpointer) btn_ok, "clicked",
                    G_CALLBACK (synce_on_btn_ok_clicked),
                    NULL);

  gtk_label_set_mnemonic_widget (GTK_LABEL (label13), txt_path);

  atko = gtk_widget_get_accessible (e2calendarmenu);
  atk_object_set_description (atko, "Select the calendar you wish to sync.");

  atko = gtk_widget_get_accessible (e2addressbookmenu);
  atk_object_set_description (atko, "Select the addressbook you wish to sync.");

  atko = gtk_widget_get_accessible (e2todomenu);
  atk_object_set_description (atko, "Select the tasks you wish to sync.");


  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (synce_wnd_options, synce_wnd_options, "synce_wnd_options");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, label15, "label15");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, label12, "label12");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, table1, "table1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, label13, "label13");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, button1, "button1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, image1, "image1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, label14, "label14");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, txt_path, "txt_path");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, label16, "label16");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, label17, "label17");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, table2, "table2");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, lbl_passwd, "lbl_passwd");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, chk_mode, "chk_mode");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, txt_mode, "txt_mode");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2label4, "e2label4");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2hbox1, "e2hbox1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2label5, "e2label5");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2calendarmenu, "e2calendarmenu");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2convertwidget1, "e2convertwidget1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2label6, "e2label6");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2hbox2, "e2hbox2");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2label7, "e2label7");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2addressbookmenu, "e2addressbookmenu");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2menu1, "e2menu1");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2label8, "e2label8");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2hbox3, "e2hbox3");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2label9, "e2label9");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2todomenu, "e2todomenu");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, e2convertwidget2, "e2convertwidget2");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, hseparator10, "hseparator10");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, hbuttonbox2, "hbuttonbox2");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, btn_cancel, "btn_cancel");
  GLADE_HOOKUP_OBJECT (synce_wnd_options, btn_ok, "btn_ok");

  gtk_window_add_accel_group (GTK_WINDOW (synce_wnd_options), accel_group);

  return synce_wnd_options;
}

