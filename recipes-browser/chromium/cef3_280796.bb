DESCRIPTION = "Chromium Embedded Framework"

include chromium.inc

RDEPENDS_${PN} += "pango cairo fontconfig pciutils pulseaudio freetype fontconfig-utils"

SRCREV_tools = "99bcb0e676eb396bcf8e1af3903aa4b578aeeee0"
SRCREV_cef = "bbad53dfca9f98dddcb31a590410fece0a4f0234"
SRCREV_egl = "a5b81b7617ba6757802b9b5f8c950034d5f961ec"
SRCREV_FORMAT = "cef_egl_tools"

SRC_URI = "http://people.linaro.org/~zoltan.kuscsik/chromium-browser/chromium_rev_${PV}.tar.xz \
           git://github.com/kuscsik/chromiumembedded.git;protocol=https;destsuffix=src/cef;branch=aura;name=cef \
           git://github.com/kuscsik/ozone-egl.git;protocol=https;destsuffix=src/ui/ozone/platform/egl;branch=master;name=egl \
           git://chromium.googlesource.com/chromium/tools/depot_tools.git;protocol=https;destsuffix=depot_tools;branch=master;name=tools \
           file://cef-simple \
           file://${PV}/01_get_svn_version_from_LASTCHANGE.patch \
           file://${PV}/0001-bignum.cc-disable-warning-from-gcc-5.patch \
           file://${PV}/0002-image_util.cc-disable-warning-from-gcc-5.patch \
           file://${PV}/0003-gtest-typed-test.h-disable-warning-unused-definition.patch \
           file://${PV}/0004-SaturatedArithmetic.h-put-parentheses-to-silence-war.patch \
"
SRC_URI[md5sum] = "9efbb50283b731042e62b9bd5e312b2f"
SRC_URI[sha256sum] = "f608e97dadf6ea4d885b24fd876896d46840fa39bf743ea2025075aee9fb348d"

S = "${WORKDIR}/chromium_rev_${PV}"

do_fetch[vardeps] += "SRCREV_FORMAT SRCREV_cef SRCREV_egl SRCREV_tools"

export GYP_GENERATORS="ninja"
export BUILD_TARGET_ARCH="${TARGET_ARCH}"

EXTRA_OEGYP =	" \
    ${@base_contains('DISTRO_FEATURES', 'ld-is-gold', '-Ddisable_fatal_linker_warnings=1', '', d)} \
"

CHROMIUM_PAK_FILES += " \
    cef_100_percent.pak \
    cef_200_percent.pak \
    cef_resources.pak \
    cef.pak \
"

do_configure_prepend() {
    # there is no rule for the x86-64 architecture, recycle the i586 one
    cp  cef/i586_ozone.gypi  cef/x86_64_ozone.gypi
}

do_configure_append() {
    export PATH=${WORKDIR}/depot_tools:"$PATH"
    # End of LD Workaround
    #-----------------------
    # Configure cef
    #------------------------
    cd cef
    ./cef_create_projects.sh -I ${BUILD_TARGET_ARCH}_ozone.gypi --depth ../ ${EXTRA_OEGYP}
    cd -
}

# Workaround to disable qa_configure
do_qa_configure() {
    echo "do_qa_configure"
}

do_compile() {
    ninja -C out/${CHROMIUM_BUILD_TYPE} ${PARALLEL_MAKE} cefsimple
}

CHROMIUM_PAK_FILES += " \
    cef_100_percent.pak \
    cef_200_percent.pak \
    cef_resources.pak \
    cef.pak \
   "

do_install_append() {
  if [ -f "${WORKDIR}/cef-simple" ]; then
    install -Dm 0755 ${WORKDIR}/cef-simple ${D}${bindir}/cef-simple
  fi
  if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/cefsimple" ]; then
    install -Dm 0755 ${B}/out/${CHROMIUM_BUILD_TYPE}/cefsimple ${D}${bindir}/${BPN}/cefsimple
  fi
  if [ -f "${B}/out/${CHROMIUM_BUILD_TYPE}/lib/libcef.so" ]; then
    install -Dm 0755 ${B}/out/${CHROMIUM_BUILD_TYPE}/lib/libcef.so ${D}${libdir}/${BPN}/libcef.so
  fi
}
