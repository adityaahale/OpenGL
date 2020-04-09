#import <UIKit/UIKit.h>

enum{
    ATA_ATTRIBUTE_POSITION = 0,
    ATA_ATTRIBUTE_COLOR,
    ATA_ATTRIBUTE_NORMAL,
    ATA_ATTRIBUTE_TEXTURE0
};
@interface GLESView : UIView <UIGestureRecognizerDelegate>
-(void)startAnimation;
-(void)stopAnimation;
@end
