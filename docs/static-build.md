# Статическая сборка зависимостей и desktop-клиента planer

Сборка Qt 6.11.1 и Boost 1.91.0 из `/mnt/dev/sources` и линковка `desktop/planer`
со статическим Qt.

Desktop-клиент использует **Qt Quick**, **QuickControls2** и **Network**.

## Каталоги

```
/mnt/dev/
  sources/                          # исходники (архивы уже распакованы)
    qt-everywhere-src-6.11.1/
    boost_1_91_0/
  build/                            # временные каталоги сборки
    qt-6.11.1-static/
  libs/                             # установленные библиотеки
    qt/6.11.1/
    boost/1.91.0/
```

## Переменные окружения

```bash
export DEV_ROOT=/mnt/dev
export SOURCES=$DEV_ROOT/sources
export QT_SRC=$SOURCES/qt-everywhere-src-6.11.1
export BOOST_SRC=$SOURCES/boost_1_91_0

export LIBS_ROOT=$DEV_ROOT/libs
export QT_PREFIX=$LIBS_ROOT/qt/6.11.1
export BOOST_ROOT=$LIBS_ROOT/boost/1.91.0

export PATH=$QT_PREFIX/bin:$PATH
```

```bash
mkdir -p "$DEV_ROOT/build" "$LIBS_ROOT/qt" "$LIBS_ROOT/boost"
```

## Системные зависимости (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y \
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
```

Список xcb-зависимостей: [Qt for Linux - Requirements](https://doc.qt.io/qt-6/linux-requirements.html).

## Сборка Qt 6.11.1 (static)

```bash
mkdir -p "$DEV_ROOT/build/qt-6.11.1-static"
cd "$DEV_ROOT/build/qt-6.11.1-static"

$QT_SRC/configure \
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
    -skip qt3d,qt5compat,qtactiveqt,qtcanvaspainter,qtcharts,qtcoap,qtconnectivity,qtdatavis3d,qtgraphs,qtgrpc,qthttpserver,qtimageformats,qtlanguageserver,qtlocation,qtlottie,qtmqtt,qtmultimedia,qtnetworkauth,qtopcua,qtopenapi,qtpositioning,qtquick3d,qtquick3dphysics,qtquickeffectmaker,qtquicktimeline,qtremoteobjects,qtscxml,qtsensors,qtserialbus,qtserialport,qtspeech,qttasktree,qttools,qttranslations,qtvirtualkeyboard,qtwayland,qtwebchannel,qtwebengine,qtwebsockets,qtwebview,qtdoc

cmake --build . --parallel "$(nproc)"
cmake --install .
```

Для planer нужны модули: `qtbase`, `qtdeclarative`, `qtshadertools`, `qtsvg`.

Проверка:

```bash
qmake6 -query QT_INSTALL_PREFIX
file "$QT_PREFIX/lib/libQt6Core.a"
```

## Сборка Boost 1.91.0 (static)

```bash
cd "$BOOST_SRC"

./bootstrap.sh --prefix="$BOOST_ROOT"

./b2 -j"$(nproc)" \
    link=static \
    runtime-link=shared \
    variant=release \
    threading=multi \
    --with-filesystem \
    --with-thread \
    --with-program_options \
    --with-regex \
    install
```

Header-only библиотеки можно использовать без сборки: `export BOOST_ROOT=$BOOST_SRC`.

## Статическая сборка appplaner

Через Makefile (см. `make help`):

```bash
make static-deps          # один раз: системные пакеты
make static-build JOBS=8  # Qt + Boost + desktop
make static-verify
make static-desktop-run
```

Или напрямую: `./scripts/static-build.sh all`

Проверка линковки:

```bash
ldd desktop/planer/build/static/appplaner | grep -i qt
# libQt6*.so быть не должно
```

## Упаковка .deb

Скрипт [`scripts/static-deb.sh`](../scripts/static-deb.sh) собирает `.deb` из статического бинарника
и QML-модуля `planer/` (нужен для `loadFromModule` при запуске вне каталога сборки).
Бинарник ставится в `/usr/lib/planer/bin/planer`, QML — в `/usr/lib/planer/planer/`,
в `/usr/bin/planer` — wrapper с `QML2_IMPORT_PATH`.

Шаблоны упаковки: [`build/package/debian/`](../build/package/debian/).
Готовый артефакт: `dist/planer-desktop_<version>_<arch>.deb`.

```bash
make static-desktop-deb
sudo dpkg -i dist/planer-desktop_0.1_amd64.deb
# при missing deps: sudo apt-get install -f
planer
```

Собирайте `.deb` на целевом дистрибутиве — версии в `Depends` привязаны к системе сборки.

## Ссылки

- [Qt for Linux - Building from Source](https://doc.qt.io/qt-6/linux-building.html)
- [Qt Configure Options](https://doc.qt.io/qt-6/configure-options.html)
- [Boost Getting Started on Unix](https://www.boost.org/doc/libs/release/more/getting_started/unix-variants.html)
