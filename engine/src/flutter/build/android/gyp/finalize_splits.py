#!/usr/bin/env python
#
# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Signs and zipaligns split APKs.

This script is require only by GYP (not GN).
"""

import optparse
import sys

import finalize_apk

def main():
  parser = optparse.OptionParser()
  parser.add_option('--zipalign-path', help='Path to the zipalign tool.')
  parser.add_option('--resource-packaged-apk-path',
      help='Base path to input .ap_s.')
  parser.add_option('--base-output-path',
      help='Path to output .apk, minus extension.')
  parser.add_option('--key-path', help='Path to keystore for signing.')
  parser.add_option('--key-passwd', help='Keystore password')
  parser.add_option('--key-name', help='Keystore name')
  parser.add_option('--densities',
      help='Comma separated list of densities finalize.')

  options, _ = parser.parse_args()
  options.load_library_from_zip = 0

  if options.densities:
    for density in options.densities.split(','):
      options.unsigned_apk_path = ("%s-%s" %
          (options.resource_packaged_apk_path, density))
      options.final_apk_path = ("%s-density-%s.apk" %
          (options.base_output_path, density))
      finalize_apk.FinalizeApk(options)
  else:
    raise Exception('Language splits not yet implemented')


if __name__ == '__main__':
  sys.exit(main())
