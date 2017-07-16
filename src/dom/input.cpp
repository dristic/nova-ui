// Copyright 2017 Dan Ristic

#include "src/dom/input.h"

#include "src/core.h"
#include "src/document.h"
#include "src/rendering.h"

namespace Chimera {

Input::Input(Document& document)
    : Element("input", document)
    , value{""}
    , mFrames{0}
    { }

Input::~Input() { }

bool Input::handleEvent(Event* event) {
    if (event->type == EventType::Input) {
        if (mDocument->focusManager.focusedElement == this) {
            auto textInputEvent = dynamic_cast<TextInputEvent*>(event);
            value += textInputEvent->value;

            TextInputEvent dispatchEvent{value};
            dispatch(EventType::Change, dynamic_cast<Event*>(&dispatchEvent));
        }
    } else if (event->type == EventType::Key) {
        if (mDocument->focusManager.focusedElement == this) {
            auto keyEvent = dynamic_cast<KeyEvent*>(event);

            switch (keyEvent->key) {
            case KeyType::Backspace:
            {
                if (value != "")
                {
                    value.pop_back();
                }

                TextInputEvent dispatchEvent{value};
                dispatch(EventType::Change, dynamic_cast<Event*>(&dispatchEvent));

                break;
            }
            default:
                break;
            }
        }
    } else if (event->type == EventType::MouseDown) {
        auto mouseDownEvent = dynamic_cast<MouseDownEvent*>(event);

        if (layout.rect.contains(float(mouseDownEvent->x), float(mouseDownEvent->y))) {
            mDocument->focusManager.focusElement(this);
            return false;
        }
    } else if (event->type == EventType::MouseMove) {
        MouseMoveEvent* mouseMoveEvent = dynamic_cast<MouseMoveEvent*>(event);
        if (layout.rect.contains(float(mouseMoveEvent->x), float(mouseMoveEvent->y))) {
            mDocument->setCursorType(CursorType::IBeam);
        }
    }

    return true;
}

void Input::render(DrawData* data) {
    style.update(this, mDocument->animationController);

    if (layout.intrinsicHeight < style.fontSize)
    {
        layout.intrinsicHeight = style.fontSize;
    }

    // Do layout
    layout.layout();

    scissor.x = layout.rect.x;
    scissor.y = layout.rect.y;
    scissor.width = layout.rect.width;
    scissor.height = layout.rect.height;

    // Draw border rectangle
    if (style.borderWidth > 0) {
        Rect borderRect;
        borderRect.x = layout.rect.x - style.borderWidth;
        borderRect.y = layout.rect.y - style.borderWidth;
        borderRect.width = layout.rect.width + (style.borderWidth * 2);
        borderRect.height = layout.rect.height + (style.borderWidth * 2);

        data->addRectFilled(borderRect, style.borderColor);
    }

    // Draw normal background
    data->addRectFilled(layout.rect, style.backgroundColor);

    bool isFocused = mDocument->focusManager.focusedElement == this;

    // Draw current text
    if (value != "" || placeholder != "") {
        auto renderText = value != "" ? value : placeholder;

        auto color = value != "" ? style.color : Color::fromRGBA(150, 150, 150, 0.6f);

        float textWidth = data->measureText(renderText, style.fontFamily, style.fontSize);
        float pad = textWidth > layout.rect.width ?
            layout.rect.width - textWidth : 0;

        Rect position{
            layout.rect.x,
            layout.rect.y + ((layout.rect.height - style.fontSize) / 2),
            layout.rect.width,
            layout.rect.height
        };

        data->addText(position, renderText, style.fontFamily, style.fontSize, layout.rect, style.color);
    }

    // Draw input line
    if (isFocused) {
        mFrames++;
        if (mFrames < 15) {
            float textWidth = data->measureText(value, style.fontFamily, style.fontSize);

            Rect inputLine{
                layout.rect.x + textWidth,
                layout.rect.y + ((layout.rect.height - style.fontSize) / 2),
                1,
                float(style.fontSize)
            };

            data->addRectFilled(inputLine, style.color);
        } else {
            if (mFrames > 30) {
                mFrames = 0;
            }
        }
    }
}

}  // namespace Chimera
