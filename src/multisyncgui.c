#include "multisync.h"

int main (int argc, char *argv[])
{
	MSyncEnv *env;
	env = g_malloc0(sizeof(MSyncEnv));
	
	g_thread_init(NULL);
	gtk_init(&argc, &argv);
	gdk_threads_init();
	gdk_threads_enter();

	if(!msync_env_init(env))
		return -1;
	
	gtk_main ();
	gdk_threads_leave();

	return 0;
}
