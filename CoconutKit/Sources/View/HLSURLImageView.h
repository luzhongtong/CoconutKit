//
//  HLSURLImageView.h
//  CoconutKit
//
//  Created by Samuel Défago on 09.05.12.
//  Copyright (c) 2012 Hortis. All rights reserved.
//

#import "HLSURLConnection.h"

@class HLSAnimation;
@class HLSZeroingWeakRef;

typedef enum {
    HLSLoadingTransitionStyleEnumBegin = 0,
    HLSLoadingTransitionStyleNone = HLSLoadingTransitionStyleEnumBegin,
    HLSLoadingTransitionStyleCrossDissolve,
    HLSLoadingTransitionStyleFlipFromLeft,
    HLSLoadingTransitionStyleFlipfromRight,
    HLSLoadingTransitionStyleEnumEnd,
    HLSLoadingTransitionStyleEnumSize = HLSLoadingTransitionStyleEnumEnd - HLSLoadingTransitionStyleEnumBegin
} HLSLoadingTransitionStyle;

@interface HLSURLImageView : UIView <HLSURLConnectionDelegate> {
@private
    HLSZeroingWeakRef *_connectionZeroingWeakRef;
    UIImageView *_imageView;
    UIImage *_emptyImage;
    UIActivityIndicatorView *_activityIndicatorView;
    HLSAnimation *_loadingAnimation;
}

// Use nil to clear
- (void)loadWithRequest:(NSURLRequest *)request;

@property (nonatomic, readonly, retain) UIImage *image;

@property (nonatomic, retain) UIImage *emptyImage;

@end
