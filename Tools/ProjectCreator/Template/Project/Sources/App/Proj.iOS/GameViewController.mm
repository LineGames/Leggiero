////////////////////////////////////////////////////////////////////////////////
// GameViewController.mm (${{ProgramName}} - iOS)
//
// Game View Controller Implementation
////////////////////////////////////////////////////////////////////////////////

#import "GameViewController.h"

#include "${{ProgramName}}iOSSetting.h"


@implementation GameViewController
{
    
}


- (id)init
{
    self = [super init];
    if (self != nil)
    {
        m_isLandscape = ${{ProgramName}}::Setting::kIsLandscapeMode;
    }
    return self;
}

- (void)viewDidLoad
{
    m_isLandscape = ${{ProgramName}}::Setting::kIsLandscapeMode;
    [super viewDidLoad];
}

@end
