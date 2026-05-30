# 外部依赖安装指南

## Ubuntu/Debian

```bash
# FFmpeg
sudo apt install ffmpeg

# OpenCV
sudo apt install libopencv-dev

# COLMAP
sudo apt install colmap

# OpenMVS (从源码编译)
git clone https://github.com/cdcseacave/openMVS.git
cd openMVS && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install

# obj2gltf
npm install -g obj2gltf
```

## macOS

```bash
# Homebrew
brew install ffmpeg opencv colmap

# OpenMVS 和 obj2gltf 与 Linux 相同 (从源码编译 / npm)
```

## 验证安装

```bash
ffmpeg -version
colmap --help
DensifyPointCloud --help
ReconstructMesh --help
InterfaceCOLMAP --help
npx obj2gltf --help
```

## Qt 6.10.3

Qt 已安装于 `/home/myTool/Qt/6.10.3/gcc_64/`。
CMake 配置时需指定: `-DCMAKE_PREFIX_PATH=/home/myTool/Qt/6.10.3/gcc_64`
