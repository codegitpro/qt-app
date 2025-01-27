//
//  UIImage+IQMediaPickerController.m
//  IQMediaPickerController
//
//  Created by Iftekhar on 04/08/18.
//

#import "UIImage+IQMediaPickerController.h"
#import "IQMediaPickerController.h"

@implementation UIImage (IQMediaPickerController)

+(UIImage*)imageInsideMediaPickerBundleNamed:(NSString*)name
{
    static NSBundle *bundle = nil;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        // Get the top level "bundle" which may actually be the framework
        bundle = [NSBundle bundleForClass:[IQMediaPickerController class]];
    });
    
    return [UIImage imageNamed:name inBundle:bundle compatibleWithTraitCollection:nil];
}

@end
