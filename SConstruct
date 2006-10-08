
env = Environment()
env.ParseConfig('pkg-config --cflags --libs libglade-2.0')
env.ParseConfig('pkg-config --cflags --libs opensync-1.0')
env.ParseConfig('pkg-config --cflags --libs osengine-1.0')
env.Append(LINKFLAGS = r' -Wl,--rpath -Wl,/usr/local/lib')
env.Program(['src/multisync.c', 'src/msync_env.c', 'src/msync_group.c', 'src/msync_plugin.c', 'src/msync_support.c', 'src/msync_callbacks.c', 'src/plugins/msync_default.c'])
