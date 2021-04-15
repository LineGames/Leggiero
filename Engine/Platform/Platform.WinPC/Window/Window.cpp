////////////////////////////////////////////////////////////////////////////////
// Window/Window.cpp (Leggiero - Platform.WinPC)
//
// Implementation of Window
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "Window.h"

// Standard Library
#include <atomic>
#include <memory>

// External Library
#include <tpcshrd.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Platform.WinPC
#include "WindowCreateParam.h"
#include "WindowException.h"
#include "WindowMessage.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			namespace _Internal
			{
				//////////////////////////////////////////////////////////////////////////////// Thread Arguments

				struct WindowThreadFunctionArgs
				{
				public:
					Window *_this;

					WindowCreateParam			createParam;

					std::atomic<bool>			isCreateSuccess;
					WindowCreateFailException	createFailInfo;

					WindowThreadFunctionArgs(const WindowThreadFunctionArgs &rhs)
						: _this(rhs._this), createParam(rhs.createParam), isCreateSuccess(rhs.isCreateSuccess.load()), createFailInfo(rhs.createFailInfo)
					{
					}

					WindowThreadFunctionArgs(Window *_this, const WindowCreateParam &createParam)
						: _this(_this), createParam(createParam), isCreateSuccess(false)
					{
					}
				};
			}

			//////////////////////////////////////////////////////////////////////////////// Window

			//------------------------------------------------------------------------------
			Window::Window(const WindowCreateParam &createParam, IWindowEventObserver *messageObserver)
				: m_hWnd(NULL), m_isValidWindow(false), m_isTouchEnabled(false), m_lastError(ERROR_SUCCESS), m_messageObserver(messageObserver)
			{
				m_createWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

				std::shared_ptr<_Internal::WindowThreadFunctionArgs> threadFuncArgs(std::make_shared<_Internal::WindowThreadFunctionArgs>(this, createParam));
				m_messageLoopThread = new pthread_t;
				pthread_create(m_messageLoopThread, NULL, WindowThreadFunction, (void *)&threadFuncArgs);
				WaitForSingleObject(m_createWaitEvent, INFINITE);

				if (!threadFuncArgs->isCreateSuccess.load())
				{
					delete m_messageLoopThread;
					m_messageLoopThread = NULL;

					throw threadFuncArgs->createFailInfo;
				}
			}

			//------------------------------------------------------------------------------
			Window::~Window()
			{
				if (m_isValidWindow)
				{
					m_isValidWindow = false;
					if (m_isTouchEnabled)
					{
						m_isTouchEnabled = false;
						UnregisterTouchWindow(m_hWnd);
					}
					DestroyWindow(m_hWnd);
				}

#ifdef LEGGIERO_OPTIMIZE_WINDOW_ONLYONE
				if (_messageOwnerWindow == this)
				{
					_messageOwnerWindow = NULL;
				}
#else
				WindowManager::GetInstance().UnRegisterWindow(*this);
#endif

				if (m_messageLoopThread != NULL)
				{
					pthread_join(*m_messageLoopThread, NULL);
					delete m_messageLoopThread;
					m_messageLoopThread = NULL;
				}
			}

			//------------------------------------------------------------------------------
			void *__PTW32_CDECL Window::WindowThreadFunction(void *Args)
			{
				std::shared_ptr<_Internal::WindowThreadFunctionArgs> threadFuncArgsCopy(*((std::shared_ptr<_Internal::WindowThreadFunctionArgs> *)Args));
				_Internal::WindowThreadFunctionArgs &threadFuncArgs = (*threadFuncArgsCopy);

				threadFuncArgs.isCreateSuccess.store(false);
				{
					auto signalFunc = [&threadFuncArgs] { SetEvent(threadFuncArgs._this->m_createWaitEvent); };
					FINALLY_OF_BLOCK(_CREATE_EVENT_SIGNAL, signalFunc);

					if (!WindowRegisterClass(threadFuncArgs._this, threadFuncArgs.createParam))
					{
						threadFuncArgs.createFailInfo.isClassRegisterFailed = true;
						threadFuncArgs.createFailInfo.isWindowCreateFailed = false;
						threadFuncArgs.createFailInfo.lastError = GetLastError();
						return NULL;
					}
					if (!WindowCreateWindow(threadFuncArgs._this, threadFuncArgs.createParam))
					{
						threadFuncArgs.createFailInfo.isClassRegisterFailed = false;
						threadFuncArgs.createFailInfo.isWindowCreateFailed = true;
						threadFuncArgs.createFailInfo.lastError = GetLastError();
						return NULL;
					}

					ShowWindow(threadFuncArgs._this->m_hWnd, threadFuncArgs.createParam.nCmdShow);

					// Touch Support
					int digitigerCheckValue = GetSystemMetrics(SM_DIGITIZER);
					if (digitigerCheckValue & NID_READY)
					{
						threadFuncArgs._this->m_isTouchEnabled = true;
						RegisterTouchWindow(threadFuncArgs._this->m_hWnd, TWF_FINETOUCH | TWF_WANTPALM);
						DisableWindowGestures(threadFuncArgs._this);
					}

#ifdef LEGGIERO_OPTIMIZE_WINDOW_ONLYONE
					_messageOwnerWindow = threadFuncArgs._this;
#else
					WindowManager::GetInstance().RegisterWindow(*threadFuncArgs._this);
#endif

					threadFuncArgs.isCreateSuccess.store(true);
					threadFuncArgs._this->m_isValidWindow = true;
				}

				MessageLoopFunction(threadFuncArgs._this);
				threadFuncArgs._this->m_isValidWindow = false;

				IWindowEventObserver *eventObserver = threadFuncArgs._this->m_messageObserver;
				if (eventObserver != NULL)
				{
					eventObserver->OnDestoryed(threadFuncArgs._this->m_hWnd);
				}

				return NULL;
			}

			//------------------------------------------------------------------------------
			bool Window::WindowRegisterClass(Window *_this, const WindowCreateParam &createParam)
			{
				WNDCLASSEX wcex;
				wcex.cbSize = sizeof(WNDCLASSEX);
				wcex.style = createParam.classStyle;
				wcex.lpfnWndProc = WindowMessageCallback;
				wcex.cbClsExtra = 0;
				wcex.cbWndExtra = 0;
				wcex.hInstance = createParam.hInstance;
				wcex.hIcon = createParam.hIcon;
				wcex.hCursor = createParam.hCursor;
				wcex.hbrBackground = createParam.hbrBackground;
				wcex.lpszMenuName = createParam.menuName;
				wcex.lpszClassName = createParam.className;
				wcex.hIconSm = createParam.hIconSm;

				ATOM res = RegisterClassEx(&wcex);
				return (res != 0);
			}

			//------------------------------------------------------------------------------
			bool Window::WindowCreateWindow(Window *_this, const WindowCreateParam &createParam)
			{
				int width;
				int height;
				if (createParam.isSetContentsSize)
				{
					RECT preCalculate;
					SetRect(&preCalculate, 0, 0, createParam.width, createParam.height);
					AdjustWindowRect(&preCalculate, createParam.windowStyle,
						(createParam.hMenu == NULL) ? FALSE : TRUE);
					width = preCalculate.right - preCalculate.left;
					height = preCalculate.bottom - preCalculate.top;
				}
				else
				{
					width = createParam.width;
					height = createParam.height;
				}

				_this->m_hWnd = CreateWindow(
					createParam.className,
					createParam.windowName,
					createParam.windowStyle,
					createParam.x, createParam.y,
					width, height,
					createParam.hWndParent,
					createParam.hMenu,
					NULL,
					createParam.lpParam);

				return (_this->m_hWnd != NULL);
			}

			//------------------------------------------------------------------------------
			bool Window::DisableWindowGestures(Window *_this)
			{
				ATOM atomID = 0;
				DWORD dwHwndTabletProperty = (
					TABLET_DISABLE_PRESSANDHOLD |		// disables press and hold (right-click) gesture  
					TABLET_DISABLE_PENTAPFEEDBACK |		// disables UI feedback on pen up (waves)  
					TABLET_DISABLE_PENBARRELFEEDBACK |	// disables UI feedback on pen button down  
					TABLET_DISABLE_FLICKS);				// disables pen flicks (back, forward, drag down, drag up)   
				LPCTSTR tabletAtom = L"MicrosoftTabletPenServiceProperty";

				// Get the Tablet PC atom ID
				atomID = GlobalAddAtom(tabletAtom);

				if (atomID)
				{
					// Try to disable press and hold gesture by 
					// setting the window property, return the result
					SetProp(_this->m_hWnd, tabletAtom, (HANDLE)((uint64_t)dwHwndTabletProperty));
				}

				{
					// set the settings in the gesture configuration
					GESTURECONFIG gc[] = { 0, 0, GC_ALLGESTURES };

					UINT uiGcs = 1;
					BOOL bResult = SetGestureConfig(_this->m_hWnd, 0, uiGcs, gc, sizeof(GESTURECONFIG));

					if (!bResult)
					{
						// error handling - never happened though while testing
					}
				}

				return true;
			}

			//------------------------------------------------------------------------------
			void Window::MessageLoopFunction(Window *_this)
			{
				BOOL getMessageRet;
				MSG msg;
				while (_this->m_isValidWindow &&
					(getMessageRet = GetMessage(&msg, NULL, 0, 0)) != 0)
				{
					if (getMessageRet == -1)
					{
						_this->m_lastError = GetLastError();
					}
					else
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					Sleep(0);
				}
			}


			//////////////////////////////////////////////////////////////////////////////// WindowCreateFailException

			//------------------------------------------------------------------------------
			// Create a concrete exception based on fail information
			WindowCreateFailException::WindowCreateFailException(
				bool isClassRegisterFailed, bool isWindowCreateFailed,
				DWORD lastError)
				: std::exception("Failed to create window")
				, isClassRegisterFailed(isClassRegisterFailed)
				, isWindowCreateFailed(isWindowCreateFailed)
				, lastError(lastError)
			{
			}
		}
	}
}
