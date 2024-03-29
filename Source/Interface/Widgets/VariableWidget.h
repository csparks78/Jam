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
#pragma once
#include <QWidget>
#include "IconButton.h"
#include "Math/Vec2F.h"
#include "Utils/String.h"

namespace Jam::Editor
{
    struct VariableStepData;

    namespace State
    {
        class VariableStateObject;
    }

    class VariableStepWidget;
    class R32Widget;

    class VariableWidget final : public QWidget
    {
        Q_OBJECT
    signals:
        void textEntered(const String& text);
        void wantsToDelete();

    private:
        R32Widget*          _line{nullptr};
        IconButton*         _del{nullptr};
        VariableStepWidget* _edit{nullptr};

        State::VariableStateObject* _state{nullptr};

    public:
        explicit VariableWidget(State::VariableStateObject* obj, QWidget* parent = nullptr);

        void setName(const String& name) const;

        void setRange(const Vec2F& range) const;

    private:
        void construct();

        void connectSignals();

        void stepDataChanged(const VariableStepData& data) const;

        void onValueChange(const R32& data) const;

        void onDelete();
    };

}  // namespace Jam::Editor
