git submodule update --init --recursive

# build libzip
Set-Location .\extern\libzip
mkdir build -ErrorAction SilentlyContinue
Set-Location build

emconfigure cmake ..
emmake make
# emmake make test
# emmake make install