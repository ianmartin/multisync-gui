#include "multisync.h"

MSyncPair *msync_pair_new(void)
{
	MSyncPair *pair = g_malloc0(sizeof(MSyncPair));
	g_assert(pair);
	return pair;
}

void msync_pair_free(MSyncPair *pair)
{
	g_assert(pair);
	g_free(pair);
}

void msync_pairs_load(MSyncEnv *env)
{
	int i;
	for (i = 0; i < osync_num_groups(env->osync); i++) {
		MSyncPair *pair = msync_pair_new();
		pair->group = osync_get_nth_group(env->osync, i);
		env->syncpairs = g_list_append(env->syncpairs, pair);
	}
}
