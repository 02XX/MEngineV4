# MEngine

一个基于Vulkan的高性能渲染引擎

## 核心功能

## 快速开始

### 要求

- ​**编译器**: GCC 11+/Clang 12+/MSVC 2022+（必须支持 C++20）
- ​**Vulkan**: SDK 1.3+ 并配置环境变量
- ​**包管理**: 推荐使用 [vcpkg](https://vcpkg.io/)

### 编译

```shell
git clone https://github.com/02XX/MEngineV4.git
cd MEngine
cmake . --list-presets  
cmake . -B build --preset=Release_x64-windows_clang
```

## 项目结构

```shell
MEngine
├───Core
├───Function
├───Platform
├───Resource
├───Test
└───Tool
```

## 如何贡献

1. 提交 [Issue](https://github.com/02XX/MEngineV4/issues) 说明问题或建议
2. Fork 项目并创建功能分支
3. 确保代码符合 clang-format 规范
4. 提交 Pull Request 并关联 Issue

## 许可证

GNU General Public License v3. - 详见[LICENSE](LICENSE)文件
