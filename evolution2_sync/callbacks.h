#include <gnome.h>


gboolean
evo2_optwin_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
evo2_cancelbutton_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
evo2_okbutton_clicked                  (GtkButton       *button,
                                        gpointer         user_data);
