
#include "FloatBezierCurve.h"
#include "Arduino.h"
#define COMPRESSED_SIGNAL_MAX 8192.0f

FloatBezierCurve::FloatBezierCurve(
    //float controllPoints[8]
    //unsigned long startTimeInMs,
    //long duration,
    //int startY,
    //long startControlX,
    //int startControlY,
    //int endY,
    //long endControlX,
    //int endControlY
    )
{
    /*this->curveStartTimeInMs = controllPoints[0];
    this->duration = controllPoints[1];

    this->startY = controllPoints[2];
    this->startControlX = controllPoints[3];
    this->startControlY = controllPoints[4];

    this->endY = controllPoints[5];
    this->endControlX = controllPoints[6];
    this->endControlY = controllPoints[7];*/
    /*this->curveStartTimeInMs = startTimeInMs;
    this->duration = duration;

    this->startY = startY;
    this->startControlX = startControlX;
    this->startControlY = startControlY;

    this->endY = endY;
    this->endControlX = endControlX;
    this->endControlY = endControlY;*/
}
void FloatBezierCurve::setControllPoints(float controllPoints[8])
{
    this->curveStartTimeInMs = controllPoints[0];
    this->duration = controllPoints[1];

    this->startY = controllPoints[2];
    this->startControlX = controllPoints[3];
    this->startControlY = controllPoints[4];

    this->endY = controllPoints[5];
    this->endControlX = controllPoints[6];
    this->endControlY = controllPoints[7];
}

float FloatBezierCurve::getValue(unsigned long currentTimeMs)
{
    return Evaluate(currentTimeMs - curveStartTimeInMs) ;
}

unsigned long FloatBezierCurve::getEndTimeMs()
{
    return curveStartTimeInMs + (unsigned long)duration;
}

unsigned long FloatBezierCurve::getStartTimeMs()
{
    return curveStartTimeInMs;
}

float FloatBezierCurve::Evaluate(unsigned long x)
{
    float uLower = 0;
    float uUpper = 1;
    float u = lastU;

    while (true)
    {
        float evaluatedX = 0, evaluatedY = 0;
        EvaluateForUX(u, evaluatedX);

        if (abs(evaluatedX - x) < 1)
        {
            EvaluateForUY(u, evaluatedY);
            lastU = u;
            return evaluatedY;
        }
        else if (evaluatedX > x)
        {
            uUpper = u;
        }
        else if (evaluatedX < x)
        {
            uLower = u;
        }

        u = (uUpper - uLower) / 2 + uLower;
    }
}

void FloatBezierCurve::EvaluateForUX(float u, float &outx)
{
    float p11x = (float)startControlX * u;
    float p12x = lerp((float)startControlX, (float)(duration + endControlX), u);
    float p13x = lerp((float)(duration + endControlX), (float)duration, u);
    float p21x = lerp(p11x, p12x, u);
    float p22x = lerp(p12x, p13x, u);

    outx = lerp(p21x, p22x, u);
}

void FloatBezierCurve::EvaluateForUY(float u, float &outy)
{
    float p11y = lerp((float)startY, (float)(startY + startControlY), u);
    float p12y = lerp((float)(startY + startControlY), (float)(endY + endControlY), u);
    float p13y = lerp((float)(endY + endControlY), (float)endY, u);
    float p21y = lerp(p11y, p12y, u);
    float p22y = lerp(p12y, p13y, u);

    outy = lerp(p21y, p22y, u);
}

float FloatBezierCurve::lerp(float start, float end, float u)
{
    float result = ((end - start) * u) + start;
    return result;
}

bool FloatBezierCurve::isInProgress(unsigned long currentTimeMs)
{
    return currentTimeMs >= getStartTimeMs() && currentTimeMs <= getEndTimeMs();
}
    
float FloatBezierCurve::getStartMovement()
{
    return startY ;
}
float FloatBezierCurve::getEndMovement()
{
    return endY ;
}
