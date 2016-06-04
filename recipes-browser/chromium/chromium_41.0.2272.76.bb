
include chromium-browser.inc

LIC_FILES_CHKSUM = "file://LICENSE;md5=537e0b52077bf0a616d0a0c8a79bc9d5"

SRC_URI += "\
        file://chromium-41/0004-Remove-hard-coded-values-for-CC-and-CXX.patch \
        file://chromium-41/0005-removed_using_of_non_existing_method.patch \
        ${@bb.utils.contains('PACKAGECONFIG', 'ignore-lost-context', 'file://chromium-41/0001-Remove-accelerated-Canvas-support-from-blacklist.patch', '', d)} \
        ${@bb.utils.contains('PACKAGECONFIG', 'impl-side-painting', 'file://chromium-41/0002-Add-Linux-to-impl-side-painting-whitelist.patch', '', d)} \
        ${@bb.utils.contains('PACKAGECONFIG', 'disable-api-keys-info-bar', 'file://chromium-41/0003-Disable-API-keys-info-bar.patch', '', d)} \
"
SRC_URI[md5sum] = "2941d1de53b1344eb6dbfb6cd20be83e"
SRC_URI[sha256sum] = "31e2ae88ad4604e79dbcf7258f66106bd51e88413ebda6a1b1d181967535cd8a"

# ozone-wayland ########################################################################################################################
OZONE_WAYLAND_GIT_BRANCH = "Milestone-Carnival"
OZONE_WAYLAND_GIT_SRCREV = "ebf634371f66955d04234f30e6b52432d16951c6"

OZONE_WAYLAND_EXTRA_PATCHES = " \
        file://chromium-41/0005-Remove-X-libraries-from-GYP-files.patch \
        ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://chromium-41/0010-systemd-218.patch', '', d)} \
"

# Component build is broken in ozone-wayland for Chromium 41
python() {
    if (d.getVar('CHROMIUM_USE_OZONE_WAYLAND', True) == '1'):
        if bb.utils.contains('PACKAGECONFIG', 'component-build', True, False, d):
            bb.fatal("Chromium 41 Wayland version cannot be built in component-mode")
}

# ozone-egl ###########################################################################################################################
OZONE_EGL_GIT_BRANCH = "chromium-41.0.2272.x"
OZONE_EGL_GIT_SRCREV = "1f71d654585946844acbe0d44c856c1f57b35ad8"

# ozone-fb ###########################################################################################################################
OZONE_FB_GIT_BRANCH = "chromium-41.0.2272.x"
OZONE_FB_GIT_SRCREV = "0b962c137fd20d75f8d0beb3a64be6a668786fdd"

#######################################################################################################################################
EXTRA_GYP_DEFINES += "\
    use_allocator=tcmalloc \
"
