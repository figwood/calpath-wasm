#!/bin/zsh

# 设置 Emscripten 环境
echo "===== 设置 Emscripten 环境 ====="
source ./emsdk/emsdk_env.sh

# 清理之前的构建
echo "===== 清理之前的构建 ====="
rm -rf build/CMakeFiles
rm -f build/satpath.js build/satpath.wasm

# 使用 emcmake 配置项目
echo "===== 用 emcmake 配置项目 ====="
emcmake cmake -DCMAKE_BUILD_TYPE=Release .

# 编译 WebAssembly 代码
echo "===== 使用 emmake 编译 WebAssembly 代码 ====="
emmake make

# 复制构建文件到 build 目录
echo "===== 复制构建文件到 build 目录 ====="
cp satpath.js satpath.wasm build/

# 杀死所有正在运行的 Python HTTP 服务器
echo "===== 杀死所有正在运行的 Python HTTP 服务器 ====="
pkill -f "python.*http.server" || echo "没有找到运行中的 HTTP 服务器"

# 启动 HTTP 服务器
echo "===== 启动 HTTP 服务器 ====="
echo "访问地址: http://localhost:8082"
python3 -m http.server 8082
