
## program below

usage()
{
    cat <<EOF
Usage: mbgl-config [OPTION]

Known values for OPTION are:

  -h --help         display this help and exit
  -v --version      version information
  --git-revision    git hash from "git rev-list --max-count=1 HEAD"
  --git-describe    git decribe output
  --defines         pre-processor defines for mbgl build
  --prefix          mbgl prefix [default $CONFIG_PREFIX]
  --lib-name        mbgl library name
  --libs            library linking information
  --dep-libs        library linking information for mbgl dependencies
  --ldflags         library paths (-L) information
  --includes        include paths (-I) for mbgl headers
  --dep-includes    include paths (-I) for mbgl dependencies
  --cxxflags        c++ compiler flags and pre-processor defines
  --cflags          all include paths, compiler flags, and pre-processor defines (for back-compatibility)
  --cxx             c++ compiler used to build mbgl
  --all-flags       all compile and link flags
EOF

    exit $1
}

echoerr() { echo "$@" 1>&2; }

if test $# -eq 0; then
    usage 1
fi

while test $# -gt 0; do
    case "$1" in
    esac

    case "$1" in

    --help)
      usage 0
      ;;

    -h)
      usage 0
      ;;

    -v)
      echo ${CONFIG_MBGL_VERSION_STRING}
      ;;

    --version)
      echo ${CONFIG_MBGL_VERSION_STRING}
      ;;

    --version-number)
      echo ${CONFIG_MBGL_VERSION}
      ;;

    --git-revision)
      echo ${CONFIG_MBGL_GIT_REVISION}
      ;;

    --git-describe)
      echo ${CONFIG_MBGL_GIT_DESCRIBE}
      ;;

    --defines)
      echo ${CONFIG_MBGL_DEFINES}
      ;;

    --prefix)
      echo ${CONFIG_MBGL_PREFIX}
      ;;

    --lib-name)
      echo ${CONFIG_MBGL_LIBNAME}
      ;;

    --libs)
      echo -L${CONFIG_MBGL_LIBPATH} -l${CONFIG_MBGL_LIBNAME}
      ;;

    --dep-libs)
      echo ${CONFIG_MBGL_DEP_LIBS}
      ;;

    --ldflags)
      echo ${CONFIG_MBGL_LDFLAGS}
      ;;

    --includes)
      echo -I${CONFIG_MBGL_INCLUDES}
      ;;

    --dep-includes)
      echo ${CONFIG_MBGL_DEP_INCLUDES}
      ;;

    --cxxflags)
      echo ${CONFIG_MBGL_CXXFLAGS}
      ;;

    --cflags)
      echo -I${CONFIG_MBGL_INCLUDE} ${CONFIG_MBGL_DEP_INCLUDES} ${CONFIG_MBGL_DEFINES} ${CONFIG_MBGL_CXXFLAGS}
      ;;

    --cxx)
      echo ${CONFIG_MBGL_CXX}
      ;;

    --all-flags)
      echo -I${CONFIG_MBGL_INCLUDES} ${CONFIG_MBGL_DEP_INCLUDES} ${CONFIG_MBGL_DEFINES} ${CONFIG_MBGL_CXXFLAGS} -L${CONFIG_MBGL_LIBPATH} -l${CONFIG_MBGL_LIBNAME} ${CONFIG_MBGL_LDFLAGS} ${CONFIG_MBGL_DEP_LIBS}
      ;;

    *)
  # push to stderr any invalid options
  echo "unknown option $1" 1>&2;
  ;;
    esac
    shift
done

exit 0
