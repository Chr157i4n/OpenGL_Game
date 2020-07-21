#pragma once
#include "UI_Element.h"

class UI_Element_ProgressBar : public UI_Element
{
public:
    UI_Element_ProgressBar(int x, int y, int w, int h, float value=0, uint64 lifespan=0, bool isDebugInfo=false);

    virtual void drawUI_Element();

private:
	float value = 0;
public:
    float getValue() const;
    void setValue(float value);

};

