#!/bin/bash
echo "compile commenced at:" $(date)
args="${@:2}"

if [ $# -gt 0 ]; then
  emcc -std=c++1z $1.cpp StructureUtils.cpp FastNoise.cpp TextureUtils.cpp WorldUtils.cpp Game.cpp -s WASM=1 -s USE_SDL=2 -O3 -o $1.js \
  -s EXPORTED_FUNCTIONS="['_main', '_get_info', '_ecount']" \
  -s EXTRA_EXPORTED_RUNTIME_METHODS=["cwrap"] \
  -s USE_SDL_IMAGE=2\
  -s ALLOW_MEMORY_GROWTH=1 --use-preload-plugins\
  -s SDL2_IMAGE_FORMATS='["bmp","png"]'\
  -lSDL \
  --preload-file ../Resources \
  -s ASSERTIONS=1

  if [ $? -eq 0 ]; then
      echo OK
      sudo git add $args
      sudo git commit -m "building"
      sudo git push

      if [ $? -eq 1 ]; then
        echo FAIL
      fi      

  else
      echo FAIL
  fi
else
  echo "Invalid number of arguments passed"
fi
