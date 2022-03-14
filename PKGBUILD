pkgname=pulse-combine-sinks
pkgver=0.0.0
pkgrel=1
arch=('x86_64')
depends=('pulseaudio')
pkgdesc="Extremely simple utility which finds pulse sinks by human-readable names and combines them."
url="https://github.com/andrewerf/pulse-combine-sinks"
source=('git+https://github.com/andrewerf/pulse-combine-sinks.git')
md5sums=('SKIP')
license=('MIT')

build() {
    cd $pkgname
    mkdir -p build
    cd build
    cmake ..
    make
}

package() {
    cd $pkgname/build
    DESTDIR="$pkgdir/" cmake --install .
}
