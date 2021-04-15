////////////////////////////////////////////////////////////////////////////////
// Service/iOSTextInputDialog.cpp (Leggiero - Platform.iOS)
//
// iOS Text Input Dialog Service Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "iOSTextInputDialog.h"

// System Library
#import <UIKit/UIKit.h>


#pragma mark - iOS Stuffs

@interface ShowAlert : NSObject
{
    @public std::string inputDialogText;
    @public bool        isInputDialogOpened;
    @public bool        isInputDialogCanceled;
    
    UIWindow    *alertWindow;
    UITextField *textField;
}

+ (ShowAlert *)Alert;

@end


@implementation ShowAlert

+ (ShowAlert *)Alert
{
    ShowAlert *pAlert = [[ShowAlert alloc] init];
    return pAlert;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        inputDialogText = "";
        isInputDialogOpened = false;
        isInputDialogCanceled = false;
        
        alertWindow = nil;
    }
    return self;
}

- (BOOL)SimpleTextInput:(NSString *)title
              acceptMsg:(NSString *)accept
              cancelMsg:(NSString *)cancel
             defaultMsg:(NSString *)defaultMessage
           initialValue:(NSString *)initialValue
{
    [self createAlertContainerWindow];
    
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:title
                                                                   message:nil
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction *acceptAction = [UIAlertAction actionWithTitle:accept
                                                           style:UIAlertActionStyleDefault
                                                         handler:^(UIAlertAction *action) {
        [self processOnOk];
    }];
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:cancel
                                                           style:UIAlertActionStyleCancel
                                                         handler:^(UIAlertAction *action) {
        [self processOnCancel];
    }];
    
    [alert addAction:acceptAction];
    [alert addAction:cancelAction];
    
    [alert addTextFieldWithConfigurationHandler:^(UITextField *textField) {
        textField.placeholder = defaultMessage;
        textField.text = initialValue;
    }];
    textField = alert.textFields[0];
    inputDialogText = [initialValue UTF8String];
    
    [alertWindow.rootViewController presentViewController:alert
                                                 animated:YES
                                               completion:nil];
    isInputDialogOpened = true;
    
    return true;
}

- (void)processOnOk
{
    isInputDialogCanceled = false;
    inputDialogText = [[textField text] UTF8String];
    
    textField = nil;
    [self destroyAlertContainerWindow];
    isInputDialogOpened = false;
}

- (void)processOnCancel
{
    isInputDialogCanceled = true;
    
    textField = nil;
    [self destroyAlertContainerWindow];
    isInputDialogOpened = false;
}

- (void)createAlertContainerWindow
{
    alertWindow = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    alertWindow.rootViewController = [[UIViewController alloc] init];
    
    id<UIApplicationDelegate> delegate = [UIApplication sharedApplication].delegate;
    if ([delegate respondsToSelector:@selector(window)])
    {
        alertWindow.tintColor = delegate.window.tintColor;
    }
    
    UIWindow *topWindow = [UIApplication sharedApplication].windows.lastObject;
    if (topWindow != nil)
    {
        alertWindow.windowLevel = topWindow.windowLevel + 1;
    }
    
    [alertWindow makeKeyAndVisible];
}

- (void)destroyAlertContainerWindow
{
    alertWindow = nil;
}

@end


#pragma mark - Leggiero Objects

namespace Leggiero
{
	namespace Platform
	{
		namespace iOS
		{
			//////////////////////////////////////////////////////////////////////////////// IOSTextInputDialog

			//------------------------------------------------------------------------------
            IOSTextInputDialog::IOSTextInputDialog()
			{
                m_alertDialog = nil;
			}

			//------------------------------------------------------------------------------
            IOSTextInputDialog::~IOSTextInputDialog()
			{
			}

			//------------------------------------------------------------------------------
			// Open Text Input Dialog
			void IOSTextInputDialog::OpenTextInputDialog(const char *dialogTitle, const char *dialogMessage, const GameDataString &initialText, const char *okButtonCaption, const char *cancelButtonCaption)
			{
                NSString *title = [NSString stringWithUTF8String:dialogTitle];
                NSString *defaultMessage = [NSString stringWithUTF8String:dialogMessage];
                NSString *initialTextString = [NSString stringWithUTF8String:initialText.c_str()];
                NSString *acceptButtonName = [NSString stringWithUTF8String:okButtonCaption];
                NSString *cancelButtonName = [NSString stringWithUTF8String:cancelButtonCaption];
                
                m_alertDialog = [ShowAlert Alert];
                [m_alertDialog SimpleTextInput:title acceptMsg:acceptButtonName cancelMsg:cancelButtonName defaultMsg:defaultMessage initialValue:initialTextString];
			}

			//------------------------------------------------------------------------------
			// Check whether the text input dialog is open now
			bool IOSTextInputDialog::IsTextInputDialogOpen()
			{
                if (m_alertDialog == nil)
                {
                    return false;
                }
                return ((ShowAlert *)m_alertDialog)->isInputDialogOpened;
			}

			//------------------------------------------------------------------------------
			// Get last value input to the text input dialog
			GameDataString IOSTextInputDialog::GetTextInputDialogText()
			{
                if (m_alertDialog == nil)
                {
                    return "";
                }
                return ((ShowAlert *)m_alertDialog)->inputDialogText;
			}

			//------------------------------------------------------------------------------
			// Check whether the dialog was closed as OK or Canceled
			bool IOSTextInputDialog::IsDialogClosedOK()
			{
                if (m_alertDialog == nil)
                {
                    return false;
                }
                return !((ShowAlert *)m_alertDialog)->isInputDialogCanceled;
			}
		}
	}
}
