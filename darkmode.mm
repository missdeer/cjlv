#import <Foundation/Foundation.h>

bool isDarkMode()
{
    NSString *osxMode = [[NSUserDefaults standardUserDefaults] stringForKey:@"AppleInterfaceStyle"];
    if (osxMode == nil)
        return false;
    return [osxMode isEqualToString:@"Dark"];
}
