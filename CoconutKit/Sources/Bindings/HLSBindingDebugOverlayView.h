//
//  HLSBindingDebugOverlayView.h
//  CoconutKit
//
//  Created by Samuel Défago on 02/12/13.
//  Copyright (c) 2013 Hortis. All rights reserved.
//

@interface HLSBindingDebugOverlayView : UIView <UIPopoverControllerDelegate>

- (id)initWithDebuggedViewController:(UIViewController *)debuggedViewController recursive:(BOOL)recursive;

- (void)show;

@end