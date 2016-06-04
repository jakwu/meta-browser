# Recipe files have to perform the following tasks after including this file:
# 1) Add patches to SRC_URI. Version specific patches should be contained in a
#    "chromium-XX" subdirectory, where XX is the major version. There are also
#    patches that are shared amongst versions but may one day no longer be
#    needed (like unistd2.patch). These do not belong in such a subdirectory,
#    but still need to be explicitely be added. Do NOT add ozone-wayland patches
#    to SRC_URI here!
# 2) Add md5sum and sha256sum hashes of the tarball.
# 3) Add ozone-wayland patches to the OZONE_WAYLAND_EXTRA_PATCHES variable.
#    The rule with the chromium-XX subdirectory also applies here.
# 4) Set the OZONE_WAYLAND_GIT_BRANCH and OZONE_WAYLAND_GIT_SRCREV values.
# 5) Optionally, set values for these variables:
#    * OZONE_WAYLAND_PATCH_FILE_GLOB
#    * CHROMIUM_X11_DEPENDS
#    * CHROMIUM_X11_GYP_DEFINES
#    * CHROMIUM_WAYLAND_DEPENDS
#    * CHROMIUM_WAYLAND_GYP_DEFINES

include chromium.inc
DESCRIPTION = "Chromium browser"
DEPENDS += "libgnome-keyring"
CROMIUM_VERSION = "chromium-${PV}"
SRC_URI = "\
        http://gsdview.appspot.com/chromium-browser-official/${CROMIUM_VERSION}.tar.xz \
        file://google-chrome \
        file://google-chrome.desktop \
"

CHROMIUM_BUILD_TARGET ?= "chrome chrome_sandbox"

# PACKAGECONFIG options
# ^^^^^^^^^^^^^^^^^^^^^
# * use-egl: (on by default)
#       Without this packageconfig, the Chromium build will use GLX for
#       creating an OpenGL context in X11, and regular OpenGL for painting
#       operations. Neither are desirable on embedded platforms. With this
#       packageconfig, EGL and OpenGL ES 2.x are used instead.
#
# * disable-api-keys-info-bar: (off by default)
#       This disables the info bar that warns: "Google API keys are missing".
#       With some builds, missing API keys are considered OK, so the bar needs
#       to go. Conversely, if Chromium is compiled with this option off and
#       the user wishes to disable the warning, the following lines can be
#       added to the "google-chrome" script (see patchset) before the
#       chromium binary is called:
#           export GOOGLE_API_KEY="no"
#           export GOOGLE_DEFAULT_CLIENT_ID="no"
#           export GOOGLE_DEFAULT_CLIENT_SECRET="no"
#
# * component-build: (off by default)
#       Enables component build mode. By default, all of Chromium (with the
#       exception of FFmpeg) is linked into one big binary. The linker step
#       requires at least 8 GB RAM. Component mode was created to facilitate
#       development and testing, since with it, there is not one big binary;
#       instead, each component is linked to a separate shared object. Use
#       component mode for development, testing, and in case the build machine
#       is not a 64-bit one, or has less than 8 GB RAM.
#
# * ignore-lost-context: (off by default)
#       There is a flaw in the HTML Canvas specification. When the canvas'
#       backing store is some kind of hardware resource like an OpenGL
#       texture, this resource might get lost. In case of OpenGL textures,
#       this happens when the OpenGL context gets lost. The canvas should then
#       be repainted, but nothing in the Canvas standard reflects that. This
#       packageconfig is to be used if the underlying OpenGL (ES) drivers do
#       not lose the context, or if losing the context is considered okay
#       (note that canvas contents can vanish then).
#
# * impl-side-painting: (off by default)
#       This is a new painting mechanism. Still in
#       development stages, it can improve performance See
#       http://www.chromium.org/developers/design-documents/impl-side-painting
#       for more.
#
# * kiosk-mode: (off by default)
#       Enable this option if you want your browser to start up full-screen,
#       without any menu bars, without any clutter, and without any initial
#       start-up indicators.
#
# * proprietary-codecs: (off by default)
#       Enable this option if you want to add support for additional proprietary
#       codecs, most notably MPEG standards (h.264, h.265, MP4, MP3, AAC, MPEG-2 ..)


PACKAGECONFIG ??= "use-egl"

# this makes sure the dependencies for the EGL mode are present; otherwise, the configure scripts
# automatically and silently fall back to GLX
PACKAGECONFIG[use-egl] = ",,virtual/egl virtual/libgles2"

# Additional PACKAGECONFIG options - listed here to avoid warnings
PACKAGECONFIG[component-build] = ""
PACKAGECONFIG[disable-api-keys-info-bar] = ""
PACKAGECONFIG[ignore-lost-context] = ""
PACKAGECONFIG[impl-side-painting] = ""
PACKAGECONFIG[kiosk-mode] = ""
PACKAGECONFIG[proprietary-codecs] = ""
PACKAGECONFIG[disable-nacl] = ""


CHROMIUM_USE_X11 ?= "${@bb.utils.contains('DISTRO_FEATURES', 'wayland', '0', \
                     bb.utils.contains('DISTRO_FEATURES', 'x11', '1', \
                     '0', d), d)}"
# Conditionally add ozone-wayland and its patches to the Chromium sources
# You can override this by setting CHROMIUM_USE_OZONE_WAYLAND=1 or CHROMIUM_USE_OZONE_WAYLAND=0 in local.conf
CHROMIUM_USE_OZONE_WAYLAND ?= "${@bb.utils.contains('DISTRO_FEATURES', 'x11', '0', \
                     bb.utils.contains('DISTRO_FEATURES', 'wayland', '1', \
                     '0', d),d)}"
# Conditionally add ozone-egl and its patches to the Chromium sources
# You can override this by setting CHROMIUM_USE_OZONE_EGL=1 or CHROMIUM_USE_OZONE_EGL=0 in local.conf
CHROMIUM_USE_OZONE_EGL ?= "${@bb.utils.contains('DISTRO_FEATURES', 'x11', '0', \
                                bb.utils.contains('DISTRO_FEATURES', 'wayland', '0', \
                                  bb.utils.contains('PACKAGECONFIG', 'use-egl', '1', \
                                  '0', d), \
                                d), \
                              d)}"

CHROMIUM_USE_OZONE_FB ?= "${@bb.utils.contains('PACKAGECONFIG', 'use-egl', '0', '1', d)}"
                     
CHROMIUM_EXTRA_ARGS ?= " \
    ${@oe.utils.conditional('CHROMIUM_USE_OZONE_EGL', '1', '--ozone-platform=egl', '', d)} \
    ${@oe.utils.conditional('CHROMIUM_USE_OZONE_FB', '1', '--ozone-platform=fb', '', d)} \
    ${@bb.utils.contains('CHROMIUM_BUILD_TARGET', 'chrome_sandbox', '', '--no-sandbox', d)} \
    ${@bb.utils.contains('PACKAGECONFIG', 'use-egl', '--use-gl=egl', '--disable-gpu', d)} \
    ${@bb.utils.contains('PACKAGECONFIG', 'ignore-lost-context', '--gpu-no-context-lost', '', d)} \
    ${@bb.utils.contains('PACKAGECONFIG', 'impl-side-painting', '--enable-gpu-rasterization --enable-impl-side-painting', '', d)} \
    ${@bb.utils.contains('PACKAGECONFIG', 'kiosk-mode', '--start-fullscreen --kiosk --no-first-run', '', d)} \
"


# ozone-wayland ########################################################################################################################
OZONE_WAYLAND_GIT_BRANCH ?= "master"
OZONE_WAYLAND_GIT_SRCREV ?= "0"
SRC_URI += "${@oe.utils.conditional('CHROMIUM_USE_OZONE_WAYLAND', '1', 'git://github.com/01org/ozone-wayland.git;destsuffix=${CROMIUM_VERSION}/ozone;branch=${OZONE_WAYLAND_GIT_BRANCH};rev=${OZONE_WAYLAND_GIT_SRCREV}', '', d)} "

# Variable for extra ozone-wayland patches, typically extended by BSP layer .bbappends
# IMPORTANT: do not simply add extra ozone-wayland patches to the SRC_URI in a
# .bbappend, since the base ozone-wayland patches need to be applied first (see below)
OZONE_WAYLAND_EXTRA_PATCHES ?= ""
# using 00*.patch to skip the WebRTC patches in ozone-wayland
# the WebRTC patches remove X11 libraries from the linker flags, which is
# already done by another patch (see above). Furthermore, to be able to use
# these patches, it is necessary to update the git repository in third_party/webrtc,
# which would further complicate this recipe.
OZONE_WAYLAND_PATCH_FILE_GLOB = "00*.patch"

do_patch[prefuncs] += "${@oe.utils.conditional('CHROMIUM_USE_OZONE_WAYLAND', '1', 'add_ozone_wayland_patches', '', d)}"

python add_ozone_egl_patches() {
    import glob
    srcdir = d.getVar('S', True)
    # find all ozone-egl patches and add them to SRC_URI
    upstream_patches_dir = srcdir + "/ozone/patches"
    if os.path.exists(upstream_patches_dir):
        upstream_patches = glob.glob(upstream_patches_dir + "/" + d.getVar('OZONE_WAYLAND_PATCH_FILE_GLOB', True))
        upstream_patches.sort()
        for upstream_patch in upstream_patches:
            d.appendVar('SRC_URI', ' file://' + upstream_patch)
    # then, add the extra patches to SRC_URI order matters;
    # extra patches may depend on the base ozone-egl ones
    d.appendVar('SRC_URI', ' ' + d.getVar('OZONE_WAYLAND_EXTRA_PATCHES', True))
}


# ozone-egl ###########################################################################################################################
OZONE_EGL_GIT_BRANCH ?= "master"
OZONE_EGL_GIT_SRCREV ?= "0"
OZONE_EGL_SRC_URI ?= "git://github.com/jakwu/ozone-egl.git;destsuffix=${CROMIUM_VERSION}/ozone-egl;branch=${OZONE_EGL_GIT_BRANCH};rev=${OZONE_EGL_GIT_SRCREV}"
SRC_URI += "${@oe.utils.conditional('CHROMIUM_USE_OZONE_EGL', '1', '${OZONE_EGL_SRC_URI}', '', d)} "
OZONE_EGL_PATCH_FILE_GLOB = "*.patch"
OZONE_EGL_EXTRA_PATCHES ?= ""

do_patch[prefuncs] += "${@oe.utils.conditional('CHROMIUM_USE_OZONE_EGL', '1', 'add_ozone_egl_patches', '', d)}"

python add_ozone_egl_patches() {
    import glob
    srcdir = d.getVar('S', True)
    # find all ozone-egl patches and add them to SRC_URI
    upstream_patches_dir = srcdir + "/ozone-egl/patches"
    if os.path.exists(upstream_patches_dir):
        upstream_patches = glob.glob(upstream_patches_dir + "/" + d.getVar('OZONE_EGL_PATCH_FILE_GLOB', True))
        upstream_patches.sort()
        for upstream_patch in upstream_patches:
            d.appendVar('SRC_URI', ' file://' + upstream_patch)
    # then, add the extra patches to SRC_URI order matters;
    # extra patches may depend on the base ozone-egl ones
    d.appendVar('SRC_URI', ' ' + d.getVar('OZONE_EGL_EXTRA_PATCHES', True))
}

# ozone-egl ###########################################################################################################################
OZONE_FB_GIT_BRANCH ?= "master"
OZONE_FB_GIT_SRCREV ?= "0"
OZONE_FB_SRC_URI ?= "git://github.com/jakwu/ozone-fb.git;destsuffix=${CROMIUM_VERSION}/ozone-fb;branch=${OZONE_FB_GIT_BRANCH};rev=${OZONE_FB_GIT_SRCREV}"
SRC_URI += "${@oe.utils.conditional('CHROMIUM_USE_OZONE_FB', '1', '${OZONE_FB_SRC_URI}', '', d)} "
OZONE_FB_PATCH_FILE_GLOB = "*.patch"
OZONE_FB_EXTRA_PATCHES ?= ""

do_patch[prefuncs] += "${@oe.utils.conditional('CHROMIUM_USE_OZONE_FB', '1', 'add_ozone_fb_patches', '', d)}"

python add_ozone_fb_patches() {
    import glob
    srcdir = d.getVar('S', True)
    # find all ozone-egl patches and add them to SRC_URI
    upstream_patches_dir = srcdir + "/ozone-fb/patches"
    if os.path.exists(upstream_patches_dir):
        upstream_patches = glob.glob(upstream_patches_dir + "/" + d.getVar('OZONE_FB_PATCH_FILE_GLOB', True))
        upstream_patches.sort()
        for upstream_patch in upstream_patches:
            d.appendVar('SRC_URI', ' file://' + upstream_patch)
    # then, add the extra patches to SRC_URI order matters;
    # extra patches may depend on the base ozone-egl ones
    d.appendVar('SRC_URI', ' ' + d.getVar('OZONE_FB_EXTRA_PATCHES', True))
}


#######################################################################################################################################
EXTRA_GYP_DEFINES ?= " \
    use_system_bzip2=1 \
    v8_use_snapshot=1 v8_use_external_startup_data=0 \
    use_kerberos=0 linux_link_kerberos=0 \
    use_gnome_keyring=0 linux_link_gnome_keyring=0 \
    enable_plugins=1 \
    enable_pdf=1 pdf_enable_v8=1 \
    ${@bb.utils.contains('PACKAGECONFIG', 'disable-nacl', 'disable_nacl=1', '', d)} \    
"

OE_GYP_DEFINES = " \
	angle_use_commit_id=0 \
	clang=0 \
	host_clang=0 \
	disable_fatal_linker_warnings=1 \
	werror= \
	${@bb.utils.contains('DISTRO_FEATURES', 'ld-is-gold', '', 'linux_use_gold_binary=0', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'ld-is-gold', '', 'linux_use_gold_flags=0', d)} \
	${@bb.utils.contains('PACKAGECONFIG', 'component-build', 'component=shared_library', '', d)} \
	${@bb.utils.contains('PACKAGECONFIG', 'proprietary-codecs', 'proprietary_codecs=1 ffmpeg_branding=Chrome', '', d)} \
	${EXTRA_GYP_DEFINES} \
"

GYP_DEFINES += " ${OE_GYP_DEFINES}"

# These are present as their own variables, since they have changed between versions
# a few times in the past already; making them variables makes it easier to handle that
CHROMIUM_X11_DEPENDS ?= "pango cairo xextproto gtk+ libxi libxss"
CHROMIUM_X11_GYP_DEFINES ?= ""
CHROMIUM_OZONE_WAYLAND_DEPENDS ?= "pango cairo wayland libxkbcommon"
CHROMIUM_OZONE_WAYLAND_GYP_DEFINES ?= "use_ash=1 use_aura=1 chromeos=0 use_ozone=1 ozone_auto_platforms=0"
# NOTE: We need at leased one font installed. Otherwise the renderer process will crash sillently.
CHROMIUM_OZONE_EGL_DEPENDS ?= "liberation-fonts"
CHROMIUM_OZONE_EGL_GYP_DEFINES ?= "embedded=1 ozone_platform_egl=1 ozone_platform=egl ozone_auto_platforms=0"
# NOTE: We need at leased one font installed. Otherwise the renderer process will crash sillently.
CHROMIUM_OZONE_FB_DEPENDS ?= "liberation-fonts"
CHROMIUM_OZONE_FB_GYP_DEFINES ?= "embedded=1 ozone_platform_fb=1 ozone_platform=fb ozone_auto_platforms=0"

python() {
    if d.getVar('CHROMIUM_USE_OZONE_WAYLAND', True) == '1':
        d.appendVar('DEPENDS', ' %s ' % d.getVar('CHROMIUM_OZONE_WAYLAND_DEPENDS', True))
        d.appendVar('GYP_DEFINES', ' %s ' % d.getVar('CHROMIUM_OZONE_WAYLAND_GYP_DEFINES', True))
    elif d.getVar('CHROMIUM_USE_X11', True) == '1':
        d.appendVar('DEPENDS', ' %s ' % d.getVar('CHROMIUM_X11_DEPENDS', True))
        d.appendVar('GYP_DEFINES', ' %s ' % d.getVar('CHROMIUM_X11_GYP_DEFINES', True))
    elif d.getVar('CHROMIUM_USE_OZONE_EGL', True) == '1':
        d.appendVar('DEPENDS', ' %s ' % d.getVar('CHROMIUM_OZONE_EGL_DEPENDS', True))
        d.appendVar('GYP_DEFINES', ' %s ' % d.getVar('CHROMIUM_OZONE_EGL_GYP_DEFINES', True))
    else:
        d.appendVar('DEPENDS', ' %s ' % d.getVar('CHROMIUM_OZONE_FB_DEPENDS', True))
        d.appendVar('GYP_DEFINES', ' %s ' % d.getVar('CHROMIUM_OZONE_FB_GYP_DEFINES', True))
}

# Some sanity checks.
python do_check_variables() {
    CHROMIUM_BUILD_TYPE = d.getVar('CHROMIUM_BUILD_TYPE', True)
    CHROMIUM_USE_X11 = d.getVar('CHROMIUM_USE_X11', True)
    CHROMIUM_USE_OZONE_WAYLAND = d.getVar('CHROMIUM_USE_OZONE_WAYLAND', True)
    CHROMIUM_USE_OZONE_EGL = d.getVar('CHROMIUM_USE_OZONE_EGL', True)
    CHROMIUM_USE_OZONE_FB = d.getVar('CHROMIUM_USE_OZONE_FB', True)
    DISTRO_FEATURES = d.getVar("DISTRO_FEATURES", True).split()

    if CHROMIUM_BUILD_TYPE not in ['Release', 'Debug']:
        bb.fatal("Wrong value for CHROMIUM_BUILD_TYPE. Please set it either to \'Release\' or to \'Debug\'")

    if CHROMIUM_USE_X11 not in ['0', '1']:
        bb.fatal("Wrong value for CHROMIUM_USE_X11. Please set it to \'1\' to enable the feature or to \'0\' to disable it")
    if CHROMIUM_USE_OZONE_WAYLAND not in ['0', '1']:
        bb.fatal("Wrong value for CHROMIUM_USE_OZONE_WAYLAND. Please set it to \'1\' to enable the feature or to \'0\' to disable it")
    if CHROMIUM_USE_OZONE_EGL not in ['0', '1']:
        bb.fatal("Wrong value for CHROMIUM_USE_OZONE_EGL. Please set it to \'1\' to enable the feature or to \'0\' to disable it")
    if CHROMIUM_USE_OZONE_FB not in ['0', '1']:
        bb.fatal("Wrong value for CHROMIUM_USE_OZONE_FB. Please set it to \'1\' to enable the feature or to \'0\' to disable it")

    if ( (CHROMIUM_USE_OZONE_WAYLAND == '1') and ('wayland' not in DISTRO_FEATURES) ):
        bb.warn("You have selected to build Chromium with Wayland support, but you have not enabled wayland in DISTRO_FEATURES")
    if ( (CHROMIUM_USE_X11 == '1') and ('x11' not in DISTRO_FEATURES) ):
        bb.warn("You have selected to build Chromium witht X11 support, but you have not enabled x11 in DISTRO_FEATURES")

    # Print both on log.do_checkvariables and on the console the configuration that is selected.
    # This useful both for throubleshooting and for checking how the build is finally configured.
    if (CHROMIUM_USE_X11 == '1'):
        bb.plain("INFO: Chromium has been configured with X11 support. Build type is \'%s\'" %CHROMIUM_BUILD_TYPE)
    if (CHROMIUM_USE_OZONE_WAYLAND == '1'):
        bb.plain("INFO: Chromium has been configured with Wayland support (ozone-wayland). Build type is \'%s\'" %CHROMIUM_BUILD_TYPE)
    if (CHROMIUM_USE_OZONE_EGL == '1'):
        bb.plain("INFO: Chromium has been configured with EGLFB support (ozone-egl). Build type is \'%s\'" %CHROMIUM_BUILD_TYPE)
    if (CHROMIUM_USE_OZONE_FB == '1'):
        bb.plain("INFO: Chromium has been configured with frame buffer support (ozone-fb). Build type is \'%s\'" %CHROMIUM_BUILD_TYPE)
}
addtask check_variables before do_fetch

do_configure_append() {
	export GYP_GENERATORS="ninja"
	build/gyp_chromium --depth=.
}

do_compile() {
    # build with ninja
    ninja -C ${S}/out/${CHROMIUM_BUILD_TYPE} ${PARALLEL_MAKE} ${CHROMIUM_BUILD_TARGET}
}


do_install_append() {

	# Add extra command line arguments to google-chrome script by modifying
        # the dummy "CHROME_EXTRA_ARGS" line
        sed -i "s/^CHROME_EXTRA_ARGS=\"\"/CHROME_EXTRA_ARGS=\"${CHROMIUM_EXTRA_ARGS}\"/" ${D}${bindir}/google-chrome

        # update ROOT_HOME with the root user's $HOME
        sed -i "s#ROOT_HOME#${ROOT_HOME}#" ${D}${bindir}/google-chrome

	# Always adding this libdir (not just with component builds), because the
        # LD_LIBRARY_PATH line in the google-chromium script refers to it
        install -d ${D}${libdir}/${BPN}/
        if [ -n "${@bb.utils.contains('PACKAGECONFIG', 'component-build', 'component-build', '', d)}" ]; then
                install -m 0755 ${B}/out/${CHROMIUM_BUILD_TYPE}/lib/*.so ${D}${libdir}/${BPN}/
        fi
  # Cleanup
        for f in libpdf.so libosmesa.so libffmpegsumo.so; do
                if [ -f "${D}${libdir}/${BPN}/$f" ]; then
                  rm -f ${D}${libdir}/${BPN}/$f
                fi
        done

}
