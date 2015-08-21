## program below

usage()
{
    cat <<EOF
Usage:
  mbgl-config [OPTION...]

Help Options:
  -h, --help        Show help options

Application Options:
  --prefix          output prefix where mbgl is installed
  --libs            output -L flags
  --includedir      output include directory path
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

    --prefix)
      echo ${CONFIG_MBGL_PREFIX}
      ;;

    --libs)
      echo ${CONFIG_MBGL_PREFIX}/lib/libmbgl.a ${CONFIG_MBGL_LDFLAGS}
      ;;

    --includedir)
      echo ${CONFIG_MBGL_PREFIX}/include
      ;;

    *)
  # push to stderr any invalid options
  echo "unknown option $1" 1>&2;
  ;;
    esac
    shift
done

exit 0
