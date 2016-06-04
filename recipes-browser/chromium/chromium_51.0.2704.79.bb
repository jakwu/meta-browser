
include chromium-browser.inc

LIC_FILES_CHKSUM = "file://LICENSE;md5=0fca02217a5d49a14dfe2d11837bb34d"

SRC_URI += "\
        file://chromium-51/add_missing_stat_h_include.patch \
        file://chromium-51/0004-Remove-hard-coded-values-for-CC-and-CXX.patch \
        file://chromium-51/0005-Override-root-filesystem-access-restriction.patch \
        file://chromium-51/0006-link-with-latomic.patch \
        file://chromium-51/0007-no-extra-flags-for-chromecast.patch \
        ${@bb.utils.contains('PACKAGECONFIG', 'ignore-lost-context', 'file://chromium-51/0001-Remove-accelerated-Canvas-support-from-blacklist.patch', '', d)} \
        ${@bb.utils.contains('PACKAGECONFIG', 'disable-api-keys-info-bar', 'file://chromium-51/0002-Disable-API-keys-info-bar.patch', '', d)} \
"
SRC_URI[md5sum] = "9bfc70f8c4f37afeb3e5fa26336bf9cf"
SRC_URI[sha256sum] = "2323da80158f86e465f8fe257f681816ad58d831aaebddbb5337d72c5a86a036"

# ozone-wayland ########################################################################################################################
CHROMIUM_USE_OZONE_WAYLAND = "0"

# ozone-egl ###########################################################################################################################
CHROMIUM_USE_OZONE_EGL = "0"

# ozone-fb ###########################################################################################################################
OZONE_FB_GIT_BRANCH = "chromium-51.0.2704.x"
OZONE_FB_GIT_SRCREV = "57d05a8bcaf19d0ca193feaf30533eebe892d47d"

#######################################################################################################################################
EXTRA_GYP_DEFINES += " \
    use_allocator=tcmalloc \
    ${@bb.utils.contains('CHROMIUM_BUILD_TARGET', 'cast_shell', 'chromecast=1', '', d)} \    
    ${@oe.utils.conditional('CHROMIUM_USE_OZONE_WAYLAND', '1', 'is_cast_desktop_build=1', '', d)} \
    ${@oe.utils.conditional('CHROMIUM_USE_X11', '1', 'is_cast_desktop_build=1', '', d)} \    
"
