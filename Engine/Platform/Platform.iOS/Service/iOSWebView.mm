////////////////////////////////////////////////////////////////////////////////
// Service/iOSWebView.cpp (Leggiero - Platform.iOS)
//
// iOS Web View Service Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "iOSWebView.h"

// System Library
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>

// Leggiero.Platform.iOS
#include "../iOSPlatformUtility.h"


#pragma mark - Internal Utilities

namespace Leggiero
{
	namespace Platform
	{
		namespace iOS
		{
			namespace _Internal
			{
				//------------------------------------------------------------------------------
				// This is for single web view usage at a time
				static NSArray *s_currentWebViews = nil;

				//------------------------------------------------------------------------------
				UIWindow *_FindFirstKeyWindow()
				{
					NSArray *windows = [[UIApplication sharedApplication] windows];
					for (UIWindow *currentWindow in windows)
					{
						if ([currentWindow isKeyWindow] == YES)
						{
							return currentWindow;
						}
					}
					return nil;
				}

				//------------------------------------------------------------------------------
				void CloseCurrentWebViews()
				{
					NSArray *closingViews = s_currentWebViews;
					s_currentWebViews = nil;
					if (closingViews == nil)
					{
						return;
					}
					
					for (UIView *currentView in closingViews)
					{
						[currentView removeFromSuperview];
					}
				}
			}
		}
	}
}


#pragma mark - iOS Stuffs

@interface UIViewController (WebViewCloser)
- (void)closeButtonClicked;
@end

@implementation UIViewController (WebViewCloser)

- (void)closeButtonClicked
{
    Leggiero::Platform::iOS::_Internal::CloseCurrentWebViews();
}

@end


#pragma mark - Leggiero Objects

namespace Leggiero
{
	namespace Platform
	{
		namespace iOS
		{
			//////////////////////////////////////////////////////////////////////////////// IOSWebView

			//------------------------------------------------------------------------------
            IOSWebView::IOSWebView()
			{
			}

			//------------------------------------------------------------------------------
            IOSWebView::~IOSWebView()
			{
			}

			//------------------------------------------------------------------------------
			// Open URL in a Page
			void IOSWebView::OpenURLByPage(const char *pageTitle, const char *buttonCaption, const char *url)
			{
                UIWindow *keyWindow = _Internal::_FindFirstKeyWindow();
				if (keyWindow == nil)
				{
					// Something Wrong. We are using one window.
					return;
				}
				UIViewController *rootViewController = [keyWindow rootViewController];
				
				if (_Internal::s_currentWebViews != nil)
				{
					_Internal::CloseCurrentWebViews();
				}
				
				constexpr CGFloat kNavigationBarReferenceHeight = 36.0f;
				constexpr CGFloat kNavigationButtonMargin = 12.0f;
				constexpr CGFloat kNavigationTitleFontSize = 20.0f;
				constexpr CGFloat kNavigationTitleBottomMargin = 6.0f;
				
				// Initialize Fake Navigation Bar
				UIView *fakeNavigationBar = [[UIView alloc] init];
                if (@available(iOS 13.0, *)) {
                    fakeNavigationBar.backgroundColor = UIColor.systemBackgroundColor;
                    fakeNavigationBar.layer.borderColor = [UIColor separatorColor].CGColor;
                } else {
                    fakeNavigationBar.backgroundColor = [UIColor colorWithRed:1.0f green:1.0f blue:1.0f alpha:1.0f];
                    fakeNavigationBar.layer.borderColor = [UIColor colorWithRed:0.235f green:0.235f blue:0.263f alpha:1.0f].CGColor;
                }
				fakeNavigationBar.layer.borderWidth = 1.0f;
				fakeNavigationBar.userInteractionEnabled = YES;
				[rootViewController.view addSubview:fakeNavigationBar];
				
				fakeNavigationBar.translatesAutoresizingMaskIntoConstraints = NO;
				[fakeNavigationBar.leftAnchor constraintEqualToAnchor:rootViewController.view.leftAnchor].active = YES;
				[fakeNavigationBar.rightAnchor constraintEqualToAnchor:rootViewController.view.rightAnchor].active = YES;
				[fakeNavigationBar.topAnchor constraintEqualToAnchor:rootViewController.view.topAnchor].active = YES;
				[fakeNavigationBar.bottomAnchor constraintEqualToAnchor:rootViewController.view.layoutMarginsGuide.topAnchor constant:kNavigationBarReferenceHeight].active = YES;
				
				UIButton *closeButton = [[UIButton alloc] init];
				[closeButton setTitle:[NSString stringWithUTF8String:buttonCaption] forState:UIControlStateNormal];
                if (@available(iOS 13.0, *)) {
                    [closeButton setTitleColor:UIColor.linkColor forState:UIControlStateNormal];
                } else {
                    [closeButton setTitleColor:[UIColor colorWithRed:0.0f green:0.478f blue:1.0f alpha:1.0f] forState:UIControlStateNormal];
                }
				[closeButton sizeToFit];
				[closeButton addTarget:rootViewController action:@selector(closeButtonClicked) forControlEvents:UIControlEventTouchUpInside];
				[fakeNavigationBar addSubview:closeButton];
				
				closeButton.translatesAutoresizingMaskIntoConstraints = NO;
				[closeButton.rightAnchor constraintEqualToAnchor:fakeNavigationBar.layoutMarginsGuide.rightAnchor constant:-kNavigationButtonMargin].active = YES;
				[closeButton.bottomAnchor constraintEqualToAnchor:fakeNavigationBar.bottomAnchor].active = YES;
				
				UILabel *titleLabel = [[UILabel alloc] init];
				[titleLabel setText:[NSString stringWithUTF8String:pageTitle]];
				[titleLabel setFont:[UIFont boldSystemFontOfSize:kNavigationTitleFontSize]];
				[titleLabel sizeToFit];
				[fakeNavigationBar addSubview:titleLabel];
				
				titleLabel.translatesAutoresizingMaskIntoConstraints = NO;
				[titleLabel.centerXAnchor constraintEqualToAnchor:fakeNavigationBar.centerXAnchor].active = YES;
				[titleLabel.bottomAnchor constraintEqualToAnchor:fakeNavigationBar.bottomAnchor constant:-kNavigationTitleBottomMargin].active = YES;
				
				// Initialize Bottom Margin Area
				UIView *bottomMarginArea = [[UIView alloc] init];
                if (@available(iOS 13.0, *)) {
                    bottomMarginArea.backgroundColor = UIColor.systemBackgroundColor;
                    bottomMarginArea.layer.borderColor = [UIColor separatorColor].CGColor;
                } else {
                    bottomMarginArea.backgroundColor = [UIColor colorWithRed:1.0f green:1.0f blue:1.0f alpha:1.0f];
                    bottomMarginArea.layer.borderColor = [UIColor colorWithRed:0.235f green:0.235f blue:0.263f alpha:1.0f].CGColor;
                }
				bottomMarginArea.layer.borderWidth = 1.0f;
				bottomMarginArea.userInteractionEnabled = YES;
				[rootViewController.view addSubview:bottomMarginArea];
				
				bottomMarginArea.translatesAutoresizingMaskIntoConstraints = NO;
				[bottomMarginArea.leftAnchor constraintEqualToAnchor:rootViewController.view.leftAnchor].active = YES;
				[bottomMarginArea.rightAnchor constraintEqualToAnchor:rootViewController.view.rightAnchor].active = YES;
				[bottomMarginArea.topAnchor constraintEqualToAnchor:rootViewController.view.layoutMarginsGuide.bottomAnchor].active = YES;
				[bottomMarginArea.bottomAnchor constraintEqualToAnchor:rootViewController.view.bottomAnchor].active = YES;
				
				// Initialize Web View
				NSURL *urlToOpen = [NSURL URLWithString:[NSString stringWithUTF8String:url]];
				NSURLRequest *urlRequest = [NSURLRequest requestWithURL:urlToOpen];
				
				WKWebView *webView = [[WKWebView alloc] init];
				
				[webView loadRequest:urlRequest];
				
				[rootViewController.view addSubview:webView];
				
				webView.translatesAutoresizingMaskIntoConstraints = NO;
				[webView.leftAnchor constraintEqualToAnchor:rootViewController.view.leftAnchor].active = YES;
				[webView.rightAnchor constraintEqualToAnchor:rootViewController.view.rightAnchor].active = YES;
				[webView.topAnchor constraintEqualToAnchor:rootViewController.view.layoutMarginsGuide.topAnchor constant:kNavigationBarReferenceHeight].active = YES;
				[webView.bottomAnchor constraintEqualToAnchor:rootViewController.view.layoutMarginsGuide.bottomAnchor].active = YES;
				
				// Register to closer
				_Internal::s_currentWebViews = [NSArray arrayWithObjects:fakeNavigationBar, bottomMarginArea, webView, nil];
			}
			
			//------------------------------------------------------------------------------
			// Check Whether URL Page is Opened or NOT
			bool IOSWebView::IsURLPageOpened()
			{
				return (_Internal::s_currentWebViews != nil);
			}

			//------------------------------------------------------------------------------
			// Open URL
			void IOSWebView::OpenURL(const char *url)
			{
                NSString *urlString = [NSString stringWithUTF8String:url];
                NSURL *urlObject = [NSURL URLWithString:urlString];
                [[UIApplication sharedApplication] openURL:urlObject options:@{} completionHandler:nil];
			}

			//------------------------------------------------------------------------------
			// Check given URL Scheme is openable or not
			bool IOSWebView::CanOpenURL(const char *urlScheme)
			{
                NSURL *urlString = [NSURL URLWithString:[NSString stringWithUTF8String:urlScheme]];
                if ([[UIApplication sharedApplication] canOpenURL:urlString])
                {
                    return true;
                }
                return false;
			}
		}
	}
}
