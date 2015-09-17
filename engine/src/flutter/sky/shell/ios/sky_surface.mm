// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "sky_surface.h"

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

#include "base/time/time.h"
#include "mojo/public/cpp/bindings/interface_request.h"
#include "sky/services/engine/input_event.mojom.h"
#include "sky/shell/mac/platform_view_mac.h"
#include "sky/shell/shell_view.h"
#include "sky/shell/shell.h"
#include "sky/shell/ui_delegate.h"

#ifndef NDEBUG
#include "document_watcher.h"
#endif

static inline sky::EventType EventTypeFromUITouchPhase(UITouchPhase phase) {
  switch (phase) {
    case UITouchPhaseBegan:
      return sky::EVENT_TYPE_POINTER_DOWN;
    case UITouchPhaseMoved:
    case UITouchPhaseStationary:
      // There is no EVENT_TYPE_POINTER_STATIONARY. So we just pass a move type
      // with the same coordinates
      return sky::EVENT_TYPE_POINTER_MOVE;
    case UITouchPhaseEnded:
      return sky::EVENT_TYPE_POINTER_UP;
    case UITouchPhaseCancelled:
      return sky::EVENT_TYPE_POINTER_CANCEL;
  }

  return sky::EVENT_TYPE_UNKNOWN;
}

static inline int64 InputEventTimestampFromNSTimeInterval(
    NSTimeInterval interval) {
  return base::TimeDelta::FromSecondsD(interval).InMilliseconds();
}

@implementation SkySurface {
  BOOL _platformViewInitialized;
  CGPoint _lastScrollTranslation;

  sky::SkyEnginePtr _sky_engine;
  scoped_ptr<sky::shell::ShellView> _shell_view;

#ifndef NDEBUG
  DocumentWatcher *_document_watcher;
#endif
}

-(instancetype) initWithShellView:(sky::shell::ShellView *) shellView {
  self = [super init];
  if (self) {
    _shell_view.reset(shellView);
    self.multipleTouchEnabled = YES;
  }
  return self;
}

- (gfx::AcceleratedWidget)acceleratedWidget {
  return (gfx::AcceleratedWidget)self.layer;
}

- (void)layoutSubviews {
  [super layoutSubviews];

  [self configureLayerDefaults];

  [self setupPlatformViewIfNecessary];

  CGSize size = self.bounds.size;
  CGFloat scale = [UIScreen mainScreen].scale;

  sky::ViewportMetricsPtr metrics = sky::ViewportMetrics::New();
  metrics->physical_width = size.width * scale;
  metrics->physical_height = size.height * scale;
  metrics->device_pixel_ratio = scale;
  metrics->padding_top =
      [UIApplication sharedApplication].statusBarFrame.size.height;

  [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleLightContent animated:NO];
  _sky_engine->OnViewportMetricsChanged(metrics.Pass());
}

- (void)configureLayerDefaults {
  CAEAGLLayer* layer = reinterpret_cast<CAEAGLLayer*>(self.layer);
  layer.allowsGroupOpacity = YES;
  layer.opaque = YES;
  CGFloat screenScale = [UIScreen mainScreen].scale;
  layer.contentsScale = screenScale;
  // Note: shouldRasterize is still NO. This is just a defensive measure
  layer.rasterizationScale = screenScale;
}

- (void)setupPlatformViewIfNecessary {
  if (_platformViewInitialized) {
    return;
  }

  _platformViewInitialized = YES;

  [self notifySurfaceCreation];
  [self connectToEngineAndLoad];
}

- (sky::shell::PlatformViewMac*)platformView {
  auto view = static_cast<sky::shell::PlatformViewMac*>(_shell_view->view());
  DCHECK(view);
  return view;
}

- (void)notifySurfaceCreation {
  self.platformView->SurfaceCreated(self.acceleratedWidget);
}

- (NSString*)skyInitialLoadURL {
  NSUserDefaults *standardDefaults = [NSUserDefaults standardUserDefaults];
  NSString *target = [standardDefaults stringForKey:@"target"];
  NSString *server = [standardDefaults stringForKey:@"server"];
  if (server && target) {
    return [NSString stringWithFormat:@"http://%@/%@", server, target];
  }
  return [NSBundle mainBundle].infoDictionary[@"org.domokit.sky.load_url"];
}

- (NSString*)skyInitialBundleURL {
  NSString *skyxBundlePath = [[NSBundle mainBundle] pathForResource:@"app" ofType:@"skyx"];
#ifndef NDEBUG
  NSFileManager *fileManager = [NSFileManager defaultManager];
  NSError *error = nil;
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [paths objectAtIndex:0];
  NSString *skyxDocsPath = [documentsDirectory stringByAppendingPathComponent:@"app.skyx"];

  // Write an empty file to help identify the correct simulator app by its bundle id. See sky_tool for its use.
  NSString *bundleIDPath = [documentsDirectory stringByAppendingPathComponent:[[NSBundle mainBundle] bundleIdentifier]];
  NSData *data = [[NSData alloc] initWithBytes:"" length:0];
  if (![data writeToFile:bundleIDPath options:NSDataWritingAtomic error:&error]) {
      NSLog(@"Couldn't write the bundle id file %@: auto reloading on the iOS simulator won't work\n%@", bundleIDPath, error);
  }

  if (skyxBundlePath != nil && [fileManager fileExistsAtPath:skyxDocsPath] == NO) {
    if ([fileManager copyItemAtPath:skyxBundlePath toPath:skyxDocsPath error:&error]) {
      return skyxDocsPath;
    }
    NSLog(@"Error encountered copying app.skyx from the Bundle to the Documents directory. Dynamic reloading will not be possible. %@", error);
    return skyxBundlePath;
  }
  return skyxDocsPath;
#endif
  return skyxBundlePath;
}

- (void)connectToEngineAndLoad {
  auto interface_request = mojo::GetProxy(&_sky_engine);
  self.platformView->ConnectToEngine(interface_request.Pass());

  NSString *endpoint = self.skyInitialBundleURL;
  if (endpoint.length > 0) {
#ifndef NDEBUG
    _document_watcher = [[DocumentWatcher alloc] initWithDocumentPath:endpoint callbackBlock:^{
      mojo::String string(endpoint.UTF8String);
      _sky_engine->RunFromBundle(string);
    }];
#endif
    // Load from bundle
    mojo::String string(endpoint.UTF8String);
    _sky_engine->RunFromBundle(string);
    return;
  }

  endpoint = self.skyInitialLoadURL;
  if (endpoint.length > 0) {
    // Load from URL
    mojo::String string(endpoint.UTF8String);
    _sky_engine->RunFromNetwork(string);
    return;
  }
}

- (void)notifySurfaceDestruction {
  self.platformView->SurfaceDestroyed();
}

#ifndef NDEBUG
- (void)didMoveToWindow {
  if (self.window == nil) {
    [_document_watcher cancel];
    [_document_watcher release];
    _document_watcher = nil;
  }
}
#endif

#pragma mark - UIResponder overrides for raw touches

- (void)dispatchTouches:(NSSet*)touches phase:(UITouchPhase)phase {
  auto eventType = EventTypeFromUITouchPhase(phase);
  const CGFloat scale = [UIScreen mainScreen].scale;

  for (UITouch* touch in touches) {
    auto input = sky::InputEvent::New();
    input->type = eventType;
    input->time_stamp = InputEventTimestampFromNSTimeInterval(touch.timestamp);

    input->pointer_data = sky::PointerData::New();
    input->pointer_data->kind = sky::POINTER_KIND_TOUCH;

    #define LOWER_32(x) (*((int32_t *) &x))
    input->pointer_data->pointer = LOWER_32(touch);
    #undef LOWER_32

    CGPoint windowCoordinates = [touch locationInView:nil];

    input->pointer_data->x = windowCoordinates.x * scale;
    input->pointer_data->y = windowCoordinates.y * scale;

    _sky_engine->OnInputEvent(input.Pass());
  }
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event {
  [self dispatchTouches:touches phase:UITouchPhaseBegan];
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
  [self dispatchTouches:touches phase:UITouchPhaseMoved];
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event {
  [self dispatchTouches:touches phase:UITouchPhaseEnded];
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event {
  [self dispatchTouches:touches phase:UITouchPhaseCancelled];
}

#pragma mark - Misc.

+ (Class)layerClass {
  return [CAEAGLLayer class];
}

- (void)dealloc {
  [self notifySurfaceDestruction];
  [super dealloc];
}

@end

void SaveFrameToSkPicture() {
  @autoreleasepool {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                         NSUserDomainMask, YES);
    NSString* basePath = paths.firstObject;

    if (basePath.length == 0) {
      return;
    }

    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"HH_mm_ss"];
    NSString* dateString = [formatter stringFromDate:[NSDate date]];
    [formatter release];

    NSString* path =
        [NSString stringWithFormat:@"%@/%@.trace.skp", basePath, dateString];

    base::FilePath filePath(path.UTF8String);
    sky::shell::Shell::Shared().tracing_controller().SaveFrameToSkPicture(
        filePath);
  }
}
