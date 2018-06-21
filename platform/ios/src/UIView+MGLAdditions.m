#import "UIView+MGLAdditions.h"

@implementation UIView (MGLAdditions)

- (NSLayoutYAxisAnchor *)mgl_safeTopAnchor {
    if (@available(iOS 11.0, *)) {
        return self.safeAreaLayoutGuide.topAnchor;
    } else {
        return self.topAnchor;
    }
}

- (NSLayoutXAxisAnchor *)mgl_safeLeadingAnchor {
    if (@available(iOS 11.0, *)) {
        return self.safeAreaLayoutGuide.leadingAnchor;
    } else {
        return self.leadingAnchor;
    }
}

- (NSLayoutYAxisAnchor *)mgl_safeBottomAnchor {
    if (@available(iOS 11.0, *)) {
        return self.safeAreaLayoutGuide.bottomAnchor;
    } else {
        return self.bottomAnchor;
    }
}

- (NSLayoutXAxisAnchor *)mgl_safeTrailingAnchor {
    if (@available(iOS 11.0, *)) {
        return self.safeAreaLayoutGuide.trailingAnchor;
    } else {
        return self.trailingAnchor;
    }
}

@end
