TEMPLATE = subdirs

libraryproj.subdir = libthedesk

deskproj.subdir = desktop
deskproj.depends = libraryproj

pluginsproj.subdir = plugins
pluginsproj.depends = libraryproj

startdeskproj.subdir = startdesk
startdeskproj.depends = libraryproj

SUBDIRS = libraryproj \
    deskproj \
    platform \
    pluginsproj \
    polkitagent \
    startdeskproj

packagesExist(wayfire) {
    SUBDIRS +=  wayfire-plugins
}
