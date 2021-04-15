////////////////////////////////////////////////////////////////////////////////
// Service/WinTextInputDialog.cpp (Leggiero - Platform.WinPC)
//
// Windows Text Input Dialog Service Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WinTextInputDialog.h"

// Standard Library
#include <map>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Platform.WinPC
#include "../resource.h"
#include "../WindowsUtility.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			namespace _Internal
			{
				//////////////////////////////////////////////////////////////////////////////// WinTextInputDialogMessageDispatcher

				class WinTextInputDialogMessageDispatcher
				{
				public:
					WinTextInputDialogMessageDispatcher();

				public:
					void RegisterDialog(HWND hWnd, WinTextInputDialog *controller);
					void UnRegisterDialog(HWND hWnd);

					WinTextInputDialog *FindController(HWND hWnd);

				private:
					std::map<HWND, WinTextInputDialog *>	m_dialogTable;
					SRWLOCK									m_tableLock;
				};

				WinTextInputDialogMessageDispatcher::WinTextInputDialogMessageDispatcher()
				{
					InitializeSRWLock(&m_tableLock);
				}

				void WinTextInputDialogMessageDispatcher::RegisterDialog(HWND hWnd, WinTextInputDialog *controller)
				{
					PSRWLOCK lockPointer = &m_tableLock;
					auto unlockFunc = [lockPointer] { ReleaseSRWLockExclusive(lockPointer); };
					FINALLY_OF_BLOCK(_UNLOCK_TABLE, unlockFunc);

					AcquireSRWLockExclusive(lockPointer);

					m_dialogTable.insert(std::make_pair(hWnd, controller));
				}

				void WinTextInputDialogMessageDispatcher::UnRegisterDialog(HWND hWnd)
				{
					PSRWLOCK lockPointer = &m_tableLock;
					auto unlockFunc = [lockPointer] { ReleaseSRWLockExclusive(lockPointer); };
					FINALLY_OF_BLOCK(_UNLOCK_TABLE, unlockFunc);

					AcquireSRWLockExclusive(lockPointer);

					m_dialogTable.erase(hWnd);
				}

				WinTextInputDialog *WinTextInputDialogMessageDispatcher::FindController(HWND hWnd)
				{
					PSRWLOCK lockPointer = &m_tableLock;
					auto unlockFunc = [lockPointer] { ReleaseSRWLockShared(lockPointer); };
					FINALLY_OF_BLOCK(_UNLOCK_TABLE, unlockFunc);

					AcquireSRWLockShared(lockPointer);

					std::map<HWND, WinTextInputDialog *>::iterator findIt = m_dialogTable.find(hWnd);
					if (findIt != m_dialogTable.end())
					{
						return findIt->second;
					}
					return nullptr;
				}

				WinTextInputDialogMessageDispatcher g_winTextInputDialogMessageDispatcher;
			}


			//////////////////////////////////////////////////////////////////////////////// WinTextInputDialog

			//------------------------------------------------------------------------------
			WinTextInputDialog::WinTextInputDialog(HINSTANCE hInstance, HWND hWndParent)
				: m_isOpen(false)
				, m_hInstance(hInstance), m_hWndParent(hWndParent)
				, m_hWnd(NULL)
				, m_isOk(false)
			{
			}

			//------------------------------------------------------------------------------
			WinTextInputDialog::~WinTextInputDialog()
			{
				if (m_hWnd != NULL)
				{
					EndDialog(m_hWnd, FALSE);
					m_hWnd = NULL;
				}
			}

			//------------------------------------------------------------------------------
			// Open Text Input Dialog
			void WinTextInputDialog::OpenTextInputDialog(const char *dialogTitle, const char *dialogMessage, const GameDataString &initialText, const char *okButtonCaption, const char *cancelButtonCaption)
			{
				if (m_isOpen)
				{
					return;
				}

				m_dialogTitle = dialogTitle;
				m_initialText = initialText;

				// dialogMessage, okButtonCaption, and cancelButtonCaption are Ignored now

				DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_LEGGIERO_INPUT_DIALOG), m_hWndParent, (DLGPROC)WinTextInputDialog::_InputDialogProcess, (LPARAM)this);
			}

			//------------------------------------------------------------------------------
			// Check whether the text input dialog is open now
			bool WinTextInputDialog::IsTextInputDialogOpen()
			{
				return m_isOpen;
			}

			//------------------------------------------------------------------------------
			// Get last value input to the text input dialog
			GameDataString WinTextInputDialog::GetTextInputDialogText()
			{
				return m_textValue;
			}

			//------------------------------------------------------------------------------
			LRESULT CALLBACK WinTextInputDialog::_InputDialogProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message)
				{
					case WM_INITDIALOG:
						{
							WinTextInputDialog *pDialog = (WinTextInputDialog *)lParam;
							_Internal::g_winTextInputDialogMessageDispatcher.RegisterDialog(hWnd, pDialog);

							pDialog->_OnInitDialog(hWnd);
						}
						break;

					case WM_COMMAND:
						{
							int wmId = LOWORD(wParam);
							switch (wmId)
							{
								case IDOK:
									{
										WinTextInputDialog *pDialog = _Internal::g_winTextInputDialogMessageDispatcher.FindController(hWnd);
										if (pDialog != nullptr)
										{
											pDialog->_OnOk();
											_Internal::g_winTextInputDialogMessageDispatcher.UnRegisterDialog(hWnd);
										}

										EndDialog(hWnd, TRUE);
									}
									break;

								case IDCANCEL:
									{
										WinTextInputDialog *pDialog = _Internal::g_winTextInputDialogMessageDispatcher.FindController(hWnd);
										if (pDialog != nullptr)
										{
											pDialog->_OnCancel();
											_Internal::g_winTextInputDialogMessageDispatcher.UnRegisterDialog(hWnd);
										}

										EndDialog(hWnd, FALSE);
									}
									break;
							}
						}
						break;
				}

				return 0;
			}

			//------------------------------------------------------------------------------
			void WinTextInputDialog::_OnInitDialog(HWND hWnd)
			{
				m_hWnd = hWnd;
				m_isOpen = true;

				std::vector<wchar_t> utf16Title = Utility::GameDataString_2_WSTR(m_dialogTitle);
				SetWindowTextW(hWnd, &utf16Title[0]);

				std::vector<wchar_t> utf16TextValue = Utility::GameDataString_2_WSTR(m_initialText);
				SetDlgItemTextW(hWnd, IDC_INPUT, &utf16TextValue[0]);

				SetFocus(GetDlgItem(hWnd, IDC_INPUT));
			}

			//------------------------------------------------------------------------------
			void WinTextInputDialog::_OnOk()
			{
				const size_t MAX_VALUE_SIZE = 2048;

				TCHAR buffer[MAX_VALUE_SIZE];
				GetDlgItemText(m_hWnd, IDC_INPUT, buffer, MAX_VALUE_SIZE);
				m_textValue = Utility::TSTR_2_GameDataString(buffer);
				
				m_isOk = true;
				m_isOpen = false;
				m_hWnd = NULL;
			}

			//------------------------------------------------------------------------------
			void WinTextInputDialog::_OnCancel()
			{
				m_textValue = "";

				m_isOk = false;
				m_isOpen = false;
				m_hWnd = NULL;
			}
		}
	}
}
