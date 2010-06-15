/*
    Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005, 2010 Rob Buis <buis@kde.org>
    Copyright (C) Research In Motion Limited 2010. All rights reserved.

    Based on khtml code by:
    Copyright (C) 1999 Antti Koivisto (koivisto@kde.org)
    Copyright (C) 1999-2003 Lars Knoll (knoll@kde.org)
    Copyright (C) 2002-2003 Dirk Mueller (mueller@kde.org)
    Copyright (C) 2002 Apple Computer, Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#if ENABLE(SVG)
#include "SVGRenderStyle.h"

#include "CSSPrimitiveValue.h"
#include "CSSValueList.h"
#include "IntRect.h"
#include "NodeRenderStyle.h"
#include "SVGStyledElement.h"

using namespace std;

namespace WebCore {

SVGRenderStyle::SVGRenderStyle()
{
    static SVGRenderStyle* defaultStyle = new SVGRenderStyle(CreateDefault);

    fill = defaultStyle->fill;
    stroke = defaultStyle->stroke;
    text = defaultStyle->text;
    stops = defaultStyle->stops;
    misc = defaultStyle->misc;
    shadowSVG = defaultStyle->shadowSVG;
    inheritedResources = defaultStyle->inheritedResources;
    resources = defaultStyle->resources;

    setBitDefaults();
}

SVGRenderStyle::SVGRenderStyle(CreateDefaultType)
{
    setBitDefaults();

    fill.init();
    stroke.init();
    text.init();
    stops.init();
    misc.init();
    shadowSVG.init();
    inheritedResources.init();
    resources.init();
}

SVGRenderStyle::SVGRenderStyle(const SVGRenderStyle& other)
    : RefCounted<SVGRenderStyle>()
{
    fill = other.fill;
    stroke = other.stroke;
    text = other.text;
    stops = other.stops;
    misc = other.misc;
    shadowSVG = other.shadowSVG;
    inheritedResources = other.inheritedResources;
    resources = other.resources;

    svg_inherited_flags = other.svg_inherited_flags;
    svg_noninherited_flags = other.svg_noninherited_flags;
}

SVGRenderStyle::~SVGRenderStyle()
{
}

bool SVGRenderStyle::operator==(const SVGRenderStyle& other) const
{
    return fill == other.fill
        && stroke == other.stroke
        && text == other.text
        && stops == other.stops
        && misc == other.misc
        && shadowSVG == other.shadowSVG
        && inheritedResources == other.inheritedResources
        && resources == other.resources
        && svg_inherited_flags == other.svg_inherited_flags
        && svg_noninherited_flags == other.svg_noninherited_flags;
}

bool SVGRenderStyle::inheritedNotEqual(const SVGRenderStyle* other) const
{
    return fill != other->fill
        || stroke != other->stroke
        || text != other->text
        || inheritedResources != other->inheritedResources
        || svg_inherited_flags != other->svg_inherited_flags;
}

void SVGRenderStyle::inheritFrom(const SVGRenderStyle* svgInheritParent)
{
    if (!svgInheritParent)
        return;

    fill = svgInheritParent->fill;
    stroke = svgInheritParent->stroke;
    text = svgInheritParent->text;
    inheritedResources = svgInheritParent->inheritedResources;

    svg_inherited_flags = svgInheritParent->svg_inherited_flags;
}

float SVGRenderStyle::cssPrimitiveToLength(const RenderObject* item, CSSValue* value, float defaultValue)
{
    CSSPrimitiveValue* primitive = static_cast<CSSPrimitiveValue*>(value);

    unsigned short cssType = (primitive ? primitive->primitiveType() : (unsigned short) CSSPrimitiveValue::CSS_UNKNOWN);
    if (!(cssType > CSSPrimitiveValue::CSS_UNKNOWN && cssType <= CSSPrimitiveValue::CSS_PC))
        return defaultValue;

    if (cssType == CSSPrimitiveValue::CSS_PERCENTAGE) {
        SVGStyledElement* element = static_cast<SVGStyledElement*>(item->node());
        SVGElement* viewportElement = (element ? element->viewportElement() : 0);
        if (viewportElement) {
            float result = primitive->getFloatValue() / 100.0f;
            return SVGLength::PercentageOfViewport(result, element, LengthModeOther);
        }
    }

    return primitive->computeLengthFloat(const_cast<RenderStyle*>(item->style()), item->document()->documentElement()->renderStyle());
}

static void getSVGShadowExtent(ShadowData* shadow, float& top, float& right, float& bottom, float& left)
{
    top = 0.0f;
    right = 0.0f;
    bottom = 0.0f;
    left = 0.0f;

    float blurAndSpread = shadow->blur() + shadow->spread();

    top = min(top, shadow->y() - blurAndSpread);
    right = max(right, shadow->x() + blurAndSpread);
    bottom = max(bottom, shadow->y() + blurAndSpread);
    left = min(left, shadow->x() - blurAndSpread);
}

void SVGRenderStyle::inflateForShadow(IntRect& repaintRect) const
{
    ShadowData* svgShadow = shadow();
    if (!svgShadow)
        return;

    FloatRect repaintFloatRect = FloatRect(repaintRect);
    inflateForShadow(repaintFloatRect);
    repaintRect = enclosingIntRect(repaintFloatRect);
}

void SVGRenderStyle::inflateForShadow(FloatRect& repaintRect) const
{
    ShadowData* svgShadow = shadow();
    if (!svgShadow)
        return;

    float shadowTop;
    float shadowRight;
    float shadowBottom;
    float shadowLeft;
    getSVGShadowExtent(svgShadow, shadowTop, shadowRight, shadowBottom, shadowLeft);

    repaintRect.move(shadowLeft, shadowTop);
    repaintRect.setSize(repaintRect.size() + FloatSize(shadowRight - shadowLeft, shadowBottom - shadowTop));
}

}

#endif // ENABLE(SVG)
