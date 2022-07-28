////////////////////////////////////////////////////////////////////////////////
// LeggieroViewController.h (Leggiero - Platform.iOS)
//
// Game View Controller Base Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __PLATFORM__LEGGIERO_VIEW_CONTROLLER_H
#define __PLATFORM__LEGGIERO_VIEW_CONTROLLER_H

// External Library
#import <MetalANGLE/MGLKViewController.h>


// Our iOS view controller base class
@interface LeggieroViewController : MGLKViewController
{
@protected
    bool m_isLandscape;
}

// Override point for initial showing before first frame
- (id)getInitialShowingUIView;
- (void)setInitialShowingViewTo:(UIView *)parentView target:(UIView *)target;
- (void)handleFirstFrameConfirmed;

@end

#endif
