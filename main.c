#include "multisync.h"

MSyncEnv *env;

int main (int argc, char *argv[])
{

  gnome_program_init (PACKAGE, VERSION, LIBGNOMEUI_MODULE, argc, argv, GNOME_PARAM_APP_DATADIR, PACKAGE_DATA_DIR, NULL);

	printf("Starting up!\n");
	env = g_malloc0(sizeof(MSyncEnv));
	env->osync = osync_env_new();
	
	msync_register_plugins(env);
	osync_env_initialize(env->osync);
	osync_env_load_groups_dir(env->osync);
	msync_pairs_load(env);
	env->mainwindow = create_mainwindow();
	env->optionwindow = create_syncpairwin();
	msync_show_main(env);
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

