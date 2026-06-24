#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

DESKTOP_STATIC_BUILD="${DESKTOP_STATIC_BUILD:-$REPO_ROOT/desktop/planer/build/static}"
DEB_OUTPUT_DIR="${DEB_OUTPUT_DIR:-$REPO_ROOT/dist}"
DEB_PACKAGE_DIR="${DEB_PACKAGE_DIR:-$REPO_ROOT/build/package/debian}"
CMAKE_LISTS="${CMAKE_LISTS:-$REPO_ROOT/desktop/planer/CMakeLists.txt}"

PACKAGE_NAME="${PACKAGE_NAME:-planer-desktop}"
PACKAGE_MAINTAINER="${PACKAGE_MAINTAINER:-Planer Team <planer@localhost>}"
BINARY_NAME="${BINARY_NAME:-planer}"
SOURCE_BINARY="${SOURCE_BINARY:-appplaner}"
PLANER_LIB_DIR="${PLANER_LIB_DIR:-/usr/lib/planer}"

STAGING=""

log() {
  printf '==> %s\n' "$*"
}

die() {
  printf 'error: %s\n' "$*" >&2
  exit 1
}

require_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "команда не найдена: $1 (установите dpkg-dev)"
}

require_file() {
  [[ -f "$1" ]] || die "файл не найден: $1"
}

require_dir() {
  [[ -d "$1" ]] || die "каталог не найден: $1"
}

cleanup() {
  [[ -n "$STAGING" ]] && rm -rf "$STAGING"
}

read_package_version() {
  if [[ -n "${PACKAGE_VERSION:-}" ]]; then
    printf '%s\n' "$PACKAGE_VERSION"
    return 0
  fi

  require_file "$CMAKE_LISTS"
  local version
  version="$(grep -E '^project\(planer VERSION ' "$CMAKE_LISTS" \
    | sed -E 's/^project\(planer VERSION ([^ )]+).*/\1/')"
  [[ -n "$version" ]] || die "не удалось прочитать VERSION из $CMAKE_LISTS"
  printf '%s\n' "$version"
}

check_static_qt() {
  local binary="$1"
  if ldd "$binary" 2>/dev/null | grep -qi qt; then
    printf 'warning: desktop линкуется с libQt6*.so (ожидается static Qt)\n' >&2
  else
    log "desktop: libQt6*.so не найдены в ldd"
  fi
}

read_shlib_depends() {
  local staging_dir="$1"
  local binary_path="$2"
  local output

  output="$(cd "$staging_dir" && dpkg-shlibdeps -O --ignore-missing-info "$binary_path")"
  printf '%s\n' "$output" | sed -n 's/^shlibs:Depends=//p'
}

main() {
  local binary="$DESKTOP_STATIC_BUILD/$SOURCE_BINARY"
  local qml_module="$DESKTOP_STATIC_BUILD/planer"
  local desktop_file="$DEB_PACKAGE_DIR/planer-desktop.desktop"
  local wrapper_file="$DEB_PACKAGE_DIR/planer-wrapper.sh"
  local version arch depends deb_path lib_root

  [[ -f "$binary" ]] || die "сначала соберите desktop: make static-desktop ($binary не найден)"
  require_dir "$qml_module"
  require_file "$qml_module/qmldir"
  require_file "$desktop_file"
  require_file "$wrapper_file"
  require_cmd dpkg-deb
  require_cmd dpkg-shlibdeps
  require_cmd dpkg

  check_static_qt "$binary"

  version="$(read_package_version)"
  arch="$(dpkg --print-architecture)"
  STAGING="$(mktemp -d)"
  trap cleanup EXIT

  lib_root="${STAGING}${PLANER_LIB_DIR}"

  log "staging: $STAGING"
  install -d "$STAGING/usr/bin"
  install -d "$STAGING/usr/share/applications"
  install -d "$lib_root/bin"
  install -d "$STAGING/DEBIAN"
  install -d "$STAGING/debian"

  install -m 755 "$binary" "$lib_root/bin/$BINARY_NAME"
  cp -a "$qml_module" "$lib_root/"
  install -m 755 "$wrapper_file" "$STAGING/usr/bin/$BINARY_NAME"
  install -m 644 "$desktop_file" "$STAGING/usr/share/applications/planer-desktop.desktop"

  cat >"$STAGING/debian/control" <<EOF
Source: $PACKAGE_NAME
Package: $PACKAGE_NAME
Version: $version
Architecture: $arch
Maintainer: $PACKAGE_MAINTAINER
Description: Planer desktop client
EOF

  depends="$(read_shlib_depends "$STAGING" "${PLANER_LIB_DIR#/}/bin/$BINARY_NAME")"
  [[ -n "$depends" ]] || die "dpkg-shlibdeps не вернул Depends"

  rm -rf "$STAGING/debian"

  cat >"$STAGING/DEBIAN/control" <<EOF
Package: $PACKAGE_NAME
Version: $version
Section: utils
Priority: optional
Architecture: $arch
Maintainer: $PACKAGE_MAINTAINER
Depends: $depends
Description: Planer desktop client
 Qt Quick desktop client for planer (static Qt build).
EOF

  mkdir -p "$DEB_OUTPUT_DIR"
  deb_path="$DEB_OUTPUT_DIR/${PACKAGE_NAME}_${version}_${arch}.deb"

  log "Depends: $depends"
  log "сборка $deb_path"
  dpkg-deb --root-owner-group --build "$STAGING" "$deb_path"

  log "готово: $deb_path"
}

main "$@"
