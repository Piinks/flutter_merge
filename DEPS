# This file is automatically processed to create .DEPS.git which is the file
# that gclient uses under git.
#
# See http://code.google.com/p/chromium/wiki/UsingGit
#
# To test manually, run:
#   python tools/deps2git/deps2git.py -o .DEPS.git -w <gclientdir>
# where <gcliendir> is the absolute path to the directory containing the
# .gclient file (the parent of 'src').
#
# Then commit .DEPS.git locally (gclient doesn't like dirty trees) and run
#   gclient sync..
# Verify the thing happened you wanted. Then revert your .DEPS.git change
# DO NOT CHECK IN CHANGES TO .DEPS.git upstream. It will be automatically
# updated by a bot when you modify this one.
#
# When adding a new dependency, please update the top-level .gitignore file
# to list the dependency's destination directory.

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  'swiftshader_git': 'https://swiftshader.googlesource.com',
  'dart_git': 'https://dart.googlesource.com',
  'fuchsia_git': 'https://fuchsia.googlesource.com',
  'github_git': 'https://github.com',
  'skia_git': 'https://skia.googlesource.com',
  # OCMock is for testing only so there is no google clone
  'ocmock_git': 'https://github.com/erikdoe/ocmock.git',
  'skia_revision': 'f124108e23259cd309dcafea1efbd22460b4ee84',

  # When updating the Dart revision, ensure that all entries that are
  # dependencies of Dart are also updated to match the entries in the
  # Dart SDK's DEPS file for that revision of Dart. The DEPS file for
  # Dart is: https://github.com/dart-lang/sdk/blob/master/DEPS.
  # You can use //tools/dart/create_updated_flutter_deps.py to produce
  # updated revision list of existing dependencies.
  'dart_revision': '8233c4763a9c602f8e4c53b5a4ac70c8e6501f6f',

  # WARNING: DO NOT EDIT MANUALLY
  # The lines between blank lines above and below are generated by a script. See create_updated_flutter_deps.py
  'dart_boringssl_gen_rev': '429ccb1877f7987a6f3988228bc2440e61293499',
  'dart_boringssl_rev': '4dfd5af70191b068aebe567b8e29ce108cee85ce',
  'dart_clock_rev': 'a494269254ba978e7ef8f192c5f7fec3fc05b9d3',
  'dart_collection_rev': 'e4bb038ce2d8e66fb15818aa40685c68d53692ab',
  'dart_dart_style_tag': '1.3.10',
  'dart_http_retry_tag': '0.1.1',
  'dart_http_throttle_tag': '1.0.2',
  'dart_intl_tag': '0.17.0-nullsafety',
  'dart_linter_tag': '0.1.127',
  'dart_oauth2_tag': '1.6.0',
  'dart_protobuf_rev': '0d03fd588df69e9863e2a2efc0059dee8f18d5b2',
  'dart_pub_rev': '8ea96121dbb3f762f63e641d49141927dc30aac1',
  'dart_resource_rev': '6b79867d0becf5395e5819a75720963b8298e9a7',
  'dart_root_certificates_rev': '7e5ec82c99677a2e5b95ce296c4d68b0d3378ed8',
  'dart_shelf_packages_handler_tag': '2.0.0',
  'dart_shelf_proxy_tag': '0.1.0+7',
  'dart_shelf_static_rev': '779a6e99b320ce9ed4ff6b88bd0cdc40ea5c62c4',
  'dart_sse_tag': '9a486d058a17e880afa9cc1c3c0dd8bad726bcbc',
  'dart_stack_trace_tag': '6788afc61875079b71b3d1c3e65aeaa6a25cbc2f',
  'dart_stagehand_tag': 'v3.3.11',
  'dart_stream_channel_tag': 'd7251e61253ec389ee6e045ee1042311bced8f1d',
  'dart_typed_data_tag': 'f94fc57b8e8c0e4fe4ff6cfd8290b94af52d3719',
  'dart_usage_tag': '16fbfd90c58f16e016a295a880bc722d2547d2c9',
  'dart_watcher_rev': '64e254eba16f56d41f10d72c0b1cb24e130e1f8b',

  'ocmock_tag': 'v3.7.1',

  # Build bot tooling for iOS
  'ios_tools_revision': '69b7c1b160e7107a6a98d948363772dc9caea46f',

  # Checkout Android dependencies only on platforms where we build for Android targets.
  'download_android_deps': 'host_os == "mac" or host_os == "linux"',

  # Checkout Windows dependencies only if we are building on Windows.
  'download_windows_deps' : 'host_os == "win"',

  # Checkout Linux dependencies only when building on Linux.
  'download_linux_deps': 'host_os == "linux"',

  # An LLVM backend needs LLVM binaries and headers. To avoid build time
  # increases we can use prebuilts. We don't want to download this on every
  # CQ/CI bot nor do we want the average Dart developer to incur that cost.
  # So by default we will not download prebuilts. This varible is needed in
  # the flutter engine to ensure that Dart gn has access to it as well.
  "checkout_llvm": False,
}

gclient_gn_args_file = 'src/third_party/dart/build/config/gclient_args.gni'
gclient_gn_args = [
  'checkout_llvm'
]

# Only these hosts are allowed for dependencies in this DEPS file.
# If you need to add a new host, contact chrome infrastructure team.
allowed_hosts = [
  'chromium.googlesource.com',
  'fuchsia.googlesource.com',
  'github.com',
  'skia.googlesource.com',
]

deps = {
  'src': 'https://github.com/flutter/buildroot.git' + '@' + '4985b584a430ea3fda9433dccafa5047531e0974',

   # Fuchsia compatibility
   #
   # The dependencies in this section should match the layout in the Fuchsia gn
   # build. Eventually, we'll manage these dependencies together with Fuchsia
   # and not have to specific specific hashes.

  'src/third_party/benchmark':
   Var('fuchsia_git') + '/third_party/benchmark' + '@' + 'a779ffce872b4c811beef482e18bd0b63626aa42',

  'src/third_party/googletest':
   Var('fuchsia_git') + '/third_party/googletest' + '@' + '3fef9015bfe7617d57806cd7c73a653b28559fae',

  'src/third_party/rapidjson':
   Var('fuchsia_git') + '/third_party/rapidjson' + '@' + 'ef3564c5c8824989393b87df25355baf35ff544b',

  'src/third_party/harfbuzz':
   Var('fuchsia_git') + '/third_party/harfbuzz' + '@' + '9c55f4cf3313d68d68f68419e7a57fb0771fcf49',

  'src/third_party/libcxx':
   Var('fuchsia_git') + '/third_party/libcxx' + '@' + '7524ef50093a376f334a62a7e5cebf5d238d4c99',

  'src/third_party/libcxxabi':
   Var('fuchsia_git') + '/third_party/libcxxabi' + '@' + '74d1e602c76350f0760bf6907910e4f3a4fccffe',

  'src/third_party/glfw':
   Var('fuchsia_git') + '/third_party/glfw' + '@' + '999f3556fdd80983b10051746264489f2cb1ef16',

   # Chromium-style
   #
   # As part of integrating with Fuchsia, we should eventually remove all these
   # Chromium-style dependencies.

  'src/ios_tools':
   Var('chromium_git') + '/chromium/src/ios.git' + '@' + Var('ios_tools_revision'),

  'src/third_party/icu':
   Var('chromium_git') + '/chromium/deps/icu.git' + '@' + 'c2a4cae149aae7fd30c4cbe3cf1b30df03b386f1',

  'src/third_party/khronos':
   Var('chromium_git') + '/chromium/src/third_party/khronos.git' + '@' + '7122230e90547962e0f0c627f62eeed3c701f275',

  'src/third_party/boringssl':
   Var('github_git') + '/dart-lang/boringssl_gen.git' + '@' + Var('dart_boringssl_gen_rev'),

  'src/third_party/boringssl/src':
   'https://boringssl.googlesource.com/boringssl.git' + '@' + Var('dart_boringssl_rev'),

  'src/third_party/dart':
   Var('dart_git') + '/sdk.git' + '@' + Var('dart_revision'),

  # WARNING: Unused Dart dependencies in the list below till "WARNING:" marker are removed automatically - see create_updated_flutter_deps.py.

  'src/third_party/dart/third_party/pkg/args':
   Var('dart_git') + '/args.git@6921e2f63796368bbe65a64ad943df84932dff55',

  'src/third_party/dart/third_party/pkg/async':
   Var('dart_git') + '/async.git@695b3ac280f107c84adf7488743abfdfaaeea68f',

  'src/third_party/dart/third_party/pkg/bazel_worker':
   Var('dart_git') + '/bazel_worker.git@060c55a933d39798681a4f533b161b81dc48d77e',

  'src/third_party/dart/third_party/pkg/boolean_selector':
   Var('dart_git') + '/boolean_selector.git@665e6921ab246569420376f827bff4585dff0b14',

  'src/third_party/dart/third_party/pkg/charcode':
   Var('dart_git') + '/charcode.git@bcd8a12c315b7a83390e4865ad847ecd9344cba2',

  'src/third_party/dart/third_party/pkg/cli_util':
   Var('dart_git') + '/cli_util.git@50cc840b146615899e97b892578848401b2028d5',

  'src/third_party/dart/third_party/pkg/clock':
   Var('dart_git') + '/clock.git' + '@' + Var('dart_clock_rev'),

  'src/third_party/dart/third_party/pkg/collection':
   Var('dart_git') + '/collection.git' + '@' + Var('dart_collection_rev'),

  'src/third_party/dart/third_party/pkg/convert':
   Var('dart_git') + '/convert.git@6513985a1b1ea8a0b987fbef699250ce2cdc3cca',

  'src/third_party/dart/third_party/pkg/crypto':
   Var('dart_git') + '/crypto.git@c89a5be0375875fe7ff71625fa2b79f5a421f06d',

  'src/third_party/dart/third_party/pkg/csslib':
   Var('dart_git') + '/csslib.git@6f77b3dcee957d3e2d5083f666221a220e9ed1f1',

  'src/third_party/dart/third_party/pkg/dart2js_info':
   Var('dart_git') + '/dart2js_info.git@e0acfeb5affdf94c53067e68bd836adf589628fd',

  'src/third_party/dart/third_party/pkg/dartdoc':
   Var('dart_git') + '/dartdoc.git@a1d86f2c992f4660ddcc09b27733396e92765d2a',

  'src/third_party/dart/third_party/pkg/ffi':
   Var('dart_git') + '/ffi.git@a5d4232cd38562c75a3ed847baa340e399538028',

  'src/third_party/dart/third_party/pkg/file':
   Var('dart_git') + '/external/github.com/google/file.dart/@0e09370f581ab6388d46fda4cdab66638c0171a1',

  'src/third_party/dart/third_party/pkg/fixnum':
   Var('dart_git') + '/fixnum.git@16d3890c6dc82ca629659da1934e412292508bba',

  'src/third_party/dart/third_party/pkg/glob':
   Var('dart_git') + '/glob.git@7c0ef8d4fa086f6b185c4dd724b700e7d7ad8f79',

  'src/third_party/dart/third_party/pkg/html':
   Var('dart_git') + '/html.git@22f17e97fedeacaa1e945cf84d8016284eed33a6',

  'src/third_party/dart/third_party/pkg/http':
   Var('dart_git') + '/http.git@1617b728fc48f64fb0ed7dc16078c03adcc64179',

  'src/third_party/dart/third_party/pkg/http_multi_server':
   Var('dart_git') + '/http_multi_server.git@e8c8be7f15b4fb50757ff5bf29766721fbe24fe4',

  'src/third_party/dart/third_party/pkg/http_parser':
   Var('dart_git') + '/http_parser.git@5dd4d16693242049dfb43b5efa429fedbf932e98',

  'src/third_party/dart/third_party/pkg/http_retry':
   Var('dart_git') + '/http_retry.git' + '@' + Var('dart_http_retry_tag'),

  'src/third_party/dart/third_party/pkg/http_throttle':
   Var('dart_git') + '/http_throttle.git' + '@' + Var('dart_http_throttle_tag'),

  'src/third_party/dart/third_party/pkg/intl':
   Var('dart_git') + '/intl.git' + '@' + Var('dart_intl_tag'),

  'src/third_party/dart/third_party/pkg/json_rpc_2':
   Var('dart_git') + '/json_rpc_2.git@b8dfe403fd8528fd14399dee3a6527b55802dd4d',

  'src/third_party/dart/third_party/pkg/linter':
   Var('dart_git') + '/linter.git' + '@' + Var('dart_linter_tag'),

  'src/third_party/dart/third_party/pkg/logging':
   Var('dart_git') + '/logging.git@e2f633b543ef89c54688554b15ca3d7e425b86a2',

  'src/third_party/dart/third_party/pkg/markdown':
   Var('dart_git') + '/markdown.git@6f89681d59541ddb1cf3a58efbdaa2304ffc3f51',

  'src/third_party/dart/third_party/pkg/matcher':
   Var('dart_git') + '/matcher.git@9cae8faa7868bf3a88a7ba45eb0bd128e66ac515',

  'src/third_party/dart/third_party/pkg/mime':
   Var('dart_git') + '/mime.git@c931f4bed87221beaece356494b43731445ce7b8',

  'src/third_party/dart/third_party/pkg/mockito':
   Var('dart_git') + '/mockito.git@d39ac507483b9891165e422ec98d9fb480037c8b',

  'src/third_party/dart/third_party/pkg/mustache':
   Var('dart_git') + '/external/github.com/xxgreg/mustache@664737ecad027e6b96d0d1e627257efa0e46fcb1',

  'src/third_party/dart/third_party/pkg/oauth2':
   Var('dart_git') + '/oauth2.git' + '@' + Var('dart_oauth2_tag'),

  'src/third_party/dart/third_party/pkg/path':
   Var('dart_git') + '/path.git@62ecd5a78ffe5734d14ed0df76d20309084cd04a',

  'src/third_party/dart/third_party/pkg/pedantic':
   Var('dart_git') + '/pedantic.git@a884ea2db943b8756cc94385990bd750aec06928',

  'src/third_party/dart/third_party/pkg/pool':
   Var('dart_git') + '/pool.git@7abe634002a1ba8a0928eded086062f1307ccfae',

  'src/third_party/dart/third_party/pkg/protobuf':
   Var('dart_git') + '/protobuf.git' + '@' + Var('dart_protobuf_rev'),

  'src/third_party/dart/third_party/pkg/pub':
   Var('dart_git') + '/pub.git' + '@' + Var('dart_pub_rev'),

  'src/third_party/dart/third_party/pkg/pub_semver':
   Var('dart_git') + '/pub_semver.git@10569a1e867e909cf5db201f73118020453e5db8',

  'src/third_party/dart/third_party/pkg/resource':
   Var('dart_git') + '/resource.git' + '@' + Var('dart_resource_rev'),

  'src/third_party/dart/third_party/pkg/shelf':
   Var('dart_git') + '/shelf.git@fa5afaa38bd51dedeeaa25b7bfd8822cabbcc57f',

  'src/third_party/dart/third_party/pkg/shelf_packages_handler':
   Var('dart_git') + '/shelf_packages_handler.git' + '@' + Var('dart_shelf_packages_handler_tag'),

  'src/third_party/dart/third_party/pkg/shelf_proxy':
   Var('dart_git') + '/shelf_proxy.git' + '@' + Var('dart_shelf_proxy_tag'),

  'src/third_party/dart/third_party/pkg/shelf_static':
   Var('dart_git') + '/shelf_static.git' + '@' + Var('dart_shelf_static_rev'),

  'src/third_party/dart/third_party/pkg/shelf_web_socket':
   Var('dart_git') + '/shelf_web_socket.git@8050a55b16faa5052a3e5d7dcdc170c59b6644f2',

  'src/third_party/dart/third_party/pkg/source_map_stack_trace':
   Var('dart_git') + '/source_map_stack_trace.git@1c3026f69d9771acf2f8c176a1ab750463309cce',

  'src/third_party/dart/third_party/pkg/source_maps':
   Var('dart_git') + '/source_maps.git@53eb92ccfe6e64924054f83038a534b959b12b3e',

  'src/third_party/dart/third_party/pkg/source_span':
   Var('dart_git') + '/source_span.git@49ff31eabebed0da0ae6634124f8ba5c6fbf57f1',

  'src/third_party/dart/third_party/pkg/sse':
   Var('dart_git') + '/sse.git' + '@' + Var('dart_sse_tag'),

  'src/third_party/dart/third_party/pkg/stack_trace':
   Var('dart_git') + '/stack_trace.git' + '@' + Var('dart_stack_trace_tag'),

  'src/third_party/dart/third_party/pkg/stagehand':
   Var('dart_git') + '/stagehand.git' + '@' + Var('dart_stagehand_tag'),

  'src/third_party/dart/third_party/pkg/stream_channel':
   Var('dart_git') + '/stream_channel.git' + '@' + Var('dart_stream_channel_tag'),

  'src/third_party/dart/third_party/pkg/string_scanner':
   Var('dart_git') + '/string_scanner.git@1b63e6e5db5933d7be0a45da6e1129fe00262734',

  'src/third_party/dart/third_party/pkg/term_glyph':
   Var('dart_git') + '/term_glyph.git@6a0f9b6fb645ba75e7a00a4e20072678327a0347',

  'src/third_party/dart/third_party/pkg/test':
   Var('dart_git') + '/test.git@e37a93bbeae23b215972d1659ac865d71287ff6a',

  'src/third_party/dart/third_party/pkg/test_reflective_loader':
   Var('dart_git') + '/test_reflective_loader.git@b76ae201ab9c6f3b90643958965e7cc215a38e9b',

  'src/third_party/dart/third_party/pkg/typed_data':
   Var('dart_git') + '/typed_data.git' + '@' + Var('dart_typed_data_tag'),

  'src/third_party/dart/third_party/pkg/usage':
   Var('dart_git') + '/usage.git' + '@' + Var('dart_usage_tag'),

  'src/third_party/dart/third_party/pkg/watcher':
   Var('dart_git') + '/watcher.git' + '@' + Var('dart_watcher_rev'),

  'src/third_party/dart/third_party/pkg/web_socket_channel':
   Var('dart_git') + '/web_socket_channel.git@490061ef0e22d3c8460ad2802f9948219365ad6b',

  'src/third_party/dart/third_party/pkg/yaml':
   Var('dart_git') + '/yaml.git@cca02c9e4f6826d62644901ed65c6d72b90a0713',

  'src/third_party/dart/third_party/pkg_tested/dart_style':
   Var('dart_git') + '/dart_style.git' + '@' + Var('dart_dart_style_tag'),

  'src/third_party/dart/third_party/pkg_tested/package_config':
   Var('dart_git') + '/package_config.git@9c586d04bd26fef01215fd10e7ab96a3050cfa64',

  'src/third_party/dart/tools/sdks':
   {'packages': [{'version': 'version:2.12.0-0.0.dev', 'package': 'dart/dart-sdk/${{platform}}'}], 'dep_type': 'cipd'},

  # WARNING: end of dart dependencies list that is cleaned up automatically - see create_updated_flutter_deps.py.

  'src/third_party/colorama/src':
   Var('chromium_git') + '/external/colorama.git' + '@' + '799604a1041e9b3bc5d2789ecbd7e8db2e18e6b8',

  'src/third_party/freetype2':
   Var('fuchsia_git') + '/third_party/freetype2' + '@' + 'edab12c07ac05d1185616688f338b1ad15936796',

  'src/third_party/root_certificates':
   Var('dart_git') + '/root_certificates.git' + '@' + Var('dart_root_certificates_rev'),

  'src/third_party/skia':
   Var('skia_git') + '/skia.git' + '@' +  Var('skia_revision'),

  'src/third_party/ocmock':
   Var('ocmock_git') + '@' +  Var('ocmock_tag'),

  'src/third_party/libjpeg-turbo':
   Var('fuchsia_git') + '/third_party/libjpeg-turbo' + '@' + '0fb821f3b2e570b2783a94ccd9a2fb1f4916ae9f',

  'src/third_party/libwebp':
   Var('chromium_git') + '/webm/libwebp.git' + '@' + '0.6.0',

  'src/third_party/wuffs':
   Var('skia_git') + '/external/github.com/google/wuffs.git' + '@' +  '00cc8a50aa0c86b6bcb37e9ece8fb100047cc17b',

  'src/third_party/fontconfig/src':
   Var('chromium_git') + '/external/fontconfig.git' + '@' + 'c336b8471877371f0190ba06f7547c54e2b890ba',

  'src/third_party/gyp':
   Var('chromium_git') + '/external/gyp.git' + '@' + '4801a5331ae62da9769a327f11c4213d32fb0dad',

   # Headers for Vulkan 1.1
   'src/third_party/vulkan':
   Var('github_git') + '/KhronosGroup/Vulkan-Docs.git' + '@' + 'v1.1.91',

   'src/third_party/swiftshader':
   Var('swiftshader_git') + '/SwiftShader.git' + '@' + '5d1e8540407c138f47028d64684f3da599430aa4',

   'src/third_party/angle':
   Var('github_git') + '/google/angle.git' + '@' + 'ed90c9917b1b7fc6def319184b10609c11b2bd84',

   'src/third_party/angle/third_party/vulkan-headers/src':
   Var('fuchsia_git') + '/third_party/Vulkan-Headers.git' + '@' + '5de4e8fab88ef0bd6994d9ddbcc864e3179b9e79',

   'src/third_party/abseil-cpp':
   Var('chromium_git') + '/chromium/src/third_party/abseil-cpp.git' + '@' + '2d8c1340f0350828f1287c4eaeebefcf317bcfc9',

  'src/third_party/pkg/when':
   Var('dart_git') + '/when.git' + '@' + '0.2.0',

   'src/third_party/android_tools/ndk': {
     'packages': [
       {
        'package': 'flutter/android/ndk/${{platform}}',
        'version': 'version:r21.0.6113669'
       }
     ],
     'condition': 'download_android_deps',
     'dep_type': 'cipd',
   },

   'src/third_party/android_tools/google-java-format': {
     'packages': [
       {
        'package': 'flutter/android/google-java-format',
        'version': 'version:1.7-1'
       }
     ],
     # We want to be able to format these as part of CI, and the CI step that
     # checks formatting runs without downloading the rest of the Android build
     # tooling. Therefore unlike all the other Android-related tools, we want to
     # download this every time.
     'dep_type': 'cipd',
   },

  'src/third_party/android_tools/sdk/build-tools': {
     'packages': [
       {
        'package': 'flutter/android/sdk/build-tools/${{platform}}',
        'version': 'version:30.0.1'
       }
     ],
     'condition': 'download_android_deps',
     'dep_type': 'cipd',
   },

  'src/third_party/android_tools/sdk/platform-tools': {
     'packages': [
       {
        'package': 'flutter/android/sdk/platform-tools/${{platform}}',
        'version': 'version:30.0.4'
       }
     ],
     'condition': 'download_android_deps',
     'dep_type': 'cipd',
   },

  'src/third_party/android_tools/sdk/platforms': {
     'packages': [
       {
        'package': 'flutter/android/sdk/platforms',
        'version': 'version:30r3'
       }
     ],
     'condition': 'download_android_deps',
     'dep_type': 'cipd',
   },

  'src/third_party/android_tools/sdk/tools': {
     'packages': [
       {
        'package': 'flutter/android/sdk/tools/${{platform}}',
        'version': 'version:26.1.1'
       }
     ],
     'condition': 'download_android_deps',
     'dep_type': 'cipd',
   },

  'src/third_party/android_tools/sdk/licenses': {
     'packages': [
       {
        'package': 'flutter/android/sdk/licenses',
        'version': 'latest',
       }
     ],
    'condition': 'download_android_deps',
    'dep_type': 'cipd',
  },

  'src/third_party/android_embedding_dependencies': {
     'packages': [
       {
        'package': 'flutter/android/embedding_bundle',
        'version': 'last_updated:2020-09-11T17:57:41-0700'
       }
     ],
     'condition': 'download_android_deps',
     'dep_type': 'cipd',
   },

  'src/flutter/third_party/gn': {
    'packages': [
      {
        'package': 'gn/gn/${{platform}}',
        'version': 'git_revision:1e3fd10c5df6b704fc764ee388149e4f32862823'
      },
    ],
    'dep_type': 'cipd',
  },

  'src/buildtools/{host_os}-x64/clang': {
    'packages': [
      {
        'package': 'fuchsia/third_party/clang/${{platform}}',
        'version': 'git_revision:7e9747b50bcb1be28d4a3236571e8050835497a6'
      }
    ],
    'condition': 'host_os == "mac" or host_os == "linux"',
    'dep_type': 'cipd',
  },

  # TODO(fxb/4443): Remove this when Fuchsia can provide the Windows Clang Toolchain
  'src/third_party/llvm-build/Release+Asserts': {
    'packages': [
      {
        'package': 'flutter/clang/win-amd64',
        'version': 'git_revision:5ec206df8534d2dd8cb9217c3180e5ddba587393'
      }
    ],
    'condition': 'download_windows_deps',
    'dep_type': 'cipd',
  },

   # Get the SDK from https://chrome-infra-packages.appspot.com/p/fuchsia/sdk/core at the 'latest' tag
   # Get the toolchain from https://chrome-infra-packages.appspot.com/p/fuchsia/clang at the 'goma' tag

   'src/fuchsia/sdk/mac': {
     'packages': [
       {
        'package': 'fuchsia/sdk/core/mac-amd64',
        'version': 'L7-xj4YqzzT_c4Bb-WxklGGXMIeQE5tNUiJ4S3JnbpoC'
       }
     ],
     'condition': 'host_os == "mac"',
     'dep_type': 'cipd',
   },
   'src/fuchsia/toolchain/mac': {
     'packages': [
       {
        'package': 'fuchsia/clang/mac-amd64',
        'version': 'OzTZOKkICT0yD82Dbx0jvVn5hN5eOSi6ByVTDseE7i0C'
       }
     ],
     'condition': 'host_os == "mac"',
     'dep_type': 'cipd',
   },
   'src/fuchsia/sdk/linux': {
     'packages': [
       {
        'package': 'fuchsia/sdk/core/linux-amd64',
        'version': 'QniFAAjTThCxdu7j1R-UD2peFrF2Mx1wgVKNKLtDai8C'
       }
     ],
     'condition': 'host_os == "linux"',
     'dep_type': 'cipd',
   },
   'src/fuchsia/toolchain/linux': {
     'packages': [
       {
        'package': 'fuchsia/clang/linux-amd64',
        'version': 'OT6p30bQQhyCzRSy7xPsSbZ88J3PWOnneenkMZ0j7kIC'
       }
     ],
     'condition': 'host_os == "linux"',
     'dep_type': 'cipd',
   },
}

hooks = [
  {
    # This clobbers when necessary (based on get_landmines.py). It must be the
    # first hook so that other things that get/generate into the output
    # directory will not subsequently be clobbered.
    'name': 'landmines',
    'pattern': '.',
    'action': [
        'python',
        'src/build/landmines.py',
    ],
  },
  {
    # Update the Windows toolchain if necessary.
    'name': 'win_toolchain',
    'condition': 'download_windows_deps',
    'pattern': '.',
    'action': ['python', 'src/build/vs_toolchain.py', 'update'],
  },
  {
    'name': 'generate_package_files',
    'pattern': '.',
    'cwd': 'src/',
    'action': ['python', 'flutter/tools/generate_package_files.py'],
  },
  {
    # Ensure that we don't accidentally reference any .pyc files whose
    # corresponding .py files have already been deleted.
    'name': 'remove_stale_pyc_files',
    'pattern': 'src/tools/.*\\.py',
    'action': [
        'python',
        'src/tools/remove_stale_pyc_files.py',
        'src/tools',
    ],
  },
  {
    'name': '7zip',
    'pattern': '.',
    'condition': 'download_windows_deps',
    'action': [
      'download_from_google_storage',
      '--no_auth',
      '--no_resume',
      '--bucket',
      'dart-dependencies',
      '--platform=win32',
      '--extract',
      '-s',
      'src/third_party/dart/third_party/7zip.tar.gz.sha1',
    ],
  },
  {
    'name': 'dia_dll',
    'pattern': '.',
    'condition': 'download_windows_deps',
    'action': [
      'python',
      'src/flutter/tools/dia_dll.py',
    ],
  },
  {
    'name': 'linux_sysroot_x64',
    'pattern': '.',
    'condition': 'download_linux_deps',
    'action': [
      'python',
      'src/build/linux/sysroot_scripts/install-sysroot.py',
      '--arch=x64'],
  },
  {
    'name': 'linux_sysroot_arm64',
    'pattern': '.',
    'condition': 'download_linux_deps',
    'action': [
      'python',
      'src/build/linux/sysroot_scripts/install-sysroot.py',
      '--arch=arm64'],
  },
  {
    'name': 'dart package config',
    'pattern': '.',
    'action': [
      'python',
      'src/flutter/tools/run_third_party_dart.py',
    ]
  }
]
