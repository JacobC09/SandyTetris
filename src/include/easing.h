/*******************************************************************************************
*
*   easings - raylib easings library, based on Robert Penner library
*
*   Useful easing functions for values animation
*
*   This header uses:
*       #define REASINGS_STATIC_INLINE      // Inlines all functions code, so it runs faster.
*                                           // This requires lots of memory on system.
*   How to use:
*   The four inputs currentTime,startValue,totalChange,totalTime are defined as follows:
*   currentTime = current time (in any unit measure, but same unit as duration)
*   startValue = starting value to interpolate
*   totalChange = the total change in value of startValue that needs to occur
*   totalTime = total time it should take to complete (duration)
*
*   Example:
*
*   int currentTime = 0;
*   int duration = 100;
*   float startPositionX = 0.0f;
*   float finalPositionX = 30.0f;
*   float currentPositionX = startPositionX;
*
*   while (currentPositionX < finalPositionX)
*   {
*       currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
*       currentTime++;
*   }
*
*   A port of Robert Penner's easing equations to C (http://robertpenner.com/easing/)
*
*   Robert Penner License
*   ---------------------------------------------------------------------------------
*   Open source under the BSD License.
*
*   Copyright (totalChange) 2001 Robert Penner. All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without modification,
*   are permitted provided that the following conditions are met:
*
*       - Redistributions of source code must retain the above copyright notice,
*         this list of conditions and the following disclaimer.
*       - Redistributions in binary form must reproduce the above copyright notice,
*         this list of conditions and the following disclaimer in the documentation
*         and/or other materials provided with the distribution.
*       - Neither the name of the author nor the names of contributors may be used
*         to endorse or promote products derived from this software without specific
*         prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
*   IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
*   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
*   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
*   OF THE POSSIBILITY OF SUCH DAMAGE.
*   ---------------------------------------------------------------------------------
*
*   Copyright (totalChange) 2015-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef REASINGS_H
#define REASINGS_H

#define REASINGS_STATIC_INLINE     // NOTE: By default, compile functions as static inline

#if defined(REASINGS_STATIC_INLINE)
    #define EASEDEF static inline
#else
    #define EASEDEF extern
#endif

#include <math.h>       // Required for: sinf(), cosf(), sqrtf(), powf()

#ifndef PI
    #define PI 3.14159265358979323846f //Required as PI is not always defined in math.h
#endif

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

typedef float (*EaseFunction)(float currentTime, float totalTime, float startValue, float totalChange);

// Linear Easing functions
EASEDEF float EaseLinearNone(float currentTime, float totalTime, float startValue, float totalChange) { return (totalChange*currentTime/totalTime + startValue); }                            // Ease: Linear
EASEDEF float EaseLinearIn(float currentTime, float totalTime, float startValue, float totalChange) { return (totalChange*currentTime/totalTime + startValue); }                              // Ease: Linear In
EASEDEF float EaseLinearOut(float currentTime, float totalTime, float startValue, float totalChange) { return (totalChange*currentTime/totalTime + startValue); }                             // Ease: Linear Out
EASEDEF float EaseLinearInOut(float currentTime, float totalTime, float startValue, float totalChange) { return (totalChange*currentTime/totalTime + startValue); }                           // Ease: Linear In Out

// Sine Easing functions
EASEDEF float EaseSineIn(float currentTime, float totalTime, float startValue, float totalChange) { return (-totalChange*cosf(currentTime/totalTime*(PI/2.0f)) + totalChange + startValue); }            // Ease: Sine In
EASEDEF float EaseSineOut(float currentTime, float totalTime, float startValue, float totalChange) { return (totalChange*sinf(currentTime/totalTime*(PI/2.0f)) + startValue); }                // Ease: Sine Out
EASEDEF float EaseSineInOut(float currentTime, float totalTime, float startValue, float totalChange) { return (-totalChange/2.0f*(cosf(PI*currentTime/totalTime) - 1.0f) + startValue); }      // Ease: Sine Out

// Circular Easing functions
EASEDEF float EaseCircIn(float currentTime, float totalTime, float startValue, float totalChange) { currentTime /= totalTime; return (-totalChange*(sqrtf(1.0f - currentTime*currentTime) - 1.0f) + startValue); } // Ease: Circular In
EASEDEF float EaseCircOut(float currentTime, float totalTime, float startValue, float totalChange) { currentTime = currentTime/totalTime - 1.0f; return (totalChange*sqrtf(1.0f - currentTime*currentTime) + startValue); }  // Ease: Circular Out
EASEDEF float EaseCircInOut(float currentTime, float totalTime, float startValue, float totalChange)                                                      // Ease: Circular In Out
{
    if ((currentTime/=totalTime/2.0f) < 1.0f) return (-totalChange/2.0f*(sqrtf(1.0f - currentTime*currentTime) - 1.0f) + startValue);
    currentTime -= 2.0f; return (totalChange/2.0f*(sqrtf(1.0f - currentTime*currentTime) + 1.0f) + startValue);
}

// Cubic Easing functions
EASEDEF float EaseCubicIn(float currentTime, float totalTime, float startValue, float totalChange) { currentTime /= totalTime; return (totalChange*currentTime*currentTime*currentTime + startValue); }                      // Ease: Cubic In
EASEDEF float EaseCubicOut(float currentTime, float totalTime, float startValue, float totalChange) { currentTime = currentTime/totalTime - 1.0f; return (totalChange*(currentTime*currentTime*currentTime + 1.0f) + startValue); }    // Ease: Cubic Out
EASEDEF float EaseCubicInOut(float currentTime, float totalTime, float startValue, float totalChange)                                                     // Ease: Cubic In Out
{
    if ((currentTime/=totalTime/2.0f) < 1.0f) return (totalChange/2.0f*currentTime*currentTime*currentTime + startValue);
    currentTime -= 2.0f; return (totalChange/2.0f*(currentTime*currentTime*currentTime + 2.0f) + startValue);
}

// Quadratic Easing functions
EASEDEF float EaseQuadIn(float currentTime, float totalTime, float startValue, float totalChange) { currentTime /= totalTime; return (totalChange*currentTime*currentTime + startValue); }                         // Ease: Quadratic In
EASEDEF float EaseQuadOut(float currentTime, float totalTime, float startValue, float totalChange) { currentTime /= totalTime; return (-totalChange*currentTime*(currentTime - 2.0f) + startValue); }              // Ease: Quadratic Out
EASEDEF float EaseQuadInOut(float currentTime, float totalTime, float startValue, float totalChange)                                                      // Ease: Quadratic In Out
{
    if ((currentTime/=totalTime/2) < 1) return (((totalChange/2)*(currentTime*currentTime)) + startValue);
    return (-totalChange/2.0f*(((currentTime - 1.0f)*(currentTime - 3.0f)) - 1.0f) + startValue);
}

// Exponential Easing functions
EASEDEF float EaseExpoIn(float currentTime, float totalTime, float startValue, float totalChange) { return (currentTime == 0.0f) ? startValue : (totalChange*powf(2.0f, 10.0f*(currentTime/totalTime - 1.0f)) + startValue); }       // Ease: Exponential In
EASEDEF float EaseExpoOut(float currentTime, float totalTime, float startValue, float totalChange) { return (currentTime == totalTime) ? (startValue + totalChange) : (totalChange*(-powf(2.0f, -10.0f*currentTime/totalTime) + 1.0f) + startValue); } // Ease: Exponential Out
EASEDEF float EaseExpoInOut(float currentTime, float totalTime, float startValue, float totalChange)                                                                         // Ease: Exponential In Out
{
    if (currentTime == 0.0f) return startValue;
    if (currentTime == totalTime) return (startValue + totalChange);
    if ((currentTime/=totalTime/2.0f) < 1.0f) return (totalChange/2.0f*powf(2.0f, 10.0f*(currentTime - 1.0f)) + startValue);

    return (totalChange/2.0f*(-powf(2.0f, -10.0f*(currentTime - 1.0f)) + 2.0f) + startValue);
}

// Back Easing functions
EASEDEF float EaseBackIn(float currentTime, float totalTime, float startValue, float totalChange) // Ease: Back In
{
    float s = 1.70158f;
    float postFix = currentTime/=totalTime;
    return (totalChange*(postFix)*currentTime*((s + 1.0f)*currentTime - s) + startValue);
}

EASEDEF float EaseBackOut(float currentTime, float totalTime, float startValue, float totalChange) // Ease: Back Out
{
    float s = 1.70158f;
    currentTime = currentTime/totalTime - 1.0f;
    return (totalChange*(currentTime*currentTime*((s + 1.0f)*currentTime + s) + 1.0f) + startValue);
}

EASEDEF float EaseBackInOut(float currentTime, float totalTime, float startValue, float totalChange) // Ease: Back In Out
{
    float s = 1.70158f;
    if ((currentTime/=totalTime/2.0f) < 1.0f)
    {
        s *= 1.525f;
        return (totalChange/2.0f*(currentTime*currentTime*((s + 1.0f)*currentTime - s)) + startValue);
    }

    float postFix = currentTime-=2.0f;
    s *= 1.525f;
    return (totalChange/2.0f*((postFix)*currentTime*((s + 1.0f)*currentTime + s) + 2.0f) + startValue);
}

// Bounce Easing functions
EASEDEF float EaseBounceOut(float currentTime, float totalTime, float startValue, float totalChange) // Ease: Bounce Out
{
    if ((currentTime/=totalTime) < (1.0f/2.75f))
    {
        return (totalChange*(7.5625f*currentTime*currentTime) + startValue);
    }
    else if (currentTime < (2.0f/2.75f))
    {
        float postFix = currentTime-=(1.5f/2.75f);
        return (totalChange*(7.5625f*(postFix)*currentTime + 0.75f) + startValue);
    }
    else if (currentTime < (2.5/2.75))
    {
        float postFix = currentTime-=(2.25f/2.75f);
        return (totalChange*(7.5625f*(postFix)*currentTime + 0.9375f) + startValue);
    }
    else
    {
        float postFix = currentTime-=(2.625f/2.75f);
        return (totalChange*(7.5625f*(postFix)*currentTime + 0.984375f) + startValue);
    }
}

EASEDEF float EaseBounceIn(float currentTime, float totalTime, float startValue, float totalChange) { return (totalChange - EaseBounceOut(totalTime - currentTime, 0.0f, totalChange, totalTime) + startValue); } // Ease: Bounce In
EASEDEF float EaseBounceInOut(float currentTime, float totalTime, float startValue, float totalChange) // Ease: Bounce In Out
{
    if (currentTime < totalTime/2.0f) return (EaseBounceIn(currentTime*2.0f, 0.0f, totalChange, totalTime)*0.5f + startValue);
    else return (EaseBounceOut(currentTime*2.0f - totalTime, 0.0f, totalChange, totalTime)*0.5f + totalChange*0.5f + startValue);
}

// Elastic Easing functions
EASEDEF float EaseElasticIn(float currentTime, float totalTime, float startValue, float totalChange) // Ease: Elastic In
{
    if (currentTime == 0.0f) return startValue;
    if ((currentTime/=totalTime) == 1.0f) return (startValue + totalChange);

    float p = totalTime*0.3f;
    float a = totalChange;
    float s = p/4.0f;
    float postFix = a*powf(2.0f, 10.0f*(currentTime-=1.0f));

    return (-(postFix*sinf((currentTime*totalTime-s)*(2.0f*PI)/p )) + startValue);
}

EASEDEF float EaseElasticOut(float currentTime, float totalTime, float startValue, float totalChange) // Ease: Elastic Out
{
    if (currentTime == 0.0f) return startValue;
    if ((currentTime/=totalTime) == 1.0f) return (startValue + totalChange);

    float p = totalTime*0.3f;
    float a = totalChange;
    float s = p/4.0f;

    return (a*powf(2.0f,-10.0f*currentTime)*sinf((currentTime*totalTime-s)*(2.0f*PI)/p) + totalChange + startValue);
}

EASEDEF float EaseElasticInOut(float currentTime, float totalTime, float startValue, float totalChange) // Ease: Elastic In Out
{
    if (currentTime == 0.0f) return startValue;
    if ((currentTime/=totalTime/2.0f) == 2.0f) return (startValue + totalChange);

    float p = totalTime*(0.3f*1.5f);
    float a = totalChange;
    float s = p/4.0f;

    if (currentTime < 1.0f)
    {
        float postFix = a*powf(2.0f, 10.0f*(currentTime-=1.0f));
        return -0.5f*(postFix*sinf((currentTime*totalTime-s)*(2.0f*PI)/p)) + startValue;
    }

    float postFix = a*powf(2.0f, -10.0f*(currentTime-=1.0f));

    return (postFix*sinf((currentTime*totalTime-s)*(2.0f*PI)/p)*0.5f + totalChange + startValue);
}

class InterpolationFunction {
public:
    enum EasingFunction {
        EaseIn,
        EaseOut,
        EaseInOut
    };
    
    EasingFunction func;

    float duration;
    float timer; 

    InterpolationFunction(EasingFunction _func, float _duration)
        : func(_func), duration(_duration) {};
    
    void increment() {
        if (timer < duration) timer++;
    }

    void decrement() {
        if (timer > 0) timer--;
    }

    void reset() {
        timer = 0;
    }
    
    float value() {
        switch (func)
        {
        case EasingFunction::EaseIn:
            return EaseCubicIn(timer, duration, 0, 1);
        case EasingFunction::EaseOut:
            return EaseCubicOut(timer, duration, 0, 1);
        case EasingFunction::EaseInOut:
            return EaseCubicInOut(timer, duration, 0, 1);
        default:
            return 0;
        }
    } 

};


#if defined(__cplusplus)
}
#endif

#endif // REASINGS_H