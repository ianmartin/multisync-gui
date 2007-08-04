#! /usr/bin/env python
# encoding: utf-8
# Matthias Jahn <jahn.matthias@freenet.de>, 2006 (pmarat)

import os, sys
from Params import fatal

# the following two variables are used by the target "waf dist"
VERSION='0.92.0'
APPNAME='multisync-gui'

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = '_build_'

def to_unix_path(args):
	path_list=args.split('\\')
	return "/".join(path_list)
def p_j(*args):
	return(to_unix_path(os.path.join(*args)))

def opt_helper(opt, what, true_false, descript):
    if true_false:
	opt.add_option('--disable-%s' %(what), action='store_false', default=True,
	            help='%s' %(descript), dest='os_%s' %(what))
    else:
	opt.add_option('--enable-%s' %(what), action='store_true', default=False,
	            help='%s' %(descript), dest='os_%s' %(what))


def build(bld):
    # process subfolders from here
    print "waf: Entering directory `"+os.path.join(os.getcwd(),blddir)+"'"
    sys.stdout.flush()
    bld.add_subdirs('src misc')

def configure(conf):
	import Params, pproc
	# define some shortcuts for common used methodes
	# and variables
	d=conf.add_define
	e=conf.env
	P=Params.g_options
	e['PREFIX']=os.path.abspath(e['PREFIX']) #fix prefix remove '/' at end
	prefix=e['PREFIX']
	cwd=os.getcwd()

	if not conf.check_tool('compiler_cc'):
		fatal("Error: no C compiler was found in PATH.")
	conf.check_tool('misc')

	# pkg-config example - look in src/wscript_build, obj.uselib ..
	if not conf.check_pkg('glib-2.0', destvar='GLIB'):
		fatal("you need glib-2.0 (libglib2.0-dev)")
	if not conf.check_pkg('libxml-2.0', destvar='XML'):
		fatal("you need libxml-2.0 (libxml2-dev)")
	if not conf.check_pkg('gtk+-2.0 gthread-2.0', destvar='GTK',      vnum='2.6.0'):
		fatal("you need libgtk-2.0-dev")
	if not conf.check_pkg('libglade-2.0',        destvar='GLADE',    vnum='2.0.1'):
		fatal("you need libglade-2.0-dev")

	pkgconf = conf.create_pkgconfig_configurator()
	pkgconf.uselib = 'OPENSYNC'
	pkgconf.name = 'opensync-1.0'
	pkgconf.version = '0.31'
	pkgconf.variables = "configdir plugindir formatsdir"
	if not pkgconf.run():
		fatal("you need opensync http://www.opensync.org/ (libopensync0-dev)")

	if not P.os_develop_mode :
		e['MULTISYNC_DATA'] = p_j(e['PREFIX'], 'share/multisync-gui')
		e['MULTISYNC_ICON_DIR'] = p_j(e['PREFIX'], 'share/pixmaps/multisync-gui')
		d('MULTISYNC_GLADE', p_j(e['MULTISYNC_DATA'], 'multisync-gui.glade'))
		d('MULTISYNC_ICON', p_j(e['MULTISYNC_ICON_DIR'], 'multisync.png'))
	else:
		e['MULTISYNC_DATA'] = ""
		e['MULTISYNC_ICON_DIR'] = p_j(cwd, 'misc')
		d('MULTISYNC_GLADE', p_j(cwd, blddir, "default", "src", 'multisync-gui.glade'))
		d('MULTISYNC_ICON', p_j(e['MULTISYNC_ICON_DIR'], 'multisync.png'))
		pproc.call(["rm", "-rf", "multisync-gui"])
		pproc.call(["ln", "-s", "%s/_build_/default/src/multisync-gui" %cwd, "multisync-gui"])
	
	if P.os_rpath:
		e['RPATH_GLOBAL_PATH'] = ['-Wl,--rpath=%s' % p_j(prefix, 'lib')]

	#Set project global include path
	conf.env['CXXFLAGS_GLOBAL_PATH'] = '-I%s' % os.getcwd()
	if 'CCFLAGS' in os.environ :
		conf.env['CCFLAGS'] = os.environ['CCFLAGS']
	# write a config.h
	conf.write_config_header('config.h')


def set_options(opt):
	#just do some cleanups in the option list
	try:
		tmp_prefix = opt.parser.get_option("--prefix")
		opt.parser.remove_option("--prefix")
		tmp_destdir = opt.parser.get_option("--destdir")
		opt.parser.remove_option("--destdir")
		inst_dir = opt.add_option_group("Installation directories",
			'By default, waf install will install all the files in\
			"/usr/local/bin", "/usr/local/lib" etc.  You can specify \
			an installation prefix other than "/usr/local" using "--prefix",\
			for instance "--prefix=$HOME"')
		inst_dir.add_option(tmp_prefix)
		inst_dir.add_option(tmp_destdir)
	except: pass

	opt.tool_options('compiler_cc')
	os_compiler_opts = opt.add_option_group("Multisync-Gui Build Options")
	os_compiler_opts.add_option('--develop-mode', action='store_true', default=False,
		help='Should  developer mode be enabled? \
		This way the program don`t need to be installed to be runned', dest='os_develop_mode')
	opt_helper(os_compiler_opts, "rpath", True, 'Should rpath be disabled?')
