mkdir out -ErrorAction SilentlyContinue | Out-Null

em++ `
 -x c++ src/main.cpp src/pmx/Pmx.cpp extern/zip/src/zip.c `
 -std=c++11 `
 -g4 `
 -o out/main.js `
 -s WASM=1 `
 -s ASSERTIONS=1 `
 -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' `
 -s ASYNCIFY=1 `
 -s ALLOW_MEMORY_GROWTH=1 `
 -s DISABLE_EXCEPTION_CATCHING=2 `
 -s FORCE_FILESYSTEM=1

Copy-Item .\index.html .\out\index.html
Copy-Item .\server.js .\out\server.js