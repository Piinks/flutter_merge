// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

export const baseUri = getBaseURI();

function getBaseURI() {
  const base = document.querySelector("base");
  return (base && base.getAttribute("href")) || "";
}

export function joinPathSegments(...segments) {
  return segments.filter((segment) => !!segment).map((segment, i) => {
    if (i === 0) {
      return stripRightSlashes(segment);
    } else {
      return stripLeftSlashes(stripRightSlashes(segment));
    }
  }).filter(x => x.length).join("/")
}

function stripLeftSlashes(s) {
  let i = 0;
  while (i < s.length) {
    if (s.charCodeAt(i) !== "/") {
      break;
    }
    i++;
  }
  return s.substring(i);
}

function stripRightSlashes(s) {
  let i = s.length;
  while (i > 0) {
    if (s.charCodeAt(i - 1) !== "/") {
      break;
    }
    i--;
  }
  return s.substring(0, i);
}

/**
 * Calculates the proper base URL for CanvasKit/Skwasm assets.
 * 
 * @param {import("./types").FlutterConfiguration} config
 * @param {import("./types").BuildConfig} buildConfig
 */
export function getCanvaskitBaseUrl(config, buildConfig) {
  if (config.canvasKitBaseUrl) {
    return config.canvasKitBaseUrl;
  }
  if (buildConfig.engineRevision && !buildConfig.useLocalCanvasKit) {
    return joinPathSegments("https://www.gstatic.com/flutter-canvaskit", buildConfig.engineRevision);
  }
  return "/canvaskit";
}
