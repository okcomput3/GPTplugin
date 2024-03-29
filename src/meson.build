epoxy_dep = dependency('epoxy', version: '>= 1.5.4', required: true)

gtk = dependency('gtk+-3.0', version: '>= 3.24')
# Define Skia include directory and library
cpp = meson.get_compiler('cpp')

# Assuming Skia is installed in /usr/local
skia_inc = include_directories('/usr/local/include/skia')
# Use the compiler object to find the Skia library
# If the Skia library files (e.g., .so, .a) are in /usr/local/lib/skia
skia_lib_dir = '/usr/local/lib/skia'

# Attempt to find the Skia library within the specified directory
skia_lib = cpp.find_library('skia', dirs : [skia_lib_dir])

if not skia_lib.found()
    error('Skia library not found in ' + skia_lib_dir)
endif

annotate = shared_module('annotate', 'annotate.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

if giomm.found()
autorotate = shared_module('autorotate-iio', 'autorotate-iio.cpp',
    dependencies: [wayfire, giomm],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))
endif

background_view = shared_module('background-view', 'background-view.cpp',
    dependencies: [wayfire, wf_protos],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

benchmark = shared_module('bench', 'bench.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

crosshair = shared_module('crosshair', 'crosshair.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

focus_change = shared_module('focus-change', 'focus-change.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

focus_steal_prevent = shared_module('focus-steal-prevent', 'focus-steal-prevent.cpp',
    dependencies: [wayfire, evdev],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

follow_focus = shared_module('follow-focus', 'follow-focus.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

force_fullscreen = shared_module('force-fullscreen', 'force-fullscreen.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

if giomm.found()
glib_main_loop = shared_module('glib-main-loop', 'glib-main-loop.cpp',
    dependencies: [wayfire, giomm],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))
endif

hide_cursor = shared_module('hide-cursor', 'hide-cursor.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

joinviews = shared_module('join-views', 'join-views.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

keycolor = shared_module('keycolor', 'keycolor.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

magnifier = shared_module('mag', 'mag.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

if json.found()
obs = shared_module('obs', 'obs.cpp',
    dependencies: [wayfire, json],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))
endif

showrepaint = shared_module('showrepaint', 'showrepaint.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

view_shot = shared_module('view-shot', 'view-shot.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

water = shared_module('water', 'water.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

window_zoom = shared_module('winzoom', 'window-zoom.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

libcurl_dep = dependency('libcurl')
# Find EGL dependency
egl_dep = dependency('egl', required : true)

    shared_module('workspace-names', 'workspace-names.cpp',
    dependencies: [wayfire, libcurl_dep, gtk, skia_lib, epoxy_dep, egl_dep], # Added epoxy_dep here
    include_directories: skia_inc,
    install: true,
    install_dir: join_paths(get_option('libdir'), 'wayfire'))


hinge = shared_module('hinge', 'hinge.cpp',
    dependencies: [wayfire],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))

shared_module('GPTscale', ['GPTscale.cpp'],
        dependencies: [wayfire, libcurl_dep],
        install: true,
        install_dir: join_paths(get_option('libdir'), 'wayfire'))

shared_module('GPTscale-title-filter', 'GPTscale-title-filter.cpp',
        dependencies: [wayfire, libcurl_dep],
        install: true,
        install_dir: join_paths(get_option('libdir'), 'wayfire'))
