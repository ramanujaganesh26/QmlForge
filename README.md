# QmlForge - QML Live Editor & Playground

QmlForge is a high-performance Qt 6 C++ desktop application designed for real-time QML code editing, testing, and live preview rendering.

---

## Overview

QmlForge provides an integrated live-coding playground for Qt Quick and QML developers. Developers can write QML components in a feature-rich editor while inspecting layout, state, and animation updates side-by-side in real time.

---

## Features

### Live Preview & Auto-Run
- **Real-Time Live Editing**: Automated 300 ms debounced preview refresh as code is updated.
- **Component Cache Invalidation**: Automatic clearing of the `QQmlEngine` component cache on each run ensuring edits take effect instantly.
- **Embedded Window Rendering**: Top-level `ApplicationWindow` and `Window` root elements are automatically embedded directly into the live preview canvas without spawning external desktop windows.

### Code Editor Engine
- **Line Numbers & Line Highlighting**: Dynamic line number gutter with active line highlighting.
- **Smart Indentation**: Auto-indentation for braces, brackets, and parentheses (`{`, `[`, `(`) and tab-to-4-spaces conversion.
- **Bracket Matching**: Visual bracket pair matching for `()`, `{}`, and `[]`.
- **QML Syntax Highlighting**: Highlighting for QML types, language keywords, numbers, strings, property declarations, and single/multiline comments.
- **Typography & Font Zooming**: Monospace font rendering with `Ctrl + Mouse Wheel` zooming and `Ctrl + 0` font reset.

### Error & Output Console
- **Interactive Line Navigation**: Double-click error messages to move the editor cursor directly to the corresponding line and column.
- **Console Log Stream**: Intercepts `console.log()` and `console.warn()` statements with execution timestamps.
- **Status Reporting**: Live execution status indicators showing build state and error counts.

### Theme & Canvas Control
- **Dark & Light Themes**: Preset themes tailored for low-light and bright working environments.
- **Canvas Color Selector**: Customizable preview background color for testing transparent or colored QML components.
- **PNG Preview Export**: High-resolution image exporter for capturing current preview framebuffers.

### Included QML Examples
Built-in preset examples accessible via the toolbar:
1. **Rectangle & Colors**: Mouse event handling, color randomization, and dynamic property bindings.
2. **Animation**: Infinite rotation and scale easing animations.
3. **ListView**: Custom list item delegates with data models and price tags.
4. **Grid Layout**: Responsive 3x3 layout grid with repeater elements.
5. **Button & Signals**: Interactive buttons and signal handling.
6. **Timer & States**: State transitions controlled by active timer events.

---

## Keyboard Shortcuts

| Action | Shortcut |
| :--- | :--- |
| **Run QML Code** | `Ctrl + R` / `F5` |
| **New File** | `Ctrl + N` |
| **Open File** | `Ctrl + O` |
| **Save File** | `Ctrl + S` |
| **Save As** | `Ctrl + Shift + S` |
| **Reset Font Size** | `Ctrl + 0` |
| **Zoom Font Size** | `Ctrl + Mouse Wheel` |
| **Undo / Redo** | `Ctrl + Z` / `Ctrl + Y` |

---

## System Requirements

- **Qt Framework**: Qt 6.5 or newer (Widgets, Qml, Quick, QuickWidgets)
- **C++ Compiler**: C++17 compliant compiler (MSVC 2019/2022, GCC 10+, or Clang 11+)
- **Build Tool**: CMake 3.16 or newer (`CMAKE_AUTOMOC` enabled)
- **Operating System**: Windows 10 / 11 (64-bit)

---

## Build Instructions

```powershell
# Configure CMake build
cmake -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.3\mingw_64" -DCMAKE_BUILD_TYPE=Release

# Build application binary
cmake --build build --config Release
```

---

## Windows Deployment & Packaging

To generate a standalone distribution package including all required Qt Quick runtime modules and DLLs:

```powershell
powershell -ExecutionPolicy Bypass -File .\package.ps1
```

The resulting zip package is saved to:
```text
dist/QmlForge-v1.0.0-Windows-x64.zip
```

---

## Directory Structure

```text
QmlForge/
├── CMakeLists.txt         # Root CMake build configuration
├── main.cpp               # Application entry point & Qt message handler
├── MainWindow.h/.cpp      # Main window UI, splitters, toolbars, settings
├── CodeEditor.h/.cpp      # Line numbers, auto-indent, bracket matching
├── PreviewPane.h/.cpp     # QQuickWidget wrapper & window embedding
├── ConsolePane.h/.cpp     # Error table & console log stream
├── QmlHighlighter.h/.cpp  # QML syntax highlighter
├── resources.qrc          # Qt resource manifest for preset QML examples
├── QmlForge.rc            # Windows resource file
├── package.ps1            # Release build & deployment script
├── examples/              # Embedded QML examples
│   ├── rectangle.qml
│   ├── animation.qml
│   ├── listview.qml
│   ├── grid.qml
│   ├── button.qml
│   └── timer_state.qml
├── CHANGELOG.md           # Release history
├── LICENSE                # MIT License
└── README.md              # Project documentation
```

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Maintainer Information

- **Developer**: work-with-ram (`ramanujaganeshlk.work@gmail.com`)
- **Repository**: [https://github.com/ramanujaganesh26/QmlForge.git](https://github.com/ramanujaganesh26/QmlForge.git)
