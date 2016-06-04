LICENSE = "BSD"
DEPENDS = "xz-native pciutils pulseaudio nss zlib-native libav cups ninja-native gconf libexif libdrm"

COMPATIBLE_MACHINE = "(-)"
COMPATIBLE_MACHINE_x86 = "(.*)"
COMPATIBLE_MACHINE_x86-64 = "(.*)"
COMPATIBLE_MACHINE_armv6 = "(.*)"
COMPATIBLE_MACHINE_armv7a = "(.*)"

# The build type defaults to Release. If you want a Debug build, you can set
# the variable CHROMIUM_BUILD_TYPE to "Debug" in your conf/local.conf file.
CHROMIUM_BUILD_TYPE ??= "Release"

inherit gettext pythonnative


GYP_ARCH_DEFINES = ""
GYP_ARCH_DEFINES_i586 = " target_arch=ia32"
GYP_ARCH_DEFINES_x86-64 = " target_arch=x64"
GYP_ARCH_DEFINES_armv7a = " target_arch=arm armv7=1 arm_thumb=1 arm_neon=1 ${@bb.utils.contains('TUNE_FEATURES', 'callconvention-hard', 'arm_float_abi=hard', 'arm_float_abi=softfp', d)}"
GYP_ARCH_DEFINES_armv6 = " target_arch=arm armv7=0 arm_neon=0"

CHROMIUM_EXTRA_CFLAGS ?= ""
GYP_DEFINES += "${GYP_ARCH_DEFINES} sysroot=''"

do_unpack[postfuncs] += "prepare_proj_dir"

prepare_proj_dir() {
    touch ${S}/chrome/test/data/webui/i18n_process_css_test.html
}

do_configure() {
	cd ${S}
	GYP_DEFINES="${GYP_DEFINES}" export GYP_DEFINES
	# replace LD with CXX, to workaround a possible gyp issue?
	LD="${CXX}" export LD
	CC="${CC} ${CFLAGS} ${CHROMIUM_EXTRA_CFLAGS}" export CC
	CXX="${CXX} ${CXXFLAGS} ${CHROMIUM_EXTRA_CFLAGS}" export CXX
	CC_host="${BUILD_CC}" export CC_host
	CXX_host="${BUILD_CXX}" export CXX_host
}

CHROMIUM_PAK_FILES ?= " \
    content_resources.pak \
    keyboard_resources.pak \
    chrome_100_percent.pak \
    chrome_200_percent.pak \
    chrome_material_100_percent.pak \
    chrome_material_200_percent.pak \
    product_logo_48.png \
    resources.pak \
    locales/*.pak \
    content_shell.pak \
    cast_shell.pak \
"

do_install() {
	if [ -f "${WORKDIR}/google-chrome" ]; then
		install -Dm 0755 ${WORKDIR}/google-chrome ${D}${bindir}/google-chrome
	fi
	if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/chrome_sandbox" ]; then
		install -Dm 4755 ${B}/out/${CHROMIUM_BUILD_TYPE}/chrome_sandbox ${D}${sbindir}/chrome-devel-sandbox
	fi
	if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/chrome" ]; then
		install -Dm 0755 ${B}/out/${CHROMIUM_BUILD_TYPE}/chrome ${D}${bindir}/${BPN}/chrome
	fi
	if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/content_shell" ]; then
		install -Dm 0755 ${B}/out/${CHROMIUM_BUILD_TYPE}/content_shell ${D}${bindir}/${BPN}/content_shell
	fi
  if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/cast_shell" ]; then
    install -Dm 0755 ${B}/out/${CHROMIUM_BUILD_TYPE}/cast_shell ${D}${bindir}/${BPN}/cast_shell
  fi
	if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/icudtl.dat" ]; then
		install -Dm 0644 ${B}/out/${CHROMIUM_BUILD_TYPE}/icudtl.dat ${D}${bindir}/${BPN}/icudtl.dat
	fi
	if [ -f "${WORKDIR}/google-chrome.desktop" ]; then
		install -Dm 0644 ${WORKDIR}/google-chrome.desktop ${D}${datadir}/applications/google-chrome.desktop
	fi
	#Chromium plugins libs
	for f in libpdf.so libosmesa.so libffmpegsumo.so; do
		if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/$f" ]; then
			install -Dm 0644 ${B}/out/${CHROMIUM_BUILD_TYPE}/$f ${D}${bindir}/${BPN}/$f
		fi
    if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/lib/$f" ]; then
      install -Dm 0644 ${B}/out/${CHROMIUM_BUILD_TYPE}/$f ${D}${bindir}/${BPN}/$f
    fi
	done

	# Chromium *.pak files
	for f in ${CHROMIUM_PAK_FILES}; do
		if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/$f" ]; then
			install -Dm 0644 ${B}/out/${CHROMIUM_BUILD_TYPE}/$f ${D}${bindir}/${BPN}/$f
		fi
	done

}

PACKAGES += "${PN}-codecs-ffmpeg ${PN}-plugin-pdf"

FILES_${PN}-codecs-ffmpeg = "${bindir}/${BPN}/libffmpegsumo.so"
FILES_${PN}-plugin-pdf = "${bindir}/${BPN}/libpdf.so"
FILES_${PN}-libosmesa = "${bindir}/${BPN}/libosmesa.so"

FILES_${PN} = "${bindir}/${BPN} ${datadir}/applications ${sbindir}/ ${libdir}/${BPN}/"
FILES_${PN} += "${bindir} ${libdir}"
FILES_${PN} += "${bindir}/${BPN}/*.pak"
FILES_${PN} += "${bindir}/${BPN}/locales/*.pak"

FILES_${PN}-dbg += "${bindir}/${BPN}/.debug/ ${libdir}/${BPN}/.debug/ ${libdir}/.debug/"

PACKAGE_DEBUG_SPLIT_STYLE = "debug-without-src"

INSANE_SKIP_${PN} = "ldflags"
SOLIBS = ".so"
FILES_SOLIBSDEV = ""
