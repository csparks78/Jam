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
#include "StringWidget.h"
#include <QBoxLayout>
#include <QLineEdit>
#include <QWidget>
#include "Interface/Areas/OutputArea.h"
#include "Interface/Constants.h"
#include "Interface/Extensions.h"

namespace Jam::Editor
{

    StringWidget::StringWidget(QWidget* parent) :
        QWidget(parent)
    {
        construct();
    }

    void StringWidget::construct()
    {
        View::applyColorRoles(this, QPalette::Base);
        setMinimumHeight(Const::ButtonHeight);

        const auto layout = new QHBoxLayout();
        View::layoutDefaults(layout);

        _line = new QLineEdit();
        View::lineEditDefaults(_line);

        layout->addWidget(_line);
        setLayout(layout);

        connectSignals();
    }

    void StringWidget::finished()
    {
        setText(_line->text().toStdString());
        emit editingFinished(_str);
    }

    void StringWidget::connectSignals()
    {
        connect(_line,
                &QLineEdit::editingFinished,
                this,
                &StringWidget::finished);
        connect(_line,
                &QLineEdit::returnPressed,
                this,
                &StringWidget::finished);
    }

    void StringWidget::setText(const String& str)
    {
        if (_str != str)
        {
            Su::filterAscii(_str, str);

            _line->setText(QString::fromStdString(_str));

            emit editingFinished(_str);
        }
    }

    void StringWidget::setReadOnly(bool v) const
    {
        _line->setReadOnly(v);
    }

    bool StringWidget::isReadOnly() const
    {
        return _line->isReadOnly();
    }

}  // namespace Jam::Editor
