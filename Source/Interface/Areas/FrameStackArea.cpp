/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include "FrameStackArea.h"
#include <QLabel>
#include <QVBoxLayout>
#include "AreaType.h"
#include "FrameStackAreaContent.h"
#include "Interface/Area/Area.h"
#include "Interface/Area/AreaToolbar.h"
#include "Interface/Constants.h"
#include "Interface/Events/EventTypes.h"
#include "Interface/Extensions.h"
#include "Interface/Widgets/IconButton.h"

namespace Jam::Editor
{
    FrameStackArea::FrameStackArea(AreaCreator* creator, QWidget* parent) :
        Area(creator, AtFrameStack, parent)
    {
        construct();
    }

    FrameStackArea::~FrameStackArea() = default;

    void FrameStackArea::construct()
    {
        View::widgetDefaults(this);
        View::applyColorRoles(this, Const::AreaRole);

        const auto layout = new QVBoxLayout();
        View::layoutDefaults(layout);
        AreaToolBar* tools = toolbar();

        const auto home = IconButton::createToolButton(Icons::Home);
        tools->addWidget(
            home,
            0,
            Qt::AlignRight);

        _private = new FrameStackAreaContent();
        layout->addWidget(tools);
        layout->addWidget(_private, 1);

        connect(home, &QPushButton::clicked, this, [=]
                { _private->resetAxis(); });

        setLayout(layout);
    }

    bool FrameStackArea::event(QEvent* event)
    {
        switch ((AreaEvents)event->type())
        {
        case ProjectOpened:
            _private->resetAxis();
            break;
        case ProjectClosed:
        case LayerSelect:
        case SplitEvent:
        case MergeLeftEvent:
        case MergeRightEvent:
        case SwitchContentEvent:
            break;
        }
        return Area::event(event);
    }

}  // namespace Jam::Editor
