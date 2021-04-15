============
Known Issues
============

현재 Leggiero 엔진에는 아래와 같은, 알려졌으나 고쳐지지 않은 이슈들이 존재합니다:

* iOS에서 MGLKView의 시작 tansition animation이 수정되지 않습니다. iOS 앱 시작 시, 원하는 형태의 자연스러운 애니메이션 연출이 어렵습니다.
  
  해당 문제에 대해서는 MetalANGLE 라이브러리의 GitHub 저장소에 issue를 생성해 문의한 상태입니다.
  

* Android에서 back button을 통해 앱을 종료하였다가 다시 기동시키면, 화면이 출력되지 않습니다.
  back button을 통해 home screen으로 나갈 경우 Activity는 파괴되나 프로세스는 종료되지 않아, 프로세스 내에서의 EGL 상태 내에 문제가 있는 것이 아닐까 추정하고 있습니다.
  
  몇 가지 해결책을 시도해 보았으나 정상 동작이 이뤄지지 않아, back button으로 앱이 종료되지 않도록 기본 설정을 수정해 두었습니다.
  각 프로젝트 어플리케이션의 Activity에서 핸들러를 override 하여, back button의 처리를 아예 앱을 종료하도록 수정하는 것도 해결책이 될 수 있습니다.
