#import "MBXAnnotationView.h"

@interface MBXAnnotationView ()

@property (nonatomic) UIView *centerView;

@end

@implementation MBXAnnotationView

- (void)layoutSubviews {
    [super layoutSubviews];
    if (!self.centerView) {
        self.backgroundColor = [UIColor blueColor];
        self.centerView = [[UIView alloc] initWithFrame:CGRectInset(self.bounds, 1.0, 1.0)];
        self.centerView.backgroundColor = self.centerColor;
        [self addSubview:self.centerView];
    }
}

- (void)setCenterColor:(UIColor *)centerColor {
    if (![_centerColor isEqual:centerColor]) {
        _centerColor = centerColor;
        self.centerView.backgroundColor = centerColor;
    }
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
    
    self.layer.borderColor = selected ? [UIColor blackColor].CGColor : [UIColor whiteColor].CGColor;
    self.layer.borderWidth = selected ? 2.0 : 0;
}

@end
