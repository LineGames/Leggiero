=======
License
=======

Leggiero
========

Leggiero 엔진은 MIT 라이선스 하에서 제공됩니다.

.. code-block:: none
    
    Copyright 2021-2022 (c) Kim, Young Soo and LINE GAMES Corporation.
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights 
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
    copies of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in 
    all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
    IN THE SOFTWARE.

해당 이용 허가는 엔진, 툴, 본 문서를 포함한 엔진 저장소의 저작물에 적용됩니다.


Libraries
=========

Leggiero에서는 다양한 라이브러리를 이용하고 있습니다.
개별 라이브러리의 이용 여부 및 이용 조건은 `Library Usage <92_library_usage.html>`_ 문서에 정리되어 있습니다.

라이브러리의 바이너리 및 코드는 라이브러리 저장소를 통해 제공되고 있으며, 각각의 이용 조건을 담은 문서(COPYING, LICENSE 파일 등)를 함께 첨부하였습니다.

OpenAL soft 등의 LGPL과 같이 전염성 있는 라이선스가 적용된 라이브러리는 동적 연결을 통해 이용하고 있으며, 해당 라이선스들의 전염성 조항은 본 엔진에 적용되지 않습니다.


LegToy
======

LegToy는 Leggiero 엔진의 동작 예시를 위해 따로 작성된 별도의 프로젝트이며, Leggiero 엔진과는 다른 라이선스가 적용됩니다.

Codes
-----

LegToy의 코드에는 Leggiero 엔진과 같은 MIT 라이선스가 적용됩니다.
코드 저작물의 예시에는 아래와 같은 내용이 포함됩니다:

- .h, .cpp, .mm, .java 등의 확장자를 가지는 C++, Java, Objective-C 등의 프로그래밍 언어로 작성된 소스 코드들
- .desc.xml 의 형식을 가진, UI Description 정의

Resources
---------

LegToy는 Leggiero 엔진의 동작 예시 제공을 위해 게임 <Protocol:hyperspace Diver> 등에서 유래한, 저작권 있는 리소스를 사용합니다.
해당 리소스들은 사유 소프트웨어로, Leggiero 엔진의 시험을 위한 LegToy 프로젝트에서의 활용을 제외한 경우에서의 별도 허가가 없는 복제와 사용이 금지됩니다.

LegToy 저장소 내 Bundle 폴더의 모든 파일들의 내용물은, 별도 언급이 없는 한 사유 소프트웨어입니다.

  - 앞 절에서 언급한 UI Description 정의 파일은 MIT 라이선스가 적용되는 코드로 간주합니다.
    
  - Bundle/Base/Showcase/Typeset/Article.dat에 저장된, 에드가 앨런 포우의 소설 "The Cask of Amontillado"는 Public Domain에 속한 저작물입니다.
    
  - Bundle/Base/UI/Font 폴더 내의 글꼴 파일들은 개별 저작권을 따릅니다(OFL, Apache). 동 폴더 내 license_info.txt 파일 및 본 문서 내에 글꼴 별 적용 라이선스를 표기하였습니다.
    
