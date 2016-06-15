
include chromium-browser.inc

LIC_FILES_CHKSUM = "file://LICENSE;md5=0fca02217a5d49a14dfe2d11837bb34d"

SRC_URI += "\
        file://chromium-51/add_missing_stat_h_include.patch \
        file://chromium-51/0004-Remove-hard-coded-values-for-CC-and-CXX.patch \
        file://chromium-51/0005-Override-root-filesystem-access-restriction.patch \
        file://chromium-51/0006-link-with-latomic.patch \
        ${@bb.utils.contains('PACKAGECONFIG', 'component-build', 'file://chromium-51/0007-no-extra-flags-for-chromecast.patch', '', d)} \
        file://chromium-51/0008-Create-client-cert-store-in-content_shell.patch \
        file://chromium-51/0009-Do-not-use-ozone-cast-funktion-in-chromecast.patch \
        ${@bb.utils.contains('PACKAGECONFIG', 'ignore-lost-context', 'file://chromium-51/0001-Remove-accelerated-Canvas-support-from-blacklist.patch', '', d)} \
        ${@bb.utils.contains('PACKAGECONFIG', 'disable-api-keys-info-bar', 'file://chromium-51/0002-Disable-API-keys-info-bar.patch', '', d)} \
"
SRC_URI[md5sum] = "4bb0e0032391b53d8dead82462f0c4cd"
SRC_URI[sha256sum] = "33a3fe1cb8fa5be7b35aa8ae00c8b0a6dc90975bfa9aea9865180b57b0a4dc6a"

# ozone-wayland ########################################################################################################################
CHROMIUM_USE_OZONE_WAYLAND = "0"

# ozone-egl ###########################################################################################################################
CHROMIUM_USE_OZONE_EGL = "0"

# ozone-fb ###########################################################################################################################
OZONE_FB_GIT_BRANCH = "chromium-51.0.2704.x"
OZONE_FB_GIT_SRCREV = "57d05a8bcaf19d0ca193feaf30533eebe892d47d"

#######################################################################################################################################
# Component build is unsupported for build target cast_shell
#python() {
#    if bb.utils.contains('PACKAGECONFIG', 'component-build', True, False, d):
#        if bb.utils.contains('CHROMIUM_BUILD_TARGET', 'cast_shell', True, False, d):
#            bb.fatal("Chromium 51 cast_shell cannot be built in component-mode")
#}

#######################################################################################################################################
EXTRA_GYP_DEFINES += " \
    use_allocator=tcmalloc \
    ${@bb.utils.contains('CHROMIUM_BUILD_TARGET', 'cast_shell', 'chromecast=1', '', d)} \    
    ${@oe.utils.conditional('CHROMIUM_USE_OZONE_WAYLAND', '1', 'is_cast_desktop_build=1', '', d)} \
    ${@oe.utils.conditional('CHROMIUM_USE_X11', '1', 'is_cast_desktop_build=1', '', d)} \    
"
