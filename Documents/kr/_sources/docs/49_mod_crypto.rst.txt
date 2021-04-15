===================
Cryptography Module
===================

Cryptography 모듈은 각종 암호학적 도구 사용을 위한 편의기능을 제공합니다.


Module Common Information
=========================

**Project:** *Modules/Crypto*

**Namespace:** ``Leggiero::Crypto``

Module Interfaces
-----------------
- `Cryptography Module Interface`_

Components
----------
(없음)


Cryptography Module Interface
=============================
암호 모듈에 대한 인터페이스를 제공합니다.

**Type:** ``Leggiero::Crypto::CryptoModule``

**Module Id:** ``EngineModuleIdType::kCrypto`` (81)

Dependencies
------------
(없음)

Overview
--------
암호학적 도구들을 이용하기 위한 라이브러리의 상태를 초기화하기 위한 모듈 인터페이스입니다.

모듈 초기화 이후, 암호학적 해시나 AES, RSA를 포함한 몇 가지 암호화 알고리즘, 그리고 HMAC-SHA1 서명 알고리즘을 이용할 수 있습니다.
