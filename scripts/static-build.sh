#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

DEV_ROOT="${DEV_ROOT:-/mnt/dev}"
SOURCES="${SOURCES:-$DEV_ROOT/sources}"
LIBS_ROOT="${LIBS_ROOT:-$DEV_ROOT/libs}"
BUILD_ROOT="${BUILD_ROOT:-$DEV_ROOT/build}"

QT_VERSION="${QT_VERSION:-6.11.1}"
BOOST_VERSION="${BOOST_VERSION:-1.91.0}"

QT_SRC="${QT_SRC:-$SOURCES/qt-everywhere-src-$QT_VERSION}"
BOOST_SRC="${BOOST_SRC:-$SOURCES/boost_${BOOST_VERSION//./_}}"

QT_PREFIX="${QT_PREFIX:-$LIBS_ROOT/qt/$QT_VERSION}"
BOOST_ROOT="${BOOST_ROOT:-$LIBS_ROOT/boost/$BOOST_VERSION}"

QT_BUILD_DIR="${QT_BUILD_DIR:-$BUILD_ROOT/qt-$QT_VERSION-static}"
DESKTOP_STATIC_BUILD="${DESKTOP_STATIC_BUILD:-$REPO_ROOT/desktop/planer/build/static}"

JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"
FORCE="${FORCE:-0}"

QT_SKIP="${QT_SKIP:-qt3d,qt5compat,qtactiveqt,qtcanvaspainter,qtcharts,qtcoap,qtconnectivity,qtdatavis3d,qtgraphs,qtgrpc,qthttpserver,qtimageformats,qtlanguageserver,qtlocation,qtlottie,qtmqtt,qtmultimedia,qtnetworkauth,qtopcua,qtopenapi,qtpositioning,qtquick3d,qtquick3dphysics,qtquickeffectmaker,qtquicktimeline,qtremoteobjects,qtscxml,qtsensors,qtserialbus,qtserialport,qtspeech,qttasktree,qttools,qttranslations,qtvirtualkeyboard,qtwayland,qtwebchannel,qtwebengine,qtwebsockets,qtwebview,qtdoc}"

log() {
  printf '==> %s\n' "$*"
}

die() {
  printf 'error: %s\n' "$*" >&2
  exit 1
}

require_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "команда не найдена: $1"
}

require_dir() {
  [[ -d "$1" ]] || die "каталог не найден: $1"
}

require_file() {
  [[ -f "$1" ]] || die "файл не найден: $1"
}

usage() {
  cat <<EOF
Usage: $(basename "$0") <command>

Commands:
  deps      установить системные пакеты (sudo apt)
  dirs      создать каталоги build/libs
  qt        собрать и установить статический Qt
  boost     собрать и установить статический Boost
  desktop   собрать desktop/planer со статическим Qt
  all       qt + boost + desktop
  verify    проверить установленные библиотеки

Переменные окружения:
  DEV_ROOT, SOURCES, LIBS_ROOT, BUILD_ROOT
  QT_PREFIX, BOOST_ROOT, QT_BUILD_DIR, DESKTOP_STATIC_BUILD
  JOBS, FORCE=1 (пересборка без пропуска)

Пример:
  make static-deps
  make static-build JOBS=8
EOF
}

cmd_dirs() {
  log "каталоги: $BUILD_ROOT, $LIBS_ROOT"
  mkdir -p "$BUILD_ROOT" "$LIBS_ROOT/qt" "$LIBS_ROOT/boost"
}

cmd_deps() {
  require_cmd sudo
  require_cmd apt-get
  log "установка системных пакетов"
  sudo apt-get update
  sudo apt-get install -y \
    build-essential cmake ninja-build python3 perl pkg-config \
    bison flex gperf \
    libssl-dev libicu-dev libglib2.0-dev libdbus-1-dev \
    libdouble-conversion-dev libb2-dev libpcre2-dev \
    libfontconfig1-dev libfreetype-dev \
    libx11-dev libx11-xcb-dev libxcb1-dev libxcb-cursor-dev \
    libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev \
    libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev \
    libxcb-shape0-dev libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev \
    libxcb-xfixes0-dev libxcb-xkb-dev libxext-dev libxfixes-dev libxi-dev \
    libxkbcommon-dev libxkbcommon-x11-dev libxrender-dev \
    libgl1-mesa-dev libegl1-mesa-dev libdrm-dev libgbm-dev
}

qt_installed() {
  [[ -f "$QT_PREFIX/lib/libQt6Core.a" ]]
}

cmd_qt() {
  require_dir "$QT_SRC"
  require_file "$QT_SRC/configure"
  require_cmd cmake

  cmd_dirs

  if qt_installed && [[ "$FORCE" != "1" ]]; then
    log "Qt уже установлен в $QT_PREFIX (FORCE=1 для пересборки)"
    return 0
  fi

  mkdir -p "$QT_BUILD_DIR"
  cd "$QT_BUILD_DIR"

  if [[ ! -f "$QT_BUILD_DIR/build.ninja" && ! -f "$QT_BUILD_DIR/Makefile" ]] || [[ "$FORCE" == "1" ]]; then
    log "configure Qt -> $QT_PREFIX"
    "$QT_SRC/configure" \
      -static \
      -release \
      -prefix "$QT_PREFIX" \
      -opensource \
      -confirm-license \
      -no-widgets \
      -openssl-linked \
      -dbus-linked \
      -fontconfig \
      -xcb-xlib \
      -nomake examples \
      -nomake tests \
      -skip "$QT_SKIP"
  else
    log "configure пропущен (уже выполнен в $QT_BUILD_DIR)"
  fi

  log "сборка Qt ($JOBS jobs)"
  cmake --build . --parallel "$JOBS"

  log "install Qt -> $QT_PREFIX"
  cmake --install .

  require_file "$QT_PREFIX/lib/libQt6Core.a"
  log "Qt установлен: $QT_PREFIX"
}

boost_installed() {
  [[ -f "$BOOST_ROOT/lib/libboost_filesystem.a" ]]
}

cmd_boost() {
  require_dir "$BOOST_SRC"
  require_file "$BOOST_SRC/bootstrap.sh"

  cmd_dirs

  if boost_installed && [[ "$FORCE" != "1" ]]; then
    log "Boost уже установлен в $BOOST_ROOT (FORCE=1 для пересборки)"
    return 0
  fi

  cd "$BOOST_SRC"

  if [[ ! -x "$BOOST_SRC/b2" ]] || [[ "$FORCE" == "1" ]]; then
    log "bootstrap Boost -> $BOOST_ROOT"
    ./bootstrap.sh --prefix="$BOOST_ROOT"
  fi

  log "сборка Boost ($JOBS jobs)"
  ./b2 -j"$JOBS" \
    link=static \
    runtime-link=shared \
    variant=release \
    threading=multi \
    --with-filesystem \
    --with-thread \
    --with-program_options \
    --with-regex \
    install

  require_file "$BOOST_ROOT/lib/libboost_filesystem.a"
  log "Boost установлен: $BOOST_ROOT"
}

cmd_desktop() {
  qt_installed || die "Qt не установлен: $QT_PREFIX (make static-qt)"

  require_cmd cmake
  export PATH="$QT_PREFIX/bin:$PATH"

  log "configure desktop -> $DESKTOP_STATIC_BUILD"
  cmake -S "$REPO_ROOT/desktop/planer" -B "$DESKTOP_STATIC_BUILD" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$QT_PREFIX" \
    -DQt6_DIR="$QT_PREFIX/lib/cmake/Qt6" \
    -DCMAKE_FIND_LIBRARY_SUFFIXES=".a"

  log "сборка desktop ($JOBS jobs)"
  cmake --build "$DESKTOP_STATIC_BUILD" --parallel "$JOBS"

  require_file "$DESKTOP_STATIC_BUILD/appplaner"
  log "desktop собран: $DESKTOP_STATIC_BUILD/appplaner"
}

cmd_verify() {
  log "DEV_ROOT=$DEV_ROOT"
  log "QT_PREFIX=$QT_PREFIX"
  log "BOOST_ROOT=$BOOST_ROOT"

  if qt_installed; then
    log "Qt OK: $QT_PREFIX/lib/libQt6Core.a"
    PATH="$QT_PREFIX/bin:$PATH" qmake6 -query QT_INSTALL_PREFIX
  else
    printf 'Qt: не установлен\n' >&2
  fi

  if boost_installed; then
    log "Boost OK: $BOOST_ROOT/lib/libboost_filesystem.a"
  else
    printf 'Boost: не установлен\n' >&2
  fi

  if [[ -x "$DESKTOP_STATIC_BUILD/appplaner" ]]; then
    log "desktop OK: $DESKTOP_STATIC_BUILD/appplaner"
    if ldd "$DESKTOP_STATIC_BUILD/appplaner" 2>/dev/null | grep -qi qt; then
      printf 'warning: desktop линкуется с libQt6*.so\n' >&2
    else
      log "desktop: libQt6*.so не найдены в ldd"
    fi
  else
    printf 'desktop: не собран\n' >&2
  fi
}

cmd_all() {
  cmd_qt
  cmd_boost
  cmd_desktop
}

main() {
  [[ $# -ge 1 ]] || {
    usage
    exit 1
  }

  case "$1" in
    deps) cmd_deps ;;
    dirs) cmd_dirs ;;
    qt) cmd_qt ;;
    boost) cmd_boost ;;
    desktop) cmd_desktop ;;
    all) cmd_all ;;
    verify) cmd_verify ;;
    -h | --help | help) usage ;;
    *) die "неизвестная команда: $1 (см. --help)" ;;
  esac
}

main "$@"
