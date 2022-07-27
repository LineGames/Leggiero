////////////////////////////////////////////////////////////////////////////////
// LeggieroViewController.mm (Leggiero - Platform.iOS)
//
// Game View Controller Base Implementation with MetalANGLE
////////////////////////////////////////////////////////////////////////////////

// My Header
#import "LeggieroViewController.h"

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// External Library
#include <MetalANGLE/GLES3/gl3.h>

// Leggiero.Platform.iOS
#include "LeggieroIOSCommon.h"
#include "iOSPlatformUtility.h"


@implementation LeggieroViewController
{
    float   m_scale;
    bool    m_isInsetSet;
    bool    m_isGraphicInitialized;
    
    bool    m_isFirstFrame;
    id      m_beforeFirstShowingView;
}


#pragma mark - Initializations

- (id)init
{
    self = [super init];
    if (self != nil)
    {
        m_isInsetSet = false;
        m_isGraphicInitialized = false;
        m_beforeFirstShowingView = nil;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    m_isFirstFrame = true;
    
    [self initializeScreenGL];
    
    m_beforeFirstShowingView = [self getInitialShowingUIView];
    if (m_beforeFirstShowingView != nil)
    {
        UIView *showingView = m_beforeFirstShowingView;
        [self setInitialShowingViewTo:self.view target:showingView];
    }
    
    [self initializeGameApplication];
}


- (void)initializeScreenGL
{
    self.preferredFramesPerSecond = [UIScreen mainScreen].maximumFramesPerSecond;
    
    m_scale = 1.0f;
    if ([[UIScreen mainScreen] respondsToSelector:@selector(nativeScale)])
    {
        m_scale = [[UIScreen mainScreen] nativeScale];
    }
    else if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
    {
        m_scale = [[UIScreen mainScreen] scale];
    }
    self.glView.layer.contentsScale = m_scale;
    
    // Create OpenGL context
    self.glView.drawableColorFormat = MGLDrawableColorFormatRGBA8888;
    self.glView.drawableDepthFormat = MGLDrawableDepthFormat24;
    self.glView.drawableStencilFormat = MGLDrawableStencilFormat8;
    self.glView.drawableMultisample = MGLDrawableMultisample4X;
    
    MGLContext *context = [[MGLContext alloc] initWithAPI:kMGLRenderingAPIOpenGLES3];
    self.glView.context = context;

    [MGLContext setCurrentContext:context];
    
    Leggiero::Platform::iOS::Graphics::SetMainGLContextInformation(context);
}


- (void)initializeGameApplication
{
    Leggiero::Platform::iOS::pApplication = new Leggiero::Platform::iOS::IOSPlatformApplication();
    
    // Process Screen Size
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    CGSize effectiveSize = screenSize;
    if ((m_isLandscape && (screenSize.width < screenSize.height))
        || (!m_isLandscape && (screenSize.width > screenSize.height)))
    {
        effectiveSize.width = screenSize.height;
        effectiveSize.height = screenSize.width;
    }
    
    Leggiero::Platform::iOS::pApplication->SetScreenSize(effectiveSize.width * m_scale, effectiveSize.height * m_scale, true);
    
    // Process PPI
    float screenPPI = Leggiero::Platform::iOS::CalculatePPI(effectiveSize.width * m_scale, effectiveSize.height * m_scale);
    Leggiero::Platform::iOS::pApplication->SetApproximateScreenPPI(screenPPI, screenPPI);
    
    // Process Application Metadata
    bool isIPad = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad);
    Leggiero::Platform::iOS::pApplication->SetIsTablet(isIPad);
    
    // Get Locale Information
    NSString *currentLanguage = [[NSLocale currentLocale] objectForKey:NSLocaleLanguageCode];
    NSString *currentCountry = [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode];
    id localeString = nil;
    if (currentCountry == nullptr)
    {
        localeString = [NSString stringWithFormat:@"%@", currentLanguage];
    }
    else
    {
        localeString = [NSString stringWithFormat:@"%@_%@", currentLanguage, currentCountry];
    }
    Leggiero::Platform::iOS::pApplication->SetSystemLocale([localeString UTF8String]);
    
    Leggiero::Platform::iOS::pApplication->InitializeGame();
    if (m_isInsetSet)
    {
        Leggiero::Platform::iOS::pApplication->InitializeGameGraphic();
        m_isGraphicInitialized = true;
    }
}


- (void)dealloc
{
    bool isNeedToClearGLContext = ([MGLContext currentContext] == self.glView.context);
    
    if (Leggiero::Platform::iOS::pApplication != nullptr)
    {
        [MGLContext setCurrentContext:self.glView.context];
        Leggiero::Platform::iOS::pApplication->TerminateGameGraphic();
        
        Leggiero::Platform::iOS::pApplication->TerminateGame();
        delete Leggiero::Platform::iOS::pApplication;
    }
    Leggiero::Platform::iOS::pApplication = nullptr;
    
    if (isNeedToClearGLContext)
    {
        [MGLContext setCurrentContext:nil];
    }
}


#pragma mark - Before First Frame Showing

- (id)getInitialShowingUIView
{
    return nil;
}

- (void)setInitialShowingViewTo:(UIView *)parentView
                         target:(UIView *)target
{
    target.translatesAutoresizingMaskIntoConstraints = NO;
    [parentView addSubview:target];
    
    [target.leadingAnchor constraintEqualToAnchor:parentView.leadingAnchor].active = YES;
    [target.trailingAnchor constraintEqualToAnchor:parentView.trailingAnchor].active = YES;
    [target.topAnchor constraintEqualToAnchor:parentView.topAnchor].active = YES;
    [target.bottomAnchor constraintEqualToAnchor:parentView.bottomAnchor].active = YES;
}

- (void)handleFirstFrameConfirmed
{
}


#pragma mark - Event Handlings

- (void)mglkView:(MGLKView *)view drawInRect:(CGRect)rect
{
    glViewport(0, 0, view.drawableSize.width, view.drawableSize.height);
    
    Leggiero::Platform::iOS::IOSPlatformApplication *pApp = Leggiero::Platform::iOS::pApplication;
    if (pApp != nullptr)
    {
        if (!m_isGraphicInitialized)
        {
            Leggiero::Platform::iOS::pApplication->InitializeGameGraphic();
            m_isGraphicInitialized = true;
        }
        
        if (!m_isFirstFrame)
        {
            if (m_beforeFirstShowingView != nil)
            {
                [m_beforeFirstShowingView removeFromSuperview];
                m_beforeFirstShowingView = nil;
                [self handleFirstFrameConfirmed];
            }
        }
        
        pApp->DrawFrame();
        m_isFirstFrame = false;
    }
}


- (void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    
    Leggiero::Platform::iOS::IOSPlatformApplication *pApp = Leggiero::Platform::iOS::pApplication;
    if (pApp != nullptr)
    {
        UIEdgeInsets safeInsets = self.view.safeAreaInsets;
        Leggiero::Platform::iOS::pApplication->SetSafePadding((int)(safeInsets.left * m_scale), (int)(safeInsets.top * m_scale), (int)(safeInsets.right * m_scale), (int)(safeInsets.bottom * m_scale));
        m_isInsetSet = true;
    }
}


- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
    
    Leggiero::Platform::iOS::IOSPlatformApplication *pApp = Leggiero::Platform::iOS::pApplication;
    if (pApp != nullptr)
    {
        pApp->SetScreenSize(size.width * m_scale, size.height * m_scale, false);
    }
}


#pragma mark - Setting Overridings

- (BOOL)prefersStatusBarHidden
{
    return YES;
}


- (BOOL)shouldAutorotate
{
    return YES;
}


#pragma mark - Handle Touch Input

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (Leggiero::Platform::iOS::pApplication == nullptr)
    {
        return;
    }
    
    NSTimeInterval currentUpTime = [[NSProcessInfo processInfo] systemUptime];
    Leggiero::GameTimeClockType::time_point currentClockTime = Leggiero::GameTimeClockType::now();
    for (UITouch *aTouch in touches)
    {
        CGPoint touchLoc = [aTouch locationInView:self.view];
        Leggiero::GameTimeClockType::time_point touchTime = currentClockTime + std::chrono::duration_cast<Leggiero::GameTimeClockType::duration>(std::chrono::duration<double>(aTouch.timestamp - currentUpTime));
        
        Leggiero::Platform::iOS::pApplication->OnTouchDown((Leggiero::Application::DeviceTouchIdType)aTouch,
                                    touchLoc.x * m_scale, touchLoc.y * m_scale,
                                    touchTime);
    }
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (Leggiero::Platform::iOS::pApplication == nullptr)
    {
        return;
    }
    
    NSTimeInterval currentUpTime = [[NSProcessInfo processInfo] systemUptime];
    Leggiero::GameTimeClockType::time_point currentClockTime = Leggiero::GameTimeClockType::now();
    for (UITouch *aTouch in touches)
    {
        CGPoint touchLoc = [aTouch locationInView:self.view];
        Leggiero::GameTimeClockType::time_point touchTime = currentClockTime + std::chrono::duration_cast<Leggiero::GameTimeClockType::duration>(std::chrono::duration<double>(aTouch.timestamp - currentUpTime));
        
        Leggiero::Platform::iOS::pApplication->OnTouchMove((Leggiero::Application::DeviceTouchIdType)aTouch,
                                    touchLoc.x * m_scale, touchLoc.y * m_scale,
                                    touchTime);
    }
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (Leggiero::Platform::iOS::pApplication == nullptr)
    {
        return;
    }
    
    NSTimeInterval currentUpTime = [[NSProcessInfo processInfo] systemUptime];
    Leggiero::GameTimeClockType::time_point currentClockTime = Leggiero::GameTimeClockType::now();
    for (UITouch *aTouch in touches)
    {
        CGPoint touchLoc = [aTouch locationInView:self.view];
        Leggiero::GameTimeClockType::time_point touchTime = currentClockTime + std::chrono::duration_cast<Leggiero::GameTimeClockType::duration>(std::chrono::duration<double>(aTouch.timestamp - currentUpTime));
        
        Leggiero::Platform::iOS::pApplication->OnTouchUp((Leggiero::Application::DeviceTouchIdType)aTouch,
                                    touchLoc.x * m_scale, touchLoc.y * m_scale,
                                    touchTime);
    }
}


- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (Leggiero::Platform::iOS::pApplication == nullptr)
    {
        return;
    }
    
    NSTimeInterval currentUpTime = [[NSProcessInfo processInfo] systemUptime];
    Leggiero::GameTimeClockType::time_point currentClockTime = Leggiero::GameTimeClockType::now();
    for (UITouch *aTouch in touches)
    {
        Leggiero::GameTimeClockType::time_point touchTime = currentClockTime + std::chrono::duration_cast<Leggiero::GameTimeClockType::duration>(std::chrono::duration<double>(aTouch.timestamp - currentUpTime));
        
        Leggiero::Platform::iOS::pApplication->OnTouchCancel((Leggiero::Application::DeviceTouchIdType)aTouch,
                                  touchTime);
    }
}

@end
