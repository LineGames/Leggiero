============
Sound Module
============

Sound 모듈은 게임에서의 사운드 처리와 재생 기능을 제공합니다.

재생할 사운드를 특성에 따라 Sound와 BGM으로 나누어 처리할 수 있게 하였고, Sound 모듈에는 각각을 처리할 수 있는 두 컴퍼넌트를 구현하였습니다.


Module Common Information
=========================

**Project:** *Modules/Sound*

**Namespace:** ``Leggiero::Sound``

Module Interfaces
-----------------
(없음)

Components
----------
- `Sound Mixer Component`_
- `BGM Player Component`_

Notes
-----
사운드 파일의 디코딩을 포함하는 유틸리티 API 기능들을 모듈에 대한 별도의 초기화 과정 없이, ``Leggiero::Sound::ISoundProvider`` 인터페이스를 통해 이용할 수 있습니다.


Sound Provider
==============
사운드 모듈에서는 ``Leggiero::Sound::ISoundProvider`` 인터페이스를 통해 몇 가지 사운드 형식에 대한 처리를 지원합니다.

현재 .wav, .ogg, .mp3 형식의 사운드 디코딩을 Leggiero에서 공통으로 제공하고 있습니다.
모듈에 대한 별도의 초기화 과정 없이도, 사운드 모듈의 *Provider* 서브디렉터리의 헤더를 포함하셔서 ``ISoundProvider`` 객체를 직접 생성하여 사용하실 수 있습니다.


Sound Mixer Component
=====================
사운드의 재생과 업데이트, 관리를 위한 컴퍼넌트입니다.

**Type:** ``Leggiero::Sound::SoundMixerComponent``

**Component Id:** ``EngineComponentIdType::kSoundMixer`` (301)

Dependencies
------------
- `Application Component <41_mod_application.html>`_

Overview
--------
적은 latency를 가져야 하거나 정교한 컨트롤이 필요한 게임 Sound의 재생을 지원합니다.
개별 사운드는 ``ISoundProvider`` 의 형태로 제공됩니다.

Back-end
--------
플랫폼에 따라, 사운드 재생을 위해 서로 다른 back-end 구현을 사용합니다.

- iOS와 Windows PC 플랫폼에서는 OpenAL Soft 라이브러리를 이용하여, **OpenAL** back-end 믹서를 사용합니다.
- Android 플랫폼에서는 **Oboe** back-end 믹서를 사용합니다.

Play Sound
----------
아래와 같은 인터페이스를 통해 사운드를 재생할 수 있습니다:

.. code-block:: C++
    :caption: part of SoundMixerComponent.h
    
    virtual std::shared_ptr<SoundPlayingContext> Play(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately = true, float volume = 1.0f, bool isPauseAfterFinish = false) = 0;
    virtual std::shared_ptr<LoopingSoundPlayingContext> PlayLoopingSound(std::shared_ptr<ISoundProvider> sound, SampleNumberType loopStartSamplePosition, SampleNumberType loopFinishSamplePosition, bool isStartImmediately = true, float volume = 1.0f) = 0;
    virtual std::shared_ptr<BufferedSoundPlayingContext> PlayBufferedSound(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately = true, float volume = 1.0f, bool isLooping = false, bool isPauseAfterFinish = false) = 0;
    

사운드를 재생하면 Playing Context 형식의 사운드 **재생 핸들** 이 반환됩니다. 재생 핸들을 통해 각 사운드의 재생 상태를 확인하고, 사운드 재생에 대한 조작을 수행할 수 있습니다.

사운드 재생에 있어, 다음의 사항들을 참고해 주세요:

- ``PlayLoopingSound`` 메서드는 지정된 사운드를 일정 구간을 계속 반복하도록 재생하는 동작을 수행합니다.
  
  재생 핸들에서 루프를 해제하는 것이 가능하여, 적절하게 구성된 사운드를 원하는 시간 동안 지속적으로 재생하는 식으로 사용이 가능합니다.
  
  루프를 해제하지 않으면, 영원히 재생을 계속하게 되므로 주의해 주세요.
  
- ``PlayBufferedSound`` 메서드로 사운드를 재생하면, 미리 사운드의 전체 내용을 버퍼링하여 사운드를 재생합니다.
  
  재생 중 디코딩으로 인한 지연을 방지할 수 있으나, 사운드 데이터를 원본 포맷으로 메모리에 저장해 두어야 해서 많은 자원을 소모할 수 있습니다.
  
- 구현 방식의 문제로, 한 번 재생이 끝난(stop) **재생 핸들** 은 되감거나 다시 재생할 수 없습니다.
  
  재생 핸들의 재사용을 원하시면, ``isPauseAfterFinish`` 인자를 ``true`` 로 세팅하여, 플레이 종료 시 *stop* 대신 *pause* 상태로 대기하도록 해 주세요.
  
  재생이 끝나는(stop) 것은 **재생 핸들** 이 나타내는 특정 재생 행위의 context 입니다. ``ISoundProvider`` 인터페이스를 통해 제공되는 사운드 데이터는 재생 종료 후에도 재사용이 가능합니다.
  
- ``isPauseAfterFinish`` 가 ``false`` 인 경우, Play 함수의 **재생 핸들** 반환 값을 무시하는 식으로 fire-and-forgot 방식의 사운드 재생이 가능합니다. 이 경우, 해당 사운드의 재생이 끝날 때 까지 믹서가 사운드의 수명을 유지할 책임을 집니다.
  
  다만, PlayLoopingSound 를 fire-and-forgot 방식으로 호출하면 사운드의 재생이 영원히 끝나지 않을 것이란 점은 유념해 주세요.
  

.. Note:: ``ClearNonManagedSounds`` 메서드가 재생 핸들을 잃어버린 사운드를 정리해 주는 역할을 담당하기는 합니다만, 해당 함수의 사용은 Leggiero에서 권장하는 프로그래밍 방식이 아닙니다.


BGM Player Component
====================
배경 음악의 재생을 위한 컴퍼넌트입니다.

**Type:** ``Leggiero::Sound::BGMPlayerComponent``

**Component Id:** ``EngineComponentIdType::kBGMPlayer`` (302)

Dependencies
------------
- `Sound Mixer Component`_
- `Bundle File Resource Component <42_mod_filesystem.html>`_ *(Optional)*

Overview
--------
배경 음악으로 재생해야 할 사운드의 재생을 위한 컴퍼넌트입니다.
실제 재생을 플랫폼에 위임하여, 정교한 제어가 필요하지 않다면, 상대적으로 긴 음악 데이터도 좀 더 적은 리소스를 소비해 재생할 수 있습니다. 추가로, 플랫폼에서 지원한다면 사운드의 디코딩에 하드웨어 가속을 받을 수도 있습니다.

.. Note:: Windows PC 플랫폼에서는 BGM Player Component가 Sound Mixer Component를 이용하여 BGM을 Sound와 동일하게 처리하여 재생합니다.

Play Sound
----------
아래와 같은 인터페이스를 통해 BGM을 재생할 수 있습니다:

.. code-block:: C++
    :caption: part of BGMPlayerComponent.h
    
    virtual std::shared_ptr<IBGMPlayingHandle> PlayFromFile(const std::string &filePath, bool isStartImmediately = false, float volume = 1.0f, bool isLooping = false, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler = IBGMPlayingHandle::PlayFinishHandlerType()) = 0;
    virtual std::shared_ptr<IBGMPlayingHandle> PlayInBundle(const std::string &virtualPath, bool isStartImmediately = false, float volume = 1.0f, bool isLooping = false, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler = IBGMPlayingHandle::PlayFinishHandlerType()) = 0;
    

BGM Player Component의 생성 시, ``isUseBundleFileSystem`` 옵션을 ``true`` 로 설정한 경우에만 ``PlayInBundle`` 메서드를 이용할 수 있습니다.
이 경우, BGM Player Component는 Bundle File Resource Component에 의존성을 가지게 됩니다.

플랫폼에 재생을 위임하므로, 파일 시스템 상에 존재하여 경로를 알고 있거나, 번들 리소스로 포함된 사운드만을 재생할 수 있습니다.
지원하는 파일의 형식은 플랫폼과 어플리케이션이 구동되는 기기의 종류에 따라 다릅니다만, mp3 파일은 일반적으로 지원됩니다.

``IBGMPlayingHandle`` 을 통해 BGM의 재생을 제어할 수 있으며, 이 재생 핸들은 플랫폼에서 제공하는 BGM 플레이어의 래핑에 가까운 구현을 가지고 있습니다.
