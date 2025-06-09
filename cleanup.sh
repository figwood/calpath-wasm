#!/bin/zsh

echo "===== 开始清理临时文件和缓存 ====="

# 显示清理前的空间使用情况
echo "清理前的空间使用情况:"
du -sh .

# 删除 CMake 缓存和中间文件
echo "===== 删除 CMake 缓存和中间文件 ====="
rm -rf CMakeFiles
rm -rf build/CMakeFiles
rm -f CMakeCache.txt
rm -f build/CMakeCache.txt
rm -f cmake_install.cmake
rm -f build/cmake_install.cmake
rm -f Makefile
rm -f build/Makefile

# 删除编译产生的中间文件和对象文件
echo "===== 删除编译产生的中间文件和对象文件 ====="
find . -name "*.o" -delete
find . -name "*.obj" -delete
find . -name "*.d" -delete

# 清理 Emscripten 缓存 (注意: 这可能会导致下次编译时重新下载一些需要的文件)
echo "===== 清理 Emscripten 缓存 ====="
# 首先设置 Emscripten 环境变量以获取缓存路径
source ./emsdk/emsdk_env.sh
if [[ -d "$EM_CACHE" ]]; then
  echo "找到 Emscripten 缓存目录: $EM_CACHE"
  # 清理缓存，保留必要的系统文件
  find "$EM_CACHE" -name "tmp*" -type d -exec rm -rf {} \; 2>/dev/null || true
  find "$EM_CACHE" -name "emscripten_temp" -type d -exec rm -rf {} \; 2>/dev/null || true
  find "$EM_CACHE" -name "cache" -type d -exec rm -rf {} \; 2>/dev/null || true
else
  echo "未找到 Emscripten 缓存目录"
fi

# 保留最终的构建输出文件，但删除中间文件
echo "===== 保留最终构建输出，删除中间文件 ====="
# 保存当前的 wasm 和 js 文件，以防需要
if [[ -f "satpath.js" && -f "satpath.wasm" ]]; then
  echo "保存当前的 satpath.js 和 satpath.wasm 文件"
  if [[ ! -d "build" ]]; then
    mkdir -p build
  fi
  cp satpath.js build/ 2>/dev/null || true
  cp satpath.wasm build/ 2>/dev/null || true
fi

# 删除 emsdk 下载中的临时文件
echo "===== 清理 emsdk 下载缓存 ====="
rm -rf ./emsdk/downloads/*.tmp
find ./emsdk/downloads -name "*.zip" -delete
find ./emsdk/downloads -name "*.tar.gz" -delete
find ./emsdk/downloads -name "*.tar.xz" -delete

# 显示清理后的空间使用情况
echo "===== 清理完成 ====="
echo "清理后的空间使用情况:"
du -sh .

echo "===== 清理过程完成 ====="
