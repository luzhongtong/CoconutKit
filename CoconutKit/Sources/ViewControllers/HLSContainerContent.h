//
//  HLSContainerContent.h
//  CoconutKit
//
//  Created by Samuel Défago on 27.07.11.
//  Copyright 2011 Hortis. All rights reserved.
//

#import "HLSContainerStackView.h"

/**
 * View controllers inserted into view controller containers exhibit common properties:
 *   - they belong to a container, which they must be able to identify, and they should not be inserted into several
 *     containers at the same time
 *   - they are inserted into a container with some transition animation, and removed from it with the corresponding
 *     reverse animation
 *   - a child view controller's view frame is adjusted to match the container view it is inserted into
 *   - a child view controller's view should be created lazily at the time it is really required
 *   - it must be possible to preload a view controller container before it gets actually displayed (i.e. before the
 *     container view is loaded)
 *   - a view controller container must retain the child view controllers it manages
 *   - a view controller's view properties should be restored when it is removed from a container. It might namely
 *     happen that a client caches this view controller for later reuse
 *   - view lifecycle and rotation events must be forwarded correctly from the container to the contained view controllers
 *   - the view controller containment chain must be preserved, so that view controller properties can be automatically
 *     forwarded from a child to its parent (and higher up in the view controller hierarchy if this parent is itself
 *     embedded into a container). This makes it possible for a navigation controller to use relevant child view controller 
 *     properties when it presents the corresponding parent view controller, for example. Moreover, this ensures that when 
 *     a child view controller presents another view controller modally, it is actually its furthest ancestor who does. 
 *     Finally, this guarantees that the -[UIViewController interfaceOrientation] method returns a correct result
 *   - the iOS 5 containment API defines new methods -[UIViewController isMovingTo/FromParentViewController] so that
 *     a child knows when it is inserted or removed from a container. These methods must return a correct result
 *     for custom containers as well
 *   - when a view controller is removed from a container, its view must not be released. This lets clients decide whether
 *     they want to cache the associated view (by retaining the view controller elswhere) or not (if the view controller 
 *     is not retained elsewhere, it will simply be deallocated when it gets removed from the container, and so will be 
 *     its view)
 *
 * The private HLSContainerContent class provides a way to ensure that all above common properties can be easily 
 * fulfilled. It can be seen as some kind of smart pointer object, keeping ownership of a view controller as long
 * as it belongs to a container, and destroyed when the view controller is removed from the container (the view
 * controller itself might be retained elsewhere for caching purposes, though). All interactions with a child view 
 * controller must happen through the HLSContainerContent interface to guarantee proper status tracking and to 
 * ensure that the view is created when it is really needed.
 *
 * HLSContainerContent can only be used when implementing containers for which automatic view lifecycle event forwarding
 * has been disabled, i.e. for which the
 *    -[UIViewController automaticallyForwardAppearanceAndRotationMethodsToChildViewControllers]
 * method returns NO (a feature available as of iOS 5).
 * 
 * Designated initializer: initWithViewController:containerViewController:transitionStyle:duration:
 */
@interface HLSContainerContent : NSObject {
@private
    UIViewController *m_viewController;                         // The embedded view controller
    UIViewController *m_containerViewController;                // The container it is inserted into
    Class m_transitionClass;                                    // The transition animation class used when inserting the view controller
    NSTimeInterval m_duration;                                  // The transition animation duration
    HLSContainerStackView *m_containerStackView;                // The container stack view into which the view controller's view is inserted
    CGRect m_originalViewFrame;                                 // The view controller's view frame prior to insertion
    UIViewAutoresizing m_originalAutoresizingMask;              // The view controller's view autoresizing mask prior to insertion
    BOOL m_movingToParentViewController;
    BOOL m_movingFromParentViewController;
}

/**
 * Return the container of the specified class, in which a given view controller has been inserted, or nil if none
 */
+ (UIViewController *)containerViewControllerKindOfClass:(Class)containerViewControllerClass forViewController:(UIViewController *)viewController;

/**
 * Initialize a container content object. Expect the view controller to be managed (which is retained), the container 
 * in which it is inserted into (not retained), as well as the details of the transition animation with which it gets 
 * displayed. Use the reserved kAnimationTransitionDefaultDuration duration to use the default animation duration.
 */
- (id)initWithViewController:(UIViewController *)viewController
     containerViewController:(UIViewController *)containerViewController
             transitionClass:(Class)transitionClass
                    duration:(NSTimeInterval)duration;

/**
 * The attached view controller. If you need to access its view, do not use the -[UIViewController view] property
 * (this triggers lazy creation). Instead, use the view insertion methods below when you really need to instantiate 
 * the view (i.e. when building up the container view hierarchy), and the -[HLSContainerContent viewIfLoaded] accessor 
 * to access a view which you created this way.
 */
@property (nonatomic, readonly, retain) UIViewController *viewController;

/**
 * The container into which a view controller has been inserted
 */
@property (nonatomic, readonly, assign) UIViewController *containerViewController;

/**
 * The transition properties to be applied when the view controller's view gets displayed
 */
@property (nonatomic, readonly, assign) Class transitionClass;
@property (nonatomic, readonly, assign) NSTimeInterval duration;

/**
 * Return YES iff the view controller has been added to a container
 */
@property (nonatomic, readonly, assign, getter=isAddedToContainerView) BOOL addedToContainerView;

/**
 * Instantiate (if not already) and add the view controller's view as subview of the view where a container displays
 * its contents (an instance of HLSContainerStackView). A view container can manage several separate container views
 *
 * The index starts at 0 and cannot be greater than [containerView.subviews count]. The 'add' method
 * is equivalent to the 'insert' method with index = [containerView.subviews count]. 
 * 
 * The frame of a view controller's view is automatically adjusted to match the container view bounds. This matches the
 * usual behavior of built-in view controller containers (UINavigationController, UITabBarController)
 */
- (void)addAsSubviewIntoContainerStackView:(HLSContainerStackView *)stackView;
- (void)insertAsSubviewIntoContainerStackView:(HLSContainerStackView *)stackView atIndex:(NSUInteger)index;

/**
 * Return the view controller's view if it has been added to a container view, nil otherwise. This does not perform
 * view creation (use the addAsSubviewIntoContainerView: or insertAsSubviewIntoContainerView: methods for this
 * purpose), forcing you to create the view when you actually need it
 */
- (UIView *)viewIfLoaded;

/**
 * Release all view and view-related resources. This also forwards the viewDidUnload message to the underlying view 
 * controller
 */
- (void)releaseViews;

/**
 * Forward the corresponding view lifecycle events to the view controller, ensuring that forwarding occurs only if
 * the view controller current lifecycle phase is coherent
 *
 * Remark: No methods have been provided for viewDidLoad (which is called automatically when the view has been loaded)
 *         and viewDidUnload (which container implementations must not call directly; use the releaseViews method above)
 */
- (void)viewWillAppear:(BOOL)animated movingToParentViewController:(BOOL)movingToParentViewController;
- (void)viewDidAppear:(BOOL)animated movingToParentViewController:(BOOL)movingToParentViewController;
- (void)viewWillDisappear:(BOOL)animated movingFromParentViewController:(BOOL)movingFromParentViewController;
- (void)viewDidDisappear:(BOOL)animated movingFromParentViewController:(BOOL)movingFromParentViewController;

/**
 * Forward the corresponding view rotation events to the view controller
 *
 * Remark: No methods have been provided for the deprecated 2-step rotation methods
 */
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation;
- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration;
- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration;
- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation;

@end
