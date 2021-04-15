===========
Font Module
===========

Font 모듈은 글꼴을 처리하여 글씨의 그림을 얻어내고, 글월을 조판하기 위해 쓰여집니다.


Module Common Information
=========================

**Project:** *Modules/Font*

**Namespace:** ``Leggiero::Font``

Module Interfaces
-----------------
- `Font Module Interface`_

Components
----------
- `Glyph Manager Component`_


Font Module Interface
=====================
Font 모듈에 대한 인터페이스를 제공합니다.

**Type:** ``Leggiero::Font::FontModule``

**Module Id:** ``EngineModuleIdType::kFont`` (220)

Dependencies
------------
(없음)

Overview
--------
Font 모듈 안에서 글씨의 그리기에 쓰는 FreeType2 모음함수의 안쪽 상태를 초기화하거나 정리합니다.

.. Note:: Leggiero에서는 글씨의 그리기에 FreeType을 쓰고 있지만, 별도의 유니코드용 배치 모음함수를 쓰고 있지 않습니다. GSUB/GPOS 정보를 이용하는 อักษรไทย 같은 글씨는 바르게 그려지지 않을 수 있습니다.

Load a Font
-----------
Font 모듈이 초기화 된 후, 글꼴을 불러올 수 있습니다.

*FreeType/FreeTypeFontFace.h* 에 있는 함수들로 FreeType 모음함수를 써서 글꼴을 불러와 쓸 수 있습니다.

Typesetting
-----------
FreeType2 모음함수를 이용해 글월의 간단한 조판도 할 수 있습니다.

*Common/Typeset.h* 에서 조판을 하기 위한 함수를 보실 수 있습니다.


Glyph Manager Component
=======================
글씨 그림을 그리고, 갖고 있을 수 있는 엔진 컴퍼넌트입니다.

**Type:** ``Leggiero::Font::GlyphManagerComponent``

**Component Id:** ``EngineComponentIdType::kGlyphManager`` (221)

Dependencies
------------
- `Graphic Resource Manager Component <43_mod_graphics.html>`_

Overview
--------
주어진 글꼴에 맞춰 원하는 글씨의 그림을 그려 OpenGL ES의 질감으로써 쓸 수 있게 해 줍니다.
매번 다시 그리지 않아도 되도록, 한 번 그린 글씨의 그림은 가지고 있습니다.
