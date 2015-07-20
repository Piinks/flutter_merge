#!/usr/bin/env python
# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import argparse
from datetime import datetime
import logging
import os
import shutil
import subprocess
import sys

# Generates the sky_sdk from the template at sky/sdk.

SKY_TOOLS_DIR = os.path.dirname(os.path.abspath(__file__))
SKY_DIR = os.path.dirname(SKY_TOOLS_DIR)
SRC_ROOT = os.path.dirname(SKY_DIR)

DEFAULT_REL_BUILD_DIR = os.path.join('out', 'android_Release')

def git_revision():
    return subprocess.check_output(['git', 'rev-parse', 'HEAD']).strip()


def ensure_dir_exists(path):
    if not os.path.exists(path):
        os.makedirs(path)


def copy(from_root, to_root, filter_func=None):
    assert os.path.exists(from_root), "%s does not exist!" % from_root
    if os.path.isfile(from_root):
        ensure_dir_exists(os.path.dirname(to_root))
        shutil.copy(from_root, to_root)
        return

    ensure_dir_exists(to_root)

    for root, dirs, files in os.walk(from_root):
        # filter_func expects paths not names, so wrap it to make them absolute.
        wrapped_filter = None
        if filter_func:
            wrapped_filter = lambda name: filter_func(os.path.join(root, name))

        for name in filter(wrapped_filter, files):
            from_path = os.path.join(root, name)
            root_rel_path = os.path.relpath(from_path, from_root)
            to_path = os.path.join(to_root, root_rel_path)
            to_dir = os.path.dirname(to_path)
            if not os.path.exists(to_dir):
                os.makedirs(to_dir)
            shutil.copy(from_path, to_path)

        dirs[:] = filter(wrapped_filter, dirs)


def confirm(prompt):
    response = raw_input('%s [N]|y: ' % prompt)
    return response and response.lower() == 'y'


def delete_all_non_hidden_files_in_directory(root, non_interactive=False):
    to_delete = [os.path.join(root, p)
        for p in os.listdir(root) if not p.startswith('.')]
    if not to_delete:
        return
    if not non_interactive:
        prompt = 'This will delete everything in %s:\n%s\nAre you sure?' % (
            root, '\n'.join(to_delete))
        if not confirm(prompt):
            print 'User aborted.'
            sys.exit(2)

    for path in to_delete:
        if os.path.isdir(path) and not os.path.islink(path):
            shutil.rmtree(path)
        else:
            os.remove(path)


def main():
    logging.basicConfig(level=logging.WARN)
    parser = argparse.ArgumentParser(description='Deploy a new sky_sdk.')
    parser.add_argument('sdk_root', type=str)
    parser.add_argument('--build-dir', action='store', type=str,
        default=os.path.join(SRC_ROOT, DEFAULT_REL_BUILD_DIR))
    parser.add_argument('--non-interactive', action='store_true')
    args = parser.parse_args()

    build_dir = os.path.abspath(args.build_dir)
    sdk_root = os.path.abspath(args.sdk_root)

    print 'Building SDK from %s into %s' % (build_dir, sdk_root)
    start_time = datetime.now()

    def sdk_path(rel_path):
        return os.path.join(sdk_root, rel_path)

    def src_path(rel_path):
        return os.path.join(SRC_ROOT, rel_path)

    ensure_dir_exists(sdk_root)
    # Manually clear sdk_root above to avoid deleting dot-files.
    delete_all_non_hidden_files_in_directory(sdk_root, args.non_interactive)

    copy(src_path('sky/sdk/README.md'), sdk_root)

    ensure_dir_exists(sdk_path('packages'))
    subprocess.check_call([
        'mojo/tools/prepare_pub_packages.py',
        '--out-dir', sdk_path('packages'),
        'out'
    ])

    with open(sdk_path('LICENSES.sky'), 'w') as license_file:
        subprocess.check_call([src_path('tools/licenses.py'), 'credits'],
            stdout=license_file)

    time_delta = datetime.now() - start_time
    print 'SDK built at %s in %ss' % (sdk_root, time_delta.total_seconds())


if __name__ == '__main__':
    main()
