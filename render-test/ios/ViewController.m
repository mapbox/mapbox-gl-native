#import "ViewController.h"
#import "iosTestRunner.h"

@interface ViewController ()
{
    IosTestRunner* i;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    i = [[IosTestRunner alloc]init];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
