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
//  In order to run test runner with app itself instead of with unit test, comment out the following line.
//  i = [[IosTestRunner alloc]init];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
