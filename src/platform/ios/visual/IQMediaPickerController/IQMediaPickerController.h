//
//  IQMediaPickerController.h
//  https://github.com/hackiftekhar/IQMediaPickerController
//  Copyright (c) 2013-17 Iftekhar Qurashi.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#import <UIKit/UINavigationController.h>
#import <Photos/PhotosTypes.h>
#import <AVFoundation/AVCaptureDevice.h>

//! Project version number for IQMediaPickerController.
FOUNDATION_EXPORT double IQMediaPickerControllerVersionNumber;

//! Project version string for IQMediaPickerController.
FOUNDATION_EXPORT const unsigned char IQMediaPickerControllerVersionString[];

#import "IQMediaPickerSelection.h"
#import "AITheme.h"

@protocol IQMediaPickerControllerDelegate;

@interface IQMediaPickerController : UINavigationController
- (_Nonnull instancetype)initWithTheme:(AITheme *_Nonnull)theme;

@property(nonatomic, weak, nullable) id<IQMediaPickerControllerDelegate,UINavigationControllerDelegate> delegate;
@property NSUInteger maximumItemCount;

@property(nonatomic, nullable) NSArray <NSNumber * > * mediaTypes;
@property(nonatomic) NSTimeInterval videoMaximumDuration;
@property(nonatomic) NSTimeInterval audioMaximumDuration;
@property(nonatomic, nullable) NSArray <AVCaptureSessionPreset> * allowedVideoQualities;

@end

@protocol IQMediaPickerControllerDelegate <NSObject>

- (void)mediaPickerController:(IQMediaPickerController*_Nonnull)controller didFinishMedias:(IQMediaPickerSelection *_Nonnull)selection;
- (void)mediaPickerControllerDidCancel:(IQMediaPickerController *_Nonnull)controller;

@end
