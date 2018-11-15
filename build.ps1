mkdir out -ErrorAction SilentlyContinue | Out-Null

em++ src/main.cpp src/pmx/Helper.cpp `
 -std=c++11 `
 -o out/main.js `
 -s WASM=1 `
 -s ASSERTIONS=1 `
 -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' `
 -s FORCE_FILESYSTEM=1

Copy-Item .\minimal.html .\out\minimal.html
Copy-Item .\server.js .\out\server.js