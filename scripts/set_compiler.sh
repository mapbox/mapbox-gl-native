#!/usr/bin/env bash

if [ ! -z "${_CXX}" ]; then export CXX="${_CXX}" ; fi
if [ ! -z "${_CC}" ]; then export CC="${_CC}" ; fi

if [ "${CCACHE:-0}" -ge 1 ]; then
    export CXX="ccache ${CXX}"
    export CC="ccache ${CC}"

    # ccache splits up the compile steps, so we end up with unused arguments in some steps.
    # Clang also thinks that ccache isn't interactive, so we explicitly need to enable color.
    if [ $(echo | ${CXX} -dM -E - | grep -c "#define __clang__ 1") -ge 1 ]; then
        export CXX="${CXX} -Qunused-arguments -fcolor-diagnostics"
        export CC="${CC} -Qunused-arguments -fcolor-diagnostics"
    else
        # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60304
        # GCC normally throws this error which is in GTest, but *only* when compilation and
        # preprocessing aren't combined in one step. However, when using ccache with GCC, we are
        # running them in separate steps, so this warning/error is shown.
        export CXX="${CXX} -Wno-conversion-null"
        export CC="${CC} -Wno-conversion-null"
    fi
fi

echo "export CXX=\"${CXX}\""
echo "export CC=\"${CC}\""
${CXX} --version
