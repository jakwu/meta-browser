
include chromium-browser.inc

LIC_FILES_CHKSUM = "file://LICENSE;md5=537e0b52077bf0a616d0a0c8a79bc9d5"

SRC_URI += "\
        file://unistd-2.patch \
        file://chromium-40/fix-build-error-with-GCC-in-Debug-mode.patch \
        file://chromium-40/add_missing_stat_h_include.patch \
        file://chromium-40/0001-bignum.cc-disable-warning-from-gcc-5.patch \
        file://chromium-40/0002-image_util.cc-disable-warning-from-gcc-5.patch \
        file://chromium-40/0004-Remove-hard-coded-values-for-CC-and-CXX.patch \
        ${@bb.utils.contains('PACKAGECONFIG', 'ignore-lost-context', 'file://chromium-40/0001-Remove-accelerated-Canvas-support-from-blacklist.patch', '', d)} \
        ${@bb.utils.contains('PACKAGECONFIG', 'impl-side-painting', 'file://chromium-40/0002-Add-Linux-to-impl-side-painting-whitelist.patch', '', d)} \
        ${@bb.utils.contains('PACKAGECONFIG', 'disable-api-keys-info-bar', 'file://chromium-40/0003-Disable-API-keys-info-bar.patch', '', d)} \
"
SRC_URI[md5sum] = "1f5093bd7e435fdebad070e74bfb3438"
SRC_URI[sha256sum] = "f72fda9ff1ea256ab911610ee532eadf8303137d431f2481d01d3d60e5e64149"

# ozone-wayland ########################################################################################################################
OZONE_WAYLAND_GIT_BRANCH = "Milestone-ThanksGiving"
OZONE_WAYLAND_GIT_SRCREV = "5d7baa9bc3b8c88e9b7e476e3d6bc8cd44a887fe"

# Variable for extra ozone-wayland patches, typically extended by BSP layer .bbappends
# IMPORTANT: do not simply add extra ozone-wayland patches to the SRC_URI in a
# .bbappend, since the base ozone-wayland patches need to be applied first (see below)
OZONE_WAYLAND_EXTRA_PATCHES = " \
        file://chromium-40/0005-Remove-X-libraries-from-GYP-files.patch \
        ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://chromium-40/0010-systemd-218.patch', '', d)} \
"

# Component build is broken in ozone-wayland for Chromium 40,
# and is not planned to work again before version 41
python() {
    if (d.getVar('CHROMIUM_USE_OZONE_WAYLAND', True) == '1'):
        if bb.utils.contains('PACKAGECONFIG', 'component-build', True, False, d):
            bb.fatal("Chromium 40 Wayland version cannot be built in component-mode")
}

# ozone-egl ###########################################################################################################################
OZONE_EGL_GIT_BRANCH = "chromium-40.0.2214.x"
OZONE_EGL_GIT_SRCREV = "6f16fe474c58e46261adc22b0d01abc8d5a58d9d"

# ozone-fb ###########################################################################################################################
OZONE_FB_GIT_BRANCH = "chromium-40.0.2214.x"
OZONE_FB_GIT_SRCREV = "e8b794a79660d987c08119baf72d4f0236724bbc"

#######################################################################################################################################
EXTRA_GYP_DEFINES += "\
    linux_use_tcmalloc=1 \
"
