/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "InspectorClientImpl.h"

#include "DOMWindow.h"
#include "FloatRect.h"
#include "NotImplemented.h"
#include "Page.h"
#include "WebDevToolsAgentImpl.h"
#include "WebDevToolsMessageData.h"
#include "WebRect.h"
#include "WebURL.h"
#include "WebURLRequest.h"
#include "WebViewClient.h"
#include "WebViewImpl.h"
#include <wtf/Vector.h>

using namespace WebCore;

namespace WebKit {

InspectorClientImpl::InspectorClientImpl(WebViewImpl* webView)
    : m_inspectedWebView(webView)
{
    ASSERT(m_inspectedWebView);
}

InspectorClientImpl::~InspectorClientImpl()
{
}

void InspectorClientImpl::inspectorDestroyed()
{
    // Our lifetime is bound to the WebViewImpl.
}

void InspectorClientImpl::openInspectorFrontend(InspectorController*)
{
}

static void invalidateNodeBoundingRect(WebViewImpl* webView)
{
    // FIXME: Is it important to just invalidate the rect of the node region
    // given that this is not on a critical codepath?  In order to do so, we'd
    // have to take scrolling into account.
    const WebSize& size = webView->size();
    WebRect damagedRect(0, 0, size.width, size.height);
    if (webView->client())
        webView->client()->didInvalidateRect(damagedRect);
}

void InspectorClientImpl::highlight(Node* node)
{
    // InspectorController does the actually tracking of the highlighted node
    // and the drawing of the highlight. Here we just make sure to invalidate
    // the rects of the old and new nodes.
    hideHighlight();
}

void InspectorClientImpl::hideHighlight()
{
    // FIXME: able to invalidate a smaller rect.
    invalidateNodeBoundingRect(m_inspectedWebView);
}

void InspectorClientImpl::populateSetting(const String& key, String* value)
{
    WebString string;
    m_inspectedWebView->inspectorSetting(key, &string);
    *value = string;
}

void InspectorClientImpl::storeSetting(const String& key, const String& value)
{
    m_inspectedWebView->setInspectorSetting(key, value);
}

bool InspectorClientImpl::sendMessageToFrontend(const WebCore::String& message)
{
    WebDevToolsAgentImpl* devToolsAgent = static_cast<WebDevToolsAgentImpl*>(m_inspectedWebView->devToolsAgent());
    if (!devToolsAgent)
        return false;

    WebVector<WebString> arguments(size_t(1));
    arguments[0] = message;
    WebDevToolsMessageData data;
    data.className = "ToolsAgentDelegate";
    data.methodName = "dispatchOnClient";
    data.arguments.swap(arguments);
    devToolsAgent->sendRpcMessage(data);
    return true;
}

} // namespace WebKit
