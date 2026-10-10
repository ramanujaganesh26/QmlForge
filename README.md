<<<<<<< HEAD
# QmlForge
QmlForge is a fast, lightweight Qt 6 C++ desktop application designed for live QML editing and real time playground testing.
=======
# QmlForge - Live QML Editor & Playground

QmlForge is a modern, lightweight Qt 6 C++ desktop application for writing and live-testing QML code.

![QmlForge](resources/icons/app_icon.png)

## Features
- **Instant QML Live Preview**: Write QML and see real-time updates powered by `QQuickWidget`.
- **Feature-Rich Code Editor**: Line numbers, current-line highlight, tab-to-spaces, auto-indent, bracket matching, syntax highlighting, font zooming.
- **Interactive Console**: Detailed QML errors with double-click line jump, real-time `console.log` and `console.warn` stream.
- **Examples Library**: Preset examples covering QtQuick controls, animations, layouts, lists, and states.
- **Auto-Run Mode**: Debounced auto-execution on typing for a smooth live coding experience.
- **Modern Themes**: Dark and Light theme presets with customizable preview canvas background.

## Requirements
- **Qt 6.5+** (Widgets, Qml, Quick, QuickWidgets)
- **CMake 3.16+**
- **C++17 compiler** (MSVC 2019/2022, GCC 10+, or Clang 11+)

## Building from Source

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Packaging (Windows)
Run the PowerShell packaging script to generate a standalone zip release:

```powershell
.\package.ps1
```

## License
[MIT License](LICENSE)
>>>>>>> 06274da (Initial commit of QmlForge v1.0.0)
