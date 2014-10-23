# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'includes': [
        'custom/custom.gypi',
    ],
    'variables': {
        'bindings_core_v8_dir': '.',
        'bindings_core_v8_files': [
            '<@(bindings_core_v8_custom_files)',
            'ActiveDOMCallback.cpp',
            'ActiveDOMCallback.h',
            'ArrayValue.cpp',
            'ArrayValue.h',
            'BindingSecurity.cpp',
            'BindingSecurity.h',
            'CallbackPromiseAdapter.h',
            'CustomElementBinding.cpp',
            'CustomElementBinding.h',
            'CustomElementConstructorBuilder.cpp',
            'CustomElementConstructorBuilder.h',
            'CustomElementWrapper.cpp',
            'CustomElementWrapper.h',
            'DOMDataStore.cpp',
            'DOMDataStore.h',
            'DOMWrapperMap.h',
            'DOMWrapperWorld.cpp',
            'DOMWrapperWorld.h',
            'Dictionary.cpp',
            'Dictionary.h',
            'DictionaryHelperForBindings.h',
            'DictionaryHelperForCore.cpp',
            'ExceptionMessages.cpp',
            'ExceptionMessages.h',
            'ExceptionState.cpp',
            'ExceptionState.h',
            'ExceptionStatePlaceholder.cpp',
            'ExceptionStatePlaceholder.h',
            'Nullable.h',
            'PostMessage.h',
            'RetainedDOMInfo.cpp',
            'RetainedDOMInfo.h',
            'RetainedObjectInfo.h',
            'ScheduledAction.cpp',
            'ScheduledAction.h',
            'ScopedPersistent.h',
            'ScriptCallStackFactory.cpp',
            'ScriptCallStackFactory.h',
            'ScriptController.cpp',
            'ScriptController.h',
            'ScriptEventListener.cpp',
            'ScriptEventListener.h',
            'ScriptFunction.cpp',
            'ScriptFunction.h',
            'ScriptFunctionCall.cpp',
            'ScriptFunctionCall.h',
            'ScriptGCEvent.cpp',
            'ScriptGCEvent.h',
            'ScriptHeapSnapshot.cpp',
            'ScriptHeapSnapshot.h',
            'ScriptPreprocessor.cpp',
            'ScriptPreprocessor.h',
            'ScriptProfiler.cpp',
            'ScriptProfiler.h',
            'ScriptPromise.cpp',
            'ScriptPromise.h',
            'ScriptPromiseResolver.cpp',
            'ScriptPromiseResolver.h',
            'ScriptPromiseProperties.h',
            'ScriptPromiseProperty.h',
            'ScriptPromisePropertyBase.cpp',
            'ScriptPromisePropertyBase.h',
            'ScriptRegexp.cpp',
            'ScriptRegexp.h',
            'ScriptSourceCode.h',
            'ScriptState.cpp',
            'ScriptState.h',
            'ScriptString.cpp',
            'ScriptString.h',
            'ScriptValue.cpp',
            'ScriptValue.h',
            'ScriptWrappable.cpp',
            'ScriptWrappable.h',
            'SerializedScriptValue.cpp',
            'SerializedScriptValue.h',
            'SharedPersistent.h',
            'V8AbstractEventListener.cpp',
            'V8AbstractEventListener.h',
            'V8Binding.cpp',
            'V8Binding.h',
            'V8BindingMacros.h',
            'V8CustomElementLifecycleCallbacks.cpp',
            'V8CustomElementLifecycleCallbacks.h',
            'V8DOMConfiguration.cpp',
            'V8DOMConfiguration.h',
            'V8DOMWrapper.cpp',
            'V8DOMWrapper.h',
            'V8ErrorHandler.cpp',
            'V8ErrorHandler.h',
            'V8EventListener.cpp',
            'V8EventListener.h',
            'V8EventListenerList.cpp',
            'V8EventListenerList.h',
            'V8GCController.cpp',
            'V8GCController.h',
            'V8GCForContextDispose.cpp',
            'V8GCForContextDispose.h',
            'V8HiddenValue.cpp',
            'V8HiddenValue.h',
            'V8Initializer.cpp',
            'V8Initializer.h',
            'V8LazyEventListener.cpp',
            'V8LazyEventListener.h',
            'V8MutationCallback.cpp',
            'V8MutationCallback.h',
            'V8ObjectConstructor.cpp',
            'V8ObjectConstructor.h',
            'V8PerContextData.cpp',
            'V8PerContextData.h',
            'V8PerIsolateData.cpp',
            'V8PerIsolateData.h',
            'V8RecursionScope.cpp',
            'V8RecursionScope.h',
            'V8ScriptRunner.cpp',
            'V8ScriptRunner.h',
            'V8StringResource.cpp',
            'V8StringResource.h',
            'V8ThrowException.cpp',
            'V8ThrowException.h',
            'V8ValueCache.cpp',
            'V8ValueCache.h',
            'WindowProxy.cpp',
            'WindowProxy.h',
            'WrapperTypeInfo.h',
        ],
    },
}
