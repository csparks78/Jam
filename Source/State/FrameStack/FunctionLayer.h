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
#include "BaseLayer.h"
#include "Equation/Statement.h"
#include "Equation/StmtParser.h"
#include "FunctionStateObject.h"
#include "State/FrameStack/GridLayer.h"

namespace Jam::Editor::State
{
    class FunctionLayer final : public BaseLayer
    {
    private:
        Vec2F          _origin{0.f, 0.f};
        Axis           _axis;
        Eq::StmtParser _parser;
        Eq::Statement  _stmt;
        VInt           _xLoc{JtNpos};
        String         _text;

        FunctionObjectArray _array;
        FunctionObjectArray _expr;

        Vec2F eval(R32 i0, const Eq::SymbolArray& sym);

        bool resizeEvent(const Vec2I& oldSize) override;

        bool injectVec2FImpl(const FrameStackCode& code,
                             const Vec2F&          size) override;

        void render(RenderContext& canvas) override;
        void renderExpression(RenderContext& canvas, I32 i0);

    public:
        FunctionLayer();
        ~FunctionLayer() override;

        void setAxis(const Axis& units);

        void setOrigin(const Vec2F& origin);

        const String& getText() const;

        bool update() override;

        VariableStateObject*   createVariable();
        ExpressionStateObject* createExpression();

        void removeVariable(VariableStateObject* vso);
        void removeExpression(ExpressionStateObject* eso);

        const FunctionObjectArray& objects() const;
    };

    inline void FunctionLayer::setAxis(const Axis& units)
    {
        _axis.set(units);
    }

    inline void FunctionLayer::setOrigin(const Vec2F& origin)
    {
        _origin = origin;
    }

    inline const FunctionObjectArray& FunctionLayer::objects() const
    {
        return _array;
    }

    inline const String& FunctionLayer::getText() const
    {
        return _text;
    }

}  // namespace Jam::Editor::State