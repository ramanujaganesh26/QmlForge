# Changelog

All notable changes to **QmlForge** will be documented in this file.

## [1.0.0] - 2026-10-10

### Added
- **Live QML Editor & Runner**: Real-time QML editing with instant preview using `QQuickWidget`.
- **Code Editor Enhancements**: Monospace typography, line number gutter, active line highlighting, bracket matching, auto-indentation, and 4-space tab handling.
- **Error & Log Console**: Live capture of Qt console outputs (`console.log`, `console.warn`) and QML parse errors with line navigation.
- **Syntax Highlighting**: Comprehensive QML syntax highlighter (`QmlHighlighter`) with dark and light theme palette support.
- **File Operations**: Open, Save, Save As, Drag and Drop support, relative file import paths.
- **Live Reload & Auto-Run**: Debounced 600ms auto-run timer for live preview update without manual trigger.
- **QML Examples Preset**: Built-in QML examples (Rectangle & Colors, Animations, ListView, Grid Layout, Button Signals, Timer & State).
- **Theming & Zoom**: Light/Dark theme switching, font zooming with Ctrl+Wheel / Ctrl+0, preview canvas background color picker.
- **Deployment & Extras**: Standalone Windows deployment support with `windeployqt`, `qmllint` validation, export preview to PNG, and tabbed file editor interface.
