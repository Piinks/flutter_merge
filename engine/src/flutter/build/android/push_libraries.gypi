# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This file is meant to be included into an action to provide a rule that
# pushes stripped shared libraries to the attached Android device. This should
# only be used with the gyp_managed_install flag set.
#
# To use this, create a gyp target with the following form:
#  {
#    'actions': [
#      'variables': {
#        'ordered_libraries_file': 'file generated by write_ordered_libraries'
#        'strip_stamp': 'stamp from strip action to block on'
#        'libraries_source_dir': 'location where stripped libraries are stored'
#        'device_library_dir': 'location on the device where to put pushed libraries',
#        'push_stamp': 'file to touch when the action is complete'
#        'configuration_name': 'The build CONFIGURATION_NAME'
#      },
#      'includes': [ '../../build/android/push_libraries.gypi' ],
#    ],
#  },
#

{
  'action_name': 'push_libraries_<(_target_name)',
  'message': 'Pushing libraries to device for <(_target_name)',
  'inputs': [
    '<(DEPTH)/build/android/gyp/util/build_utils.py',
    '<(DEPTH)/build/android/gyp/util/md5_check.py',
    '<(DEPTH)/build/android/gyp/push_libraries.py',
    '<(strip_stamp)',
    '<(strip_additional_stamp)',
    '<(build_device_config_path)',
    '<(pack_relocations_stamp)',
  ],
  'outputs': [
    '<(push_stamp)',
  ],
  'action': [
    'python', '<(DEPTH)/build/android/gyp/push_libraries.py',
    '--build-device-configuration=<(build_device_config_path)',
    '--libraries-dir=<(libraries_source_dir)',
    '--device-dir=<(device_library_dir)',
    '--libraries=@FileArg(<(ordered_libraries_file):libraries)',
    '--stamp=<(push_stamp)',
    '--configuration-name=<(configuration_name)',
  ],
}
