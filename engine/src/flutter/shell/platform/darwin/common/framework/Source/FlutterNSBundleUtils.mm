// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <Foundation/Foundation.h>

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"

FLUTTER_ASSERT_ARC

NSBundle* FLTFrameworkBundleInternal(NSString* flutterFrameworkBundleID, NSURL* searchURL) {
  NSDirectoryEnumerator<NSURL*>* frameworkEnumerator = [NSFileManager.defaultManager
                 enumeratorAtURL:searchURL
      includingPropertiesForKeys:nil
                         options:NSDirectoryEnumerationSkipsSubdirectoryDescendants |
                                 NSDirectoryEnumerationSkipsHiddenFiles
                    // Skip directories where errors are encountered.
                    errorHandler:nil];

  for (NSURL* candidate in frameworkEnumerator) {
    NSBundle* flutterFrameworkBundle = [NSBundle bundleWithURL:candidate];
    if ([flutterFrameworkBundle.bundleIdentifier isEqualToString:flutterFrameworkBundleID]) {
      return flutterFrameworkBundle;
    }
  }
  return nil;
}

NSBundle* FLTGetApplicationBundle() {
  NSBundle* mainBundle = [NSBundle mainBundle];
  // App extension bundle is in <AppName>.app/PlugIns/Extension.appex.
  if ([mainBundle.bundleURL.pathExtension isEqualToString:@"appex"]) {
    // Up two levels.
    return [NSBundle bundleWithURL:mainBundle.bundleURL.URLByDeletingLastPathComponent
                                       .URLByDeletingLastPathComponent];
  }
  return mainBundle;
}

NSBundle* FLTFrameworkBundleWithIdentifier(NSString* flutterFrameworkBundleID) {
  NSBundle* appBundle = FLTGetApplicationBundle();
  NSBundle* flutterFrameworkBundle =
      FLTFrameworkBundleInternal(flutterFrameworkBundleID, appBundle.privateFrameworksURL);
  if (flutterFrameworkBundle == nil) {
    // Fallback to slow implementation.
    flutterFrameworkBundle = [NSBundle bundleWithIdentifier:flutterFrameworkBundleID];
  }
  if (flutterFrameworkBundle == nil) {
    flutterFrameworkBundle = [NSBundle mainBundle];
  }
  return flutterFrameworkBundle;
}

NSURL* FLTAssetsURLFromBundle(NSBundle* bundle) {
  NSString* assetsPathFromInfoPlist = [bundle objectForInfoDictionaryKey:@"FLTAssetsPath"];
  NSString* flutterAssetsPath = assetsPathFromInfoPlist ?: @"flutter_assets";
  NSURL* assets = [bundle URLForResource:flutterAssetsPath withExtension:nil];

  if ([assets checkResourceIsReachableAndReturnError:NULL]) {
    return assets;
  }
  return nil;
}
