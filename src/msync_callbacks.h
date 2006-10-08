#ifndef MSYNC_CALLBACKS_H_
#define MSYNC_CALLBACKS_H_

void on_exit(gpointer user_data);

void on_newgroupbuttonapply_clicked(GtkButton *button, gpointer user_data);
void on_editgroupclosebutton_clicked(GtkButton* button, gpointer user_data);
void on_editgroupaddmemberbutton_clicked(GtkButton* button, gpointer user_data);
void on_editgroupaddmemberapplybutton_clicked(GtkButton* button, gpointer user_data);
void on_editgrouptreeview_change(GtkTreeSelection *selection, gpointer user_data);

void on_buttondelete_clicked(GtkButton *button, gpointer user_data);
void on_buttonedit_clicked(GtkButton *button, gpointer user_data);
void on_buttonsyncronize_clicked(GtkButton *button, gpointer user_data);

void on_toolbutton_newgroup_clicked(GtkMenuItem *menuitem, gpointer user_data);
void on_toolbutton_about_clicked(GtkMenuItem *menuitem, gpointer user_data);

#endif /*MSYNC_CALLBACKS_H_*/
