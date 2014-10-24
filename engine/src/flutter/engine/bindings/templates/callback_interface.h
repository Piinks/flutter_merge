// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file has been auto-generated by {{code_generator}}. DO NOT MODIFY!

#ifndef {{v8_class}}_h
#define {{v8_class}}_h

{% filter conditional(conditional_string) %}
{% for filename in header_includes %}
#include "{{filename}}"
{% endfor %}

namespace blink {

class {{v8_class}} final : public {{cpp_class}}, public ActiveDOMCallback {
public:
    static PassOwnPtrWillBeRawPtr<{{v8_class}}> create(v8::Handle<v8::Function> callback, ScriptState* scriptState)
    {
        return adoptPtrWillBeNoop(new {{v8_class}}(callback, scriptState));
    }

    virtual ~{{v8_class}}();

{% for method in methods %}
    virtual {{method.cpp_type}} {{method.name}}({{method.argument_declarations | join(', ')}}) override;
{% endfor %}
private:
    {{v8_class}}(v8::Handle<v8::Function>, ScriptState*);

    ScopedPersistent<v8::Function> m_callback;
    RefPtr<ScriptState> m_scriptState;
};

}
{% endfilter %}
#endif // {{v8_class}}_h
