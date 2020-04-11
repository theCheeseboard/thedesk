TEMPLATE = subdirs

libraryproj.subdir = libthedesk

deskproj.subdir = desktop
deskproj.depends = libraryproj

pluginsproj.subdir = plugins
pluginsproj.depends = libraryproj

SUBDIRS = libraryproj \
    deskproj \
    pluginsproj \
    startdesk
