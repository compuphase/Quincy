# Quincy
An IDE for the Pawn scripting language, see [https://github.com/compuphase/pawn].

The Pawn language is different enough from C that it may confuse the syntax highlighting of a standard code editor (in "C" mode). Quincy is a simple IDE that is made specifically for Pawn.

The current Quincy is a complete rewrite of the original release. The older version of Quincy was a derivative of Al Stevens' Quincy, from his book "Teach Yourself C++" (and also published in Dr. Dobb's Journal). It used MFC and ran only on Microsoft Windows.

This release uses wxWidgets, for portability. Although it may still build with wxWidgets 2.8, it was developed and tested with version 3.0. In fact, version 3.1 is recommended, because the editor control (an essential component of an IDE) got a significant upgrade between version 3.0 and 3.1. ("editor control" refers to scintilla).
