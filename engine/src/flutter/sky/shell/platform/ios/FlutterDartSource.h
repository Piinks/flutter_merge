// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_SHELL_PLATFORM_IOS_FLUTTERDARTSOURCE_H_
#define SKY_SHELL_PLATFORM_IOS_FLUTTERDARTSOURCE_H_

#import <Foundation/Foundation.h>

typedef void (^ValidationResult)(BOOL result, NSString* message);

@interface FlutterDartSource : NSObject

@property(nonatomic, readonly) NSURL* dartMain;
@property(nonatomic, readonly) NSURL* packageRoot;
@property(nonatomic, readonly) NSURL* flxArchive;

- (instancetype)initWithDartMain:(NSURL*)dartMain
                     packageRoot:(NSURL*)packageRoot
                      flxArchive:(NSURL*)flxArchive NS_DESIGNATED_INITIALIZER;

- (void)validate:(ValidationResult)result;

@end

#endif  // SKY_SHELL_PLATFORM_IOS_FLUTTERDARTSOURCE_H_
