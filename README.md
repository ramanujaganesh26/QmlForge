# QmlForge - QML Live Editor & Playground

> A fast, modern Qt 6 C++ desktop application for writing, testing, and live-previewing QML code in real time.

---

## Overview

**QmlForge** provides an interactive live-coding workspace for Qt Quick and QML developers. Write QML components in an intuitive, feature-rich editor and instantly inspect layout, state, and animation changes side-by-side.

---

## Key Features

### ⚡ Live Preview & Real-Time Auto-Run
- **Debounced Auto-Run**: Instant 300 ms live preview refresh as you type.
- **Cache Clearing**: Clears `QQmlEngine` component cache on every run so edits reflect immediately.
- **Smart Window Embedding**: Top-level `ApplicationWindow` and `Window` elements are automatically rendered directly inside the preview pane without external desktop pop-ups.

### 📝 Advanced Code Editor
- **Line Numbers & Highlighting**: Dynamic line number gutter with active line highlighting.
- **Smart Indentation**: Auto-indents brackets `{`, `[`, `(` and converts Tab key to 4 spaces.
- **Bracket Matching**: Visual highlighting for matching parentheses `()`, braces `{}`, and brackets `[]`.
- **QML Syntax Highlighting**: Highlighting for QML types, keywords, numbers, strings, property declarations, and comments.
- **Typography & Font Zooming**: Monospace font support with `Ctrl + Mouse Wheel` zooming and `Ctrl + 0` reset.

### 🐞 Interactive Log & Error Console
- **Direct Line Navigation**: Double-click error messages to jump the editor cursor directly to the error line and column.
- **Live Output Stream**: Intercepts `console.log()` and `console.warn()` statements with precise timestamps.
- **Clear Status Indicators**: Live execution status labels (`Run OK` or error counts).

### 🎨 Themes & Customization
- **Dark & Light Themes**: Catppuccin-inspired dark palette and clean light theme.
- **Canvas Color Picker**: Custom background color selector for testing transparent or custom-colored QML roots.
- **PNG Preview Export**: High-resolution PNG frame-buffer exporter for saving preview states.

### 📚 Built-in QML Examples
Preset examples accessible directly from the toolbar:
1. **Rectangle & Colors**: Mouse interactions, dynamic property bindings, and color randomization.
2. **Animation**: Infinite rotation and pulse scaling easing animations.
3. **ListView**: Styled list delegates with data models and dynamic pricing tags.
4. **Grid Layout**: Responsive 3x3 layout grid with repeater elements.
5. **Button & Signals**: Interactive buttons and state tracking.
6. **Timer & States**: State machine transitions toggled by active timers.

---

## Keyboard Shortcuts

| Action | Shortcut |
| :--- | :--- |
| **Run QML Code** | `Ctrl + R` / `F5` |
| **New Document** | `Ctrl + N` |
| **Open QML File** | `Ctrl + O` |
| **Save File** | `Ctrl + S` |
| **Save As** | `Ctrl + Shift + S` |
| **Reset Font Size** | `Ctrl + 0` |
| **Zoom Font** | `Ctrl + Mouse Wheel` |
| **Undo / Redo** | `Ctrl + Z` / `Ctrl + Y` |

---

## Tech Stack & Requirements

- **Qt Framework**: Qt 6.5+ (Widgets, Qml, Quick, QuickWidgets)
- **Language**: C++17
- **Build System**: CMake 3.16+ (`CMAKE_AUTOMOC`, `CMAKE_AUTORCC`, `CMAKE_AUTOUIC`)
- **Supported OS**: Windows 10 / 11 (x64)

---

## Building from Source

### Prerequisites
Make sure Qt 6 and a valid C++ compiler (MSVC 2019/2022 or GCC 13+) are installed.

```powershell
# Configure CMake build
cmake -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.3\mingw_64" -DCMAKE_BUILD_TYPE=Release

# Build executable
cmake --build build --config Release
```

---

## Packaging & Windows Distribution

Run the automated packaging PowerShell script to generate a standalone release bundle containing all required QtQuick controls and runtime DLLs:

```powershell
powershell -ExecutionPolicy Bypass -File .\package.ps1
```

The output zip archive will be generated at:
```text
dist/QmlForge-v1.0.0-Windows-x64.zip
```

---

## Project Structure

```text
QmlForge/
├── CMakeLists.txt         # Root CMake build configuration
├── main.cpp               # Application entry point & Qt message handler
├── MainWindow.h/.cpp      # Main window UI, toolbars, splitters, settings
├── CodeEditor.h/.cpp      # Line numbers, auto-indent, bracket matching
├── PreviewPane.h/.cpp     # QQuickWidget wrapper & window embedding
├── ConsolePane.h/.cpp     # Error table & console log stream
├── QmlHighlighter.h/.cpp  # QML syntax highlighter
├── resources.qrc          # Qt resource manifest for preset QML examples
├── QmlForge.rc            # Windows binary resource script
├── package.ps1            # Release build & windeployqt packaging script
├── examples/              # Embedded QML examples
│   ├── rectangle.qml
│   ├── animation.qml
│   ├── listview.qml
│   ├── grid.qml
│   ├── button.qml
│   └── timer_state.qml
├── CHANGELOG.md           # Version changelog
├── LICENSE                # MIT License
└── README.md              # Project documentation
```

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Author & Maintainer

- **Developer**: work-with-ram (`ramanujaganeshlk.work@gmail.com`)
- **GitHub Repository**: [ramanujaganesh26/QmlForge](https://github.com/ramanujaganesh26/QmlForge.git)
