#! /usr/bin/env python
# encoding: utf-8
# Matthias Jahn <jahn.matthias@freenet.de>, 2006 (pmarat)

import os, sys

# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='multisync-qad'

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = '_build_'

def build(bld):
    # process subfolders from here
    bld.add_subdirs('src misc')

def configure(conf):
	import Params
	conf.check_tool('gcc misc')
	
	# pkg-config example - look in src/wscript_build, obj.uselib ..
	if not conf.check_pkg('glib-2.0', destvar='GLIB'):
		print "you need glib-2.0 (libglib2.0-dev)"
		sys.exit(1)
	    
	if not conf.check_pkg('libxml-2.0', destvar='XML'):
		print "you need libxml-2.0 (libxml2-dev)"
		sys.exit(1)
	    
	if not conf.check_pkg('gtk+-2.0',            destvar='GTK',      vnum='2.6.0'):
		print "you need libgtk-2.0-dev"
		sys.exit(1)
		
	if not conf.check_pkg('libglade-2.0',        destvar='GLADE',    vnum='2.0.1'):
		print "you need libglade-2.0-dev"
		sys.exit(1)
		
	pkgconf = conf.create_pkgconfig_configurator()
	pkgconf.uselib = 'OPENSYNC'
	pkgconf.name = 'opensync-1.0'
	pkgconf.variables = "configdir plugindir formatsdir"
	if not pkgconf.run():
		print "you need opensync http://www.opensync.org/ (libopensync0-dev)"
		sys.exit(1)
	
	if not conf.check_pkg('osengine-1.0', vnum='0.19', destvar='OPENSYNCENGINE'):
		print "you need opensync http://www.opensync.org/ (libopensync0-dev)"
		sys.exit(1)
	if not Params.g_options.os_develop_mode :
		conf.add_define('MULTISYNC_DATA', os.path.join(conf.env['PREFIX'], 'share/multisync-qad'))
		conf.add_define('MULTISYNC_ICON_DIR', os.path.join(conf.env['PREFIX'], 'share/pixmaps/multisync-qad'))
	
	#Set project global include path
	conf.env['CXXFLAGS_GLOBAL_PATH'] = '-I%s' % os.getcwd()
	
	# write a config.h
	conf.write_config_header('config.h')

def set_options(opt):
	opt.add_option('--develop-mode', action='store_true', default=False,
		help='Should  developer mode be enabled? \
		\nThis way the program don`t need to be installed to run', dest='os_develop_mode')
	opt.add_option('--build', type='string', help='not needed automake option', dest='ignore')
	opt.add_option('--includedir', type='string', help='not needed automake option', dest='ignore')
	opt.add_option('--mandir', type='string', help='not needed automake option', dest='ignore')
	opt.add_option('--infodir', type='string', help='not needed automake option', dest='ignore')
	opt.add_option('--sysconfdir', type='string', help='not needed automake option', dest='ignore')
	opt.add_option('--localstatedir', type='string', help='not needed automake option', dest='ignore')
	opt.add_option('--libexecdir', type='string', help='not needed automake option', dest='ignore')
	opt.add_option('--disable-maintainer-mode', action='store_true', default=False, help='not needed automake option', dest='ignore')
	opt.add_option('--disable-dependency-tracking', action='store_true', default=False, help='not needed automake option', dest='ignore')
	

