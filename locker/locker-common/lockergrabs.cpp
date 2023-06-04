#include "lockergrabs.h"

#ifdef HAVE_X11
    #include <tx11info.h>

    #include <X11/Xlib.h>
#endif

void LockerGrabs::establishGrab() {
#ifdef HAVE_X11
    if (tX11Info::isPlatformX11()) {
        XGrabKeyboard(tX11Info::display(), tX11Info::appRootWindow(), True, GrabModeAsync, GrabModeAsync, CurrentTime);
        XGrabPointer(tX11Info::display(), tX11Info::appRootWindow(), True, None, GrabModeAsync, GrabModeAsync, tX11Info::appRootWindow(), None, CurrentTime);
    }
#endif
}

void LockerGrabs::releaseGrab() {
#ifdef HAVE_X11
    if (tX11Info::isPlatformX11()) {
        XUngrabKeyboard(tX11Info::display(), CurrentTime);
        XUngrabPointer(tX11Info::display(), CurrentTime);
    }
#endif
}
